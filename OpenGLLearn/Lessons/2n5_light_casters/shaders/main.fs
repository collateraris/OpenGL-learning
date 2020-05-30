#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 

struct Light {

	vec3  position;
    vec3  direction;
    float cutOff;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
  
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 color;

uniform mat4 model;
uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
	vec3 lightDir = normalize(light.position - FragPos);
	
	float theta = dot(lightDir, normalize(-light.direction));
	
	if(theta > light.cutOff){ 
	
		vec3 norm = normalize(Normal);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = diff * light.diffuse * vec3(texture(material.diffuse, TexCoords));

		vec3 viewDir = normalize(viewPos - FragPos);
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
		vec3 specularMap = vec3(texture(material.specular, TexCoords));
		vec3 specular =  spec * light.specular * specularMap;
		
		color = vec4(ambient + diffuse + specular, 1.0f);
		
	}
	else
	{
		color = vec4(ambient, 1.0f);
	};
	
}