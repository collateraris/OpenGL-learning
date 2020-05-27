#version 330 core
layout (location = 0) in vec3 position; 
layout (location = 1) in vec3 color; 

uniform float rightShiftX;

out vec3 ourColor; 

void main()
{
    gl_Position = vec4(position.x + rightShiftX, position.y * -1.f, position.z, 1.0f);
    ourColor = color; 
}   