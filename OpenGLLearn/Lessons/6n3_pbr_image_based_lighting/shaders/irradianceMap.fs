#version 430 core
precision mediump float;
const float PI = 3.1415926535897932384626;

in vec3 vLocalPos;

uniform samplerCube uEnvironmentMap;

vec3 HemisphereConvolution(in samplerCube environmentMap, in vec3 localPos, float sampleDelta);

void main()
{
    vec3 localPos = normalize(vLocalPos);
    vec3 irradiance = HemisphereConvolution(uEnvironmentMap, localPos, 0.025);

    gl_FragColor = vec4(irradiance, 1.0);
}

vec3 HemisphereConvolution(in samplerCube environmentMap, in vec3 localPos, float sampleDelta)
{
    vec3 up = vec3(0., 1., 0.);
    vec3 right = cross(up, localPos);
    up = cross(localPos, right);

    int nrSamples = 0;
    float PI_2 = 2. * PI;
    float HALF_PI = 0.5 * PI;

    vec3 convResult = vec3(0.);

    for (float phi = 0.; phi < PI_2; phi += sampleDelta)
    {
        for (float theta = 0.; theta < HALF_PI; theta += sampleDelta)
        {
            vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));

            vec3 worldSample = tangentSample.x * right + tangentSample.y * up + tangentSample.z * localPos;

            convResult += texture(environmentMap, worldSample).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
    }

    return PI / float(nrSamples) * convResult;
}
