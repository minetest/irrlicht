// Copyright (C) 2023 Vitaliy Lobachevskiy
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "Driver.h"
#include <cassert>

namespace irr {
namespace video {

	E_DRIVER_TYPE COpenGLES2Driver::getDriverType() const {
		return EDT_OGLES2;
	}

	OpenGLVersion COpenGLES2Driver::getVersionFromOpenGL() const {
		auto version_string = reinterpret_cast<const char *>(glGetString(GL_VERSION));
		int major, minor;
		if (sscanf(version_string, "OpenGL ES %d.%d", &major, &minor) != 2) {
			os::Printer::log("Failed to parse OpenGL ES version string", version_string, ELL_ERROR);
			return {OpenGLSpec::ES, 0, 0, 0};
		}
		return {OpenGLSpec::ES, (u8)major, (u8)minor, 0};
	}

	void COpenGLES2Driver::initFeatures() {
		assert (Version.Spec == OpenGLSpec::ES);
		assert (Version.Major >= 2);
		if (Version.Major >= 3)
			initExtensionsNew();
		else
			initExtensionsOld();

		// COGLESCoreExtensionHandler::Feature
		static_assert(MATERIAL_MAX_TEXTURES <= 8, "Only up to 8 textures are guaranteed");
		Feature.BlendOperation = true;
		Feature.ColorAttachment = 1;
		if (Version.Major >= 3 || FeatureAvailable[IRR_GL_EXT_draw_buffers])
			Feature.ColorAttachment = GetInteger(GL_MAX_COLOR_ATTACHMENTS);
		Feature.MaxTextureUnits = MATERIAL_MAX_TEXTURES;
		if (Version.Major >= 3 || FeatureAvailable[IRR_GL_EXT_draw_buffers])
			Feature.MultipleRenderTarget = GetInteger(GL_MAX_DRAW_BUFFERS);

		// COGLESCoreExtensionHandler
		if (FeatureAvailable[IRR_GL_EXT_texture_filter_anisotropic])
			MaxAnisotropy = GetInteger(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT);
		if (Version.Major >= 3 || FeatureAvailable[IRR_GL_EXT_draw_range_elements])
			MaxIndices = GetInteger(GL_MAX_ELEMENTS_INDICES);
		MaxTextureSize = GetInteger(GL_MAX_TEXTURE_SIZE);
		if (FeatureAvailable[IRR_GL_EXT_texture_lod_bias])
			glGetFloatv(GL_MAX_TEXTURE_LOD_BIAS, &MaxTextureLODBias);
		glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, DimAliasedLine); // NOTE: this is not in the OpenGL ES 2.0 spec...
		glGetFloatv(GL_ALIASED_POINT_SIZE_RANGE, DimAliasedPoint);
	}

	IVideoDriver* createOGLES2Driver(const SIrrlichtCreationParameters& params, io::IFileSystem* io, IContextManager* contextManager)
	{
		os::Printer::log("Using COpenGLES2Driver", ELL_INFORMATION);
		COpenGLES2Driver* driver = new COpenGLES2Driver(params, io, contextManager);
		driver->genericDriverInit(params.WindowSize, params.Stencilbuffer);	// don't call in constructor, it uses virtual function calls of driver
		return driver;
	}

}
}
