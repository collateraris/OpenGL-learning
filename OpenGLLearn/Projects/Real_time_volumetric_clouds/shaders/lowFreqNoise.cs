#version 430 core

layout (local_size_x = 4, local_size_y = 4, local_size_z = 4) in;

layout (rgba8, binding = 0) uniform image3D outTexture;

layout (rgba8, binding = 1) uniform image3D perlinNoise128;
layout (rgba8, binding = 2) uniform image3D worleyPerlinNoize128;
layout (rgba8, binding = 3) uniform image3D worleyPerlinNoize64;
layout (rgba8, binding = 4) uniform image3D worleyPerlinNoize32;

float remap(float value, float minValue, float maxValue, float newMinValue, float newMaxValue);

void main()
{
    vec3 st32 = gl_GlobalInvocationID.xyz;
    ivec3 pixel32 = ivec3(st32);
    ivec3 pixel64 = pixel32 * 2;
    ivec3 pixel128 = pixel64 * 2;
    vec4 noise = vec4(1.);
    noise.r = imageLoad(perlinNoise128, pixel128).r;
    noise.g = imageLoad(worleyPerlinNoize128, pixel128).r;
    noise.b = imageLoad(worleyPerlinNoize64, pixel64).r;
    noise.a = imageLoad(worleyPerlinNoize32, pixel32).r;

    float finalValue = noise.x * 0.625 + noise.y * 0.25 + noise.z * 0.125;

    imageStore(outTexture, pixel32, vec4(finalValue));
}

float remap(float value, float minValue, float maxValue, float newMinValue, float newMaxValue)
{
    return newMinValue + (value - minValue)/(maxValue - minValue) * (newMaxValue - newMinValue);
}