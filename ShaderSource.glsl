 
#version 450 core
#extension GL_EXT_control_flow_attributes : enable
layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

struct PixelOutput {
    float r, g, b, d, l;
};

layout(std430, binding = 0) buffer OutputBuffer {
    PixelOutput pixels[];
};

layout(push_constant) uniform PushConstants {
    float camPX; float camPY; float camPZ;
    float time;
    float camFX; float camFY; float camFZ;
    float width;
    float camUX; float camUY; float camUZ;
    float height;
    float camRX; float camRY; float camRZ;
    float maxDistance;
    float surfTolerance;
    float viewportX;
    float viewportY;
    float pad2;
} push;
    
const float HALFPI = 1.570796;
const float PI = 3.1415923;
const float TAU = 6.283185;
const float ROOT2 = 1.414213;
const float FLOAT_MAX = 3.402823466e38;
const float NORM_CHECK_OFFSET = 0.001;
const float AMB_LIGHT_POW = 0.15;
const float SHADOW_TOLERANCE = 0.1;

// UTILITY FUNCTIONS //
vec3 rotateVec3(vec3 p, vec3 axis, float rotations) {
    vec3 a = normalize(axis);
    float angle = rotations * TAU;
    return p * cos(angle) + cross(a, p) * sin(angle) + a * dot(a, p) * (1.0 - cos(angle));
}

vec2 rotateVec2(vec2 p, float rotations) {
    float c = cos(rotations * TAU);
    float s = sin(rotations * TAU);
    mat2 m = mat2(c, -s, s, c);
    return m * p;
}

vec3 blendColors(float alpha, vec3 colorA, vec3 colorB, float colorBlending)
{
	alpha *= colorBlending;
	alpha -= 0.5 * (colorBlending - 1.0);
	alpha = clamp(alpha, 0.0, 1.0);
	return mix(colorA, colorB, alpha);
}

// OPERATOR FUNCTIONS //
float oscillateSine(float initVal, float period, float amplitude, float phase, float time) {
    return initVal + sin(TAU * (time + (phase * period)) / period) * amplitude;
}

float oscillateLinear(float initVal, float period, float amplitude, float phase, float time) {
    float progress = mod(0.25 * period + time + phase * period, period) / period;
	progress -= 0.5;
	progress *= 2.0;
	progress = abs(progress);
	progress -= 0.5;
	progress *= 2.0;
    return initVal + progress * amplitude;
}

vec3 oscillateSineV3(vec3 initVal, vec3 period, vec3 amplitude, vec3 phase, float time) {
    return initVal + sin(TAU * (vec3(time) + (phase * period)) / period) * amplitude;
}

vec3 oscillateLinearV3(vec3 initVal, vec3 period, vec3 amplitude, vec3 phase, float time) {
    vec3 progress = vec3(
        mod(0.25 * period.x + time + phase.x * period.x, period.x) / period.x,
        mod(0.25 * period.y + time + phase.y * period.y, period.y) / period.y,
        mod(0.25 * period.z + time + phase.z * period.z, period.z) / period.z
	);
	progress -= 0.5;
	progress *= 2.0;
	progress = abs(progress);
	progress -= 0.5;
	progress *= 2.0;

	return initVal + progress * amplitude;
}

vec3 translateLinear(vec3 vector, vec2 transitTimes, vec2 endDelays, float phase, float time) {
	float progress = mod(time + phase + transitTimes.x * 0.5, transitTimes.x + transitTimes.y + endDelays.x + endDelays.y);
	vec3 start = vector * -0.5;
	if (progress <= transitTimes.x) {
		progress /= transitTimes.x;
		return mix(start, -start, progress);
	}
	float timeFrame = transitTimes.x + endDelays.x;
	if (progress <= timeFrame) {
		return -start;
	}
	if (progress <= (timeFrame + transitTimes.y)) {
		progress = (progress - transitTimes.x - endDelays.x) / transitTimes.y;
		return mix(-start, start, progress);
	}
	return start;
}

vec3 rotateScan(vec3 p, vec3 axis, vec2 transitTimes, vec2 endDelays, float rotations, float phase, float time) {
    float progress = mod(time + phase, transitTimes.x + transitTimes.y + endDelays.x + endDelays.y);
	if (progress <= transitTimes.x) {
		progress /= transitTimes.x;
        return rotateVec3(p, axis, rotations * progress);
	}
	float timeFrame = transitTimes.x + endDelays.x;
	if (progress <= timeFrame) {
        return rotateVec3(p, axis, rotations);
	}
	timeFrame += transitTimes.y;
	if (progress <= timeFrame) {
		progress = (progress - (transitTimes.x + endDelays.x)) / transitTimes.y;
        return rotateVec3(p, axis, rotations * (1.0 - progress));
	}
	return p;
}

vec3 rotateDelay(vec3 p, vec3 axis, float rotations, float rotateTime, float delayTime, float phase, float time) {
    float cycleTime = rotateTime + delayTime;
	float completeCycles = floor((time + phase) / cycleTime);
	float progress = mod(time + phase, cycleTime);
	completeCycles += min(rotateTime, progress) / rotateTime;
    return rotateVec3(p, axis, completeCycles * rotations);
}

// OBJECT FUNCTIONS //
float box(vec3 p, vec3 dimensions) {
    p = abs(p) - dimensions * 0.5;
    return length(max(p,0.0)) + min(max(p.x,max(p.y,p.z)),0.0);
}

float boxFrame(vec3 p, vec3 dimensions, float edgeThickness) {
    p = abs(p)-dimensions;
    vec3 q = abs(p+edgeThickness)-edgeThickness;
    return min(min(
        length(max(vec3(p.x,q.y,q.z),0.0))+min(max(p.x,max(q.y,q.z)),0.0),
        length(max(vec3(q.x,p.y,q.z),0.0))+min(max(q.x,max(p.y,q.z)),0.0)),
        length(max(vec3(q.x,q.y,p.z),0.0))+min(max(q.x,max(q.y,p.z)),0.0));
}

float sphere(vec3 p, float radius) {
    return length(p) - radius;
}

float torus(vec3 p, vec2 radii) {
    return length(vec2(length(vec2(p.x, p.z)) - radii.x, p.y)) - radii.y;
}

float capsuleX(vec3 p, float bodyLength, float radius) {
    p.x = p.x > 0.0 ? max(0.0, p.x - 0.5 * bodyLength) : min(0.0, p.x + 0.5 * bodyLength);
    return length(p) - radius;
}

float capsuleY(vec3 p, float bodyLength, float radius) {
    p.y = p.y > 0.0 ? max(0.0, p.y - 0.5 * bodyLength) : min(0.0, p.y + 0.5 * bodyLength);
    return length(p) - radius;
}

float capsuleZ(vec3 p, float bodyLength, float radius) {
    p.z = p.z > 0.0 ? max(0.0, p.z - 0.5 * bodyLength) : min(0.0, p.z + 0.5 * bodyLength);
    return length(p) - radius;
}

float crystal(vec3 p, float edgeLength) {
    p = abs(p);
    return (p.x + p.y + p.z - edgeLength) * 0.57735027;
}

float cylinderX(vec3 p, float bodyLength, float radius) {
    vec2 yz = vec2(p.y, p.z);
    vec2 d = abs(vec2(length(yz), p.x )) - vec2(radius, bodyLength);
    return min(max(d.x, d.y), 0.0) + length(max(d, 0.0));
}

float cylinderY(vec3 p, float bodyLength, float radius) {
    vec2 xz = vec2(p.x, p.z);
    vec2 d = abs(vec2(length(xz), p.y )) - vec2(radius, bodyLength);
    return min(max(d.x, d.y), 0.0) + length(max(d, 0.0));
}

float cylinderZ(vec3 p, float bodyLength, float radius) {
    vec2 xy = vec2(p.x, p.y);
    vec2 d = abs(vec2(length(xy), p.z )) - vec2(radius, bodyLength);
    return min(max(d.x, d.y), 0.0) + length(max(d, 0.0));
}

float chainLink(vec3 p, float sideLength, vec2 radii) {
    p.x = p.x > 0.0 ? max(0.0, p.x - sideLength * 0.5) : min(0.0, p.x + sideLength * 0.5);
    return length(vec2(length(vec2(p.x, p.z)) - radii.x, p.y)) - radii.y;
}

float tube(vec3 p, float height, vec2 radii) {
    p.y = p.y > 0.0 ? max(0.0, p.y - height * 0.5) : min(0.0, p.y + height * 0.5);
    return length(vec2(length(vec2(p.x, p.z)) - radii.x, p.y)) - radii.y;
}

float cone(vec3 p, float height, float angle) {
    float c = cos(angle);
    float s = sin(angle);
    vec2 q = height * vec2(s / c, -1.0);
    vec2 w = vec2( length(p.xz), p.y );
    vec2 a = w - q*clamp( dot(w,q)/dot(q,q), 0.0, 1.0 );
    vec2 b = w - q*vec2( clamp( w.x/q.x, 0.0, 1.0 ), 1.0 );
    float k = sign( q.y );
    float d = min(dot( a, a ),dot(b, b));
    float t = max( k*(w.x*q.y-w.y*q.x),k*(w.y-q.y)  );
    return sqrt(d)*sign(t);
}

void sampleScene(in vec3 p0, in float maxDist, out float d0, out vec3 color0, in float time) {
	d0 = maxDist;
	float blend;
	color0 = vec3(0.0);

	float pmFloat0, pmFloat1, pmFloat2;
	vec2 pmVec20, pmVec21, pmVec22;
	vec3 pmVec30, pmVec31, pmVec32;

	// SDFObject //
	color0 = vec3(1.0000000, 1.0000000, 1.0000000);
	p0 -= vec3(0.0000000, 0.0000000, 0.0000000);

	// SDFObject //
	vec3 p1;
	float d1 = FLOAT_MAX;
	vec3 color1;
	p1 = p0;
	d1 = FLOAT_MAX;
	color1 = vec3(0.0000000, 0.0000000, 1.0000000);
	p1 -= vec3(0.0000000, 0.0000000, 0.0000000);
	// SDFPlaneXY //
	d1 = abs(dot(p1, vec3(0.7071068, 0.7071068, 0.0000000)) + 2);


	// COMBAdd //
	if (d1 < d0) {
		d0 = d1;
		color0 = color1; 
	}

	// OPMirrorXZ //
	p0.xz = abs(p0.xz);

	// OPRotate //
	pmVec30 = vec3(0.0000000, 1.0000000, 0.0000000);
	pmFloat0 = -0.125000;
	p0 = rotateVec3(p0, pmVec30, pmFloat0);

	// OPMirrorX //
	p0.x = abs(p0.x);

	// OPRotate //
	pmVec30 = vec3(0.0000000, 1.0000000, 0.0000000);
	pmFloat0 = -0.062500;
	p0 = rotateVec3(p0, pmVec30, pmFloat0);

	// OPMirrorX //
	p0.x = abs(p0.x);

	// OPRotate //
	pmVec30 = vec3(0.0000000, 1.0000000, 0.0000000);
	pmFloat0 = -0.031250;
	p0 = rotateVec3(p0, pmVec30, pmFloat0);

	// OPTranslate //
	p0 -= vec3(0.0000000, 0.0000000, 3.0000000); 

	// SDFObject //
	p1 = p0;
	d1 = FLOAT_MAX;
	color1 = vec3(0.0000000, 1.0000000, 0.0000000);
	p1 -= vec3(0.0000000, 0.0000000, 0.0000000);
	// SDFCone //
	d1 = cone(p1, 2, 0.15708);


	// COMBAdd //
	if (d1 < d0) {
		d0 = d1;
		color0 = color1; 
	}

}


void main() {
    uint coordX = gl_GlobalInvocationID.x;
    uint coordY = gl_GlobalInvocationID.y;
    
    // 2. Strict bounds check (Never remove this!)
    if (coordX >= uint(push.width) || coordY >= uint(push.height)) return;

    vec2 viewportCoords = vec2(float(coordX) / push.width, float(coordY) / push.height);
    vec2 viewportSize = vec2(push.viewportX, push.viewportY);

    vec3 camPos = vec3(push.camPX, push.camPY, push.camPZ);
    vec3 camForward = vec3(push.camFX, push.camFY, push.camFZ);
    vec3 camRight = vec3(push.camRX, push.camRY, push.camRZ);
    vec3 camUp = vec3(push.camUX, push.camUY, push.camUZ);

    vec3 screenPoint = camPos + camForward;
    screenPoint -= camRight * 0.5 * viewportSize.x;
    screenPoint += camRight * viewportSize.x * viewportCoords.x;
    screenPoint += camUp * 0.5 * viewportSize.y;
    screenPoint -= camUp * viewportSize.y * viewportCoords.y;

    vec3 rayDir = normalize(screenPoint - camPos);

    float rayLength = 0.0;

    uint outIndex = (coordY * uint(push.width)) + coordX;
    pixels[outIndex].r = 0.0;
    pixels[outIndex].g = 0.0;
    pixels[outIndex].b = 0.0;
    pixels[outIndex].l = 0.0;
    pixels[outIndex].d = FLOAT_MAX;

    bool hit = false;
    vec3 p0;
    float d0;
    vec3 color;

	while (rayLength < push.maxDistance) {
        p0 = camPos + rayDir * rayLength;  // Marched sample point with inherited manipulation
        d0 = push.maxDistance;  // Blendable/selectable distance values
        color = vec3(0.0);

        sampleScene(p0, push.maxDistance, d0, color, push.time);

        if (d0 < push.surfTolerance) {
            pixels[outIndex].d = rayLength / push.maxDistance;
            hit = true;
            break;
        }
		rayLength += d0;
	}

    [[branch]] if (hit) {
        vec3 np = p0 - rayDir * push.surfTolerance;
        vec3 offsetX = vec3(NORM_CHECK_OFFSET, 0.0, 0.0);
        vec3 offsetY = vec3(0.0, NORM_CHECK_OFFSET, 0.0);
        vec3 offsetZ = vec3(0.0, 0.0, NORM_CHECK_OFFSET);

        float d1 = push.maxDistance;
        float d2 = push.maxDistance;
        vec3 colorSample;

        vec3 normal;
        sampleScene(np + offsetX, push.maxDistance, d1, colorSample, push.time);
        sampleScene(np - offsetX, push.maxDistance, d2, colorSample, push.time);
        normal.x = d1 - d2;
        d1 = push.maxDistance;
        d2 = push.maxDistance;
        sampleScene(np + offsetY, push.maxDistance, d1, colorSample, push.time);
        sampleScene(np - offsetY, push.maxDistance, d2, colorSample, push.time);
        normal.y = d1 - d2;
        d1 = push.maxDistance;
        d2 = push.maxDistance;
        sampleScene(np + offsetZ, push.maxDistance, d1, colorSample, push.time);
        sampleScene(np - offsetZ, push.maxDistance, d2, colorSample, push.time);
        normal.z = d1 - d2;

        normal = normalize(normal);

		// LIGHT SAMPLING //
		float lightDist;
		float d0;
		float rayLength;
		vec3 cumulativeColor = vec3(0.0);
		pixels[outIndex].r = 0.0;
		pixels[outIndex].g = 0.0;
		pixels[outIndex].b = 0.0;
		pixels[outIndex].l = 0.0;

		// SAMPLE LIGHT [0] //
		rayDir = vec3(5.0000000, 2.0000000, 0.0000000) - p0;
		lightDist = length(rayDir);
		rayDir = normalize(rayDir);
		rayLength = push.surfTolerance+0.001;
		d0 = push.maxDistance;
		while (true) {
			np = p0 + rayDir * SHADOW_TOLERANCE + rayLength * rayDir;

			sampleScene(np, lightDist, d0, colorSample, push.time);
			rayLength += d0;
			if (d0 < push.surfTolerance || rayLength >= lightDist) {
				float attenuation = 1.0 / (lightDist * lightDist);
				float directLight = (rayLength >= lightDist) ? attenuation * 150 : 0.0;
				float ambLight = 150 * AMB_LIGHT_POW * attenuation;
				float dot = max(0.0, dot(rayDir, normal));
				directLight *= dot;
				float lightPower = directLight + ambLight;
				cumulativeColor += vec3(1.0000000, 1.0000000, 1.0000000) * lightPower;
				break;
			}
		}

		cumulativeColor = clamp(cumulativeColor * color, 0.0, 1.0);
		cumulativeColor = mix(cumulativeColor, cumulativeColor, 0.2);
		pixels[outIndex].r = clamp(cumulativeColor.r, 0.0, 1.0);
		pixels[outIndex].g = clamp(cumulativeColor.g, 0.0, 1.0);
		pixels[outIndex].b = clamp(cumulativeColor.b, 0.0, 1.0);
		pixels[outIndex].l = min(1.0, length(cumulativeColor));
	}
}
