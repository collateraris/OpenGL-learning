#version 430 core

layout (local_size_x = 4, local_size_y = 4, local_size_z = 4) in;

layout (rgba8, binding = 0) uniform image3D outTexture;

uniform vec3 uResolution;

float perlinNoise3( in vec3 p );

float voronoiNoise2( in vec2 st);

float random( float n );

vec2 random2( vec2 n );

#define MOD3 vec3(.1031,.11369,.13787)
vec3 hash( in vec3 p );

void main()
{
    vec3 st = gl_GlobalInvocationID.xyz;
    ivec3 pixel = ivec3(st);
    st /= uResolution;
    st.x *= uResolution.x / uResolution.y;
    st *= 2. - 1;
    st *= 4.;
    vec2 uv2 = vec2(st);
    mat2 m = mat2(1.5, 1.3, -1.5, 1.3 );
    float w = (1.0 + perlinNoise3(vec3(st))) * 
              ((1.0 + voronoiNoise2(uv2)) + 
              (0.5 * voronoiNoise2(uv2 * 2.)) + 
              (0.25 * voronoiNoise2(uv2 * 4.)));

    // Draw the min distance (distance field)
    vec3 color = vec3(w * 0.25);

    imageStore(outTexture, pixel, vec4(color, 1.));
}

float perlinNoise3( in vec3 p )
{
    vec3 i = floor( p );
    vec3 f = fract( p );
	
	vec3 u = f*f*(3.0-2.0*f);

    return mix( mix( mix( dot( hash( i + vec3(0.0,0.0,0.0) ), f - vec3(0.0,0.0,0.0) ), 
                          dot( hash( i + vec3(1.0,0.0,0.0) ), f - vec3(1.0,0.0,0.0) ), u.x),
                     mix( dot( hash( i + vec3(0.0,1.0,0.0) ), f - vec3(0.0,1.0,0.0) ), 
                          dot( hash( i + vec3(1.0,1.0,0.0) ), f - vec3(1.0,1.0,0.0) ), u.x), u.y),
                mix( mix( dot( hash( i + vec3(0.0,0.0,1.0) ), f - vec3(0.0,0.0,1.0) ), 
                          dot( hash( i + vec3(1.0,0.0,1.0) ), f - vec3(1.0,0.0,1.0) ), u.x),
                     mix( dot( hash( i + vec3(0.0,1.0,1.0) ), f - vec3(0.0,1.0,1.0) ), 
                          dot( hash( i + vec3(1.0,1.0,1.0) ), f - vec3(1.0,1.0,1.0) ), u.x), u.y), u.z );
}

float voronoiNoise2( in vec2 st)
{
    vec2 i = floor(st);
    vec2 f = fract(st);

    float dis = 2.0;
    for (int y= -1; y <= 1; y++) 
    {
        for (int x= -1; x <= 1; x++) 
        {
            // Neighbor place in the grid
            vec2 p = i + vec2(x,y);

            float d = length(random2(p) + vec2(x, y) - f);
            if (dis > d)
            {
             	dis = d;   
            }
        }
    }
    
    return 1.0 - dis;
}

float random( float n )
{
 	return fract(cos(n*89.42)*343.42);
}

vec2 random2( vec2 n )
{
 	return vec2(random(n.x*23.62-300.0+n.y*34.35), random(n.x*45.13+256.0+n.y*38.89)); 
}

vec3 hash( in vec3 p )
{
	p = vec3( dot(p,vec3(127.1,311.7, 74.7)),
			  dot(p,vec3(269.5,183.3,246.1)),
			  dot(p,vec3(113.5,271.9,124.6)));

	return -1.0 + 2.0*fract(sin(p)*43758.5453123);
}




