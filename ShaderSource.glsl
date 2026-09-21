 
        #version 450
        layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

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

	        while (rayLength < push.maxDistance) {
                float d0 = push.maxDistance;
                vec3 p0 = camPos + rayDir * rayLength;

 //// START DYNAMICALLY GENERATED CODE ////

p0 -= vec3(0.0000000, 0.0000000, 0.0000000);
vec3 p1 = p0;
float d1 = d0;
p1 -= vec3(0.0000000, 0.0000000, 1.0000000);
p1 = mat3(-0.3333333, 0.6666667, 0.6666667, 0.6666667, -0.3333333, 0.6666667, 0.6666667, 0.6666667, -0.3333333) * p1;
d1 = sphere(p1, 0.5); 
vec3 p2 = p1;
float d2 = d1;
p2 -= vec3(0.0000000, -1.0000000, 0.0000000);
p2 = mat3(-0.3333333, 0.6666667, 0.6666667, 0.6666667, -0.3333333, 0.6666667, 0.6666667, 0.6666667, -0.3333333) * p2;
d2 = sphere(p2, 0.5); 
if (d2 < d1) d1 = d2;
if (d1 < d0) d0 = d1;

//// END DYNAMICALLY GENERATED CODE ////

                if (d0 < push.surfTolerance) {
                    float dRatio = rayLength / push.maxDistance;
                    pixels[outIndex].l = dRatio;
                    float cVal = dRatio * (dRatio >= 0.99 ? 0.0 : 255.0);
                    pixels[outIndex].r = 0.0;
                    pixels[outIndex].g = 255.0;
                    pixels[outIndex].b = 0.0;
                    return;
                }
		        rayLength += d0;
	        }
        }
    