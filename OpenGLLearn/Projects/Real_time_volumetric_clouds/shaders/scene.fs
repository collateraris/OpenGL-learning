#version 430 core

in vec3 vPosition;
in vec2 vTexCoords;

uniform sampler2D uTexture;

void main()
{
    vec3 value = texture(uTexture, vTexCoords).rgb; 
    gl_FragColor = vec4(value, 1.);
}