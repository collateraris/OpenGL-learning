#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT {
    vec3 vFragPos;
    vec2 vTexCoords;
    vec3 vFragPosInView;
    vec3 vNormalInView;
} vs_out;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjectionView;

void main()
{
    vec4 wPos = uModel * vec4(aPos, 1.);
    mat3 ViewModel = mat3(uView * uModel);
    vs_out.vFragPos = vec3(wPos);
    vs_out.vFragPosInView = ViewModel * aPos;
    vs_out.vTexCoords = aTexCoords;
    mat3 normalMatrix = transpose(inverse(mat3(ViewModel)));
    vs_out.vNormalInView = normalMatrix * aNormal;
    gl_Position = uProjectionView * wPos;
}
	