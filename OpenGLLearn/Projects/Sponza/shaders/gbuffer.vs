#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT {
    vec3 vFragPos;
    vec2 vTexCoords;
} vs_out;

uniform mat4 uModel;
uniform mat4 uProjectionView;

void main()
{
    vec4 wPos = uModel * vec4(aPos, 1.);
    vs_out.vFragPos = vec3(wPos);
    vs_out.vTexCoords = aTexCoords;
    gl_Position = uProjectionView * wPos;
}
	