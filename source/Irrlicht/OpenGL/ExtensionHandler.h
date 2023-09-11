// Copyright (C) 2023 Vitaliy Lobachevskiy
// Copyright (C) 2015 Patryk Nadrowski
// Copyright (C) 2009-2010 Amundis
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#pragma once

#include <unordered_set>

#include "EDriverFeatures.h"
#include "irrTypes.h"
#include "os.h"
#include <mt_opengl.h>

#include "Common.h"

#include "COGLESCoreExtensionHandler.h"

namespace irr
{
namespace video
{

	class COpenGL3ExtensionHandler : public COGLESCoreExtensionHandler
	{
	public:
		COpenGL3ExtensionHandler() : COGLESCoreExtensionHandler() {}

		void initExtensionsOld();
		void initExtensionsNew();

		/// Checks whether a named extension is present
		bool queryExtension(const std::string &name) const noexcept;

		bool queryFeature(video::E_VIDEO_DRIVER_FEATURE feature) const
		{
			switch (feature)
			{
			case EVDF_RENDER_TO_TARGET:
			case EVDF_HARDWARE_TL:
			case EVDF_MULTITEXTURE:
			case EVDF_BILINEAR_FILTER:
			case EVDF_MIP_MAP:
			case EVDF_MIP_MAP_AUTO_UPDATE:
			case EVDF_VERTEX_SHADER_1_1:
			case EVDF_PIXEL_SHADER_1_1:
			case EVDF_PIXEL_SHADER_1_2:
			case EVDF_PIXEL_SHADER_2_0:
			case EVDF_VERTEX_SHADER_2_0:
			case EVDF_ARB_GLSL:
			case EVDF_TEXTURE_NSQUARE:
			case EVDF_TEXTURE_NPOT:
			case EVDF_FRAMEBUFFER_OBJECT:
			case EVDF_VERTEX_BUFFER_OBJECT:
			case EVDF_COLOR_MASK:
			case EVDF_ALPHA_TO_COVERAGE:
			case EVDF_POLYGON_OFFSET:
			case EVDF_BLEND_OPERATIONS:
			case EVDF_BLEND_SEPARATE:
			case EVDF_TEXTURE_MATRIX:
			case EVDF_TEXTURE_CUBEMAP:
				return true;
			case EVDF_ARB_VERTEX_PROGRAM_1:
			case EVDF_ARB_FRAGMENT_PROGRAM_1:
			case EVDF_GEOMETRY_SHADER:
			case EVDF_MULTIPLE_RENDER_TARGETS:
			case EVDF_MRT_BLEND:
			case EVDF_MRT_COLOR_MASK:
			case EVDF_MRT_BLEND_FUNC:
			case EVDF_OCCLUSION_QUERY:
				return false;
			case EVDF_STENCIL_BUFFER:
				return StencilBuffer;
			default:
				return false;
			};
		}

		static int GetInteger(unsigned int key) {
			int val = 0;
			GL.GetIntegerv(key, &val);
			return val;
		};

		inline void irrGlActiveTexture(unsigned int texture)
		{
			GL.ActiveTexture(texture);
		}

		inline void irrGlCompressedTexImage2D(unsigned int target, int level, unsigned int internalformat, int width, int height, int border,
			int imageSize, const void* data)
		{
			os::Printer::log("Compressed textures aren't supported", ELL_ERROR);
		}

		inline void irrGlCompressedTexSubImage2D(unsigned int target, int level, int xoffset, int yoffset, int width, int height,
			unsigned int format, int imageSize, const void* data)
		{
			os::Printer::log("Compressed textures aren't supported", ELL_ERROR);
		}

		inline void irrGlUseProgram(unsigned int prog)
		{
			GL.UseProgram(prog);
		}

		inline void irrGlBindFramebuffer(unsigned int target, unsigned int framebuffer)
		{
			GL.BindFramebuffer(target, framebuffer);
		}

		inline void irrGlDeleteFramebuffers(int n, const unsigned int *framebuffers)
		{
			GL.DeleteFramebuffers(n, framebuffers);
		}

		inline void irrGlGenFramebuffers(int n, unsigned int *framebuffers)
		{
			GL.GenFramebuffers(n, framebuffers);
		}

		inline unsigned int irrGlCheckFramebufferStatus(unsigned int target)
		{
			return GL.CheckFramebufferStatus(target);
		}

		inline void irrGlFramebufferTexture2D(unsigned int target, unsigned int attachment, unsigned int textarget, unsigned int texture, int level)
		{
			GL.FramebufferTexture2D(target, attachment, textarget, texture, level);
		}

		inline void irrGlGenerateMipmap(unsigned int target)
		{
			GL.GenerateMipmap(target);
		}

		inline void irrGlDrawBuffer(unsigned int mode)
		{
			GL.DrawBuffer(mode);
		}

		inline void irrGlDrawBuffers(int n, const unsigned int *bufs)
		{
			GL.DrawBuffers(n, bufs);
		}

		inline void irrGlBlendFuncSeparate(unsigned int sfactorRGB, unsigned int dfactorRGB, unsigned int sfactorAlpha, unsigned int dfactorAlpha)
		{
			GL.BlendFuncSeparate(sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
		}

		inline void irrGlBlendEquation(unsigned int mode)
		{
			GL.BlendEquation(mode);
		}

		bool AnisotropicFilterSupported = false;
		bool BlendMinMaxSupported = false;

	private:
		void addExtension(std::string name);
		void updateLegacyExtensionList();

		std::unordered_set<std::string> Extensions;
	};

}
}
