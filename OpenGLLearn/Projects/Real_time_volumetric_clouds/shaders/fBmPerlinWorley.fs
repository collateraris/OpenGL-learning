#version 330 core
precision mediump float; 
in vec3 vPosition;

uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform float u_time;

vec2 random2(in vec2 p ) {
    return fract(sin(vec2(dot(p,vec2(127.1,311.7)),dot(p,vec2(269.5,183.3))))*43758.5453);
}

float random (in vec2 st) {
    return fract(sin(dot(st.xy,vec2(12.9898,78.233))) * 43758.5453123);
}

float voronoi2dnoise(in vec2 st)
{
    vec2 ist = floor(st);
    vec2 fst = fract(st);

    float minDist = 1.;
    vec2 minPoint;

    for (int y = -1; y <= 1; y++) 
        for (int x = -1; x <= 1; x++) 
        {
            vec2 neighbor = vec2(x, y);
            vec2 point = random2(ist + neighbor);

            vec2 diff = neighbor + point - fst;

            float dist = length(diff);

            if (dist < minDist)
            {
                minDist = dist;
                minPoint = point;
            }
        }

    return random(minPoint);   
}

#define OCTAVES 6
float fbmVoronoi2D(in vec2 st)
{
    float value = 0.;
    float amplitude = .5;
    float frequency = 0.;

    float lacunarity = 2.;
    float gain = 0.5;
    vec2 shift = vec2(100.);
    mat2 rot = mat2(cos(0.5), sin(0.5), -sin(0.5), cos(0.5));

    for (int i = 0; i < OCTAVES; ++i)
    {
        value += amplitude * voronoi2dnoise(st);
        st = rot * st * lacunarity + shift;
        amplitude *= gain;
    }
    return value;
}

float fbmWorley2D(in vec2 st)
{
    return clamp((1. - fbmVoronoi2D(st)) * 1.5 - 0.25, 0., 1.);
}

void main()
{
    vec2 st = gl_FragCoord.xy / u_resolution.xy;
    st.x *= u_resolution.x / u_resolution.y;

    vec3 color = vec3(0.);
    color += fbmWorley2D(st * 15.0);

    gl_FragColor = vec4(color, 1.0);
}

