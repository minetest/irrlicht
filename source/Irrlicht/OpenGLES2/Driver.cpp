// Copyright (C) 2023 Vitaliy Lobachevskiy
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "Driver.h"
#include <cassert>
#include <CColorConverter.h>

namespace irr {
namespace video {

	E_DRIVER_TYPE COpenGLES2Driver::getDriverType() const {
		return EDT_OGLES2;
	}

	OpenGLVersion COpenGLES2Driver::getVersionFromOpenGL() const {
		auto version_string = reinterpret_cast<const char *>(GL.GetString(GL.VERSION));
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

		if (Version.Major >= 3) {
			// NOTE floating-point formats may not be suitable for render targets.
			TextureFormats[ECF_A1R5G5B5] = {GL.RGB5_A1, GL.RGBA, GL.UNSIGNED_SHORT_5_5_5_1, CColorConverter::convert_A1R5G5B5toR5G5B5A1};
			TextureFormats[ECF_R5G6B5] = {GL.RGB565, GL.RGB, GL.UNSIGNED_SHORT_5_6_5};
			TextureFormats[ECF_R8G8B8] = {GL.RGB8, GL.RGB, GL.UNSIGNED_BYTE};
			TextureFormats[ECF_A8R8G8B8] = {GL.RGBA8, GL.RGBA, GL.UNSIGNED_BYTE, CColorConverter::convert_A8R8G8B8toA8B8G8R8};
			TextureFormats[ECF_R16F] = {GL.R16F, GL.RED, GL.HALF_FLOAT};
			TextureFormats[ECF_G16R16F] = {GL.RG16F, GL.RG, GL.HALF_FLOAT};
			TextureFormats[ECF_A16B16G16R16F] = {GL.RGBA16F, GL.RGBA, GL.HALF_FLOAT};
			TextureFormats[ECF_R32F] = {GL.R32F, GL.RED, GL.FLOAT};
			TextureFormats[ECF_G32R32F] = {GL.RG32F, GL.RG, GL.FLOAT};
			TextureFormats[ECF_A32B32G32R32F] = {GL.RGBA32F, GL.RGBA, GL.FLOAT};
			TextureFormats[ECF_R8] = {GL.R8, GL.RED, GL.UNSIGNED_BYTE};
			TextureFormats[ECF_R8G8] = {GL.RG8, GL.RG, GL.UNSIGNED_BYTE};
			TextureFormats[ECF_D16] = {GL.DEPTH_COMPONENT16, GL.DEPTH_COMPONENT, GL.UNSIGNED_SHORT};
			TextureFormats[ECF_D24S8] = {GL.DEPTH24_STENCIL8, GL.DEPTH_STENCIL, GL.UNSIGNED_INT_24_8};

			if (FeatureAvailable[IRR_GL_EXT_texture_format_BGRA8888])
				TextureFormats[ECF_A8R8G8B8] = {GL.RGBA, GL.BGRA, GL.UNSIGNED_BYTE};
			else if (FeatureAvailable[IRR_GL_APPLE_texture_format_BGRA8888])
				TextureFormats[ECF_A8R8G8B8] = {GL.BGRA, GL.BGRA, GL.UNSIGNED_BYTE};

			if (FeatureAvailable[IRR_GL_OES_depth32])
				TextureFormats[ECF_D32] = {GL.DEPTH_COMPONENT32, GL.DEPTH_COMPONENT, GL.UNSIGNED_INT};
		} else {
			// NOTE These are *texture* formats. They may or may not be suitable
			// for render targets. The specs only talks on *sized* formats for the
			// latter but forbids creating textures with sized internal formats,
			// reserving them for renderbuffers.

			static const unsigned int HALF_FLOAT_OES = 0x8D61; // not equal to GL_HALF_FLOAT
			TextureFormats[ECF_A1R5G5B5] = {GL.RGBA, GL.RGBA, GL.UNSIGNED_SHORT_5_5_5_1, CColorConverter::convert_A1R5G5B5toR5G5B5A1};
			TextureFormats[ECF_R5G6B5] = {GL.RGB, GL.RGB, GL.UNSIGNED_SHORT_5_6_5};
			TextureFormats[ECF_R8G8B8] = {GL.RGB, GL.RGB, GL.UNSIGNED_BYTE};
			TextureFormats[ECF_A8R8G8B8] = {GL.RGBA, GL.RGBA, GL.UNSIGNED_BYTE, CColorConverter::convert_A8R8G8B8toA8B8G8R8};

			if (FeatureAvailable[IRR_GL_EXT_texture_format_BGRA8888])
				TextureFormats[ECF_A8R8G8B8] = {GL.RGBA, GL.BGRA, GL.UNSIGNED_BYTE};
			else if (FeatureAvailable[IRR_GL_APPLE_texture_format_BGRA8888])
				TextureFormats[ECF_A8R8G8B8] = {GL.BGRA, GL.BGRA, GL.UNSIGNED_BYTE};

			if (FeatureAvailable[IRR_GL_OES_texture_half_float]) {
				TextureFormats[ECF_A16B16G16R16F] = {GL.RGBA, GL.RGBA, HALF_FLOAT_OES};
			}
			if (FeatureAvailable[IRR_GL_OES_texture_float]) {
				TextureFormats[ECF_A32B32G32R32F] = {GL.RGBA, GL.RGBA, GL.FLOAT};
			}
			if (FeatureAvailable[IRR_GL_EXT_texture_rg]) {
				TextureFormats[ECF_R8] = {GL.RED, GL.RED, GL.UNSIGNED_BYTE};
				TextureFormats[ECF_R8G8] = {GL.RG, GL.RG, GL.UNSIGNED_BYTE};

				if (FeatureAvailable[IRR_GL_OES_texture_half_float]) {
					TextureFormats[ECF_R16F] = {GL.RED, GL.RED, HALF_FLOAT_OES};
					TextureFormats[ECF_G16R16F] = {GL.RG, GL.RG, HALF_FLOAT_OES};
				}
				if (FeatureAvailable[IRR_GL_OES_texture_float]) {
					TextureFormats[ECF_R32F] = {GL.RED, GL.RED, GL.FLOAT};
					TextureFormats[ECF_G32R32F] = {GL.RG, GL.RG, GL.FLOAT};
				}
			}

			if (FeatureAvailable[IRR_GL_OES_depth_texture]) {
				TextureFormats[ECF_D16] = {GL.DEPTH_COMPONENT, GL.DEPTH_COMPONENT, GL.UNSIGNED_SHORT};
				if (FeatureAvailable[IRR_GL_OES_depth32])
					TextureFormats[ECF_D32] = {GL.DEPTH_COMPONENT, GL.DEPTH_COMPONENT, GL.UNSIGNED_INT};
				if (FeatureAvailable[IRR_GL_OES_packed_depth_stencil])
					TextureFormats[ECF_D24S8] = {GL.DEPTH_STENCIL, GL.DEPTH_STENCIL, GL.UNSIGNED_INT_24_8};
			}
		}

		const bool MRTSupported = Version.Major >= 3 || queryExtension("GL_EXT_draw_buffers");
		AnisotropicFilterSupported = queryExtension("GL_EXT_texture_filter_anisotropic");
		BlendMinMaxSupported = (Version.Major >= 3) || FeatureAvailable[IRR_GL_EXT_blend_minmax];
		const bool TextureLODBiasSupported = queryExtension("GL_EXT_texture_lod_bias");

		// COGLESCoreExtensionHandler::Feature
		static_assert(MATERIAL_MAX_TEXTURES <= 8, "Only up to 8 textures are guaranteed");
		Feature.BlendOperation = true;
		Feature.ColorAttachment = 1;
		if (MRTSupported)
			Feature.ColorAttachment = GetInteger(GL.MAX_COLOR_ATTACHMENTS);
		Feature.MaxTextureUnits = MATERIAL_MAX_TEXTURES;
		if (MRTSupported)
			Feature.MultipleRenderTarget = GetInteger(GL.MAX_DRAW_BUFFERS);

		// COGLESCoreExtensionHandler
		// TODO: Replace this with something in mt_opengl.h
		//if (AnisotropicFilterSupported)
			//MaxAnisotropy = GetInteger(GL.MAX_TEXTURE_MAX_ANISOTROPY_EXT);
		if (Version.Major >= 3 || queryExtension("GL_EXT_draw_range_elements"))
			MaxIndices = GetInteger(GL.MAX_ELEMENTS_INDICES);
		MaxTextureSize = GetInteger(GL.MAX_TEXTURE_SIZE);
		if (TextureLODBiasSupported)
			GL.GetFloatv(GL.MAX_TEXTURE_LOD_BIAS, &MaxTextureLODBias);
		GL.GetFloatv(GL.ALIASED_LINE_WIDTH_RANGE, DimAliasedLine); // NOTE: this is not in the OpenGL ES 2.0 spec...
		// TODO: Replace this with something in mt_opengl.h
		//GL.GetFloatv(GL.ALIASED_POINT_SIZE_RANGE, DimAliasedPoint);
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
