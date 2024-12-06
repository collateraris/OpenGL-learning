#version 330 core
precision mediump float; 
in vec3 vPosition;

uniform vec2 resolution;
uniform float u_time;

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

float random1(in vec2 st) 
{
    return fract(sin(dot(st.xy, vec2(12.9898,78.233)))*43758.5453123);
}

vec2 random2(in vec2 p ) {
    return fract(sin(vec2(dot(p,vec2(127.1,311.7)),dot(p,vec2(269.5,183.3))))*43758.5453);
}

float lerp(float a, float b, float t)
{
    return (a + (b - a) * t);
}

vec2 modulo2(in vec2 a, in vec2 b)
{
    vec2 posDivident = mod(a, b) + b;
    return mod(posDivident, b);
}

vec3 modulo3(in vec3 a, in vec3 b)
{
    vec3 posDivident = mod(a, b) + b;
    return mod(posDivident, b);
}

float perlinNoise2(in vec2 st, in vec2 T)
{
    vec2 min = floor(st);
    vec2 max = ceil(st);

    max = modulo2(max, T);
    min = modulo2(min, T);

    vec2 f = fract(st);

    // Four corners in 2D of a tile
    float a = random1(min);
    float b = random1(vec2(max.x, min.y) + vec2(1.0, 0.0));
    float c = random1(vec2(min.x, max.y) + vec2(0.0, 1.0));
    float d = random1(max + vec2(1.0, 1.0));

    // Smooth Interpolation

    // Cubic Hermine Curve.  Same as SmoothStep()
    vec2 u = f*f*(3.0-2.0*f);
    // u = smoothstep(0.,1.,f);

    // Mix 4 coorners percentages
    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
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

#define OCTAVES 6
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

float fbmPerlinNoise2(in vec2 st)
{
    float noise = 0.;
    float amplitude = 0.5;
    float frequency = 2.;
    vec2 T = vec2(4., 4.);

    float persistance = 0.433;
    float roughness = 4.;

    for (int i = 0; i < OCTAVES; ++i)
    {
        noise += amplitude * perlinNoise2(st, T);
        st *= frequency ;
        T *= frequency;
        amplitude *= persistance;
        frequency *= roughness;
    }

    return noise;
}

void main()
{
    vec2 st = gl_FragCoord.xy/resolution.xy;
    st.x *= resolution.x/resolution.y;

    vec3 color = vec3(0.0);
    vec3 p = vec3(st, u_time * 0.25);
    color += fbmPerlinNoise3(p * 15.);

    gl_FragColor = vec4(color, 1.0);
}

