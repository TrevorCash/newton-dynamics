/* Copyright (c) <2003-2016> <Julio Jerez, Newton Game Dynamics>
*
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
*
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
*
* 3. This notice may not be removed or altered from any source distribution.
*/

#ifndef _DG_MATH_AVX_H_
#define _DG_MATH_AVX_H_
#include "dgNewtonPluginStdafx.h"

DG_MSC_AVX_ALIGMENT
class dgFloatAvx
{
	#define PERMUTE_MASK(w, z, y, x)		_MM_SHUFFLE (w, z, y, x)
	public:
	DG_INLINE dgFloatAvx()
	{
	}

	DG_INLINE dgFloatAvx(float val)
		:m_type(_mm256_set1_ps (val))
	{
	}

	DG_INLINE dgFloatAvx(const __m256 type)
		:m_type(type)
	{
	}

	DG_INLINE dgFloatAvx(const dgFloatAvx& copy)
		:m_type(copy.m_type)
	{
	}

	DG_INLINE dgFloatAvx (const dgVector& low, const dgVector& high)
		:m_type (_mm256_loadu2_m128 (&high.m_x, &low.m_x))
	{
	}

	DG_INLINE static void ClearFlops()
	{
		#ifdef _DEBUG
		m_flopsCount = 0;
		#endif
	}

	DG_INLINE static void IncFlops()
	{
		#ifdef _DEBUG
		m_flopsCount++;
		#endif
	}

	DG_INLINE static dgUnsigned32 GetFlops()
	{
#ifdef _DEBUG
		return m_flopsCount;
#else 
		return 0;
#endif
	}


	DG_INLINE void Store (dgFloatAvx* const ptr ) const
	{
		_mm256_store_ps((float*) ptr, m_type);
	}

	DG_INLINE dgFloatAvx operator+ (const dgFloatAvx& A) const
	{
		IncFlops();
		return _mm256_add_ps(m_type, A.m_type);
	}

	DG_INLINE dgFloatAvx operator* (const dgFloatAvx& A) const
	{
		IncFlops();
		return _mm256_mul_ps(m_type, A.m_type);
	}

	DG_INLINE static void Transpose4x8(dgFloatAvx& src0, dgFloatAvx& src1, dgFloatAvx& src2, dgFloatAvx& src3)
	{
		__m256 tmp0(_mm256_unpacklo_ps(src0.m_type, src1.m_type));
		__m256 tmp1(_mm256_unpacklo_ps(src2.m_type, src3.m_type));
		__m256 tmp2(_mm256_unpackhi_ps(src0.m_type, src1.m_type));
		__m256 tmp3(_mm256_unpackhi_ps(src2.m_type, src3.m_type));

		src0 = _mm256_shuffle_ps(tmp0, tmp1, PERMUTE_MASK(1, 0, 1, 0));
		src1 = _mm256_shuffle_ps(tmp0, tmp1, PERMUTE_MASK(3, 2, 3, 2));
		src2 = _mm256_shuffle_ps(tmp2, tmp3, PERMUTE_MASK(1, 0, 1, 0));
		src3 = _mm256_shuffle_ps(tmp2, tmp3, PERMUTE_MASK(3, 2, 3, 2));
	}

	union
	{
		__m256 m_type;
		__m256i m_typeInt;
		float m_f[8];
		int m_i[8];
	};

	static dgFloatAvx m_one;
	static dgFloatAvx m_zero;
#ifdef _DEBUG
	static dgUnsigned32 m_flopsCount;
#endif
} DG_GCC_AVX_ALIGMENT;

DG_MSC_AVX_ALIGMENT
class dgVector3Avx
{
	public:
	DG_INLINE dgVector3Avx()
		:m_x()
		,m_y()
		,m_z()
	{
	}

	DG_INLINE dgVector3Avx(const dgVector3Avx& val)
		:m_x(val.m_x)
		,m_y(val.m_y)
		,m_z(val.m_z)
	{
	}

	DG_INLINE dgVector3Avx(const dgFloatAvx& x, const dgFloatAvx& y, const dgFloatAvx& z)
		:m_x(x)
		,m_y(y)
		,m_z(z)
	{
	}

	DG_INLINE dgVector3Avx(const dgVector& v0, const dgVector& v1, const dgVector& v2, const dgVector& v3, const dgVector& v4, const dgVector& v5, const dgVector& v6, const dgVector& v7)
		:m_x()
		,m_y()
		,m_z()
	{
		dgFloatAvx r0(v0, v4);
		dgFloatAvx r1(v1, v5);
		dgFloatAvx r2(v2, v6);
		dgFloatAvx r3(v3, v7);
		dgFloatAvx::Transpose4x8(r0, r1, r2, r3);
		m_x = r0;
		m_y = r1;
		m_z = r2;
	}

	DG_INLINE void Store(dgVector3Avx* const ptr) const
	{
		m_x.Store(&ptr->m_x);
		m_y.Store(&ptr->m_y);
		m_z.Store(&ptr->m_z);
	}

	DG_INLINE dgVector3Avx Scale(const dgFloatAvx& a) const
	{
		return dgVector3Avx(m_x * a, m_y * a, m_z * a);
	}

	DG_INLINE dgVector3Avx operator* (const dgVector3Avx& a) const
	{
		return dgVector3Avx(m_x * a.m_x, m_y * a.m_y, m_z * a.m_z);
	}

	dgFloatAvx m_x;
	dgFloatAvx m_y;
	dgFloatAvx m_z;
} DG_GCC_AVX_ALIGMENT;


DG_MSC_AVX_ALIGMENT
class dgVector6Avx
{
	public:
	dgVector3Avx m_linear;
	dgVector3Avx m_angular;

} DG_GCC_AVX_ALIGMENT;


DG_MSC_AVX_ALIGMENT
class dgMatrix3x3Avx
{
	public:
	DG_INLINE dgMatrix3x3Avx()
		:m_right()
		,m_up()
		,m_front()
	{
	}

	DG_INLINE dgMatrix3x3Avx(const dgMatrix3x3Avx& val)
		:m_right(val.m_right)
		,m_up(val.m_up)
		,m_front(val.m_front)
	{
	}

	DG_INLINE dgMatrix3x3Avx(const dgVector3Avx& x, const dgVector3Avx& y, const dgVector3Avx& z)
		:m_right(x)
		,m_up(y)
		,m_front(z)
	{
	}

	DG_INLINE dgMatrix3x3Avx(const dgMatrix& matrix0, const dgMatrix& matrix1, const dgMatrix& matrix2, const dgMatrix& matrix3, const dgMatrix& matrix4, const dgMatrix& matrix5, const dgMatrix& matrix6, const dgMatrix& matrix7)
		:m_right(matrix0[0], matrix1[0], matrix2[0], matrix3[0], matrix4[0], matrix5[0], matrix6[0], matrix7[0])
		,m_up(matrix0[1], matrix1[1], matrix2[1], matrix3[1], matrix4[1], matrix5[1], matrix6[1], matrix7[1])
		,m_front(matrix0[2], matrix1[2], matrix2[2], matrix3[2], matrix4[2], matrix5[2], matrix6[2], matrix7[2])
	{
	}

	DG_INLINE void Store(dgMatrix3x3Avx* const ptr) const
	{
		m_front.Store(&ptr->m_front);
		m_up.Store(&ptr->m_up);
		m_right.Store(&ptr->m_right);
	}

	DG_INLINE dgMatrix3x3Avx Transposed() const
	{
		return dgMatrix3x3Avx(dgVector3Avx(m_front.m_x, m_up.m_x, m_right.m_x),
							  dgVector3Avx(m_front.m_y, m_up.m_y, m_right.m_y),
							  dgVector3Avx(m_front.m_z, m_up.m_z, m_right.m_z));
	}

	DG_INLINE dgMatrix3x3Avx operator* (const dgMatrix3x3Avx& a) const
	{
		return dgMatrix3x3Avx(a.RotateVector(m_front), a.RotateVector(m_up), a.RotateVector(m_right));
	}

	DG_INLINE dgVector3Avx RotateVector(const dgVector3Avx& a) const
	{
		dgFloatAvx x(a.m_x * m_front.m_x + a.m_y * m_up.m_x + a.m_z * m_right.m_x);
		dgFloatAvx y(a.m_x * m_front.m_y + a.m_y * m_up.m_y + a.m_z * m_right.m_y);
		dgFloatAvx z(a.m_x * m_front.m_z + a.m_y * m_up.m_z + a.m_z * m_right.m_z);
		return dgVector3Avx(x, y, z);
	}

	DG_INLINE dgVector3Avx UnrotateVector(const dgVector3Avx& a) const
	{
		dgFloatAvx x(a.m_x * m_front.m_x + a.m_y * m_front.m_y + a.m_z * m_front.m_z);
		dgFloatAvx y(a.m_x * m_up.m_x    + a.m_y * m_up.m_y    + a.m_z * m_up.m_z);
		dgFloatAvx z(a.m_x * m_right.m_x + a.m_y * m_right.m_y + a.m_z * m_right.m_z);
		return dgVector3Avx(x, y, z);
	}
	
	dgVector3Avx m_front;
	dgVector3Avx m_up;
	dgVector3Avx m_right;
	
} DG_GCC_AVX_ALIGMENT;

#endif

