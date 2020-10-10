#version 430 core

in vec2 vTexCoords;

uniform sampler2D uTexture;

void main()
{
    vec4 color = texture(uTexture, vTexCoords);
    float r = color.r;
    gl_FragColor = vec4(r, r, r, 1.0);
    //gl_FragColor = vec4(1., 0, 0, 1.0);
}