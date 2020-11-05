#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTex;

out VS_OUT{
    vec4 vPos;
    vec2 vTex;
    vec3 vNorm;
} fs_out;

uniform mat4 uModelMatrix;

void main()
{
    fs_out.vPos = uModelMatrix * vec4(aPos, 1.0);
    fs_out.vTex = aTex;
    fs_out.vNorm = aNorm;

    gl_Position = fs_out.vPos;
}