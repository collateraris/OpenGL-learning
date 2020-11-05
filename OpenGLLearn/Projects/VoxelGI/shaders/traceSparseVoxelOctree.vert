#version 450 core

layout (location = 0) in vec3 aVertexPosition;

uniform mat4 uInvViewMatrix;
uniform mat4 uInvProjectionMatrix;

out vec3 vPassDir;

void main()
{
    gl_Position = vec4(aVertexPosition, 1.0f);

    vec4 origin = uInvViewMatrix * vec4(0, 0, 0, 1);
    vec4 originPlusDir = (uInvViewMatrix * uInvProjectionMatrix) * vec4(aVertexPosition.xy, 0, 1);
    originPlusDir.xyz /= originPlusDir.w;
    vPassDir = normalize(vec3(originPlusDir - origin));
}