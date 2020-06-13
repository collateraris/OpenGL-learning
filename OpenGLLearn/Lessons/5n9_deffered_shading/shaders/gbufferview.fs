#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gbuffercomponent;

void main()
{
    vec3 color = texture(gbuffercomponent, TexCoords).rgb;
    FragColor = vec4(color, 1.0);
}