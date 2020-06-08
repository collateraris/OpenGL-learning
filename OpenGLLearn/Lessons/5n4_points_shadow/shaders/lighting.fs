#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D diffuseTexture;
uniform samplerCube shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float far_plane;
uniform int debug;

float ShadowCalculation();

vec3 LightDirectionCalculation();

void main()
{	
	vec4 result = vec4(1.0);
	if (debug != 0)
	{
		vec3 fragToLight = fs_in.FragPos - lightPos; 
		float closestDepth = texture(shadowMap, fragToLight).r;
		result = vec4(vec3(closestDepth), 1.0); 
	}
	else
	{
		vec3 lighting = LightDirectionCalculation();

		result = vec4(lighting, 1.0);
	};

	float gamma = 2.2;
    FragColor.rgb = pow(result.rgb, vec3(1.0/gamma));
}

float ShadowCalculation()
{	
	vec3 fragToLight = fs_in.FragPos - lightPos; 
    float closestDepth = texture(shadowMap, fragToLight).r;
	closestDepth *= far_plane; 
	
	float currentDepth = length(fragToLight); 
	
	vec3 normal = normalize(fs_in.Normal);

	vec3 lightDir = normalize(lightPos - fs_in.FragPos);
	
	float bias = max(0.05 * (1.0 - dot(lightDir, normal)), 0.05);
	
	float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
	
	return shadow;
}

vec3 LightDirectionCalculation()
{
	vec3 lightColor = vec3(0.3);
	
	vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
	
	vec3 normal = normalize(fs_in.Normal);

	vec3 lightDir = normalize(lightPos - fs_in.FragPos);
	
	vec3 viewDir = normalize(viewPos - fs_in.FragPos);
	
	vec3 median = normalize(lightDir + viewDir);
	
	float spec = pow(max(dot(median, normal), 0.0), 64);
		
	vec3 specular = spec * lightColor;
		
	vec3 ambient = 0.05 * lightColor;
	
	float diff = max(dot(lightDir, normal), 0.0);
	
	vec3 diffuse = diff * lightColor;
	
	float shadow = ShadowCalculation();
	
	vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
	
	return lighting;
}