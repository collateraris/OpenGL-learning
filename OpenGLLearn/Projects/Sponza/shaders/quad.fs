#version 430 core

in vec2 vTexCoords;

uniform sampler2D uTexture;

void main()
{
    float r = texture(uTexture, vTexCoords).r;
    gl_FragColor = vec4(r, r, r, 1.0);
    //gl_FragColor = vec4(1., 0, 0, 1.0);
}