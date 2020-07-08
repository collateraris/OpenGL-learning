#version 330 core
precision mediump float; 
in vec3 vPosition;

uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform float u_time;

vec2 random2( vec2 p ) {
    return fract(sin(vec2(dot(p,vec2(127.1,311.7)),dot(p,vec2(269.5,183.3))))*43758.5453);
}

void main()
{
    vec2 st = gl_FragCoord.xy / u_resolution.xy;
    st.x *= u_resolution.x / u_resolution.y;

    vec3 color = vec3(0.);
    
    st *= 10.;

    vec2 ist = floor(st);
    vec2 fst = fract(st);

    float minDist = 1.;
    vec2 minPoint;

    for (int y = -1; y <= 1; y++) 
        for (int x = -1; x <= 1; x++) 
        {
            vec2 neighbor = vec2(x, y);
            vec2 point = random2(ist + neighbor);
            point = 0.5 + 0.25 * cos(u_time + 6.2831 * point) + 0.25 * sin(u_time + 6.2831 * point);

            vec2 diff = neighbor + point - fst;

            float dist = length(diff);

            if (dist < minDist)
            {
                minDist = dist;
                minPoint = point;
            }
        }

    color += minDist * 2.;
    color.rg = minPoint;

    color -= abs(sin(80. * minDist)) * 0.07;

    color += 1.- step(0.02, minDist);
    color.r += step(.98, fst.x) + step(.98, fst.y);    

    gl_FragColor = vec4(color, 1.0);
}

