// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_FAST_MATH_COMPAT_H_INCLUDED__
#define __IRR_FAST_MATH_COMPAT_H_INCLUDED__

#include "irrMath.h"

namespace irr
{
namespace core
{


// IRRLICHT_FAST_MATH functions which I wanted to kick out because they return
// wrong results. But last time I proposed that I've been asked to keep them for 
// Burnings software renderer. So to avoid changing that accidentally or messing up
// it's speed I'll keep them around, but only as internal header.
// They should not be used otherwise any longer.

	// Some examples for unexpected results when using this with IRRLICHT_FAST_MATH:
	// Input 1, expected 1, got 0
	// Input 3, expected 3, got 2
	// Input -1.40129846e-45, expected -1, got 0
	REALINLINE s32 floor32_fast(f32 x)
	{
#ifdef IRRLICHT_FAST_MATH
		const f32 h = 0.5f;

		s32 t;

#if defined(_MSC_VER) && !defined(_WIN64)
		__asm
		{
			fld	x
			fsub	h
			fistp	t
		}
#elif defined(__GNUC__)
		__asm__ __volatile__ (
			"fsub %2 \n\t"
			"fistpl %0"
			: "=m" (t)
			: "t" (x), "f" (h)
			: "st"
			);
#else
		return (s32) floorf ( x );
#endif
		return t;
#else // no fast math
		return (s32) floorf ( x );
#endif
	}
	
	// Some examples for unexpected results when using this with IRRLICHT_FAST_MATH:
	// Input 1.40129846e-45, expected 1, got 0
	// Input -1, expected -1, got 0
	// Input -3, expected -3, got -2
	REALINLINE s32 ceil32_fast ( f32 x )
	{
#ifdef IRRLICHT_FAST_MATH
		const f32 h = 0.5f;

		s32 t;

#if defined(_MSC_VER) && !defined(_WIN64)
		__asm
		{
			fld	x
			fadd	h
			fistp	t
		}
#elif defined(__GNUC__)
		__asm__ __volatile__ (
			"fadd %2 \n\t"
			"fistpl %0 \n\t"
			: "=m"(t)
			: "t"(x), "f"(h)
			: "st"
			);
#else
		return (s32) ceilf ( x );
#endif
		return t;
#else // not fast math
		return (s32) ceilf ( x );
#endif
	}	
	
	// Some examples for unexpected results when using this with IRRLICHT_FAST_MATH:
	// Input 0.5, expected 1, got 0
	// Input 2.5, expected 3, got 2
	// Input -1.40129846e-45, expected -nan(ind), got -inf
	// Input -2.80259693e-45, expected -nan(ind), got -inf	
	REALINLINE s32 round32_fast(f32 x)
	{
#if defined(IRRLICHT_FAST_MATH)
		s32 t;

#if defined(_MSC_VER) && !defined(_WIN64)
		__asm
		{
			fld   x
			fistp t
		}
#elif defined(__GNUC__)
		__asm__ __volatile__ (
			"fistpl %0 \n\t"
			: "=m"(t)
			: "t"(x)
			: "st"
			);
#else
		return (s32) round_(x);
#endif
		return t;
#else // no fast math
		return (s32) round_(x);
#endif
	}	
	
} // end namespace core
} // end namespace irr	

#endif // __IRR_FAST_MATH_COMPAT_H_INCLUDED__
