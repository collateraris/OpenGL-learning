#version 430 core

in vec2 vTexCoords;

uniform sampler2D uPosition;
uniform sampler2D uNormal;
uniform sampler2D uAO;

uniform vec3 uLightPosition;
uniform vec3 uLightColor;

void main()
{
    const float linear    = 0.09;
    const float quadratic = 0.032;

    // retrieve data from gbuffer
    vec3 FragPos = texture(uPosition, vTexCoords).rgb;
    vec3 Normal = texture(uNormal, vTexCoords).rgb;
    vec3 Diffuse = vec3(0.95);
    float AmbientOcclusion = texture(uAO, vTexCoords).r;
    
    // then calculate lighting as usual
    vec3 ambient = vec3(0.3 * Diffuse * AmbientOcclusion);
    vec3 lighting  = ambient; 
    vec3 viewDir  = normalize(-FragPos); // viewpos is (0.0.0)
    // diffuse
    vec3 lightDir = normalize(uLightPosition - FragPos);
    vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * uLightColor;
    // specular
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), 8.0);
    vec3 specular = uLightColor * spec;
    // attenuation
    float distance = length(uLightPosition - FragPos);
    float attenuation = 1.0 / (1.0 + linear * distance + quadratic * distance * distance);
    diffuse *= attenuation;
    specular *= attenuation;
    lighting += diffuse + specular;

    gl_FragColor = vec4(lighting, 1.0);
}