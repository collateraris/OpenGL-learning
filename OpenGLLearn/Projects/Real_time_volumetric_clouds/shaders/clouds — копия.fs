#version 330 core
#define M_PI 3.1415926535897932384626433832795
out vec4 FragColor;
  
struct SunLight {
    vec3 direction;
	vec3 color;
};

in vec3 fragPos;

uniform SunLight sunLight;
uniform vec3 viewPos;
uniform mat4 invMVP;

vec3 ScatteringCalculation(SunLight sunLight, vec3 viewDir, vec3 rayOrigin, vec3 intersectionPos);

bool RaySphereIntersection(vec3 rayOrigin, vec3 rayDirection, float sphereRadius, vec3 sphereOrigin, inout vec3 position); 

void main()
{	
	const vec3 rayOrigin = vec3(0.0, 6371000.0, 0.0);
	const vec3 sphereOrigin = vec3(0.0);
	const float sphereRadius = 6471000.0;
	vec3 FragCoord = gl_FragCoord.xyz;
	//FragCoord = vec3( invMVP * vec4(FragCoord, 1.0));
	//vec2 iResolution = vec2(800, 600);
	//FragCoord.xy = FragCoord.xy / iResolution.xy;
	//FragCoord.xy = FragCoord.xy * 2. - vec2(1.0);
	//FragCoord.x *= iResolution.x/iResolution.y;
	//vec3 position = vec3( invMVP * vec4(FragCoord, 1.0));
	vec3 position = viewPos;
	vec3 viewDir = normalize(position - FragCoord);
	
	const int STEPS = 64;
	float STEP_SIZE = 0.01;

	for (int i = 0; i < STEPS; ++i)
	{
		float dist = distance(sphereOrigin, position);
		if (dist < 1)
		{
			FragColor = vec4(1.0, 0., 0.0, 1.0);
			return;
		}
		position += viewDir * STEP_SIZE;
	}
	FragColor = vec4(1.0);
	return;
	
	//vec3 position = vec3(0.0);
	if(RaySphereIntersection(rayOrigin, viewDir, sphereRadius, sphereOrigin, position))
	{
		FragColor = vec4(1.0, 0., 0.0, 1.0);		
	}
	else
	{
		FragColor = vec4(1.0);
	}
}

bool RaySphereIntersection(vec3 rayOrigin, vec3 rayDirection, float sphereRadius, vec3 sphereOrigin, inout vec3 position)
{	
	vec3 L = sphereOrigin - rayOrigin;
	float tca = dot(rayDirection, L);
	if (tca < 0) return false;
	
	float sqrRadius = sphereRadius * sphereRadius;
	float sqrD = dot(L,L) - tca * tca;
	
	if (sqrD > sqrRadius) return false;
	
	float thc = sqrt(sqrRadius - sqrD);
	float t0 = tca - thc; 
    float t1 = tca + thc;
	
	if (t0 > t1)
	{
		position = rayOrigin + t1 * rayDirection;
	}
	else
	{
		position = rayOrigin + t0 * rayDirection;
	}
	
	return true;
}

vec3 ScatteringCalculation(SunLight sunLight, vec3 viewDir, vec3 rayOrigin, vec3 intersectionPos)
{
	const int STEP_COUNT = 32;
	float dist = distance(rayOrigin, intersectionPos);
	float deltaDist = dist / STEP_COUNT;
	
	float angleBetweenViewAndLight = dot(sunLight.direction, viewDir);
	//Rayleigh scattering
	float Pr = 3 * M_PI / 16. * (1. + angleBetweenViewAndLight * angleBetweenViewAndLight);
	vec3 BoR = vec3(58e-7, 135e-7, 331e-7);
	float H0R = 8000; //metres
	
	vec3 scatteringResult = vec3(0.0);
	vec3 rayleighScattering = vec3(0.0);
	for(float h = 0.0; h < dist; h += deltaDist)
	{
		
	}
	
	return scatteringResult;
}