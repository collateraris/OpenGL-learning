#version 450 core

in vec2 vTex;

uniform sampler2D uDepthTex;
uniform float uNear = 0.1;
uniform float uFar = 1000.;

float LinearizeDepth(in float depth)
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * uNear * uFar) / (uFar + uNear - z * (uFar - uNear));
};

void main()
{
    float depth = texture(uDepthTex, vTex).r;
    depth = LinearizeDepth(depth);
    depth /= (uFar - uNear) * 0.05f;
    gl_FragColor = vec4(depth, depth, depth, 1.0f);
}
