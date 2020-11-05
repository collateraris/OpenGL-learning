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

in VS_OUT
{
    vec4 vPos;
    vec2 vTex;
} fs_in;

uniform sampler2D uDepthTex;
uniform mat4 uInvViewProjMatrix;

void ReconstructPosWS(out vec4 positionWS)
{
    float depth = texture(uDepthTex, fs_in.vTex).r;
    vec4 projPos = vec4(fs_in.vTex, depth, 1.0);
    projPos.xy = projPos.xy * 2.0 - 1.0;
    projPos.y *= -1.0;
    positionWS = projPos * uInvViewProjMatrix;
    positionWS.xyz /= positionWS.w;
};

// Decode specified mask into a float3 color (range 0.0f-1.0f).
void DecodeColor(in uint colorMask, out vec3 color)
{
    color.r = (colorMask>>16u) & 0x000000ff;
    color.g = (colorMask>>8u) & 0x000000ff;
    color.b = colorMask & 0x000000ff;
    color /= 255.0f;
};

void main()
{
    vec4 posWS;
    ReconstructPosWS(posWS);

    vec3 color = vec3(0.5f, 0.5f, 0.5f);
    // Get offset into voxel grid.
    vec3 offset = (posWS.xyz - gridParBuffer.snappedGridPositions.xyz) *
                                    gridParBuffer.invGridCellSizes;
    offset = round(offset);

    // Get position in voxel grid. 
    ivec3 voxelPos = ivec3(16, 16, 16) + ivec3(offset);
    // Only output voxels that are inside the boundaries of the grid.
    if ((voxelPos .x>-1) && (voxelPos .x<32) && ( voxelPos .y>-1) &&
        (voxelPos.y<32) && ( voxelPos.z>-1) && (voxelPos.z<32))
    {
        int gridIndex = (voxelPos.z * 1024) + (voxelPos.y * 32) + voxelPos.x;
        uint colorMask = voxelGridBuffer[gridIndex].colorMask;

        DecodeColor(colorMask, color);
    };

    gl_FragColor = vec4(color, 1.0);
}