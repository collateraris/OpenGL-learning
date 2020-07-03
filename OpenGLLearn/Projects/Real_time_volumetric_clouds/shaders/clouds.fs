#version 330 core

out vec4 FragColor;
  
in vec3 fragPos;
uniform vec3 viewPos;

float sphereDistance(vec3 pos, vec3 center, float radius);

void main()
{	
	const vec3 rayOrigin = vec3(0.0, 6371000.0, 0.0);
	const vec3 sphereOrigin = vec3(0.0);
	const float sphereRadius = 6471000.0;
	vec3 position = fragPos;
	vec3 direction = normalize(position - viewPos);
	
	const int STEPS = 64;
	const float MIN_DISTANCE = 0.1;

	for (int i = 0; i < STEPS; ++i)
	{
		float dist = sphereDistance(position, sphereOrigin, 1.);
		if (dist < MIN_DISTANCE)
		{
			FragColor = vec4(1.0, 0., 0.0, 1.0);
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
