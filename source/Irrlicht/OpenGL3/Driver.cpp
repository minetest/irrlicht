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
		int major, minor, profile;
		GL.GetIntegerv(GL.MAJOR_VERSION, &major);
		GL.GetIntegerv(GL.MINOR_VERSION, &minor);
		GL.GetIntegerv(GL.CONTEXT_PROFILE_MASK, &profile);
		// The spec is clear a context can’t be both core and compatibility at the same time.
		// However, the returned value is a mask. Ask Khronos why. -- numzero
		if (profile & GL.CONTEXT_COMPATIBILITY_PROFILE_BIT)
			return {OpenGLSpec::Compat, (u8)major, (u8)minor, 0};
		if (profile & GL.CONTEXT_CORE_PROFILE_BIT)
			return {OpenGLSpec::Core, (u8)major, (u8)minor, 0};
		os::Printer::log("Got unrecognized OpenGL profile", ELL_ERROR);
		return {OpenGLSpec::Core, (u8)major, (u8)minor, 0};
	}

	void COpenGL3Driver::initFeatures() {
		assert (Version.Spec == OpenGLSpec::Core);
		assert (isVersionAtLeast(3, 3));
		initExtensionsNew();

		TextureFormats[ECF_A1R5G5B5] = {GL.RGB5_A1, GL.BGRA, GL.UNSIGNED_SHORT_1_5_5_5_REV}; // WARNING: may not be renderable
		TextureFormats[ECF_R5G6B5] = {GL.RGB, GL.RGB, GL.UNSIGNED_SHORT_5_6_5}; // GL_RGB565 is an extension until 4.1
		TextureFormats[ECF_R8G8B8] = {GL.RGB8, GL.RGB, GL.UNSIGNED_BYTE}; // WARNING: may not be renderable
		TextureFormats[ECF_A8R8G8B8] = {GL.RGBA8, GL.BGRA, GL.UNSIGNED_INT_8_8_8_8_REV};
		TextureFormats[ECF_R16F] = {GL.R16F, GL.RED, GL.HALF_FLOAT};
		TextureFormats[ECF_G16R16F] = {GL.RG16F, GL.RG, GL.HALF_FLOAT};
		TextureFormats[ECF_A16B16G16R16F] = {GL.RGBA16F, GL.RGBA, GL.HALF_FLOAT};
		TextureFormats[ECF_R32F] = {GL.R32F, GL.RED, GL.FLOAT};
		TextureFormats[ECF_G32R32F] = {GL.RG32F, GL.RG, GL.FLOAT};
		TextureFormats[ECF_A32B32G32R32F] = {GL.RGBA32F, GL.RGBA, GL.FLOAT};
		TextureFormats[ECF_R8] = {GL.R8, GL.RED, GL.UNSIGNED_BYTE};
		TextureFormats[ECF_R8G8] = {GL.RG8, GL.RG, GL.UNSIGNED_BYTE};
		TextureFormats[ECF_R16] = {GL.R16, GL.RED, GL.UNSIGNED_SHORT};
		TextureFormats[ECF_R16G16] = {GL.RG16, GL.RG, GL.UNSIGNED_SHORT};
		TextureFormats[ECF_D16] = {GL.DEPTH_COMPONENT16, GL.DEPTH_COMPONENT, GL.UNSIGNED_SHORT};
		TextureFormats[ECF_D32] = {GL.DEPTH_COMPONENT32, GL.DEPTH_COMPONENT, GL.UNSIGNED_INT}; // WARNING: may not be renderable (?!)
		TextureFormats[ECF_D24S8] = {GL.DEPTH24_STENCIL8, GL.DEPTH_STENCIL, GL.UNSIGNED_INT_24_8};

		AnisotropicFilterSupported = isVersionAtLeast(4, 6) || queryExtension("GL_ARB_texture_filter_anisotropic") || queryExtension("GL_EXT_texture_filter_anisotropic");
		BlendMinMaxSupported = true;

		// COGLESCoreExtensionHandler::Feature
		static_assert(MATERIAL_MAX_TEXTURES <= 16, "Only up to 16 textures are guaranteed");
		Feature.BlendOperation = true;
		Feature.ColorAttachment = GetInteger(GL.MAX_COLOR_ATTACHMENTS);
		Feature.MaxTextureUnits = MATERIAL_MAX_TEXTURES;
		Feature.MultipleRenderTarget = GetInteger(GL.MAX_DRAW_BUFFERS);

		// COGLESCoreExtensionHandler
		if (AnisotropicFilterSupported)
			MaxAnisotropy = GetInteger(GL.MAX_TEXTURE_MAX_ANISOTROPY);
		MaxIndices = GetInteger(GL.MAX_ELEMENTS_INDICES);
		MaxTextureSize = GetInteger(GL.MAX_TEXTURE_SIZE);
		GL.GetFloatv(GL.MAX_TEXTURE_LOD_BIAS, &MaxTextureLODBias);
		GL.GetFloatv(GL.ALIASED_LINE_WIDTH_RANGE, DimAliasedLine);
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
