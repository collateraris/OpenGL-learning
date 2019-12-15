#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 LightColor;
in vec3 LightDir;

uniform sampler2D texture_diffuse1;

void main()
{
	float ambientStrenght = 0.1f;
	vec4 ambient = vec4(ambientStrenght * LightColor, 1.0);
	vec4 objColor = texture(texture_diffuse1, TexCoords);
	FragColor = ambient * objColor; 
}