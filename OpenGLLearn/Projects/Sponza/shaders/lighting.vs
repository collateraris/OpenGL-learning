#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;

uniform mat4 uProjectionView;
uniform mat4 uModel;

out mat3 vTBN;
out vec3 vWorldPos;
out vec2 vTexCoords;

void main()
{
    vWorldPos = vec3(uModel * vec4(aPos, 1.));
    vTexCoords = aTexCoords;
    vec3 T = normalize(vec3(uModel * vec4(aTangent, 0.0)));
    vec3 N = normalize(vec3(uModel * vec4(aNormal,0.0)));
    vec3 B = cross(N, T);
    vTBN = mat3(T, B, N);

    gl_Position = uProjectionView * vec4(vWorldPos, 1.);
} 