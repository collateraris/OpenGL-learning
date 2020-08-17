#version 430 core
layout (location = 0) in vec3 aPos;

uniform mat4 uProjectionView;

out vec3 vLocalPos;

void main()
{
    vLocalPos = aPos;

    gl_Position = uProjectionView * vec4(aPos, 1.);
} 