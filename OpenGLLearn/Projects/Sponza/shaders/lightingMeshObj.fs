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
uniform mat4 uLightSpaceModel;

uniform samplerCube uIrradianceMap;
uniform samplerCube uPrefilterMap;
uniform sampler2D   uBrdfLUT;
uniform sampler2D uPosition;
uniform sampler2D uNormal;
uniform sampler2D uAlbedo;
uniform sampler2D uRoughnessMetallic;
uniform sampler2D uAO;
uniform sampler2D uShadowMap;
uniform sampler2D uFogNoise;

vec3 fresnelSchlick(float cosTheta, in vec3 F0);

vec3 fresnelSchlickRoughness(float cosTheta, in vec3 F0, float roughness);

float DistributionGGX(in vec3 normal, in vec3 halfwayDir, float roughness);

float GeometrySchlickGGX(float NdotV, float roughness);

float GeometrySmith(in vec3 normal, in vec3 viewDir, in vec3 lightDir, float roughness);

bool InShadow(in vec3 position);

float SoftShadow(in vec3 position);

float Fog(in vec3 fragPos);

void main()
{
    vec3 WorldPos = texture(uPosition, vTexCoords).xyz;

    vec3 albedo = pow(texture(uAlbedo, vTexCoords).rgb, vec3(2.2));
    vec3 normal = texture(uNormal, vTexCoords).xyz;
    vec3 irradiance = texture(uIrradianceMap, normal).rgb;

    float in_shadow = InShadow(WorldPos) ? 0.25 : 1.;

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
        vec3 specular = numeratorBRDF / denominatorBRDF * in_shadow;

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

    vec3 ambient = (kD * diffuse + specular) * ao * in_shadow;
    vec3 color = ambient + Lo;
    
    float fog = Fog(WorldPos);
    color = mix(color, vec3(0.07, 0.06, 0.03), fog);

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
    vec4 fragPosLightSpace = uLightSpaceModel * vec4(position, 1.0);
    vec3 shadow_clip = fragPosLightSpace.xyz / fragPosLightSpace.w;
    shadow_clip = shadow_clip * 0.5 + 0.5;    
    float closestDepth = texture(uShadowMap, shadow_clip.xy).r;
    float currentDepth = shadow_clip.z;
    float bias = 0.0005;
    return currentDepth - bias > closestDepth  ? true : false;
}

float SoftShadow(in vec3 position)
{
    vec4 fragPosLightSpace = uLightSpaceModel * vec4(position, 1.0);
    vec3 shadow_clip = fragPosLightSpace.xyz / fragPosLightSpace.w;
    shadow_clip = shadow_clip * 0.5 + 0.5; 
    float shadow = 0.0;
    float bias = 0.0005;
    vec2 texelSize = 1.0 / textureSize(uShadowMap, 0);
    float currentDepth = shadow_clip.z;
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(uShadowMap, shadow_clip.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    return shadow /= 9.0;
}

float Fog(in vec3 fragPos)
{
    const float fogDistance = 16.0;
    const float fogSamples = 64.0;

    vec3 fogOrigin = uViewPos;
    vec3 fogDirection = fragPos - uViewPos;
    float fogDistanceToPoint = length(fogDirection);
    float fogStep = fogDistanceToPoint / fogSamples;
    fogDirection /= fogDistanceToPoint;

    float random = texture(uFogNoise, vTexCoords).r;

    float fog = 0.0;
    float minDist = min(fogDistanceToPoint, fogDistance);
    for (float t = 0.0; t < minDist; t += fogStep)
    {
        vec3 currentPosition = fogOrigin + fogDirection * (t + fogStep * random);
        
        if (!InShadow(currentPosition))
        {
            fog += 1.0 / fogSamples;
        }
    }

    return clamp(fog, 0.0, 1.0);
}



