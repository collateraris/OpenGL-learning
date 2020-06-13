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
    vec3 I = normalize(vs_in.Position - cameraPos);
    vec3 R = reflect(I, normalize(vs_in.Normal));
    color = vec4(texture(skybox, R).rgb, 1.0);
}