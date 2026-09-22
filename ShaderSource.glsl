 
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
    
        float sphere(vec3 p, float radius) {
            return length(p) - radius;
        }
        
        float box(vec3 p, vec3 dimensions) {
            p = abs(p) - dimensions;
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

		void sampleScene(in vec3 p0, in float maxDist, out float d0, out vec3 color, out float lightVal) {
			d0 = maxDist;
			vec3 p1 = p0;
			float d1 = d0;
			d1 = tube(p1, 2, vec2(1.0000000, 0.1250000));

			if (d1 < d0) d0 = d1;
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
                vec3 p0 = camPos + rayDir * rayLength;
                float d0 = push.maxDistance;
                vec3 color = vec3(0.0);
                float lightVal;

                sampleScene(p0, push.maxDistance, d0, color, lightVal);

                if (d0 < push.surfTolerance) {
                    pixels[outIndex].l = rayLength / push.maxDistance;
                    pixels[outIndex].r = 1.0;
                    pixels[outIndex].g = 0.0;
                    pixels[outIndex].b = 0.0;
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