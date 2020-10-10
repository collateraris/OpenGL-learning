#version 430 core
out float FragColor;

in vec2 vTexCoords;
in mat3 vViewMatrix;

uniform sampler2D uPosition;
uniform sampler2D uNormal;
uniform sampler2D uNoise;

uniform vec3 uSamples[64];
uniform vec2 uNoiseScale;
uniform int uKernelSize;// = 64;
uniform float uRadius;// = 0.5;
uniform float uBias;// = 0.025;

uniform mat4 uProjection;

void main()
{
    // transform to view space
    vec3 fragPos = vViewMatrix * texture(uPosition, vTexCoords).xyz;
    vec3 normal =  vViewMatrix * normalize(texture(uNormal, vTexCoords).rgb);
    // 
    vec3 randomVec = normalize(texture(uNoise, vTexCoords * uNoiseScale).xyz);

    // create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    // iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;
    for(int i = 0; i < uKernelSize; ++i)
    {
        vec3 samplePos = TBN * uSamples[i]; // from tangent to view-space
        samplePos = fragPos + samplePos * uRadius;

        vec4 offset = vec4(samplePos, 1.0);
        offset = uProjection * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0 

        float sampleDepth = texture(uPosition, offset.xy).z;

        float rangeCheck = smoothstep(0.0, 1.0, uRadius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + uBias ? 1.0 : 0.0) * rangeCheck;
    }

    occlusion = 1.0 - (occlusion / uKernelSize);
    
    FragColor = occlusion;
}
