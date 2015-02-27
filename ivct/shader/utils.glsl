#define PI 3.14159265358979
#define PI_2 6.28318530717958

float saturate(float x)
{
	return clamp(x, 0.0, 1.0);
}
vec2 saturate(vec2 x)
{
	return clamp(x, vec2(0.0), vec2(1.0));
}
vec3 saturate(vec3 x)
{
	return clamp(x, vec3(0.0), vec3(1.0));
}
vec4 saturate(vec4 x)
{
	return clamp(x, vec4(0.0), vec4(1.0));
}

float GetLuminance(vec3 rgb)
{
	const vec3 W = vec3(0.212671, 0.715160, 0.072169);
	return dot(rgb, W);
}


// Expects: vec2(atan(normal.y, normal.x), normal.z), remapped to 16bit int
vec3 UnpackNormal16I(vec2 packedNormal)
{
	// Unpack from 16bit signed int.
	packedNormal.x *= PI / 32767.0;
	packedNormal.y *= 1.0 / 32767.0;

	float sinPhi = sqrt(1.0 - packedNormal.y*packedNormal.y);
	return normalize(vec3(cos(packedNormal.x)*sinPhi, sin(packedNormal.x)*sinPhi, packedNormal.y));
}

// Returns: vec2(atan(normal.y, normal.x), normal.z)
ivec2 PackNormal16I(vec3 normal)
{
	// Invalid implementation which tries to remove division by 0 with an epsilon:
	//const float ATAN_EPS = 0.0001;
	//return vec2(atan(normal.y, step(abs(normal.x), ATAN_EPS) * ATAN_EPS + normal.x), normal.z);

	// A mathematical correct and more robust alternative would be using 2 atan functions:
	// http://stackoverflow.com/questions/26070410/robust-atany-x-on-glsl-for-converting-xy-coordinate-to-angle
	//return vec2(PI/2 - mix(atan(normal.x, normal.y), atan(normal.y, normal.x), abs(normal.x) > abs(normal.y)), normal.z);

	// Alternative fix (selfmade):
	// When x is zero then cos(packedNormal0)==0. So packedNormal0 may be +-PI/2
	// dependent on the sign of y.
	float packedNormal0 = normal.x == 0.0 ? (sign(normal.y)*PI/2) : atan(normal.y, normal.x);
	float packedNormal1 = normal.z;

	// Pack to 16bit signed int.
	return ivec2(packedNormal0 * (32767.0 / PI), packedNormal1 * 32767.0);
}