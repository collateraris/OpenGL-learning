#version 430 core
#define PI 3.141592

layout (local_size_x = 4, local_size_y = 4) in;

layout (rgba32f, binding = 0) uniform image2D outTexture;

layout (rgba8, binding = 1) uniform image3D uLowFreqNoiseTex;
layout (rgba8, binding = 2) uniform image3D uHighFreqNoiseTex;
layout (rgba8, binding = 3) uniform image2D uWeatherMapTex;

struct Params
{
    float time;
    vec3 sunDir;
    vec3 sunColor;
};

uniform Params uParams;

struct Properties
{
    float density;
    float coverage;
    float attenuationT;
    float attenuationS;
    float sunIntensity;   
};

uniform Properties uProperties;

uniform vec3 uViewDir;
uniform vec2 uResolution;

struct CloudsInfo
{
    float cloudsMin;// = 6415.;
    float cloudsMax;// = 6435.;
    vec3 rayOrigin;// = vec3(0., 6400, 0.);
    vec3 sphereOrigin;// = vec3(0.);
    float lowerAtmosphereLayer;// = 6415.0;
};

vec4 mainMarching(in vec3 viewDir, in CloudsInfo cloudsInfo);

float cloudSampleDirectDensity(in vec3 position, in CloudsInfo cloudsInfo);

float cloudSampleDensity(in vec3 position, in CloudsInfo cloudsInfo);

float cloudGetHeight(in vec3 position, in vec3 sphereOrigin, float cloudsMin, float cloudsMax);

float remap(float value, float minValue, float maxValue, float newMinValue, float newMaxValue);

bool crossRaySphereOutFar(in vec3 ro, in vec3 rd, in vec3 so, float sr, inout vec3 position);

vec3 modulo3(in vec3 a, in vec3 b);

vec2 modulo2(in vec2 a, in vec2 b);

vec3 atmosphere(vec3 rayDir, vec3 rayOrigin, vec3 sunPos, float sunIntensity);

vec2 rsi(vec3 r0, vec3 rd, float sr); 

void main()
{
    CloudsInfo cloudsInfo;
    cloudsInfo.cloudsMin = 6415.;
    cloudsInfo.cloudsMax = 6435.;
    cloudsInfo.rayOrigin = vec3(0., 6400, 0.);
    cloudsInfo.sphereOrigin = vec3(0.);
    cloudsInfo.lowerAtmosphereLayer = 6415.0;

    vec4 color = vec4(0.);

    ivec2 pixel = ivec2(gl_GlobalInvocationID.xy);
	vec3 f = normalize(-uViewDir); 
    color += mainMarching(f, cloudsInfo);
    color += atmosphere(f, cloudsInfo.rayOrigin, uParams.sunDir, uProperties.sunIntensity);

    imageStore(outTexture, pixel, color);
}

vec4 mainMarching(in vec3 viewDir, in CloudsInfo cloudsInfo)
{
    vec3 position;
    
    if (!crossRaySphereOutFar(cloudsInfo.rayOrigin, viewDir, cloudsInfo.sphereOrigin, cloudsInfo.lowerAtmosphereLayer, position))
        return vec4(1., 0., 0., 1.);//vec4(0.);

    float avrStep = (cloudsInfo.cloudsMax - cloudsInfo.cloudsMin) / 64.;

    vec3 color = vec3(0.);
    float transmittance = 1.;

    int mmSamples = 128;
    for(int i = 0; i < mmSamples; ++i)
    {
        float density = cloudSampleDensity(position, cloudsInfo) * avrStep;
        if(density > 0.)
        {
            float sunDensity = cloudSampleDirectDensity(position, cloudsInfo);

            float m2 = exp(-uProperties.attenuationT * sunDensity);
            float m3 = uProperties.attenuationS * density;
            float light = uProperties.sunIntensity * m2 * m3;
            color += uParams.sunColor * light * transmittance;
            transmittance *= exp(-uProperties.attenuationT * density);
            //return vec4(0., 1., 0., 1.);
        }

        position += viewDir * avrStep;

		if(transmittance < 0.05 || length(position) > 6435.0)
			break;
    }

    return vec4(color, 1. - transmittance);
}

float cloudSampleDirectDensity(in vec3 position, in CloudsInfo cloudsInfo)
{
    float avrStep = (cloudsInfo.cloudsMax - cloudsInfo.cloudsMin) * 0.01;
    float sunDensity = 0.;

    int ddSamples = 4;
    for(int i = 0; i < ddSamples; ++i)
    {
        float step = avrStep;

        if (i == (ddSamples - 1))
            step *= 6.0;
        
        position += uParams.sunDir * step;

        float density = cloudSampleDensity(position, cloudsInfo) * step;
        sunDensity += density;
    }
    return sunDensity;
}

float cloudSampleDensity(in vec3 position, in CloudsInfo cloudsInfo)
{
    position.xz += vec2(0.2) * uParams.time;

    vec4 weather = imageLoad(uWeatherMapTex, ivec2(modulo2(position.xz, vec2(128))));

    float height = cloudGetHeight(position, cloudsInfo.sphereOrigin, cloudsInfo.cloudsMin, cloudsInfo.cloudsMax);

    float SRb = clamp(remap(height, 0, 0.07, 0, 1), 0., 1.);
    float SRt = clamp(remap(height, weather.b * 0.2, weather.b, 0, 1), 0., 1.);
    float SA = SRb * SRt;

    float DRb = height * clamp(remap(height, 0, 0.15, 0, 1), 0., 1.);
    float DRt = height * clamp(remap(height, 0.9, 1, 0, 1), 0., 1.);
    float DA = DRb * DRt * weather.a * 2. * uProperties.density;

    float SNsample = imageLoad(uLowFreqNoiseTex, ivec3(modulo3(position / 48., vec3(32)))).x * 0.85 
        + imageLoad(uHighFreqNoiseTex, ivec3(modulo3(position / 4.8, vec3(128)))).x * 0.15;

    float WMc = max(weather.r, clamp(uProperties.coverage - 0.5, 0, 1) * weather.g * 2.);
    float d = clamp(remap(SNsample * SA, 1 - uProperties.coverage * WMc, 1, 0, 1), 0.1, 1.) * DA;  
    return d;
}

float cloudGetHeight(in vec3 position, in vec3 sphereOrigin, float cloudsMin, float cloudsMax)
{
    float heightFraction = (distance(position, sphereOrigin) - cloudsMin) / (cloudsMax - cloudsMin);

    return clamp(heightFraction, 0., 1.);
}

float remap(float value, float minValue, float maxValue, float newMinValue, float newMaxValue)
{
    return newMinValue + (value - minValue)/(maxValue - minValue) * (newMaxValue - newMinValue);
}

bool crossRaySphereOutFar(in vec3 ro, in vec3 rd, in vec3 so, float sr, inout vec3 position)
{
    vec3 L = so - ro;
    L = normalize(L);
    float tca = dot(L, rd);
    if (tca < 0.) return false;

    float sqD = dot(L, L) - tca * tca;
    float sqRadius = sr * sr;

    if (sqD > sqRadius) return false;

    float thc = sqrt(sqRadius - sqD);
    float t0 = tca - thc; 
    float t1 = tca + thc;

    if (t1 < t0)
    {
        float tmp_t = t0;
        t0 = t1;
        t1 = tmp_t;
    }

    if (t0 < 0) 
    {
        t0 = t1;
        if (t0 < 0) return false;
    }

    position = ro + t0 * rd;
    return true; 
}

vec3 modulo3(in vec3 a, in vec3 b)
{
    vec3 posDivident = mod(a, b) + b;
    return mod(posDivident, b);
}

vec2 modulo2(in vec2 a, in vec2 b)
{
    vec2 posDivident = mod(a, b) + b;
    return mod(posDivident, b);
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
	float pRlh = 3.0 / (16.0 * PI) * (1.0 + mumu);
	float pMie = 3.0 / (8.0 * PI) * ((1.0 - gg) * (mumu + 1.0)) / (pow(1.0 + gg - 2.0 * mu * gMie, 1.5) * (2.0 + gg));
	
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

