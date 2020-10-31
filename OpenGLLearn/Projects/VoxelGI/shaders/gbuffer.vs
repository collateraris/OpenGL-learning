#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;

out VS_OUT {
    mat3 vTBN;
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
    vec3 T = normalize(vec3(uModel * vec4(aTangent,   0.0)));
    vec3 N = normalize(vec3(uModel * vec4(aNormal,    0.0)));
    vec3 B = cross(N, T);
    vs_out.vTBN = mat3(T, B, N);
    gl_Position = uProjectionView * wPos;
}
	