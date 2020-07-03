#version 330 core

out vec4 FragColor;

struct LightDir
{
	vec3 direction;
	vec3 color;
};
  
in vec3 fragPos;
uniform vec3 viewPos;
uniform LightDir light;
uniform float sinTime;

const vec3 sphereOrigin = vec3(0.0);
const float sphereRadius = 0.75;
const vec3 sphereColor = vec3(1.0, 0., 0.0);
const float sphereSpecularPower = 32.0;

float sphereDistance(vec3 pos, vec3 center, float radius);

vec4 simpleLambert(vec3 normal, vec3 viewDir);

float map(vec3 pos);

vec3 normalEstimate(vec3 pos);

vec4 renderSurface(vec3 pos, vec3 viewDir);

float vmax(vec3 v);

float sdf_boxcheap(vec3 pos, vec3 center, float size);

float sdf_alpha_blend(float d1, float d2, float alpha);

float sdf_sphere(vec3 pos, vec3 center, float radius);

void main()
{	
	vec3 position = fragPos;
	vec3 direction = normalize(position - viewPos);
	
	const int STEPS = 64;
	const float MIN_DISTANCE = 0.01;

	for (int i = 0; i < STEPS; ++i)
	{
		float dist = sdf_alpha_blend(sdf_sphere(position, sphereOrigin, sphereRadius),
									sdf_boxcheap(position, sphereOrigin, sphereRadius), (sinTime + 1.) * 0.5);
		if (dist < MIN_DISTANCE)
		{
			FragColor =  renderSurface(position, direction);
			return;
		}
		position += direction * dist;
	}
	FragColor = vec4(1.0);
}

float sphereDistance(vec3 pos, vec3 center, float radius)
{
	return distance(pos, center) - radius;
}

vec4 simpleLambert(vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(light.direction);
	float NdotLightDir = max(dot(normal, lightDir), 0.0);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	vec3 specular = pow(max(dot(normal, halfwayDir),0.0), sphereSpecularPower) * light.color;
	vec3 ambient = sphereColor * 0.3;
	vec4 color;
	color.xyz = light.color * sphereColor * NdotLightDir + specular + ambient;
	color.w = 1;
	return color;
}

float map(vec3 pos)
{
	return sphereDistance(pos, sphereOrigin, sphereRadius);
}

vec3 normalEstimate(vec3 pos)
{
	const float eps = 0.01;
	return normalize
	 ( vec3( map(pos + vec3(eps, 0, 0) ) - map(pos - vec3(eps, 0, 0)),
		 map(pos + vec3(0, eps, 0) ) - map(pos - vec3(0, eps, 0)),
		 map(pos + vec3(0, 0, eps) ) - map(pos - vec3(0, 0, eps))
		 )
	 );
}

vec4 renderSurface(vec3 pos, vec3 viewDir)
{
	vec3 n = normalEstimate(pos);
	return simpleLambert(n, viewDir);
}

float vmax(vec3 v)
{
	return max(max(v.x, v.y), v.z);
}

float sdf_boxcheap(vec3 pos, vec3 center, float size)
{
	return vmax(abs(pos - center) - size);
}

float sdf_alpha_blend(float d1, float d2, float alpha)
{
	return alpha * d1 + (1 - alpha) * d2;
}

float sdf_sphere(vec3 pos, vec3 center, float radius)
{
	return sphereDistance(pos, center, radius);
}
