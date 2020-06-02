#version 330 core
out vec4 color;

in VS_OUT
{
    vec3 Normal;
	vec3 Position;
} vs_in;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main()
{    
	float ratio = 1.00 / 1.330;
    vec3 I = normalize(vs_in.Position - cameraPos);
    vec3 R = refract(I, normalize(vs_in.Normal), ratio);
    color = vec4(texture(skybox, R).rgb, 1.0);
}