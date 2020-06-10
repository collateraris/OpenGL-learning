#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 aInstanceMatrix;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    gl_Position = projection * view * aInstanceMatrix * vec4(aPos, 1.0f);
    vs_out.Normal = normalize(mat3(transpose(inverse(aInstanceMatrix))) * aNormal);
    vs_out.FragPos = vec3(aInstanceMatrix * vec4(aPos, 1.0f));
    vs_out.TexCoords = aTexCoords;   
}