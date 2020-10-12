#version 430 core
layout (location = 0) in vec3 aPos;

uniform mat4 uLightSpaceModel;

void main()
{
    gl_Position = uLightSpaceModel * vec4(aPos, 1.0);
}
