// Copyright (C) 2002-2012 Nikolaus Gebhardt / Thomas Alten
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __S_VIDEO_2_SOFTWARE_COMPILE_CONFIG_H_INCLUDED__
#define __S_VIDEO_2_SOFTWARE_COMPILE_CONFIG_H_INCLUDED__

#include "IrrCompileConfig.h"

// Generic Render Flags for burning's video rasterizer
// defined now in irrlicht compile config

#if defined(PATCH_SUPERTUX_8_0_1)
	#undef BURNINGVIDEO_RENDERER_BEAUTIFUL

	//#define SOFTWARE_DRIVER_2_PERSPECTIVE_CORRECT
	#define SOFTWARE_DRIVER_2_SUBTEXEL
	//#define SOFTWARE_DRIVER_2_BILINEAR
	#define SOFTWARE_DRIVER_2_LIGHTING
	#define SOFTWARE_DRIVER_2_USE_VERTEX_COLOR
	#define SOFTWARE_DRIVER_2_USE_SEPARATE_SPECULAR_COLOR
	#define SOFTWARE_DRIVER_2_32BIT
	#define SOFTWARE_DRIVER_2_MIPMAPPING
	#define SOFTWARE_DRIVER_2_USE_WBUFFER
	#define SOFTWARE_DRIVER_2_TEXTURE_TRANSFORM
	#define SOFTWARE_DRIVER_2_TEXTURE_MAXSIZE		256
	#define SOFTWARE_DRIVER_2_SCANLINE_MAG_MIN
	#define SOFTWARE_DRIVER_2_CLIPPING
	#define SOFTWARE_DRIVER_2_2D_AS_2D
#endif


#ifdef BURNINGVIDEO_RENDERER_BEAUTIFUL
	#define SOFTWARE_DRIVER_2_PERSPECTIVE_CORRECT
	#define SOFTWARE_DRIVER_2_SUBTEXEL
	#define SOFTWARE_DRIVER_2_BILINEAR
	#define SOFTWARE_DRIVER_2_LIGHTING
	#define SOFTWARE_DRIVER_2_USE_VERTEX_COLOR
	#define SOFTWARE_DRIVER_2_USE_SEPARATE_SPECULAR_COLOR
	#define SOFTWARE_DRIVER_2_32BIT
	#define SOFTWARE_DRIVER_2_MIPMAPPING
	#define SOFTWARE_DRIVER_2_USE_WBUFFER
	#define SOFTWARE_DRIVER_2_TEXTURE_TRANSFORM
	#define SOFTWARE_DRIVER_2_TEXTURE_MAXSIZE		0
	#define SOFTWARE_DRIVER_2_SCANLINE_MAG_MIN
	#define SOFTWARE_DRIVER_2_CLIPPING
	#define SOFTWARE_DRIVER_2_2D_AS_3D
#endif

//! Set Flags for Windows Mobile
#ifdef BURNINGVIDEO_RENDERER_CE
	#define SOFTWARE_DRIVER_2_PERSPECTIVE_CORRECT
	#define SOFTWARE_DRIVER_2_SUBTEXEL
	//#define SOFTWARE_DRIVER_2_BILINEAR
	//#define SOFTWARE_DRIVER_2_LIGHTING
	#define SOFTWARE_DRIVER_2_USE_VERTEX_COLOR
	#define SOFTWARE_DRIVER_2_16BIT
	#define SOFTWARE_DRIVER_2_MIPMAPPING
	#define SOFTWARE_DRIVER_2_USE_WBUFFER
	//#define SOFTWARE_DRIVER_2_TEXTURE_TRANSFORM
	#define SOFTWARE_DRIVER_2_TEXTURE_MAXSIZE		64
	#define SOFTWARE_DRIVER_2_SCANLINE_MAG_MIN
	//#define SOFTWARE_DRIVER_2_CLIPPING
	#define SOFTWARE_DRIVER_2_2D_AS_2D
#endif

#ifdef BURNINGVIDEO_RENDERER_FAST
	#define SOFTWARE_DRIVER_2_PERSPECTIVE_CORRECT
	#define SOFTWARE_DRIVER_2_SUBTEXEL
	//#define SOFTWARE_DRIVER_2_BILINEAR
	//#define SOFTWARE_DRIVER_2_LIGHTING
	#define SOFTWARE_DRIVER_2_USE_VERTEX_COLOR
	//#define SOFTWARE_DRIVER_2_32BIT
	#define SOFTWARE_DRIVER_2_16BIT
	#define SOFTWARE_DRIVER_2_MIPMAPPING
	#define SOFTWARE_DRIVER_2_USE_WBUFFER
	#define SOFTWARE_DRIVER_2_TEXTURE_MAXSIZE		256
	#define SOFTWARE_DRIVER_2_SCANLINE_MAG_MIN
	#define SOFTWARE_DRIVER_2_CLIPPING
	#define SOFTWARE_DRIVER_2_2D_AS_2D
#endif

#ifdef BURNINGVIDEO_RENDERER_ULTRA_FAST
	#define BURNINGVIDEO_RENDERER_FAST

	//#define SOFTWARE_DRIVER_2_PERSPECTIVE_CORRECT
	//#define SOFTWARE_DRIVER_2_SUBTEXEL
	//#define SOFTWARE_DRIVER_2_BILINEAR
	//#define SOFTWARE_DRIVER_2_LIGHTING
	#define SOFTWARE_DRIVER_2_USE_VERTEX_COLOR
	//#define SOFTWARE_DRIVER_2_32BIT
	#define SOFTWARE_DRIVER_2_16BIT
	//#define SOFTWARE_DRIVER_2_MIPMAPPING
	//#define SOFTWARE_DRIVER_2_USE_WBUFFER
	//#define SOFTWARE_DRIVER_2_TEXTURE_TRANSFORM
	#define SOFTWARE_DRIVER_2_TEXTURE_MAXSIZE		128
	#define SOFTWARE_DRIVER_2_SCANLINE_MAG_MIN
	//#define SOFTWARE_DRIVER_2_CLIPPING
	#define SOFTWARE_DRIVER_2_2D_AS_2D
#endif

// Derivate flags

// texture format
#ifdef SOFTWARE_DRIVER_2_32BIT
	#define	BURNINGSHADER_COLOR_FORMAT	ECF_A8R8G8B8
#else
	#define	BURNINGSHADER_COLOR_FORMAT	ECF_A1R5G5B5
#endif

// mip mapping - precalculated texture filter
#if defined ( SOFTWARE_DRIVER_2_MIPMAPPING )
	#if defined( BURNINGVIDEO_RENDERER_BEAUTIFUL )
		#define SOFTWARE_DRIVER_2_MIPMAPPING_MAX		16
		#define SOFTWARE_DRIVER_2_MIPMAPPING_MIN_SIZE   1
	#elif defined ( BURNINGVIDEO_RENDERER_CE )
		#define SOFTWARE_DRIVER_2_MIPMAPPING_MAX		4
		#define SOFTWARE_DRIVER_2_MIPMAPPING_MIN_SIZE   8
	#else
		#define SOFTWARE_DRIVER_2_MIPMAPPING_MAX		4
		#define SOFTWARE_DRIVER_2_MIPMAPPING_MIN_SIZE   8
	#endif
#else
	#define SOFTWARE_DRIVER_2_MIPMAPPING_MAX		1
	#define SOFTWARE_DRIVER_2_MIPMAPPING_MIN_SIZE   1
#endif



#ifndef REALINLINE
	#ifdef _MSC_VER
		#define REALINLINE __forceinline
	#else
		#define REALINLINE inline
	#endif
#endif


// null check necessary (burningvideo only)
#define reciprocal_zero(x) ((x) != 0.f ? 1.f / (x):0.f)
static inline float reciprocal_zero2(float x) { return x != 0.f ? 1.f / x : 0.f; }
#define reciprocal_one(x) ((x) != 0.f ? 1.f / (x):1.f)

#define fill_convention_left(x) (s32) ceilf(x)
#define fill_convention_right(x) ((s32) ceilf(x))-1
#define fill_convention_none(x) (s32) (x)
//#define fill_convention_left(x) 65536 - int(65536.0f - x)
//#define fill_convention_right(x) 65535 - int(65536.0f - x)


//Check coordinates are in render target/window space
//#define SOFTWARE_DRIVER_2_DO_CLIPCHECK
#if defined (SOFTWARE_DRIVER_2_DO_CLIPCHECK) && defined(_WIN32)
#define SOFTWARE_DRIVER_2_CLIPCHECK      if( xStart < 0 || xStart + dx >= (s32)RenderTarget->getDimension().Width || line.y < 0 || line.y >= (s32) RenderTarget->getDimension().Height ) __debugbreak()
#define SOFTWARE_DRIVER_2_CLIPCHECK_REF  if( pShader.xStart < 0 || pShader.xStart + pShader.dx >= (s32)RenderTarget->getDimension().Width || line.y < 0 || line.y >= (s32) RenderTarget->getDimension().Height ) __debugbreak()
#define SOFTWARE_DRIVER_2_CLIPCHECK_WIRE if( aposx < 0 || aposx >= (s32)RenderTarget->getDimension().Width || aposy < 0 || aposy >= (s32) RenderTarget->getDimension().Height ) __debugbreak()

inline float reciprocal_zero_no(const float x)
{
	if (x*x <= 0.00001f) __debugbreak();
	return 1.f / x;
}
#else
#define SOFTWARE_DRIVER_2_CLIPCHECK
#define SOFTWARE_DRIVER_2_CLIPCHECK_REF
#define SOFTWARE_DRIVER_2_CLIPCHECK_WIRE

#define reciprocal_zero_no(x) 1.f/x
#endif

//!scanline renderer emulate line
enum edge_test_flag
{
	edge_test_pass = 1,		//! not wireframe
	edge_test_left = 0,
	edge_test_first_line = 2,
	edge_test_point = 4
};
//if any edge test flag is set result=1 else 0. ( pass height test for degenerate triangle )
#define reciprocal_edge(x) ((x) != 0.f ? 1.f / (x):(~EdgeTestPass)&1)

//! normalize from fixed point Color Max to fixed point [0;1]
#define fix_color_norm(x) x = (x+1) >> COLOR_MAX_LOG2

//! from 1 bit to 5 bit
#ifdef SOFTWARE_DRIVER_2_32BIT
	#define fix_alpha_color_max(x)
#else
	#define fix_alpha_color_max(x) if (x) x = (x << COLOR_MAX_LOG2) - 1
#endif

// Check windows
#if _WIN32 || _WIN64
#if _WIN64
	#define ENV64BIT
#else
	#define ENV32BIT
#endif
#endif

// Check GCC
#if __GNUC__
#if __x86_64__ || __ppc64__
	#define ENV64BIT
#else
	#define ENV32BIT
#endif
#endif

#if defined(ENV64BIT) && defined(BURNINGVIDEO_RENDERER_BEAUTIFUL)
	typedef float ipoltype;
#else
	typedef float ipoltype;
#endif

#define	ipol_lower_equal_0(n)	((n) <= (ipoltype)0.0)
#define	ipol_greater_0(n)		((n) >  (ipoltype)0.0)

#if	(_MSC_VER >= 1600 )
	#define burning_restrict __restrict
#else
	#define burning_restrict
#endif

/*
	if (condition) state |= mask; else state &= ~mask;
*/
static inline void burning_setbit(size_t &state, int condition, size_t mask)
{
	if (condition) state |= mask;
	else state &= ~mask;
}

/*
	if (condition) state |= m; else state &= ~m;
*/
REALINLINE void burning_setbit32(unsigned int &state, int condition, const unsigned int mask)
{
	// 0, or any positive to mask
	//s32 conmask = -condition >> 31;
	state ^= ((-condition >> 31) ^ state) & mask;
}

#define burning_stringify(s) #s
#define burning_create_indirect(s) create_##s
#define burning_create(s) burning_create_indirect(s)


#if defined(PATCH_SUPERTUX_8_0_1)
#define getData lock
#define snprintf_irr sprintf_s

#ifdef SOFTWARE_DRIVER_2_USE_VERTEX_COLOR
#ifdef SOFTWARE_DRIVER_2_USE_SEPARATE_SPECULAR_COLOR
#define BURNING_MATERIAL_MAX_COLORS 2
#else
#define BURNING_MATERIAL_MAX_COLORS 1
#endif
#else
#define BURNING_MATERIAL_MAX_COLORS 0
#endif

#ifndef _IRR_OVERRIDE_
#define _IRR_OVERRIDE_ /**/
#endif

#define fix_to_color fix_to_sample
#define fix4_to_color fix4_to_sample
#define vec4_to_fix getSample_color
#define SOFTWARE_DRIVER_2_MIPMAPPING_LOD_BIAS 0

namespace irr {

	REALINLINE void memcpy32_small(void * dest, const void *source, size_t bytesize)
	{
		size_t c = bytesize >> 2;

		do
		{
			((unsigned int *)dest)[c - 1] = ((unsigned int *)source)[c - 1];
		} while (--c);

	}

} // namespace irr
#endif // #if defined(PATCH_SUPERTUX_8_0_1)


#endif // __S_VIDEO_2_SOFTWARE_COMPILE_CONFIG_H_INCLUDED__
