#version 330 core
in vec3 ourColor;
in vec2 TexCoord;

out vec4 color;

uniform sampler2D ourTexture;
uniform sampler2D ourTexture1;

void main()
{
    color = mix(texture(ourTexture, TexCoord), texture(ourTexture1, TexCoord), 0.2);
}