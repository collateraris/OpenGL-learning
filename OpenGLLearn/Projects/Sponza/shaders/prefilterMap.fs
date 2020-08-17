#version 430 core
precision mediump float;
const float PI = 3.1415926535897932384626;

in vec3 vLocalPos;

uniform samplerCube uEnvironmentMap;
uniform float uRoughness;

float RadicalInverse_VdC(uint bits);

vec2 Hammersley(uint i, uint N);

vec3 ImportanceSampleGGX(in vec2 Xi, in vec3 N, float roughness);

float DistributionGGX(float NdotH, float roughness);

void main()
{
    vec3 N = normalize(vLocalPos);    
    vec3 R = N;
    vec3 V = R;

    const uint SAMPLE_COUNT = 1024u;
    float totalWeight = 0.0;   
    vec3 prefilteredColor = vec3(0.0);
    float PI_4 = 4. * PI;     
    for(uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
        vec3 H = ImportanceSampleGGX(Xi, N, uRoughness);
        vec3 L = normalize(2. * dot(V, H) * H - V);

        float NdotL = max(dot(N, L), 0.);
        if (NdotL > 0.)
        {
            float NdotH = max(dot(N, H), 0.);
            float HdotV = max(dot(V, H), 0.);
            float D   = DistributionGGX(NdotH, uRoughness);
            float pdf = D * NdotH / (4.0 * HdotV) + 0.0001; 

            float resolution = 512.0;
            float saTexel  = PI_4 / (6.0 * resolution * resolution);
            float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);

            float mipLevel = uRoughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);

            prefilteredColor += textureLod(uEnvironmentMap, L, mipLevel).rgb * NdotL;
            totalWeight += NdotL;
        }
    }

    prefilteredColor /= totalWeight;

    gl_FragColor = vec4(prefilteredColor, 1.0);
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

float DistributionGGX(float NdotH, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    NdotH  = max(NdotH, 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}
