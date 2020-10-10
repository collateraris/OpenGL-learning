#version 430 core
layout (location = 0) in vec3 aPos;

uniform mat4 uLightSpaceModelMatrix;

void main()
{
    gl_Position = uLightSpaceModelMatrix * vec4(aPos, 1.0);
}
