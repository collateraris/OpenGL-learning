#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 texCoords;

uniform mat4 mvp;

out vec3 vPosition;

void main()
{
    gl_Position = vec4(aPos, 1.0);
	vPosition = aPos;
}