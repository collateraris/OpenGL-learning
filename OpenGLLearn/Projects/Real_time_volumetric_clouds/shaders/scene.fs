#version 430 core

in vec3 vPosition;
in vec2 vTexCoords;

uniform sampler2D uTexture;

void main()
{
    vec3 value = texture(uTexture, vTexCoords).rgb;
    value = value/(1 +value);
    value = pow(value, vec3(1.0/2.2));
    gl_FragColor = vec4(value, 1.);
}