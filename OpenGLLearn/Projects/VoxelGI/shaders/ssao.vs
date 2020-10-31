#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 vTexCoords;
out vec2 vViewRay;

uniform float uAspectRatio;
uniform float uTanHalfFOV;

void main()
{
    vTexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);

    vViewRay.x = aPos.x * uAspectRatio * uTanHalfFOV;
    vViewRay.y = aPos.y * uAspectRatio;
}