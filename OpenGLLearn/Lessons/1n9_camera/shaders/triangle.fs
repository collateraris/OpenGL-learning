#version 330 core
in vec2 TexCoord;

out vec4 color;

uniform sampler2D ourTexture;
uniform sampler2D ourTexture1;
uniform float mixTexture;

void main()
{
    color = mix(texture(ourTexture, TexCoord), texture(ourTexture1, TexCoord), mixTexture);
}