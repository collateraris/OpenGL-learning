#version 430 core
precision highp float;
const float PI = 3.1415926535897932384626; 

in vec2 vTexCoords;

#define NR_POINT_LIGHTS 1  
uniform vec3 uLightPosition;
uniform vec3 uLightColor;
uniform vec3 uLightDir;

uniform vec3 uViewPos;
uniform int uAOInclude;
uniform mat4 uLightSpaceMatrix;

uniform samplerCube uIrradianceMap;
uniform samplerCube uPrefilterMap;
uniform sampler2D   uBrdfLUT;
uniform sampler2D uPosition;
uniform sampler2D uNormal;
uniform sampler2D uAlbedo;
uniform sampler2D uRoughnessMetallic;
uniform sampler2D uAO;
uniform sampler2D uShadowMap;

vec3 fresnelSchlick(float cosTheta, in vec3 F0);

vec3 fresnelSchlickRoughness(float cosTheta, in vec3 F0, float roughness);

float DistributionGGX(in vec3 normal, in vec3 halfwayDir, float roughness);

float GeometrySchlickGGX(float NdotV, float roughness);

float GeometrySmith(in vec3 normal, in vec3 viewDir, in vec3 lightDir, float roughness);

bool InShadow(in vec3 position);

void main()
{
    vec3 WorldPos = texture(uPosition, vTexCoords).xyz;

    vec3 albedo = pow(texture(uAlbedo, vTexCoords).rgb, vec3(2.2));
    vec3 normal = texture(uNormal, vTexCoords).xyz;
    vec3 irradiance = texture(uIrradianceMap, normal).rgb;

    if (InShadow(WorldPos))
    {
        vec3 color = irradiance * albedo * 0.10;
        color = color / (color + vec3(1.));
        color = pow(color, vec3(1./ 2.2));
        gl_FragColor = vec4(color, 1.0);
        return;
    }

    vec2 roughnessMetallic = texture(uRoughnessMetallic, vTexCoords).xy;
    float roughness = roughnessMetallic.x;
    float metallic = roughnessMetallic.y;
    float ao = 1.0;
    if (uAOInclude != 0)
    {
        ao = texture(uAO, vTexCoords).r;
    }

    vec3 lightDir = uLightDir;
    vec3 viewDir = normalize(uViewPos - WorldPos);

    float invPI = 1. / PI;

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0.);
    for (int i = 0; i < NR_POINT_LIGHTS; ++i)
    {
        vec3 halfwayDir = normalize(viewDir + lightDir);

        float distance = length(uLightPosition - WorldPos);
        float attenuation = 1.0 / (distance * distance);
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
    vec3 color = ambient + Lo;

    color = color / (color + vec3(1.));
    color = pow(color, vec3(1./ 2.2));
    gl_FragColor = vec4(color, 1.0);
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

bool InShadow(in vec3 position)
{
    // perform perspective divide
    vec4 fragPosLightSpace = uLightSpaceMatrix * vec4(position, 1.0);
    vec3 shadow_clip = fragPosLightSpace.xyz / fragPosLightSpace.w;
    shadow_clip = shadow_clip * 0.5 + 0.5;    
    float closestDepth = texture(uShadowMap, shadow_clip.xy).r;

    bool inLight = dot(uLightDir, position - closestDepth) > -0.1;
    bool inShadowMap = shadow_clip.x > 0.0 && shadow_clip.y > 0.0 && shadow_clip.x < 1.0 && shadow_clip.y < 1.0;

    return inLight && inShadowMap;
}


