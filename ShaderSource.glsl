 
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

		void sampleScene(in vec3 p0, in float maxDist, out float d0, out vec3 color, out float lightVal) {
			d0 = maxDist;
			vec3 p1 = p0;
			float d1 = d0;
			p1 -= vec3(0.0000000, 0.0000000, 1.0000000);
			p1 = mat3(-0.3333333, 0.6666667, 0.6666667, 0.6666667, -0.3333333, 0.6666667, 0.6666667, 0.6666667, -0.3333333) * p1;
			d1 = sphere(p1, 0.5); 

			vec3 p2 = p1;
			float d2 = d1;
			p2 -= vec3(0.0000000, -1.0000000, 0.0000000);
			p2 = mat3(0.0012099, -0.0012085, 0.9999985, 0.9999985, 0.0012099, -0.0012085, -0.0012085, 0.9999985, 0.0012099) * p2;
			d2 = box(p2, vec3(0.5000000, 0.2500000, 0.7500000)); 

			vec3 p3 = p2;
			float d3 = d2;
			p3 -= vec3(0.0000000, 2.0000000, 0.0000000);
			p3 = mat3(0.0012099, -0.0012085, 0.9999985, 0.9999985, 0.0012099, -0.0012085, -0.0012085, 0.9999985, 0.0012099) * p3;
			d3 = boxFrame(p3, vec3(0.5000000, 0.2500000, 0.7500000), 0.05); 

			vec3 p4 = p3;
			float d4 = d3;
			p4 -= vec3(0.0000000, 2.0000000, 0.0000000);
			p4 = mat3(0.0012099, -0.0012085, 0.9999985, 0.9999985, 0.0012099, -0.0012085, -0.0012085, 0.9999985, 0.0012099) * p4;
			d4 = boxFrame(p4, vec3(0.5000000, 0.2500000, 0.7500000), 0.05); 

			vec3 p5 = p4;
			float d5 = d4;
			p5 -= vec3(0.0000000, 2.0000000, 0.0000000);
			p5 = mat3(0.0012099, -0.0012085, 0.9999985, 0.9999985, 0.0012099, -0.0012085, -0.0012085, 0.9999985, 0.0012099) * p5;
			d5 = boxFrame(p5, vec3(0.5000000, 0.2500000, 0.7500000), 0.05); 

			vec3 p6 = p5;
			float d6 = d5;
			p6 -= vec3(0.0000000, 2.0000000, 0.0000000);
			p6 = mat3(0.0012099, -0.0012085, 0.9999985, 0.9999985, 0.0012099, -0.0012085, -0.0012085, 0.9999985, 0.0012099) * p6;
			d6 = boxFrame(p6, vec3(0.5000000, 0.2500000, 0.7500000), 0.05); 

			vec3 p7 = p6;
			float d7 = d6;
			p7 -= vec3(0.0000000, 2.0000000, 0.0000000);
			p7 = mat3(0.0012099, -0.0012085, 0.9999985, 0.9999985, 0.0012099, -0.0012085, -0.0012085, 0.9999985, 0.0012099) * p7;
			d7 = boxFrame(p7, vec3(0.5000000, 0.2500000, 0.7500000), 0.05); 

			vec3 p8 = p7;
			float d8 = d7;
			p8 -= vec3(0.0000000, 2.0000000, 0.0000000);
			p8 = mat3(0.0012099, -0.0012085, 0.9999985, 0.9999985, 0.0012099, -0.0012085, -0.0012085, 0.9999985, 0.0012099) * p8;
			d8 = boxFrame(p8, vec3(0.5000000, 0.2500000, 0.7500000), 0.05); 

			vec3 p9 = p8;
			float d9 = d8;
			p9 -= vec3(0.0000000, 2.0000000, 0.0000000);
			p9 = mat3(0.0012099, -0.0012085, 0.9999985, 0.9999985, 0.0012099, -0.0012085, -0.0012085, 0.9999985, 0.0012099) * p9;
			d9 = boxFrame(p9, vec3(0.5000000, 0.2500000, 0.7500000), 0.05); 

			vec3 p10 = p9;
			float d10 = d9;
			p10 -= vec3(0.0000000, 2.0000000, 0.0000000);
			p10 = mat3(0.0012099, -0.0012085, 0.9999985, 0.9999985, 0.0012099, -0.0012085, -0.0012085, 0.9999985, 0.0012099) * p10;
			d10 = boxFrame(p10, vec3(0.5000000, 0.2500000, 0.7500000), 0.05); 

			vec3 p11 = p10;
			float d11 = d10;
			p11 -= vec3(0.0000000, 2.0000000, 0.0000000);
			p11 = mat3(0.0012099, -0.0012085, 0.9999985, 0.9999985, 0.0012099, -0.0012085, -0.0012085, 0.9999985, 0.0012099) * p11;
			d11 = boxFrame(p11, vec3(0.5000000, 0.2500000, 0.7500000), 0.05); 

			vec3 p12 = p11;
			float d12 = d11;
			p12 -= vec3(0.0000000, 2.0000000, 0.0000000);
			p12 = mat3(0.0012099, -0.0012085, 0.9999985, 0.9999985, 0.0012099, -0.0012085, -0.0012085, 0.9999985, 0.0012099) * p12;
			d12 = boxFrame(p12, vec3(0.5000000, 0.2500000, 0.7500000), 0.05); 

			vec3 p13 = p12;
			float d13 = d12;
			p13 -= vec3(0.0000000, 2.0000000, 0.0000000);
			p13 = mat3(0.0012099, -0.0012085, 0.9999985, 0.9999985, 0.0012099, -0.0012085, -0.0012085, 0.9999985, 0.0012099) * p13;
			d13 = boxFrame(p13, vec3(0.5000000, 0.2500000, 0.7500000), 0.05); 

			vec3 p14 = p13;
			float d14 = d13;
			p14 -= vec3(0.0000000, 2.0000000, 0.0000000);
			p14 = mat3(0.0012099, -0.0012085, 0.9999985, 0.9999985, 0.0012099, -0.0012085, -0.0012085, 0.9999985, 0.0012099) * p14;
			d14 = boxFrame(p14, vec3(0.5000000, 0.2500000, 0.7500000), 0.05); 

			vec3 p15 = p14;
			float d15 = d14;
			p15 -= vec3(0.0000000, 2.0000000, 0.0000000);
			p15 = mat3(0.0012099, -0.0012085, 0.9999985, 0.9999985, 0.0012099, -0.0012085, -0.0012085, 0.9999985, 0.0012099) * p15;
			d15 = boxFrame(p15, vec3(0.5000000, 0.2500000, 0.7500000), 0.05); 

			if (d15 < d14) d14 = d15;
			if (d14 < d13) d13 = d14;
			if (d13 < d12) d12 = d13;
			if (d12 < d11) d11 = d12;
			if (d11 < d10) d10 = d11;
			if (d10 < d9) d9 = d10;
			if (d9 < d8) d8 = d9;
			if (d8 < d7) d7 = d8;
			if (d7 < d6) d6 = d7;
			if (d6 < d5) d5 = d6;
			if (d5 < d4) d4 = d5;
			if (d4 < d3) d3 = d4;
			if (d3 < d2) d2 = d3;
			if (d2 < d1) d1 = d2;
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
            pixels[outIndex].l = 1.0;

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