#version 430 core

in vec3 vPosition;

uniform sampler3D uTexture;

void main()
{
    vec3 value = texture(uTexture, vec3(vPosition.x, vPosition.y, 1)).rgb; 
    gl_FragColor = vec4(value, 1.);
}