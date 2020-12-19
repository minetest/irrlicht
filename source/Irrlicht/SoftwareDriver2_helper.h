// Copyright (C) 2002-2012 Nikolaus Gebhardt / Thomas Alten
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

/*
	History:
	- changed behavior for log2 textures ( replaced multiplies by shift )
*/

#ifndef __S_VIDEO_2_SOFTWARE_HELPER_H_INCLUDED__
#define __S_VIDEO_2_SOFTWARE_HELPER_H_INCLUDED__

#include "SoftwareDriver2_compile_config.h"
#include "irrMath.h"
#include "irrMathFastCompat.h"
#include "CSoftwareTexture2.h"
#include "SMaterial.h"


namespace irr
{

// supporting different packed pixel needs many defines...

#if defined(SOFTWARE_DRIVER_2_32BIT)
	typedef u32	tVideoSample;
	typedef u32	tStencilSample;

	#define	MASK_A	0xFF000000
	#define	MASK_R	0x00FF0000
	#define	MASK_G	0x0000FF00
	#define	MASK_B	0x000000FF

	#define	SHIFT_A	(unsigned)24
	#define	SHIFT_R	(unsigned)16
	#define	SHIFT_G	(unsigned)8
	#define	SHIFT_B	(unsigned)0

	#define	COLOR_MAX					0xFF
	#define	COLOR_MAX_LOG2				8
	#define	COLOR_BRIGHT_WHITE			0xFFFFFFFF

	#define SOFTWARE_DRIVER_2_TEXTURE_GRANULARITY	(unsigned)2
	#define SOFTWARE_DRIVER_2_RENDERTARGET_GRANULARITY	(unsigned)2
#else
	typedef u16	tVideoSample;
	typedef u8	tStencilSample;

	#define	MASK_A	0x8000
	#define	MASK_R	0x7C00
	#define	MASK_G	0x03E0
	#define	MASK_B	0x001F

	#define	SHIFT_A	(unsigned)15
	#define	SHIFT_R	(unsigned)10
	#define	SHIFT_G	(unsigned)5
	#define	SHIFT_B	(unsigned)0

	#define	COLOR_MAX					0x1F
	#define	COLOR_MAX_LOG2				5
	#define	COLOR_BRIGHT_WHITE			0xFFFF
	#define SOFTWARE_DRIVER_2_TEXTURE_GRANULARITY	(unsigned)1
	#define SOFTWARE_DRIVER_2_RENDERTARGET_GRANULARITY	(unsigned)1

#endif




// ----------------------- Generic ----------------------------------
//! align_next - align to next upper 2^n
#define align_next(num,to) (((num) + (to-1)) & (~(to-1)))

//! a more useful memset for pixel. dest must be aligned at least to 4 byte
// (standard memset only works with 8-bit values)
inline void memset32(void * dest, const u32 value, size_t bytesize)
{
	u32 * d = (u32*) dest;

	size_t i;

	// loops unrolled to reduce the number of increments by factor ~8.
	i = bytesize >> (2 + 3);
	while (i)
	{
		d[0] = value;
		d[1] = value;
		d[2] = value;
		d[3] = value;

		d[4] = value;
		d[5] = value;
		d[6] = value;
		d[7] = value;

		d += 8;
		i -= 1;
	}

	i = (bytesize >> 2 ) & 7;
	while (i)
	{
		d[0] = value;
		d += 1;
		i -= 1;
	}
}

//! a more useful memset for pixel. dest must be aligned at least to 2 byte
// (standard memset only works with 8-bit values)
inline void memset16(void * dest, const u16 value, size_t bytesize)
{
	u16 * d = (u16*) dest;

	size_t i;

	// loops unrolled to reduce the number of increments by factor ~8.
	i = bytesize >> (1 + 3);
	while (i)
	{
		d[0] = value;
		d[1] = value;
		d[2] = value;
		d[3] = value;

		d[4] = value;
		d[5] = value;
		d[6] = value;
		d[7] = value;

		d += 8;
		--i;
	}

	i = (bytesize >> 1 ) & 7;
	while (i)
	{
		d[0] = value;
		++d;
		--i;
	}
}

//! memset interleaved
inline void memset32_interlaced(void* dest, const u32 value, size_t pitch,u32 height,const interlaced_control Interlaced)
{
	if (Interlaced.bypass) return memset32(dest, value, pitch * height);

	u8* dst = (u8*)dest;
	interlace_scanline_data line;
	for (line.y = 0; line.y < height; line.y += SOFTWARE_DRIVER_2_STEP_Y)
	{
		interlace_scanline_enabled memset32(dst, value, pitch);
		dst += pitch;
	}
}

// byte-align structures
#include "irrpack.h"

//IEEE Standard for Floating - Point Arithmetic(IEEE 754)
typedef union {
	float f;
	unsigned int u;
	struct { unsigned int frac:23; unsigned exp:8; unsigned int sign:1; } fields;
	struct { unsigned int frac_exp:31; } abs;
} ieee754 PACK_STRUCT;

// Default alignment
#include "irrunpack.h"

// 0.5f as integer
#define ieee754_zero_dot_5	0x3f000000
#define ieee754_one			0x3f800000
#define ieee754_two			0x40000000

#if 0
// integer log2 of a float ieee 754. [not used anymore]
static inline s32 s32_log2_f32( f32 f)
{
	//u32 x = IR ( f ); return ((x & 0x7F800000) >> 23) - 127;
	ieee754 _log2;
	_log2.f = f;
	return _log2.fields.exp ? _log2.fields.exp - 127 : 10000000; /*denormal very high number*/
}
#endif

// integer log2 of an integer. returning 0 as denormal
static inline s32 s32_log2_s32(u32 in)
{
	s32 ret = 0;
	while (in > 1)
	{
		in >>= 1;
		ret++;
	}
	return ret;
	//return s32_log2_f32( (f32) x);
	//ieee754 _log2;_log2.f = (f32) in; return _log2.fields.exp - 127;
}

#if 0
static inline s32 s32_abs(s32 x)
{
	s32 b = x >> 31;
	return (x ^ b ) - b;
}


//! conditional set based on mask and arithmetic shift
REALINLINE u32 if_mask_a_else_b ( const u32 mask, const u32 a, const u32 b )
{
	return ( mask & ( a ^ b ) ) ^ b;
}
#endif

// ------------------ Video---------------------------------------
/*!
	Pixel = dest * ( 1 - alpha ) + source * alpha
	alpha [0;256]
*/
REALINLINE u32 PixelBlend32 ( const u32 c2, const u32 c1, const u32 alpha )
{
	u32 srcRB = c1 & 0x00FF00FF;
	u32 srcXG = c1 & 0x0000FF00;

	u32 dstRB = c2 & 0x00FF00FF;
	u32 dstXG = c2 & 0x0000FF00;


	u32 rb = srcRB - dstRB;
	u32 xg = srcXG - dstXG;

	rb *= alpha;
	xg *= alpha;
	rb >>= 8;
	xg >>= 8;

	rb += dstRB;
	xg += dstXG;

	rb &= 0x00FF00FF;
	xg &= 0x0000FF00;

	return rb | xg;
}

/*!
	Pixel = dest * ( 1 - alpha ) + source * alpha
	alpha [0;32]
*/
inline u16 PixelBlend16 ( const u16 c2, const u16 c1, const u16 alpha )
{
	const u16 srcRB = c1 & 0x7C1F;
	const u16 srcXG = c1 & 0x03E0;

	const u16 dstRB = c2 & 0x7C1F;
	const u16 dstXG = c2 & 0x03E0;

	u32 rb = srcRB - dstRB;
	u32 xg = srcXG - dstXG;

	rb *= alpha;
	xg *= alpha;
	rb >>= 5;
	xg >>= 5;

	rb += dstRB;
	xg += dstXG;

	rb &= 0x7C1F;
	xg &= 0x03E0;

	return (u16)(rb | xg);
}

/*
	Pixel = c0 * (c1/31). c0 Alpha retain
*/
inline u16 PixelMul16 ( const u16 c0, const u16 c1)
{
	return (u16)((( ( (c0 & 0x7C00) * (c1 & 0x7C00) ) & 0x3E000000 ) >> 15 ) |
			(( ( (c0 & 0x03E0) * (c1 & 0x03E0) ) & 0x000F8000 ) >> 10 ) |
			(( ( (c0 & 0x001F) * (c1 & 0x001F) ) & 0x000003E0 ) >> 5 ) |
			(c0 & 0x8000));
}

/*
	Pixel = c0 * (c1/31).
*/
inline u16 PixelMul16_2 ( u16 c0, u16 c1)
{
	return	(u16)(( ( (c0 & 0x7C00) * (c1 & 0x7C00) ) & 0x3E000000 ) >> 15 |
			( ( (c0 & 0x03E0) * (c1 & 0x03E0) ) & 0x000F8000 ) >> 10 |
			( ( (c0 & 0x001F) * (c1 & 0x001F) ) & 0x000003E0 ) >> 5  |
			( c0 & c1 & 0x8000));
}

/*
	Pixel = c0 * (c1/255). c0 Alpha Retain
*/
REALINLINE u32 PixelMul32 ( const u32 c0, const u32 c1)
{
	return	(c0 & 0xFF000000) |
			(( ( (c0 & 0x00FF0000) >> 12 ) * ( (c1 & 0x00FF0000) >> 12 ) ) & 0x00FF0000 ) |
			(( ( (c0 & 0x0000FF00) * (c1 & 0x0000FF00) ) >> 16 ) & 0x0000FF00 ) |
			(( ( (c0 & 0x000000FF) * (c1 & 0x000000FF) ) >> 8  ) & 0x000000FF);
}

/*
	Pixel = c0 * (c1/255).
*/
REALINLINE u32 PixelMul32_2 ( const u32 c0, const u32 c1)
{
	return	(( ( (c0 & 0xFF000000) >> 16 ) * ( (c1 & 0xFF000000) >> 16 ) ) & 0xFF000000 ) |
			(( ( (c0 & 0x00FF0000) >> 12 ) * ( (c1 & 0x00FF0000) >> 12 ) ) & 0x00FF0000 ) |
			(( ( (c0 & 0x0000FF00) * (c1 & 0x0000FF00) ) >> 16 ) & 0x0000FF00 ) |
			(( ( (c0 & 0x000000FF) * (c1 & 0x000000FF) ) >> 8  ) & 0x000000FF);
}

/*
	Pixel = clamp ( c0 + c1, 0, 255 )
*/
REALINLINE u32 PixelAdd32 ( const u32 c2, const u32 c1)
{
	u32 sum = ( c2 & 0x00FFFFFF )  + ( c1 & 0x00FFFFFF );
	u32 low_bits = ( c2 ^ c1 ) & 0x00010101;
	s32 carries  = ( sum - low_bits ) & 0x01010100;
	u32 modulo = sum - carries;
	u32 clamp = carries - ( carries >> 8 );
	return modulo | clamp;
}

#if 0

// 1 - Bit Alpha Blending
inline u16 PixelBlend16 ( const u16 destination, const u16 source )
{
	if((source & 0x8000) == 0x8000)
		return source; // The source is visible, so use it.
	else
		return destination; // The source is transparent, so use the destination.
}

// 1 - Bit Alpha Blending 16Bit SIMD
inline u32 PixelBlend16_simd ( const u32 destination, const u32 source )
{
	switch(source & 0x80008000)
	{
		case 0x80008000: // Both source pixels are visible
			return source;

		case 0x80000000: // Only the first source pixel is visible
			return (source & 0xFFFF0000) | (destination & 0x0000FFFF);

		case 0x00008000: // Only the second source pixel is visible.
			return (destination & 0xFFFF0000) | (source & 0x0000FFFF);

		default: // Neither source pixel is visible.
			return destination;
	}
}
#else

// 1 - Bit Alpha Blending
inline u16 PixelBlend16 ( const u16 c2, const u16 c1 )
{
	u16 mask = ((c1 & 0x8000) >> 15 ) + 0x7fff;
	return (c2 & mask ) | ( c1 & ~mask );
}

// 1 - Bit Alpha Blending 16Bit SIMD
inline u32 PixelBlend16_simd ( const u32 c2, const u32 c1 )
{
	u32 mask = ((c1 & 0x80008000) >> 15 ) + 0x7fff7fff;
	return (c2 & mask ) | ( c1 & ~mask );
}

#endif

/*!
	Pixel = dest * ( 1 - SourceAlpha ) + source * SourceAlpha (OpenGL blending)
*/
inline u32 PixelBlend32 ( const u32 c2, const u32 c1 )
{
	// alpha test
	u32 alpha = c1 & 0xFF000000;

	if ( 0 == alpha )
		return c2;
	if ( 0xFF000000 == alpha )
	{
		return c1;
	}

	alpha >>= 24;

	// add highbit alpha, if ( alpha > 127 ) alpha += 1;
	alpha += ( alpha >> 7);

	u32 srcRB = c1 & 0x00FF00FF;
	u32 srcXG = c1 & 0x0000FF00;

	u32 dstRB = c2 & 0x00FF00FF;
	u32 dstXG = c2 & 0x0000FF00;


	u32 rb = srcRB - dstRB;
	u32 xg = srcXG - dstXG;

	rb *= alpha;
	xg *= alpha;
	rb >>= 8;
	xg >>= 8;

	rb += dstRB;
	xg += dstXG;

	rb &= 0x00FF00FF;
	xg &= 0x0000FF00;

	return (c1 & 0xFF000000) | rb | xg;
}



// ------------------ Fix Point ----------------------------------

#if defined(ENV64BIT)
typedef s32 tFixPoint;
typedef u32 tFixPointu;
#else
typedef s32 tFixPoint;
typedef u32 tFixPointu;
#endif

// Fix Point 12 (overflow on s32)
#if 0
	#define FIX_POINT_PRE			12
	#define FIX_POINT_FRACT_MASK	0xFFF
	#define FIX_POINT_UNSIGNED_MASK	0x7FFFF000
	#define FIX_POINT_ONE			0x1000
	#define FIX_POINT_ZERO_DOT_FIVE	0x0800
	#define FIX_POINT_F32_MUL		4096.f
#endif

// Fix Point 11 (overflow on s32)
#if 0
	#define FIX_POINT_PRE			11
	#define FIX_POINT_FRACT_MASK	0x7FF
	#define FIX_POINT_UNSIGNED_MASK	0xFFFFF800
	#define FIX_POINT_ONE			0x800
	#define FIX_POINT_ZERO_DOT_FIVE	0x400
	#define FIX_POINT_F32_MUL		2048.f
#endif

// Fix Point 10
#if 1
	#define FIX_POINT_PRE			10
	#define FIX_POINT_FRACT_MASK	0x000003FF
	#define FIX_POINT_UNSIGNED_MASK	0x7FFFFE00
	#define FIX_POINT_ONE			0x00000400
	#define FIX_POINT_ZERO_DOT_FIVE	0x00000200
	#define FIX_POINT_F32_MUL		1024.f
#endif

// Fix Point 9
#if 0
	#define FIX_POINT_PRE			9
	#define FIX_POINT_FRACT_MASK	0x1FF
	#define FIX_POINT_UNSIGNED_MASK	0x7FFFFE00
	#define FIX_POINT_ONE			0x200
	#define FIX_POINT_ZERO_DOT_FIVE	0x100
	#define FIX_POINT_F32_MUL		512.f
#endif

// Fix Point 7
#if 0
	#define FIX_POINT_PRE			7
	#define FIX_POINT_FRACT_MASK	0x7F
	#define FIX_POINT_UNSIGNED_MASK	0x7FFFFF80
	#define FIX_POINT_ONE			0x80
	#define FIX_POINT_ZERO_DOT_FIVE	0x40
	#define FIX_POINT_F32_MUL		128.f
#endif

#define	FIXPOINT_COLOR_MAX		( COLOR_MAX << FIX_POINT_PRE )

#if   FIX_POINT_PRE == 10 && COLOR_MAX == 255
	#define FIX_POINT_HALF_COLOR	0x1FE00
	#define FIX_POINT_COLOR_ERROR	4
#elif FIX_POINT_PRE == 12 && COLOR_MAX == 255
	#define FIX_POINT_HALF_COLOR	0x7F800
	#define FIX_POINT_COLOR_ERROR	16
#elif FIX_POINT_PRE == 10 && COLOR_MAX == 31
	#define FIX_POINT_HALF_COLOR	0x3E00
	#define FIX_POINT_COLOR_ERROR	32
#else
	#define FIX_POINT_HALF_COLOR	( (tFixPoint) ( ((f32) COLOR_MAX / 2.f * FIX_POINT_F32_MUL ) ) )
	#define FIX_POINT_COLOR_ERROR	(1<<(FIX_POINT_PRE-COLOR_MAX_LOG2))
#endif


/*
	convert signed integer to fixpoint
*/
inline tFixPoint s32_to_fixPoint (const s32 x)
{
	return x << FIX_POINT_PRE;
}

#if 0
inline tFixPointu u32_to_fixPoint (const u32 x)
{
	return x << FIX_POINT_PRE;
}
#endif

inline u32 fixPointu_to_u32 (const tFixPointu x)
{
	return (u32)(x >> FIX_POINT_PRE);
}


// 1/x * FIX_POINT
#define fix_inverse32(x) (FIX_POINT_F32_MUL / (x))
#define fix_inverse32_color(x) ((FIX_POINT_F32_MUL*COLOR_MAX) / (x))


/*
	convert float to fixpoint
	fast convert (fistp on x86) HAS to be used..
	hints: compileflag /QIfist for msvc7. msvc 8.0 has smth different
	others should use their favourite assembler..
*/
#if 0
static inline int f_round2(f32 f)
{
	f += (3<<22);
	return IR(f) - 0x4b400000;
}
#endif

/*
	convert f32 to Fix Point.
	multiply is needed anyway, so scale mulby
*/
/*
REALINLINE tFixPoint tofix0 (const f32 x, const f32 mulby = FIX_POINT_F32_MUL )
{
	return (tFixPoint) (x * mulby);
}
*/
#define tofix(x,y) (tFixPoint)(x * y)


/*
	Fix Point , Fix Point Multiply
*/
/*
REALINLINE tFixPointu imulFixu(const tFixPointu x, const tFixPointu y)
{
	return (x * y) >> (tFixPointu) FIX_POINT_PRE;
}
*/
#define imulFixu(x,y) (((x) * (y)) >> (tFixPointu) FIX_POINT_PRE)


/*
	Fix Point , Fix Point Multiply
*/
REALINLINE tFixPoint imulFix(const tFixPoint x, const tFixPoint y)
{
	return (x * y) >> FIX_POINT_PRE;
}

#define imulFix_simple(x,y) ((x*y)>>FIX_POINT_PRE)

#if 0
/*
	Fix Point , Fix Point Multiply x * y * 2
*/
REALINLINE tFixPoint imulFix2(const tFixPoint x, const tFixPoint y)
{
	return ( x * y) >> ( FIX_POINT_PRE -1 );
}
#endif

/*
	Multiply x * y * 1 FIXPOINT_COLOR_MAX
*/
REALINLINE tFixPoint imulFix_tex1(const tFixPoint x, const tFixPoint y)
{
#if SOFTWARE_DRIVER_2_TEXTURE_COLOR_FORMAT == ECF_A8R8G8B8
	return (((tFixPointu)x >> 2)*(((tFixPointu)y + FIX_POINT_ONE) >> 2)) >> (tFixPointu) (FIX_POINT_PRE + 4);
#else
	return (x * (y+ FIX_POINT_ONE)) >> (FIX_POINT_PRE + 5);
#endif
}

/*
	Multiply x * y * 2
*/
REALINLINE tFixPoint imulFix_tex2(const tFixPoint x, const tFixPoint y)
{
	return ( ( (tFixPointu) x >> 2 ) * ( (tFixPointu) y >> 2 ) ) >> (tFixPointu) ( FIX_POINT_PRE + 3 );
}

/*
	Multiply x * y * 4 clamp
*/

REALINLINE tFixPoint imulFix_tex4(const tFixPoint x, const tFixPoint y)
{
#if SOFTWARE_DRIVER_2_TEXTURE_COLOR_FORMAT == ECF_A8R8G8B8
	tFixPoint a = (((tFixPointu)x >> 2)*(((tFixPointu)y + FIX_POINT_ONE) >> 2)) >> (tFixPointu)(FIX_POINT_PRE + 2);
#else
	tFixPoint a = (x * (y + FIX_POINT_ONE)) >> (FIX_POINT_PRE + 3);
#endif
	tFixPoint mask = (a - FIXPOINT_COLOR_MAX) >> 31;
	return (a & mask) | (FIXPOINT_COLOR_MAX & ~mask);
}


/*!
	clamp FixPoint to maxcolor in FixPoint, min(a,COLOR_MAX)
*/
REALINLINE tFixPoint clampfix_maxcolor ( const tFixPoint a)
{
	tFixPoint c = (a - FIXPOINT_COLOR_MAX) >> 31;
	return (a & c) | ( FIXPOINT_COLOR_MAX & ~c);
}


/*!
	clamp FixPoint to 0 in FixPoint, max(a,0)
*/
REALINLINE tFixPoint clampfix_mincolor ( const tFixPoint a)
{
	return a - ( a & ( a >> 31 ) );
}

REALINLINE tFixPoint saturateFix ( const tFixPoint a)
{
	return clampfix_mincolor ( clampfix_maxcolor ( a ) );
}


#if 0
// rount fixpoint to int
inline s32 roundFix ( const tFixPoint x )
{
	return (s32)(( x + FIX_POINT_ZERO_DOT_FIVE ) >> FIX_POINT_PRE);
}
#endif

// x in [0;1[
#if 0
inline s32 f32_to_23Bits(const f32 x)
{
	f32 y = x + 1.f;
	return IR(y) & 0x7FFFFF;	// last 23 bits
}
#endif

/*!
	fixpoint in [0..Fixpoint_color] to VideoSample xrgb
*/
REALINLINE tVideoSample fix_to_sample ( const tFixPoint r, const tFixPoint g, const tFixPoint b )
{
	return	( FIXPOINT_COLOR_MAX & FIXPOINT_COLOR_MAX) << ( SHIFT_A - FIX_POINT_PRE ) |
			( r & FIXPOINT_COLOR_MAX) << ( SHIFT_R - FIX_POINT_PRE ) |
			( g & FIXPOINT_COLOR_MAX) >> ( FIX_POINT_PRE - SHIFT_G ) |
			( b & FIXPOINT_COLOR_MAX) >> ( FIX_POINT_PRE - SHIFT_B );
}


/*!
	fixpoint to VideoSample argb
	a in [0;1]
	rgb in [0;255] colormax
*/
REALINLINE tVideoSample fix4_to_sample ( const tFixPoint a, const tFixPoint r, const tFixPoint g, const tFixPoint b )
{
	return	( a & (FIX_POINT_FRACT_MASK - 1 )) << ( SHIFT_A - 1 ) |
			( r & FIXPOINT_COLOR_MAX) << ( SHIFT_R - FIX_POINT_PRE ) |
			( g & FIXPOINT_COLOR_MAX) >> ( FIX_POINT_PRE - SHIFT_G ) |
			( b & FIXPOINT_COLOR_MAX) >> ( FIX_POINT_PRE - SHIFT_B );
}

/*!
	return fixpoint from VideoSample granularity FIXPOINT_COLOR_MAX
*/
inline void color_to_fix ( tFixPoint &r, tFixPoint &g, tFixPoint &b, const tVideoSample t00 )
{
	(tFixPointu&) r =	(t00 & MASK_R) >> ( SHIFT_R - FIX_POINT_PRE );
	(tFixPointu&) g =	(t00 & MASK_G) << ( FIX_POINT_PRE - SHIFT_G );
	(tFixPointu&) b =	(t00 & MASK_B) << ( FIX_POINT_PRE - SHIFT_B );
}

/*!
	return fixpoint from VideoSample granularity FIXPOINT_COLOR_MAX
*/
inline void color_to_fix ( tFixPoint &a, tFixPoint &r, tFixPoint &g, tFixPoint &b, const tVideoSample t00 )
{
	(tFixPointu&) a =	(t00 & MASK_A) >> ( SHIFT_A - FIX_POINT_PRE );
	(tFixPointu&) r =	(t00 & MASK_R) >> ( SHIFT_R - FIX_POINT_PRE );
	(tFixPointu&) g =	(t00 & MASK_G) << ( FIX_POINT_PRE - SHIFT_G );
	(tFixPointu&) b =	(t00 & MASK_B) << ( FIX_POINT_PRE - SHIFT_B );
}

/*!
	return fixpoint from VideoSample granularity 0..FIX_POINT_ONE
*/
inline void color_to_fix1 ( tFixPoint &r, tFixPoint &g, tFixPoint &b, const tVideoSample t00 )
{
	(tFixPointu&) r =	(t00 & MASK_R) >> ( SHIFT_R + COLOR_MAX_LOG2 - FIX_POINT_PRE );
	(tFixPointu&) g =	(t00 & MASK_G) >> ( SHIFT_G + COLOR_MAX_LOG2 - FIX_POINT_PRE );
	(tFixPointu&) b =	(t00 & MASK_B) << ( FIX_POINT_PRE - COLOR_MAX_LOG2 );

	//0..255 -> 0..256 | c += c >= 0.5 ? 1 : 0
	r += (r & FIX_POINT_ZERO_DOT_FIVE) ? FIX_POINT_COLOR_ERROR : 0;
	g += (g & FIX_POINT_ZERO_DOT_FIVE) ? FIX_POINT_COLOR_ERROR : 0;
	b += (b & FIX_POINT_ZERO_DOT_FIVE) ? FIX_POINT_COLOR_ERROR : 0;
}

/*!
	return fixpoint from VideoSample granularity 0..FIX_POINT_ONE
*/
inline void color_to_fix1 ( tFixPoint &a, tFixPoint &r, tFixPoint &g, tFixPoint &b, const tVideoSample t00 )
{
	(tFixPointu&) a =	(t00 & MASK_A) >> ( SHIFT_A + COLOR_MAX_LOG2 - FIX_POINT_PRE );
	(tFixPointu&) r =	(t00 & MASK_R) >> ( SHIFT_R + COLOR_MAX_LOG2 - FIX_POINT_PRE );
	(tFixPointu&) g =	(t00 & MASK_G) >> ( SHIFT_G + COLOR_MAX_LOG2 - FIX_POINT_PRE );
	(tFixPointu&) b =	(t00 & MASK_B) << ( FIX_POINT_PRE - COLOR_MAX_LOG2 );

	//0..255 -> 0..256 | c += c >= 0.5 ? 1 : 0
	a += (a & FIX_POINT_ZERO_DOT_FIVE) ? FIX_POINT_COLOR_ERROR : 0;
	r += (r & FIX_POINT_ZERO_DOT_FIVE) ? FIX_POINT_COLOR_ERROR : 0;
	g += (g & FIX_POINT_ZERO_DOT_FIVE) ? FIX_POINT_COLOR_ERROR : 0;
	b += (b & FIX_POINT_ZERO_DOT_FIVE) ? FIX_POINT_COLOR_ERROR : 0;

}

/*!
	return fixpoint from VideoSample granularity FIXPOINT_COLOR_MAX
*/
inline void color_to_fix(tFixPoint c[4], const tVideoSample t00)
{
	c[0] = (t00 & MASK_A) >> (SHIFT_A - FIX_POINT_PRE);
	c[1] = (t00 & MASK_R) >> (SHIFT_R - FIX_POINT_PRE);
	c[2] = (t00 & MASK_G) << (FIX_POINT_PRE - SHIFT_G);
	c[3] = (t00 & MASK_B) << (FIX_POINT_PRE - SHIFT_B);
}

/*!
	return fixpoint from VideoSample granularity 0..FIX_POINT_ONE
*/
inline void color_to_fix1(tFixPoint c[4], const tVideoSample t00)
{
	c[0] = (t00 & MASK_A) >> (SHIFT_A + COLOR_MAX_LOG2 - FIX_POINT_PRE);
	c[1] = (t00 & MASK_R) >> (SHIFT_R + COLOR_MAX_LOG2 - FIX_POINT_PRE);
	c[2] = (t00 & MASK_G) >> (SHIFT_G + COLOR_MAX_LOG2 - FIX_POINT_PRE);
	c[3] = (t00 & MASK_B) << (FIX_POINT_PRE - COLOR_MAX_LOG2);

	//0..255 -> 0..256 | c += c >= 0.5 ? 1 : 0
	c[0] += (c[0] & FIX_POINT_ZERO_DOT_FIVE) ? FIX_POINT_COLOR_ERROR : 0;
	c[1] += (c[1] & FIX_POINT_ZERO_DOT_FIVE) ? FIX_POINT_COLOR_ERROR : 0;
	c[2] += (c[2] & FIX_POINT_ZERO_DOT_FIVE) ? FIX_POINT_COLOR_ERROR : 0;
	c[3] += (c[3] & FIX_POINT_ZERO_DOT_FIVE) ? FIX_POINT_COLOR_ERROR : 0;

}



//! ----- FP24 1.23 fix point z-buffer

#if 1
typedef f32 fp24;
#else
struct fp24
{
	u32 v;

	fp24() {}

	fp24 ( const f32 f )
	{
		f32 y = f + 1.f;
		v = ((u32&)y) & 0x7FFFFF;	// last 23 bits
	}

	void operator=(const f32 f )
	{
	f32 y = f + 1.f;
		v = ((u32&)y) & 0x7FFFFF;	// last 23 bits
		}

	void operator+=(const fp24 &other )
	{
		v += other.v;
	}

	operator f32 () const
	{
		f32 r = FR ( v );
		return r + 1.f;
	}

};
#endif


// ------------------------ Internal Texture -----------------------------

struct sInternalTexture
{
	//power-of-two
	void* data; //tVideoSample* Texture->lock(miplevel)
	size_t textureXMask;
	size_t textureYMask;

	size_t pitchlog2;

	video::CSoftwareTexture2 *Texture;
	s32 lodFactor; // magnify/minify
};



// get video sample plain
static inline tVideoSample getTexel_plain ( const sInternalTexture* t, const tFixPointu tx, const tFixPointu ty )
{
	size_t ofs;

	ofs = ( ( ty & t->textureYMask ) >> FIX_POINT_PRE ) << t->pitchlog2;
	ofs |= ( tx & t->textureXMask ) >> ( FIX_POINT_PRE - SOFTWARE_DRIVER_2_TEXTURE_GRANULARITY );

	// texel
	return *((tVideoSample*)( (u8*) t->data + ofs ));
}

// get video sample to fix
inline void getTexel_fix ( tFixPoint &r, tFixPoint &g, tFixPoint &b,
						const sInternalTexture* t, const tFixPointu tx, const tFixPointu ty
								)
{
	size_t ofs;

	ofs = ( ((ty + FIX_POINT_ZERO_DOT_FIVE) & t->textureYMask ) >> FIX_POINT_PRE ) << t->pitchlog2;
	ofs |= ((tx + FIX_POINT_ZERO_DOT_FIVE) & t->textureXMask ) >> ( FIX_POINT_PRE - SOFTWARE_DRIVER_2_TEXTURE_GRANULARITY );

	// texel
	tVideoSample t00;
	t00 = *((tVideoSample*)( (u8*) t->data + ofs ));

	r = (t00 & MASK_R) >> ( SHIFT_R - FIX_POINT_PRE);
	g = (t00 & MASK_G) << ( FIX_POINT_PRE - SHIFT_G );
	b = (t00 & MASK_B) << ( FIX_POINT_PRE - SHIFT_B );

}

// get video sample to fixpoint colormax
inline void getTexel_fix(tFixPoint &a, tFixPoint &r, tFixPoint &g, tFixPoint &b,
	const sInternalTexture* t, const tFixPointu tx, const tFixPointu ty
)
{
	size_t ofs;

	ofs = (((ty+ FIX_POINT_ZERO_DOT_FIVE) & t->textureYMask) >> FIX_POINT_PRE) << t->pitchlog2;
	ofs |= ((tx+ FIX_POINT_ZERO_DOT_FIVE) & t->textureXMask) >> (FIX_POINT_PRE - SOFTWARE_DRIVER_2_TEXTURE_GRANULARITY);

	// texel
	tVideoSample t00;
	t00 = *((tVideoSample*)((u8*)t->data + ofs));

	a = (t00 & MASK_A) >> (SHIFT_A - FIX_POINT_PRE);
	r = (t00 & MASK_R) >> (SHIFT_R - FIX_POINT_PRE);
	g = (t00 & MASK_G) << (FIX_POINT_PRE - SHIFT_G);
	b = (t00 & MASK_B) << (FIX_POINT_PRE - SHIFT_B);

}

#if 0
// get video sample to fixpoint
static REALINLINE void getTexel_fix ( tFixPoint &a,
			const sInternalTexture * t, const tFixPointu tx, const tFixPointu ty)
{
	size_t ofs;

	ofs = ( ((ty + FIX_POINT_ZERO_DOT_FIVE) & t->textureYMask ) >> FIX_POINT_PRE ) << t->pitchlog2;
	ofs |= ((tx + FIX_POINT_ZERO_DOT_FIVE) & t->textureXMask ) >> ( FIX_POINT_PRE - SOFTWARE_DRIVER_2_TEXTURE_GRANULARITY );

	// texel
	tVideoSample t00;
	t00 = *((tVideoSample*)( (u8*) t->data + ofs ));

	a = (t00 & MASK_A) >> ( SHIFT_A - FIX_POINT_PRE);
}
#endif

/*
	load a sample from internal texture at position tx,ty to fixpoint
*/
#if defined(SOFTWARE_DRIVER_2_BILINEAR)

#if 0
// texture2D in fixpoint color range bilinear
static REALINLINE void getSample_texture(tFixPoint &r, tFixPoint &g, tFixPoint &b,
	const sInternalTexture* burning_restrict t, const tFixPointu tx, const tFixPointu ty
)
{
#if 0
	if (t->lodFactor > 0)
	{
		size_t ofs;

		ofs = (((ty + FIX_POINT_ZERO_DOT_FIVE) & t->textureYMask) >> FIX_POINT_PRE) << t->pitchlog2;
		ofs += ((tx + FIX_POINT_ZERO_DOT_FIVE) & t->textureXMask) >> (FIX_POINT_PRE - SOFTWARE_DRIVER_2_TEXTURE_GRANULARITY);

		// texel
		tVideoSample t00;
		t00 = *((tVideoSample*)((u8*)t->data + ofs));

		r = (t00 & MASK_R) >> (SHIFT_R - FIX_POINT_PRE);
		g = (t00 & MASK_G) << (FIX_POINT_PRE - SHIFT_G);
		b = (t00 & MASK_B) << (FIX_POINT_PRE - SHIFT_B);
		return;
	}
#endif

	tFixPointu r00, g00, b00;
	tFixPointu r01, g01, b01;
	tFixPointu r10, g10, b10;
	tFixPointu r11, g11, b11;

	size_t o0, o1, o2, o3;
	tVideoSample t00;

	//wraps positive (ignoring negative)
	o0 = (((ty)& t->textureYMask) >> FIX_POINT_PRE) << t->pitchlog2;
	o1 = (((ty + FIX_POINT_ONE) & t->textureYMask) >> FIX_POINT_PRE) << t->pitchlog2;
	o2 = ((tx)& t->textureXMask) >> (FIX_POINT_PRE - SOFTWARE_DRIVER_2_TEXTURE_GRANULARITY);
	o3 = ((tx + FIX_POINT_ONE) & t->textureXMask) >> (FIX_POINT_PRE - SOFTWARE_DRIVER_2_TEXTURE_GRANULARITY);

	t00 = *((tVideoSample*)((u8*)t->data + (o0 + o2)));
	r00 = (t00 & MASK_R) >> SHIFT_R;
	g00 = (t00 & MASK_G) >> SHIFT_G;
	b00 = (t00 & MASK_B);

	t00 = *((tVideoSample*)((u8*)t->data + (o0 + o3)));
	r10 = (t00 & MASK_R) >> SHIFT_R;
	g10 = (t00 & MASK_G) >> SHIFT_G;
	b10 = (t00 & MASK_B);

	t00 = *((tVideoSample*)((u8*)t->data + (o1 + o2)));
	r01 = (t00 & MASK_R) >> SHIFT_R;
	g01 = (t00 & MASK_G) >> SHIFT_G;
	b01 = (t00 & MASK_B);

	t00 = *((tVideoSample*)((u8*)t->data + (o1 + o3)));
	r11 = (t00 & MASK_R) >> SHIFT_R;
	g11 = (t00 & MASK_G) >> SHIFT_G;
	b11 = (t00 & MASK_B);


	tFixPointu fracx = tx & FIX_POINT_FRACT_MASK;
	tFixPointu fracy = ty & FIX_POINT_FRACT_MASK;

	//w00 w01 w10 w11
	tFixPointu w[4];
	w[0] = imulFixu(FIX_POINT_ONE - fracx, FIX_POINT_ONE - fracy);
	w[1] = imulFixu(FIX_POINT_ONE - fracx, fracy);
	w[2] = imulFixu(fracx, FIX_POINT_ONE - fracy);
	w[3] = imulFixu(fracx, fracy);

	r = (r00 * w[0]) +
		(r01 * w[1]) +
		(r10 * w[2]) +
		(r11 * w[3]);

	g = (g00 * w[0]) +
		(g01 * w[1]) +
		(g10 * w[2]) +
		(g11 * w[3]);

	b = (b00 * w[0]) +
		(b01 * w[1]) +
		(b10 * w[2]) +
		(b11 * w[3]);

}

#else

// texture2D in fixpoint color range bilinear
static REALINLINE void getSample_texture(tFixPoint &r, tFixPoint &g, tFixPoint &b,
	const sInternalTexture* burning_restrict tex, const tFixPointu tx, const tFixPointu ty
)
{
#if 0
	if (tex->lodFactor > 1)
	{
		//nearest neighbor
		size_t ofs;
		ofs = (((ty + FIX_POINT_ZERO_DOT_FIVE) & tex->textureYMask) >> FIX_POINT_PRE) << tex->pitchlog2;
		ofs += ((tx + FIX_POINT_ZERO_DOT_FIVE) & tex->textureXMask) >> (FIX_POINT_PRE - SOFTWARE_DRIVER_2_TEXTURE_GRANULARITY);

		tVideoSample t00;
		t00 = *((tVideoSample*)((u8*)tex->data + ofs));

		r = (t00 & MASK_R) >> (SHIFT_R - FIX_POINT_PRE);
		g = (t00 & MASK_G) << (FIX_POINT_PRE - SHIFT_G);
		b = (t00 & MASK_B) << (FIX_POINT_PRE - SHIFT_B);
		return;
	}
#endif
	//w00 w01 w10 w11
	tFixPointu w[4];
	{
		tFixPointu fracx = tx & FIX_POINT_FRACT_MASK;
		tFixPointu fracy = ty & FIX_POINT_FRACT_MASK;
		w[0] = imulFixu(FIX_POINT_ONE - fracx, FIX_POINT_ONE - fracy);
		w[1] = imulFixu(fracx, FIX_POINT_ONE - fracy);
		w[2] = imulFixu(FIX_POINT_ONE - fracx, fracy);
		w[3] = imulFixu(fracx, fracy);
	}

	//wraps positive (ignoring negative)
	tVideoSample t[4];
	{
		size_t o0, o1, o2, o3;
		o0 = (((ty) & tex->textureYMask) >> FIX_POINT_PRE) << tex->pitchlog2;
		o1 = (((ty + FIX_POINT_ONE) & tex->textureYMask) >> FIX_POINT_PRE) << tex->pitchlog2;
		o2 = ((tx)& tex->textureXMask) >> (unsigned)(FIX_POINT_PRE - SOFTWARE_DRIVER_2_TEXTURE_GRANULARITY);
		o3 = ((tx + FIX_POINT_ONE) & tex->textureXMask) >> (unsigned)(FIX_POINT_PRE - SOFTWARE_DRIVER_2_TEXTURE_GRANULARITY);

		t[0] = *((tVideoSample*)((u8*)tex->data + (o0 + o2)));
		t[1] = *((tVideoSample*)((u8*)tex->data + (o0 + o3)));
		t[2] = *((tVideoSample*)((u8*)tex->data + (o1 + o2)));
		t[3] = *((tVideoSample*)((u8*)tex->data + (o1 + o3)));
	}

	r = (((t[0] & MASK_R) >> SHIFT_R) * w[0]) +
		(((t[1] & MASK_R) >> SHIFT_R) * w[1]) +
		(((t[2] & MASK_R) >> SHIFT_R) * w[2]) +
		(((t[3] & MASK_R) >> SHIFT_R) * w[3]);

	g = (((t[0] & MASK_G) >> SHIFT_G) * w[0]) +
		(((t[1] & MASK_G) >> SHIFT_G) * w[1]) +
		(((t[2] & MASK_G) >> SHIFT_G) * w[2]) +
		(((t[3] & MASK_G) >> SHIFT_G) * w[3]);

	b = ((t[0] & MASK_B) * w[0]) +
		((t[1] & MASK_B) * w[1]) +
		((t[2] & MASK_B) * w[2]) +
		((t[3] & MASK_B) * w[3]);
}

#endif

// get Sample bilinear
static REALINLINE void getSample_texture(tFixPoint &a, tFixPoint &r, tFixPoint &g, tFixPoint &b,
	const sInternalTexture* burning_restrict tex, const tFixPointu tx, const tFixPointu ty
)
{

	tFixPointu a00, r00, g00, b00;
	tFixPointu a01, r01, g01, b01;
	tFixPointu a10, r10, g10, b10;
	tFixPointu a11, r11, g11, b11;

	size_t o0, o1, o2, o3;
	tVideoSample t00;

	o0 = (((ty)& tex->textureYMask) >> FIX_POINT_PRE) << tex->pitchlog2;
	o1 = (((ty + FIX_POINT_ONE) & tex->textureYMask) >> FIX_POINT_PRE) << tex->pitchlog2;
	o2 = ((tx)& tex->textureXMask) >> (FIX_POINT_PRE - SOFTWARE_DRIVER_2_TEXTURE_GRANULARITY);
	o3 = ((tx + FIX_POINT_ONE) & tex->textureXMask) >> (FIX_POINT_PRE - SOFTWARE_DRIVER_2_TEXTURE_GRANULARITY);

	t00 = *((tVideoSample*)((u8*)tex->data + (o0 + o2)));
	a00 = (t00 & MASK_A) >> SHIFT_A;
	r00 = (t00 & MASK_R) >> SHIFT_R;
	g00 = (t00 & MASK_G) >> SHIFT_G;
	b00 = (t00 & MASK_B);

	t00 = *((tVideoSample*)((u8*)tex->data + (o0 + o3)));
	a10 = (t00 & MASK_A) >> SHIFT_A;
	r10 = (t00 & MASK_R) >> SHIFT_R;
	g10 = (t00 & MASK_G) >> SHIFT_G;
	b10 = (t00 & MASK_B);

	t00 = *((tVideoSample*)((u8*)tex->data + (o1 + o2)));
	a01 = (t00 & MASK_A) >> SHIFT_A;
	r01 = (t00 & MASK_R) >> SHIFT_R;
	g01 = (t00 & MASK_G) >> SHIFT_G;
	b01 = (t00 & MASK_B);

	t00 = *((tVideoSample*)((u8*)tex->data + (o1 + o3)));
	a11 = (t00 & MASK_A) >> SHIFT_A;
	r11 = (t00 & MASK_R) >> SHIFT_R;
	g11 = (t00 & MASK_G) >> SHIFT_G;
	b11 = (t00 & MASK_B);

	const tFixPointu txFract = tx & FIX_POINT_FRACT_MASK;
	const tFixPointu txFractInv = FIX_POINT_ONE - txFract;

	const tFixPointu tyFract = ty & FIX_POINT_FRACT_MASK;
	const tFixPointu tyFractInv = FIX_POINT_ONE - tyFract;

	const tFixPointu w00 = imulFixu(txFractInv, tyFractInv);
	const tFixPointu w10 = imulFixu(txFract, tyFractInv);
	const tFixPointu w01 = imulFixu(txFractInv, tyFract);
	const tFixPointu w11 = imulFixu(txFract, tyFract);

	a = (a00 * w00) +
		(a01 * w01) +
		(a10 * w10) +
		(a11 * w11);

	fix_alpha_color_max(a);

	r = (r00 * w00) +
		(r01 * w01) +
		(r10 * w10) +
		(r11 * w11);

	g = (g00 * w00) +
		(g01 * w01) +
		(g10 * w10) +
		(g11 * w11);

	b = (b00 * w00) +
		(b01 * w01) +
		(b10 * w10) +
		(b11 * w11);

}

#else // SOFTWARE_DRIVER_2_BILINEAR

// get Sample linear == getSample_fixpoint

static REALINLINE void getSample_texture(tFixPoint &r, tFixPoint &g, tFixPoint &b,
	const sInternalTexture* burning_restrict t, const tFixPointu tx, const tFixPointu ty
)
{
	size_t ofs;
	ofs = (((ty + FIX_POINT_ZERO_DOT_FIVE) & t->textureYMask) >> FIX_POINT_PRE) << t->pitchlog2;
	ofs += ((tx + FIX_POINT_ZERO_DOT_FIVE) & t->textureXMask) >> (FIX_POINT_PRE - SOFTWARE_DRIVER_2_TEXTURE_GRANULARITY);

	// texel
	const tVideoSample t00 = *((tVideoSample*)((u8*)t->data + ofs));

	(tFixPointu &)r = (t00 & MASK_R) >> (SHIFT_R - FIX_POINT_PRE);
	(tFixPointu &)g = (t00 & MASK_G) << (FIX_POINT_PRE - SHIFT_G);
	(tFixPointu &)b = (t00 & MASK_B) << (FIX_POINT_PRE - SHIFT_B);
}

static REALINLINE void getSample_texture(tFixPoint &a, tFixPoint &r, tFixPoint &g, tFixPoint &b,
	const sInternalTexture* burning_restrict t, const tFixPointu tx, const tFixPointu ty
)
{
	size_t ofs;
	ofs = (((ty + FIX_POINT_ZERO_DOT_FIVE) & t->textureYMask) >> FIX_POINT_PRE) << t->pitchlog2;
	ofs += ((tx + FIX_POINT_ZERO_DOT_FIVE) & t->textureXMask) >> (FIX_POINT_PRE - SOFTWARE_DRIVER_2_TEXTURE_GRANULARITY);

	// texel
	const tVideoSample t00 = *((tVideoSample*)((u8*)t->data + ofs));

	(tFixPointu &)a = (t00 & MASK_A) >> (SHIFT_A - FIX_POINT_PRE);
	fix_alpha_color_max(a);
	(tFixPointu &)r = (t00 & MASK_R) >> (SHIFT_R - FIX_POINT_PRE);
	(tFixPointu &)g = (t00 & MASK_G) << (FIX_POINT_PRE - SHIFT_G);
	(tFixPointu &)b = (t00 & MASK_B) << (FIX_POINT_PRE - SHIFT_B);
}


#endif // SOFTWARE_DRIVER_2_BILINEAR


// 2D Region closed [x0;x1]
struct AbsRectangle
{
	s32 x0;
	s32 y0;
	s32 x1;
	s32 y1;
};

//! 2D Intersection test
inline bool intersect ( AbsRectangle &dest, const AbsRectangle& a, const AbsRectangle& b)
{
	dest.x0 = core::s32_max( a.x0, b.x0 );
	dest.y0 = core::s32_max( a.y0, b.y0 );
	dest.x1 = core::s32_min( a.x1, b.x1 );
	dest.y1 = core::s32_min( a.y1, b.y1 );
	return dest.x0 < dest.x1 && dest.y0 < dest.y1;
}

#if 0
// some 1D defines
struct sIntervall
{
	s32 start;
	s32 end;
};

// returning intersection width
inline s32 intervall_intersect_test( const sIntervall& a, const sIntervall& b)
{
	return core::s32_min( a.end, b.end ) - core::s32_max( a.start, b.start );
}

#endif

// strings
static inline void tiny_strncpy(char* to, const char* from, const size_t count)
{
	for (size_t r = 0; r < count && (*to = *from) != '\0'; ++from, ++to, ++r);
	*to = '\0';
}

#define tiny_strcpy(a, b) tiny_strncpy(a,b,sizeof(a)-1)


// tiny_isequal = !strncmp(a,b,sizeof(a)-1)
static inline int tiny_isequal(const char *s1, const char *s2, size_t n)
{
	do {
		if (*s1 != *s2++) return 0;
		if (*s1++ == 0)
			break;
	} while (--n != 0);
	return 1;
}

#define tiny_istoken(a, b) tiny_isequal(a,b,sizeof(a)-1) != 0


} // end namespace irr

#endif

