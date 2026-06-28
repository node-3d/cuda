#ifndef UTIL_MATH_H
#define UTIL_MATH_H

// Reads an indexed integer reference, while preserving negative sentinel values.
inline __device__ int __refRead(int *raar, int i) {
	return i < 0 ? i : raar[i];
};


// Adds two int3 vectors component-wise.
inline __host__ __device__ int3 operator + (int3 a, int3 b) {
	return make_int3(a.x + b.x, a.y + b.y, a.z + b.z);
}


// Negates both components of an int2 vector.
inline __host__ __device__ int2 operator - (int2 &a) {
	return make_int2(-a.x, -a.y);
}

// Adds two int2 vectors component-wise.
inline __host__ __device__ int2 operator + (int2 a, int2 b) {
	return make_int2(a.x + b.x, a.y + b.y);
}

// Adds another int2 vector into this int2 vector in place.
inline __host__ __device__ void operator += (int2 &a, int2 b) {
	a.x += b.x;
	a.y += b.y;
}

// Subtracts two int2 vectors component-wise.
inline __host__ __device__ int2 operator - (int2 a, int2 b) {
	return make_int2(a.x - b.x, a.y - b.y);
}

// Subtracts another int2 vector from this int2 vector in place.
inline __host__ __device__ void operator -= (int2 &a, int2 b) {
	a.x -= b.x;
	a.y -= b.y;
}

// Multiplies two int2 vectors component-wise.
inline __host__ __device__ int2 operator * (int2 a, int2 b) {
	return make_int2(a.x * b.x, a.y * b.y);
}

// Scales an int2 vector by an integer scalar.
inline __host__ __device__ int2 operator * (int2 a, int s) {
	return make_int2(a.x * s, a.y * s);
}

// Scales an int2 vector by an integer scalar with scalar-first syntax.
inline __host__ __device__ int2 operator * (int s, int2 a) {
	return make_int2(a.x * s, a.y * s);
}

// Scales an int2 vector by an integer scalar in place.
inline __host__ __device__ void operator *= (int2 &a, int s) {
	a.x *= s;
	a.y *= s;
}

// float2 functions
////////////////////////////////////////////////////////////////////////////////

// Creates a float2 with both components set to the same scalar.
inline __host__ __device__ float2 make_float2(float s) {
	return make_float2(s, s);
}

// Converts an int2 vector to a float2 vector.
inline __host__ __device__ float2 make_float2(int2 a) {
	return make_float2(float(a.x), float(a.y));
}

// Negates both components of a float2 vector.
inline __host__ __device__ float2 operator - (float2 &a) {
	return make_float2(-a.x, -a.y);
}

// Adds two float2 vectors component-wise.
inline __host__ __device__ float2 operator + (float2 a, float2 b) {
	return make_float2(a.x + b.x, a.y + b.y);
}

// Adds another float2 vector into this float2 vector in place.
inline __host__ __device__ void operator += (float2 &a, float2 b) {
	a.x += b.x;
	a.y += b.y;
}

// Subtracts two float2 vectors component-wise.
inline __host__ __device__ float2 operator - (float2 a, float2 b) {
	return make_float2(a.x - b.x, a.y - b.y);
}

// Subtracts another float2 vector from this float2 vector in place.
inline __host__ __device__ void operator -= (float2 &a, float2 b) {
	a.x -= b.x;
	a.y -= b.y;
}

// Multiplies two float2 vectors component-wise.
inline __host__ __device__ float2 operator * (float2 a, float2 b) {
	return make_float2(a.x * b.x, a.y * b.y);
}

// Scales a float2 vector by a scalar.
inline __host__ __device__ float2 operator * (float2 a, float s) {
	return make_float2(a.x * s, a.y * s);
}

// Scales a float2 vector by a scalar with scalar-first syntax.
inline __host__ __device__ float2 operator * (float s, float2 a) {
	return make_float2(a.x * s, a.y * s);
}

// Scales a float2 vector by a scalar in place.
inline __host__ __device__ void operator *= (float2 &a, float s) {
	a.x *= s;
	a.y *= s;
}

// Divides two float2 vectors component-wise.
inline __host__ __device__ float2 operator / (float2 a, float2 b) {
	return make_float2(a.x / b.x, a.y / b.y);
}

// Divides a float2 vector by a scalar.
inline __host__ __device__ float2 operator / (float2 a, float s) {
	float inv = 1.0f / s;
	return a * inv;
}

// Legacy scalar/vector division overload; scales the vector by the scalar inverse.
inline __host__ __device__ float2 operator / (float s, float2 a) {
	float inv = 1.0f / s;
	return a * inv;
}

// Divides a float2 vector by a scalar in place.
inline __host__ __device__ void operator /= (float2 &a, float s) {
	float inv = 1.0f / s;
	a *= inv;
}

// Computes the dot product of two float2 vectors.
inline __host__ __device__ float dot(float2 a, float2 b) {
	return a.x * b.x + a.y * b.y;
}

// Computes the Euclidean length of a float2 vector.
inline __host__ __device__ float length(float2 v) {
	return sqrtf(dot(v, v));
}

// float3 functions
////////////////////////////////////////////////////////////////////////////////

// Creates a float3 with all components set to the same scalar.
inline __host__ __device__ float3 make_float3(float s) {
	return make_float3(s, s, s);
}

// Converts a float2 vector to float3 with z set to zero.
inline __host__ __device__ float3 make_float3(float2 a) {
	return make_float3(a.x, a.y, 0.0f);
}

// Converts a float2 vector to float3 with an explicit z component.
inline __host__ __device__ float3 make_float3(float2 a, float s) {
	return make_float3(a.x, a.y, s);
}

// Converts a float4 vector to float3 by discarding the w component.
inline __host__ __device__ float3 make_float3(float4 a) {
	return make_float3(a.x, a.y, a.z);
}

// Converts an int3 vector to a float3 vector.
inline __host__ __device__ float3 make_float3(int3 a) {
	return make_float3(float(a.x), float(a.y), float(a.z));
}

// Negates all components of a float3 vector.
inline __host__ __device__ float3 operator - (float3 &a) {
	return make_float3(-a.x, -a.y, -a.z);
}

// Adds two float3 vectors component-wise.
inline __host__ __device__ float3 operator + (float3 a, float3 b) {
	return make_float3(a.x + b.x, a.y + b.y, a.z + b.z);
}

// Adds a scalar to each component of a float3 vector.
inline __host__ __device__ float3 operator + (float3 a, float b) {
	return make_float3(a.x + b, a.y + b, a.z + b);
}

// Adds another float3 vector into this float3 vector in place.
inline __host__ __device__ void operator += (float3 &a, float3 b) {
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
}


// Adds two float4 vectors component-wise.
inline __host__ __device__ float4 operator + (float4 a, float4 b) {
	return make_float4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}


// Adds another float4 vector into this float4 vector in place.
inline __host__ __device__ void operator += (float4 &a, float4 b) {
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	a.w += b.w;
}


// Subtracts two float3 vectors component-wise.
inline __host__ __device__ float3 operator - (float3 a, float3 b) {
	return make_float3(a.x - b.x, a.y - b.y, a.z - b.z);
}

// Subtracts a scalar from each component of a float3 vector.
inline __host__ __device__ float3 operator - (float3 a, float b) {
	return make_float3(a.x - b, a.y - b, a.z - b);
}

// Subtracts another float3 vector from this float3 vector in place.
inline __host__ __device__ void operator -= (float3 &a, float3 b) {
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
}

// Multiplies two float3 vectors component-wise.
inline __host__ __device__ float3 operator * (float3 a, float3 b) {
	return make_float3(a.x * b.x, a.y * b.y, a.z * b.z);
}

// Scales a float3 vector by a scalar.
inline __host__ __device__ float3 operator * (float3 a, float s) {
	return make_float3(a.x * s, a.y * s, a.z * s);
}

// Scales a float3 vector by a scalar with scalar-first syntax.
inline __host__ __device__ float3 operator * (float s, float3 a) {
	return make_float3(a.x * s, a.y * s, a.z * s);
}

// Scales a float3 vector by a scalar in place.
inline __host__ __device__ void operator *= (float3 &a, float s) {
	a.x *= s;
	a.y *= s;
	a.z *= s;
}


// Multiplies two float4 vectors component-wise.
inline __host__ __device__ float4 operator * (float4 a, float4 b) {
	return make_float4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}

// Scales a float4 vector by a scalar.
inline __host__ __device__ float4 operator * (float4 a, float s) {
	return make_float4(a.x * s, a.y * s, a.z * s, a.w * s);
}

// Scales a float4 vector by a scalar with scalar-first syntax.
inline __host__ __device__ float4 operator * (float s, float4 a) {
	return make_float4(a.x * s, a.y * s, a.z * s, a.w * s);
}

// Scales a float4 vector by a scalar in place.
inline __host__ __device__ void operator *= (float4 &a, float s) {
	a.x *= s;
	a.y *= s;
	a.z *= s;
	a.w *= s;
}

// Divides two float3 vectors component-wise.
inline __host__ __device__ float3 operator / (float3 a, float3 b) {
	return make_float3(a.x / b.x, a.y / b.y, a.z / b.z);
}

// Divides a float3 vector by a scalar.
inline __host__ __device__ float3 operator / (float3 a, float s) {
	float inv = 1.0f / s;
	return a * inv;
}

// Legacy scalar/vector division overload;
scales the vector by the scalar inverse.
inline __host__ __device__ float3 operator / (float s, float3 a) {
	float inv = 1.0f / s;
	return a * inv;
}

// Divides a float3 vector by a scalar in place.
inline __host__ __device__ void operator /= (float3 &a, float s) {
	float inv = 1.0f / s;
	a *= inv;
}

// Computes the dot product of two float3 vectors.
inline __host__ __device__ float dot(float3 a, float3 b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

// Computes the Euclidean length of a float3 vector.
inline __host__ __device__ float length(float3 v) {
	return sqrtf(dot(v, v));
}

// Atomically adds a float value to a device float address.
inline __device__ float atomicAddFloat(float *address, float val) {
	return atomicAdd(address, val);
}

// Atomically adds each component of a float2 value to a device float2 address.
inline __device__ void atomicAddFloat2(float2 *a, float2 b) {
	atomicAddFloat(&(a->x), b.x);
	atomicAddFloat(&(a->y), b.y);
}

// Atomically adds each component of a float4 value to a device float4 address.
inline __device__ void atomicAddFloat4(float4 *a, float4 b) {
	atomicAddFloat(&(a->x), b.x);
	atomicAddFloat(&(a->y), b.y);
	atomicAddFloat(&(a->z), b.z);
	atomicAddFloat(&(a->w), b.w);
}

// Atomically adds each component of an int2 value to a device int2 address.
inline __device__ void atomicAdd2(int2 *a, int2 b) {
	atomicAdd(&(a->x), b.x);
	atomicAdd(&(a->y), b.y);
}

// Atomically adds each component of an int4 value to a device int4 address.
inline __device__ void atomicAdd4(int4 *a, int4 b) {
	atomicAdd(&(a->x), b.x);
	atomicAdd(&(a->y), b.y);
	atomicAdd(&(a->z), b.z);
	atomicAdd(&(a->w), b.w);
}

// Returns true when all float3 components are finite.
inline __host__ __device__ bool isfinite(float3 a) {
	return isfinite(a.x) && isfinite(a.y) && isfinite(a.z);
}

// Returns true when all float2 components are finite.
inline __host__ __device__ bool isfinite(float2 a) {
	return isfinite(a.x) && isfinite(a.y);
}

// Builds one triangular color-channel intensity from a hue phase offset.
inline __device__ float p(float d,float t) {
	float v;
	
	v = 2.0f - abs(d - t) * 6.0f;
	v = v < -2.0f ? -2.0f - v : v;
	v = v < 1.0f ? v>0.0f ? v : 0.0f: 1.0f;
	
	return v;
};


// Converts a normalized hue value to an RGBA color with alpha set to one.
inline __device__ float4 t2rgb(float t) {
	float4 rgb;
	
	t -= floor(t);
	
	rgb.x = p(3.0f / 3.0f, t);
	rgb.y = p(1.0f / 3.0f, t);
	rgb.z = p(2.0f / 3.0f, t);
	rgb.w = 1;
	
	return rgb;
}

// Converts hue plus whiteness to an RGBA color, with alpha set to one.
inline __device__ float4 tw2rgbw(float t,float w) {
	float4 rgb;
	
	t -= floor(t);
	
	rgb.x = p(3.0f / 3.0f, t);
	rgb.y = p(1.0f / 3.0f, t);
	rgb.z = p(2.0f / 3.0f, t);
	
	if (w > 1) w=1;
	
	rgb.x += (1 - rgb.x) * w;
	rgb.y += (1 - rgb.y) * w;
	rgb.z += (1 - rgb.z) * w;
	rgb.w = 1;
	
	return rgb;
}

// Converts hue plus whiteness to an RGBA color; shorter alias for tw2rgbw.
inline __device__ float4 tw2rgb(float t,float w) {
	float4 rgb;
	
	t -= floor(t);
	
	rgb.x = p(3.0f / 3.0f, t);
	rgb.y = p(1.0f / 3.0f, t);
	rgb.z = p(2.0f / 3.0f, t);
	
	if (w > 1) w=1;
	
	rgb.x += (1 - rgb.x) * w;
	rgb.y += (1 - rgb.y) * w;
	rgb.z += (1 - rgb.z) * w;
	
	rgb.w = 1;
	
	return rgb;
}


// Mixes an existing RGB color toward white by the given whiteness amount.
inline __device__ float4 rgbw2rgb(float4 rgb, float w) {
	if (w > 1) w=1;
	
	rgb.x += (1 - rgb.x) * w;
	rgb.y += (1 - rgb.y) * w;
	rgb.z += (1 - rgb.z) * w;
	
	return rgb;
};

// Converts center, radius, and normalized turn value to XY coordinates.
inline __device__ float2 crt2xy(float2 c, float r, float t) {
	float a = t * 2.0 * 3.14159265359;
	
	c.x += cos(a) * r;
	c.y += sin(a) * r;
	
	return c;
};

// Places an indexed point around a center using a golden-angle spiral.
inline __device__ float2 cri2xy(float2 c, float rr, int i) {
	float2 rv = { 0, 0 };
	
	if (i == 0) {
		rv = c;
		rv.x += rr;
		return rv;
	};
	
	// Golden ratio conjugate used as a stable angular increment.
	float Fi = (sqrtf(5) - 1) / 2;
	
	float r = sqrtf(i) * Fi * rr * 2;
	return crt2xy(c, r, Fi * i);
}

// Converts radius and normalized turn value to XY coordinates around the origin.
inline __device__ float2 rt2xy(float r, float t) {
	float a = t * 2 * 3.14159265359;
	
	float2 c = { cos(a) * r, sin(a) * r };
	
	return c;
};

// Converts a direction vector to a normalized turn value in cycles.
inline __device__ float atan2t(float2 dpos) {
	return atan2(dpos.y, dpos.x) / 2 / 3.14159265359;
};

// Computes the normalized turn angle from one point to another.
inline __device__ float f2t(float2 f, float2 t) {
	return atan2t(t - f);
};

// Places an indexed point on a golden-angle spiral around the origin.
inline __device__ float2 ri2xy(float rr, int i) {
	float2 rv = { 0, 0 };
	
	if (i == 0) {
		rv.x += rr;
		return rv;
	};
	
	// Golden ratio conjugate used as a stable angular increment.
	// The explicit double constant preserves large-index angular precision.
	double Fi = 0.6180339887498948482;
	float r = sqrtf(i) * Fi * rr * 2;
	
	double a = Fi * i;
	// Keep angle reduction in double precision before passing it to rt2xy.
	a -= floor(a);
	
	return rt2xy(r, a);
}

#endif
