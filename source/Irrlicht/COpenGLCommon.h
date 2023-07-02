// Copyright (C) 2015 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#ifdef _IRR_COMPILE_WITH_OPENGL_

#if defined(_IRR_WINDOWS_API_)
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#define GL_GLEXT_LEGACY 1
	#include <GL/gl.h>
	#include <GL/glext.h>
	#include <GL/wglext.h>
#elif defined(_IRR_COMPILE_WITH_OSX_DEVICE_)
	#include <OpenGL/gl.h>
#elif defined(_IRR_COMPILE_WITH_SDL_DEVICE_) && !defined(_IRR_COMPILE_WITH_X11_DEVICE_)
	#define GL_GLEXT_LEGACY 1
	#include <SDL_video.h>
	#include <SDL_opengl.h>
	#ifdef __APPLE__
		#include <OpenGL/glext.h>
	#else
		#include <GL/glext.h>
	#endif
#else
	#define GL_GLEXT_LEGACY 1
	#define GLX_GLXEXT_LEGACY 1
	#include <GL/gl.h>
	#include <GL/glx.h>
	#include <GL/glext.h>
	#include <GL/glxext.h>
#endif

#ifndef GL_ARB_shader_objects
typedef char GLcharARB;
typedef unsigned int GLhandleARB;
#endif

#ifndef GL_VERSION_2_0
typedef char GLchar;
#endif

// Blending definitions.

#if !defined(GL_VERSION_1_4)
#if defined(GL_EXT_blend_subtract) || defined(GL_EXT_blend_minmax) || defined(GL_EXT_blend_logic_op)
#define GL_FUNC_ADD GL_FUNC_ADD_EXT
#else
#define GL_FUNC_ADD 0
#endif
#endif

// FBO definitions.

#if !defined(GL_VERSION_3_0) && !defined(GL_ARB_framebuffer_object)
#ifdef GL_EXT_framebuffer_object
#define GL_FRAMEBUFFER GL_FRAMEBUFFER_EXT
#define GL_COLOR_ATTACHMENT0 GL_COLOR_ATTACHMENT0_EXT
#define GL_DEPTH_ATTACHMENT GL_DEPTH_ATTACHMENT_EXT
#define GL_STENCIL_ATTACHMENT GL_STENCIL_ATTACHMENT_EXT
#define GL_FRAMEBUFFER_COMPLETE GL_FRAMEBUFFER_COMPLETE_EXT
#define GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT
#define GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT
#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT
#define GL_FRAMEBUFFER_INCOMPLETE_FORMATS GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT
#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT
#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT
#define GL_FRAMEBUFFER_UNSUPPORTED GL_FRAMEBUFFER_UNSUPPORTED_EXT
#else
#define GL_FRAMEBUFFER 0
#define GL_COLOR_ATTACHMENT0 0
#define GL_DEPTH_ATTACHMENT 0
#define GL_STENCIL_ATTACHMENT 0
#define GL_FRAMEBUFFER_COMPLETE 0
#define GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER 1
#define GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER 2
#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT 3
#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT 4
#define GL_FRAMEBUFFER_UNSUPPORTED 5
#endif
#endif

#ifdef GL_EXT_framebuffer_object
#define GL_FRAMEBUFFER_INCOMPLETE_FORMATS GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT
#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT
#else
#define GL_FRAMEBUFFER_INCOMPLETE_FORMATS 6
#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS 7
#endif

// MipMaps definitions.

#if !defined(GL_VERSION_1_4)
#ifdef GL_SGIS_generate_mipmap
#define GL_GENERATE_MIPMAP GL_GENERATE_MIPMAP_SGIS
#define GL_GENERATE_MIPMAP_HINT GL_GENERATE_MIPMAP_HINT_SGIS
#else
#define GL_GENERATE_MIPMAP 0
#define GL_GENERATE_MIPMAP_HINT 0
#endif
#endif

// Texture definitions.

#if !defined(GL_VERSION_1_3)
#ifdef GL_ARB_multitexture
#define GL_TEXTURE0 GL_TEXTURE0_ARB
#else
#define GL_TEXTURE0 0
#endif
#endif

#if !defined(GL_VERSION_1_3)
#ifdef GL_ARB_texture_cube_map
#define GL_TEXTURE_CUBE_MAP GL_TEXTURE_CUBE_MAP_ARB
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB
#elif defined(GL_EXT_texture_cube_map)
#define GL_TEXTURE_CUBE_MAP GL_TEXTURE_CUBE_MAP_EXT
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT
#else
#define GL_TEXTURE_CUBE_MAP 0
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X 0
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X 0
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y 0
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y 0
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z 0
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z 0
#endif
#endif

// To check if this header is in the current compile unit (different GL driver implementations use different "GLCommon" headers in Irrlicht)
#define IRR_COMPILE_GL_COMMON

namespace irr
{
namespace video
{

	// Forward declarations.

	class COpenGLCoreFeature;

	template <class TOpenGLDriver>
	class COpenGLCoreTexture;

	template <class TOpenGLDriver, class TOpenGLTexture>
	class COpenGLCoreRenderTarget;

	template <class TOpenGLDriver, class TOpenGLTexture>
	class COpenGLCoreCacheHandler;

	class COpenGLDriver;
	typedef COpenGLCoreTexture<COpenGLDriver> COpenGLTexture;
	typedef COpenGLCoreRenderTarget<COpenGLDriver, COpenGLTexture> COpenGLRenderTarget;
	class COpenGLCacheHandler;

}
}

#endif
