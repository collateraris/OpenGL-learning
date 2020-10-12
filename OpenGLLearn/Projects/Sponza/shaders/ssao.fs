#version 430 core
out float FragColor;

in vec2 vTexCoords;
in vec2 vViewRay;

uniform sampler2D uDepthMap;

uniform vec3 uSamples[64];
uniform int uKernelSize;// = 32;
uniform float uRadius;// = 0.5;
uniform float uBias;// = 0.0025;

uniform mat4 uProjection;

float CalcViewZ(in sampler2D depthMap, in vec2 uv, in mat4 projection)
{
    float Depth = texture(depthMap, uv).x;
    float ViewZ = projection[3][2] / (2 * Depth - 1 - projection[2][2]);
    return ViewZ;
}

void main()
{
    // get input for SSAO algorithm
    float viewZ = CalcViewZ(uDepthMap, vTexCoords, uProjection);
    float viewX = vViewRay.x * viewZ;
    float viewY = vViewRay.y * viewZ;

    vec3 fragPos = vec3(viewX, viewY, viewZ);

    float occlusion = 0.0;
    int kernelSize = uKernelSize;
    float radius = uRadius;
    float bias = uBias;
    for(int i = 0; i < kernelSize; ++i)
    {
        // get sample position
        vec3 samplePos = fragPos + uSamples[i]; 
        vec4 offset = vec4(samplePos , 1.0);
        offset = uProjection * offset;
        offset.xy /= offset.w;
        offset.xy = offset.xy * 0.5 + vec2(0.5);
        // get sample depth
        float sampleDepth = CalcViewZ(uDepthMap, offset.xy, uProjection); 
        
        if (abs(fragPos.z - sampleDepth) <= radius) 
        {
            occlusion += step(sampleDepth, samplePos.z + bias);
        }       
    }
    occlusion = 1.0 - (occlusion / kernelSize);
    
    FragColor = pow(occlusion, 2.0);
}