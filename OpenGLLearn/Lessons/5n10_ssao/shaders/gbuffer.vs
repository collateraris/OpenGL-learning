#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in mat4 aInstanceMatrix;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

void main()
{
	vec4 viewPos = view * aInstanceMatrix * vec4(aPos, 1.0f);
    gl_Position = projection * viewPos;
	vs_out.Normal = normalize(mat3(transpose(aInstanceMatrix)) * aNormal);
    vs_out.FragPos = viewPos.rgb;
    vs_out.TexCoords = aTexCoords;
}
	