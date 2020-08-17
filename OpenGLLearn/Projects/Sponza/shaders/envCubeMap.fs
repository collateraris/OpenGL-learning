#version 430 core
precision mediump float;
 
in vec3 vLocalPos;

uniform samplerCube uEnvironmentMap;

void main()
{
    vec3 envColor = textureLod(uEnvironmentMap, vLocalPos, 0.).rgb;
    
    envColor = envColor / (envColor + vec3(1.0));
    envColor = pow(envColor, vec3(1.0/2.2)); 

    gl_FragColor = vec4(envColor, 1.0);
}



