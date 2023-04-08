// Copyright (C) 2023 Vitaliy Lobachevskiy
// Copyright (C) 2015 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#define GL_GLEXT_PROTOTYPES
#include <SDL_video.h>
#include <SDL_opengl.h>

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

	class COpenGL3DriverBase;
	class COpenGL3Texture;
	typedef COpenGLCoreRenderTarget<COpenGL3DriverBase, COpenGL3Texture> COpenGL3RenderTarget;
	typedef COpenGLCoreCacheHandler<COpenGL3DriverBase, COpenGL3Texture> COpenGL3CacheHandler;

}
}
