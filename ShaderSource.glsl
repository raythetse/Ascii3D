 
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
const int AMB_OCC_SAMPLES = 5;
const float AMB_OCC_ATTEN = 0.5;
const float AMB_OCC_REACH = 0.1;
const float AMB_OCC_MIN = 0.2;

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

float extrude2D(float pDotY, float d2D, float height) {
    vec2 w = vec2(d2D, abs(pDotY) - height * 0.5);
    return min(max(w.x, w.y), 0.0) + length(max(w, 0.0));
}

vec3 revolve2D(vec3 p, float radius, float rotations, vec2 rotateOffset)
{
    p = vec3(length(vec2(p.x, p.y)) - radius, p.z, p.z);
    return rotateVec3(p, vec3(0.0, 1.0, 0.0), rotations) - vec3(rotateOffset.x, 0.0, rotateOffset.y);
}

vec3 blendColors(float alpha, vec3 colorA, vec3 colorB, float colorBlending)
{
	alpha *= colorBlending;
	alpha -= 0.5 * (colorBlending - 1.0);
	alpha = clamp(alpha, 0.0, 1.0);
	return mix(colorA, colorB, alpha);
}

bool isEnabled(float onTime, float offTime, float phase, float time) {
    return mod(time + phase, onTime + offTime) <= onTime;
}

// OPERATION FUNCTIONS //
float smoothCycle1D(float initVal, float period, float amplitude, float phase, float time) {
    return initVal + sin(TAU * (time + (phase * period)) / period) * amplitude;
}

vec2 smoothCycle2D(vec2 initVal, vec2 period, vec2 amplitude, vec2 phase, float time) {
    return initVal + sin(TAU * (vec2(time) + (phase * period)) / period) * amplitude;
}

vec3 smoothCycle3D(vec3 initVal, vec3 period, vec3 amplitude, vec3 phase, float time) {
    return initVal + sin(TAU * (vec3(time) + (phase * period)) / period) * amplitude;
}

float linearCycle1D(float initVal, float amplitude, float forwardTime, float delayTime1, float backwardTime, float delayTime2, float phase, float time) {
	float totalTime = forwardTime + delayTime1 + backwardTime + delayTime2;
	float cycleTime = mod(time + phase, totalTime);
	float progress = 0.0;

	if (cycleTime < forwardTime) {
		progress = cycleTime / forwardTime;
	}
	else if ((cycleTime -= forwardTime) < delayTime1) {
		progress = 1.0;
	}
	else if ((cycleTime -= delayTime1) < backwardTime) {
		progress = 1.0 - (cycleTime / backwardTime);
	}
	else {
		progress = 0.0;
	}

	return initVal + mix(-1.0, 1.0, progress) * amplitude;
}

vec2 linearCycle2D(vec2 initVal, vec2 amplitude, vec2 forwardTime, vec2 delayTime1, vec2 backwardTime, vec2 delayTime2, vec2 phase, float time) {
    return vec2 (
        linearCycle1D(initVal.x, amplitude.x, forwardTime.x, delayTime1.x, backwardTime.x, delayTime2.x, phase.x, time),
        linearCycle1D(initVal.y, amplitude.y, forwardTime.y, delayTime1.y, backwardTime.y, delayTime2.y, phase.y, time)
    );
}  

vec3 linearCycle3D(vec3 initVal, vec3 amplitude, vec3 forwardTime, vec3 delayTime1, vec3 backwardTime, vec3 delayTime2, vec3 phase, float time) {
    return vec3 (
        linearCycle1D(initVal.x, amplitude.x, forwardTime.x, delayTime1.x, backwardTime.x, delayTime2.x, phase.x, time),
        linearCycle1D(initVal.y, amplitude.y, forwardTime.y, delayTime1.y, backwardTime.y, delayTime2.y, phase.y, time),
        linearCycle1D(initVal.z, amplitude.z, forwardTime.z, delayTime1.z, backwardTime.z, delayTime2.z, phase.z, time)
    );
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

vec3 repeatCircle(vec3 p, float repetitions) {
	float b = TAU / repetitions;
	float a = atan(p.z, p.x);

	if (a < 0.0) a += TAU;

	float i = floor((a + b * 0.5) / b);

	if (i >= repetitions) i = 0.0;
	float c = b * i;
    float sn = sin(c);
    float cs = cos(c);

    vec2 p2D = vec2(p.x, p.z);
	p2D = mat2(cs, -sn, sn, cs) * p2D;

    return vec3(p2D.x, p.y, p2D.y);
}

// OBJECT FUNCTIONS //
float box(vec3 p, vec3 dimensions) {
    p = abs(p) - dimensions * 0.5;
    return length(max(p,0.0)) + min(max(p.x,max(p.y,p.z)),0.0);
}

float boxFrame(vec3 p, vec3 dimensions, float edgeThickness) {
    p = abs(p) - dimensions * 0.5;
    float halfThick = edgeThickness * 0.5;
    vec3 q = abs(p + halfThick) - halfThick;
    return min(min(
        length(max(vec3(p.x, q.y, q.z), 0.0)) + min(max(p.x, max(q.y, q.z)), 0.0),
        length(max(vec3(q.x, p.y, q.z), 0.0)) + min(max(q.x, max(p.y, q.z)), 0.0)),
        length(max(vec3(q.x, q.y, p.z), 0.0)) + min(max(q.x, max(q.y, p.z)), 0.0));
}

float sphere(vec3 p, float radius) {
    return length(p) - radius;
}

float torus(vec3 p, vec2 radii) {
    return length(vec2(length(vec2(p.x, p.z)) - radii.x, p.y)) - radii.y;
}

float torusSegment(vec3 p, vec2 radii, float sweep) {
    float angle = PI * sweep;
    float sin = sin(angle);
    float cos = cos(angle);
    p.x = abs(p.x);
    float k = (cos * p.x > sin * p.z) ? dot(p.xz, vec2(sin, cos)) : length(p.xz);
    return sqrt(dot(p, p) + radii.x * radii.x - 2.0 * radii.x * k) - radii.y;
}

float capsule(vec3 p, float bodyLength, float radius) {
    float halfBL = bodyLength * 0.5; 
    p.y -= clamp( p.y, -halfBL, halfBL);
    return length(p) - radius;
}

float crystal(vec3 p, float edgeLength) {
    p = abs(p);
    return (p.x + p.y + p.z - edgeLength) * 0.57735027;
}

float cylinder(vec3 p, float bodyLength, float radius) {
    vec2 xz = vec2(p.x, p.z);
    vec2 d = abs(vec2(length(xz), p.y )) - vec2(radius, bodyLength * 0.5);
    return min(max(d.x, d.y), 0.0) + length(max(d, 0.0));
}

float chainLink(vec3 p, float sideLength, vec2 radii) {
    p.x = p.x > 0.0 ? max(0.0, p.x - sideLength * 0.5) : min(0.0, p.x + sideLength * 0.5);
    return length(vec2(length(vec2(p.x, p.z)) - radii.x, p.y)) - radii.y;
}

float cone(vec3 p, float height, float sweep) {
    float angle = sweep * HALFPI;
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

float solidAngle(vec3 p, float sweep, float radius) {
    float angle = sweep * PI;
    vec2 sincos = vec2(sin(angle), cos(angle));
    vec2 q = vec2(length(p.xz), p.y);
    float l = length(q) - radius;
    float m = length(q - sincos * clamp(dot(q, sincos), 0.0, radius));
    return max(l, m * sign(sincos.y * q.x - sincos.x * q.y));
}

float ellipsoid(vec3 p, vec3 radii) {
    float k0 = length(p / radii);
    float k1 = length(p / (radii * radii));
    return k0 * (k0 - 1.0) / k1;
}

float taperedCapsule(vec3 p, vec2 radii, float height) {
    float b = (radii.x - radii.y) / height;
    float a = sqrt(1.0 - b * b);

    vec2 q = vec2(length(p.xz), p.y);
    float k = dot(q, vec2(-b, a));
    if(k < 0.0) return length(q) - radii.x;
    if(k > a * height) return length(q - vec2(0.0, height)) - radii.y;
    return dot(q, vec2(a, b)) - radii.x;
}

float taperedCylinder(vec3 p, float sideLength, vec2 radii) {
    float halfLength = sideLength * 0.5;
    vec2 q = vec2(length(p.xz), p.y);
    vec2 k1 = vec2(radii.y, halfLength);
    vec2 k2 = vec2(radii.y - radii.x, 2.0 * halfLength);
    vec2 ca = vec2(q.x - min(q.x, (q.y < 0.0) ? radii.x : radii.y), abs(q.y) - halfLength);
    vec2 cb = q - k1 + k2 * clamp(dot(k1 - q, k2) / dot(k2, k2), 0.0, 1.0 );
    float s = (cb.x < 0.0 && ca.y < 0.0) ? -1.0 : 1.0;
    return s * sqrt(min(dot(ca, ca), dot(cb, cb)));
}

float rhombus(vec3 p, vec2 dimensions, int operation, float magnitude, float revolveRotations, vec2 revolveRotateOffset)
{
    if (operation == 1) p = revolve2D(p, magnitude, revolveRotations, revolveRotateOffset);
    vec2 dim2D = vec2(dimensions.x, -dimensions.y);
    vec2 p2D = vec2(p.x, p.z);
    p2D = abs(p2D);
    float h = clamp((dot(dim2D, p2D) + dim2D.y * dim2D.y) / dot(dim2D, dim2D), 0.0, 1.0 );
    p2D -= dim2D * vec2(h, h-1.0);
    float d2D = length(p2D) * sign(p2D.x);
    return operation == 0 ? extrude2D(p.y, d2D, magnitude) : d2D;
}

float equilateralTriangle(vec3 p, float base, int operation, float magnitude, float revolveRotations, vec2 revolveRotateOffset) {
    if (operation == 1) p = revolve2D(p, magnitude, revolveRotations, revolveRotateOffset);
    float halfBase = base * 0.5;
    vec2 p2D = vec2(p.x, p.z - base * ROOT3 / 6.0 + halfBase);
    p2D.x = abs(p2D.x);
    p2D -= vec2(0.5, 0.5 * ROOT3) * max(p2D.x + ROOT3 * p2D.y, 0.0);
    p2D -= vec2(clamp(p2D.x, -halfBase, halfBase), -halfBase / ROOT3);
    float d2D = length(p2D) * sign(-p2D.y);
    return operation == 0 ? extrude2D(p.y, d2D, magnitude) : d2D;
}

float isoscelesTriangle(vec3 p, vec2 dimensions, int operation, float magnitude, float revolveRotations, vec2 revolveRotateOffset) {
    if (operation == 1) p = revolve2D(p, magnitude, revolveRotations, revolveRotateOffset);
    vec2 p2D = vec2(abs(p.x), p.z);
    vec2 a = p2D - dimensions * clamp(dot(p2D, dimensions) / dot(dimensions, dimensions), 0.0, 1.0 );
    vec2 b = p2D - dimensions * vec2(clamp(p2D.x / dimensions.x, 0.0, 1.0 ), 1.0 );
    float s = -sign(dimensions.y);
    vec2 d = min(vec2(dot(a, a), s * (p2D.x * dimensions.y - p2D.y * dimensions.x)), vec2(dot(b, b), s * (p2D.y - dimensions.y)));
    float d2D = -sqrt(d.x) * sign(d.y);
    return operation == 0 ? extrude2D(p.y, d2D, magnitude) : d2D;
}

float pieSlice(vec3 p, float sweep, float radius, int operation, float magnitude, float revolveRotations, vec2 revolveRotateOffset) {
    if (operation == 1) p = revolve2D(p, magnitude, revolveRotations, revolveRotateOffset);
    float angle = sweep * PI;
    vec2 sincos = vec2(sin(angle), cos(angle));
    vec2 p2D = vec2(p.x, p.z);
    p2D.x = abs(p2D.x);
    float l = length(p2D) - radius;
    float m = length(p2D - sincos * clamp(dot(p2D, sincos), 0.0, radius));
    float d2D = max(l, m * sign(sincos.y * p2D.x - sincos.x * p2D.y));
    return operation == 0 ? extrude2D(p.y, d2D, magnitude) : d2D;
}

float pentagon(vec3 p, float radius, int operation, float magnitude, float revolveRotations, vec2 revolveRotateOffset) {
    if (operation == 1) p = revolve2D(p, magnitude, revolveRotations, revolveRotateOffset);
    const vec3 k = vec3(0.809016994, 0.587785252, 0.726542528);
    vec2 p2D = vec2(abs(p.x), p.z);
    p2D -= 2.0 * min(dot(vec2(-k.x, k.y), p2D), 0.0) * vec2(-k.x, k.y);
    p2D -= 2.0 * min(dot(vec2( k.x, k.y), p2D), 0.0) * vec2( k.x, k.y);
    p2D -= vec2(clamp(p2D.x, -radius * k.z, radius * k.z), radius);    
    float d2D = length(p2D) * sign(p2D.y);
    return operation == 0 ? extrude2D(p.y, d2D, magnitude) : d2D;
}

float hexagon(vec3 p, float radius, int operation, float magnitude, float revolveRotations, vec2 revolveRotateOffset) {
    if (operation == 1) p = revolve2D(p, magnitude, revolveRotations, revolveRotateOffset);
    const vec3 k = vec3(-0.866025404,0.5,0.577350269);
    vec2 p2D = vec2(abs(p.x), abs(p.z));
    p2D -= 2.0 * min(dot(k.xy, p2D), 0.0) * k.xy;
    p2D -= vec2(clamp(p2D.x, -k.z * radius, k.z * radius), radius);
    float d2D = length(p2D) * sign(p2D.y);
    return operation == 0 ? extrude2D(p.y, d2D, magnitude) : d2D;
}

float octagon(vec3 p, float radius, int operation, float magnitude, float revolveRotations, vec2 revolveRotateOffset) {
    if (operation == 1) p = revolve2D(p, magnitude, revolveRotations, revolveRotateOffset);
    const vec3 k = vec3(-0.9238795325, 0.3826834323, 0.4142135623);
    vec2 p2D = vec2(abs(p.x), abs(p.z));
    p2D -= 2.0 * min(dot(vec2(k.x, k.y), p2D), 0.0) * vec2(k.x, k.y);
    p2D -= 2.0 * min(dot(vec2(-k.x, k.y), p2D), 0.0) * vec2(-k.x, k.y);
    p2D -= vec2(clamp(p2D.x, -k.z * radius, k.z * radius), radius);
    float d2D = length(p2D) * sign(p2D.y);
    return operation == 0 ? extrude2D(p.y, d2D, magnitude) : d2D;
}

float star(vec3 p, float numPoints, float radius, float ratio, int operation, float magnitude, float revolveRotations, vec2 revolveRotateOffset) {
    if (operation == 1) p = revolve2D(p, magnitude, revolveRotations, revolveRotateOffset);
    float a = PI / numPoints;
    vec2 asincos = vec2(sin(a), cos(a));
    vec2 acs = vec2(asincos.y, asincos.x);
    float e = PI / mix(2.0, numPoints, ratio);
    vec2 esincos = vec2(sin(e), cos(e));
    vec2 ecs = vec2(esincos.y, esincos.x);
    float an = PI / numPoints;
    vec2 p2D = vec2(abs(p.x), p.z);
    float bn = mod(atan(p2D.x, p2D.y), 2.0 * an) - an;
    float sinbn = sin(bn);
    float cosbn = cos(bn);
    p2D = length(p2D) * vec2(cosbn, abs(sinbn));
    p2D -= radius * acs;
    p2D += ecs * clamp(-dot(p2D, ecs), 0.0, radius * acs.y / ecs.y);
    float d2D = length(p2D) * sign(p2D.x);
    return operation == 0 ? extrude2D(p.y, d2D, magnitude) : d2D;
}

float triangle(vec3 p, vec2 point1, vec2 point2, vec2 point3, int operation, float magnitude, float revolveRotations, vec2 revolveRotateOffset) {
    if (operation == 1) p = revolve2D(p, magnitude, revolveRotations, revolveRotateOffset);
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
    return operation == 0 ? extrude2D(p.y, d2D, magnitude) : d2D;    
}
    
void sampleScene(in vec3 p0, in float maxDist, out float d0, out vec3 color0, in float time) {
	d0 = maxDist;
	float blend;
	color0 = vec3(0.0);

	float pmFloat0, pmFloat1, pmFloat2, d1, d2, d3, d4, d5, d6, d7, d8, d9, d10;
	vec2 pmVec20, pmVec21, pmVec22;
	vec3 pmVec30, pmVec31, pmVec32, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, color1, color2, color3, color4, color5, color6, color7, color8, color9, color10;

	// SDFObject //
	pmVec30 = vec3(1.0000000, 1.0000000, 1.0000000);
	color0 = pmVec30;

	// SDFObject //
	pmVec30 = vec3(1.0000000, 1.0000000, 1.0000000);
	p1 = p0;
	d1 = FLOAT_MAX;
	color1 = pmVec30;
	// SDFStar //
	pmFloat0 = 2.000000;
	pmFloat1 = smoothCycle1D(1.0000000, 5.6470000, 0.5000000, 0.0000000, time);
	pmFloat2 = smoothCycle1D(0.5000000, 8.0000000, 0.4500000, 0.0000000, time);
	pmVec20.x = 6.000000;
	pmVec20.y = (1.0000000 + 0.2500000 * (0.0000000 + time));
	pmVec22 = smoothCycle2D(vec2(0.0000000, 0.0000000), vec2(2.1740000, 3.6781001), vec2(0.5000000, 0.5000000), vec2(0.0000000, 0.0000000), time);
	d1 = star(p1, pmVec20.x, pmFloat1, pmFloat2, 1, pmFloat0, pmVec20.y, pmVec22);


	// COMBAdd //
	if (d1 < d0) {
		d0 = d1;
		color0 = color1; 
	}
	else if (false) {
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
        vec3 np = p0 - rayDir * min(d0, push.surfTolerance);
        const vec2 offset = vec2(NORM_CHECK_OFFSET, 0.0);

        float d1 = push.maxDistance;
        float d2 = push.maxDistance;
        vec3 colorSample;

        vec3 normal;
        sampleScene(np + offset.xyy, push.maxDistance, d1, colorSample, push.time);
        sampleScene(np - offset.xyy, push.maxDistance, d2, colorSample, push.time);
        normal.x = d1 - d2;
        d1 = push.maxDistance;
        d2 = push.maxDistance;
        sampleScene(np + offset.yxy, push.maxDistance, d1, colorSample, push.time);
        sampleScene(np - offset.yxy, push.maxDistance, d2, colorSample, push.time);
        normal.y = d1 - d2;
        d1 = push.maxDistance;
        d2 = push.maxDistance;
        sampleScene(np + offset.yyx, push.maxDistance, d1, colorSample, push.time);
        sampleScene(np - offset.yyx, push.maxDistance, d2, colorSample, push.time);
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
		rayDir = vec3(1.0000000, 10.0000000, -7.5000000) - p0;
		lightDist = length(rayDir);
		rayDir = normalize(rayDir);
		rayLength = push.surfTolerance+0.001;
		d0 = push.maxDistance;

		while (true) {
			np = p0 + rayDir * (SHADOW_TOLERANCE + rayLength);

			sampleScene(np, lightDist, d0, colorSample, push.time);
			rayLength += d0;
			if (d0 < push.surfTolerance || rayLength >= lightDist) {
				float attenuation = 1.0 / (lightDist * lightDist);
				float directLight = (rayLength >= lightDist) ? attenuation * 200 : 0.0;
				float ambLight = 200 * 0.15 * attenuation;
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
        occ = clamp(1.0 - 3.0 * occ, AMB_OCC_MIN, 1.0);   

        pixels[outIndex].r *= occ;
        pixels[outIndex].g *= occ;
        pixels[outIndex].b *= occ;
        pixels[outIndex].l *= occ;
    	}
}
