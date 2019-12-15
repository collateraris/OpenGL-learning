#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 LightColor;
in vec3 LightDir;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;

void main()
{
	float ambientStrenght = 0.1f;
	vec4 ambient = vec4(ambientStrenght * LightColor, 1.0);
	vec4 objColor = texture(texture_diffuse1, TexCoords);
	
	vec3 normal = texture(texture_normal1, TexCoords).rgb;
	normal = normalize(normal * 2.0 - 1.0);
	float diff = max(dot(normal, LightDir), 0.0);
	vec4 diffuse = vec4(diff * LightColor, 1.0);
	
	FragColor = (ambient + diffuse) * objColor; 
}