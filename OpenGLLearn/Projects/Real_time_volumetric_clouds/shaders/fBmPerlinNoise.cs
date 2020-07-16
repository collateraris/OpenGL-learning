#version 430

layout (local_size_x = 4, local_size_y = 4, local_size_z = 4) in;

layout (rgba8, binding = 0) uniform image3D outTexture;

void main()
{
    ivec3 pixel = ivec3(gl_GlobalInvocationID.xyz);

    imageStore(outTexture, pixel, vec4(1., 0., 0.5, 1.));
}
