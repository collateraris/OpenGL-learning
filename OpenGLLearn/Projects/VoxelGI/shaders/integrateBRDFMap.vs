#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec3 vLocalPos;
out vec2 vTexCoords;

void main()
{
    vLocalPos = aPos;
    vTexCoords = aTexCoords;

    gl_Position = vec4(aPos, 1.);
} 