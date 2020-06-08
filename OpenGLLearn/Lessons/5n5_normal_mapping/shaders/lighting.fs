#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;

uniform vec3 lightPos;
uniform vec3 viewPos;

vec3 LightDirectionCalculation();

void main()
{	
	vec3 lighting = LightDirectionCalculation();

	vec4 result = vec4(lighting, 1.0);

	float gamma = 2.2;
    FragColor.rgb = pow(result.rgb, vec3(1.0/gamma));
}

vec3 LightDirectionCalculation()
{
	vec3 lightColor = vec3(0.3);
	
	vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
	
	vec3 normal = texture(normalTexture, fs_in.TexCoords).rgb;
	normal = normalize(normal * 2.0 - 1.0);

	vec3 lightDir = normalize(lightPos - fs_in.FragPos);
	
	vec3 viewDir = normalize(viewPos - fs_in.FragPos);
	
	vec3 median = normalize(lightDir + viewDir);
	
	float spec = pow(max(dot(median, normal), 0.0), 64);
		
	vec3 specular = spec * lightColor;
		
	vec3 ambient = 0.05 * lightColor;
	
	float diff = max(dot(lightDir, normal), 0.0);
	
	vec3 diffuse = diff * lightColor;
	
	vec3 lighting = (ambient + diffuse + specular) * color;
	
	return lighting;
}