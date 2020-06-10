#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

struct Light {
    vec3 position;
    vec3 color;
};

#define NR_POINT_LIGHTS 4
uniform Light lights[NR_POINT_LIGHTS];
uniform sampler2D diffuseTexture;
uniform vec3 viewPos;

void main()
{           
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    float ambient_coef = 0.0;
	vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    // lighting
    vec3 lighting = vec3(0.0);
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
    {
        vec3 lightDir = normalize(lights[i].position - fs_in.FragPos);
		vec3 halfway = normalize(lightDir + viewDir);
		float spec = pow(max(dot(normal, halfway), 0.0), 64);
		vec3 specular = spec * lights[i].color;
        float diff = max(dot(lightDir, normal), 0.0);
        vec3 diffuse = lights[i].color * diff;
		vec3 ambient = lights[i].color * ambient_coef;
        vec3 result = (ambient + diffuse + specular) * color;        
        // attenuation (use quadratic as we have gamma correction)
        float distance = length(fs_in.FragPos - lights[i].position);
		float attenuation = 1.0 / (distance * distance);
        result *= attenuation;
        lighting += result;              
    }
	
    FragColor = vec4(lighting, 1.0);
}