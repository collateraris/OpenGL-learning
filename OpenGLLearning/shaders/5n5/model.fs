#version 330 core
struct Material{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_normal1;
	float shininess;
};

uniform Material material;

struct DirLight{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform DirLight dirLight;

uniform vec3 viewPos;

in vec2 TexCoords;
in vec3 FragPos;
in mat3 TBN;

out vec4 FragColor;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);

void main()
{
	vec3 normal = texture(material.texture_normal1, TexCoords).rgb;
	normal = normalize(normal * 2.0 - 1.0);
	normal = normalize(TBN * normal);
 
	vec3 viewDir = normalize(viewPos - FragPos);
	
	vec3 result = CalcDirLight(dirLight, normal, viewDir);
	
	FragColor = vec4(result, 0.0f);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoords));
	
	vec3 lightDir = normalize(-light.direction);	
	float diff = max(dot(normal, lightDir), 0.0f);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoords));
	
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords));
	
	return (ambient + diffuse + specular);
}