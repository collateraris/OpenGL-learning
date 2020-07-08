#version 330 core
precision mediump float; 
in vec3 vPosition;

uniform vec2 resolution;

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

float lerp(float a, float b, float t)
{
    return (a + (b - a) * t);
}

float noise(in vec3 st)
{
    vec3 i = floor(st);
    vec3 f = fract(st);

    vec3 nz;
    for(int z = 0; z <=1; ++z)
    {
        vec3 ny;
        for(int y = 0; y <=1; ++y)
        {
            vec3 nx;
            for(int x = 0; x <=1; ++x)
            {
                vec3 shift = vec3(x, y, z);
                vec3 c = i + shift;
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
float fbm(in vec3 st)
{
    float value = 0.;
    float amplitude = .5;
    float frequency = 0.;

    float lacunarity = 2.;
    float gain = 0.5;

    for (int i = 0; i < OCTAVES; ++i)
    {
        value += amplitude * noise(st);
        st *= lacunarity;
        amplitude *= gain;
    }
    return value;
}

void main()
{
    vec3 st = vec3(gl_FragCoord.xy / resolution.xy, 1.0);
    st.x *= resolution.x / resolution.y;

    vec3 color = vec3(0.);
    color += fbm(st * 5.0);

    gl_FragColor = vec4(color, 1.0);
}

