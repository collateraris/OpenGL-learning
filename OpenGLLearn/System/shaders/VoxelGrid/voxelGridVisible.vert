#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

out VS_OUT
{
    vec4 vPos;
    vec2 vTex;
} fs_out;

void main()
{
    fs_out.vPos = vec4(aPos, 1.0);
    fs_out.vTex = aTex;

    gl_Position = fs_out.vPos;
}