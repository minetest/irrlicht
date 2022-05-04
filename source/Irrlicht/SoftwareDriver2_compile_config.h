// Copyright (C) 2002-2012 Nikolaus Gebhardt / Thomas Alten
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef S_VIDEO_2_SOFTWARE_COMPILE_CONFIG_H_INCLUDED
#define S_VIDEO_2_SOFTWARE_COMPILE_CONFIG_H_INCLUDED

#include "IrrCompileConfig.h"

// Generic Render Flags for burning's video rasterizer
// defined now in irrlicht compile config

#if defined(PATCH_SUPERTUX_8_0_1_with_1_9_0)
#undef BURNINGVIDEO_RENDERER_BEAUTIFUL

#define SOFTWARE_DRIVER_2_PERSPECTIVE_CORRECT
#define SOFTWARE_DRIVER_2_SUBTEXEL
#define SOFTWARE_DRIVER_2_BILINEAR
#define SOFTWARE_DRIVER_2_LIGHTING
#define SOFTWARE_DRIVER_2_USE_VERTEX_COLOR
#define SOFTWARE_DRIVER_2_USE_SEPARATE_SPECULAR_COLOR
#define SOFTWARE_DRIVER_2_USE_WBUFFER
#define SOFTWARE_DRIVER_2_32BIT
#define	SOFTWARE_DRIVER_2_TEXTURE_COLOR_FORMAT	ECF_A8R8G8B8
#define	SOFTWARE_DRIVER_2_RENDERTARGET_COLOR_FORMAT	ECF_A8R8G8B8
#define SOFTWARE_DRIVER_2_TEXTURE_MAXSIZE		0x100000
#define SOFTWARE_DRIVER_2_TEXTURE_TRANSFORM
#define SOFTWARE_DRIVER_2_MIPMAPPING_MAX		16
#define SOFTWARE_DRIVER_2_MIPMAPPING_MIN_SIZE   1
#define SOFTWARE_DRIVER_2_CLIPPING
#define SOFTWARE_DRIVER_2_2D_AS_3D
#define SOFTWARE_DRIVER_2_INTERLACED
#define SOFTWARE_DRIVER_2_RENDERTARGET_SCALE

#endif

#ifdef BURNINGVIDEO_RENDERER_BEAUTIFUL
#define SOFTWARE_DRIVER_2_PERSPECTIVE_CORRECT
#define SOFTWARE_DRIVER_2_SUBTEXEL
#define SOFTWARE_DRIVER_2_BILINEAR
#define SOFTWARE_DRIVER_2_LIGHTING
#define SOFTWARE_DRIVER_2_USE_VERTEX_COLOR
#define SOFTWARE_DRIVER_2_USE_SEPARATE_SPECULAR_COLOR
#define SOFTWARE_DRIVER_2_USE_WBUFFER
#define SOFTWARE_DRIVER_2_32BIT
#define	SOFTWARE_DRIVER_2_TEXTURE_COLOR_FORMAT	ECF_A8R8G8B8
#define	SOFTWARE_DRIVER_2_RENDERTARGET_COLOR_FORMAT	ECF_A8R8G8B8
#define SOFTWARE_DRIVER_2_TEXTURE_MAXSIZE		0x100000
#define SOFTWARE_DRIVER_2_TEXTURE_TRANSFORM
#define SOFTWARE_DRIVER_2_MIPMAPPING_MAX		16
#define SOFTWARE_DRIVER_2_MIPMAPPING_MIN_SIZE   1
#define SOFTWARE_DRIVER_2_CLIPPING
#define SOFTWARE_DRIVER_2_2D_AS_3D
#define SOFTWARE_DRIVER_2_INTERLACED
#define SOFTWARE_DRIVER_2_RENDERTARGET_SCALE
#endif

//! Set Flags for Windows Mobile
#ifdef BURNINGVIDEO_RENDERER_CE
#define SOFTWARE_DRIVER_2_PERSPECTIVE_CORRECT
#define SOFTWARE_DRIVER_2_SUBTEXEL
//#define SOFTWARE_DRIVER_2_BILINEAR
//#define SOFTWARE_DRIVER_2_LIGHTING
#define SOFTWARE_DRIVER_2_USE_VERTEX_COLOR
#define SOFTWARE_DRIVER_2_USE_WBUFFER
#define SOFTWARE_DRIVER_2_16BIT
#define	SOFTWARE_DRIVER_2_TEXTURE_COLOR_FORMAT	ECF_A1R5G5B5
#define	SOFTWARE_DRIVER_2_RENDERTARGET_COLOR_FORMAT	ECF_A1R5G5B5
#define SOFTWARE_DRIVER_2_TEXTURE_MAXSIZE		64
//#define SOFTWARE_DRIVER_2_TEXTURE_TRANSFORM
#define SOFTWARE_DRIVER_2_MIPMAPPING_MAX		4
#define SOFTWARE_DRIVER_2_MIPMAPPING_MIN_SIZE   8
//#define SOFTWARE_DRIVER_2_CLIPPING
#define SOFTWARE_DRIVER_2_2D_AS_2D
#endif

#ifdef BURNINGVIDEO_RENDERER_FAST
#define SOFTWARE_DRIVER_2_PERSPECTIVE_CORRECT
#define SOFTWARE_DRIVER_2_SUBTEXEL
//#define SOFTWARE_DRIVER_2_BILINEAR
//#define SOFTWARE_DRIVER_2_LIGHTING
#define SOFTWARE_DRIVER_2_USE_VERTEX_COLOR
#define SOFTWARE_DRIVER_2_USE_WBUFFER
#define SOFTWARE_DRIVER_2_16BIT
#define SOFTWARE_DRIVER_2_TEXTURE_MAXSIZE		256
#define	SOFTWARE_DRIVER_2_TEXTURE_COLOR_FORMAT	ECF_A1R5G5B5
#define	SOFTWARE_DRIVER_2_RENDERTARGET_COLOR_FORMAT	ECF_A1R5G5B5
#define SOFTWARE_DRIVER_2_MIPMAPPING_MAX		4
#define SOFTWARE_DRIVER_2_MIPMAPPING_MIN_SIZE   8
#define SOFTWARE_DRIVER_2_CLIPPING
#define SOFTWARE_DRIVER_2_2D_AS_2D
#define SOFTWARE_DRIVER_2_INTERLACED
#endif

#ifdef BURNINGVIDEO_RENDERER_ULTRA_FAST
#define BURNINGVIDEO_RENDERER_FAST

//#define SOFTWARE_DRIVER_2_PERSPECTIVE_CORRECT
//#define SOFTWARE_DRIVER_2_SUBTEXEL
//#define SOFTWARE_DRIVER_2_BILINEAR
//#define SOFTWARE_DRIVER_2_LIGHTING
#define SOFTWARE_DRIVER_2_USE_VERTEX_COLOR
//#define SOFTWARE_DRIVER_2_USE_WBUFFER
#define SOFTWARE_DRIVER_2_16BIT
#define SOFTWARE_DRIVER_2_TEXTURE_MAXSIZE		256
#define	SOFTWARE_DRIVER_2_TEXTURE_COLOR_FORMAT	ECF_A1R5G5B5
#define	SOFTWARE_DRIVER_2_RENDERTARGET_COLOR_FORMAT	ECF_A1R5G5B5
//#define SOFTWARE_DRIVER_2_TEXTURE_TRANSFORM
#define SOFTWARE_DRIVER_2_MIPMAPPING_MAX		1
#define SOFTWARE_DRIVER_2_MIPMAPPING_MIN_SIZE   1
//#define SOFTWARE_DRIVER_2_CLIPPING
#define SOFTWARE_DRIVER_2_2D_AS_2D
#define SOFTWARE_DRIVER_2_INTERLACED
#endif


#ifndef REALINLINE
#ifdef _MSC_VER
#define REALINLINE __forceinline
#else
#define REALINLINE inline
#endif
#endif

#define reciprocal_zero(x) ((x) != 0.f ? 1.f / (x):0.f)
#define reciprocal_zero_pos_underflow(x) ((x) != 0.f ? 1.f / (x):0.f)
#define reciprocal_one(x) ((x) != 0.f ? 1.f / (x):1.f)

//Control Scanline output
#define SOFTWARE_DRIVER_2_STEP_X 1
#define SOFTWARE_DRIVER_2_STEP_Y 1

// null check necessary (burningvideo only)
#define fill_step_y(y) ((y) != 0.f ? (float)1.f / (y):0.f)
static inline float fill_step_x(float x) { return x != 0.f ? (float)SOFTWARE_DRIVER_2_STEP_X / x : 0.f; }

#define interlace_control_bit 1
#define interlace_control_mask ((1<<interlace_control_bit)-1)
union interlaced_control
{
	struct {
		unsigned enable : 1;
		unsigned bypass : 1;
		unsigned nr : interlace_control_bit;

		unsigned target_scalex : 2; // 0 means 1
		unsigned target_scaley : 2;

		unsigned tex_scalex : 2; // 0 means 1
		unsigned tex_scaley : 2;
	};
	unsigned raw;
};
struct interlace_scanline_data { unsigned int y; };

static inline interlaced_control interlaced_disabled()
{
	interlaced_control v;
	v.raw = 0;
	v.bypass = 1;
/*
	v.enable = 0;
	v.bypass = 1;
	v.nr = 0;
	v.target_scalex = 0;
	v.target_scaley = 0;
	v.tex_scalex = 0;
	v.tex_scaley = 0;
*/
	return v;
}
#if defined(SOFTWARE_DRIVER_2_INTERLACED)
#define interlace_scanline_active ((line.y & interlace_control_mask) == Interlaced.nr)
#define if_interlace_scanline_active if (interlace_scanline_active)
#define if_interlace_scanline if ( Interlaced.bypass || interlace_scanline_active )
#else
#define if_interlace_scanline_active
#define if_interlace_scanline
#endif

#define if_scissor_test_y if ((~TL_Flag & TL_SCISSOR) || ((line.y >= Scissor.y0) & (line.y <= Scissor.y1)))
#define if_scissor_test_x if ((~TL_Flag & TL_SCISSOR) || ((i+xStart >= Scissor.x0) & (i+xStart <= Scissor.x1)))

// https://inst.eecs.berkeley.edu/~cs184/sp04/as/as2/assgn-02_faqs.html
//#define fill_convention_top_left(x) (s32) ceilf(x)
//#define fill_convention_right(x) (s32) floorf(x)
//#define fill_convention_right(x) (((s32) ceilf(x))-1)

// http://www.chrishecker.com/images/9/97/Gdmtex2.pdf
#define fill_convention_top(y) (s32) ceilf(y)
#define fill_convention_down(y) (((s32) ceilf(y))-1)
#define fill_convention_left(x) (s32) ceilf(x)
#define fill_convention_right(x) (((s32) ceilf(x))-1)


#define fill_convention_none(x) (s32) (x)
#define fill_convention_edge(x) (s32) floorf(fabsf(x))

//fixpoint
//#define fill_convention_top_left(x) 65536 - int(65536.0f - x)
//#define fill_convention_right(x) 65535 - int(65536.0f - x)


//Check that coordinates are in render target/window space
//#define SOFTWARE_DRIVER_2_DO_CLIPCHECK
#if defined (SOFTWARE_DRIVER_2_DO_CLIPCHECK) && defined(_WIN32)
#define SOFTWARE_DRIVER_2_CLIPCHECK      if( xStart < 0 || xStart + dx >= (s32)RenderTarget->getDimension().Width || line.y < 0 || line.y >= (s32) RenderTarget->getDimension().Height ) __debugbreak()
#define SOFTWARE_DRIVER_2_CLIPCHECK_REF  if( pShader.xStart < 0 || pShader.xStart + pShader.dx >= (s32)RenderTarget->getDimension().Width || line.y < 0 || line.y >= (s32) RenderTarget->getDimension().Height ) __debugbreak()
#define SOFTWARE_DRIVER_2_CLIPCHECK_WIRE if( aposx < 0 || aposx >= (s32)RenderTarget->getDimension().Width || aposy < 0 || aposy >= (s32) RenderTarget->getDimension().Height ) __debugbreak()

inline float reciprocal_zero_no(const float x)
{
	if (x * x <= 0.00001f) __debugbreak();
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
#if defined(SOFTWARE_DRIVER_2_32BIT)
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
#define	ipol_lower_0(n)			((n) <  (ipoltype)0.0)
#define	ipol_greater_0(n)		((n) >  (ipoltype)0.0)

#if	(_MSC_VER > 1700 )
#define burning_restrict __restrict
#else
#define burning_restrict
#endif

#if 0 && defined(_MSC_VER) && (_MSC_VER > 1500) && !defined(_DEBUG)
#pragma auto_inline(on)
#pragma inline_depth(255)
#pragma inline_recursion(on)
#endif


#define burning_stringify(s) #s
#define burning_create_indirect(s) create_##s
#define burning_create(s) burning_create_indirect(s)

// don't want intend on namespaces (autoformat)
#define burning_namespace_start namespace irr { namespace video {
#define burning_namespace_end } /* end namespace video*/ } /* end namespace irr */

#if defined(PATCH_SUPERTUX_8_0_1_with_1_9_0)

#if defined(_MSC_VER) && _MSC_VER > 1310 && !defined (_WIN32_WCE)
#define snprintf_irr sprintf_s
#elif defined(__APPLE__)
#define snprintf_irr snprintf
#elif !defined(__CYGWIN__)
#define swprintf_irr _snwprintf
#define snprintf_irr _snprintf
#endif


//#define EVDF_DEPTH_CLAMP (video::E_VIDEO_DRIVER_FEATURE) 43
#define E_CUBE_SURFACE int
#define ECFN_DISABLED 0

namespace irr {
	namespace video {

		//! Enum for the flags of clear buffer
		enum E_CLEAR_BUFFER_FLAG
		{
			ECBF_NONE = 0,
			ECBF_COLOR = 1,
			ECBF_DEPTH = 2,
			ECBF_STENCIL = 4,
			ECBF_ALL = ECBF_COLOR | ECBF_DEPTH | ECBF_STENCIL
		};

		//! For SMaterial.ZWriteEnable
		enum E_ZWRITE
		{
			EZW_OFF = 0,
			EZW_AUTO,
			EZW_ON
		};
	}
}
#endif // PATCH_SUPERTUX_8_0_1_with_1_9_0

//! Size of a static C-style array.
#define array_size(_arr)  ((sizeof(_arr)/sizeof(*_arr)))

//! Compiler Align
#if defined(_MSC_VER)
#if defined(ENV64BIT)
#define ALIGN(x) __declspec(align(x))
#else
// ALIGN(16) not working
#define ALIGN(x) __declspec(align(8))
#endif
#elif defined(__GNUC__)
#define ALIGN(x) __attribute__ ((aligned(x)))
#else
#define ALIGN(x)
#endif

#endif // S_VIDEO_2_SOFTWARE_COMPILE_CONFIG_H_INCLUDED
