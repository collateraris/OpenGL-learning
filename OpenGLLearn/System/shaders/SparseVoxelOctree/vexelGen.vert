#version 450 core

layout (location = 0) in vec3 aVertexPosition;

uniform mat4 uModelMatrix;

void main() 
{
    gl_Position = uModelMatrix * vec4(aVertexPosition, 1.f);
}