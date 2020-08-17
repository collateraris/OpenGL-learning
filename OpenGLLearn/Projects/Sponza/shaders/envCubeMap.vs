#version 430 core
layout (location = 0) in vec3 aPos;

uniform mat4 uProjectionRotView;

out vec3 vLocalPos;

void main()
{
    vLocalPos = aPos;
    vec4 clipPos = uProjectionRotView * vec4(vLocalPos, 1.);

    gl_Position = clipPos.xyww;
} 