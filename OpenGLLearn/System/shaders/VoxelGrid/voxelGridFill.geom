#version 450 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

struct GRID_PARAMS
{
    mat4 gridViewProjMatrices[6];     // viewProjMatrices for generating the voxel-grids
    vec4 gridCellSizes;               // (inverse) sizes of grid-cells (FINE_GRID/ COARSE_GRID)
    vec4 gridPositions;            // center of FINE_GRID/
    vec4 snappedGridPositions;     // center of FINE_GRID, snapped to the corresponding grid-cell extents 
    vec4 lastSnappedGridPositions; // snapped grid positions of FINE_GRID from last frame (w-component = frame-interval)
    vec4 globalIllumParams;           // x = flux amplifier, y = occlusion amplifier, z = diffuse GI-contribution power
};

layout(binding = 0, std430) buffer gridParams_buffer
{
    GRID_PARAMS gridParBuffer;
};

in VS_OUT{
    vec4 vPos;
    vec2 vTex;
    vec3 vNorm;
} gs_in[];

out vec4 gPos;
out vec4 gPosWS;
out vec2 gTex;
out vec3 gNorm;

struct GEOM_OUT
{
    vec4 gPos;
    vec4 gPosWS;
    vec2 gTex;
    vec3 gNorm;
};

const float cTexelSize = 1. / 64.;

const mat3 cDirectionMatrix =
{
    -vec3(0.0f,0.0f, -1.0f), // back to front
    -vec3( -1.0f ,0.0f,0.0f), // right to left
    -vec3(0.0f, -1.0f ,0.0f) // top to down
};

int GetViewIndex(in vec3 normal)
{
    vec3 dotProducts = abs(normal * cDirectionMatrix);
    float maximum = max (max( dotProducts.x,dotProducts.y), dotProducts.z);
    if(maximum == dotProducts.x)
       return 0;
    if(maximum == dotProducts.y)
        return 1;
    return 2;
};

void main()
{
    vec3 faceNormal = normalize(gs_in[0].vNorm + gs_in[1].vNorm + gs_in[2].vNorm);

    // Get view , at which the current triangle is most visible , in order to
    // achieve highest possible rasterization of the primitive.
    int viewIndex = GetViewIndex(faceNormal);

    GEOM_OUT _output[3];
    for( int i = 0; i < 3; ++i)
    {
        _output[i].gPos = gridParBuffer.gridViewProjMatrices[viewIndex] * gs_in[i].vPos;
        _output[i].gPosWS = gs_in[i].vPos; // world - space position
        _output[i].gTex = gs_in[i].vTex;
        _output[i].gNorm = gs_in[i].vNorm;
    };

    // Increase size of triangle in normalized device coordinates by the
    // texel size of the currently bound render-target.
    vec2 sideA = normalize(_output[1].gPos.xy - _output[0].gPos.xy);
    vec2 sideB = normalize(_output[2].gPos.xy - _output[1].gPos.xy);
    vec2 sideC = normalize(_output[0].gPos.xy - _output[2].gPos.xy);    

    _output[0].gPos.xy += normalize(sideC - sideA) * cTexelSize;
    _output[1].gPos.xy += normalize(sideA - sideB) * cTexelSize;
    _output[2].gPos.xy += normalize(sideB - sideC) * cTexelSize;

    for(int i = 0; i < 3; ++i)
    {
      gl_Position = _output[i].gPos;
      gPos = _output[i].gPos;
      gPosWS = _output[i].gPosWS;
      gNorm = _output[i].gNorm;
      gTex = _output[i].gTex;

      EmitVertex();
    }

    EndPrimitive();
}