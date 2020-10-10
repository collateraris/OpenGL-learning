#version 430 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedo;
layout (location = 3) out vec3 gRoughnessMetallic;

in VS_OUT {
    vec3 vFragPos;
    vec2 vTexCoords;
} fs_in;

struct Material {
    sampler2D texture_diffuse_0;
    sampler2D texture_specular_0; // for roughness
    sampler2D texture_normal_0;
    sampler2D texture_ambient_0; // for metallic
};

uniform Material material;

void main()
{           
	gPosition = fs_in.vFragPos;
	gNormal = texture(material.texture_normal_0, fs_in.vTexCoords).rgb;
	gAlbedo = texture(material.texture_diffuse_0, fs_in.vTexCoords);
	gRoughnessMetallic.x = texture(material.texture_specular_0, fs_in.vTexCoords).r;
    gRoughnessMetallic.y = texture(material.texture_ambient_0, fs_in.vTexCoords).r;
}