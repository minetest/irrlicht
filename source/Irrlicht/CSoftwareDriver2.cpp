// Copyright (C) 2002-2012 Nikolaus Gebhardt / Thomas Alten
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"
#include "CSoftwareDriver2.h"

#ifdef _IRR_COMPILE_WITH_BURNINGSVIDEO_

#include "SoftwareDriver2_helper.h"
#include "CSoftwareTexture.h"
#include "CSoftwareTexture2.h"
#include "CSoftware2MaterialRenderer.h"
#include "S3DVertex.h"
#include "S4DVertex.h"
#include "CBlit.h"


// Matrix now here

template <class T>
bool mat33_transposed_inverse(irr::core::CMatrix4<T>& out, const irr::core::CMatrix4<T>& M)
{
	const T* burning_restrict m = M.pointer();

	double d =
		(m[0] * m[5] - m[1] * m[4]) * (m[10] * m[15] - m[11] * m[14]) -
		(m[0] * m[6] - m[2] * m[4]) * (m[9] * m[15] - m[11] * m[13]) +
		(m[0] * m[7] - m[3] * m[4]) * (m[9] * m[14] - m[10] * m[13]) +
		(m[1] * m[6] - m[2] * m[5]) * (m[8] * m[15] - m[11] * m[12]) -
		(m[1] * m[7] - m[3] * m[5]) * (m[8] * m[14] - m[10] * m[12]) +
		(m[2] * m[7] - m[3] * m[6]) * (m[8] * m[13] - m[9] * m[12]);

	if (fabs(d) < DBL_MIN)
	{
		out.makeIdentity();
		return false;
	}

	d = 1.0 / d;
	T* burning_restrict o = out.pointer();
	o[0] = (T)(d * (m[5] * (m[10] * m[15] - m[11] * m[14]) + m[6] * (m[11] * m[13] - m[9] * m[15]) + m[7] * (m[9] * m[14] - m[10] * m[13])));
	o[4] = (T)(d * (m[9] * (m[2] * m[15] - m[3] * m[14]) + m[10] * (m[3] * m[13] - m[1] * m[15]) + m[11] * (m[1] * m[14] - m[2] * m[13])));
	o[8] = (T)(d * (m[13] * (m[2] * m[7] - m[3] * m[6]) + m[14] * (m[3] * m[5] - m[1] * m[7]) + m[15] * (m[1] * m[6] - m[2] * m[5])));
	o[12] = 0.f;

	o[1] = (T)(d * (m[6] * (m[8] * m[15] - m[11] * m[12]) + m[7] * (m[10] * m[12] - m[8] * m[14]) + m[4] * (m[11] * m[14] - m[10] * m[15])));
	o[5] = (T)(d * (m[10] * (m[0] * m[15] - m[3] * m[12]) + m[11] * (m[2] * m[12] - m[0] * m[14]) + m[8] * (m[3] * m[14] - m[2] * m[15])));
	o[9] = (T)(d * (m[14] * (m[0] * m[7] - m[3] * m[4]) + m[15] * (m[2] * m[4] - m[0] * m[6]) + m[12] * (m[3] * m[6] - m[2] * m[7])));
	o[13] = 0.f;

	o[2] = (T)(d * (m[7] * (m[8] * m[13] - m[9] * m[12]) + m[4] * (m[9] * m[15] - m[11] * m[13]) + m[5] * (m[11] * m[12] - m[8] * m[15])));
	o[6] = (T)(d * (m[11] * (m[0] * m[13] - m[1] * m[12]) + m[8] * (m[1] * m[15] - m[3] * m[13]) + m[9] * (m[3] * m[12] - m[0] * m[15])));
	o[10] = (T)(d * (m[15] * (m[0] * m[5] - m[1] * m[4]) + m[12] * (m[1] * m[7] - m[3] * m[5]) + m[13] * (m[3] * m[4] - m[0] * m[7])));
	o[14] = 0.f;

	o[3] = 0.f;
	o[7] = 0.f;
	o[11] = 0.f;
	o[15] = 1.f;

	return true;
}

#if 0
template <class T>
bool mat44_transposed_inverse(irr::core::CMatrix4<T>& out, const irr::core::CMatrix4<T>& M)
{
	const T* burning_restrict m = M.pointer();

	double d =
		(m[0] * m[5] - m[1] * m[4]) * (m[10] * m[15] - m[11] * m[14]) -
		(m[0] * m[6] - m[2] * m[4]) * (m[9] * m[15] - m[11] * m[13]) +
		(m[0] * m[7] - m[3] * m[4]) * (m[9] * m[14] - m[10] * m[13]) +
		(m[1] * m[6] - m[2] * m[5]) * (m[8] * m[15] - m[11] * m[12]) -
		(m[1] * m[7] - m[3] * m[5]) * (m[8] * m[14] - m[10] * m[12]) +
		(m[2] * m[7] - m[3] * m[6]) * (m[8] * m[13] - m[9] * m[12]);

	if (fabs(d) < DBL_MIN)
	{
		out.makeIdentity();
		return false;
	}

	d = 1.0 / d;
	T* burning_restrict o = out.pointer();
	o[0] = (T)(d * (m[5] * (m[10] * m[15] - m[11] * m[14]) + m[6] * (m[11] * m[13] - m[9] * m[15]) + m[7] * (m[9] * m[14] - m[10] * m[13])));
	o[4] = (T)(d * (m[9] * (m[2] * m[15] - m[3] * m[14]) + m[10] * (m[3] * m[13] - m[1] * m[15]) + m[11] * (m[1] * m[14] - m[2] * m[13])));
	o[8] = (T)(d * (m[13] * (m[2] * m[7] - m[3] * m[6]) + m[14] * (m[3] * m[5] - m[1] * m[7]) + m[15] * (m[1] * m[6] - m[2] * m[5])));
	o[12] = (T)(d * (m[1] * (m[7] * m[10] - m[6] * m[11]) + m[2] * (m[5] * m[11] - m[7] * m[9]) + m[3] * (m[6] * m[9] - m[5] * m[10])));

	o[1] = (T)(d * (m[6] * (m[8] * m[15] - m[11] * m[12]) + m[7] * (m[10] * m[12] - m[8] * m[14]) + m[4] * (m[11] * m[14] - m[10] * m[15])));
	o[5] = (T)(d * (m[10] * (m[0] * m[15] - m[3] * m[12]) + m[11] * (m[2] * m[12] - m[0] * m[14]) + m[8] * (m[3] * m[14] - m[2] * m[15])));
	o[9] = (T)(d * (m[14] * (m[0] * m[7] - m[3] * m[4]) + m[15] * (m[2] * m[4] - m[0] * m[6]) + m[12] * (m[3] * m[6] - m[2] * m[7])));
	o[13] = (T)(d * (m[2] * (m[7] * m[8] - m[4] * m[11]) + m[3] * (m[4] * m[10] - m[6] * m[8]) + m[0] * (m[6] * m[11] - m[7] * m[10])));

	o[2] = (T)(d * (m[7] * (m[8] * m[13] - m[9] * m[12]) + m[4] * (m[9] * m[15] - m[11] * m[13]) + m[5] * (m[11] * m[12] - m[8] * m[15])));
	o[6] = (T)(d * (m[11] * (m[0] * m[13] - m[1] * m[12]) + m[8] * (m[1] * m[15] - m[3] * m[13]) + m[9] * (m[3] * m[12] - m[0] * m[15])));
	o[10] = (T)(d * (m[15] * (m[0] * m[5] - m[1] * m[4]) + m[12] * (m[1] * m[7] - m[3] * m[5]) + m[13] * (m[3] * m[4] - m[0] * m[7])));
	o[14] = (T)(d * (m[3] * (m[5] * m[8] - m[4] * m[9]) + m[0] * (m[7] * m[9] - m[5] * m[11]) + m[1] * (m[4] * m[11] - m[7] * m[8])));

	o[3] = (T)(d * (m[4] * (m[10] * m[13] - m[9] * m[14]) + m[5] * (m[8] * m[14] - m[10] * m[12]) + m[6] * (m[9] * m[12] - m[8] * m[13])));
	o[7] = (T)(d * (m[8] * (m[2] * m[13] - m[1] * m[14]) + m[9] * (m[0] * m[14] - m[2] * m[12]) + m[10] * (m[1] * m[12] - m[0] * m[13])));
	o[11] = (T)(d * (m[12] * (m[2] * m[5] - m[1] * m[6]) + m[13] * (m[0] * m[6] - m[2] * m[4]) + m[14] * (m[1] * m[4] - m[0] * m[5])));
	o[15] = (T)(d * (m[0] * (m[5] * m[10] - m[6] * m[9]) + m[1] * (m[6] * m[8] - m[4] * m[10]) + m[2] * (m[4] * m[9] - m[5] * m[8])));

	return true;
}
#endif

// difference to CMatrix4<T>::getInverse . higher precision in determinant. return identity on failure
template <class T>
bool mat44_inverse(irr::core::CMatrix4<T>& out, const irr::core::CMatrix4<T>& M)
{
	const T* burning_restrict m = M.pointer();

	double d =
		(m[0] * m[5] - m[1] * m[4]) * (m[10] * m[15] - m[11] * m[14]) -
		(m[0] * m[6] - m[2] * m[4]) * (m[9] * m[15] - m[11] * m[13]) +
		(m[0] * m[7] - m[3] * m[4]) * (m[9] * m[14] - m[10] * m[13]) +
		(m[1] * m[6] - m[2] * m[5]) * (m[8] * m[15] - m[11] * m[12]) -
		(m[1] * m[7] - m[3] * m[5]) * (m[8] * m[14] - m[10] * m[12]) +
		(m[2] * m[7] - m[3] * m[6]) * (m[8] * m[13] - m[9] * m[12]);

	if (fabs(d) < DBL_MIN)
	{
		out.makeIdentity();
		return false;
	}

	d = 1.0 / d;
	T* burning_restrict o = out.pointer();
	o[0] = (T)(d * (m[5] * (m[10] * m[15] - m[11] * m[14]) + m[6] * (m[11] * m[13] - m[9] * m[15]) + m[7] * (m[9] * m[14] - m[10] * m[13])));
	o[1] = (T)(d * (m[9] * (m[2] * m[15] - m[3] * m[14]) + m[10] * (m[3] * m[13] - m[1] * m[15]) + m[11] * (m[1] * m[14] - m[2] * m[13])));
	o[2] = (T)(d * (m[13] * (m[2] * m[7] - m[3] * m[6]) + m[14] * (m[3] * m[5] - m[1] * m[7]) + m[15] * (m[1] * m[6] - m[2] * m[5])));
	o[3] = (T)(d * (m[1] * (m[7] * m[10] - m[6] * m[11]) + m[2] * (m[5] * m[11] - m[7] * m[9]) + m[3] * (m[6] * m[9] - m[5] * m[10])));

	o[4] = (T)(d * (m[6] * (m[8] * m[15] - m[11] * m[12]) + m[7] * (m[10] * m[12] - m[8] * m[14]) + m[4] * (m[11] * m[14] - m[10] * m[15])));
	o[5] = (T)(d * (m[10] * (m[0] * m[15] - m[3] * m[12]) + m[11] * (m[2] * m[12] - m[0] * m[14]) + m[8] * (m[3] * m[14] - m[2] * m[15])));
	o[6] = (T)(d * (m[14] * (m[0] * m[7] - m[3] * m[4]) + m[15] * (m[2] * m[4] - m[0] * m[6]) + m[12] * (m[3] * m[6] - m[2] * m[7])));
	o[7] = (T)(d * (m[2] * (m[7] * m[8] - m[4] * m[11]) + m[3] * (m[4] * m[10] - m[6] * m[8]) + m[0] * (m[6] * m[11] - m[7] * m[10])));

	o[8] = (T)(d * (m[7] * (m[8] * m[13] - m[9] * m[12]) + m[4] * (m[9] * m[15] - m[11] * m[13]) + m[5] * (m[11] * m[12] - m[8] * m[15])));
	o[9] = (T)(d * (m[11] * (m[0] * m[13] - m[1] * m[12]) + m[8] * (m[1] * m[15] - m[3] * m[13]) + m[9] * (m[3] * m[12] - m[0] * m[15])));
	o[10] = (T)(d * (m[15] * (m[0] * m[5] - m[1] * m[4]) + m[12] * (m[1] * m[7] - m[3] * m[5]) + m[13] * (m[3] * m[4] - m[0] * m[7])));
	o[11] = (T)(d * (m[3] * (m[5] * m[8] - m[4] * m[9]) + m[0] * (m[7] * m[9] - m[5] * m[11]) + m[1] * (m[4] * m[11] - m[7] * m[8])));

	o[12] = (T)(d * (m[4] * (m[10] * m[13] - m[9] * m[14]) + m[5] * (m[8] * m[14] - m[10] * m[12]) + m[6] * (m[9] * m[12] - m[8] * m[13])));
	o[13] = (T)(d * (m[8] * (m[2] * m[13] - m[1] * m[14]) + m[9] * (m[0] * m[14] - m[2] * m[12]) + m[10] * (m[1] * m[12] - m[0] * m[13])));
	o[14] = (T)(d * (m[12] * (m[2] * m[5] - m[1] * m[6]) + m[13] * (m[0] * m[6] - m[2] * m[4]) + m[14] * (m[1] * m[4] - m[0] * m[5])));
	o[15] = (T)(d * (m[0] * (m[5] * m[10] - m[6] * m[9]) + m[1] * (m[6] * m[8] - m[4] * m[10]) + m[2] * (m[4] * m[9] - m[5] * m[8])));

	return true;
}


// void CMatrix4<T>::transformVec4(T *out, const T * in) const
template <class T>
inline void transformVec4Vec4(const irr::core::CMatrix4<T>& m, T* burning_restrict out, const T* burning_restrict in)
{
	const T* burning_restrict M = m.pointer();

	out[0] = in[0] * M[0] + in[1] * M[4] + in[2] * M[8] + in[3] * M[12];
	out[1] = in[0] * M[1] + in[1] * M[5] + in[2] * M[9] + in[3] * M[13];
	out[2] = in[0] * M[2] + in[1] * M[6] + in[2] * M[10] + in[3] * M[14];
	out[3] = in[0] * M[3] + in[1] * M[7] + in[2] * M[11] + in[3] * M[15];
}

template <class T>
inline void transformVec3Vec3(const irr::core::CMatrix4<T>& m, T* burning_restrict out, const T* burning_restrict in)
{
	const T* burning_restrict M = m.pointer();
	out[0] = in[0] * M[0] + in[1] * M[4] + in[2] * M[8] + M[12];
	out[1] = in[0] * M[1] + in[1] * M[5] + in[2] * M[9] + M[13];
	out[2] = in[0] * M[2] + in[1] * M[6] + in[2] * M[10] + M[14];
}

#if 0
// void CMatrix4<T>::transformVect(T *out, const core::vector3df &in) const
template <class T>
inline void transformVec3Vec4(const irr::core::CMatrix4<T>& m, T* burning_restrict out, const core::vector3df& in)
{
	const T* burning_restrict M = m.pointer();
	out[0] = in.X * M[0] + in.Y * M[4] + in.Z * M[8] + M[12];
	out[1] = in.X * M[1] + in.Y * M[5] + in.Z * M[9] + M[13];
	out[2] = in.X * M[2] + in.Y * M[6] + in.Z * M[10] + M[14];
	out[3] = in.X * M[3] + in.Y * M[7] + in.Z * M[11] + M[15];
}
#endif

template <class T>
inline void rotateMat44Vec3Vec4(const irr::core::CMatrix4<T>& m, T* burning_restrict out, const T* burning_restrict in)
{
	const T* burning_restrict M = m.pointer();

	out[0] = in[0] * M[0] + in[1] * M[4] + in[2] * M[8];
	out[1] = in[0] * M[1] + in[1] * M[5] + in[2] * M[9];
	out[2] = in[0] * M[2] + in[1] * M[6] + in[2] * M[10];
	out[3] = in[0] * M[3] + in[1] * M[7] + in[2] * M[11];
	//out[3] = 0.f;
}

template <class T>
inline void rotateMat33Vec3Vec4(const irr::core::CMatrix4<T>& m, T* burning_restrict out, const T* burning_restrict in)
{
	const T* burning_restrict M = m.pointer();

	out[0] = in[0] * M[0] + in[1] * M[4] + in[2] * M[8];
	out[1] = in[0] * M[1] + in[1] * M[5] + in[2] * M[9];
	out[2] = in[0] * M[2] + in[1] * M[6] + in[2] * M[10];
	out[3] = 0.f; //in[0] * M[3] + in[1] * M[7] + in[2] * M[11];
}

#if 0
template <class T>
irr::video::sVec4 operator* (const irr::core::CMatrix4<T>& m, const irr::core::vector3df& in)
{
	const T* burning_restrict M = m.pointer();
	return irr::video::sVec4(
		in.X * M[0] + in.Y * M[4] + in.Z * M[8] + M[12],
		in.X * M[1] + in.Y * M[5] + in.Z * M[9] + M[13],
		in.X * M[2] + in.Y * M[6] + in.Z * M[10] + M[14],
		in.X * M[3] + in.Y * M[7] + in.Z * M[11] + M[15]);
}

template <class T>
irr::video::sVec4 operator* (const irr::core::vector3df& in, const irr::core::CMatrix4<T>& m)
{
	const T* burning_restrict M = m.pointer();
	return irr::video::sVec4(
		in.X * M[0] + in.Y * M[1] + in.Z * M[2] + M[3],
		in.X * M[4] + in.Y * M[5] + in.Z * M[6] + M[7],
		in.X * M[8] + in.Y * M[9] + in.Z * M[10] + M[11],
		in.X * M[12] + in.Y * M[13] + in.Z * M[14] + M[15]);
}
#endif

template <class T>
irr::video::sVec4 operator* (const irr::core::CMatrix4<T>& m, const irr::video::sVec4& v)
{
	const T* burning_restrict M = m.pointer();
	const float* burning_restrict in = &v.x;
	return irr::video::sVec4(
		in[0] * M[0] + in[1] * M[4] + in[2] * M[8] + in[3] * M[12],
		in[0] * M[1] + in[1] * M[5] + in[2] * M[9] + in[3] * M[13],
		in[0] * M[2] + in[1] * M[6] + in[2] * M[10] + in[3] * M[14],
		in[0] * M[3] + in[1] * M[7] + in[2] * M[11] + in[3] * M[15]);
}

template <class T>
irr::video::sVec4 operator* (const irr::video::sVec4& v, const irr::core::CMatrix4<T>& m)
{
	const T* burning_restrict M = m.pointer();
	const float* burning_restrict in = &v.x;
	return irr::video::sVec4(
		in[0] * M[0] + in[1] * M[1] + in[2] * M[2] + in[3] * M[3],
		in[0] * M[4] + in[1] * M[5] + in[2] * M[6] + in[3] * M[7],
		in[0] * M[8] + in[1] * M[9] + in[2] * M[10] + in[3] * M[11],
		in[0] * M[12] + in[1] * M[13] + in[2] * M[14] + in[3] * M[15]);
}

static inline float dot(const irr::video::sVec4& a, const irr::video::sVec4& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

static inline float inversesqrt(const float x )
{
	return x!= 0.f ? 1.f / sqrtf(x) : 0.f;
}


static inline irr::video::sVec4 operator-(const irr::video::sVec4& a)
{
	return irr::video::sVec4(-a.x, -a.y, -a.z, -a.w);
}

static inline irr::video::sVec4 normalize(const irr::video::sVec4& a)
{
	float l = a.x * a.x + a.y * a.y + a.z * a.z + a.w * a.w;
	if (l < 0.00000001f)
		return irr::video::sVec4(0.f, 0.f, 1.f, 1.f);
	l = 1.f / sqrtf(l);
	return irr::video::sVec4(a.x * l, a.y * l, a.z * l, a.w * l);
}

// sVec3 xyz
static inline irr::video::sVec4 cross(const irr::video::sVec4& a, const irr::video::sVec4& b)
{
	return irr::video::sVec4(a.y * b.z - b.y * a.z, a.z * b.x - b.z * a.x, a.x * b.y - b.x * a.y, 0.f);
}


void irr::video::sVec4::setA8R8G8B8(const u32 argb)
{
	//error term necessary. cancels out(somehow) at 255 argb((tofixpoint(r/w)+fix_0.5)
	static const f32 is = 1.f / (255.f);
	r = ((argb & 0x00FF0000) >> 16) * is;
	g = ((argb & 0x0000FF00) >> 8) * is;
	b = ((argb & 0x000000FF)) * is;
	a = ((argb & 0xFF000000) >> 24) * is;
}


//need to prevent floating point over/underflow
//based on https://github.com/ekmett/approximate/blob/master/cbits/fast.c powf_fast_precise
static inline float powf_limit(const float a, const float b)
{
	if (a < 0.00000001f)
		return 0.f;
	else if (a >= 1.f)
		return a * b;

	/* calculate approximation with fraction of the exponent */
	int e = (int)b;
	union { float f; int x; } u = { a };
	u.x = (int)((b - e) * (u.x - 1065353216) + 1065353216);

	float r = 1.0f;
	float ua = a;
	while (e) {
		if (e & 1) {
			r *= ua;
		}
		if (ua < 0.000000001f)
			return 0.f;
		ua *= ua;
		e >>= 1;
	}

	r *= u.f;
	return r;
}


/*
	if (condition) state |= m; else state &= ~m;
*/
REALINLINE void burning_setbit32(unsigned int& state, int condition, const unsigned int mask)
{
	// 0, or any positive to mask
	//s32 conmask = -condition >> 31;
	state ^= ((-condition >> 31) ^ state) & mask;
}

/*
	if (condition) state |= mask; else state &= ~mask;
*/
static inline void burning_setbit(size_t& state, int condition, size_t mask)
{
	if (condition) state |= mask;
	else state &= ~mask;
}


// IImage::fill
static void image_fill(irr::video::IImage* image, const irr::video::SColor& color, const interlaced_control interlaced)
{
	if (0 == image)
		return;

	unsigned int c = color.color;

	switch (image->getColorFormat())
	{
	case irr::video::ECF_A1R5G5B5:
		c = color.toA1R5G5B5();
		c |= c << 16;
		break;
	default:
		break;
	}
	irr::memset32_interlaced(image->getData(), c, image->getPitch(), image->getDimension().Height, interlaced);
}



//setup Antialias. v0.52 uses as Interlaced
void get_scale(interlaced_control& o, const irr::SIrrlichtCreationParameters& params)
{
	o.raw = 0;
	o.bypass = 1;
#if !defined(SOFTWARE_DRIVER_2_RENDERTARGET_SCALE)
	return;
#endif

	//test case
	if (0 || params.WindowSize.Width <= 160 || params.WindowSize.Height <= 128)
	{
		return;
	}

	union scale_setup
	{
		struct
		{
			unsigned char x : 3;
			unsigned char y : 3;
			unsigned char i : 2;
		};
		unsigned char v;
	};

	scale_setup s;
	s.x = 1;
	s.y = 1;
	s.i = 0;

	switch (params.AntiAlias)
	{
	default:
	case 0:	s.x = 1; s.y = 1; s.i = 0; break;
	case 2:	s.x = 1; s.y = 1; s.i = 1; break;
	case 4: s.x = 2; s.y = 2; s.i = 0; break;
	case 8: s.x = 2; s.y = 2; s.i = params.Vsync ? 1 : 0; break;
	case 16:s.x = 4; s.y = 4; s.i = 0; break;
	case 32:s.x = 4; s.y = 4; s.i = 1; break;

	case 3:	s.x = 3; s.y = 3; s.i = 0; break;
	case 5:	s.x = 3; s.y = 3; s.i = 1; break;
	}

	/*
		if (params.WindowSize.Height > 384)
		{
			s.i = params.Vsync ? 0 : 1;
			s.x = params.AntiAlias ? 1 : 2;
			s.y = params.AntiAlias ? 1 : 2;
		}
	*/

	o.enable = s.i;
	o.target_scalex = s.x - 1;
	o.tex_scalex = 0; // s.x >= 2 ? s.x - 1 : 0;

#if defined(PATCH_SUPERTUX_8_0_1_with_1_9_0)
	o.enable = params.Vsync ? 0 : 1;

	switch (params.AntiAlias)
	{
	default:
	case 0:	o.target_scalex = 0; o.tex_scalex = 0; break;
	case 2:	o.target_scalex = 1; o.tex_scalex = 0; break;
	case 4:	o.target_scalex = 1; o.tex_scalex = 1; break;
	case 8:	o.target_scalex = 2; o.tex_scalex = 1; break;
	}
#endif

	o.bypass = o.enable == 0;
	o.nr = 0;
	o.target_scaley = o.target_scalex;
	o.tex_scaley = o.tex_scalex;

	if (o.enable || o.target_scalex || o.tex_scalex)
	{
		char buf[256];
		snprintf_irr(buf, sizeof(buf), "Burningvideo: Interlaced:%u,%u target:%u,%u tex:%u,%u",
			o.enable,
			o.bypass,
			o.target_scalex,
			o.target_scaley,
			o.tex_scalex,
			o.tex_scaley
		);
		irr::os::Printer::log(buf, irr::ELL_NONE);
	}
}

#if 0
//code snippets
#include <IrrlichtDevice.h>
#include <ICameraSceneNode.h>
#include <ISceneManager.h>
#include <EDriverTypes.h>
void switch_between_ortho_and_perspective_projection(irr::IrrlichtDevice* device, irr::video::E_DRIVER_TYPE driverType)
{
	//switch between ortho and perspective projection
	irr::scene::ICameraSceneNode* cam = device->getSceneManager()->addCameraSceneNode();
	cam->setPosition(irr::core::vector3df(300, 250, -300));
	cam->setTarget(irr::core::vector3df(0, 20, 0));
	if (1 || driverType != irr::video::EDT_BURNINGSVIDEO)
	{
		cam->setProjectionMatrix(irr::core::matrix4().buildProjectionMatrixOrthoLH(120, 90, 0.9f, 5000.f, driverType != irr::video::EDT_OPENGL), true);
	}
	else
	{
		irr::f32 w = (2.f * 0.9f) / (2.f / 120.f * (cam->getTarget() - cam->getPosition()).getLength());
		cam->setProjectionMatrix(irr::core::matrix4().buildProjectionMatrixPerspectiveLH(w, w * (90.f / 120.f), 0.9f, 5000.f, driverType != irr::video::EDT_OPENGL), true);
	}
}

/*
	For using an alternative camera in the examples.
	Try to translate the viewpoint (Maya internal CameraRotation)
*/
static inline void switchToMayaCamera(irr::IrrlichtDevice* device)
{
	if (!device) return;

	irr::scene::ICameraSceneNode* camera = device->getSceneManager()->getActiveCamera();
	if (!camera || camera->getID() == 54321) return;


	irr::core::vector3df target = camera->getTarget() - camera->getPosition();
	irr::core::vector3df relativeRotation = target.getHorizontalAngle();

	irr::scene::ICameraSceneNode* maya = device->getSceneManager()->addCameraSceneNodeMaya(
		0, -1500, 1000, 1500,
		54321,
		target.getLength(),
		true,
		relativeRotation.X + 90, relativeRotation.Y
	);
	if (maya)
	{
		maya->setNearValue(camera->getNearValue());
		maya->setFarValue(camera->getFarValue());
	}

	device->getCursorControl()->setVisible(true);
	device->setResizable(true);
}
#endif


//turn on/off fpu exception
void fpu_exception(int on)
{
	return;
#if defined(_WIN32)
	_clearfp();
	_controlfp(on ? _EM_INEXACT : -1, _MCW_EM);
#endif
}

burning_namespace_start

//! constructor
CBurningVideoDriver::CBurningVideoDriver(const irr::SIrrlichtCreationParameters& params, io::IFileSystem* io, video::IImagePresenter* presenter)
	: CNullDriver(io, params.WindowSize), BackBuffer(0), Presenter(presenter),
	WindowId(0), SceneSourceRect(0),
	RenderTargetTexture(0), RenderTargetSurface(0), CurrentShader(0),
	DepthBuffer(0), StencilBuffer(0)
{
	//enable fpu exception
	fpu_exception(1);

#ifdef _DEBUG
	setDebugName("CBurningVideoDriver");
#endif

	VertexCache_map_source_format();

	//Use AntiAlias(hack) to shrink BackBuffer Size and keep ScreenSize the same as Input

	//Control Interlaced/scaled BackBuffer
	get_scale(Interlaced, params);
	TexBias[ETF_STACK_3D] = 1.f;
	TexBias[ETF_STACK_2D] = 1.f;

	// create backbuffer.
	core::dimension2du use(params.WindowSize.Width / (Interlaced.target_scalex + 1),
		params.WindowSize.Height / (Interlaced.target_scaley + 1));
	BackBuffer = new CImage(SOFTWARE_DRIVER_2_RENDERTARGET_COLOR_FORMAT, use);
	if (BackBuffer)
	{
		//BackBuffer->fill(SColor(0));
		image_fill(BackBuffer, SColor(0), interlaced_disabled());

		// create z buffer
		if (params.ZBufferBits)
			DepthBuffer = video::createDepthBuffer(BackBuffer->getDimension());

		// create stencil buffer
		if (params.Stencilbuffer)
			StencilBuffer = video::createStencilBuffer(BackBuffer->getDimension(), 8);
	}

	DriverAttributes->setAttribute("MaxIndices", 1 << 16);
	DriverAttributes->setAttribute("MaxTextures", BURNING_MATERIAL_MAX_TEXTURES);
	DriverAttributes->setAttribute("MaxTextureSize", SOFTWARE_DRIVER_2_TEXTURE_MAXSIZE);
	DriverAttributes->setAttribute("MaxLights", 1024); //glsl::gl_MaxLights);
	DriverAttributes->setAttribute("MaxTextureLODBias", 16.f);
	DriverAttributes->setAttribute("Version", 50);

	// create triangle renderers

	memset(BurningShader, 0, sizeof(BurningShader));
	//BurningShader[ETR_FLAT] = createTRFlat2(DepthBuffer);
	//BurningShader[ETR_FLAT_WIRE] = createTRFlatWire2(DepthBuffer);
	BurningShader[ETR_GOURAUD] = createTriangleRendererGouraud2(this);
	BurningShader[ETR_GOURAUD_NOZ] = createTriangleRendererGouraudNoZ2(this);
	//BurningShader[ETR_GOURAUD_ALPHA] = createTriangleRendererGouraudAlpha2(this );
	BurningShader[ETR_GOURAUD_ALPHA_NOZ] = createTRGouraudAlphaNoZ2(this); // 2D
	//BurningShader[ETR_GOURAUD_WIRE] = createTriangleRendererGouraudWire2(DepthBuffer);
	//BurningShader[ETR_TEXTURE_FLAT] = createTriangleRendererTextureFlat2(DepthBuffer);
	//BurningShader[ETR_TEXTURE_FLAT_WIRE] = createTriangleRendererTextureFlatWire2(DepthBuffer);
	BurningShader[ETR_TEXTURE_GOURAUD] = createTriangleRendererTextureGouraud2(this);
	BurningShader[ETR_TEXTURE_GOURAUD_LIGHTMAP_M1] = createTriangleRendererTextureLightMap2_M1(this);
	BurningShader[ETR_TEXTURE_GOURAUD_LIGHTMAP_M2] = createTriangleRendererTextureLightMap2_M2(this);
	BurningShader[ETR_TEXTURE_GOURAUD_LIGHTMAP_M4] = createTriangleRendererGTextureLightMap2_M4(this);
	BurningShader[ETR_TEXTURE_LIGHTMAP_M4] = createTriangleRendererTextureLightMap2_M4(this);
	BurningShader[ETR_TEXTURE_GOURAUD_LIGHTMAP_ADD] = createTriangleRendererTextureLightMap2_Add(this);
	BurningShader[ETR_TEXTURE_GOURAUD_DETAIL_MAP] = createTriangleRendererTextureDetailMap2(this);

	BurningShader[ETR_TEXTURE_GOURAUD_WIRE] = createTriangleRendererTextureGouraudWire2(this);
	BurningShader[ETR_TEXTURE_GOURAUD_NOZ] = createTRTextureGouraudNoZ2(this);
	BurningShader[ETR_TEXTURE_GOURAUD_ADD] = createTRTextureGouraudAdd2(this);
	BurningShader[ETR_TEXTURE_GOURAUD_ADD_NO_Z] = createTRTextureGouraudAddNoZ2(this);
	BurningShader[ETR_TEXTURE_GOURAUD_VERTEX_ALPHA] = createTriangleRendererTextureVertexAlpha2(this);

	BurningShader[ETR_TEXTURE_GOURAUD_ALPHA] = createTRTextureGouraudAlpha(this);
	BurningShader[ETR_TEXTURE_GOURAUD_ALPHA_NOZ] = createTRTextureGouraudAlphaNoZ(this);

	//BurningShader[ETR_NORMAL_MAP_SOLID] = createTRNormalMap(this, EMT_NORMAL_MAP_SOLID);
	BurningShader[ETR_STENCIL_SHADOW] = createTRStencilShadow(this);
	BurningShader[ETR_TEXTURE_BLEND] = createTRTextureBlend(this);

	BurningShader[ETR_TRANSPARENT_REFLECTION_2_LAYER] = createTriangleRendererTexture_transparent_reflection_2_layer(this);
	//BurningShader[ETR_REFERENCE] = createTriangleRendererReference ( this );

	BurningShader[ETR_COLOR] = create_burning_shader_color(this);

	// add the same renderer for all solid types
	CSoftware2MaterialRenderer_SOLID* smr = new CSoftware2MaterialRenderer_SOLID(this);
	CSoftware2MaterialRenderer_TRANSPARENT_ADD_COLOR* tmr = new CSoftware2MaterialRenderer_TRANSPARENT_ADD_COLOR(this);
	//CSoftware2MaterialRenderer_UNSUPPORTED * umr = new CSoftware2MaterialRenderer_UNSUPPORTED ( this );

	//!TODO: addMaterialRenderer depends on pushing order....
	addMaterialRenderer(smr); // EMT_SOLID
	addMaterialRenderer(smr); // EMT_SOLID_2_LAYER,
	addMaterialRenderer(smr); // EMT_LIGHTMAP,
	addMaterialRenderer(tmr); // EMT_LIGHTMAP_ADD,
	addMaterialRenderer(smr); // EMT_LIGHTMAP_M2,
	addMaterialRenderer(smr); // EMT_LIGHTMAP_M4,
	addMaterialRenderer(smr); // EMT_LIGHTMAP_LIGHTING,
	addMaterialRenderer(smr); // EMT_LIGHTMAP_LIGHTING_M2,
	addMaterialRenderer(smr); // EMT_LIGHTMAP_LIGHTING_M4,
	addMaterialRenderer(smr); // EMT_DETAIL_MAP,
	addMaterialRenderer(smr); // EMT_SPHERE_MAP,
	addMaterialRenderer(smr); // EMT_REFLECTION_2_LAYER,
	addMaterialRenderer(tmr); // EMT_TRANSPARENT_ADD_COLOR,
	addMaterialRenderer(tmr); // EMT_TRANSPARENT_ALPHA_CHANNEL,
	addMaterialRenderer(tmr); // EMT_TRANSPARENT_ALPHA_CHANNEL_REF,
	addMaterialRenderer(tmr); // EMT_TRANSPARENT_VERTEX_ALPHA,
	addMaterialRenderer(tmr); // EMT_TRANSPARENT_REFLECTION_2_LAYER,

#if 0
	addMaterialRenderer(smr); // EMT_NORMAL_MAP_SOLID,
	addMaterialRenderer(tmr); // EMT_NORMAL_MAP_TRANSPARENT_ADD_COLOR,
	addMaterialRenderer(tmr); // EMT_NORMAL_MAP_TRANSPARENT_VERTEX_ALPHA,
	addMaterialRenderer(smr); // EMT_PARALLAX_MAP_SOLID,
	addMaterialRenderer(tmr); // EMT_PARALLAX_MAP_TRANSPARENT_ADD_COLOR,
	addMaterialRenderer(tmr); // EMT_PARALLAX_MAP_TRANSPARENT_VERTEX_ALPHA,
#else
	// add normal map renderers
	s32 tmp = 0;
	video::IMaterialRenderer* renderer = 0;
	renderer = createTRNormalMap(this, tmp, EMT_NORMAL_MAP_SOLID); renderer->drop();
	renderer = createTRNormalMap(this, tmp, EMT_NORMAL_MAP_TRANSPARENT_ADD_COLOR); renderer->drop();
	renderer = createTRNormalMap(this, tmp, EMT_NORMAL_MAP_TRANSPARENT_VERTEX_ALPHA); renderer->drop();

	renderer = createTRParallaxMap(this, tmp, EMT_PARALLAX_MAP_SOLID); renderer->drop();
	renderer = createTRParallaxMap(this, tmp, EMT_PARALLAX_MAP_TRANSPARENT_ADD_COLOR); renderer->drop();
	renderer = createTRParallaxMap(this, tmp, EMT_PARALLAX_MAP_TRANSPARENT_VERTEX_ALPHA); renderer->drop();

#endif

	addMaterialRenderer(tmr); // EMT_ONETEXTURE_BLEND

	smr->drop();
	tmr->drop();
	//umr->drop ();

	// select render target
	setRenderTargetImage2(BackBuffer, 0, 0);

	//reset Lightspace
	EyeSpace.init();

	// select the right renderer
	setMaterial(Material.org);
	samples_passed = 0;
}


//! destructor
CBurningVideoDriver::~CBurningVideoDriver()
{
	// delete Backbuffer
	if (BackBuffer)
	{
		BackBuffer->drop();
		BackBuffer = 0;
	}
	//release textures
	if (CurrentShader)
	{
	}
	Material.mat2D.setTexture(0, 0);

	// deleteMaterialRenders
	for (s32 i = 0; i < ETR2_COUNT; ++i)
	{
		if (BurningShader[i])
		{
			BurningShader[i]->drop();
			BurningShader[i] = 0;
		}
	}
	//deleteMaterialRenders();

	// delete Additional buffer
	if (StencilBuffer)
	{
		StencilBuffer->drop();
		StencilBuffer = 0;
	}

	if (DepthBuffer)
	{
		DepthBuffer->drop();
		DepthBuffer = 0;
	}

	if (RenderTargetTexture)
	{
		RenderTargetTexture->drop();
		RenderTargetTexture = 0;
	}

	if (RenderTargetSurface)
	{
		RenderTargetSurface->drop();
		RenderTargetSurface = 0;
	}

	fpu_exception(0);
}



//! queries the features of the driver, returns true if feature is available
bool CBurningVideoDriver::queryFeature(E_VIDEO_DRIVER_FEATURE feature) const
{
	int on = 0;
	switch (feature)
	{
#ifdef SOFTWARE_DRIVER_2_BILINEAR
	case EVDF_BILINEAR_FILTER:
		on = 1;
		break;
#endif
#if SOFTWARE_DRIVER_2_MIPMAPPING_MAX > 1
	case EVDF_MIP_MAP:
		on = 1;
		break;
#endif
	case EVDF_STENCIL_BUFFER:
		on = StencilBuffer != 0;
		break;

	case EVDF_RENDER_TO_TARGET:
	case EVDF_MULTITEXTURE:
	case EVDF_HARDWARE_TL:
	case EVDF_TEXTURE_NSQUARE:
	case EVDF_TEXTURE_MATRIX:
		on = 1;
		break;

	case EVDF_ARB_FRAGMENT_PROGRAM_1:
	case EVDF_ARB_VERTEX_PROGRAM_1:
		on = 1;
		break;
#if defined(PATCH_SUPERTUX_8_0_1_with_1_9_0)
	case EVDF_TEXTURE_NPOT:
	case EVDF_ARB_GLSL:
		on = 1;
		break;
#else
	case EVDF_TEXTURE_NPOT: // for 2D
		on = 0;
		break;
	case EVDF_DEPTH_CLAMP: // shadow
		on = 1;
		break;
#endif

#if defined(SOFTWARE_DRIVER_2_2D_AS_3D)
#if defined(IRRLICHT_FREE_CANVAS)
	case EVDF_VIEWPORT_SCALE_GUI:
		on = 1;
		break;
#endif
#endif
	case EVDF_OCCLUSION_QUERY:
		on = 1;
		break;
	default:
		on = 0;
		break;
	}

	return on && FeatureEnabled[feature];
}


//matrix multiplication
void CBurningVideoDriver::transform_calc(E_TRANSFORMATION_STATE_BURNING_VIDEO state)
{
	size_t* flag = TransformationFlag[TransformationStack];
	if (flag[state] & ETF_VALID) return;

	//check
	size_t ok = 0;
	switch (state)
	{
	case ETS_MODEL_VIEW_PROJ:
		if (0 == (flag[ETS_VIEW_PROJECTION] & ETF_VALID)) transform_calc(ETS_VIEW_PROJECTION);
		ok = flag[ETS_WORLD] & flag[ETS_VIEW] & flag[ETS_PROJECTION] & flag[ETS_VIEW_PROJECTION] & ETF_VALID;
		break;
	case ETS_VIEW_PROJECTION:
		ok = flag[ETS_VIEW] & flag[ETS_PROJECTION] & ETF_VALID;
		break;
	case ETS_MODEL_VIEW:
		ok = flag[ETS_WORLD] & flag[ETS_VIEW] & ETF_VALID;
		break;
	case ETS_NORMAL:
		ok = flag[ETS_MODEL_VIEW] & ETF_VALID;
		break;
	case ETS_MODEL_INVERSE:
		ok = flag[ETS_WORLD] & ETF_VALID;
		break;
	default:
		break;
	}

	if (!ok)
	{
		char buf[256];
		sprintf(buf, "transform_calc not valid for %d", state);
		os::Printer::log(buf, ELL_WARNING);
	}

	core::matrix4* matrix = Transformation[TransformationStack];

	switch (state)
	{
	case ETS_MODEL_VIEW_PROJ:
		if (flag[ETS_WORLD] & ETF_IDENTITY)
		{
			matrix[state] = matrix[ETS_VIEW_PROJECTION];
		}
		else
		{
			matrix[state].setbyproduct_nocheck(matrix[ETS_VIEW_PROJECTION], matrix[ETS_WORLD]);
		}
		break;

	case ETS_VIEW_PROJECTION:
		matrix[state].setbyproduct_nocheck(matrix[ETS_PROJECTION], matrix[ETS_VIEW]);
		break;
	case ETS_MODEL_VIEW:
		if (flag[ETS_WORLD] & ETF_IDENTITY)
		{
			matrix[state] = matrix[ETS_VIEW];
		}
		else
		{
			matrix[state].setbyproduct_nocheck(matrix[ETS_VIEW], matrix[ETS_WORLD]);
		}
		break;
	case ETS_NORMAL:
		mat33_transposed_inverse(matrix[state], matrix[ETS_MODEL_VIEW]);
		break;
	case ETS_MODEL_INVERSE:
		if (flag[ETS_WORLD] & ETF_IDENTITY)
		{
			matrix[state] = matrix[ETS_WORLD];
		}
		else
		{
			mat44_inverse(matrix[state], matrix[ETS_WORLD]);
		}
		break;

	default:
		break;
	}
	flag[state] |= ETF_VALID;
}


//! sets transformation
void CBurningVideoDriver::setTransform(E_TRANSFORMATION_STATE state, const core::matrix4& mat)
{
	size_t* flag = TransformationFlag[TransformationStack];
	core::matrix4* matrix = Transformation[TransformationStack];

#if 0
	int changed = 1;
	if (flag[state] & ETF_VALID)
	{
		changed = memcmp(mat.pointer(), matrix[state].pointer(), sizeof(mat));
	}
	if (changed)
#endif
	{
		matrix[state] = mat;
		flag[state] |= ETF_VALID;
	}

	//maybe identity (mostly for texturematrix to avoid costly multiplication)
#if defined ( USE_MATRIX_TEST )
	burning_setbit(TransformationFlag[state], mat.getDefinitelyIdentityMatrix(), ETF_IDENTITY);
#else
	burning_setbit(flag[state],
		0 == memcmp(mat.pointer(), core::IdentityMatrix.pointer(), sizeof(mat)), ETF_IDENTITY
	);
#endif

#if 0
	if (changed)
#endif
		switch (state)
		{
		case ETS_PROJECTION:
			flag[ETS_MODEL_VIEW_PROJ] &= ~ETF_VALID;
			flag[ETS_VIEW_PROJECTION] &= ~ETF_VALID;
			break;
		case ETS_VIEW:
			flag[ETS_MODEL_VIEW_PROJ] &= ~ETF_VALID;
			flag[ETS_VIEW_PROJECTION] &= ~ETF_VALID;
			flag[ETS_MODEL_VIEW] &= ~ETF_VALID;
			flag[ETS_NORMAL] &= ~ETF_VALID;
			break;
		case ETS_WORLD:
			flag[ETS_MODEL_VIEW_PROJ] &= ~ETF_VALID;
			flag[ETS_MODEL_VIEW] &= ~ETF_VALID;
			flag[ETS_NORMAL] &= ~ETF_VALID;
			flag[ETS_MODEL_INVERSE] &= ~ETF_VALID;
			break;
		case ETS_TEXTURE_0:
		case ETS_TEXTURE_1:
		case ETS_TEXTURE_2:
		case ETS_TEXTURE_3:
#if _IRR_MATERIAL_MAX_TEXTURES_>4
		case ETS_TEXTURE_4:
#endif
#if _IRR_MATERIAL_MAX_TEXTURES_>5
		case ETS_TEXTURE_5:
#endif
#if _IRR_MATERIAL_MAX_TEXTURES_>6
		case ETS_TEXTURE_6:
#endif
#if _IRR_MATERIAL_MAX_TEXTURES_>7
		case ETS_TEXTURE_7:
#endif
			if (0 == (flag[state] & ETF_IDENTITY))
			{
				flag[state] |= ETF_TEXGEN_MATRIX;
			}
			break;
		default:
			break;
		}

}

//! Returns the transformation set by setTransform
const core::matrix4& CBurningVideoDriver::getTransform(E_TRANSFORMATION_STATE state) const
{
	return Transformation[TransformationStack][state];
}


bool CBurningVideoDriver::beginScene(u16 clearFlag, SColor clearColor, f32 clearDepth, u8 clearStencil, const SExposedVideoData& videoData, core::rect<s32>* sourceRect)
{
#if defined(PATCH_SUPERTUX_8_0_1_with_1_9_0)
	CNullDriver::beginScene(clearFlag & ECBF_COLOR, clearFlag & ECBF_DEPTH, clearColor, videoData, sourceRect);
#else
	CNullDriver::beginScene(clearFlag, clearColor, clearDepth, clearStencil, videoData, sourceRect);
#endif

	Interlaced.nr = (Interlaced.nr + 1) & interlace_control_mask;
	WindowId = videoData.D3D9.HWnd;
	SceneSourceRect = sourceRect;

	clearBuffers(clearFlag, clearColor, clearDepth, clearStencil);

	//memset ( TransformationFlag, 0, sizeof ( TransformationFlag ) );
	return true;
}

bool CBurningVideoDriver::endScene()
{
	CNullDriver::endScene();

	return Presenter->present(BackBuffer, WindowId, SceneSourceRect);
}


//! Create render target.
IRenderTarget* CBurningVideoDriver::addRenderTarget()
{
	CSoftwareRenderTarget2* renderTarget = new CSoftwareRenderTarget2(this);
	RenderTargets.push_back(renderTarget);

	return renderTarget;
}

#if defined(PATCH_SUPERTUX_8_0_1_with_1_9_0)
bool CBurningVideoDriver::setRenderTarget(video::ITexture* texture, bool clearBackBuffer, bool clearZBuffer, SColor color)
{
	CSoftwareRenderTarget2 target(this);
	target.RenderTexture = texture;
	target.TargetType = ERT_RENDER_TEXTURE;
	target.Textures[0] = texture;

	if (texture)
		texture->grab();

	u16 flag = 0;
	if (clearBackBuffer) flag |= ECBF_COLOR;
	if (clearZBuffer) flag |= ECBF_DEPTH;

	return setRenderTargetEx(texture ? &target : 0, flag, color, 1.f, true);
}
#endif

bool CBurningVideoDriver::setRenderTargetEx(IRenderTarget* target, u16 clearFlag, SColor clearColor, f32 clearDepth, u8 clearStencil)
{
#if !defined(PATCH_SUPERTUX_8_0_1_with_1_9_0)
	if (target && target->getDriverType() != EDT_BURNINGSVIDEO)
	{
		os::Printer::log("Fatal Error: Tried to set a render target not owned by this driver.", ELL_ERROR);
		return false;
	}
#endif
	if (RenderTargetTexture)
	{
		//switching from texture to backbuffer
		if (target == 0)
		{
			RenderTargetTexture->regenerateMipMapLevels();
		}
		RenderTargetTexture->drop();
	}

#if !defined(PATCH_SUPERTUX_8_0_1_with_1_9_0)
	RenderTargetTexture = target ? target->getTexture()[0] : 0;
#else
	RenderTargetTexture = target ? ((CSoftwareRenderTarget2*)target)->Textures[0] : 0;
#endif

	if (RenderTargetTexture)
	{
		RenderTargetTexture->grab();
		Interlaced.bypass = 1;
		setRenderTargetImage2(((CSoftwareTexture2*)RenderTargetTexture)->getImage());
	}
	else
	{
		Interlaced.bypass = Interlaced.enable == 0;
		setRenderTargetImage2(BackBuffer);
	}

	clearBuffers(clearFlag, clearColor, clearDepth, clearStencil);

	return true;
}

/*
static inline f32 map_value(f32 x, f32 in_min, f32 in_max, f32 out_min, f32 out_max) {
	return (x - in_min) * (out_max - out_min) / (f32)(in_max - in_min) + out_min;
}
*/

//! sets a render target
void CBurningVideoDriver::setRenderTargetImage2(video::IImage* color, video::IImage* depth, video::IImage* stencil)
{
	if (RenderTargetSurface)
		RenderTargetSurface->drop();

	core::dimension2d<u32> current = RenderTargetSize;
	RenderTargetSurface = color;
	RenderTargetSize.Width = 0;
	RenderTargetSize.Height = 0;

	if (RenderTargetSurface)
	{
		RenderTargetSurface->grab();
		RenderTargetSize = RenderTargetSurface->getDimension();
	}

	RatioRenderTargetScreen.x = ScreenSize.Width ? (f32)RenderTargetSize.Width / ScreenSize.Width : 1.f;
	RatioRenderTargetScreen.y = ScreenSize.Height ? (f32)RenderTargetSize.Height / ScreenSize.Height : 1.f;

	int not_changed = current == RenderTargetSize;
	burning_setbit(TransformationFlag[0][ETS_PROJECTION], not_changed, ETF_VALID);
	burning_setbit(TransformationFlag[1][ETS_PROJECTION], not_changed, ETF_VALID);

	setViewPort(core::recti(RenderTargetSize));

	if (DepthBuffer)
		DepthBuffer->setSize(RenderTargetSize);

	if (StencilBuffer)
		StencilBuffer->setSize(RenderTargetSize);
}


//--------- Transform from NDC to DC, transform TexCoo ----------------------------------------------



//--------- Transform from NDC to DC ----------------------------------------------

// used to scale <-1,-1><1,1> to viewport [scale,center]
// controls subtexel and fill convention.
// Don't tweak SOFTWARE_DRIVER_2_SUBTEXEL (-0.5f in m[1]) anymore to control texture blur effect, it's used for viewport scaling.
// naming is misleading. it will write outside memory location..

//xw = (xn+1)*(w/2) + x
void buildNDCToDCMatrix(f32* burning_restrict dc_matrix, const core::rect<s32>& viewport, const f32 center)
{
	//const f32 center = -0.5f; // combined with top / left fill convention to (0,0)-(x-1,y-1)

	f32 x0 = viewport.UpperLeftCorner.X + center;
	f32 x1 = viewport.LowerRightCorner.X - 1 - center;
	f32 y0 = viewport.UpperLeftCorner.Y + center;
	f32 y1 = viewport.LowerRightCorner.Y - 1 - center;

	dc_matrix[0] = (x1 - x0) * 0.5f;
	dc_matrix[1] = dc_matrix[0] + x0;

	dc_matrix[2] = (y0 - y1) * 0.5f;
	dc_matrix[3] = dc_matrix[2] + y1;

}



//! sets a viewport
void CBurningVideoDriver::setViewPort(const core::rect<s32>& area)
{
	//const core::rect<s32> rendert(0, 0, getCurrentRenderTargetSize().Width, getCurrentRenderTargetSize().Height);
	const core::rect<s32> rendert(0, 0, RenderTargetSize.Width, RenderTargetSize.Height);

	ViewPort = area;
	ViewPort.clipAgainst(rendert);
	const s32 viewarea = ViewPort.getArea();

	//is this even possible to be pixel-perfect if i have not the same depth range as openGL?
	//fill convention maybe flipped because window space is flipped so +-1 pixel always off?
	buildNDCToDCMatrix(Transformation_ETS_CLIPSCALE[ETF_STACK_3D], ViewPort, -0.5f);

	//Pixel Offset in window space here and not in view-space to avoid clipping
	//[-0.5,-0.5]-[w-0.5,h-0.5]
	buildNDCToDCMatrix(Transformation_ETS_CLIPSCALE[ETF_STACK_2D], ViewPort, -0.5f);

	TexBias[ETF_STACK_3D] = viewarea <= (160 * 120) ? 1.5f : Interlaced.target_scalex ? 0.75f : 0.75f;
	TexBias[ETF_STACK_2D] = 1.5f;

	if (CurrentShader)
		CurrentShader->setRenderTarget(RenderTargetSurface, ViewPort, Interlaced);
}

void CBurningVideoDriver::setScissor(int x, int y, int width, int height)
{
	//openGL
	//y = rt.Height - y - height;

	//coming from GUI
	AbsRectangle v0;
	v0.x0 = core::floor32(x * RatioRenderTargetScreen.x);
	v0.y0 = core::floor32(y * RatioRenderTargetScreen.y);
	v0.x1 = core::floor32((x + width) * RatioRenderTargetScreen.x);
	v0.y1 = core::floor32((y + height) * RatioRenderTargetScreen.y);

	AbsRectangle v1;
	v1.x0 = 0;
	v1.y0 = 0;
	v1.x1 = RenderTargetSize.Width;
	v1.y1 = RenderTargetSize.Height;

	intersect(Scissor, v0, v1);
}

/*
	generic plane clipping in homogenous coordinates
	special case ndc frustum <-w,w>,<-w,w>,<-w,w>
	can be rewritten with compares e.q near plane, a.z < -a.w and b.z < -b.w

	cam is (0,0,-1)
*/

static const sVec4 NDCPlane[6 + 2] =
{
	sVec4(0.f,  0.f, 1.f, -1.f),	// near
	sVec4(0.f,  0.f, -1.f, -1.f),	// far
	sVec4(1.f,  0.f,  0.f, -1.f),	// left
	sVec4(-1.f,  0.f,  0.f, -1.f),	// right
	sVec4(0.f,  1.f,  0.f, -1.f),	// bottom
	sVec4(0.f, -1.f,  0.f, -1.f)	// top
};

/*
	test a vertex if it's inside the standard frustum

	this is the generic one..

	f32 dotPlane;
	for ( u32 i = 0; i!= 6; ++i )
	{
		dotPlane = v->Pos.dotProduct ( NDCPlane[i] );
		burning_setbit32( flag, dotPlane <= 0.f, 1 << i );
	}

	// this is the base for ndc frustum <-w,w>,<-w,w>,<-w,w>
	burning_setbit32( flag, ( v->Pos.z - v->Pos.w ) <= 0.f, 1 );
	burning_setbit32( flag, (-v->Pos.z - v->Pos.w ) <= 0.f, 2 );
	burning_setbit32( flag, ( v->Pos.x - v->Pos.w ) <= 0.f, 4 );
	burning_setbit32( flag, (-v->Pos.x - v->Pos.w ) <= 0.f, 8 );
	burning_setbit32( flag, ( v->Pos.y - v->Pos.w ) <= 0.f, 16 );
	burning_setbit32( flag, (-v->Pos.y - v->Pos.w ) <= 0.f, 32 );

*/
#ifdef IRRLICHT_FAST_MATH

REALINLINE size_t CBurningVideoDriver::clipToFrustumTest(const s4DVertex* v) const
{
	size_t flag;
	f32 test[8];
	const f32 w = -v->Pos.w;

	// a conditional move is needed....FCOMI ( but we don't have it )
	// so let the fpu calculate and write it back.
	// cpu makes the compare, interleaving

	test[0] = v->Pos.z + w;
	test[1] = -v->Pos.z + w;
	test[2] = v->Pos.x + w;
	test[3] = -v->Pos.x + w;
	test[4] = v->Pos.y + w;
	test[5] = -v->Pos.y + w;

	const u32* a = F32_AS_U32_POINTER(test);
	flag = (a[0]) >> 31;
	flag |= (a[1] & 0x80000000) >> 30;
	flag |= (a[2] & 0x80000000) >> 29;
	flag |= (a[3] & 0x80000000) >> 28;
	flag |= (a[4] & 0x80000000) >> 27;
	flag |= (a[5] & 0x80000000) >> 26;

	/*
		flag  = (IR ( test[0] )              ) >> 31;
		flag |= (IR ( test[1] ) & 0x80000000 ) >> 30;
		flag |= (IR ( test[2] ) & 0x80000000 ) >> 29;
		flag |= (IR ( test[3] ) & 0x80000000 ) >> 28;
		flag |= (IR ( test[4] ) & 0x80000000 ) >> 27;
		flag |= (IR ( test[5] ) & 0x80000000 ) >> 26;
	*/
	/*
		flag  = F32_LOWER_EQUAL_0 ( test[0] );
		flag |= F32_LOWER_EQUAL_0 ( test[1] ) << 1;
		flag |= F32_LOWER_EQUAL_0 ( test[2] ) << 2;
		flag |= F32_LOWER_EQUAL_0 ( test[3] ) << 3;
		flag |= F32_LOWER_EQUAL_0 ( test[4] ) << 4;
		flag |= F32_LOWER_EQUAL_0 ( test[5] ) << 5;
	*/
	return flag;
}

#else


REALINLINE u32 clipToFrustumTest(const s4DVertex* v)
{
	u32 flag = 0;

	flag |= v->Pos.z <= v->Pos.w ? (size_t)VERTEX4D_CLIP_NEAR : 0;
	flag |= -v->Pos.z <= v->Pos.w ? (size_t)VERTEX4D_CLIP_FAR : 0;

	flag |= v->Pos.x <= v->Pos.w ? (size_t)VERTEX4D_CLIP_LEFT : 0;
	flag |= -v->Pos.x <= v->Pos.w ? (size_t)VERTEX4D_CLIP_RIGHT : 0;

	flag |= v->Pos.y <= v->Pos.w ? (size_t)VERTEX4D_CLIP_BOTTOM : 0;
	flag |= -v->Pos.y <= v->Pos.w ? (size_t)VERTEX4D_CLIP_TOP : 0;


	//verify with plane
/*
	size_t flag2 = 0;
	for ( u32 i = 0; i < 6; ++i )
	{
		if (v->Pos.dot_xyzw(NDCPlane[i]) <= 0.f) flag2 |= ((size_t)1) << i;
	}
	if (flag != flag2)
	{
		int g = 1;
	}
*/
	return flag;
}

#endif // _MSC_VER


u32 clipToHyperPlane(
	s4DVertexPair* burning_restrict dest,
	const s4DVertexPair* burning_restrict source,
	const u32 inCount,
	const sVec4& plane
)
{
	u32 outCount = 0;
	s4DVertexPair* out = dest;

	const s4DVertex* a;
	const s4DVertex* b = source;

	ipoltype bDotPlane;
	bDotPlane = b->Pos.dot_xyzw(plane);

	/*
		for( u32 i = 1; i < inCount + 1; ++i)
		{
	#if 0
			a = source + (i%inCount)*2;
	#else
			const s32 condition = i - inCount;
			const s32 index = (( ( condition >> 31 ) & ( i ^ condition ) ) ^ condition ) << 1;
			a = source + index;
	#endif
	*/

	// polygon scan conversion edge sharing opposite side?
	//Sutherland–Hodgman
	for (u32 i = 0; i < inCount; ++i)
	{
		a = source + (i == inCount - 1 ? 0 : s4DVertex_ofs(i + 1));

		// current point inside
		if (ipol_lower_equal_0(a->Pos.dot_xyzw(plane)))
		{
			// last point outside
			if (ipol_greater_0(bDotPlane))
			{
				// intersect line segment with plane
				//out->interpolate(*b, *a, bDotPlane / (b->Pos - a->Pos).dot_xyzw(plane));
				ipoltype denom = (b->Pos - a->Pos).dot_xyzw(plane);
				out->interpolate(*b, *a, bDotPlane / denom);
				out += sizeof_s4DVertexPairRel;
				outCount += 1;
			}

			// copy current to out
			//*out = *a;
			memcpy_s4DVertexPair(out, a);
			b = out;

			out += sizeof_s4DVertexPairRel;
			outCount += 1;
		}
		else
		{
			// current point outside
			if (ipol_lower_0(bDotPlane))
			{
				// previous was inside
				// intersect line segment with plane
				//out->interpolate(*b, *a, bDotPlane / (b->Pos - a->Pos).dot_xyzw(plane));
				ipoltype denom = (b->Pos - a->Pos).dot_xyzw(plane);
				out->interpolate(*b, *a, bDotPlane / denom);
				out += sizeof_s4DVertexPairRel;
				outCount += 1;
			}
			// pointer
			b = a;
		}

		bDotPlane = b->Pos.dot_xyzw(plane);
	}

	return outCount;
}


/*
	Clip on all planes. Clipper.data
	clipmask per face
*/
u32 CBurningVideoDriver::clipToFrustum(const u32 vIn /*, const size_t clipmask_for_face*/)
{
	s4DVertexPair* v0 = Clipper.data;
	s4DVertexPair* v1 = Clipper_disjoint.data;
	u32 vOut = vIn;

	//clear all clipping & projected flags
	const u32 flag = v0[0].flag & VERTEX4D_FORMAT_MASK;
	for (u32 g = 0; g != Clipper.ElementSize; ++g)
	{
		v0[g].flag = flag;
		v1[g].flag = flag;
	}

#if 0
	for (size_t i = 0; i < 6; ++i)
	{
		v0 = i & 1 ? Clipper_disjoint.data : Clipper.data;
		v1 = i & 1 ? Clipper.data : Clipper_disjoint.data;

		//clipMask checked outside - always clip all planes
#if 0
		if (0 == (clipMask & ((size_t)1 << i)))
		{
			vOut = vIn;
			memcpy_s4DVertexPair(v1, v0);
		}
		else
#endif
		{
			vOut = clipToHyperPlane(v1, v0, vOut, NDCPlane[i]);
			if (vOut < vIn) return vOut;
		}
	}
#endif


	vOut = clipToHyperPlane(v1, v0, vOut, NDCPlane[0]); if (vOut < vIn) return vOut;
	vOut = clipToHyperPlane(v0, v1, vOut, NDCPlane[1]); if (vOut < vIn) return vOut;
	vOut = clipToHyperPlane(v1, v0, vOut, NDCPlane[2]); if (vOut < vIn) return vOut;
	vOut = clipToHyperPlane(v0, v1, vOut, NDCPlane[3]); if (vOut < vIn) return vOut;
	vOut = clipToHyperPlane(v1, v0, vOut, NDCPlane[4]); if (vOut < vIn) return vOut;
	vOut = clipToHyperPlane(v0, v1, vOut, NDCPlane[5]);

	return vOut;
}

/*!
	Part I:
	apply Clip Scale matrix
	From Normalized Device Coordiante ( NDC ) Space to Device Coordinate ( DC ) Space

	Part II:
	Project homogeneous vector
	homogeneous to non-homogenous coordinates ( dividebyW )

	Incoming: ( xw, yw, zw, w, u, v, 1, R, G, B, A )
	Outgoing: ( xw/w, yw/w, zw/w, w/w, u/w, v/w, 1/w, R/w, G/w, B/w, A/w )

	replace w/w by 1/w
*/
//aliasing problems! [dest = source + 1]

inline void ndc_2_dc_and_project(s4DVertexPair* burning_restrict v, const u32 vIn,
	const f32* burning_restrict dc_matrix
)
{
#define src v[g]
#define dst v[g+1]

	for (u32 g = 0; g < vIn; g += sizeof_s4DVertexPairRel)
	{
		//cache doesn't work anymore?
		//if ( dst.flag & VERTEX4D_PROJECTED ) continue;
		//dst.flag = src.flag | VERTEX4D_PROJECTED;

		const f32 iw = reciprocal_zero_pos_underflow(src.Pos.w);

		// from normalized device to window coordinates (-1,-1)  viewport

		//limit sub pixel for consistent fill convention (wrong place)
#if	SOFTWARE_DRIVER_2_SUBPIXEL_LIMIT > 0 && 0
		dst.Pos.x = floorf((iw * src.Pos.x * dc_matrix[0] + dc_matrix[1]) * 128.f+0.5f) * (1.f / 128.f);
		dst.Pos.y = floorf((iw * src.Pos.y * dc_matrix[2] + dc_matrix[3]) * 128.f + 0.5f) * (1.f/ 128.f);
#else
		dst.Pos.x = iw * src.Pos.x * dc_matrix[0] + dc_matrix[1];
		dst.Pos.y = iw * src.Pos.y * dc_matrix[2] + dc_matrix[3];
#endif


		//burning uses direct Z. for OpenGL it should be -Z,[-1;1] and texture flip
#if !defined(SOFTWARE_DRIVER_2_USE_WBUFFER) || 1
		dst.Pos.z = -iw * src.Pos.z * 0.5f + 0.5f;
#endif
		dst.Pos.w = iw;

		//ortographic projection w == 1 looses stencil
		//dest[g].Pos.w = 1.f - dest[g].Pos.z;

		// Texture Coordinates will be projected after mipmap selection
		// satisfy write-combiner
		//todo: only set on flag
#if 1
#if BURNING_MATERIAL_MAX_TEXTURES > 0
		dst.Tex[0].x = src.Tex[0].x;
		dst.Tex[0].y = src.Tex[0].y;
#endif
#if BURNING_MATERIAL_MAX_TEXTURES > 1
		dst.Tex[1].x = src.Tex[1].x;
		dst.Tex[1].y = src.Tex[1].y;
#endif
#if BURNING_MATERIAL_MAX_TEXTURES > 2
		dst.Tex[2].x = src.Tex[2].x;
		dst.Tex[2].y = src.Tex[2].y;
#endif
#if BURNING_MATERIAL_MAX_TEXTURES > 3
		dst.Tex[3].x = src.Tex[3].x;
		dst.Tex[3].y = src.Tex[3].y;
#endif
#endif

		// alpha?
#if BURNING_MATERIAL_MAX_COLORS > 0
#ifdef SOFTWARE_DRIVER_2_PERSPECTIVE_CORRECT
		dst.Color[0].r = src.Color[0].r * iw;
		dst.Color[0].g = src.Color[0].g * iw;
		dst.Color[0].b = src.Color[0].b * iw;
		dst.Color[0].a = src.Color[0].a * iw;
#else
		dst.Color[0] = src.Color[0];
#endif
#endif

#if BURNING_MATERIAL_MAX_COLORS > 1
#ifdef SOFTWARE_DRIVER_2_PERSPECTIVE_CORRECT
		dst.Color[1].r = src.Color[1].r * iw;
		dst.Color[1].g = src.Color[1].g * iw;
		dst.Color[1].b = src.Color[1].b * iw;
		dst.Color[1].a = src.Color[1].a * iw;
#else
		dst.Color[1] = src.Color[1];
#endif
#endif

#if BURNING_MATERIAL_MAX_COLORS > 2
#ifdef SOFTWARE_DRIVER_2_PERSPECTIVE_CORRECT
		dst.Color[2].r = src.Color[2].r * iw;
		dst.Color[2].g = src.Color[2].g * iw;
		dst.Color[2].b = src.Color[2].b * iw;
		dst.Color[2].a = src.Color[2].a * iw;
#else
		dst.Color[2] = src.Color[2];
#endif
#endif

#if BURNING_MATERIAL_MAX_COLORS > 3
#ifdef SOFTWARE_DRIVER_2_PERSPECTIVE_CORRECT
		dst.Color[3].r = src.Color[3].r * iw;
		dst.Color[3].g = src.Color[3].g * iw;
		dst.Color[3].b = src.Color[3].b * iw;
		dst.Color[3].a = src.Color[3].a * iw;
#else
		dst.Color[3] = src.Color[3];
#endif
#endif

#if BURNING_MATERIAL_MAX_LIGHT_TANGENT > 0
#ifdef SOFTWARE_DRIVER_2_PERSPECTIVE_CORRECT
		dst.LightTangent[0].x = src.LightTangent[0].x * iw;
		dst.LightTangent[0].y = src.LightTangent[0].y * iw;
		dst.LightTangent[0].z = src.LightTangent[0].z * iw;
#else
		dst.LightTangent[0] = src.LightTangent[0];
#endif
#endif
	}

#undef src
#undef dst

}

inline void ndc_2_dc_and_project_grid(s4DVertexPair* burning_restrict v, const u32 vIn,
	const f32* burning_restrict dc_matrix
)
{
#define src v[g]
#define dst v[g+1]

	u32 i;
	u32 size;

	for (u32 g = 0; g < vIn; g += sizeof_s4DVertexPairRel)
	{
		const f32 iw = reciprocal_zero_pos_underflow(src.Pos.w);

		// from normalized device to window coordinates (-1,-1)  viewport

		//limit sub pixel for consistent fill convention (wrong place)
		dst.Pos.x = floorf((iw * src.Pos.x * dc_matrix[0] + dc_matrix[1]) * 4096.f + 0.5f) * (1.f / 4096.f);
		dst.Pos.y = floorf((iw * src.Pos.y * dc_matrix[2] + dc_matrix[3]) * 4096.f + 0.5f) * (1.f / 4096.f);

		//burning uses direct Z. for OpenGL it should be -Z,[-1;1] and texture flip
#if !defined(SOFTWARE_DRIVER_2_USE_WBUFFER) || 1
		dst.Pos.z = -iw * src.Pos.z * 0.5f + 0.5f;
#endif
		dst.Pos.w = iw;

#if BURNING_MATERIAL_MAX_TEXTURES > 0
		size = (src.flag & VERTEX4D_FORMAT_MASK_TEXTURE) >> 16;
		for (i = 0; i != size; ++i)
		{
			dst.Tex[i].x = src.Tex[i].x;
			dst.Tex[i].y = src.Tex[i].y;
		}
#endif

#if BURNING_MATERIAL_MAX_COLORS > 0
		size = (src.flag & VERTEX4D_FORMAT_MASK_COLOR) >> 20;
		for (i = 0; i != size; ++i)
		{
			// alpha?
#ifdef SOFTWARE_DRIVER_2_PERSPECTIVE_CORRECT
			dst.Color[i].r = src.Color[i].r * iw;
			dst.Color[i].g = src.Color[i].g * iw;
			dst.Color[i].b = src.Color[i].b * iw;
			dst.Color[i].a = src.Color[i].a * iw;
#else
			dst.Color[i] = src.Color[i];
#endif
		}
#endif


#if BURNING_MATERIAL_MAX_LIGHT_TANGENT > 0
		size = (src.flag & VERTEX4D_FORMAT_MASK_LIGHT) >> 24;
		for (i = 0; i != size; ++i)
		{
#ifdef SOFTWARE_DRIVER_2_PERSPECTIVE_CORRECT
			dst.LightTangent[i].x = src.LightTangent[i].x * iw;
			dst.LightTangent[i].y = src.LightTangent[i].y * iw;
			dst.LightTangent[i].z = src.LightTangent[i].z * iw;
#else
			dst.LightTangent[i] = src.LightTangent[i];
#endif
		}
#endif

}

#undef src
#undef dst

}


#define MAT_TEXTURE(tex) ( (video::CSoftwareTexture2*) Material.org.TextureLayer[tex].Texture )

//! clamp(value,0,1)
#if 0
static inline float clampfuv(const float v, const float b)
{
	// b = 1.f - (2.f * (1/width))
	return v < b ? b : v > 1.f - b ? 1.f - b : v;
	//return v < b ? b : v > 1.f-b ? 1.f-b : v;
}
#endif

static inline float clampf01(const float v)
{
	return v < 0.f ? 0.f : v > 1.f ? 1.f : v;
}


// Vertex Cache

//! setup Vertex Format
void CBurningVideoDriver::VertexCache_map_source_format()
{
	const u32 s0 = sizeof(s4DVertex);
	const u32 s1 = sizeof(s4DVertex_proxy);

	if (s1 <= sizeof_s4DVertex / 2)
	{
		os::Printer::log("BurningVideo vertex format unnecessary to large", ELL_WARNING);
	}

	//memcpy_vertex
	if (s0 != sizeof_s4DVertex || ((sizeof_s4DVertex * sizeof_s4DVertexPairRel) & 31))
	{
		os::Printer::log("BurningVideo vertex format compile problem", ELL_ERROR);
		IRR_DEBUG_BREAK_IF(1);
	}

#if defined(ENV64BIT)
	if (sizeof(void*) != 8)
	{
		os::Printer::log("BurningVideo pointer should be 8 bytes", ELL_ERROR);
		IRR_DEBUG_BREAK_IF(1);
	}

	if (((unsigned long long)Transformation & 15) || ((unsigned long long)TransformationFlag & 15))
	{
		os::Printer::log("BurningVideo Matrix Stack not 16 byte aligned", ELL_ERROR);
		IRR_DEBUG_BREAK_IF(1);
	}
#endif


	SVSize* vSize = VertexShader.vSize;
	vSize[E4VT_STANDARD].Format = VERTEX4D_FORMAT_TEXTURE_1 | VERTEX4D_FORMAT_COLOR_2_FOG;
	vSize[E4VT_STANDARD].Pitch = sizeof(S3DVertex);
	vSize[E4VT_STANDARD].TexSize = 1;
	vSize[E4VT_STANDARD].TexCooSize = 1;
	vSize[E4VT_STANDARD].ColSize = 2;

	vSize[E4VT_2TCOORDS].Format = VERTEX4D_FORMAT_TEXTURE_2 | VERTEX4D_FORMAT_COLOR_2_FOG;
	vSize[E4VT_2TCOORDS].Pitch = sizeof(S3DVertex2TCoords);
	vSize[E4VT_2TCOORDS].TexSize = 2;
	vSize[E4VT_2TCOORDS].TexCooSize = 2;
	vSize[E4VT_2TCOORDS].ColSize = 2;

	// 	EMT_NORMAL_MAP_SOLID,EMT_NORMAL_MAP_TRANSPARENT_ADD_COLOR,EMT_NORMAL_MAP_TRANSPARENT_VERTEX_ALPHA
	vSize[E4VT_TANGENTS].Format = VERTEX4D_FORMAT_TEXTURE_2 | VERTEX4D_FORMAT_COLOR_4 | VERTEX4D_FORMAT_BUMP_DOT3;
	vSize[E4VT_TANGENTS].Pitch = sizeof(S3DVertexTangents);
	vSize[E4VT_TANGENTS].TexSize = 2;
	vSize[E4VT_TANGENTS].TexCooSize = 2;
	vSize[E4VT_TANGENTS].ColSize = 4;

	// 	EMT_PARALLAX_MAP_SOLID,EMT_PARALLAX_MAP_TRANSPARENT_ADD_COLOR,EMT_PARALLAX_MAP_TRANSPARENT_VERTEX_ALPHA
	vSize[E4VT_TANGENTS_PARALLAX].Format = VERTEX4D_FORMAT_TEXTURE_2 | VERTEX4D_FORMAT_COLOR_4 | VERTEX4D_FORMAT_PARALLAX;
	vSize[E4VT_TANGENTS_PARALLAX].Pitch = sizeof(S3DVertexTangents);
	vSize[E4VT_TANGENTS_PARALLAX].TexSize = 2;
	vSize[E4VT_TANGENTS_PARALLAX].TexCooSize = 2;
	vSize[E4VT_TANGENTS_PARALLAX].ColSize = 4;

	// EMT_TRANSPARENT_REFLECTION_2_LAYER map
	vSize[E4VT_REFLECTION_MAP].Format = VERTEX4D_FORMAT_TEXTURE_2 | VERTEX4D_FORMAT_COLOR_2_FOG;
	vSize[E4VT_REFLECTION_MAP].Pitch = sizeof(S3DVertex);
	vSize[E4VT_REFLECTION_MAP].TexSize = 2;
	vSize[E4VT_REFLECTION_MAP].TexCooSize = 1; //TexCoo2 generated
	vSize[E4VT_REFLECTION_MAP].ColSize = 2;

	// shadow
	vSize[E4VT_SHADOW].Format = 0;
	vSize[E4VT_SHADOW].Pitch = sizeof(f32) * 3; // core::vector3df*
	vSize[E4VT_SHADOW].TexSize = 0;
	vSize[E4VT_SHADOW].TexCooSize = 0;
	vSize[E4VT_SHADOW].ColSize = 0;

	// color shading only (no texture)
	vSize[E4VT_NO_TEXTURE].Format = VERTEX4D_FORMAT_COLOR_2_FOG;
	vSize[E4VT_NO_TEXTURE].Pitch = sizeof(S3DVertex);
	vSize[E4VT_NO_TEXTURE].TexSize = 0;
	vSize[E4VT_NO_TEXTURE].TexCooSize = 0;
	vSize[E4VT_NO_TEXTURE].ColSize = 2;

	//Line
	vSize[E4VT_LINE].Format = VERTEX4D_FORMAT_COLOR_1;
	vSize[E4VT_LINE].Pitch = sizeof(S3DVertex);
	vSize[E4VT_LINE].TexSize = 0;
	vSize[E4VT_LINE].TexCooSize = 0;
	vSize[E4VT_LINE].ColSize = 1;

	//verify with global defines
	u32 size;
	for (size_t i = 0; i < E4VT_COUNT; ++i)
	{
		u32& flag = vSize[i].Format;

#if !defined(SOFTWARE_DRIVER_2_USE_SEPARATE_SPECULAR_COLOR)
		//flag &= ~VERTEX4D_FORMAT_SPECULAR;
#endif

		if (vSize[i].TexSize > BURNING_MATERIAL_MAX_TEXTURES)
			vSize[i].TexSize = BURNING_MATERIAL_MAX_TEXTURES;

		size = (flag & VERTEX4D_FORMAT_MASK_TEXTURE) >> 16;
		if (size > BURNING_MATERIAL_MAX_TEXTURES)
		{
			flag = (flag & ~VERTEX4D_FORMAT_MASK_TEXTURE) | (BURNING_MATERIAL_MAX_TEXTURES << 16);
		}

		size = (flag & VERTEX4D_FORMAT_MASK_COLOR) >> 20;
		if (size > BURNING_MATERIAL_MAX_COLORS)
		{
			flag = (flag & ~VERTEX4D_FORMAT_MASK_COLOR) | (BURNING_MATERIAL_MAX_COLORS << 20);
		}

		size = (flag & VERTEX4D_FORMAT_MASK_LIGHT) >> 24;
		if (size > BURNING_MATERIAL_MAX_LIGHT_TANGENT)
		{
			flag = (flag & ~VERTEX4D_FORMAT_MASK_LIGHT) | (BURNING_MATERIAL_MAX_LIGHT_TANGENT << 24);
		}
	}

	VertexShader.mem.resize(VERTEXCACHE_ELEMENT * 2);
	VertexShader.vType = E4VT_STANDARD;

	Clipper.resize(VERTEXCACHE_ELEMENT * 2);
	Clipper_disjoint.resize(VERTEXCACHE_ELEMENT * 2);

	TransformationStack = ETF_STACK_3D;
	memset(TransformationFlag, 0, sizeof(TransformationFlag));
	memset(Transformation_ETS_CLIPSCALE, 0, sizeof(Transformation_ETS_CLIPSCALE));

	Material.resetRenderStates = true;
	Material.Fallback_MaterialType = EMT_SOLID;
	Material.VertexShader = BVT_Fix;

	PushShader.CurrentShader = 0;
	PushShader.EdgeTestPass = 0;
}



/*!
	fill a cache line with transformed, light and clip test triangles
	overhead - if primitive is outside or culled, vertexLighting and TextureTransform is still done
*/
void CBurningVideoDriver::VertexCache_fill(const u32 sourceIndex, const u32 destIndex)
{
	const u8* burning_restrict source;
	s4DVertex* burning_restrict dest;

	source = (u8*)VertexShader.vertices + (sourceIndex * VertexShader.vSize[VertexShader.vType].Pitch);

	// it's a look ahead so we never hit it..
	// but give priority...
	//VertexShader.info[ destIndex ].hit = hitCount;

	// store info
	VertexShader.info[destIndex].index = sourceIndex;
	VertexShader.info[destIndex].hit = 0;

	// destination Vertex
	dest = VertexShader.mem.data + s4DVertex_ofs(destIndex);
	dest->reset_interpolate();

	//Irrlicht S3DVertex,S3DVertex2TCoords,S3DVertexTangents
	const S3DVertex* base = ((S3DVertex*)source);
	const core::matrix4* matrix = Transformation[TransformationStack];

	if (Material.VertexShader == BVT_Fix) goto fftransform;

	{
		IBurningShader* shader = (u32)Material.org.MaterialType < MaterialRenderers.size() ?
			(IBurningShader*)MaterialRenderers[Material.org.MaterialType].Renderer : CurrentShader;

		// Vertex program attribute inputs:
		sVec4 gl_Vertex(base->Pos.X, base->Pos.Y, base->Pos.Z, 1.f);
		sVec4 gl_Normal(base->Normal.X, base->Normal.Y, base->Normal.Z, 1.f);
		sVec4 gl_Color; gl_Color.setA8R8G8B8(base->Color.color);

		// Irrlicht TCoords and TCoords2 must be contiguous memory. baseTCoord has no 4 byte aligned start address!
		sVec4 gl_MultiTexCoord[4];
		const sVec2Pack* baseTCoord = (const sVec2Pack*)&base->TCoords.X;
		for (u32 m = 0; m < array_size(gl_MultiTexCoord); ++m)
		{
			if (m < VertexShader.vSize[VertexShader.vType].TexCooSize)
			{
				gl_MultiTexCoord[m].s = baseTCoord[m].x;
				gl_MultiTexCoord[m].t = baseTCoord[m].y;
			}
			else
			{
				gl_MultiTexCoord[m].s = 0.f;
				gl_MultiTexCoord[m].t = 0.f;
			}

			gl_MultiTexCoord[m].p = 1.f;
			gl_MultiTexCoord[m].q = 1.f;
		}

#define gl_MultiTexCoord0 gl_MultiTexCoord[0]
#define gl_MultiTexCoord1 gl_MultiTexCoord[1]
#define gl_MultiTexCoord2 gl_MultiTexCoord[2]
#define gl_MultiTexCoord3 gl_MultiTexCoord[3]
#define gl_Position dest->Pos
#define gl_TexCoord dest->Tex
#define gl_FrontColor dest->Color[0]
#define gl_BackColor dest->Color[1]
#define vec2 sVec2
#define vec3 sVec4
#define vec4 sVec4
#define mat4 core::matrix4
#define gl_NormalMatrix matrix[ETS_NORMAL]
#define gl_ModelViewMatrix matrix[ETS_MODEL_VIEW]
#define gl_ModelViewProjectionMatrix matrix[ETS_MODEL_VIEW_PROJ]
#define ftransform() (matrix[ETS_MODEL_VIEW_PROJ] * gl_Vertex)

#define uniform(var,name) const var& name = (const var&)*shader->getUniform(#name,BL_VERTEX_FLOAT)
#define varying(var,name) var& name = (var&)*shader->getUniform(#name,BL_FRAGMENT_FLOAT)

#ifdef _MSC_VER
#pragma warning (disable: 4244) // float/double conversion
#pragma warning (disable: 4305) // truncation
#endif
		//init for default pixelshader
		gl_FrontColor = gl_Color;

		//gl_FrontColor.setA8R8G8B8(gl_Color);
		if (Material.VertexShader == BVT_815_0x1f847599)
		{
			//varying(vec2,TexCoords);
			gl_Position = gl_Vertex;

			// TexCoords = (gl_Vertex.xy * 0.5 + 0.5);
			gl_TexCoord[0].x = gl_Vertex.x * 0.5f + 0.5f;
			gl_TexCoord[0].y = gl_Vertex.y * -0.5f + 0.5f; // runtime flip
		}
		else if (Material.VertexShader == BVT_opengl_vsh_shaderexample)
		{
			uniform(mat4, mWorldViewProj);
			uniform(mat4, mInvWorld);
			uniform(mat4, mTransWorld);
			uniform(vec3, mLightPos);	// actually just camera-pos in this case
			uniform(vec4, mLightColor);

			gl_Position = mWorldViewProj * gl_Vertex;

			// transform normal somehow (NOTE: for the real vertex normal you would use an inverse-transpose world matrix instead of mInvWorld)
			vec4 normal = vec4(gl_Normal, 0.0);
			normal = mInvWorld * normal;
			normal = normalize(normal);

			// (NOTE: not sure why transposed world is used instead of world?)
			vec4 worldpos = gl_Vertex * mTransWorld;

			vec4 lightVector = worldpos - vec4(mLightPos, 1.0);
			lightVector = normalize(lightVector);

			float tmp2 = dot(-lightVector, normal);

			sVec4 tmp = mLightColor * tmp2;
			gl_FrontColor = gl_BackColor = vec4(tmp.x, tmp.y, tmp.z, 0.0);

			gl_TexCoord[0] = gl_MultiTexCoord0;

			gl_FrontColor.clampf01();
		}
		else if (Material.VertexShader == STK_1259_0xc8226e1a)
		{
			// Creates a bubble (wave) effect by distorting the texture depending on time

			uniform(float, time);
			varying(vec2, uv);

			gl_TexCoord[0] = gl_MultiTexCoord0;
			gl_Position = ftransform();

			float delta_x = cos(time * 3.0) * sin(4.0 * gl_TexCoord[0].st.s * 6.28318531);
			float delta_y = cos(time * 2.0) * sin(3.0 * gl_TexCoord[0].st.t * 6.28318531);

			uv = gl_TexCoord[0].st_op() + vec2(0.02 * delta_x, 0.02 * delta_y);

			//fragment
			uniform(float, transparency);
			gl_TexCoord[0] = uv;
			gl_FrontColor.a *= transparency;

		}
		else if (Material.VertexShader == STK_958_0xa048973b)
		{
			// motion_blur.vert
			gl_TexCoord[0].st_op() = vec2(gl_MultiTexCoord0.s, gl_MultiTexCoord0.t);
			gl_TexCoord[0] = gl_MultiTexCoord0;
			gl_Position = gl_Vertex;
		}
		else if (Material.VertexShader == STK_1309_0x1fd689c2)
		{
			varying(vec3, lightVec);
			varying(vec3, halfVec);
			varying(vec3, eyeVec);

			uniform(vec3, lightdir);

			gl_TexCoord[0] = gl_MultiTexCoord0;

			// Building the matrix Eye Space -> Tangent Space
			vec3 n = normalize(gl_NormalMatrix * gl_Normal);
			vec3 t = normalize(gl_NormalMatrix * gl_MultiTexCoord1.xyz()); // tangent
			vec3 b = cross(n, t);

			vec3 vertexPosition = vec3(gl_ModelViewMatrix * gl_Vertex);

			// transform light and half angle vectors by tangent basis
			vec3 v;
			v.x = dot(lightdir, t);
			v.y = dot(lightdir, b);
			v.z = dot(lightdir, n);
			v.w = 0;
			lightVec = normalize(v);

			v.x = dot(vertexPosition, t);
			v.y = dot(vertexPosition, b);
			v.z = dot(vertexPosition, n);
			v.w = 0;
			eyeVec = normalize(v);

			vertexPosition = normalize(vertexPosition);

			// Normalize the halfVector to pass it to the fragment shader

			// No need to divide by two, the result is normalized anyway.
			// vec3 halfVector = normalize((vertexPosition + lightDir) / 2.0);
			vec3 halfVector = normalize(vertexPosition + lightdir);
			v.x = dot(halfVector, t);
			v.y = dot(halfVector, b);
			v.z = dot(halfVector, n);

			// No need to normalize, t,b,n and halfVector are normal vectors.
			//normalize (v);
			halfVec = v;


			gl_Position = ftransform();
		}
		else if (Material.VertexShader == STK_1204_0x072a4094)
		{
			varying(vec3, normal);
			varying(vec4, vertex_color);
			varying(vec3, lightdir2);
			uniform(vec3, lightdir);

			gl_TexCoord[0] = gl_MultiTexCoord0;
			gl_TexCoord[1] = gl_MultiTexCoord1;
			gl_Position = ftransform();
			vertex_color = gl_Color;

			//normal = normalize(gl_NormalMatrix * gl_Normal);
			normal = normalize(gl_Normal);
			lightdir2 = normalize(lightdir);
		}
		else if (Material.VertexShader == STK_1303_0xd872cdb6)
		{
			// Shader based on work by Fabien Sanglard
			// Released under the terms of CC-BY 3.0

			varying(vec3,lightVec);
			varying(vec3,halfVec);
			varying(vec3,eyeVec);

			uniform(vec3,lightdir);

			//void main()
			{

				gl_TexCoord[0] = gl_MultiTexCoord0;

				// Building the matrix Eye Space -> Tangent Space
				vec3 n = normalize(gl_NormalMatrix * gl_Normal);
				// gl_MultiTexCoord1.xyz
				vec3 t = normalize(gl_NormalMatrix * vec3(1.0, 0.0, 0.0)); // tangent
				vec3 b = cross(n, t);

				vec3 vertexPosition = vec3(gl_ModelViewMatrix * gl_Vertex);

				// transform light and half angle vectors by tangent basis
				vec3 v;
				v.x = dot(lightdir, t);
				v.y = dot(lightdir, b);
				v.z = dot(lightdir, n);
				lightVec = normalize(v);

				vertexPosition = normalize(vertexPosition);

				eyeVec = normalize(-vertexPosition); // we are in Eye Coordinates, so EyePos is (0,0,0)

				// Normalize the halfVector to pass it to the fragment shader

				// No need to divide by two, the result is normalized anyway.
				// vec3 halfVector = normalize((vertexPosition + lightDir) / 2.0);
				vec3 halfVector = normalize(vertexPosition + lightdir);
				v.x = dot(halfVector, t);
				v.y = dot(halfVector, b);
				v.z = dot(halfVector, n);

				// No need to normalize, t,b,n and halfVector are normal vectors.
				//normalize (v);
				halfVec = v;

				gl_Position = ftransform();
			}
		}
#ifdef _MSC_VER
#pragma warning (default: 4244) // conversion
#pragma warning (default: 4305) // truncation
#endif

#undef gl_MultiTexCoord0
#undef gl_MultiTexCoord1
#undef gl_MultiTexCoord2
#undef gl_MultiTexCoord3
#undef vec2
#undef vec3
#undef vec4
#undef mat4
#undef uniform
#undef varying
#undef gl_TexCoord
#undef gl_FrontColor
#undef gl_BackColor
#undef ftransform
#undef gl_NormalMatrix
#undef gl_ModelViewMatrix
#undef gl_ModelViewProjectionMatrix

		goto clipandproject;
	}

fftransform:
	// transform Model * World * Camera * Projection * NDCSpace matrix
	matrix[ETS_MODEL_VIEW_PROJ].transformVect(&dest[0].Pos.x, base->Pos);

/*
	ieee754* p = (ieee754*) &dest[0].Pos.x;
	p[0].fields.frac &= 0xFFFFFFF0;
	p[1].fields.frac &= 0xFFFFFFF0;
	p[2].fields.frac &= 0xFFFFFFF0;
	//p[3].fields.frac &= 0xFFFFFFF0;
*/
	//dest[0].Pos.x = floorf(dest[0].Pos.x * 4096.f + 0.5f) * (1.f / 4096.f);
	//dest[0].Pos.y = floorf(dest[0].Pos.y * 4096.f + 0.5f) * (1.f / 4096.f);
	//dest[0].Pos.z = floorf(dest[0].Pos.z * 4096.f + 0.5f) * (1.f / 4096.f);
	//dest[0].Pos.w = floorf(dest[0].Pos.w * 4096.f + 0.5f) * (1.f / 4096.f);

	//mhm ... maybe no goto
	if (VertexShader.vType == E4VT_SHADOW)
	{
		//core::vector3df i = base->Pos;
		//i.Z -= 0.5f;
		//matrix[ETS_MODEL_VIEW_PROJ].transformVect(&dest->Pos.x, i);

		//GL_DEPTH_CLAMP,EVDF_DEPTH_CLAMP
		//if ( dest->Pos.z < dest->Pos.w)
		//	dest->Pos.z = dest->Pos.w*0.99f;

		//glPolygonOffset // self shadow wanted or not?
		dest->Pos.w *= 1.005f;

		//flag |= v->Pos.z <= v->Pos.w ? VERTEX4D_CLIP_NEAR : 0;
		//flag |= -v->Pos.z <= v->Pos.w ? VERTEX4D_CLIP_FAR : 0;

		goto clipandproject;
	}


#if defined (SOFTWARE_DRIVER_2_LIGHTING) || defined ( SOFTWARE_DRIVER_2_TEXTURE_TRANSFORM )

	// vertex, normal in light(eye) space
	if (EyeSpace.TL_Flag & (TL_TEXTURE_TRANSFORM | TL_FOG | TL_LIGHT))
	{
		sVec4 vertex4; //eye coordinate position of vertex
		matrix[ETS_MODEL_VIEW].transformVect(&vertex4.x, base->Pos);

		f32 iw = reciprocal_zero_pos_underflow(vertex4.w);
		EyeSpace.vertex.x = vertex4.x * iw;
		EyeSpace.vertex.y = vertex4.y * iw;
		EyeSpace.vertex.z = vertex4.z * iw;
		EyeSpace.vertex.w = iw;

		//EyeSpace.cam_distance = EyeSpace.vertex.length_xyz();
/*
		if ( GL_LIGHT_MODEL_LOCAL_VIEWER == 0 )
		{
			EyeSpace.cam_dir.x = 0.f;
			EyeSpace.cam_dir.y = 0.f;
			EyeSpace.cam_dir.z = 1.f;
		}
*/
		EyeSpace.vertexn = EyeSpace.vertex;
		EyeSpace.vertexn.normalize_dir_xyz();

		//matrix[ETS_NORMAL].rotateVect(&EyeSpace.normal.x, base->Normal);
		rotateMat33Vec3Vec4(matrix[ETS_NORMAL], &EyeSpace.normal.x, &base->Normal.X);
		if (EyeSpace.TL_Flag & TL_NORMALIZE_NORMALS)
		{
			EyeSpace.normal.normalize_dir_xyz_zero();
		}

	}

#endif


#if BURNING_MATERIAL_MAX_COLORS > 0
	// apply lighting model
#if defined (SOFTWARE_DRIVER_2_LIGHTING)
	if (EyeSpace.TL_Flag & TL_LIGHT)
	{
		lightVertex_eye(dest, base->Color.color);
	}
	else
	{
		dest->Color[0].setA8R8G8B8(base->Color.color);
	}
#else
	dest->Color[0].setA8R8G8B8(base->Color.color);
#endif
#endif

	//vertex fog
	if (EyeSpace.TL_Flag & TL_FOG) //Material.org.FogEnable
	{
		f32 fog_factor = 1.f;

		// GL_FRAGMENT_DEPTH -> abs(EyeSpace.vertex.z)
		ieee754 fog_frag_coord;
		fog_frag_coord.f = EyeSpace.vertex.z;
		fog_frag_coord.fields.sign = 0;

		switch (FogType)
		{
		case EFT_FOG_LINEAR:
			fog_factor = (FogEnd - fog_frag_coord.f) * EyeSpace.fog_scale;
			break;
		case EFT_FOG_EXP:
			fog_factor = (f32)exp(-FogDensity * fog_frag_coord.f);
			break;
		case EFT_FOG_EXP2:
			fog_factor = (f32)exp(-FogDensity * FogDensity * fog_frag_coord.f * fog_frag_coord.f);
			break;
		}

		sVec4* a = dest->Color + (((VertexShader.vSize[VertexShader.vType].Format & VERTEX4D_FORMAT_MASK_COLOR)>=VERTEX4D_FORMAT_COLOR_2_FOG) ? 1 : 0);
		a->a = clampf01(fog_factor);
	}

	// Texture Coo Generation and Transform
	for (u32 m = 0; m < VertexShader.vSize[VertexShader.vType].TexSize; ++m)
	{
		sVec4 r;
		f32 tx, ty;

		// texgen
		const size_t& flag = TransformationFlag[TransformationStack][ETS_TEXTURE_0 + m];
		if (flag & ETF_TEXGEN_CAMERA_SPHERE)
		{
			//reflect(u,N) u - 2.0 * dot(N, u) * N

			const sVec4& u = EyeSpace.vertexn; // EyeSpace.vertex.normalized
			const sVec4& n = EyeSpace.normal;

			f32 dot = -2.f * n.dot_xyz(u);
			r.x = u.x + dot * n.x;
			r.y = u.y + dot * n.y;
			r.z = u.z + dot * n.z;

			//openGL
			f32 m = 2.f * sqrtf(r.x * r.x + r.y * r.y + (r.z + 1.f) * (r.z + 1.f));
			tx = r.x / m + 0.5f;
			ty = -(r.y / m + 0.5f); 	// tex flipped


			/*
			//~d3d with spheremap scale
			f32 m = 0.25f / (0.00001f + sqrtf(r.x*r.x+r.y*r.y+r.z*r.z));
			dest[0].Tex[t].x =  r.x * m + 0.5f;
			dest[0].Tex[t].y = -r.y * m + 0.5f;
			*/
		}
		else if (flag & ETF_TEXGEN_CAMERA_REFLECTION)
		{
			//reflect(u,N) u - 2.0 * dot(N, u) * N

			const sVec4& u = EyeSpace.vertexn; // EyeSpace.vertex.normalized
			const sVec4& n = EyeSpace.normal;

			f32 dot = -2.f * n.dot_xyz(u);

			//openGL
			tx = /*r.x =*/ u.x + dot * n.x;
			ty = /*r.y =*/ u.y + dot * n.y;
			//r.z = u.z + dot * n.z;

			//~d3d with spheremap transform
			//tx = r.x * 0.5f + 0.5f;
			//ty = r.y * -0.5f + 0.5f;
		}
		else if (m < VertexShader.vSize[VertexShader.vType].TexCooSize)
		{
			// Irrlicht TCoords and TCoords2 must be contiguous memory. baseTCoord has no 4 byte aligned start address!
			const sVec2Pack* baseTCoord = (const sVec2Pack*)&base->TCoords.X;

			tx = baseTCoord[m].x;
			ty = baseTCoord[m].y;
		}
		else
		{
			tx = 0.f;
			ty = 0.f;
		}

#if 0
		static const CSoftwareTexture2_Bound empty_bound = { 0.f,0.f,0.f,0.f,0 };

		const video::CSoftwareTexture2* tex = MAT_TEXTURE(t);
		const CSoftwareTexture2_Bound& texb = tex ? tex->getTexBound_index()[0] : empty_bound;
		const bool filter = Material.org.TextureLayer[t].BilinearFilter;
#endif

		//Texture Matrix Transform
		if (flag & ETF_TEXGEN_MATRIX) // !(flag & ETF_IDENTITY)
		{
			/*
				Generate texture coordinates as linear functions so that:
					u = Ux*x + Uy*y + Uz*z + Uw
					v = Vx*x + Vy*y + Vz*z + Vw
				The matrix M for this case is:
					Ux  Vx  0  0
					Uy  Vy  0  0
					Uz  Vz  0  0
					Uw  Vw  0  0
			*/

			const f32* M = matrix[ETS_TEXTURE_0 + m].pointer();

			f32 _tx = tx;
			f32 _ty = ty;
			tx = M[0] * _tx + M[4] * _ty + M[8];
			ty = M[1] * _tx + M[5] * _ty + M[9];
		}

		switch (Material.org.TextureLayer[m].TextureWrapU)
		{
		case ETC_CLAMP:
			tx = clampf01(tx);
			break;

		case ETC_CLAMP_TO_EDGE:
		case ETC_CLAMP_TO_BORDER:
			tx = clampf01(tx);
			break;
		case ETC_MIRROR:
			if (core::fract(tx) > 0.5f)
				tx = 1.f - tx;
			break;
		case ETC_MIRROR_CLAMP:
		case ETC_MIRROR_CLAMP_TO_EDGE:
		case ETC_MIRROR_CLAMP_TO_BORDER:
			tx = clampf01(tx);
			if (core::fract(tx) > 0.5f)
				tx = 1.f - tx;
			break;
		case ETC_REPEAT:
			// texel access is always modulo
		default:
			break;
		}
		switch (Material.org.TextureLayer[m].TextureWrapV)
		{
		case ETC_CLAMP:
			ty = clampf01(ty);
			break;
		case ETC_CLAMP_TO_EDGE:
		case ETC_CLAMP_TO_BORDER:
			//if (ty < 0.f) ty = 0.f;
			//else if (ty > texb.pixelclampy) ty = texb.pixelclampy;

			//ty = clampfuv(ty, filter ? texb.pixelclampy : 0.f);
			ty = clampf01(ty);
			break;
		case ETC_MIRROR:
			if (core::fract(ty) > 0.5f)
				ty = 1.f - ty;
			break;
		case ETC_MIRROR_CLAMP:
		case ETC_MIRROR_CLAMP_TO_EDGE:
		case ETC_MIRROR_CLAMP_TO_BORDER:
			ty = clampf01(ty);
			if (core::fract(ty) > 0.5f)
				ty = 1.f - ty;
			break;
		case ETC_REPEAT:
			// texel access is always modulo
		default:
			break;
		}

		dest->Tex[m].x = tx;
		dest->Tex[m].y = ty;
	}


#if BURNING_MATERIAL_MAX_LIGHT_TANGENT > 0
	if ((EyeSpace.TL_Flag & TL_LIGHT0_IS_NORMAL_MAP) &&
		((VertexShader.vSize[VertexShader.vType].Format & VERTEX4D_FORMAT_MASK_TANGENT) >= VERTEX4D_FORMAT_BUMP_DOT3)
		)
	{
		const S3DVertexTangents* tangent = ((S3DVertexTangents*)source);

		sVec4 vp;

		sVec4 light_accu;
		light_accu.set(0.f);

		/*
		* Color[0] lightcolor[0] a: vertexalpha
		* Color[1] lightcolor[1] a: fogdistance
		* Color[2] lightvector[0] 
		* Color[3] lightvector[1] 
		* LightVector[0] eyevector
		*/

		if ((VertexShader.vSize[VertexShader.vType].Format & VERTEX4D_FORMAT_MASK_TANGENT) >= VERTEX4D_FORMAT_PARALLAX)
		{
			vp.x = EyeSpace.leye.x - base->Pos.X;
			vp.y = EyeSpace.leye.y - base->Pos.Y;
			vp.z = EyeSpace.leye.z - base->Pos.Z;

			light_accu.x = vp.dot(tangent->Tangent);
			light_accu.y = -vp.dot(tangent->Binormal);
			light_accu.z = -vp.dot(tangent->Normal);
			light_accu.normalize_pack_xyz(dest->LightTangent[0], 1.f, 0.f);
		}

		const u32 maxLight = core::min_((u32)BURNING_MATERIAL_MAX_COLORS,(u32)2, EyeSpace.Light.size());
		for (u32 i = 0; i < maxLight; ++i)
		{
			const SBurningShaderLight& light = EyeSpace.Light[i];
			if (!light.LightIsOn)
				continue;

			vp.x = light.pos_local.x - base->Pos.X;
			vp.y = light.pos_local.y - base->Pos.Y;
			vp.z = light.pos_local.z - base->Pos.Z;

			f32 attenuation = inversesqrt(vp.dot_xyz(vp) * light.nmap_linearAttenuation);
			dest->Color[i].r = light.DiffuseColor.r * attenuation;
			dest->Color[i].g = light.DiffuseColor.g * attenuation;
			dest->Color[i].b = light.DiffuseColor.b * attenuation;
			dest->Color[i].clampf01();

			// lightvector transform by tangent matrix
#if BURNING_MATERIAL_MAX_COLORS >=4
			dest->Color[2+i].x = vp.dot(tangent->Tangent);
			dest->Color[2+i].y = vp.dot(tangent->Binormal);
			dest->Color[2+i].z = vp.dot(tangent->Normal);
			dest->Color[2 + i].normalize_dir_xyz_zero();
#else
			light_accu.x += (vp.x * tangent->Tangent.X + vp.y * tangent->Tangent.Y + vp.z * tangent->Tangent.Z);
			light_accu.y += (vp.x * tangent->Binormal.X + vp.y * tangent->Binormal.Y + vp.z * tangent->Binormal.Z);
			light_accu.z += (vp.x * tangent->Normal.X + vp.y * tangent->Normal.Y + vp.z * tangent->Normal.Z);
#endif
		}

#if BURNING_MATERIAL_MAX_COLORS >=4
#else
		//normalize [-1,+1] to [0,1] -> obsolete
		light_accu.normalize_pack_xyz(dest->LightTangent[0], 1.f, 0.f);
#endif
		dest->Tex[1].x = dest->Tex[0].x;
		dest->Tex[1].y = dest->Tex[0].y;

	}
	else if (EyeSpace.TL_Flag & TL_LIGHT)
	{
		//dest->LightTangent[0].x = 0.f;
		//dest->LightTangent[0].y = 0.f;
		//dest->LightTangent[0].z = 0.f;
	}
#endif //if BURNING_MATERIAL_MAX_LIGHT_TANGENT > 0

	//#endif // SOFTWARE_DRIVER_2_TEXTURE_TRANSFORM

clipandproject:

	// test vertex visibility
	const u32 flag = clipToFrustumTest(dest) | VertexShader.vSize[VertexShader.vType].Format;

	dest[s4DVertex_ofs(0)].flag =
		dest[s4DVertex_pro(0)].flag = flag;

	// to DC Space, project homogenous vertex
	if ((flag & VERTEX4D_CLIPMASK) == VERTEX4D_INSIDE)
	{
		ndc_2_dc_and_project(dest, s4DVertex_ofs(1), Transformation_ETS_CLIPSCALE[TransformationStack]);
	}

}


void SVertexShader::setIndices(const void* _indices, const video::E_INDEX_TYPE _iType)
{
	indices = _indices;
	indicesIndex = 0;
	indicesRun = 0;

	switch (_iType)
	{
	case EIT_16BIT: iType = E4IT_16BIT; break;
	case EIT_32BIT: iType = E4IT_32BIT; break;
	default: iType = (e4DIndexType)iType; break;
	}
	if (!indices)
		iType = E4IT_NONE;
}

void SVertexShader::setPrimitiveType(const scene::E_PRIMITIVE_TYPE primitiveType, const u32 primitiveCount)
{
	pType = primitiveType;
	primitiveHasVertex = 3;
	indicesPitch = 1;

	switch (pType)
	{
	default:
	case scene::EPT_POINTS:
	case scene::EPT_POINT_SPRITES:
		indexCount = primitiveCount;
		indicesPitch = 1;
		primitiveHasVertex = 1;
		break;

	case scene::EPT_LINE_STRIP:
	case scene::EPT_LINE_LOOP:
		indexCount = primitiveCount + 1;
		indicesPitch = 1;
		primitiveHasVertex = 2;
		break;
	case scene::EPT_LINES:
		indexCount = 2 * primitiveCount;
		indicesPitch = 2;
		primitiveHasVertex = 2;
		break;
	case scene::EPT_TRIANGLE_STRIP:
		indexCount = primitiveCount + 2;
		indicesPitch = 1;
		primitiveHasVertex = 3;
		break;
	case scene::EPT_TRIANGLES:
		indexCount = primitiveCount + primitiveCount + primitiveCount;
		indicesPitch = 3;
		primitiveHasVertex = 3;
		break;
	case scene::EPT_TRIANGLE_FAN:
		indexCount = primitiveCount + 2;
		indicesPitch = 1;
		primitiveHasVertex = 3;
		break;
	case scene::EPT_POLYGON:
		indexCount = primitiveCount;
		indicesPitch = 1;
		primitiveHasVertex = 3; // drawn a triangle fan
		break;

	case scene::EPT_QUAD_STRIP:
		indexCount = 2 * primitiveCount + 2;
		indicesPitch = 2;
		primitiveHasVertex = 4;
		break;
	case scene::EPT_QUADS:
		indexCount = 4 * primitiveCount;
		indicesPitch = 4;
		primitiveHasVertex = 4;
		//draw two triangles..
		break;
	}
}

void SVertexShader::set_info_miss()
{
	//memset(info, VERTEXCACHE_MISS, sizeof(info));
	for (size_t i = 0; i != VERTEXCACHE_ELEMENT; ++i)
	{
		info[i].hit = VERTEXCACHE_MISS;
		info[i].index = VERTEXCACHE_MISS;
	}
}

// get the next unique index cache line
void SVertexShader::get_next_index_cacheline()
{
	u32 i;

	// cache element 0
	switch (pType)
	{
	case scene::EPT_POLYGON:
	case scene::EPT_TRIANGLE_FAN:
		fillIndex = indicesRun ? 1 : 0;
		break;
	default:
		fillIndex = 0;
		break;
	}

	// set_info_temp_miss
	for (i = fillIndex; i != VERTEXCACHE_ELEMENT; ++i)
	{
		info_temp[i].hit = VERTEXCACHE_MISS;
		info_temp[i].index = VERTEXCACHE_MISS;
	}

	// rewind to start of primitive
	indicesIndex = indicesRun;

	while (indicesIndex < indexCount && fillIndex < VERTEXCACHE_ELEMENT)
	{
		u32 sourceIndex = index(indicesIndex);
		indicesIndex += 1;

		// if not exist, push back
		u32 exist = 0;
		for (u32 dIndex = 0; dIndex < fillIndex; ++dIndex)
		{
			if (info_temp[dIndex].index == sourceIndex)
			{
				exist = 1;
				break;
			}
		}

		if (0 == exist)
		{
			info_temp[fillIndex].index = sourceIndex;
			fillIndex += 1;
		}
	}

	// clear marks
	for (i = 0; i != VERTEXCACHE_ELEMENT; ++i)
	{
		info[i].hit = 0;
	}

	// mark all existing
	for (i = 0; i != fillIndex; ++i)
	{
		for (u32 dIndex = 0; dIndex < VERTEXCACHE_ELEMENT; ++dIndex)
		{
			if (info[dIndex].index == info_temp[i].index)
			{
				info_temp[i].hit = dIndex;
				info[dIndex].hit = 1;
				break;
			}
		}
	}
}


/*
	Cache based on linear walk indices
	fill blockwise on the next 16(Cache_Size) unique vertices in indexlist
	merge the next 16 vertices with the current
*/
void SVertexShader::getPrimitive(s4DVertexPair* face[4], CBurningVideoDriver* driver)
{
	// next primitive must be complete in cache
	if (indicesIndex - indicesRun < primitiveHasVertex && indicesIndex < indexCount)
	{
		// get the next unique indices cache line
		get_next_index_cacheline();

		// fill new
		for (u32 i = 0; i != fillIndex; ++i)
		{
			if (info_temp[i].hit != VERTEXCACHE_MISS)
				continue;

			for (u32 dIndex = 0; dIndex < VERTEXCACHE_ELEMENT; ++dIndex)
			{
				if (0 == info[dIndex].hit)
				{
					driver->VertexCache_fill(info_temp[i].index, dIndex);
					info[dIndex].hit += 1;
					info_temp[i].hit = dIndex;
					break;
				}
			}
		}
	}

	// all primitive indices are in the index cache line
	switch (pType)
	{
	case scene::EPT_POLYGON:
	case scene::EPT_TRIANGLE_FAN:
		face[0] = vertex(index(0));
		face[1] = vertex(index(indicesRun + 1));
		face[2] = vertex(index(indicesRun + 2));
		break;
	case scene::EPT_TRIANGLE_STRIP:
		face[0] = vertex(index(indicesRun + 0));
		face[(primitiveRun & 1) ? 2 : 1] = vertex(index(indicesRun + 1));
		face[(primitiveRun & 1) ? 1 : 2] = vertex(index(indicesRun + 2));
		break;
	default:
		for (u32 i = 0; i < primitiveHasVertex; ++i)
		{
			face[i] = vertex(index(indicesRun + i));
		}
		break;
	}
	indicesRun += indicesPitch;
}


/*!
*/
int CBurningVideoDriver::VertexCache_reset(const void* vertices, u32 vertexCount,
	const void* indices, u32 primitiveCount,
	E_VERTEX_TYPE vType,
	scene::E_PRIMITIVE_TYPE pType,
	E_INDEX_TYPE iType)
{

	if (0 == CurrentShader)
	{
		return 1;
	}

	VertexShader.vertices = vertices;
	VertexShader.vertexCount = vertexCount;

	switch (Material.org.MaterialType) // (Material.Fallback_MaterialType)
	{
	case EMT_PARALLAX_MAP_SOLID:
	case EMT_PARALLAX_MAP_TRANSPARENT_ADD_COLOR:
	case EMT_PARALLAX_MAP_TRANSPARENT_VERTEX_ALPHA:
		VertexShader.vType = vType == EVT_TANGENTS ? E4VT_TANGENTS_PARALLAX : (e4DVertexType)vType;
		break;
	case EMT_REFLECTION_2_LAYER:
	case EMT_TRANSPARENT_REFLECTION_2_LAYER:
		VertexShader.vType = vType == EVT_STANDARD ? E4VT_REFLECTION_MAP : (e4DVertexType)vType;
		break;
	default:
		VertexShader.vType = (e4DVertexType)vType;
		break;
	}

	//check material material->OnRender(VertexType)
	SVSize* vSize = VertexShader.vSize;
	for (int m = (int)vSize[VertexShader.vType].TexSize - 1; m >= 0; --m)
	{
		const ITexture* tex = MAT_TEXTURE(m);
		if (!tex)
		{
			//vSize[E4VT_NO_TEXTURE] = vSize[VertexShader.vType];
			vSize[E4VT_NO_TEXTURE].Format = (vSize[VertexShader.vType].Format & ~VERTEX4D_FORMAT_MASK_COLOR) | VERTEX4D_FORMAT_COLOR_1;
			vSize[E4VT_NO_TEXTURE].Pitch = vSize[VertexShader.vType].Pitch;
			vSize[E4VT_NO_TEXTURE].TexSize = m;
			vSize[E4VT_NO_TEXTURE].TexCooSize = m;

			VertexShader.vType = E4VT_NO_TEXTURE;
			//flags downconvert?
		}
	}

	VertexShader.setIndices(indices, iType);
	VertexShader.setPrimitiveType(pType, primitiveCount);

	VertexShader.set_info_miss();
	return 0;
}


//! draws a vertex primitive list
void CBurningVideoDriver::drawVertexPrimitiveList(const void* vertices, u32 vertexCount,
	const void* indexList, u32 primitiveCount,
	E_VERTEX_TYPE vType, scene::E_PRIMITIVE_TYPE pType, E_INDEX_TYPE iType)

{
	if (!checkPrimitiveCount(primitiveCount))
		return;

	CNullDriver::drawVertexPrimitiveList(vertices, vertexCount, indexList, primitiveCount, vType, pType, iType);

	if (VertexCache_reset(vertices, vertexCount, indexList, primitiveCount, vType, pType, iType))
		return;

	pushShader(pType, 1);

	//Matrices needed for this primitive
	transform_calc(ETS_MODEL_VIEW_PROJ);
	if ((EyeSpace.TL_Flag & (TL_TEXTURE_TRANSFORM | TL_FOG | TL_LIGHT)) ||
		Material.VertexShader != BVT_Fix)
	{
		transform_calc(ETS_MODEL_VIEW);
		transform_calc(ETS_NORMAL);
	}

	//objectspace
	if (EyeSpace.TL_Flag & TL_LIGHT0_IS_NORMAL_MAP)
	{
		transform_calc(ETS_MODEL_INVERSE);

		const core::matrix4* matrix = Transformation[TransformationStack];
		if ((VertexShader.vSize[VertexShader.vType].Format & VERTEX4D_FORMAT_MASK_TANGENT) >= VERTEX4D_FORMAT_PARALLAX)
		{
			transform_calc(ETS_MODEL_VIEW);
			mat44_inverse(EyeSpace.mvi, matrix[ETS_MODEL_VIEW]);

			sVec4 eye(0.f, 0.f, 0.f, 1.f);
			transformVec3Vec3(EyeSpace.mvi, &EyeSpace.leye.x, &eye.x);
		}

		const u32 maxLight = core::min_((u32)BURNING_MATERIAL_MAX_COLORS, (u32)2, EyeSpace.Light.size());
		for (u32 i = 0; i < maxLight; ++i)
		{
			SBurningShaderLight& light = EyeSpace.Light[i];
			if (!light.LightIsOn)
				continue;

			transformVec3Vec3(matrix[ETS_MODEL_INVERSE], &light.pos_local.x, &light.pos.x);
		}

	}

	if ((u32)Material.org.MaterialType < MaterialRenderers.size())
	{
		MaterialRenderers[Material.org.MaterialType].Renderer->OnRender(this, (video::E_VERTEX_TYPE)VertexShader.vType);
	}

	s4DVertexPair* face[4];

	u32 vOut;
	u32 vertex_from_clipper; // from VertexShader or CurrentOut
	u32 has_vertex_run;

	// magnitude crossproduct (area of parallelogram * 0.5 = triangle screen size, winding)
	ieee754 dc_area;

	CurrentShader->fragment_draw_count = 0;
	for (VertexShader.primitiveRun = 0; VertexShader.primitiveRun < primitiveCount; ++VertexShader.primitiveRun)
	{
		//collect pointer to face vertices
		VertexShader.getPrimitive(face, this);

		size_t clipMask_i;
		size_t clipMask_o;

		clipMask_i = face[0]->flag;
		clipMask_o = face[0]->flag;

		for (has_vertex_run = 1; has_vertex_run < VertexShader.primitiveHasVertex; ++has_vertex_run)
		{
			clipMask_i |= face[has_vertex_run]->flag; // if fully outside or outside on same side
			clipMask_o &= face[has_vertex_run]->flag; // if fully inside
		}

		clipMask_i &= VERTEX4D_CLIPMASK;
		clipMask_o &= VERTEX4D_CLIPMASK;

		if (clipMask_i != VERTEX4D_INSIDE)
		{
			// if primitive fully outside or outside on same side
			continue;
			//vOut = 0;
			//vertex_from_clipper = 0;
		}
		else if (clipMask_o == VERTEX4D_INSIDE)
		{
			// if primitive fully inside
			vOut = VertexShader.primitiveHasVertex;
			vertex_from_clipper = 0;
		}
		else
#if defined(SOFTWARE_DRIVER_2_CLIPPING)
		{
			// else if not complete inside clipping necessary

			// todo: clipping should reuse vertexcache (try to minimize clipping)
			for (has_vertex_run = 0; has_vertex_run < VertexShader.primitiveHasVertex; ++has_vertex_run)
			{
				memcpy_s4DVertexPair(Clipper.data + s4DVertex_ofs(has_vertex_run), face[has_vertex_run]);
			}

			//clipping should happen in R^3 before perspective divide, avoid flipping points
			//x = A_x * (1 - da/(da - db)) + A_y * (da/(da-db))
			vOut = clipToFrustum(VertexShader.primitiveHasVertex);
			vertex_from_clipper = 1;


			// to DC Space, project homogenous vertex
			if (vOut > VertexShader.primitiveHasVertex )
			ndc_2_dc_and_project_grid(Clipper.data, s4DVertex_ofs(vOut), Transformation_ETS_CLIPSCALE[TransformationStack]);
			else
			ndc_2_dc_and_project(Clipper.data, s4DVertex_ofs(vOut), Transformation_ETS_CLIPSCALE[TransformationStack]);

		}
#else
		{
			continue;
			vOut = 0;
			vertex_from_clipper = 0;
		}
#endif

#if BURNING_MATERIAL_MAX_TEXTURES > 0
		s32 lod_max[BURNING_MATERIAL_MAX_TEXTURES];
		for (u32 m = 0; m < VertexShader.vSize[VertexShader.vType].TexSize; ++m)
		{
			lod_max[m] = 0;
		}
#endif

		f32 t[4];


#define BURNING_MAX_MIP_CLIPPER 1

#if BURNING_MAX_MIP_CLIPPER == 1
		//select largest texture for clipped triangle
		//very small long triangles are very undersampled here ("skybox flicker")
		int use_max_mip = vertex_from_clipper && VertexShader.vSize[VertexShader.vType].TexSize &&
			vOut > VertexShader.primitiveHasVertex ? 1 : 0;

		for (int probe = use_max_mip; probe >= 0; probe -= 1)
#endif
		{
			// re-tesselate
			for (has_vertex_run = 0; (has_vertex_run + VertexShader.primitiveHasVertex) <= vOut; has_vertex_run += 1)
			{
				// set from clipped geometry ( triangle fan 0-1-2,0-2-3.. )
				if (vertex_from_clipper)
				{
					face[0] = Clipper.data + s4DVertex_ofs(0);
					face[1] = Clipper.data + s4DVertex_ofs(has_vertex_run + 1);
					face[2] = Clipper.data + s4DVertex_ofs(has_vertex_run + 2);
					face[3] = Clipper.data + s4DVertex_ofs(has_vertex_run + 3);
				}

				//area of primitive in device space
				// projected triangle screen area is used for culling ( sign of normal ) and mipmap selection
				//f32 dc_area = screenarea_inside(face);

				// magnitude crossproduct
				dc_area.f = 1.f;
				if (VertexShader.primitiveHasVertex >= 3)
				{
					const sVec4& v0 = (face[0] + s4DVertex_pro(0))->Pos;
					const sVec4& v1 = (face[1] + s4DVertex_pro(0))->Pos;
					const sVec4& v2 = (face[2] + s4DVertex_pro(0))->Pos;

					//dc_area = 2d triangle normal.crossproduct (a.x * b.y - b.x * a.y).length/2;
					dc_area.f = ((v1.x - v0.x) * (v2.y - v0.y) - (v2.x - v0.x) * (v1.y - v0.y)) /* * 0.5f */;

					//geometric clipping has problem with invisible or very small Triangles
					//size_t sign = dc_area < 0.001f ? CULL_BACK : dc_area > 0.001f ? CULL_FRONT : CULL_INVISIBLE;

					size_t sign = dc_area.fields.sign ? CULL_BACK : CULL_FRONT;
					sign |= dc_area.abs.frac_exp < CULL_EPSILON_00001 ? CULL_INVISIBLE : 0;

					if (Material.CullFlag & sign)
						continue; //not break; per clipper triangle


				// select mipmap
#if BURNING_MAX_MIP_CLIPPER == 1
				if (probe == use_max_mip)
#endif
					for (u32 m = 0; m < VertexShader.vSize[VertexShader.vType].TexSize; ++m)
					{
						video::CSoftwareTexture2* tex = MAT_TEXTURE(m);

						const sVec2& v0 = (face[0] + s4DVertex_ofs(0))->Tex[m];
						const sVec2& v1 = (face[1] + s4DVertex_ofs(0))->Tex[m];
						const sVec2& v2 = (face[2] + s4DVertex_ofs(0))->Tex[m];

						//todo: get triangle setup here
						//bbox
						t[0] = t[2] = v0.x;
						t[1] = t[3] = v0.y;
						if (v1.x < t[0]) t[0] = v1.x;
						if (v1.y < t[1]) t[1] = v1.y;
						if (v1.x > t[2]) t[2] = v1.x;
						if (v1.y > t[3]) t[3] = v1.y;

						if (v2.x < t[0]) t[0] = v2.x;
						if (v2.y < t[1]) t[1] = v2.y;
						if (v2.x > t[2]) t[2] = v2.x;
						if (v2.y > t[3]) t[3] = v2.y;

						f32 tex_area = fabsf((t[2] - t[0]) * (t[3] - t[1]));

						//tex_area = |a.x * b.y - b.x * a.y| * 0.5; // crossproduct
						//f32 tex_area = fabsf((v1.x - v0.x) * (v2.y - v0.y) - (v2.x - v0.x) * (v1.y - v0.y));

						//various over and underflow cases
						if (tex_area <= 0.000001f)
							tex_area = 0.000001f;
						else if (tex_area > 1.01f)
							tex_area = 1.f / tex_area;

						/* 2.f * tex_area * 1.6f bias. 1.6 too much for detailsmap3 */
						//not dc_area * 0.5 cancel out 2 * TexBias
						const u32 dc_area_over_tex_area = (u32)floorf( /*/tex_area > 0.0000001f ? */
							fabsf(dc_area.f) * TexBias[TransformationStack] / tex_area
							/*: 0.f*/
						);

						// get a near 1:1 ratio between pixel and texel
						// tex_area * b[lodFactor].w * b[lodFactor].h > dc_area_abs

						s32 lodFactor = 0;
						const CSoftwareTexture2_Bound* b = tex->getTexBound_index();
						while (lodFactor < SOFTWARE_DRIVER_2_MIPMAPPING_MAX &&
							b[lodFactor].area > dc_area_over_tex_area
							)
						{
							lodFactor += 1;
						}

						//clipped triangle should take single area based mipmap from unclipped face
						//skybox,billboard test case 
						//if (vertex_from_clipper) lodFactor -= 1;
						if (has_vertex_run == 0) lod_max[m] = lodFactor;
						else if (lodFactor < lod_max[m]) lod_max[m] = lodFactor;

						//CurrentShader->setTextureParam(m, tex, lodFactor);
						//select_polygon_mipmap_inside(face, m, tex->getTexBound());
					}

				}
				//else /* if (VertexShader.primitiveHasVertex == 3) */
#if BURNING_MAX_MIP_CLIPPER == 1
				if (probe > 0)
					continue;
#endif
				// set single mipmap
				for (u32 m = 0; m < VertexShader.vSize[VertexShader.vType].TexSize; ++m)
				{
					video::CSoftwareTexture2* tex = MAT_TEXTURE(m);
					CurrentShader->setTextureParam(m, tex, lod_max[m]);

					//select_polygon_mipmap_inside(face, m, tex->getTexBound());
					//currently shader receives texture coordinate as Pixelcoo of 1 Texture
					const CSoftwareTexture2_Bound& b = tex->getTexBound();
					for (u32 v = 0; v < VertexShader.primitiveHasVertex; ++v)
					{
						const sVec2& src = (face[v] + s4DVertex_ofs(0))->Tex[m];
						sVec2& dst = (face[v] + s4DVertex_pro(0))->Tex[m];

#ifdef SOFTWARE_DRIVER_2_PERSPECTIVE_CORRECT
						const f32 iw = (face[v] + s4DVertex_pro(0))->Pos.w;
						dst.x = src.x * iw * b.mat[0] + b.mat[1];
						dst.y = src.y * iw * b.mat[2] + b.mat[3];
#else
						dst.x = src.x * b.mat[0] + b.mat[1];
						dst.y = src.y * b.mat[2] + b.mat[3];
#endif 
					}
				}

				switch (VertexShader.primitiveHasVertex)
				{
				case 1:
					CurrentShader->drawPoint(face[0] + s4DVertex_pro(0));
					break;
				case 2:
					CurrentShader->drawLine(face[0] + s4DVertex_pro(0), face[1] + s4DVertex_pro(0));
					break;
				case 3:
					CurrentShader->drawWireFrameTriangle(face[0] + s4DVertex_pro(0), face[1] + s4DVertex_pro(0), face[2] + s4DVertex_pro(0));
					break;
				case 4:
					//todo:
					CurrentShader->drawWireFrameTriangle(face[0] + s4DVertex_pro(0), face[1] + s4DVertex_pro(0), face[2] + s4DVertex_pro(0));
					CurrentShader->drawWireFrameTriangle(face[0] + s4DVertex_pro(0), face[2] + s4DVertex_pro(0), face[3] + s4DVertex_pro(0));
					break;
				}

				//vertex_from_clipper = 1;
			}
		} // probe

	}

	this->samples_passed += CurrentShader->fragment_draw_count;

	//release texture
	for (u32 m = 0; m < VertexShader.vSize[VertexShader.vType].TexSize; ++m)
	{
		CurrentShader->setTextureParam(m, 0, 0);
	}
}


//! Sets the dynamic ambient light color. The default color is
//! (0,0,0,0) which means it is dark.
//! \param color: New color of the ambient light.
void CBurningVideoDriver::setAmbientLight(const SColorf& color)
{
	EyeSpace.Global_AmbientLight.setColorf(color);
}


void CBurningVideoDriver::assignHardwareLight(SBurningShaderLight& l, const SLight& dl)
{
	//	l.org = dl;
	l.Type = dl.Type;
	l.LightIsOn = true;

	l.AmbientColor.setColorf(dl.AmbientColor);
	l.DiffuseColor.setColorf(dl.DiffuseColor);
	l.SpecularColor.setColorf(dl.SpecularColor);

	//should always be valid?
	sVec4 nDirection;
	nDirection.x = dl.Direction.X;
	nDirection.y = dl.Direction.Y;
	nDirection.z = dl.Direction.Z;
	nDirection.normalize_dir_xyz();

	switch (dl.Type)
	{
	case ELT_DIRECTIONAL:
		l.pos.x = -nDirection.x;
		l.pos.y = -nDirection.y;
		l.pos.z = -nDirection.z;
		l.pos.w = 0.f;

		l.constantAttenuation = 1.f;
		l.linearAttenuation = 0.f;
		l.quadraticAttenuation = 0.f;

		l.spotDirection.x = 0.f;
		l.spotDirection.y = 0.f;
		l.spotDirection.z = -1.f;
		l.spotDirection.w = 0.f;
		l.spotCosCutoff = -1.f;
		l.spotCosInnerCutoff = 1.f;
		l.spotExponent = 0.f;
		break;

	case ELT_POINT:
		l.pos.x = dl.Position.X;
		l.pos.y = dl.Position.Y;
		l.pos.z = dl.Position.Z;
		l.pos.w = 1.f;

		l.constantAttenuation = dl.Attenuation.X;
		l.linearAttenuation = dl.Attenuation.Y;
		l.quadraticAttenuation = dl.Attenuation.Z;

		l.spotDirection.x = 0.f;
		l.spotDirection.y = 0.f;
		l.spotDirection.z = -1.f;
		l.spotDirection.w = 0.f;
		l.spotCosCutoff = -1.f;
		l.spotCosInnerCutoff = 1.f;
		l.spotExponent = 0.f;
		break;

	case ELT_SPOT:
		l.pos.x = dl.Position.X;
		l.pos.y = dl.Position.Y;
		l.pos.z = dl.Position.Z;
		l.pos.w = 1.f;

		l.constantAttenuation = dl.Attenuation.X;
		l.linearAttenuation = dl.Attenuation.Y;
		l.quadraticAttenuation = dl.Attenuation.Z;

		l.spotDirection.x = nDirection.x;
		l.spotDirection.y = nDirection.y;
		l.spotDirection.z = nDirection.z;
		l.spotDirection.w = 0.0f;
		l.spotCosCutoff = cosf(dl.OuterCone * 2.0f * core::DEGTORAD * 0.5f);
		l.spotCosInnerCutoff = cosf(dl.InnerCone * 2.0f * core::DEGTORAD * 0.5f);
		l.spotExponent = dl.Falloff;
		break;
	default:
		break;
	}

	//which means ETS_VIEW, irrlicht openGL
	setTransform(ETS_WORLD, irr::core::IdentityMatrix);
	transform_calc(ETS_MODEL_VIEW);
	//transform_calc(ETS_NORMAL);

	const core::matrix4* matrix = Transformation[TransformationStack];
	transformVec4Vec4(matrix[ETS_MODEL_VIEW], &l.pos4.x, &l.pos.x);
	rotateMat44Vec3Vec4(matrix[ETS_MODEL_VIEW], &l.spotDirection4.x, &l.spotDirection.x);

	l.nmap_linearAttenuation = 1.f / (0.001f + dl.Radius * dl.Radius);

	/*
	//case ELT_DIRECTIONAL:
	if (l.pos.w == 0.f)
	{
		l.pos4n = l.pos4;
		l.pos4n.normalize_dir_xyz();

		//GL_LIGHT_MODEL_LOCAL_VIEWER = 0

		l.halfVector = l.pos4n;
		l.halfVector.z += 1.f;
		l.halfVector.normalize_dir_xyz();

	}
*/
}

//! adds a dynamic light
s32 CBurningVideoDriver::addDynamicLight(const SLight& dl)
{
	/*s32 i0 = */CNullDriver::addDynamicLight(dl);

	SBurningShaderLight l;
	EyeSpace.Light.push_back(l);
	s32 i1 = EyeSpace.Light.size() - 1;

	//i0 and i1 must be in sync
	assignHardwareLight(EyeSpace.Light[i1], dl);

	return i1;
}


//! Turns a dynamic light on or off
void CBurningVideoDriver::turnLightOn(s32 lightIndex, bool turnOn)
{
	if ((u32)lightIndex < EyeSpace.Light.size())
	{
		SBurningShaderLight& l = EyeSpace.Light[lightIndex];
		// some glitches with STK, always set, currently twice. openGL forces ModelMatrix to Identity
		if (!l.LightIsOn && turnOn)
		{
			assignHardwareLight(l, CNullDriver::getDynamicLight(lightIndex));
		}
		l.LightIsOn = turnOn;
	}
}

//! deletes all dynamic lights there are
void CBurningVideoDriver::deleteAllDynamicLights()
{
	EyeSpace.deleteAllDynamicLights();
	CNullDriver::deleteAllDynamicLights();

}

//! returns the maximal amount of dynamic lights the device can handle
u32 CBurningVideoDriver::getMaximalDynamicLightAmount() const
{
	return 8; //no limit 8 only for convenience
}


// a != b
size_t compare_3d_material(const SMaterial& a, const SMaterial& b)
{
	size_t flag = 0;
	flag |= a.MaterialType == b.MaterialType ? 0 : 1;
	flag |= a.TextureLayer[0].Texture == b.TextureLayer[0].Texture ? 0 : 4;
	if (flag) return flag;
	return a != b;
}

//! sets a material
void CBurningVideoDriver::setMaterial(const SMaterial& material)
{
	// ---------- Override
	Material.org = material;
	OverrideMaterial.apply(Material.org);

	const SMaterial& in = Material.org;

	const u32 shaderid = (u32)in.MaterialType;

	//basically set always. 2d does its own compare
	//if (TransformationStack == ETF_STACK_2D ||	Material.resetRenderStates || compare_3d_material(Material.lastMaterial,in))
	{
		// ---------- Notify Shader
		// unset old material
		u32 shaderid_old = (u32)Material.lastMaterial.MaterialType;

		if (shaderid != shaderid_old && shaderid_old < MaterialRenderers.size())
		{
			MaterialRenderers[shaderid_old].Renderer->OnUnsetMaterial();
		}

		// set new material.
		if (shaderid < MaterialRenderers.size())
		{
			MaterialRenderers[shaderid].Renderer->OnSetMaterial(
				in, Material.lastMaterial, Material.resetRenderStates, this);
		}

		Material.lastMaterial = in;
		Material.resetRenderStates = false;
	}

	//CSoftware2MaterialRenderer sets Material.Fallback_MaterialType

	//Material.Fallback_MaterialType = material.MaterialType;

//-----------------

	//Material.org = material;
	Material.CullFlag = CULL_INVISIBLE | (in.BackfaceCulling ? CULL_BACK : 0) | (in.FrontfaceCulling ? CULL_FRONT : 0);

	size_t* flag = TransformationFlag[TransformationStack];

	EyeSpace.TL_Flag &= ~(TL_TEXTURE_TRANSFORM | TL_LIGHT0_IS_NORMAL_MAP);

#ifdef SOFTWARE_DRIVER_2_TEXTURE_TRANSFORM
	//vertextype not set!
	for (u32 m = 0; m < BURNING_MATERIAL_MAX_TEXTURES /*VertexShader.vSize[VertexShader.vType].TexSize*/; ++m)
	{
		flag[ETS_TEXTURE_0 + m] &= ~ETF_TEXGEN_MASK;
		setTransform((E_TRANSFORMATION_STATE)(ETS_TEXTURE_0 + m), in.getTextureMatrix(m));
	}
#endif

#ifdef SOFTWARE_DRIVER_2_LIGHTING

	burning_setbit(EyeSpace.TL_Flag, in.FogEnable, TL_FOG);
	burning_setbit(EyeSpace.TL_Flag, in.NormalizeNormals, TL_NORMALIZE_NORMALS);

	burning_setbit(EyeSpace.TL_Flag, in.Lighting, TL_LIGHT);
	if (EyeSpace.TL_Flag & TL_LIGHT)
	{
		burning_setbit(EyeSpace.TL_Flag, in.ColorMaterial == ECM_AMBIENT || in.ColorMaterial == ECM_DIFFUSE_AND_AMBIENT, TL_COLORMAT_AMBIENT);
		burning_setbit(EyeSpace.TL_Flag, in.ColorMaterial == ECM_DIFFUSE || in.ColorMaterial == ECM_DIFFUSE_AND_AMBIENT, TL_COLORMAT_DIFFUSE);
		burning_setbit(EyeSpace.TL_Flag, in.ColorMaterial == ECM_SPECULAR, TL_COLORMAT_SPECULAR);

		Material.AmbientColor.setA8R8G8B8(in.AmbientColor.color);
		Material.DiffuseColor.setA8R8G8B8(in.DiffuseColor.color);
		Material.EmissiveColor.setA8R8G8B8(in.EmissiveColor.color);
		Material.SpecularColor.setA8R8G8B8(in.SpecularColor.color);

		burning_setbit(EyeSpace.TL_Flag, (in.Shininess != 0.f) && (in.SpecularColor.color & 0x00ffffff), TL_SPECULAR);
	}

#endif

	//--------------- setCurrentShader

	ITexture* texture0 = in.getTexture(0);
	ITexture* texture1 = in.getTexture(1);
	//ITexture* texture2 = in.getTexture(2);
	//ITexture* texture3 = in.getTexture(3);

	//visual studio code analysis
	u32 maxTex = BURNING_MATERIAL_MAX_TEXTURES;
	if (maxTex < 1) texture0 = 0;
	if (maxTex < 2) texture1 = 0;
	//if (maxTex < 3) texture2 = 0;
	//if (maxTex < 4) texture3 = 0;


	//todo: separate depth test from depth write
	Material.depth_write = getWriteZBuffer(in);
	Material.depth_test = in.ZBuffer != ECFN_DISABLED && Material.depth_write;

	EBurningFFShader shader = Material.depth_test ? ETR_TEXTURE_GOURAUD : ETR_TEXTURE_GOURAUD_NOZ;

	switch (Material.Fallback_MaterialType) //(Material.org.MaterialType) // Material.Fallback_MaterialType)
	{
	case EMT_ONETEXTURE_BLEND:
		shader = ETR_TEXTURE_BLEND;
		break;

	case EMT_TRANSPARENT_ALPHA_CHANNEL_REF:
		Material.org.MaterialTypeParam = 0.5f;
		//fallthrough

	case EMT_TRANSPARENT_ALPHA_CHANNEL:
		if (texture0 && texture0->hasAlpha())
		{
			shader = Material.depth_test ? ETR_TEXTURE_GOURAUD_ALPHA : ETR_TEXTURE_GOURAUD_ALPHA_NOZ;
		}
		else
		{
			//fall back to EMT_TRANSPARENT_VERTEX_ALPHA
			shader = ETR_TEXTURE_GOURAUD_VERTEX_ALPHA;
		}
		break;

	case EMT_TRANSPARENT_ADD_COLOR:
		shader = Material.depth_test ? ETR_TEXTURE_GOURAUD_ADD : ETR_TEXTURE_GOURAUD_ADD_NO_Z;
		if (Material.org.BlendOperation == EBO_ADD)
			shader = ETR_TEXTURE_GOURAUD_ADD_NO_Z;
		break;

	case EMT_TRANSPARENT_VERTEX_ALPHA:
		shader = ETR_TEXTURE_GOURAUD_VERTEX_ALPHA;
		break;

	case EMT_LIGHTMAP:
	case EMT_LIGHTMAP_LIGHTING:
		if (texture1)
			shader = ETR_TEXTURE_GOURAUD_LIGHTMAP_M1;
		break;

	case EMT_LIGHTMAP_M2:
	case EMT_LIGHTMAP_LIGHTING_M2:
		if (texture1)
			shader = ETR_TEXTURE_GOURAUD_LIGHTMAP_M2;
		break;

	case EMT_LIGHTMAP_LIGHTING_M4:
		if (texture1)
			shader = ETR_TEXTURE_GOURAUD_LIGHTMAP_M4;
		break;
	case EMT_LIGHTMAP_M4:
		if (texture1)
			shader = ETR_TEXTURE_LIGHTMAP_M4;
		break;

	case EMT_LIGHTMAP_ADD:
		if (texture1)
			shader = ETR_TEXTURE_GOURAUD_LIGHTMAP_ADD;
		break;

	case EMT_DETAIL_MAP:
		if (texture1)
			shader = ETR_TEXTURE_GOURAUD_DETAIL_MAP;
		break;

	case EMT_SPHERE_MAP:
		flag[ETS_TEXTURE_0] |= ETF_TEXGEN_CAMERA_SPHERE;
		EyeSpace.TL_Flag |= TL_TEXTURE_TRANSFORM;
		break;
	case EMT_REFLECTION_2_LAYER:
	case EMT_TRANSPARENT_REFLECTION_2_LAYER:
		if (texture1)
		{
			shader = ETR_TRANSPARENT_REFLECTION_2_LAYER;
			flag[ETS_TEXTURE_1] |= ETF_TEXGEN_CAMERA_REFLECTION;
			EyeSpace.TL_Flag |= TL_TEXTURE_TRANSFORM;
		}
		break;

	case EMT_NORMAL_MAP_SOLID:
	case EMT_NORMAL_MAP_TRANSPARENT_ADD_COLOR:
	case EMT_NORMAL_MAP_TRANSPARENT_VERTEX_ALPHA:
		if (texture1)
		{
			shader = ETR_NORMAL_MAP_SOLID;
			EyeSpace.TL_Flag |= TL_TEXTURE_TRANSFORM | TL_LIGHT0_IS_NORMAL_MAP;
			EyeSpace.TL_Flag &= ~TL_LIGHT;
		}
		break;
	case EMT_PARALLAX_MAP_SOLID:
	case EMT_PARALLAX_MAP_TRANSPARENT_ADD_COLOR:
	case EMT_PARALLAX_MAP_TRANSPARENT_VERTEX_ALPHA:
		if (texture1)
		{
			shader = ETR_PARALLAX_MAP_SOLID;
			EyeSpace.TL_Flag |= TL_TEXTURE_TRANSFORM | TL_LIGHT0_IS_NORMAL_MAP;
			EyeSpace.TL_Flag &= ~TL_LIGHT;
		}
		break;

	default:
		break;

	}

	if (!texture0)
	{
		shader = Material.depth_test ? ETR_GOURAUD :
			shader == ETR_TEXTURE_GOURAUD_VERTEX_ALPHA ?
			ETR_GOURAUD_ALPHA_NOZ : // 2D Gradient
			ETR_GOURAUD_NOZ;

		shader = ETR_COLOR;
	}

	if (in.Wireframe)
	{
		IBurningShader* candidate = BurningShader[shader];
		if (!candidate || (candidate && !candidate->canWireFrame()))
		{
			shader = ETR_TEXTURE_GOURAUD_WIRE;
		}
	}

	if (in.PointCloud)
	{
		IBurningShader* candidate = BurningShader[shader];
		if (!candidate || (candidate && !candidate->canPointCloud()))
		{
			shader = ETR_TEXTURE_GOURAUD_WIRE;
		}
	}

	//shader = ETR_REFERENCE;

	// switchToTriangleRenderer
	CurrentShader = BurningShader[shader];
	if (!CurrentShader && shaderid < MaterialRenderers.size())
	{
		CurrentShader = (IBurningShader *) MaterialRenderers[shaderid].Renderer;
	}

	if (CurrentShader)
	{
		CurrentShader->setTLFlag(EyeSpace.TL_Flag);
		if (EyeSpace.TL_Flag & TL_FOG) CurrentShader->setFog(FogColor);
		if (EyeSpace.TL_Flag & TL_SCISSOR) CurrentShader->setScissor(Scissor);
		CurrentShader->setRenderTarget(RenderTargetSurface, ViewPort, Interlaced);
		CurrentShader->OnSetMaterialBurning(Material);
		CurrentShader->setEdgeTest(in.Wireframe, in.PointCloud);
	}


#if 0
	{
		//u32 shaderid = (u32)Material.org.MaterialType;
		if (shaderid < MaterialRenderers.size())
			MaterialRenderers[shaderid].Renderer->OnRender(this, (video::E_VERTEX_TYPE)VertexShader.vType);
	}
#endif
}


//! Sets the fog mode.
void CBurningVideoDriver::setFog(SColor color, E_FOG_TYPE fogType, f32 start,
	f32 end, f32 density, bool pixelFog, bool rangeFog)
{
	CNullDriver::setFog(color, fogType, start, end, density, pixelFog, rangeFog);

	EyeSpace.fog_scale = reciprocal_zero(FogEnd - FogStart);
}



#if defined(SOFTWARE_DRIVER_2_LIGHTING) && BURNING_MATERIAL_MAX_COLORS > 0


/*!
	applies lighting model
*/

void CBurningVideoDriver::lightVertex_eye(s4DVertex* dest, const u32 vertexargb)
{
	//gl_FrontLightModelProduct.sceneColor = gl_FrontMaterial.emission + gl_FrontMaterial.ambient * gl_LightModel.ambient

	sVec3Color ambient;
	sVec3Color diffuse;
	sVec3Color specular;

	// the universe started in darkness..
	ambient.set(0.f);
	diffuse.set(0.f);
	specular.set(0.f);


	u32 i;
	f32 dot;
	f32 distance;
	f32 attenuation;
	sVec4 vp;			// vertex to light
	sVec4 lightHalf;	// blinn-phong reflection

	f32 spotDot;			// cos of angle between spotlight and point on surface

	for (i = 0; i < EyeSpace.Light.size(); ++i)
	{
		const SBurningShaderLight& light = EyeSpace.Light[i];
		if (!light.LightIsOn)
			continue;

		switch (light.Type | (EyeSpace.TL_Flag & TL_SPECULAR))
		{
		case ELT_DIRECTIONAL:
		case ELT_DIRECTIONAL | TL_SPECULAR:
			// surface to light vp = light.pos4n = light.pos4

			// attenuation = 1
			// distance = 1

			// accumulate ambient
			ambient.add_rgb(light.AmbientColor);

			//angle between normal and light vector
			dot = EyeSpace.normal.dot_xyz(light.pos4);
			if (dot <= 0.f) continue;

			diffuse.mad_rgb(light.DiffuseColor, dot);

			if (!(EyeSpace.TL_Flag & TL_SPECULAR))
				continue;

			//light.halfvector
			lightHalf.x = light.pos4.x - EyeSpace.vertexn.x; // + 0.f;
			lightHalf.y = light.pos4.y - EyeSpace.vertexn.y; // + 0.f;
			lightHalf.z = light.pos4.z - EyeSpace.vertexn.z; // + 1.f;
			//lightHalf.normalize_dir_xyz();

			dot = EyeSpace.normal.dot_xyz(lightHalf);
			if (dot <= 0.f) continue;

			distance = lightHalf.length_xyz();
			distance = reciprocal_zero(distance);

			specular.mad_rgb(light.SpecularColor, powf_limit(dot * distance, Material.org.Shininess));

			break;

		case ELT_POINT:
			// surface to light
			vp.x = light.pos4.x - EyeSpace.vertex.x;
			vp.y = light.pos4.y - EyeSpace.vertex.y;
			vp.z = light.pos4.z - EyeSpace.vertex.z;

			distance = vp.length_xyz();
			attenuation = light.constantAttenuation
				+ distance * (light.linearAttenuation + light.quadraticAttenuation * distance);

			attenuation = reciprocal_one(attenuation);

			//att = clamp(1.0 - dist/radius, 0.0, 1.0); att *= att

			// accumulate ambient
			ambient.mad_rgb(light.AmbientColor, attenuation);

			// build diffuse reflection

			//angle between normal and light vector
			//vp.mul_xyz(reciprocal_zero(distance)); //normalize
			dot = EyeSpace.normal.dot_xyz(vp);
			if (dot <= 0.f) continue;

			distance = reciprocal_zero(distance);

			// diffuse component
			diffuse.mad_rgb(light.DiffuseColor, (dot * distance) * attenuation);
			break;

		case ELT_POINT | TL_SPECULAR:
			// surface to light
			vp.x = light.pos4.x - EyeSpace.vertex.x;
			vp.y = light.pos4.y - EyeSpace.vertex.y;
			vp.z = light.pos4.z - EyeSpace.vertex.z;

			distance = vp.length_xyz();
			attenuation = light.constantAttenuation
				+ distance * (light.linearAttenuation + light.quadraticAttenuation * distance);
			attenuation = reciprocal_one(attenuation);

			// accumulate ambient
			ambient.mad_rgb(light.AmbientColor, attenuation);

			// build diffuse reflection
			dot = EyeSpace.normal.dot_xyz(vp);
			if (dot <= 0.f) continue;

			distance = reciprocal_zero(distance);

			// diffuse component
			diffuse.mad_rgb(light.DiffuseColor, (dot * distance) * attenuation);

			//vp.mul_xyz(distance); //normalize

			//halfVector = normalize(VP + eye), GL_LIGHT_MODEL_LOCAL_VIEWER
			lightHalf.x = vp.x * distance - EyeSpace.vertexn.x; // + 0.f;
			lightHalf.y = vp.y * distance - EyeSpace.vertexn.y; // + 0.f;
			lightHalf.z = vp.z * distance - EyeSpace.vertexn.z; // + 1.f;

			//lightHalf.normalize_dir_xyz();
			dot = EyeSpace.normal.dot_xyz(lightHalf);
			if (dot <= 0.f) continue;

			distance = lightHalf.length_xyz();
			dot *= reciprocal_zero(distance);

			//specular += light.SpecularColor * pow(max(dot(Eyespace.normal,lighthalf),0,Material.org.Shininess)*attenuation
			specular.mad_rgb(light.SpecularColor, powf_limit(dot, Material.org.Shininess) * attenuation);

			break;

		case ELT_SPOT:
		case ELT_SPOT | TL_SPECULAR:
			// surface to light
			vp.x = light.pos4.x - EyeSpace.vertex.x;
			vp.y = light.pos4.y - EyeSpace.vertex.y;
			vp.z = light.pos4.z - EyeSpace.vertex.z;

			distance = vp.length_xyz();

			//normalize
			vp.mul_xyz(reciprocal_zero(distance));

			// point on surface inside cone of illumination
			spotDot = vp.dot_minus_xyz(light.spotDirection4);
			if (spotDot < light.spotCosCutoff)
				continue;

			attenuation = light.constantAttenuation
				+ light.linearAttenuation * distance
				+ light.quadraticAttenuation * distance * distance;
			attenuation = reciprocal_one(attenuation);
			attenuation *= powf_limit(spotDot, light.spotExponent);

			// accumulate ambient
			ambient.mad_rgb(light.AmbientColor, attenuation);


			// build diffuse reflection
			//angle between normal and light vector
			dot = EyeSpace.normal.dot_xyz(vp);
			if (dot < 0.f) continue;

			// diffuse component
			diffuse.mad_rgb(light.DiffuseColor, dot * attenuation);

			if (!(EyeSpace.TL_Flag & TL_SPECULAR))
				continue;

			lightHalf.x = vp.x - EyeSpace.vertexn.x; // + 0.f;
			lightHalf.y = vp.y - EyeSpace.vertexn.y; // + 0.f;
			lightHalf.z = vp.z - EyeSpace.vertexn.z; // + 1.f;
			lightHalf.normalize_dir_xyz();

			//specular += light.SpecularColor * pow(max(dot(Eyespace.normal,lighthalf),0,Material.org.Shininess)*attenuation
			specular.mad_rgb(light.SpecularColor,
				powf_limit(EyeSpace.normal.dot_xyz(lightHalf), Material.org.Shininess) * attenuation
			);
			break;

		default:
			break;
		}

	}

	sVec3Color vertexColor;
	vertexColor.setA8R8G8B8(vertexargb);

	// sum up lights
	//If = Ia + Id + Is
	sVec3Color dColor;
	dColor.set(0.f);

	//Ia = gl_light_model_ambient* ambient_material + ambient_light * ambient_material 
	const sVec4& amb_mat = (EyeSpace.TL_Flag & TL_COLORMAT_AMBIENT) ? vertexColor : Material.AmbientColor;
	dColor.mad_rgbv(EyeSpace.Global_AmbientLight, amb_mat);
	dColor.mad_rgbv(ambient, amb_mat);

	//Id = diffuse_light * lambertTerm dot(N,L) * diffuse_material
	dColor.mad_rgbv(diffuse, (EyeSpace.TL_Flag & TL_COLORMAT_DIFFUSE) ? vertexColor : Material.DiffuseColor);
#if 0
	dColor.mad_rgbv(diffuse, Material.DiffuseColor);

	//diffuse * vertex color.
	//has to move to shader (for vertex color only this will fit [except clamping])

	dColor.r *= vertexColor.r;
	dColor.g *= vertexColor.g;
	dColor.b *= vertexColor.b;
#endif

	//separate specular
	const sVec4& spec_mat = (EyeSpace.TL_Flag & TL_COLORMAT_SPECULAR) ? vertexColor : Material.SpecularColor;
#if defined(SOFTWARE_DRIVER_2_USE_SEPARATE_SPECULAR_COLOR)
	if ((VertexShader.vSize[VertexShader.vType].Format & VERTEX4D_FORMAT_MASK_COLOR)>=VERTEX4D_FORMAT_COLOR_2_FOG)
	{
		specular.sat_mul_xyz(dest->Color[1], spec_mat);
	}
	else if (!(EyeSpace.TL_Flag & TL_LIGHT0_IS_NORMAL_MAP) &&
		(VertexShader.vSize[VertexShader.vType].Format & VERTEX4D_FORMAT_MASK_LIGHT)
		)
	{
		specular.sat_mul_xyz(dest->LightTangent[0], spec_mat);
	}
	else
#endif
	{
		dColor.mad_rgbv(specular, spec_mat);
	}


	dColor.add_rgb(Material.EmissiveColor);

	dColor.sat_alpha_pass(dest->Color[0], vertexColor.a);

}

#endif

/*
	draw2DImage with single color scales into destination quad & cliprect(more like viewport)
	draw2DImage with 4 color scales on destination and cliprect is scissor
*/

static const u32 quad_triangle_indexList[6 + 2] = { 0,1,2,0,2,3, 3,3 };


#if defined(SOFTWARE_DRIVER_2_2D_AS_2D)

//! draws an 2d image, using a color (if color is other then Color(255,255,255,255)) and the alpha channel of the texture if wanted.
void CBurningVideoDriver::draw2DImage(const video::ITexture* texture, const core::position2d<s32>& destPos,
	const core::rect<s32>& sourceRect,
	const core::rect<s32>* clipRect, SColor color,
	bool useAlphaChannelOfTexture)
{
	if (texture)
	{
		if (texture->getOriginalSize() != texture->getSize())
		{
			core::rect<s32> destRect(destPos, sourceRect.getSize());
			SColor c4[4] = { color,color,color,color };
			draw2DImage(texture, destRect, sourceRect, clipRect, c4, useAlphaChannelOfTexture);
			return;
		}

		if (texture->getDriverType() != EDT_BURNINGSVIDEO)
		{
			os::Printer::log("Fatal Error: Tried to copy from a surface not owned by this driver.", ELL_ERROR);
			return;
		}

		if (useAlphaChannelOfTexture)
			((CSoftwareTexture2*)texture)->getImage()->copyToWithAlpha(
				RenderTargetSurface, destPos, sourceRect, color, clipRect);
		else
			((CSoftwareTexture2*)texture)->getImage()->copyTo(
				RenderTargetSurface, destPos, sourceRect, clipRect);
	}
}


//! Draws a part of the texture into the rectangle.
void CBurningVideoDriver::draw2DImage(const video::ITexture* texture,
	const core::rect<s32>& destRect,
	const core::rect<s32>& sourceRect,
	const core::rect<s32>* clipRect,
	const video::SColor* const colors, bool useAlphaChannelOfTexture)
{
	if (texture)
	{
		if (texture->getDriverType() != EDT_BURNINGSVIDEO)
		{
			os::Printer::log("Fatal Error: Tried to copy from a surface not owned by this driver.", ELL_ERROR);
			return;
		}

		u32 argb = (colors ? colors[0].color : 0xFFFFFFFF);
		eBlitter op = useAlphaChannelOfTexture ?
			(argb == 0xFFFFFFFF ? BLITTER_TEXTURE_ALPHA_BLEND : BLITTER_TEXTURE_ALPHA_COLOR_BLEND) : BLITTER_TEXTURE;

		StretchBlit(op, RenderTargetSurface, clipRect, &destRect,
			((CSoftwareTexture2*)texture)->getImage(), &sourceRect, &texture->getOriginalSize(), argb
		);

	}
}

//!Draws an 2d rectangle with a gradient.
void CBurningVideoDriver::draw2DRectangle(const core::rect<s32>& position,
	SColor colorLeftUp, SColor colorRightUp, SColor colorLeftDown, SColor colorRightDown,
	const core::rect<s32>* clip)
{
	core::rect<s32> p(position);
	if (clip) p.clipAgainst(*clip);
	if (p.isValid()) drawRectangle(RenderTargetSurface, p, colorLeftUp);
}

#endif //defined(SOFTWARE_DRIVER_2_2D_AS_2D)



//! Enable the 2d override material
void CBurningVideoDriver::enableMaterial2D(bool enable)
{
	CNullDriver::enableMaterial2D(enable);
	//burning_setbit(TransformationFlag[1][ETS_PROJECTION], 0, ETF_VALID);
}


// a != b
size_t compare_2d_material(const SMaterial& a, const SMaterial& b)
{
	size_t flag = 0;
	flag |= a.MaterialType == b.MaterialType ? 0 : 1;
	flag |= a.ZBuffer == b.ZBuffer ? 0 : 2;
	flag |= a.TextureLayer[0].Texture == b.TextureLayer[0].Texture ? 0 : 4;
	flag |= a.TextureLayer[0].BilinearFilter == b.TextureLayer[0].BilinearFilter ? 0 : 8;
	flag |= a.TextureLayer[0].TextureWrapU == b.TextureLayer[0].TextureWrapU ? 0 : 16;
	flag |= a.MaterialTypeParam == b.MaterialTypeParam ? 0 : 32;
	if (flag) return flag;

	flag |= a.TextureLayer[1].Texture == b.TextureLayer[1].Texture ? 0 : 64;
	flag |= a.ZWriteEnable == b.ZWriteEnable ? 0 : 128;

	return flag;
}

void CBurningVideoDriver::setRenderStates2DMode(const video::SColor& color, const video::ITexture* texture, bool useAlphaChannelOfTexture)
{
	//save current 3D Material
	//Material.save3D = Material.org;

	//build new 2D Material

	bool vertexAlpha = color.getAlpha() < 255;

	//2D uses textureAlpa*vertexAlpha 3D not..
	if (useAlphaChannelOfTexture && texture && texture->hasAlpha())
	{
		Material.mat2D.MaterialType = EMT_TRANSPARENT_ALPHA_CHANNEL;
	}
	else if (vertexAlpha)
	{
		Material.mat2D.MaterialType = EMT_TRANSPARENT_VERTEX_ALPHA;
	}
	else
	{
		Material.mat2D.MaterialType = EMT_SOLID;
	}

	Material.mat2D.ZBuffer = ECFN_DISABLED;
	Material.mat2D.ZWriteEnable = EZW_OFF;
	Material.mat2D.Lighting = false;

	Material.mat2D.setTexture(0, (video::ITexture*)texture);

	//used for text. so stay as sharp as possible (like HW Driver)
	bool filter = false;

	const SMaterial& currentMaterial = (!OverrideMaterial2DEnabled) ? InitMaterial2D : OverrideMaterial2D;

	filter = texture && currentMaterial.TextureLayer[0].BilinearFilter;
	Material.mat2D.setFlag(video::EMF_BILINEAR_FILTER, filter);

	Material.mat2D.TextureLayer[0].TextureWrapU = currentMaterial.TextureLayer[0].TextureWrapU;
	Material.mat2D.TextureLayer[0].TextureWrapV = currentMaterial.TextureLayer[0].TextureWrapV;

	//compare
	size_t cmp_mat = compare_2d_material(Material.org, Material.mat2D);

	//switch to 2D Matrix Stack [ Material set Texture Matrix ]
	//if (TransformationStack != ETF_STACK_2D) cmp_mat |= 256;
	TransformationStack = ETF_STACK_2D;

	//2D GUI Matrix
	if ((cmp_mat & 256) || !(TransformationFlag[TransformationStack][ETS_PROJECTION] & ETF_VALID))
	{
		const core::dimension2d<u32>& renderTargetSize = getCurrentRenderTargetSize();
		core::matrix4 m(core::matrix4::EM4CONST_NOTHING);
		m.buildProjectionMatrixOrthoLH(f32(renderTargetSize.Width), f32(-(s32)(renderTargetSize.Height)), -1.0f, 1.0f);
		m.setTranslation(core::vector3df(-1.f, 1.f, 0));
		setTransform(ETS_PROJECTION, m);

		m.makeIdentity();
		setTransform(ETS_WORLD, m);

		// pixel perfect
		//if(filter)
		//currently done in ndc to dc -0.5f
		//m.setTranslation(core::vector3df(-0.5f, -0.5f, 0.0f));

#if defined(PATCH_SUPERTUX_8_0_1_with_1_9_0)
		m.setTranslation(core::vector3df(0.375f, 0.375f, 0.0f));
#endif

		setTransform(ETS_VIEW, m);
		cmp_mat |= 8;

	}

	//compare
	if (cmp_mat)
	{
		setMaterial(Material.mat2D);
	}

	if (CurrentShader)
	{
		CurrentShader->setPrimitiveColor(color.color);
		CurrentShader->setTLFlag(EyeSpace.TL_Flag);
		if (EyeSpace.TL_Flag & TL_SCISSOR) CurrentShader->setScissor(Scissor);
	}

}

void CBurningVideoDriver::setRenderStates3DMode()
{
	//restoreRenderStates3DMode

	//setMaterial(Material.save3D);
	//switch to 3D Matrix Stack
	TransformationStack = ETF_STACK_3D;
}

//! draws a vertex primitive list in 2d
void CBurningVideoDriver::draw2DVertexPrimitiveList(const void* vertices, u32 vertexCount,
	const void* indexList, u32 primitiveCount,
	E_VERTEX_TYPE vType, scene::E_PRIMITIVE_TYPE pType, E_INDEX_TYPE iType)
{
	if (!checkPrimitiveCount(primitiveCount))
		return;

	CNullDriver::draw2DVertexPrimitiveList(vertices, vertexCount, indexList, primitiveCount, vType, pType, iType);

	bool useAlphaChannelOfTexture = false;
	video::SColor color(0xFFFFFFFF);
	switch (Material.org.MaterialType)
	{
	case EMT_TRANSPARENT_ALPHA_CHANNEL:
		useAlphaChannelOfTexture = true;
		break;
	case EMT_TRANSPARENT_VERTEX_ALPHA:
		color.setAlpha(127);
		break;
	default:
		break;
	}
	setRenderStates2DMode(color, Material.org.getTexture(0), useAlphaChannelOfTexture);

	drawVertexPrimitiveList(vertices, vertexCount,
		indexList, primitiveCount,
		vType, pType, iType);

	setRenderStates3DMode();

}

//wrapper if both enabled
#if defined(SOFTWARE_DRIVER_2_2D_AS_2D) && defined(SOFTWARE_DRIVER_2_2D_AS_3D)
#endif

//setup a quad
#if defined(SOFTWARE_DRIVER_2_2D_AS_3D)

//! draws an 2d image, using a color (if color is other then Color(255,255,255,255)) and the alpha channel of the texture if wanted.
void CBurningVideoDriver::draw2DImage(const video::ITexture* texture, const core::position2d<s32>& destPos,
	const core::rect<s32>& sourceRect,
	const core::rect<s32>* clipRect, SColor color,
	bool useAlphaChannelOfTexture)
{
	if (!texture)
		return;

	if (!sourceRect.isValid())
		return;

	// clip these coordinates
	core::rect<s32> targetRect(destPos, sourceRect.getSize());
	if (clipRect)
	{
		targetRect.clipAgainst(*clipRect);
		if (targetRect.getWidth() < 0 || targetRect.getHeight() < 0)
			return;
	}

	const core::dimension2d<u32>& renderTargetSize = getCurrentRenderTargetSize();
	targetRect.clipAgainst(core::rect<s32>(0, 0, (s32)renderTargetSize.Width, (s32)renderTargetSize.Height));
	if (targetRect.getWidth() < 0 || targetRect.getHeight() < 0)
		return;

	// ok, we've clipped everything.
	// now draw it.
	const core::dimension2d<s32> sourceSize(targetRect.getSize());
	core::position2d<s32> sourcePos(sourceRect.UpperLeftCorner + (targetRect.UpperLeftCorner - destPos));

	const core::dimension2d<u32>& tex_orgsize = texture->getOriginalSize();
	const f32 invW = 1.f / static_cast<f32>(tex_orgsize.Width);
	const f32 invH = 1.f / static_cast<f32>(tex_orgsize.Height);
	const core::rect<f32> tcoords(
		sourcePos.X * invW,
		sourcePos.Y * invH,
		(sourcePos.X + sourceSize.Width) * invW,
		(sourcePos.Y + sourceSize.Height) * invH);

	Quad2DVertices[0].Color = color;
	Quad2DVertices[1].Color = color;
	Quad2DVertices[2].Color = color;
	Quad2DVertices[3].Color = color;

	Quad2DVertices[0].Pos = core::vector3df((f32)targetRect.UpperLeftCorner.X, (f32)targetRect.UpperLeftCorner.Y, 0.0f);
	Quad2DVertices[1].Pos = core::vector3df((f32)targetRect.LowerRightCorner.X, (f32)targetRect.UpperLeftCorner.Y, 0.0f);
	Quad2DVertices[2].Pos = core::vector3df((f32)targetRect.LowerRightCorner.X, (f32)targetRect.LowerRightCorner.Y, 0.0f);
	Quad2DVertices[3].Pos = core::vector3df((f32)targetRect.UpperLeftCorner.X, (f32)targetRect.LowerRightCorner.Y, 0.0f);

	Quad2DVertices[0].TCoords = core::vector2df(tcoords.UpperLeftCorner.X, tcoords.UpperLeftCorner.Y);
	Quad2DVertices[1].TCoords = core::vector2df(tcoords.LowerRightCorner.X, tcoords.UpperLeftCorner.Y);
	Quad2DVertices[2].TCoords = core::vector2df(tcoords.LowerRightCorner.X, tcoords.LowerRightCorner.Y);
	Quad2DVertices[3].TCoords = core::vector2df(tcoords.UpperLeftCorner.X, tcoords.LowerRightCorner.Y);

	setRenderStates2DMode(color, texture, useAlphaChannelOfTexture);

	drawVertexPrimitiveList(Quad2DVertices, 4,
		quad_triangle_indexList, 2,
		EVT_STANDARD, scene::EPT_TRIANGLES, EIT_32BIT);

	setRenderStates3DMode();

}


//! Draws a part of the texture into the rectangle.
void CBurningVideoDriver::draw2DImage(const video::ITexture* texture, const core::rect<s32>& destRect,
	const core::rect<s32>& sourceRect, const core::rect<s32>* clipRect,
	const video::SColor* const colors, bool useAlphaChannelOfTexture)
{
	if (!texture)
		return;

	const core::dimension2d<u32>& st = texture->getOriginalSize();
	const f32 invW = 1.f / static_cast<f32>(st.Width);
	const f32 invH = 1.f / static_cast<f32>(st.Height);
	const core::rect<f32> tcoords(
		sourceRect.UpperLeftCorner.X * invW,
		sourceRect.UpperLeftCorner.Y * invH,
		sourceRect.LowerRightCorner.X * invW,
		sourceRect.LowerRightCorner.Y * invH);

	const video::SColor temp[4] =
	{
		0xFFFFFFFF,
		0xFFFFFFFF,
		0xFFFFFFFF,
		0xFFFFFFFF
	};

	const video::SColor* const useColor = colors ? colors : temp;


	Quad2DVertices[0].Color = useColor[0];
	Quad2DVertices[1].Color = useColor[3];
	Quad2DVertices[2].Color = useColor[2];
	Quad2DVertices[3].Color = useColor[1];

	Quad2DVertices[0].Pos = core::vector3df((f32)destRect.UpperLeftCorner.X, (f32)destRect.UpperLeftCorner.Y, 0.0f);
	Quad2DVertices[1].Pos = core::vector3df((f32)destRect.LowerRightCorner.X, (f32)destRect.UpperLeftCorner.Y, 0.0f);
	Quad2DVertices[2].Pos = core::vector3df((f32)destRect.LowerRightCorner.X, (f32)destRect.LowerRightCorner.Y, 0.0f);
	Quad2DVertices[3].Pos = core::vector3df((f32)destRect.UpperLeftCorner.X, (f32)destRect.LowerRightCorner.Y, 0.0f);

	Quad2DVertices[0].TCoords = core::vector2df(tcoords.UpperLeftCorner.X, tcoords.UpperLeftCorner.Y);
	Quad2DVertices[1].TCoords = core::vector2df(tcoords.LowerRightCorner.X, tcoords.UpperLeftCorner.Y);
	Quad2DVertices[2].TCoords = core::vector2df(tcoords.LowerRightCorner.X, tcoords.LowerRightCorner.Y);
	Quad2DVertices[3].TCoords = core::vector2df(tcoords.UpperLeftCorner.X, tcoords.LowerRightCorner.Y);


	if (clipRect)
	{
		if (!clipRect->isValid())
			return;

		//glEnable(GL_SCISSOR_TEST);
		EyeSpace.TL_Flag |= TL_SCISSOR;
		setScissor(clipRect->UpperLeftCorner.X, clipRect->UpperLeftCorner.Y,//renderTargetSize.Height - clipRect->LowerRightCorner.Y
			clipRect->getWidth(), clipRect->getHeight());
	}

	video::SColor alphaTest;
	alphaTest.color = useColor[0].color & useColor[0].color & useColor[0].color & useColor[0].color;

	setRenderStates2DMode(alphaTest, texture, useAlphaChannelOfTexture);

	drawVertexPrimitiveList(Quad2DVertices, 4,
		quad_triangle_indexList, 2,
		EVT_STANDARD, scene::EPT_TRIANGLES, EIT_32BIT);

	if (clipRect)
		EyeSpace.TL_Flag &= ~TL_SCISSOR;

	setRenderStates3DMode();

}


//!Draws an 2d rectangle with a gradient.
void CBurningVideoDriver::draw2DRectangle(const core::rect<s32>& position,
	SColor colorLeftUp, SColor colorRightUp, SColor colorLeftDown, SColor colorRightDown,
	const core::rect<s32>* clip)
{
	core::rect<s32> pos = position;

	if (clip)
		pos.clipAgainst(*clip);

	if (!pos.isValid())
		return;

	Quad2DVertices[0].Color = colorLeftUp;
	Quad2DVertices[1].Color = colorRightUp;
	Quad2DVertices[2].Color = colorRightDown;
	Quad2DVertices[3].Color = colorLeftDown;

	Quad2DVertices[0].Pos = core::vector3df((f32)pos.UpperLeftCorner.X, (f32)pos.UpperLeftCorner.Y, 0.0f);
	Quad2DVertices[1].Pos = core::vector3df((f32)pos.LowerRightCorner.X, (f32)pos.UpperLeftCorner.Y, 0.0f);
	Quad2DVertices[2].Pos = core::vector3df((f32)pos.LowerRightCorner.X, (f32)pos.LowerRightCorner.Y, 0.0f);
	Quad2DVertices[3].Pos = core::vector3df((f32)pos.UpperLeftCorner.X, (f32)pos.LowerRightCorner.Y, 0.0f);

	Quad2DVertices[0].TCoords.X = 0.f;
	Quad2DVertices[0].TCoords.Y = 0.f;
	Quad2DVertices[1].TCoords.X = 0.f;
	Quad2DVertices[1].TCoords.Y = 0.f;
	Quad2DVertices[2].TCoords.X = 0.f;
	Quad2DVertices[3].TCoords.Y = 0.f;
	Quad2DVertices[3].TCoords.X = 0.f;
	Quad2DVertices[3].TCoords.Y = 0.f;


	video::SColor alphaTest;
	alphaTest.color = colorLeftUp.color & colorRightUp.color & colorRightDown.color & colorLeftDown.color;
	setRenderStates2DMode(alphaTest, 0, 0);

	drawVertexPrimitiveList(Quad2DVertices, 4,
		quad_triangle_indexList, 2,
		EVT_STANDARD, scene::EPT_TRIANGLES, EIT_32BIT);

	setRenderStates3DMode();

}


#endif // SOFTWARE_DRIVER_2_2D_AS_3D





//! Draws a 2d line.
void CBurningVideoDriver::draw2DLine(const core::position2d<s32>& start,
	const core::position2d<s32>& end,
	SColor color)
{
	drawLine(RenderTargetSurface, start, end, color);
}


//! Draws a pixel
void CBurningVideoDriver::drawPixel(u32 x, u32 y, const SColor& color)
{
	RenderTargetSurface->setPixel(x, y, color, true);
}


//! Only used by the internal engine. Used to notify the driver that
//! the window was resized.
void CBurningVideoDriver::OnResize(const core::dimension2d<u32>& size)
{
	// make sure width and height are multiples of 2
	core::dimension2d<u32> realSize(size);
	/*
		if (realSize.Width % 2)
			realSize.Width += 1;

		if (realSize.Height % 2)
			realSize.Height += 1;
	*/
	if (ScreenSize != realSize)
	{
		if (ViewPort.getWidth() == (s32)ScreenSize.Width &&
			ViewPort.getHeight() == (s32)ScreenSize.Height)
		{
			ViewPort.UpperLeftCorner.X = 0;
			ViewPort.UpperLeftCorner.Y = 0;
			ViewPort.LowerRightCorner.X = realSize.Width;
			ViewPort.LowerRightCorner.X = realSize.Height;
		}

		ScreenSize = realSize;

		bool resetRT = (RenderTargetSurface == BackBuffer);

		if (BackBuffer)
			BackBuffer->drop();
		BackBuffer = new CImage(SOFTWARE_DRIVER_2_RENDERTARGET_COLOR_FORMAT, realSize);

		if (resetRT)
			setRenderTargetImage2(BackBuffer);
	}
}


//! returns the current render target size
const core::dimension2d<u32>& CBurningVideoDriver::getCurrentRenderTargetSize() const
{
	return (RenderTargetSurface == BackBuffer) ? ScreenSize : RenderTargetSize;
}



//! Draws a 3d line.
void CBurningVideoDriver::draw3DLine(const core::vector3df& start,
	const core::vector3df& end, SColor color_start)
{
	SColor color_end = color_start;

	VertexShader.primitiveHasVertex = 2;
	VertexShader.vType = E4VT_LINE;

	s4DVertex* v = Clipper.data;

	transform_calc(ETS_MODEL_VIEW_PROJ);
	const core::matrix4* matrix = Transformation[TransformationStack];
	matrix[ETS_MODEL_VIEW_PROJ].transformVect(&v[s4DVertex_ofs(0)].Pos.x, start);
	matrix[ETS_MODEL_VIEW_PROJ].transformVect(&v[s4DVertex_ofs(1)].Pos.x, end);

	u32 has_vertex_run;
	const u32 flag = (VertexShader.vSize[VertexShader.vType].Format);
	for (has_vertex_run = 0; has_vertex_run < VertexShader.primitiveHasVertex; has_vertex_run += 1)
	{
		v[s4DVertex_ofs(has_vertex_run)].reset_interpolate();
		v[s4DVertex_ofs(has_vertex_run)].flag = flag;
		v[s4DVertex_pro(has_vertex_run)].flag = flag;
	}

#if BURNING_MATERIAL_MAX_COLORS > 0
	v[s4DVertex_ofs(0)].Color[0].setA8R8G8B8(color_start.color);
	v[s4DVertex_ofs(1)].Color[0].setA8R8G8B8(color_end.color);
#endif

	u32 vOut;

	// vertices count per line
	vOut = clipToFrustum(VertexShader.primitiveHasVertex);
	if (vOut < VertexShader.primitiveHasVertex)
		return;

	// to DC Space, project homogenous vertex
	ndc_2_dc_and_project(v, s4DVertex_ofs(vOut), Transformation_ETS_CLIPSCALE[TransformationStack]);

	// unproject vertex color
#if 0
#if BURNING_MATERIAL_MAX_COLORS > 0
	for (g = 0; g != vOut; g += 2)
	{
		v[g + 1].Color[0].setA8R8G8B8(color.color);
	}
#endif
#endif

	pushShader(scene::EPT_LINES, 0);
	for (has_vertex_run = 0; (has_vertex_run + VertexShader.primitiveHasVertex) <= vOut; has_vertex_run += 1)
	{
		CurrentShader->drawLine(v + s4DVertex_pro(has_vertex_run), v + s4DVertex_pro(has_vertex_run + 1));
	}
	PushShader.pop();
}

// set Shader Mode based on primitive type
void CBurningVideoDriver::pushShader(scene::E_PRIMITIVE_TYPE pType, int testCurrent)
{
	int wireFrame = 0;
	int pointCloud = 0;

	switch (pType)
	{
	case scene::EPT_POINTS:
	case scene::EPT_POINT_SPRITES:
		pointCloud = 1;
		break;
	case scene::EPT_LINE_STRIP:
	case scene::EPT_LINE_LOOP:
	case scene::EPT_LINES:
		wireFrame = 1;
		break;
	default:
		return;
	}

	IBurningShader* shader = 0;
	if (wireFrame)
	{
		if (testCurrent && CurrentShader && CurrentShader->canWireFrame()) shader = CurrentShader;
		else shader = BurningShader[ETR_TEXTURE_GOURAUD_WIRE];
	}
	if (pointCloud)
	{
		if (testCurrent && CurrentShader && CurrentShader->canPointCloud()) shader = CurrentShader;
		else shader = BurningShader[ETR_TEXTURE_GOURAUD_WIRE];
	}

	if (shader)
	{
		if (shader != CurrentShader)
		{
			PushShader.push(CurrentShader);
			CurrentShader = shader;
			shader->setRenderTarget(RenderTargetSurface, ViewPort, Interlaced);
			shader->OnSetMaterialBurning(Material);
		}
		shader->setEdgeTest(wireFrame, pointCloud);
	}
}

//! \return Returns the name of the video driver. Example: In case of the DirectX8
//! driver, it would return "Direct3D8.1".
const wchar_t* CBurningVideoDriver::getName() const
{
#ifdef BURNINGVIDEO_RENDERER_BEAUTIFUL
	return L"Burning's Video 0.54 beautiful";
#elif defined(PATCH_SUPERTUX_8_0_1_with_1_9_0)
	return L"Burning's Video 0.54 STK";
#elif defined ( BURNINGVIDEO_RENDERER_ULTRA_FAST )
	return L"Burning's Video 0.54 ultra fast";
#elif defined ( BURNINGVIDEO_RENDERER_FAST )
	return L"Burning's Video 0.54 fast";
#elif defined ( BURNINGVIDEO_RENDERER_CE )
	return L"Burning's Video 0.54 CE";
#else
	return L"Burning's Video 0.54";
#endif
}

//! Returns the graphics card vendor name.
core::stringc CBurningVideoDriver::getVendorInfo()
{
	return "Burning's Video: Ing. Thomas Alten (c) 2006-2022";
}


//! Returns type of video driver
E_DRIVER_TYPE CBurningVideoDriver::getDriverType() const
{
	return EDT_BURNINGSVIDEO;
}


//! returns color format
ECOLOR_FORMAT CBurningVideoDriver::getColorFormat() const
{
	return BackBuffer ? BackBuffer->getColorFormat() : CNullDriver::getColorFormat();
}


//! Creates a render target texture.
ITexture* CBurningVideoDriver::addRenderTargetTexture(const core::dimension2d<u32>& size,
	const io::path& name, const ECOLOR_FORMAT format
#if defined(PATCH_SUPERTUX_8_0_1_with_1_9_0)
	, const bool useStencil
#endif
)
{
	if (IImage::isCompressedFormat(format))
		return 0;

	//IImage* img = createImage(SOFTWARE_DRIVER_2_RENDERTARGET_COLOR_FORMAT, size);
	//empty proxy image
	IImage* img = createImageFromData(format, size, 0, true, false);
	ITexture* tex = new CSoftwareTexture2(img, name, CSoftwareTexture2::IS_RENDERTARGET /*| CSoftwareTexture2::GEN_MIPMAP */, this);
	if (img) img->drop();
	addTexture(tex);
	tex->drop();
	return tex;
}

void CBurningVideoDriver::clearBuffers(u16 flag, SColor color, f32 depth, u8 stencil)
{
	if ((flag & ECBF_COLOR) && RenderTargetSurface) image_fill(RenderTargetSurface, color, Interlaced);
	if ((flag & ECBF_DEPTH) && DepthBuffer) DepthBuffer->clear(depth, Interlaced);
	if ((flag & ECBF_STENCIL) && StencilBuffer) StencilBuffer->clear(stencil, Interlaced);
}

#if 0
void CBurningVideoDriver::saveBuffer()
{
	static int shotCount = 0;
	char buf[256];
	if (BackBuffer)
	{
		sprintf(buf, "shot/%04d_b.png", shotCount);
		writeImageToFile(BackBuffer, buf);
	}
	if (StencilBuffer)
	{
		CImage stencil(ECF_A8R8G8B8, StencilBuffer->getSize(), StencilBuffer->lock(), true, false);
		sprintf(buf, "shot/%04d_s.ppm", shotCount);
		writeImageToFile(&stencil, buf);
	}
	shotCount += 1;
}
#endif

//! Returns an image created from the last rendered frame.
IImage* CBurningVideoDriver::createScreenShot(video::ECOLOR_FORMAT format, video::E_RENDER_TARGET target)
{
	if (target != video::ERT_FRAME_BUFFER)
		return 0;

	if (BackBuffer)
	{
		IImage* tmp = createImage(BackBuffer->getColorFormat(), BackBuffer->getDimension());
		BackBuffer->copyTo(tmp);
		return tmp;
	}
	else
		return 0;
}

ITexture* CBurningVideoDriver::createDeviceDependentTexture(const io::path& name, IImage* image)
{
	u32 flags =
		((TextureCreationFlags & ETCF_CREATE_MIP_MAPS) ? CSoftwareTexture2::GEN_MIPMAP : 0)
#if defined(PATCH_SUPERTUX_8_0_1_with_1_9_0)
		| CSoftwareTexture2::GEN_MIPMAP_AUTO
#else
		| ((TextureCreationFlags & ETCF_AUTO_GENERATE_MIP_MAPS) ? CSoftwareTexture2::GEN_MIPMAP_AUTO : 0)
#endif
		| ((TextureCreationFlags & ETCF_ALLOW_NON_POWER_2) ? CSoftwareTexture2::ALLOW_NPOT : 0)
#if defined(IRRLICHT_sRGB)
		| ((TextureCreationFlags & ETCF_IMAGE_IS_LINEAR) ? CSoftwareTexture2::IMAGE_IS_LINEAR : 0)
		| ((TextureCreationFlags & ETCF_TEXTURE_IS_LINEAR) ? CSoftwareTexture2::TEXTURE_IS_LINEAR : 0)
#endif
		;

	CSoftwareTexture2* texture = new CSoftwareTexture2(image, name, flags, this);
	return texture;
}

ITexture* CBurningVideoDriver::createDeviceDependentTextureCubemap(const io::path& name, const core::array<IImage*>& image)
{
	return 0;
}

//! Returns the maximum amount of primitives (mostly vertices) which
//! the device is able to render with one drawIndexedTriangleList
//! call.
u32 CBurningVideoDriver::getMaximalPrimitiveCount() const
{
	return 0x7FFFFFFF;
}


//! Draws a shadow volume into the stencil buffer. To draw a stencil shadow, do
//! this: First, draw all geometry. Then use this method, to draw the shadow
//! volume. Next use IVideoDriver::drawStencilShadow() to visualize the shadow.
void CBurningVideoDriver::drawStencilShadowVolume(const core::array<core::vector3df>& triangles, bool zfail, u32 debugDataVisible)
{
	const u32 count = triangles.size();
	if (!StencilBuffer || !count)
		return;

	Material.org.MaterialType = video::EMT_SOLID;
	Material.org.Lighting = false;
	Material.org.ZWriteEnable = video::EZW_OFF;
	Material.org.ZBuffer = ECFN_LESS;

	CurrentShader = BurningShader[ETR_STENCIL_SHADOW];

	CurrentShader->setRenderTarget(RenderTargetSurface, ViewPort, Interlaced);
	CurrentShader->setEdgeTest(Material.org.Wireframe, 0);

	//setMaterial
	EyeSpace.TL_Flag &= ~(TL_TEXTURE_TRANSFORM | TL_LIGHT0_IS_NORMAL_MAP);
	CurrentShader->setTLFlag(EyeSpace.TL_Flag);
	//glStencilMask(~0);
	//glStencilFunc(GL_ALWAYS, 0, ~0);

	//glEnable(GL_DEPTH_CLAMP);

	if (zfail)
	{
		Material.org.BackfaceCulling = false;
		Material.org.FrontfaceCulling = true;
		Material.CullFlag = CULL_FRONT | CULL_INVISIBLE;

		CurrentShader->setStencilOp(StencilOp_KEEP, StencilOp_INCR, StencilOp_KEEP);
		drawVertexPrimitiveList(triangles.const_pointer(), count, 0, count / 3, (video::E_VERTEX_TYPE)E4VT_SHADOW, scene::EPT_TRIANGLES, (video::E_INDEX_TYPE)E4IT_NONE);

		Material.org.BackfaceCulling = true;
		Material.org.FrontfaceCulling = false;
		Material.CullFlag = CULL_BACK | CULL_INVISIBLE;

		CurrentShader->setStencilOp(StencilOp_KEEP, StencilOp_DECR, StencilOp_KEEP);
		drawVertexPrimitiveList(triangles.const_pointer(), count, 0, count / 3, (video::E_VERTEX_TYPE)E4VT_SHADOW, scene::EPT_TRIANGLES, (video::E_INDEX_TYPE)E4IT_NONE);
	}
	else // zpass
	{
		Material.org.BackfaceCulling = true;
		Material.org.FrontfaceCulling = false;
		Material.CullFlag = CULL_BACK | CULL_INVISIBLE;

		CurrentShader->setStencilOp(StencilOp_KEEP, StencilOp_KEEP, StencilOp_INCR);
		drawVertexPrimitiveList(triangles.const_pointer(), count, 0, count / 3, (video::E_VERTEX_TYPE)E4VT_SHADOW, scene::EPT_TRIANGLES, (video::E_INDEX_TYPE)E4IT_NONE);

		Material.org.BackfaceCulling = false;
		Material.org.FrontfaceCulling = true;
		Material.CullFlag = CULL_FRONT | CULL_INVISIBLE;

		CurrentShader->setStencilOp(StencilOp_KEEP, StencilOp_KEEP, StencilOp_DECR);
		drawVertexPrimitiveList(triangles.const_pointer(), count, 0, count / 3, (video::E_VERTEX_TYPE)E4VT_SHADOW, scene::EPT_TRIANGLES, (video::E_INDEX_TYPE)E4IT_NONE);
	}
	//glDisable(GL_DEPTH_CLAMP);

}

//! Fills the stencil shadow with color. After the shadow volume has been drawn
//! into the stencil buffer using IVideoDriver::drawStencilShadowVolume(), use this
//! to draw the color of the shadow.
void CBurningVideoDriver::drawStencilShadow(bool clearStencilBuffer, video::SColor leftUpEdge,
	video::SColor rightUpEdge, video::SColor leftDownEdge, video::SColor rightDownEdge)
{
	if (!StencilBuffer)
		return;

	// draw a shadow rectangle covering the entire screen using stencil buffer
	const u32 h = RenderTargetSurface->getDimension().Height;
	const u32 w = RenderTargetSurface->getDimension().Width;

	const bool bit32 = RenderTargetSurface->getColorFormat() == ECF_A8R8G8B8;
	const tVideoSample alpha = extractAlpha(leftUpEdge.color) >> (bit32 ? 0 : 3);
	const tVideoSample src = bit32 ? leftUpEdge.color : video::A8R8G8B8toA1R5G5B5(leftUpEdge.color);

	interlace_scanline_data line;
	for (line.y = 0; line.y < h; line.y += SOFTWARE_DRIVER_2_STEP_Y)
	{
		if_interlace_scanline
		{
			tVideoSample * dst = (tVideoSample*)RenderTargetSurface->getData() + (line.y * w);
			const tStencilSample* stencil = (tStencilSample*)StencilBuffer->lock() + (line.y * w);

			if (bit32)
			{
				for (u32 x = 0; x < w; x += SOFTWARE_DRIVER_2_STEP_X)
				{
					if (stencil[x]) dst[x] = PixelBlend32(dst[x], src, alpha);
				}
			}
			else
			{
				for (u32 x = 0; x < w; x += SOFTWARE_DRIVER_2_STEP_X)
				{
					if (stencil[x]) dst[x] = PixelBlend16(dst[x], src, alpha);
				}
			}

		}
	}

	if (clearStencilBuffer)
		StencilBuffer->clear(0, Interlaced);
}


core::dimension2du CBurningVideoDriver::getMaxTextureSize() const
{
	return core::dimension2du(SOFTWARE_DRIVER_2_TEXTURE_MAXSIZE, SOFTWARE_DRIVER_2_TEXTURE_MAXSIZE);
}

bool CBurningVideoDriver::queryTextureFormat(ECOLOR_FORMAT format) const
{
	return format == SOFTWARE_DRIVER_2_RENDERTARGET_COLOR_FORMAT || format == SOFTWARE_DRIVER_2_TEXTURE_COLOR_FORMAT;
}

#if !defined(PATCH_SUPERTUX_8_0_1_with_1_9_0)
bool CBurningVideoDriver::needsTransparentRenderPass(const irr::video::SMaterial& material) const
{
	return CNullDriver::needsTransparentRenderPass(material) || material.isAlphaBlendOperation(); // || material.isTransparent();
}
#endif

s32 CBurningVideoDriver::addShaderMaterial(const c8* vertexShaderProgram,
	const c8* pixelShaderProgram,
	IShaderConstantSetCallBack* callback,
	E_MATERIAL_TYPE baseMaterial,
	s32 userData)
{
	s32 materialID = -1;

	IBurningShader* shader = new IBurningShader(
		this, materialID,
		vertexShaderProgram, 0, video::EVST_VS_1_1,
		pixelShaderProgram, 0, video::EPST_PS_1_1,
		0, 0, EGST_GS_4_0,
		scene::EPT_TRIANGLES, scene::EPT_TRIANGLE_STRIP, 0,
		callback, baseMaterial, userData);

	shader->drop();

	if (callback && materialID >= 0)
		callback->OnCreate(shader, userData);

	return materialID;
}

//! Adds a new material renderer to the VideoDriver, based on a high level shading language.
s32 CBurningVideoDriver::addHighLevelShaderMaterial(
	const c8* vertexShaderProgram,
	const c8* vertexShaderEntryPointName,
	E_VERTEX_SHADER_TYPE vsCompileTarget,
	const c8* pixelShaderProgram,
	const c8* pixelShaderEntryPointName,
	E_PIXEL_SHADER_TYPE psCompileTarget,
	const c8* geometryShaderProgram,
	const c8* geometryShaderEntryPointName,
	E_GEOMETRY_SHADER_TYPE gsCompileTarget,
	scene::E_PRIMITIVE_TYPE inType,
	scene::E_PRIMITIVE_TYPE outType,
	u32 verticesOut,
	IShaderConstantSetCallBack* callback,
	E_MATERIAL_TYPE baseMaterial,
	s32 userData
#if defined(PATCH_SUPERTUX_8_0_1_with_1_9_0)
	, E_GPU_SHADING_LANGUAGE shadingLang
#endif
)
{
	s32 materialID = -1;

	IBurningShader* shader = new IBurningShader(
		this, materialID,
		vertexShaderProgram, vertexShaderEntryPointName, vsCompileTarget,
		pixelShaderProgram, pixelShaderEntryPointName, psCompileTarget,
		geometryShaderProgram, geometryShaderEntryPointName, gsCompileTarget,
		inType, outType, verticesOut,
		callback, baseMaterial, userData);

	shader->drop();

	if (callback && materialID >= 0)
		callback->OnCreate(shader, userData);

	return materialID;
}


void CBurningVideoDriver::setFallback_Material(E_MATERIAL_TYPE fallback_MaterialType, eBurningVertexShader vertexShader)
{
	//this should be in material....
	Material.Fallback_MaterialType = fallback_MaterialType;
	Material.VertexShader = vertexShader;
}

void CBurningVideoDriver::setBasicRenderStates(const SMaterial& material,
	const SMaterial& lastMaterial,
	bool resetAllRenderstates)
{

}

//! Return an index constant for the vertex shader based on a name.
s32 CBurningVideoDriver::getVertexShaderConstantID(const c8* name)
{
	return getPixelShaderConstantID(name);
}

bool CBurningVideoDriver::setVertexShaderConstant(s32 index, const f32* floats, int count)
{
	return setPixelShaderConstant(index, floats, count);
}

bool CBurningVideoDriver::setVertexShaderConstant(s32 index, const s32* ints, int count)
{
	return setPixelShaderConstant(index, ints, count);
}

bool CBurningVideoDriver::setVertexShaderConstant(s32 index, const u32* ints, int count)
{
	return setPixelShaderConstant(index, ints, count);
}

void CBurningVideoDriver::setVertexShaderConstant(const f32* data, s32 startRegister, s32 constantAmount)
{
	//used?
	if (CurrentShader)
	{
		CurrentShader->setVertexShaderConstant(data, startRegister, constantAmount);
	}
}

//! Return an index constant for the pixel shader based on a name.
s32 CBurningVideoDriver::getPixelShaderConstantID(const c8* name)
{
	return -1;
}

bool CBurningVideoDriver::setPixelShaderConstant(s32 index, const f32* floats, int count)
{
	return false;
}

bool CBurningVideoDriver::setPixelShaderConstant(s32 index, const s32* ints, int count)
{
	return false;
}

bool CBurningVideoDriver::setPixelShaderConstant(s32 index, const u32* ints, int count)
{
	return false;
}

void CBurningVideoDriver::setPixelShaderConstant(const f32* data, s32 startRegister, s32 constantAmount = 1)
{
	//used?
	if (CurrentShader)
	{
		CurrentShader->setPixelShaderConstant(data, startRegister, constantAmount);
	}
}

//! Get pointer to the IVideoDriver interface
/** \return Pointer to the IVideoDriver interface */
IVideoDriver* CBurningVideoDriver::getVideoDriver()
{
	return this;
}


//! Run occlusion query. Draws mesh stored in query.
/** If the mesh shall not be rendered visible, use
overrideMaterial to disable the color and depth buffer. */
void CBurningVideoDriver::runOcclusionQuery(scene::ISceneNode* node, bool visible)
{
	const s32 index = OcclusionQueries.linear_search(SOccQuery(node));
	if (index != -1)
	{
		//extGlBeginQuery(GL_SAMPLES_PASSED_ARB, OcclusionQueries[index].UID);
		samples_passed = 0;
		CNullDriver::runOcclusionQuery(node, visible);
		//extGlEndQuery(GL_SAMPLES_PASSED_ARB);
	}
}


//! Update occlusion query. Retrieves results from GPU.
/** If the query shall not block, set the flag to false.
Update might not occur in this case, though */
void CBurningVideoDriver::updateOcclusionQuery(scene::ISceneNode* node, bool block)
{
	const s32 index = OcclusionQueries.linear_search(SOccQuery(node));
	if (index < 0) return;

	OcclusionQueries[index].Result = samples_passed;

}


//! Return query result.
/** Return value is the number of visible pixels/fragments.
The value is a safe approximation, i.e. can be larger than the
actual value of pixels. */
u32 CBurningVideoDriver::getOcclusionQueryResult(const scene::ISceneNode* node) const
{
	const s32 index = OcclusionQueries.linear_search(node);
	return index < 0 ? ~0 : OcclusionQueries[index].Result;
}

burning_namespace_end

#endif // _IRR_COMPILE_WITH_BURNINGSVIDEO_


burning_namespace_start

//! creates a video driver
IVideoDriver* createBurningVideoDriver(const irr::SIrrlichtCreationParameters& params, io::IFileSystem* io, video::IImagePresenter* presenter)
{
#ifdef _IRR_COMPILE_WITH_BURNINGSVIDEO_
	return new CBurningVideoDriver(params, io, presenter);
#else
	return 0;
#endif // _IRR_COMPILE_WITH_BURNINGSVIDEO_
}

burning_namespace_end

