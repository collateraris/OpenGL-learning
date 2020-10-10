#version 430 core
precision mediump float;
const float PI = 3.1415926535897932384626; 

in vec2 vTexCoords;

#define NR_POINT_LIGHTS 1  
uniform vec3 uLightPosition;
uniform vec3 uLightColor;
uniform vec3 uLightDir;
uniform mat4 uLightSpaceMatrix;

uniform vec3 uViewPos;
uniform mat3 uInvView;

uniform samplerCube uIrradianceMap;
uniform samplerCube uPrefilterMap;
uniform sampler2D   uBrdfLUT;
uniform sampler2D   uShadowMap;
uniform sampler2D uPosition;
uniform sampler2D uNormal;
uniform sampler2D uAlbedo;
uniform sampler2D uRoughnessMetallic;
uniform sampler2D uAO;
uniform sampler2D uNormalInView;

void CalculateTBN(in sampler2D normalMap, in vec3 Normal, in vec3 worldPos, out mat3 TBN);

vec3 fresnelSchlick(float cosTheta, in vec3 F0);

vec3 fresnelSchlickRoughness(float cosTheta, in vec3 F0, float roughness);

float DistributionGGX(in vec3 normal, in vec3 halfwayDir, float roughness);

float GeometrySchlickGGX(float NdotV, float roughness);

float GeometrySmith(in vec3 normal, in vec3 viewDir, in vec3 lightDir, float roughness);

vec3 GetNormalFromMap(in sampler2D normalMap, in mat3 TBN);

float ShadowCalculation(in vec4 fragPosLightSpace);

void main()
{
    vec3 WorldPos = texture(uPosition, vTexCoords).xyz;

    mat3 TBN;
    vec3 NormalFromGBuffer = texture(uNormalInView, vTexCoords).xyz * uInvView;
    CalculateTBN(uNormal, NormalFromGBuffer, WorldPos, TBN);

    vec4 FragPosLightSpace = uLightSpaceMatrix * vec4(WorldPos, 1.0);

    vec3 albedo = pow(texture(uAlbedo, vTexCoords).rgb, vec3(2.2));
    vec3 normal = GetNormalFromMap(uNormal, TBN);
    vec3 irradiance = texture(uIrradianceMap, normal).rgb;
    vec3 color = irradiance * albedo * 0.10;
    float shadow = ShadowCalculation(FragPosLightSpace);
    if (shadow >= 0.99)
    {
        color = color / (color + vec3(1.));
        color = pow(color, vec3(1./ 2.2));
        gl_FragColor = vec4(color, 1.0);
        return;
    }

    vec2 roughnessMetallic = texture(uRoughnessMetallic, vTexCoords).xy;
    float roughness = roughnessMetallic.x;
    float metallic = roughnessMetallic.y;
    float ao = texture(uAO, vTexCoords).r;

    mat3 transposeTBN = transpose(TBN);
    vec3 lightDir = transposeTBN * uLightDir;
    vec3 viewDir = TBN * normalize(uViewPos - WorldPos);

    float invPI = 1. / PI;

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0.);
    for (int i = 0; i < NR_POINT_LIGHTS; ++i)
    {
        vec3 halfwayDir = normalize(viewDir + lightDir);

        float distance = length(uLightPosition - WorldPos);
        float attenuation = 1. / (distance * distance);
        vec3 radiance = uLightColor * attenuation;

        vec3 F = fresnelSchlick(max(dot(halfwayDir, viewDir), 0.), F0);
        float NDF = DistributionGGX(normal, halfwayDir, roughness);
        float G = GeometrySmith(normal, viewDir, lightDir, roughness);

        vec3 numeratorBRDF = NDF * G * F;
        float denominatorBRDF = 4. * max(dot(normal, viewDir), 0.) * max(dot(normal, lightDir), 0.) + 0.001;
        vec3 specular = numeratorBRDF / denominatorBRDF;

        vec3 kS = F;
        vec3 kD = 1. - kS;
        kD *= 1. - metallic;

        float NdotL = max(dot(normal, lightDir), 0.);
        Lo += (kD * albedo * invPI + specular) * radiance * NdotL;
    }

    float VdotN = max(dot(normal, viewDir), 0.);
    vec3 F = fresnelSchlickRoughness(VdotN, F0, roughness);
    vec3 kS = F;
    vec3 kD = 1. - kS;
    kD *= 1. - metallic;

    vec3 diffuse = irradiance * albedo;

    vec3 R = reflect(-viewDir, normal);
    const float MAX_REFLECTION_LOD = 4.;
    vec3 prefilteredColor = textureLod(uPrefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;
    vec2 envBRDF = texture(uBrdfLUT, vec2(VdotN, roughness)).rg;
    vec3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);

    vec3 ambient = (kD * diffuse + specular) * ao;
    ambient = vec3(0.3) * albedo * ao;
    color = ambient + Lo;
    color = color / (color + vec3(1.));
    color = pow(color, vec3(1./ 2.2));
    gl_FragColor = vec4(color, 1.0);
}

void CalculateTBN(in sampler2D normalMap, in vec3 Normal, in vec3 worldPos, out mat3 TBN)
{
    vec3 tangentNormal = texture(normalMap, vTexCoords).xyz * 2. - 1.;

    vec3 Q1 = dFdx(worldPos);
    vec3 Q2 = dFdy(worldPos);
    vec2 st1 = dFdx(vTexCoords);
    vec2 st2 = dFdy(vTexCoords);

    vec3 N = normalize(Normal);
    vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
    vec3 B = -normalize(cross(N, T));
    TBN = mat3(T, B, N);
}

vec3 fresnelSchlick(float cosTheta, in vec3 F0)
{
    float oneMinusCosTheta = 1. - cosTheta;
    float oneMinusCosTheta2 = oneMinusCosTheta * oneMinusCosTheta; 
    float oneMinusCosTheta4 = oneMinusCosTheta2 * oneMinusCosTheta2;
    float oneMinusCosTheta5 = oneMinusCosTheta4 * oneMinusCosTheta;
    return F0 + (1. - F0) * oneMinusCosTheta5;
}

vec3 fresnelSchlickRoughness(float cosTheta, in vec3 F0, float roughness)
{
    float oneMinusCosTheta = 1. - cosTheta;
    float oneMinusCosTheta2 = oneMinusCosTheta * oneMinusCosTheta; 
    float oneMinusCosTheta4 = oneMinusCosTheta2 * oneMinusCosTheta2;
    float oneMinusCosTheta5 = oneMinusCosTheta4 * oneMinusCosTheta;
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * oneMinusCosTheta5;
}

float DistributionGGX(in vec3 normal, in vec3 halfwayDir, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(normal, halfwayDir), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = r * r * 0.125;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometrySmith(in vec3 normal, in vec3 viewDir, in vec3 lightDir, float roughness)
{
    float NdotV = max(dot(normal, viewDir), 0.0);
    float NdotL = max(dot(normal, lightDir), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

vec3 GetNormalFromMap(in sampler2D normalMap, in mat3 TBN)
{
    vec3 tangentNormal = texture(normalMap, vTexCoords).xyz * 2. - 1.;
    return normalize(TBN * tangentNormal);
}

float ShadowCalculation(in vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    if(projCoords.z > 1.0)
        return 0.;

    float closestDepth = texture(uShadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float bias = 0.0005;
    float shadow = currentDepth - bias > closestDepth  ? 1.0f : 0.0f;
    return shadow;
}

