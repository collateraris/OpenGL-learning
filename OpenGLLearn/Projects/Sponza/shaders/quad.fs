#version 430 core

in vec2 vTexCoords;

uniform sampler2D uTexture;

void main()
{
    float r = texture(uTexture, vTexCoords).r;
    gl_FragColor = vec4(vec3(r), 1.0);
}