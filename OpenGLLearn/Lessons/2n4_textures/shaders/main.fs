#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
}; 

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
  
in vec3 Normal;
in vec3 FragPos;
in vec3 LightPos;
in vec2 TexCoords;
out vec4 color;

uniform mat4 model;

uniform Material material;
uniform Light light;

void main()
{
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse * vec3(texture(material.diffuse, TexCoords));

    vec3 viewDir = normalize(-FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specularMap = vec3(texture(material.specular, TexCoords));
    vec3 specular =  spec * light.specular * specularMap;

    vec3 emission = vec3(0.0);
    if (specularMap.x == 0.0)
    {
        emission = vec3(texture(material.emission, TexCoords));
    }

    vec3 result = (ambient + diffuse + specular + emission);
    color = vec4(result, 1.0f);
}