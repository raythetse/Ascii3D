 
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
            float pad0;
            float pad1;
            float pad2;
        } push;
    
        const float HALFPI = 1.570796;
        const float PI = 3.1415923;
        const float TAU = 6.283185;
        const float ROOT2 = 1.414213;
        const float FLOAT_MAX = 3.402823466e38;

        vec3 rotateVec3(vec3 p, vec3 axis, float rotations) {
            vec3 a = normalize(axis);
            float angle = rotations * TAU;
            return p * cos(angle) + cross(a, p) * sin(angle) + a * dot(a, p) * (1.0 - cos(angle));
        }

        vec3 blendColors(float alpha, vec3 colorA, vec3 colorB, float colorBlending)
        {
	        alpha *= colorBlending;
	        alpha -= 0.5 * (colorBlending - 1.0);
	        alpha = clamp(alpha, 0.0, 1.0);
	        return mix(colorA, colorB, alpha);
        }

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

		void sampleScene(in vec3 p0, in float maxDist, out float d0, out vec3 color0, out float lightVal, float time) {
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
			color1 = vec3(1.0000000, 1.0000000, 1.0000000);
			p1 -= vec3(0.0000000, 0.0000000, 0.0000000);

			// OPCubeRollHeight //
			pmFloat0 = 0.500000;
			pmFloat1 = 1.000000;
			pmFloat2 = 1.000000;
			pmFloat0 = mod(time + pmFloat0, pmFloat1) * HALFPI - HALFPI;
			pmFloat1 = cos(pmFloat0);
			pmFloat0 = sin(pmFloat0);
			pmVec20 = vec2(-pmFloat2 * 0.5, pmFloat2 * 0.5);
			pmVec20 = vec2(pmVec20.x * pmFloat1 - pmVec20.y * pmFloat0, pmVec20.x * pmFloat0 + pmVec20.y * pmFloat1);
			pmFloat0 = (pmVec20.y - pmFloat2 * 0.5);
			p1.y -= pmFloat0;

			// OPTranslateLinear //
			pmFloat0 = 0.000000;
			pmVec20 = vec2(1.0000000, 1.0000000);
			pmVec21 = vec2(1.0000000, 1.0000000);
			pmVec30 = vec3(1.0000000, 0.0000000, 0.0000000);
			p1 -= translateLinear(pmVec30, pmVec20, pmVec21, pmFloat0, time);

			// OPTranslateLinear //
			pmFloat0 = 1.000000;
			pmVec20 = vec2(1.0000000, 1.0000000);
			pmVec21 = vec2(1.0000000, 1.0000000);
			pmVec30 = vec3(0.0000000, 0.0000000, 1.0000000);
			p1 -= translateLinear(pmVec30, pmVec20, pmVec21, pmFloat0, time);

			// OPCubeRoller //
			pmFloat0 = 0.500000;
			pmFloat1 = 1.000000;
			vec3 axes0[4]; 
			axes0[0] = vec3(0.0000000, 0.0000000, 1.0000000);
			axes0[1] = vec3(1.0000000, 0.0000000, 0.0000000);
			axes0[2] = vec3(0.0000000, 0.0000000, -1.0000000);
			axes0[3] = vec3(-1.0000000, 0.0000000, 0.0000000);
			pmFloat2 = 4 * pmFloat1;
			pmFloat2 = mod(time + pmFloat0, pmFloat2);
			pmFloat0 = floor(pmFloat2);
			pmFloat2 /= pmFloat1;
			p1 = rotateVec3(p1, axes0[int(pmFloat0)], 0.25 * pmFloat2);

			// SDFObject //
			vec3 p2;
			float d2 = FLOAT_MAX;
			vec3 color2;
			p2 = p1;
			d2 = FLOAT_MAX;
			color2 = vec3(1.0000000, 0.0000000, 0.0000000);
			p2 -= vec3(0.0000000, 0.0000000, 0.0000000);
			// SDFBox //
			d2 = box(p2, vec3(1.0000000, 1.0000000, 1.0000000));


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
			p1 -= vec3(0.0000000, 0.0000000, 0.0000000);

			// OPTranslate //
			p1 -= vec3(0.0000000, 1.0000000, 0.0000000); 

			// OPCubeRollHeight //
			pmFloat0 = 0.500000;
			pmFloat1 = 1.000000;
			pmFloat2 = 3.000000;
			pmFloat0 = mod(time + pmFloat0, pmFloat1) * HALFPI - HALFPI;
			pmFloat1 = cos(pmFloat0);
			pmFloat0 = sin(pmFloat0);
			pmVec20 = vec2(-pmFloat2 * 0.5, pmFloat2 * 0.5);
			pmVec20 = vec2(pmVec20.x * pmFloat1 - pmVec20.y * pmFloat0, pmVec20.x * pmFloat0 + pmVec20.y * pmFloat1);
			pmFloat0 = (pmVec20.y - pmFloat2 * 0.5);
			p1.y -= pmFloat0;

			// OPTranslateLinear //
			pmFloat0 = 0.000000;
			pmVec20 = vec2(1.0000000, 1.0000000);
			pmVec21 = vec2(1.0000000, 1.0000000);
			pmVec30 = vec3(1.0000000, 0.0000000, 0.0000000);
			p1 -= translateLinear(pmVec30, pmVec20, pmVec21, pmFloat0, time);

			// OPTranslateLinear //
			pmFloat0 = 1.000000;
			pmVec20 = vec2(1.0000000, 1.0000000);
			pmVec21 = vec2(1.0000000, 1.0000000);
			pmVec30 = vec3(0.0000000, 0.0000000, 1.0000000);
			p1 -= translateLinear(pmVec30, pmVec20, pmVec21, pmFloat0, time);

			// OPCubeRoller //
			pmFloat0 = 0.500000;
			pmFloat1 = 1.000000;
			vec3 axes1[4]; 
			axes1[0] = vec3(0.0000000, 0.0000000, -1.0000000);
			axes1[1] = vec3(-1.0000000, 0.0000000, 0.0000000);
			axes1[2] = vec3(0.0000000, 0.0000000, 1.0000000);
			axes1[3] = vec3(1.0000000, 0.0000000, 0.0000000);
			pmFloat2 = 4 * pmFloat1;
			pmFloat2 = mod(time + pmFloat0, pmFloat2);
			pmFloat0 = floor(pmFloat2);
			pmFloat2 /= pmFloat1;
			p1 = rotateVec3(p1, axes1[int(pmFloat0)], 0.25 * pmFloat2);

			// SDFObject //
			p2 = p1;
			d2 = FLOAT_MAX;
			color2 = vec3(1.0000000, 1.0000000, 0.0000000);
			p2 -= vec3(0.0000000, 0.0000000, 0.0000000);
			// SDFBox //
			d2 = box(p2, vec3(1.0000000, 1.0000000, 1.0000000));


			// COMBAdd //
			if (d2 < d1) {
				d1 = d2;
				color1 = color2; 
			}

			// COMBAddSmooth //
			pmFloat0 = 0.250000;
			pmFloat1 = 0.000000;
			pmFloat2 = 0.950000;
			blend = clamp(0.5 + 0.5 * (d1 - d0) / pmFloat0, 0.0, 1.0);
			color0 = blendColors(clamp(1.0 - blend + pmFloat1, 0.0, 1.0), color0, color1, pmFloat2);
			d0 = mix(d1, d0, blend) - pmFloat0 * blend * (1.0 - blend);

			// SDFObject //
			p1 = p0;
			d1 = FLOAT_MAX;
			color1 = vec3(1.0000000, 1.0000000, 1.0000000);
			p1 -= vec3(0.0000000, 0.0000000, 0.0000000);

			// OPRotate //
			pmVec30 = vec3(0.0000000, 1.0000000, 0.0000000);
			pmFloat0 = 0.500000;
			p1 = rotateVec3(p1, pmVec30, pmFloat0);

			// OPCubeRollHeight //
			pmFloat0 = 0.500000;
			pmFloat1 = 1.000000;
			pmFloat2 = 1.000000;
			pmFloat0 = mod(time + pmFloat0, pmFloat1) * HALFPI - HALFPI;
			pmFloat1 = cos(pmFloat0);
			pmFloat0 = sin(pmFloat0);
			pmVec20 = vec2(-pmFloat2 * 0.5, pmFloat2 * 0.5);
			pmVec20 = vec2(pmVec20.x * pmFloat1 - pmVec20.y * pmFloat0, pmVec20.x * pmFloat0 + pmVec20.y * pmFloat1);
			pmFloat0 = (pmVec20.y - pmFloat2 * 0.5);
			p1.y -= pmFloat0;

			// OPTranslateLinear //
			pmFloat0 = 0.000000;
			pmVec20 = vec2(1.0000000, 1.0000000);
			pmVec21 = vec2(1.0000000, 1.0000000);
			pmVec30 = vec3(1.0000000, 0.0000000, 0.0000000);
			p1 -= translateLinear(pmVec30, pmVec20, pmVec21, pmFloat0, time);

			// OPTranslateLinear //
			pmFloat0 = 1.000000;
			pmVec20 = vec2(1.0000000, 1.0000000);
			pmVec21 = vec2(1.0000000, 1.0000000);
			pmVec30 = vec3(0.0000000, 0.0000000, 1.0000000);
			p1 -= translateLinear(pmVec30, pmVec20, pmVec21, pmFloat0, time);

			// OPCubeRoller //
			pmFloat0 = 0.500000;
			pmFloat1 = 1.000000;
			vec3 axes2[4]; 
			axes2[0] = vec3(0.0000000, 0.0000000, 1.0000000);
			axes2[1] = vec3(1.0000000, 0.0000000, 0.0000000);
			axes2[2] = vec3(0.0000000, 0.0000000, -1.0000000);
			axes2[3] = vec3(-1.0000000, 0.0000000, 0.0000000);
			pmFloat2 = 4 * pmFloat1;
			pmFloat2 = mod(time + pmFloat0, pmFloat2);
			pmFloat0 = floor(pmFloat2);
			pmFloat2 /= pmFloat1;
			p1 = rotateVec3(p1, axes2[int(pmFloat0)], 0.25 * pmFloat2);

			// SDFObject //
			p2 = p1;
			d2 = FLOAT_MAX;
			color2 = vec3(0.0000000, 0.0000000, 1.0000000);
			p2 -= vec3(0.0000000, 0.0000000, 0.0000000);
			// SDFBox //
			d2 = box(p2, vec3(1.0000000, 1.0000000, 1.0000000));


			// COMBAdd //
			if (d2 < d1) {
				d1 = d2;
				color1 = color2; 
			}

			// COMBAddSmooth //
			pmFloat0 = 0.250000;
			pmFloat1 = 0.000000;
			pmFloat2 = 0.950000;
			blend = clamp(0.5 + 0.5 * (d1 - d0) / pmFloat0, 0.0, 1.0);
			color0 = blendColors(clamp(1.0 - blend + pmFloat1, 0.0, 1.0), color0, color1, pmFloat2);
			d0 = mix(d1, d0, blend) - pmFloat0 * blend * (1.0 - blend);

			// SDFObject //
			p1 = p0;
			d1 = FLOAT_MAX;
			color1 = vec3(1.0000000, 1.0000000, 1.0000000);
			p1 -= vec3(0.0000000, 0.0000000, 0.0000000);

			// OPTranslate //
			p1 -= vec3(0.0000000, 1.0000000, 0.0000000); 

			// OPRotate //
			pmVec30 = vec3(0.0000000, 1.0000000, 0.0000000);
			pmFloat0 = 0.500000;
			p1 = rotateVec3(p1, pmVec30, pmFloat0);

			// OPCubeRollHeight //
			pmFloat0 = 0.500000;
			pmFloat1 = 1.000000;
			pmFloat2 = 3.000000;
			pmFloat0 = mod(time + pmFloat0, pmFloat1) * HALFPI - HALFPI;
			pmFloat1 = cos(pmFloat0);
			pmFloat0 = sin(pmFloat0);
			pmVec20 = vec2(-pmFloat2 * 0.5, pmFloat2 * 0.5);
			pmVec20 = vec2(pmVec20.x * pmFloat1 - pmVec20.y * pmFloat0, pmVec20.x * pmFloat0 + pmVec20.y * pmFloat1);
			pmFloat0 = (pmVec20.y - pmFloat2 * 0.5);
			p1.y -= pmFloat0;

			// OPTranslateLinear //
			pmFloat0 = 0.000000;
			pmVec20 = vec2(1.0000000, 1.0000000);
			pmVec21 = vec2(1.0000000, 1.0000000);
			pmVec30 = vec3(1.0000000, 0.0000000, 0.0000000);
			p1 -= translateLinear(pmVec30, pmVec20, pmVec21, pmFloat0, time);

			// OPTranslateLinear //
			pmFloat0 = 1.000000;
			pmVec20 = vec2(1.0000000, 1.0000000);
			pmVec21 = vec2(1.0000000, 1.0000000);
			pmVec30 = vec3(0.0000000, 0.0000000, 1.0000000);
			p1 -= translateLinear(pmVec30, pmVec20, pmVec21, pmFloat0, time);

			// OPCubeRoller //
			pmFloat0 = 0.500000;
			pmFloat1 = 1.000000;
			vec3 axes3[4]; 
			axes3[0] = vec3(0.0000000, 0.0000000, -1.0000000);
			axes3[1] = vec3(-1.0000000, 0.0000000, 0.0000000);
			axes3[2] = vec3(0.0000000, 0.0000000, 1.0000000);
			axes3[3] = vec3(1.0000000, 0.0000000, 0.0000000);
			pmFloat2 = 4 * pmFloat1;
			pmFloat2 = mod(time + pmFloat0, pmFloat2);
			pmFloat0 = floor(pmFloat2);
			pmFloat2 /= pmFloat1;
			p1 = rotateVec3(p1, axes3[int(pmFloat0)], 0.25 * pmFloat2);

			// SDFObject //
			p2 = p1;
			d2 = FLOAT_MAX;
			color2 = vec3(0.0000000, 1.0000000, 0.0000000);
			p2 -= vec3(0.0000000, 0.0000000, 0.0000000);
			// SDFBox //
			d2 = box(p2, vec3(1.0000000, 1.0000000, 1.0000000));


			// COMBAdd //
			if (d2 < d1) {
				d1 = d2;
				color1 = color2; 
			}

			// COMBAddSmooth //
			pmFloat0 = 0.250000;
			pmFloat1 = 0.000000;
			pmFloat2 = 0.950000;
			blend = clamp(0.5 + 0.5 * (d1 - d0) / pmFloat0, 0.0, 1.0);
			color0 = blendColors(clamp(1.0 - blend + pmFloat1, 0.0, 1.0), color0, color1, pmFloat2);
			d0 = mix(d1, d0, blend) - pmFloat0 * blend * (1.0 - blend);

		}

        void main() {
            uint coordX = gl_GlobalInvocationID.x;
            uint coordY = gl_GlobalInvocationID.y;
    
            // 2. Strict bounds check (Never remove this!)
            if (coordX >= uint(push.width) || coordY >= uint(push.height)) return;

            vec2 viewportCoords = vec2(float(coordX) / push.width, float(coordY) / push.height);

            vec3 camPos = vec3(push.camPX, push.camPY, push.camPZ);
            vec3 camForward = vec3(push.camFX, push.camFY, push.camFZ);
            vec3 camRight = vec3(push.camRX, push.camRY, push.camRZ);
            vec3 camUp = vec3(push.camUX, push.camUY, push.camUZ);

            vec3 screenPoint = camPos + camForward;
            screenPoint -= camRight;
            screenPoint += camRight * 2.0 * viewportCoords.x;
            screenPoint += camUp * 0.5;
            screenPoint -= camUp * viewportCoords.y;

            vec3 rayDir = normalize(screenPoint - camPos);

            float rayLength = 0.0;

            uint outIndex = (coordY * uint(push.width)) + coordX;
            pixels[outIndex].l = 0.0;

            bool hit = false;

	        while (rayLength < push.maxDistance) {
                vec3 p0 = camPos + rayDir * rayLength;  // Marched sample point with inherited manipulation
                float d0 = push.maxDistance;  // Blendable/selectable distance values
                vec3 color = vec3(0.0);
                float lightVal;

                sampleScene(p0, push.maxDistance, d0, color, lightVal, push.time);

                if (d0 < push.surfTolerance) {
                    pixels[outIndex].l = rayLength / push.maxDistance;
                    pixels[outIndex].r = clamp(color.r, 0.0, 1.0);
                    pixels[outIndex].g = clamp(color.g, 0.0, 1.0);
                    pixels[outIndex].b = clamp(color.b, 0.0, 1.0);
                    hit = true;
                    break;
                }
		        rayLength += d0;
	        }

            // Do normal calc here;

            [[branch]] if (hit) {
                // Shadow marching
            }
        }