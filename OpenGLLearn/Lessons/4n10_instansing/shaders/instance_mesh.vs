#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in mat4 instanceMatrix;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

void main()
{
    gl_Position = projection * view * instanceMatrix * vec4(position, 1.0f);
    Normal = mat3(transpose(inverse(instanceMatrix))) * normal;
    FragPos = vec3(instanceMatrix * vec4(position, 1.0f));
    TexCoords = texCoords;
} 