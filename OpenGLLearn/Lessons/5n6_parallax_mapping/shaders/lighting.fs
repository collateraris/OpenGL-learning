#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec2 TexCoords;
	vec3 lightDir;
	vec3 viewDir;
	vec3 Normal;
} fs_in;

uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;
uniform sampler2D displacementTexture;
uniform float height_scale;

vec3 LightDirectionCalculation();

vec2 ParallaxMapping();

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
	
	vec2 texCoords = ParallaxMapping();
	
	if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
			discard;
	
	vec3 color = texture(diffuseTexture, texCoords).rgb;
	
	vec3 normal = texture(normalTexture, texCoords).rgb;
	normal = normalize(normal * 2.0 - 1.0);   

	vec3 lightDir = fs_in.lightDir;
	
	vec3 viewDir = fs_in.viewDir;
	
	vec3 median = normalize(lightDir + viewDir);
	
	float spec = pow(max(dot(median, normal), 0.0), 64);
		
	vec3 specular = spec * lightColor;
		
	vec3 ambient = 0.05 * lightColor;
	
	float diff = max(dot(lightDir, normal), 0.0);
	
	vec3 diffuse = diff * lightColor;
	
	vec3 lighting = (ambient + diffuse + specular) * color;
	
	return lighting;
}

vec2 ParallaxMapping()
{	
	float numLayers = 16;
	
	float layerDepth = 1.0 / numLayers;
	float currentLayerDepth = 0.0;
	vec2 P = fs_in.viewDir.xy * height_scale; 
	vec2 deltaTexCoords = P * layerDepth;
	
	vec2  currentTexCoords     = fs_in.TexCoords;
	float currentDepthMapValue = texture(displacementTexture, currentTexCoords).r;
	
	while(currentLayerDepth < currentDepthMapValue)
	{
		currentTexCoords -= deltaTexCoords;
		currentDepthMapValue = texture(displacementTexture, currentTexCoords).r;
		currentLayerDepth += layerDepth;
	}
	
	return currentTexCoords; 
}