#version 330 core
#define M_PI 3.1415926535897932384626433832795

precision highp float; 
in vec3 vPosition;
uniform vec3 uSunPos;
uniform vec2 resolution;
uniform vec3 eye;

vec3 atmosphere(vec3 rayDir, vec3 rayOrigin, vec3 sunPos, float sunIntensity);

vec2 rsi(vec3 r0, vec3 rd, float sr); 

void main()
{	
	vec3 color = vec3(0.0);
			
	vec3 dir = normalize(vec3(gl_FragCoord.xy - resolution * 0.5, 
                                  -resolution.y / tan(M_PI * 0.167)));
	vec3 f = normalize(-eye); 
	vec3 s = normalize(cross(f, vec3(.0,.1,.0)));
	vec3 worldDir = mat3(s,cross(s, f),-f) * dir;
	
    if (worldDir.y > 0.) 
         color = atmosphere(normalize(eye), vec3(0,6372e3,0), uSunPos, 22.0);
	color = 1.0 - exp(-1.0 * color);	 
    gl_FragColor = vec4(color, 1);
}


vec3 atmosphere(vec3 rayDir, vec3 rayOrigin, vec3 sunPos, float sunIntensity)
{
	rayDir = normalize(rayDir);
	sunPos = normalize(sunPos);
	
	const float rPlanet = 6371e3;
	const float rAtmos = 6471e3;
	
	float tA, tB;
	vec2 p = rsi(rayOrigin, rayDir, rAtmos);
    if (p.x > p.y) return vec3(0,0,0);
    p.y = min(p.y, rsi(rayOrigin, rayDir, rPlanet).x);
	tA = p.x;
	tB = p.y;

	const int iSteps = 16;
	const int jSteps = 16;
	const float inviSteps = 0.0625;
	const float invjSteps = 0.0625;
	const vec3 kRlh = vec3(5.8e-6, 13.5e-6, 33.1e-6);
	const vec3 kMie = vec3(3e-6);
	const float shRlh = 8e3;
	const float shMie = 1.2e3;
	const float invshRlh = 1./ shRlh;
	const float invshMie = 1./ shMie;
	const float gMie = 0.758;
	float mu = dot(rayDir, sunPos);
	float mumu = mu * mu;
	float gg = gMie * gMie;
	float pRlh = 3.0 / (16.0 * M_PI) * (1.0 + mumu);
	float pMie = 3.0 / (8.0 * M_PI) * ((1.0 - gg) * (mumu + 1.0)) / (pow(1.0 + gg - 2.0 * mu * gMie, 1.5) * (2.0 + gg));
	
	float iTime = tA;
	float idS = (tB - tA) * inviSteps;
	
	float iOdRlh = 0.0;
    float iOdMie = 0.0;
	
	// Initialize accumulators for Rayleigh and Mie scattering.
    vec3 totalRlh = vec3(0,0,0);
    vec3 totalMie = vec3(0,0,0);
	
	for (int i = 0; i < iSteps; i++)
	{
		vec3 iPos = rayOrigin + rayDir * (iTime + idS * 0.5);
		
		float iHeight = length(iPos) - rPlanet;
		
		// Calculate the optical depth of the Rayleigh and Mie scattering for this step.
        float odStepRlh = exp(-iHeight * invshRlh) * idS;
        float odStepMie = exp(-iHeight * invshMie) * idS;

        // Accumulate optical depth.
        iOdRlh += odStepRlh;
        iOdMie += odStepMie;
		
		// Initialize optical depth accumulators for the secondary ray.
		float jOdRlh = 0.0;
		float jOdMie = 0.0;
		
		float jdS = rsi(iPos, sunPos, rAtmos).y * invjSteps;
		
		// Initialize the secondary ray time.
		float jTime = 0.0;

		// Sample the secondary ray.
		for (int j = 0; j < jSteps; j++) {

			// Calculate the secondary ray sample position.
			vec3 jPos = iPos + sunPos * (jTime + jdS * 0.5);

			// Calculate the height of the sample.
			float jHeight = length(jPos) - rPlanet;

			// Accumulate the optical depth.
			jOdRlh += exp(-jHeight * invshRlh) * jdS;
			jOdMie += exp(-jHeight * invshMie) * jdS;

			// Increment the secondary ray time.
			jTime += jdS;
		}
		
		// Calculate attenuation.
        vec3 attn = exp(-(kMie * (iOdMie + jOdMie) + kRlh * (iOdRlh + jOdRlh)));

        // Accumulate scattering.
        totalRlh += odStepRlh * attn;
        totalMie += odStepMie * attn;
		
		iTime += idS;
	}
	
	return sunIntensity * (pRlh * kRlh * totalRlh + pMie * kMie * totalMie);
}

vec2 rsi(vec3 r0, vec3 rd, float sr)
{
    // ray-sphere intersection that assumes
    // the sphere is centered at the origin.
    // No intersection when result.x > result.y
    float a = dot(rd, rd);
    float b = 2.0 * dot(rd, r0);
    float c = dot(r0, r0) - (sr * sr);
    float d = (b*b) - 4.0*a*c;
    if (d < 0.0) return vec2(1e5,-1e5);
    return vec2(
        (-b - sqrt(d))/(2.0*a),
        (-b + sqrt(d))/(2.0*a)
    );
}

