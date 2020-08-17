#version 430 core
precision mediump float;
const float PI = 3.1415926535897932384626;

in vec3 vLocalPos;
in vec2 vTexCoords;

float RadicalInverse_VdC(uint bits);

vec2 Hammersley(uint i, uint N);

vec3 ImportanceSampleGGX(in vec2 Xi, in vec3 N, float roughness);

vec2 IntegrateBRDF(float NdotV, float roughness);

float GeometrySchlickGGX(float NdotV, float roughness);

float GeometrySmith(in vec3 N, in vec3 V, in vec3 L, float roughness);

void main()
{
    vec2 brdfColor = IntegrateBRDF(vTexCoords.x, vTexCoords.y);

    gl_FragColor.rg = brdfColor;
}

float RadicalInverse_VdC(uint bits) 
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 Hammersley(uint i, uint N)
{
    return vec2(float(i)/float(N), RadicalInverse_VdC(i));
} 

vec3 ImportanceSampleGGX(in vec2 Xi, in vec3 N, float roughness)
{
    float a = roughness*roughness;
	
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);

    vec3 tangentSample = vec3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);

    vec3 up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent   = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);

    vec3 worldSample = tangent * tangentSample.x + bitangent * tangentSample.y + N * tangentSample.z;
    return normalize(worldSample);
}

vec2 IntegrateBRDF(float NdotV, float roughness)
{
    vec3 V = vec3(sqrt(1. - NdotV * NdotV), 0., NdotV);

    float A = 0.;
    float B = 0.;

    vec3 N = vec3(0., 0., 1.);

    const uint SAMPLE_COUNT = 1024u;
    for(uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
        vec3 H = ImportanceSampleGGX(Xi, N, roughness);
        vec3 L = normalize(2. * dot(V, H) * H - V);

        float NdotL = max(L.z, 0.);

        if (NdotL > 0.)
        {
            float NdotH = max(H.z, 0.);
            float VdotH = max(dot(V, H), 0.);

            float G = GeometrySmith(N, V, L, roughness);
            float G_Vis = (G  * VdotH) / (NdotH * NdotV);

            float OneMunisVdotH = 1. - VdotH;
            float OneMunisVdotH2 = OneMunisVdotH * OneMunisVdotH;
            float OneMunisVdotH4 = OneMunisVdotH2 * OneMunisVdotH2;
            float Fc = OneMunisVdotH4 * OneMunisVdotH;

            A += (1. - Fc) * G_Vis;
            B += Fc * G_Vis;
        }
    }

    float invSAMPLE_COUNT = 1. / float(SAMPLE_COUNT);
    A *= invSAMPLE_COUNT;
    B *= invSAMPLE_COUNT;

    return vec2(A, B);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float a = roughness;
    float k = (a * a) / 2.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(in vec3 N, in vec3 V, in vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}  
