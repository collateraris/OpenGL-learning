#version 330 core

#define PI 3.141592
#define iSteps 16
#define jSteps 8
#define MOD3 vec3(.16532,.17369,.15787)

precision highp float; 
in vec3 vPosition;
uniform vec3 uSunPos;
uniform float time;
uniform vec2 resolution;
uniform vec3 eye;

vec2 rsi(vec3 r0, vec3 rd, float sr) {
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

vec3 atmosphere(vec3 r, vec3 r0, vec3 pSun, float iSun, float rPlanet, float rAtmos, vec3 kRlh, float kMie, float shRlh, float shMie, float g) {
    // Normalize the sun and view directions.
    pSun = normalize(pSun);
    r = normalize(r);

    // Calculate the step size of the primary ray.
    vec2 p = rsi(r0, r, rAtmos);
    if (p.x > p.y) return vec3(0,0,0);
    p.y = min(p.y, rsi(r0, r, rPlanet).x);
    float iStepSize = (p.y - p.x) / float(iSteps);

    // Initialize the primary ray time.
    float iTime = 0.0;

    // Initialize accumulators for Rayleigh and Mie scattering.
    vec3 totalRlh = vec3(0,0,0);
    vec3 totalMie = vec3(0,0,0);

    // Initialize optical depth accumulators for the primary ray.
    float iOdRlh = 0.0;
    float iOdMie = 0.0;

    // Calculate the Rayleigh and Mie phases.
    float mu = dot(r, pSun);
    float mumu = mu * mu;
    float gg = g * g;
    float pRlh = 3.0 / (16.0 * PI) * (1.0 + mumu);
    float pMie = 3.0 / (8.0 * PI) * ((1.0 - gg) * (mumu + 1.0)) / (pow(1.0 + gg - 2.0 * mu * g, 1.5) * (2.0 + gg));

    // Sample the primary ray.
    for (int i = 0; i < iSteps; i++) {

        // Calculate the primary ray sample position.
        vec3 iPos = r0 + r * (iTime + iStepSize * 0.5);

        // Calculate the height of the sample.
        float iHeight = length(iPos) - rPlanet;

        // Calculate the optical depth of the Rayleigh and Mie scattering for this step.
        float odStepRlh = exp(-iHeight / shRlh) * iStepSize;
        float odStepMie = exp(-iHeight / shMie) * iStepSize;

        // Accumulate optical depth.
        iOdRlh += odStepRlh;
        iOdMie += odStepMie;

        // Calculate the step size of the secondary ray.
        float jStepSize = rsi(iPos, pSun, rAtmos).y / float(jSteps);

        // Initialize the secondary ray time.
        float jTime = 0.0;

        // Initialize optical depth accumulators for the secondary ray.
        float jOdRlh = 0.0;
        float jOdMie = 0.0;

        // Sample the secondary ray.
        for (int j = 0; j < jSteps; j++) {

            // Calculate the secondary ray sample position.
            vec3 jPos = iPos + pSun * (jTime + jStepSize * 0.5);

            // Calculate the height of the sample.
            float jHeight = length(jPos) - rPlanet;

            // Accumulate the optical depth.
            jOdRlh += exp(-jHeight / shRlh) * jStepSize;
            jOdMie += exp(-jHeight / shMie) * jStepSize;

            // Increment the secondary ray time.
            jTime += jStepSize;
        }

        // Calculate attenuation.
        vec3 attn = exp(-(kMie * (iOdMie + jOdMie) + kRlh * (iOdRlh + jOdRlh)));

        // Accumulate scattering.
        totalRlh += odStepRlh * attn;
        totalMie += odStepMie * attn;

        // Increment the primary ray time.
        iTime += iStepSize;

    }

    // Calculate and return the final color.
    return iSun * (pRlh * kRlh * totalRlh + pMie * kMie * totalMie);
}

float Hash(vec3 p) {
        p  = fract(p * MOD3);
        p += dot(p.xyz, p.yzx + 19.19);
        return fract(p.x * p.y * p.z);
    }
	
float hashMix(vec3 i, vec3 f, float y, float z) {
	return mix(Hash(i + vec3(0.,y,z)), Hash(i + vec3(1.,y,z)),f.x);
}

float Noise(in vec3 p) {
	vec3 i = floor(p), f = fract(p);
	f *= f * (3.0-2.0*f);
	return mix(mix(hashMix(i,f,0.,0.), hashMix(i,f,1.,0.), f.y),
			   mix(hashMix(i,f,0.,1.), hashMix(i,f,1.,1.), f.y), f.z);
}

float CloudsFBM(vec3 p) {
	float f = 0.5 * Noise(p); p = p * 3.1;
	f += 0.250000 * Noise(p); p = p * 3.2;
	f += 0.125000 * Noise(p); p = p * 3.3;
	f += 0.062500 * Noise(p); p = p * 3.4;
	f += 0.031250 * Noise(p); p = p * 3.5;
	f += 0.015625 * Noise(p);
	return f;
}

float MapClouds(vec3 p, vec3 cameraPosition){
	p /= 1000.; // cloud size
	float dist = length(p-cameraPosition);
	p.z += time / 100. * 50.; // clouds speed
	return CloudsFBM(p) +0.01*dist - .5;
}

vec3 CreateClouds(vec3 ro, vec3 rd, float upper, float lower) {
	float diff = upper - lower,
		  beg = ((lower - ro.y) / rd.y),
		  end = ((upper - ro.y) / rd.y);
	vec3 p = ro + rd*beg,
		 add = rd * ((end - beg) / 44.0);
	p.y = 0.;
	vec2 shade, shadeSum;

	for (int i = 0; i < 44; i++) {
		if (shadeSum.y >= 1.0) break;
		shade.y = max(-MapClouds(p, ro), 0.0);
		shade.x = p.y / diff;
		shadeSum += shade * (1.0 - shadeSum.y);
		p += add;
	}
	shadeSum.x /= 10.0;
	shadeSum = min(shadeSum, 1.0);	
	vec3 sky = atmosphere(
			normalize(-eye),                // normalized ray direction
			vec3(0,6372e3,0),               // ray origin
			uSunPos,                        // position of the sun
			22.0,                           // intensity of the sun
			6371e3,                         // radius of the planet in meters
			6471e3,                         // radius of the atmosphere in meters
			vec3(5.5e-6, 13.0e-6, 22.4e-6), // Rayleigh scattering coefficient
			21e-6,                          // Mie scattering coefficient
			8e3,                            // Rayleigh scale height
			1.2e3,                          // Mie scale height
			0.758                           // Mie preferred scattering direction
		);
	// Apply exposure.
    sky = 1.0 - exp(-1.0 * sky);
	return mix(sky, min(vec3(pow(shadeSum.x, .4)), 1.0), shadeSum.y);
  }

void main()
{	
	vec3 color = vec3(0.0);
			
	vec3 dir = normalize(vec3(gl_FragCoord.xy - resolution * 0.5, 
                                  -resolution.y / tan(PI * 0.167)));
	vec3 f = normalize(-eye); 
	vec3 s = normalize(cross(f, vec3(.0,.1,.0)));
	vec3 worldDir = mat3(s,cross(s, f),-f) * dir;
	
    if (worldDir.y > 0.) 
         color = CreateClouds(eye, worldDir, 3000., 4000.);
		 
    gl_FragColor = vec4(color, 1);
}



