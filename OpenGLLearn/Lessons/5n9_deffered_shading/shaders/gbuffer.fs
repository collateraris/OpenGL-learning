#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

struct Material {
    sampler2D texture_diffuse_0;
    sampler2D texture_specular_0;
    float shininess;
}; 

uniform Material material;

void main()
{           
	gPosition = fs_in.FragPos;
	gNormal = fs_in.Normal;
	gAlbedoSpec.rgb = texture(material.texture_diffuse_0, fs_in.TexCoords).rgb;
	gAlbedoSpec.a = texture(material.texture_specular_0, fs_in.TexCoords).r;
}