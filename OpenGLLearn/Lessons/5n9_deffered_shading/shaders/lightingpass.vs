#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out VS_OUT {
	vec2 TexCoords;
	vec3 viewDir;
	vec3 FragPos;
	mat3 TBN;
} fs_out;

uniform sampler2D tangentMap;
uniform sampler2D normalMap;
uniform sampler2D positionMap;
uniform vec3 viewPos;


void main()
{
    fs_out.TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
	vec3 N = texture(normalMap, aTexCoords).rgb;
	vec3 T = texture(tangentMap, aTexCoords).rgb;
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);
	fs_out.TBN = transpose(mat3(T, B, N));
	fs_out.FragPos = texture(positionMap, aTexCoords).rgb;
	fs_out.viewDir = fs_out.TBN * normalize(viewPos - fs_out.FragPos);
}