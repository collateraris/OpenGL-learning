#version 450 core

struct GRID_PARAMS
{
    mat4 gridViewProjMatrices[3];  // viewProjMatrices for generating the voxel-grids
    vec4 gridPositions;            // center of FINE_GRID/
    vec4 globalIllumParams;        // x = flux amplifier, y = occlusion amplifier, z = diffuse GI-contribution power
    vec3 snappedGridPositions;     // center of FINE_GRID, snapped to the corresponding grid-cell extents 
    float gridCellSizes;            // (inverse) sizes of grid-cells FINE_GRID
    float invGridCellSizes;
};

layout(binding = 0, std430) buffer gridParams_buffer
{
    GRID_PARAMS gridParBuffer;
};

struct VOXEL
{
    uvec4 normalMasks; // encoded normals
    uint colorMask; // encoded color
    uint occlusion; // voxel only contains geometry info if occlusion > 0
};

layout(binding = 1, std430) buffer voxelGrid_buffer
{
    VOXEL voxelGridBuffer[];
};

struct Material {
    sampler2D texture_diffuse_0;
    sampler2D texture_specular_0; // for roughness
    sampler2D texture_normal_0;
    sampler2D texture_ambient_0; // for metallic
};

uniform Material material;

in vec4 gPos;
in vec4 gPosWS;
in vec2 gTex;
in vec3 gNorm;

// normalized directions of four faces of a tetrahedron
const mat4 cFaceVectors =
{
    vec4(0.0f,-0.57735026f ,0.81649661f, 0.f),
    vec4(0.0f,-0.57735026f,-0.81649661f, 0.f),
    vec4(-0.81649661f ,0.57735026f ,0.0f, 0.f),
    vec4 (0.81649661f,0.57735026f ,0.0f, 0.f)
};

int GetNormalIndex(in vec3 normal, out float dotProduct)
{
    vec4 dotProducts = vec4(normal, 0.) * cFaceVectors;
    float maximum = max (max( dotProducts.x,dotProducts.y),
                         max(dotProducts.z,dotProducts.w));
    int index;
    if(maximum == dotProducts.x)
        index = 0;
    else if(maximum == dotProducts.y)
        index = 1;
    else if(maximum == dotProducts.z)
        index = 2;
    else
        index = 3;
    dotProduct = dotProducts[index];
    return index;
};

// Encode specified color (range 0.0f-1.0f), so that each channel is
// stored in 8 bits of an unsigned integer.
uint EncodeColor(in vec3 color)
{
    uvec3 iColor = uvec3(color * 255.0);
    uint colorMask = (iColor.r << 16u) | (iColor.g << 8u) | iColor.b;
    return colorMask;
};

// Encode specified normal (normalized) into an unsigned integer. Each axis of
// the normal is encoded into 9 bits (1 for the sign/ 8 for the value).
uint EncodeNormal(in vec3 normal)
{
    ivec3 iNormal = ivec3(normal * 255.0);
    uvec3 iNormalSigns;
    iNormalSigns.x = (iNormal.x>>5) & 0x04000000;
    iNormalSigns.y = (iNormal.y>>14) & 0x00020000; 
    iNormalSigns.z = (iNormal.z>>23) & 0x00000100;
    iNormal = abs(iNormal);
    uint normalMask = iNormalSigns.x | (iNormal.x<<18) | iNormalSigns.y | (iNormal.y<<9) | iNormalSigns.z | iNormal.z;
    return normalMask;
};

void main()
{
    vec3 diffuseBase = texture(material.texture_diffuse_0, gTex).rgb;

    // Encode color into the lower 24 bit of an unsigned integer , using
    // 8 bit for each color channel.
    uint colorMask = EncodeColor( diffuseBase);

    // Calculate color - channel contrast of color and write value into the
    // highest 8 bit of the color mask.
    float contrast = length(diffuseBase.rrg - diffuseBase.gbb)/
                        (sqrt(2.0f) + diffuseBase.r + diffuseBase.g + diffuseBase.b);
    int iContrast = int(contrast * 255.0f);
    colorMask |= iContrast << 24;

    // Encode normal into the lower 27 bit of an unsigned integer , using
    // for each axis 8 bit for the value and 1 bit for the sign.
    vec3 normal = normalize( gNorm );
    uint normalMask = EncodeNormal( normal);

    // Calculate to which face of a tetrahedron current normal is closest
    // and write corresponding dot product into the highest 5 bit of the
    // normal mask.
    float dotProduct;
    int normalIndex = GetNormalIndex(normal, dotProduct);
    int iDotProduct = int(clamp(dotProduct, 0, 1) * 31.0);
    normalMask |= iDotProduct << 27;

    // Get offset into voxel grid.
    vec3 offset = (gPosWS.xyz - gridParBuffer.snappedGridPositions.xyz) *
                                    gridParBuffer.invGridCellSizes;
    offset = round(offset);

    // Get position in voxel grid. 
    ivec3 voxelPos = ivec3(16, 16, 16) + ivec3(offset);

    // Only output voxels that are inside the boundaries of the grid.
    if ((voxelPos .x>-1) && (voxelPos .x<32) && ( voxelPos .y>-1) &&
        (voxelPos.y<32) && ( voxelPos.z>-1) && (voxelPos.z<32))
    {
        int gridIndex = (voxelPos.z * 1024) + (voxelPos.y * 32) + voxelPos.x;
        
        // Output color.
        atomicMax(voxelGridBuffer[gridIndex].colorMask , colorMask);
        // Output normal according to normal index.
        atomicMax(voxelGridBuffer[gridIndex].normalMasks[normalIndex], normalMask);
        // Mark voxel that contains geometry information.
        atomicMax(voxelGridBuffer[gridIndex].occlusion , 1);
    }                        
}