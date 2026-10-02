 
#version 450 core
#extension GL_EXT_control_flow_attributes : enable
layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

struct PixelOutput {
    float r, g, b, l;
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
const float ROOT3 = 1.732051;
const float FLOAT_MAX = 3.402823466e38;
const float NORM_CHECK_OFFSET = 0.001;
const float AMB_LIGHT_POW = 0.15;
const float SHADOW_TOLERANCE = 0.01;
const int AMB_OCC_SAMPLES = 10;
const float AMB_OCC_ATTEN = 0.75;
const float AMB_OCC_REACH = 0.2;

// UTILITY FUNCTIONS //
vec3 rotateVec3(vec3 p, vec3 axis, float rotations) {
    vec3 a = normalize(axis);
    float angle = rotations * TAU;
    float s = sin(angle);
    float c = cos(angle);
    return p * c + cross(a, p) * s + a * dot(a, p) * (1.0 - c);
}

vec2 rotateVec2(vec2 p, float rotations) {
    float c = cos(rotations * TAU);
    float s = sin(rotations * TAU);
    mat2 m = mat2(c, -s, s, c);
    return m * p;
}

float extrude2D(double pDotY, float d2D, float height) {
    vec2 w = vec2(d2D, abs(pDotY) - height * 0.5);
    return min(max(w.x, w.y), 0.0) + length(max(w, 0.0));
}

vec3 revolve2D(vec3 p, float radius)
{
    return vec3(length(vec2(p.x, p.y)) - radius, p.z, p.z);
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

float torusSegment(vec3 p, vec2 radii, vec2 sincos) {
    p.x = abs(p.x);
    float k = (sincos.y * p.x > sincos.x * p.y) ? dot(p.xy, sincos) : length(p.xy);
    return sqrt(dot(p, p) + radii.x * radii.x - 2.0 * radii.x * k) - radii.y;
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
    vec2 d = abs(vec2(length(yz), p.x )) - vec2(radius, bodyLength * 0.5);
    return min(max(d.x, d.y), 0.0) + length(max(d, 0.0));
}

float cylinderY(vec3 p, float bodyLength, float radius) {
    vec2 xz = vec2(p.x, p.z);
    vec2 d = abs(vec2(length(xz), p.y )) - vec2(radius, bodyLength * 0.5);
    return min(max(d.x, d.y), 0.0) + length(max(d, 0.0));
}

float cylinderZ(vec3 p, float bodyLength, float radius) {
    vec2 xy = vec2(p.x, p.y);
    vec2 d = abs(vec2(length(xy), p.z )) - vec2(radius, bodyLength * 0.5);
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
    vec2 w = vec2(length(p.xz), p.y);
    vec2 a = w - q * clamp(dot(w, q) / dot(q, q), 0.0, 1.0);
    vec2 b = w - q * vec2(clamp(w.x / q.x, 0.0, 1.0), 1.0);
    float k = sign(q.y);
    float d = min(dot(a, a), dot(b, b));
    float t = max(k * (w.x * q.y - w.y * q.x), k * (w.y - q.y));
    return sqrt(d) * sign(t);
}

float solidAngle(vec3 p, vec2 c, float r) {
    vec2 q = vec2(length(p.xz), p.y);
    float l = length(q) - r;
    float m = length(q - c*clamp(dot(q, c), 0.0, r));
    return max(l, m * sign(c.y * q.x - c.x * q.y));
}

float ellipsoid(vec3 p, vec3 radii) {
    float k0 = length(p / radii);
    float k1 = length(p / (radii * radii));
    return k0 * (k0 - 1.0) / k1;
}

float rhombus(vec3 p, vec2 dimensions, int operation, float magnitude) // if not extrude, then revolve 
{
    if (operation == 1) p = revolve2D(p, magnitude);
    vec2 dim2D = vec2(dimensions.x, -dimensions.y);
    vec2 p2D = vec2(p.x, p.z);
    p2D = abs(p2D);
    float h = clamp((dot(dim2D, p2D) + dim2D.y * dim2D.y) / dot(dim2D, dim2D), 0.0, 1.0 );
    p2D -= dim2D * vec2(h, h-1.0);
    float d2D = length(p2D) * sign(p2D.x);
    return operation == 0 ? extrude2D(p.y, d2D, magnitude) : d2D;
}

float equilateralTriangle(vec3 p, float base, int operation, float magnitude) {
    if (operation == 1) p = revolve2D(p, magnitude);
    float halfBase = base * 0.5;
    vec2 p2D = vec2(p.x, p.z - base * ROOT3 / 6.0 + halfBase);
    p2D.x = abs(p2D.x);
    p2D -= vec2(0.5, 0.5 * ROOT3) * max(p2D.x + ROOT3 * p2D.y, 0.0);
    p2D -= vec2(clamp(p2D.x, -halfBase, halfBase), -halfBase / ROOT3);
    float d2D = length(p2D) * sign(-p2D.y);
    return operation == 0 ? extrude2D(p.y, d2D, magnitude * 0.5) : d2D;
}

float isoscelesTriangle(vec3 p, vec2 dimensions, int operation, float magnitude) {
    if (operation == 1) p = revolve2D(p, magnitude);
    vec2 p2D = vec2(abs(p.x), p.z);
    vec2 a = p2D - dimensions * clamp(dot(p2D, dimensions) / dot(dimensions, dimensions), 0.0, 1.0 );
    vec2 b = p2D - dimensions * vec2(clamp(p2D.x / dimensions.x, 0.0, 1.0 ), 1.0 );
    float s = -sign(dimensions.y);
    vec2 d = min(vec2(dot(a, a), s * (p2D.x * dimensions.y - p2D.y * dimensions.x)), vec2(dot(b, b), s * (p2D.y - dimensions.y)));
    float d2D = -sqrt(d.x) * sign(d.y);
    return operation == 0 ? extrude2D(p.y, d2D, magnitude * 0.5) : d2D;
}

float pieSlice(vec3 p, vec2 sincos, float radius, int operation, float magnitude) {
    if (operation == 1) p = revolve2D(p, magnitude);
    vec2 p2D = vec2(p.x, p.z);
    p2D.x = abs(p2D.x);
    float l = length(p2D) - radius;
    float m = length(p2D - sincos * clamp(dot(p2D, sincos), 0.0, radius));
    float d2D = max(l, m * sign(sincos.y * p2D.x - sincos.x * p2D.y));
    return operation == 0 ? extrude2D(p.y, d2D, magnitude * 0.5) : d2D;
}

float pentagon(vec3 p, float radius, int operation, float magnitude) {
    if (operation == 1) p = revolve2D(p, magnitude);
    const vec3 k = vec3(0.809016994, 0.587785252, 0.726542528);
    vec2 p2D = vec2(abs(p.x), p.z);
    p2D -= 2.0 * min(dot(vec2(-k.x, k.y), p2D), 0.0) * vec2(-k.x, k.y);
    p2D -= 2.0 * min(dot(vec2( k.x, k.y), p2D), 0.0) * vec2( k.x, k.y);
    p2D -= vec2(clamp(p2D.x, -radius * k.z, radius * k.z), radius);    
    float d2D = length(p2D) * sign(p2D.y);
    return operation == 0 ? extrude2D(p.y, d2D, magnitude * 0.5) : d2D;
}

float hexagon(vec3 p, float radius, int operation, float magnitude) {
    if (operation == 1) p = revolve2D(p, magnitude);
    const vec3 k = vec3(-0.866025404,0.5,0.577350269);
    vec2 p2D = vec2(abs(p.x), abs(p.z));
    p2D -= 2.0 * min(dot(k.xy, p2D), 0.0) * k.xy;
    p2D -= vec2(clamp(p2D.x, -k.z * radius, k.z * radius), radius);
    float d2D = length(p2D) * sign(p2D.y);
    return operation == 0 ? extrude2D(p.y, d2D, magnitude * 0.5) : d2D;
}

float octagon(vec3 p, float radius, int operation, float magnitude) {
    if (operation == 1) p = revolve2D(p, magnitude);
    const vec3 k = vec3(-0.9238795325, 0.3826834323, 0.4142135623);
    vec2 p2D = vec2(abs(p.x), abs(p.z));
    p2D -= 2.0 * min(dot(vec2(k.x, k.y), p2D), 0.0) * vec2(k.x, k.y);
    p2D -= 2.0 * min(dot(vec2(-k.x, k.y), p2D), 0.0) * vec2(-k.x, k.y);
    p2D -= vec2(clamp(p2D.x, -k.z * radius, k.z * radius), radius);
    float d2D = length(p2D) * sign(p2D.y);
    return operation == 0 ? extrude2D(p.y, d2D, magnitude * 0.5) : d2D;
}

float star(vec3 p, float radius, int operation, float magnitude, float an, vec2 acs, vec2 ecs) {
    if (operation == 1) p = revolve2D(p, magnitude);
    vec2 p2D = vec2(abs(p.x), p.z);
    float bn = mod(atan(p2D.x, p2D.y), 2.0 * an) - an;
    float sinbn = sin(bn);
    float cosbn = cos(bn);
    p2D = length(p2D) * vec2(cosbn, abs(sinbn));
    p2D -= radius * acs;
    p2D += ecs * clamp(-dot(p2D, ecs), 0.0, radius * acs.y / ecs.y);
    float d2D = length(p2D) * sign(p2D.x);
    return operation == 0 ? extrude2D(p.y, d2D, magnitude * 0.5) : d2D;
}

float triangle(vec3 p, vec2 point1, vec2 point2, vec2 point3, int operation, float magnitude) {
    if (operation == 1) p = revolve2D(p, magnitude);
    vec2 p2D = vec2(p.x, p.z);
    vec2 e0 = point2 - point1, e1 = point3 - point2, e2 = point1 - point3;
    vec2 v0 = p2D - point1, v1 = p2D - point2, v2 = p2D - point3;
    vec2 pq0 = v0 - e0 * clamp(dot(v0, e0) / dot(e0, e0), 0.0, 1.0 );
    vec2 pq1 = v1 - e1 * clamp(dot(v1, e1) / dot(e1, e1), 0.0, 1.0 );
    vec2 pq2 = v2 - e2 * clamp(dot(v2, e2) / dot(e2, e2), 0.0, 1.0 );
    float s = sign(e0.x * e2.y - e0.y * e2.x);
    vec2 d = min(min(vec2(dot(pq0, pq0), s * (v0.x * e0.y - v0.y * e0.x)),
                     vec2(dot(pq1, pq1), s * (v1.x * e1.y - v1.y * e1.x))),
                     vec2(dot(pq2, pq2), s * (v2.x * e2.y - v2.y * e2.x)));
    float d2D = -sqrt(d.x) * sign(d.y);
    return operation == 0 ? extrude2D(p.y, d2D, magnitude * 0.5) : d2D;    
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

	// SDFObject //
	vec3 p1;
	float d1 = FLOAT_MAX;
	vec3 color1;
	p1 = p0;
	d1 = FLOAT_MAX;
	color1 = vec3(1.0000000, 1.0000000, 1.0000000);

	// SDFObject //
	vec3 p2;
	float d2 = FLOAT_MAX;
	vec3 color2;
	p2 = p1;
	d2 = FLOAT_MAX;
	color2 = vec3(0.9176471, 0.7490196, 0.0156863);
	// SDFCylinderY //
	d2 = cylinderY(p2, 1.2, 0.46);


	// OPRoundOff //
	pmFloat0 = 0.050000;
	d2 -= pmFloat0;

	// OPMirrorXZ //
	p2.xz = abs(p2.xz);

	// OPRotate //
	pmVec30 = vec3(0.0000000, 1.0000000, 0.0000000);
	pmFloat0 = -0.125000;
	p2 = rotateVec3(p2, pmVec30, pmFloat0);

	// OPMirrorX //
	p2.x = abs(p2.x);

	// OPRotate //
	pmVec30 = vec3(0.0000000, 1.0000000, 0.0000000);
	pmFloat0 = -0.062500;
	p2 = rotateVec3(p2, pmVec30, pmFloat0);

	// OPMirrorX //
	p2.x = abs(p2.x);

	// OPRotate //
	pmVec30 = vec3(0.0000000, 1.0000000, 0.0000000);
	pmFloat0 = -0.031250;
	p2 = rotateVec3(p2, pmVec30, pmFloat0);

	// OPMirrorX //
	p2.x = abs(p2.x);

	// OPRotate //
	pmVec30 = vec3(0.0000000, 1.0000000, 0.0000000);
	pmFloat0 = -0.015625;
	p2 = rotateVec3(p2, pmVec30, pmFloat0);

	// OPTranslate //
	p2 -= vec3(0.0000000, 0.0000000, 0.5100000); 

	// OPTranslate //
	p2 -= vec3(0.0000000, 0.7500000, 0.0000000); 

	// OPRotate //
	pmVec30 = vec3(1.0000000, 0.0000000, 0.0000000);
	pmFloat0 = -0.060000;
	p2 = rotateVec3(p2, pmVec30, pmFloat0);

	// SDFObject //
	vec3 p3;
	float d3 = FLOAT_MAX;
	vec3 color3;
	p3 = p2;
	d3 = FLOAT_MAX;
	color3 = vec3(0.9176471, 0.7490196, 0.0156863);
	// SDFCone //
	d3 = cone(p3, 0.155, 0.282743);


	// OPRoundOff //
	pmFloat0 = 0.010000;
	d3 -= pmFloat0;

	// COMBAddSmooth //
	pmFloat0 = 0.010000;
	pmFloat1 = 0.000000;
	pmFloat2 = 1.000000;
	blend = clamp(0.5 + 0.5 * (d3 - d2) / pmFloat0, 0.0, 1.0);
	color2 = blendColors(clamp(1.0 - blend + pmFloat1, 0.0, 1.0), color2, color3, pmFloat2);
	d2 = mix(d3, d2, blend) - pmFloat0 * blend * (1.0 - blend);

	// COMBAdd //
	if (d2 < d1) {
		d1 = d2;
		color1 = color2; 
	}

	// OPTranslate //
	p1 -= vec3(0.0000000, -0.6000000, -0.1200000); 

	// SDFObject //
	p2 = p1;
	d2 = FLOAT_MAX;
	color2 = vec3(0.9176471, 0.7490196, 0.0156863);
	// SDFCylinderY //
	d2 = cylinderY(p2, 0, 0.35);


	// OPRoundOff //
	pmFloat0 = 0.050000;
	d2 -= pmFloat0;

	// COMBAddSmooth //
	pmFloat0 = 0.150000;
	pmFloat1 = 0.000000;
	pmFloat2 = 1.000000;
	blend = clamp(0.5 + 0.5 * (d2 - d1) / pmFloat0, 0.0, 1.0);
	color1 = blendColors(clamp(1.0 - blend + pmFloat1, 0.0, 1.0), color1, color2, pmFloat2);
	d1 = mix(d2, d1, blend) - pmFloat0 * blend * (1.0 - blend);

	// OPTranslate //
	p1 -= vec3(0.0000000, -0.0750000, 0.2000000); 

	// SDFObject //
	p2 = p1;
	d2 = FLOAT_MAX;
	color2 = vec3(0.9176471, 0.7490196, 0.0156863);
	// SDFCylinderY //
	d2 = cylinderY(p2, 0.05, 0.18);


	// OPRoundOff //
	pmFloat0 = 0.100000;
	d2 -= pmFloat0;

	// COMBAdd //
	if (d2 < d1) {
		d1 = d2;
		color1 = color2; 
	}

	// SDFObject //
	p2 = p1;
	d2 = FLOAT_MAX;
	color2 = vec3(1.0000000, 1.0000000, 1.0000000);

	// OPTranslate //
	p2 -= vec3(0.0000000, -0.1500000, 0.2000000); 

	// OPRotate //
	pmVec30 = vec3(1.0000000, 0.0000000, 0.0000000);
	pmFloat0 = 0.040000;
	p2 = rotateVec3(p2, pmVec30, pmFloat0);

	// SDFObject //
	p3 = p2;
	d3 = FLOAT_MAX;
	color3 = vec3(0.9176471, 0.7490196, 0.0156863);
	// SDFCylinderY //
	d3 = cylinderY(p3, 0.5, 0.25);


	// COMBAdd //
	if (d3 < d2) {
		d2 = d3;
		color2 = color3; 
	}

	// COMBAddSmooth //
	pmFloat0 = 0.150000;
	pmFloat1 = 0.000000;
	pmFloat2 = 1.000000;
	blend = clamp(0.5 + 0.5 * (d2 - d1) / pmFloat0, 0.0, 1.0);
	color1 = blendColors(clamp(1.0 - blend + pmFloat1, 0.0, 1.0), color1, color2, pmFloat2);
	d1 = mix(d2, d1, blend) - pmFloat0 * blend * (1.0 - blend);

	// OPTranslate //
	p1 -= vec3(0.0000000, 0.4000000, -0.6000000); 

	// SDFObject //
	p2 = p1;
	d2 = FLOAT_MAX;
	color2 = vec3(0.9176471, 0.7490196, 0.0156863);
	// SDFCapsuleZ //
	d2 = capsuleZ(p2, 0.2, 0.1);


	// COMBAddSmooth //
	pmFloat0 = 0.050000;
	pmFloat1 = 0.000000;
	pmFloat2 = 1.000000;
	blend = clamp(0.5 + 0.5 * (d2 - d1) / pmFloat0, 0.0, 1.0);
	color1 = blendColors(clamp(1.0 - blend + pmFloat1, 0.0, 1.0), color1, color2, pmFloat2);
	d1 = mix(d2, d1, blend) - pmFloat0 * blend * (1.0 - blend);

	// OPMirrorX //
	p1.x = abs(p1.x);

	// OPTranslate //
	p1 -= vec3(0.1800000, 0.3500000, 0.2000000); 

	// OPRotate //
	pmVec30 = vec3(0.8944272, 0.0000000, 0.4472136);
	pmFloat0 = 0.350000;
	p1 = rotateVec3(p1, pmVec30, pmFloat0);

	// SDFObject //
	p2 = p1;
	d2 = FLOAT_MAX;
	color2 = vec3(0.9176471, 0.7490196, 0.0156863);
	// SDFTorus //
	d2 = torus(p2, vec2(0.2000000, 0.0150000));


	// COMBAddSmooth //
	pmFloat0 = 0.100000;
	pmFloat1 = 0.000000;
	pmFloat2 = 1.000000;
	blend = clamp(0.5 + 0.5 * (d2 - d1) / pmFloat0, 0.0, 1.0);
	color1 = blendColors(clamp(1.0 - blend + pmFloat1, 0.0, 1.0), color1, color2, pmFloat2);
	d1 = mix(d2, d1, blend) - pmFloat0 * blend * (1.0 - blend);

	// COMBAdd //
	if (d1 < d0) {
		d0 = d1;
		color0 = color1; 
	}

	// SDFObject //
	p1 = p0;
	d1 = FLOAT_MAX;
	color1 = vec3(1.0000000, 1.0000000, 1.0000000);

	// OPMirrorX //
	p1.x = abs(p1.x);

	// OPTranslate //
	p1 -= vec3(0.2100000, -0.0500000, -0.4000000); 

	// SDFObject //
	p2 = p1;
	d2 = FLOAT_MAX;
	color2 = vec3(1.0000000, 1.0000000, 1.0000000);
	// SDFSphere //
	d2 = sphere(p2, 0.23);


	// OPTranslate //
	p2 -= vec3(0.0000000, -0.0500000, -0.1300000); 

	// SDFObject //
	p3 = p2;
	d3 = FLOAT_MAX;
	color3 = vec3(0.0000000, 0.0000000, 0.0000000);
	// SDFSphere //
	d3 = sphere(p3, 0.1);


	// COMBAdd //
	if (d3 < d2) {
		d2 = d3;
		color2 = color3; 
	}

	// COMBAdd //
	if (d2 < d1) {
		d1 = d2;
		color1 = color2; 
	}

	// COMBAdd //
	if (d1 < d0) {
		d0 = d1;
		color0 = color1; 
	}

	// SDFObject //
	p1 = p0;
	d1 = FLOAT_MAX;
	color1 = vec3(1.0000000, 1.0000000, 1.0000000);

	// OPMirrorX //
	p1.x = abs(p1.x);

	// OPTranslate //
	p1 -= vec3(0.5500000, -0.3500000, 0.0000000); 

	// OPRotate //
	pmVec30 = vec3(0.0000000, -1.0000000, 0.0000000);
	pmFloat0 = 0.120000;
	p1 = rotateVec3(p1, pmVec30, pmFloat0);

	// OPRotate //
	pmVec30 = vec3(0.0000000, 0.0000000, 1.0000000);
	pmFloat0 = 0.250000;
	p1 = rotateVec3(p1, pmVec30, pmFloat0);

	// SDFObject //
	p2 = p1;
	d2 = FLOAT_MAX;
	color2 = vec3(0.9176471, 0.7490196, 0.0156863);
	// SDFTorus //
	d2 = torus(p2, vec2(0.0750000, 0.0200000));


	// COMBAdd //
	if (d2 < d1) {
		d1 = d2;
		color1 = color2; 
	}

	// COMBAddSmooth //
	pmFloat0 = 0.050000;
	pmFloat1 = 0.000000;
	pmFloat2 = 1.000000;
	blend = clamp(0.5 + 0.5 * (d1 - d0) / pmFloat0, 0.0, 1.0);
	color0 = blendColors(clamp(1.0 - blend + pmFloat1, 0.0, 1.0), color0, color1, pmFloat2);
	d0 = mix(d1, d0, blend) - pmFloat0 * blend * (1.0 - blend);

	// SDFObject //
	p1 = p0;
	d1 = FLOAT_MAX;
	color1 = vec3(1.0000000, 1.0000000, 1.0000000);

	// OPTranslate //
	p1 -= vec3(0.0000000, -0.2000000, 0.3300000); 

	// SDFObject //
	p2 = p1;
	d2 = FLOAT_MAX;
	color2 = vec3(0.9450980, 0.3058824, 0.1568628);
	// SDFCone //
	d2 = cone(p2, 1.4, 0.345575);


	// OPTranslate //
	p2 -= vec3(0.0000000, -0.4000000, -0.1000000); 

	// SDFObject //
	p3 = p2;
	d3 = FLOAT_MAX;
	color3 = vec3(1.0000000, 1.0000000, 1.0000000);
	// SDFSphere //
	d3 = sphere(p3, 0.5);


	// COMBSubtract //
	if (d2 < -d3) d2 = -d3;

	// OPTranslate //
	p2 -= vec3(0.0000000, -1.4500000, 0.1000000); 

	// SDFObject //
	p3 = p2;
	d3 = FLOAT_MAX;
	color3 = vec3(0.9450980, 0.3058824, 0.1568628);
	// SDFSphere //
	d3 = sphere(p3, 0.7);


	// OPTranslate //
	p3 -= vec3(0.0000000, -0.5000000, 0.0000000); 

	// SDFObject //
	vec3 p4;
	float d4 = FLOAT_MAX;
	vec3 color4;
	p4 = p3;
	d4 = FLOAT_MAX;
	color4 = vec3(1.0000000, 1.0000000, 1.0000000);
	// SDFBox //
	d4 = box(p4, vec3(2.0000000, 1.0000000, 2.0000000));


	// COMBSubtract //
	if (d3 < -d4) d3 = -d4;

	// COMBAddSmooth //
	pmFloat0 = 0.020000;
	pmFloat1 = 0.000000;
	pmFloat2 = 1.000000;
	blend = clamp(0.5 + 0.5 * (d3 - d2) / pmFloat0, 0.0, 1.0);
	color2 = blendColors(clamp(1.0 - blend + pmFloat1, 0.0, 1.0), color2, color3, pmFloat2);
	d2 = mix(d3, d2, blend) - pmFloat0 * blend * (1.0 - blend);

	// COMBAdd //
	if (d2 < d1) {
		d1 = d2;
		color1 = color2; 
	}

	// SDFObject //
	p2 = p1;
	d2 = FLOAT_MAX;
	color2 = vec3(1.0000000, 1.0000000, 1.0000000);

	// OPMirrorX //
	p2.x = abs(p2.x);

	// OPTranslate //
	p2 -= vec3(0.5000000, -1.2500000, 0.1000000); 

	// OPRotate //
	pmVec30 = vec3(0.0000000, 0.0000000, -1.0000000);
	pmFloat0 = 0.050000;
	p2 = rotateVec3(p2, pmVec30, pmFloat0);

	// SDFObject //
	p3 = p2;
	d3 = FLOAT_MAX;
	color3 = vec3(0.9450980, 0.3058824, 0.1568628);
d3 = ellipsoid(p3, vec3(0.2500000, 0.4000000, 0.2500000));


	// OPTranslate //
	p3 -= vec3(0.0000000, -0.2500000, 0.0000000); 

	// OPOnion //
	pmFloat0 = 0.010000;
	d3 = abs(d3) - pmFloat0; 

	// SDFObject //
	p4 = p3;
	d4 = FLOAT_MAX;
	color4 = vec3(1.0000000, 1.0000000, 1.0000000);
	// SDFBox //
	d4 = box(p4, vec3(1.0000000, 0.4250000, 1.0000000));


	// COMBSubtract //
	if (d3 < -d4) d3 = -d4;

	// COMBAdd //
	if (d3 < d2) {
		d2 = d3;
		color2 = color3; 
	}

	// COMBAdd //
	if (d2 < d1) {
		d1 = d2;
		color1 = color2; 
	}

	// COMBAdd //
	if (d1 < d0) {
		d0 = d1;
		color0 = color1; 
	}

	// SDFObject //
	p1 = p0;
	d1 = FLOAT_MAX;
	color1 = vec3(1.0000000, 1.0000000, 1.0000000);

	// OPTranslate //
	p1 -= vec3(0.0000000, -1.9500000, 0.3500000); 

	// SDFObject //
	p2 = p1;
	d2 = FLOAT_MAX;
	color2 = vec3(0.0000000, 0.6156863, 0.8627451);
	// SDFSphere //
	d2 = sphere(p2, 0.68);


	// OPTranslate //
	p2 -= vec3(0.0000000, 0.9000000, 0.0000000); 

	// SDFObject //
	p3 = p2;
	d3 = FLOAT_MAX;
	color3 = vec3(1.0000000, 1.0000000, 1.0000000);
	// SDFBox //
	d3 = box(p3, vec3(2.0000000, 2.0000000, 2.0000000));


	// COMBSubtract //
	if (d2 < -d3) d2 = -d3;

	// OPMirrorX //
	p2.x = abs(p2.x);

	// OPTranslate //
	p2 -= vec3(0.2800000, -1.4000000, 0.0000000); 

	// SDFObject //
	p3 = p2;
	d3 = FLOAT_MAX;
	color3 = vec3(0.0000000, 0.6156863, 0.8627451);
	// SDFCylinderY //
	d3 = cylinderY(p3, 0.8, 0.3);


	// OPTranslate //
	p3 -= vec3(0.0000000, -0.5500000, 0.0000000); 

	// OPOnion //
	pmFloat0 = 0.010000;
	d3 = abs(d3) - pmFloat0; 

	// SDFObject //
	p4 = p3;
	d4 = FLOAT_MAX;
	color4 = vec3(1.0000000, 1.0000000, 1.0000000);
	// SDFBox //
	d4 = box(p4, vec3(2.0000000, 0.5000000, 2.0000000));


	// COMBSubtract //
	if (d3 < -d4) d3 = -d4;

	// COMBAddSmooth //
	pmFloat0 = 0.100000;
	pmFloat1 = 0.000000;
	pmFloat2 = 1.000000;
	blend = clamp(0.5 + 0.5 * (d3 - d2) / pmFloat0, 0.0, 1.0);
	color2 = blendColors(clamp(1.0 - blend + pmFloat1, 0.0, 1.0), color2, color3, pmFloat2);
	d2 = mix(d3, d2, blend) - pmFloat0 * blend * (1.0 - blend);

	// COMBAdd //
	if (d2 < d1) {
		d1 = d2;
		color1 = color2; 
	}

	// COMBAdd //
	if (d1 < d0) {
		d0 = d1;
		color0 = color1; 
	}

	// SDFObject //
	p1 = p0;
	d1 = FLOAT_MAX;
	color1 = vec3(1.0000000, 1.0000000, 1.0000000);

	// OPMirrorX //
	p1.x = abs(p1.x);

	// OPTranslate //
	p1 -= vec3(0.2800000, -3.0000000, 0.3000000); 

	// SDFObject //
	p2 = p1;
	d2 = FLOAT_MAX;
	color2 = vec3(0.9176471, 0.7490196, 0.0156863);
	// SDFCylinderY //
	d2 = cylinderY(p2, 1, 0.15);


	// COMBAdd //
	if (d2 < d1) {
		d1 = d2;
		color1 = color2; 
	}

	// COMBAdd //
	if (d1 < d0) {
		d0 = d1;
		color0 = color1; 
	}

	// SDFObject //
	p1 = p0;
	d1 = FLOAT_MAX;
	color1 = vec3(1.0000000, 1.0000000, 1.0000000);

	// OPMirrorX //
	p1.x = abs(p1.x);

	// OPTranslate //
	p1 -= vec3(0.2800000, -3.7500000, 0.3500000); 

	// SDFObject //
	p2 = p1;
	d2 = FLOAT_MAX;
	color2 = vec3(0.0000000, 0.6156863, 0.8627451);
d2 = ellipsoid(p2, vec3(0.2750000, 0.4200000, 0.7000000));


	// OPTranslate //
	p2 -= vec3(0.0000000, 0.0000000, -0.2000000); 

	// SDFObject //
	p3 = p2;
	d3 = FLOAT_MAX;
	color3 = vec3(1.0000000, 1.0000000, 1.0000000);
	// SDFSphere //
	d3 = sphere(p3, 0.5);


	// OPTranslate //
	p3 -= vec3(0.0000000, -0.2500000, 0.0000000); 

	// SDFObject //
	p4 = p3;
	d4 = FLOAT_MAX;
	color4 = vec3(1.0000000, 1.0000000, 1.0000000);
	// SDFBox //
	d4 = box(p4, vec3(2.0000000, 0.5000000, 2.0000000));


	// COMBSubtract //
	if (d3 < -d4) d3 = -d4;

	// COMBIntersect //
	if (d2 <= d3) d2 = d3;

	// OPTranslate //
	p2 -= vec3(0.0000000, 0.0000000, 0.2000000); 

	// SDFObject //
	p3 = p2;
	d3 = FLOAT_MAX;
	color3 = vec3(1.0000000, 1.0000000, 1.0000000);
d3 = ellipsoid(p3, vec3(0.3230000, 0.4230000, 0.7300000));


	// SDFObject //
	p4 = p3;
	d4 = FLOAT_MAX;
	color4 = vec3(1.0000000, 1.0000000, 1.0000000);
	// SDFBox //
	d4 = box(p4, vec3(2.0000000, 0.1000000, 2.0000000));


	// COMBIntersectSmooth //
	pmFloat0 = 0.000000;
	pmFloat1 = 0.000000;
	pmFloat2 = 1.000000;
	blend = clamp(0.5 - 0.5 * (d4 - d3) / pmFloat0, 0.0, 1.0);
	color3 = blendColors(clamp(blend + pmFloat1, 0.0, 1.0), color3, color4, pmFloat2);
	d3 = mix(d4, d3, blend) + pmFloat0 * blend * (1.0 - blend);

	// OPTranslate //
	p3 -= vec3(0.0000000, 0.0000000, -0.2500000); 

	// SDFObject //
	p4 = p3;
	d4 = FLOAT_MAX;
	color4 = vec3(1.0000000, 1.0000000, 1.0000000);
	// SDFSphere //
	d4 = sphere(p4, 0.6);


	// COMBIntersect //
	if (d3 <= d4) d3 = d4;

	// OPTranslate //
	p3 -= vec3(0.0000000, 0.4250000, 0.2000000); 

	// SDFObject //
	p4 = p3;
	d4 = FLOAT_MAX;
	color4 = vec3(1.0000000, 1.0000000, 1.0000000);
	// SDFTorus //
	d4 = torus(p4, vec2(0.1700000, 0.0550000));


	// OPTranslate //
	p4 -= vec3(-0.2200000, -0.2000000, -0.1000000); 

	// OPRotate //
	pmVec30 = vec3(0.0000000, 0.0000000, 1.0000000);
	pmFloat0 = 0.060000;
	p4 = rotateVec3(p4, pmVec30, pmFloat0);

	// SDFObject //
	vec3 p5;
	float d5 = FLOAT_MAX;
	vec3 color5;
	p5 = p4;
	d5 = FLOAT_MAX;
	color5 = vec3(1.0000000, 1.0000000, 1.0000000);
d5 = ellipsoid(p5, vec3(0.0200000, 0.1000000, 0.1000000));


	// COMBAdd //
	if (d5 < d4) {
		d4 = d5;
		color4 = color5; 
	}

	// COMBAdd //
	if (d4 < d3) {
		d3 = d4;
		color3 = color4; 
	}

	// COMBAdd //
	if (d3 < d2) {
		d2 = d3;
		color2 = color3; 
	}

	// COMBAdd //
	if (d2 < d1) {
		d1 = d2;
		color1 = color2; 
	}

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
		rayDir = vec3(10.0000000, 5.0000000, -10.0000000) - p0;
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
				float directLight = (rayLength >= lightDist) ? attenuation * 400 : 0.0;
				float ambLight = 400 * AMB_LIGHT_POW * attenuation;
				float dot = max(0.0, dot(rayDir, normal));
				directLight *= dot;
				float lightPower = directLight + ambLight;
				cumulativeColor += vec3(1.0000000, 1.0000000, 1.0000000) * lightPower;
				break;
			}
		}

		cumulativeColor = clamp(cumulativeColor * color, 0.0, 1.0);
		cumulativeColor = mix(cumulativeColor, normalize(cumulativeColor), 0.2);
		pixels[outIndex].r = clamp(cumulativeColor.r, 0.0, 1.0);
		pixels[outIndex].g = clamp(cumulativeColor.g, 0.0, 1.0);
		pixels[outIndex].b = clamp(cumulativeColor.b, 0.0, 1.0);
		pixels[outIndex].l = min(1.0, length(cumulativeColor));

        float occ = 0.0;
        float sca = 1.0;
    
        for(int i = 0; i < AMB_OCC_SAMPLES; i++) {
            float h = 0.01 + AMB_OCC_REACH * float(i) / float(AMB_OCC_SAMPLES - 1);
            float d;
            sampleScene(p0 + normal * h, push.maxDistance, d, color, push.time);
            occ += (h - d) * sca;
        
            // Attenuate the contribution of further samples
            sca *= AMB_OCC_ATTEN;
        }
    
        // Clamp the final factor to prevent negative light values
        occ = clamp(1.0 - 3.0 * occ, 0.0, 1.0);   

        pixels[outIndex].r *= occ;
        pixels[outIndex].g *= occ;
        pixels[outIndex].b *= occ;
        //pixels[outIndex].l *= occ;
    	}
}
