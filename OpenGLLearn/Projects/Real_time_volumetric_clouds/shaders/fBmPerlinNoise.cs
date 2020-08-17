#version 430 core

layout (local_size_x = 4, local_size_y = 4, local_size_z = 4) in;

layout (rgba8, binding = 0) uniform image3D outTexture;

uniform vec3 uResolution;

#define OCTAVES 6
float fbmPerlinNoise3(in vec3 st);

float perlinNoise3(in vec3 st, in vec3 T);

vec3 modulo3(in vec3 a, in vec3 b);

float lerp(float a, float b, float t);

vec3 random(in vec3 st);

void main()
{
    vec3 st = gl_GlobalInvocationID.xyz;
    ivec3 pixel = ivec3(st);
    st /= uResolution;
    st.x *= uResolution.x / uResolution.y;
    vec3 color = vec3(fbmPerlinNoise3(st * 15.));

    imageStore(outTexture, pixel, vec4(color, 1.));
}

float fbmPerlinNoise3(in vec3 st)
{
    float noise = 0.;
    float amplitude = 0.5;
    float frequency = 2.;
    vec3 T = vec3(4., 4., 4.);

    float persistance = 0.433;
    float roughness = 4.;

    for (int i = 0; i < OCTAVES; ++i)
    {
        noise += amplitude * perlinNoise3(st, T);
        st *= frequency ;
        T *= frequency;
        amplitude *= persistance;
        frequency *= roughness;
    }

    return noise;
}

float perlinNoise3(in vec3 st, in vec3 T)
{
    vec3 f = fract(st);

    vec3 min = floor(st);
    vec3 max = ceil(st);

    max = modulo3(max, T);
    min = modulo3(min, T);

    vec3 i[8];
    i[0] = vec3(min.x, min.y, min.z);
    i[1] = vec3(max.x, min.y, min.z);
    i[2] = vec3(min.x, max.y, min.z);
    i[3] = vec3(max.x, max.y, min.z);
    i[4] = vec3(min.x, min.y, max.z);
    i[5] = vec3(max.x, min.y, max.z);
    i[6] = vec3(min.x, max.y, max.z);
    i[7] = vec3(max.x, max.y, max.z);

    vec3 nz;
    int index_i = 0;
    for(int z = 0; z <=1; ++z)
    {
        vec3 ny;
        for(int y = 0; y <=1; ++y)
        {
            vec3 nx;
            for(int x = 0; x <=1; ++x)
            {
                vec3 shift = vec3(x, y, z);
                vec3 c = i[index_i] + shift;
                index_i++;
                vec3 cd = random(c) * 2. - 1.;
                vec3 compare = f - shift;
                nx[x] = dot(cd, compare);
            }
            ny[y] = lerp(nx[0], nx[1], f.x);
        }
        nz[z] = lerp(ny[0], ny[1], f.y);
    }
    
    float noise = lerp(nz[0], nz[1], f.z);
    return noise;
}

vec3 modulo3(in vec3 a, in vec3 b)
{
    vec3 posDivident = mod(a, b) + b;
    return mod(posDivident, b);
}

float lerp(float a, float b, float t)
{
    return (a + (b - a) * t);
}

vec3 random(in vec3 st)
{
    float j = 4096. * sin(dot(st, vec3(18., 59.4, 15.)));
    vec3 r;
    r.z = fract(512. * j);
    j *= .125;
    r.x = fract(512. * j);
    j *= .125;
    r.y = fract(512. * j);
    return r - 0.5;
}


