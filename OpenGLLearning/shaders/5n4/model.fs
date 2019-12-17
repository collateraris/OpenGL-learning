#version 330 core
struct Material{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_normal1;
	float shininess;
};

uniform Material material;

struct DirLight{
	vec3 position;
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform DirLight dirLight;

struct SpotLight{
	vec3 position;
	vec3 direction;
	float cutOff;
	float outerCutOff;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform SpotLight spotLight;

uniform vec3 viewPos;
uniform samplerCube depthMap;
uniform float farPlane;

in vec2 TexCoords;
in vec3 FragPos;
in mat3 TBN;

out vec4 FragColor;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
float ShadowCalculation(vec3 fragPos, vec3 lightPos);

void main()
{
	vec3 normal = texture(material.texture_normal1, TexCoords).rgb;
	normal = normalize(normal * 2.0 - 1.0);
	normal = normalize(TBN * normal);
 
	vec3 viewDir = normalize(viewPos - FragPos);
	
	vec3 result = CalcDirLight(dirLight, normal, viewDir);
	
	result += CalcSpotLight(spotLight, normal, FragPos, viewDir);
	
	FragColor = vec4(result, 0.0f);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoords));
	
	vec3 lightDir = normalize(-light.direction);	
	float diff = max(dot(normal, lightDir), 0.0f);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoords));
	
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0f), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords));
	
	float shadow = ShadowCalculation(FragPos, light.position);
	
	return (ambient + (1.0 - shadow) * (diffuse + specular));
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff)/ epsilon, 0.0f, 1.0f);
	
	vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoords));
	
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse  * diff * vec3(texture(material.texture_diffuse1, TexCoords));
	
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords));
	
	diffuse *= intensity;
	specular *= intensity;
	
	float shadow = ShadowCalculation(FragPos, light.position);
	return (ambient + (1.0 - shadow) * (diffuse + specular));
}

float ShadowCalculation(vec3 fragPos, vec3 lightPos)
{
	vec3 fragToLight = fragPos - lightPos;
	float closestDepth = texture(depthMap, fragToLight).r;
	closestDepth *= farPlane;
	float currentDepth = length(fragToLight);
	float bias = 0.05f;
	float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
	return shadow;
}