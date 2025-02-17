#version 330 core
out float FragColor;
  
in vec2 TexCoords;

uniform sampler2D positionMap;
uniform sampler2D normalMap;
uniform sampler2D texNoise;

uniform vec3 samples[64];
uniform mat4 projection;

uniform vec2 noiseScale;

int kernelSize = 64;
float radius = 0.5;
float bias = 0.025;

void main()
{	
    vec3 fragPos   = texture(positionMap, TexCoords).xyz;
	vec3 normal    = texture(normalMap, TexCoords).rgb;
	vec3 randomVec = texture(texNoise, TexCoords * noiseScale).xyz;
	
	vec3 tangent   = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN       = mat3(tangent, bitangent, normal); 
	
	float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i)
	{
		vec3 sample = TBN * samples[i];
		sample = fragPos + sample * radius;
		
		vec4 offset = vec4(sample, 1.0);
		offset      = projection * offset; 
		offset.xyz /= offset.w;
		offset.xyz  = offset.xyz * 0.5 + 0.5;

		float sampleDepth = texture(positionMap, offset.xy).z;
		occlusion += (sampleDepth >= sample.z + bias ? 1.0 : 0.0);
	}
	
	occlusion = 1.0 - (occlusion / kernelSize);   
    FragColor = occlusion;
}