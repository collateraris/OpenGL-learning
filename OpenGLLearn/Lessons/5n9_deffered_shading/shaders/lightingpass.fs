#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform vec3 viewPos;
uniform sampler2D positionMap;
uniform sampler2D normalMap;
uniform sampler2D albedoSpecularMap;

struct Light {
    vec3 position;
    vec3 color;
	float linear;
	float quadratic;
};
const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS];

vec3 CalcPointLight();

void main()
{
	vec3 result = CalcPointLight();
	
	float gamma = 2.2;
	result = pow(result, vec3(1.0/ gamma));
    
    FragColor = vec4(result, 1.0);
}

vec3 CalcPointLight()
{
	vec3 fragPos = texture(positionMap, TexCoords).rgb;
    vec3 normal = texture(normalMap, TexCoords).rgb;
    vec3 albedo = texture(albedoSpecularMap, TexCoords).rgb;
    float Specular = texture(albedoSpecularMap, TexCoords).a;

	vec3 viewDir = normalize(viewPos - fragPos);
	float ambient_coef = 0.1;
	vec3 ambient = ambient_coef * albedo;
	vec3 lighting = ambient;

	for(int i = 0; i < NR_LIGHTS; ++i)
	{
		vec3 lightDir = normalize(lights[i].position - fragPos);
		float diff = max(dot(lightDir, normal), 0.0);
		vec3 diffuse = diff * albedo * lights[i].color;
		vec3 halfwayDir = normalize(viewDir + lightDir);
		float spec = pow(max(dot(normal, halfwayDir), 0.0), 16.0);
		vec3 specular = spec * lights[i].color * Specular;

		float dist = length(lights[i].position - fragPos);
		float attenuation = 1.0 / (1.0 + lights[i].linear * dist + lights[i].quadratic * dist * dist);

		lighting += (diffuse + specular) * attenuation;
	}

	return lighting;
}