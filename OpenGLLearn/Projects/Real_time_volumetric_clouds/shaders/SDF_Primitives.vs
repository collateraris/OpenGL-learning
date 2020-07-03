#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 texCoords;

uniform mat4 mvp;

out vec3 fragPos;

void main()
{
    gl_Position = mvp * vec4(aPos, 1.0);
	fragPos = aPos;
}