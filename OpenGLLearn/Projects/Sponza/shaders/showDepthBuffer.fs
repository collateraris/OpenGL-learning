#version 430 core

in vec2 vTexCoords;

uniform sampler2D uDepthMap;

void main()
{
    float depthValue = texture(uDepthMap, vTexCoords).r;
    gl_FragColor = vec4(vec3(depthValue), 1.0); 
}