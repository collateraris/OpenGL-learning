#version 430 core
precision mediump float;
 
in vec3 vLocalPos;

uniform sampler2D uEquirectangularMap;

vec2 SampleSphericalMap(in vec3 v);

void main()
{
    vec3 localPos = normalize(vLocalPos);
    vec2 uv = SampleSphericalMap(localPos); 
    vec3 color = texture(uEquirectangularMap, uv).rgb;
    gl_FragColor = vec4(color, 1.0);
}

vec2 SampleSphericalMap(in vec3 v)
{
    const vec2 invAtan = vec2(0.1591, 0.3183);
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

