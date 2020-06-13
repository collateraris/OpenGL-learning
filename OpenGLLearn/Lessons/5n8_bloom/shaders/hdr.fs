#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D colorBuffer;
uniform sampler2D bloomBlur;

void main()
{
	float exposure = 0.1;
    vec3 hdrColor = texture(colorBuffer, TexCoords).rgb;
	vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
	hdrColor += bloomColor;
	vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
	const float gamma = 2.2;
	mapped = pow(mapped, vec3(1.0 / gamma));
    FragColor = vec4(mapped, 1.0);
}