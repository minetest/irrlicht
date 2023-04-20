// Copyright (C) 2023 Vitaliy Lobachevskiy
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "Driver.h"
#include <cassert>
#include "mt_opengl.h"

namespace irr {
namespace video {

	E_DRIVER_TYPE COpenGL3Driver::getDriverType() const {
		return EDT_OPENGL3;
	}

	OpenGLVersion COpenGL3Driver::getVersionFromOpenGL() const {
		GLint major, minor, profile;
		glGetIntegerv(GL_MAJOR_VERSION, &major);
		glGetIntegerv(GL_MINOR_VERSION, &minor);
		glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profile);
		// The spec is clear a context can’t be both core and compatibility at the same time.
		// However, the returned value is a mask. Ask Khronos why. -- numzero
		if (profile & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT)
			return {OpenGLSpec::Compat, (u8)major, (u8)minor, 0};
		if (profile & GL_CONTEXT_CORE_PROFILE_BIT)
			return {OpenGLSpec::Core, (u8)major, (u8)minor, 0};
		os::Printer::log("Got unrecognized OpenGL profile", ELL_ERROR);
		return {OpenGLSpec::Core, (u8)major, (u8)minor, 0};
	}

	void COpenGL3Driver::initFeatures() {
		assert (Version.Spec == OpenGLSpec::Compat);
		assert (isVersionAtLeast(3, 2));
		initExtensionsNew();

		AnisotropicFilterSupported = isVersionAtLeast(4, 6) || queryExtension("GL_ARB_texture_filter_anisotropic") || queryExtension("GL_EXT_texture_filter_anisotropic");
		BlendMinMaxSupported = true;

		// COGLESCoreExtensionHandler::Feature
		static_assert(MATERIAL_MAX_TEXTURES <= 16, "Only up to 16 textures are guaranteed");
		Feature.BlendOperation = true;
		Feature.ColorAttachment = GetInteger(GL_MAX_COLOR_ATTACHMENTS);
		Feature.MaxTextureUnits = MATERIAL_MAX_TEXTURES;
		Feature.MultipleRenderTarget = GetInteger(GL_MAX_DRAW_BUFFERS);

		// COGLESCoreExtensionHandler
		if (AnisotropicFilterSupported)
			MaxAnisotropy = GetInteger(GL.MAX_TEXTURE_MAX_ANISOTROPY);
		MaxIndices = GetInteger(GL_MAX_ELEMENTS_INDICES);
		MaxTextureSize = GetInteger(GL_MAX_TEXTURE_SIZE);
		glGetFloatv(GL_MAX_TEXTURE_LOD_BIAS, &MaxTextureLODBias);
		glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, DimAliasedLine);
		DimAliasedPoint[0] = 1.0f;
		DimAliasedPoint[1] = 1.0f;
	}

	IVideoDriver* createOpenGL3Driver(const SIrrlichtCreationParameters& params, io::IFileSystem* io, IContextManager* contextManager)
	{
		os::Printer::log("Using COpenGL3Driver", ELL_INFORMATION);
		COpenGL3Driver* driver = new COpenGL3Driver(params, io, contextManager);
		driver->genericDriverInit(params.WindowSize, params.Stencilbuffer);	// don't call in constructor, it uses virtual function calls of driver
		return driver;
	}

}
}
