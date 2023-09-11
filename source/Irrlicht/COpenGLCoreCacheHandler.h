// Copyright (C) 2015 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include "SMaterial.h"
#include "ITexture.h"

namespace irr
{
namespace video
{

enum ESetTextureActive
{
	EST_ACTIVE_ALWAYS,		// texture unit always active after set call
	EST_ACTIVE_ON_CHANGE	// texture unit only active after call when texture changed in cache
};


template <class TOpenGLDriver, class TOpenGLTexture>
class COpenGLCoreCacheHandler
{
	class STextureCache
	{
	public:
		STextureCache(COpenGLCoreCacheHandler& cacheHandler, E_DRIVER_TYPE driverType, u32 textureCount) :
			CacheHandler(cacheHandler), DriverType(driverType), TextureCount(textureCount)
		{
			for (u32 i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
			{
				Texture[i] = 0;
			}
		}

		~STextureCache()
		{
			clear();
		}

		const TOpenGLTexture* operator[](int index) const
		{
			if (static_cast<u32>(index) < MATERIAL_MAX_TEXTURES)
				return Texture[static_cast<u32>(index)];

			return 0;
		}

		const TOpenGLTexture* get(u32 index) const
		{
			if (index < MATERIAL_MAX_TEXTURES)
				return Texture[index];

			return 0;
		}

		bool set(u32 index, const ITexture* texture, ESetTextureActive esa=EST_ACTIVE_ALWAYS)
		{
			bool status = false;

			E_DRIVER_TYPE type = DriverType;

			if (index < MATERIAL_MAX_TEXTURES && index < TextureCount)
			{
				if ( esa == EST_ACTIVE_ALWAYS )
					CacheHandler.setActiveTexture(GL.TEXTURE0 + index);

				const TOpenGLTexture* prevTexture = Texture[index];

				if (texture != prevTexture)
				{
					if ( esa == EST_ACTIVE_ON_CHANGE )
						CacheHandler.setActiveTexture(GL.TEXTURE0 + index);

					if (texture)
					{
						type = texture->getDriverType();

						if (type == DriverType)
						{
							texture->grab();

							const TOpenGLTexture* curTexture = static_cast<const TOpenGLTexture*>(texture);
							const unsigned int curTextureType = curTexture->getOpenGLTextureType();
							const unsigned int prevTextureType = (prevTexture) ? prevTexture->getOpenGLTextureType() : curTextureType;

							if (curTextureType != prevTextureType)
							{
								GL.BindTexture(prevTextureType, 0);

#if ( defined(IRR_COMPILE_GL_COMMON) || defined(IRR_COMPILE_GLES_COMMON) )
								GL.Disable(prevTextureType);
								GL.Enable(curTextureType);
#endif
							}
#if ( defined(IRR_COMPILE_GL_COMMON) || defined(IRR_COMPILE_GLES_COMMON) )
							else if (!prevTexture)
								GL.Enable(curTextureType);
#endif

							GL.BindTexture(curTextureType, static_cast<const TOpenGLTexture*>(texture)->getOpenGLTextureName());
						}
						else
						{
							texture = 0;

							os::Printer::log("Fatal Error: Tried to set a texture not owned by this driver.", ELL_ERROR);
							os::Printer::log("Texture type", irr::core::stringc((int)type), ELL_ERROR);
							os::Printer::log("Driver (or cache handler) type", irr::core::stringc((int)DriverType), ELL_ERROR);
						}
					}

					if (!texture && prevTexture)
					{
						const unsigned int prevTextureType = prevTexture->getOpenGLTextureType();

						GL.BindTexture(prevTextureType, 0);

#if ( defined(IRR_COMPILE_GL_COMMON) || defined(IRR_COMPILE_GLES_COMMON) )
						GL.Disable(prevTextureType);
#endif
					}

					Texture[index] = static_cast<const TOpenGLTexture*>(texture);

					if (prevTexture)
						prevTexture->drop();
				}

				status = true;
			}

			return (status && type == DriverType);
		}

		void remove(ITexture* texture)
		{
			if (!texture)
				return;

			for (u32 i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
			{
				if (Texture[i] == texture)
				{
					Texture[i] = 0;

					texture->drop();
				}
			}
		}

		void clear()
		{
			for (u32 i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
			{
				if (Texture[i])
				{
					const TOpenGLTexture* prevTexture = Texture[i];

					Texture[i] = 0;

					prevTexture->drop();
				}
			}
		}

	private:
		COpenGLCoreCacheHandler& CacheHandler;

		E_DRIVER_TYPE DriverType;

		const TOpenGLTexture* Texture[MATERIAL_MAX_TEXTURES];
		u32 TextureCount;
	};

public:
	COpenGLCoreCacheHandler(TOpenGLDriver* driver) :
		Driver(driver),
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4355)	// Warning: "'this' : used in base member initializer list. ". It's OK, we don't use the reference in STextureCache constructor.
#endif
		TextureCache(STextureCache(*this, driver->getDriverType(), driver->getFeature().MaxTextureUnits)),
#if defined(_MSC_VER)
#pragma warning(pop)
#endif
		FrameBufferCount(0), BlendEquation(0), BlendSourceRGB(0),
		BlendDestinationRGB(0), BlendSourceAlpha(0), BlendDestinationAlpha(0), Blend(0), BlendEquationInvalid(false), BlendFuncInvalid(false), BlendInvalid(false),
		ColorMask(0), ColorMaskInvalid(false), CullFaceMode(GL.BACK), CullFace(false), DepthFunc(GL.LESS), DepthMask(true), DepthTest(false), FrameBufferID(0),
		ProgramID(0), ActiveTexture(GL.TEXTURE0), ViewportX(0), ViewportY(0)
	{
		const COpenGLCoreFeature& feature = Driver->getFeature();

		FrameBufferCount = core::max_(static_cast<unsigned int>(1), static_cast<unsigned int>(feature.MultipleRenderTarget));

		BlendEquation = new unsigned int[FrameBufferCount];
		BlendSourceRGB = new unsigned int[FrameBufferCount];
		BlendDestinationRGB = new unsigned int[FrameBufferCount];
		BlendSourceAlpha = new unsigned int[FrameBufferCount];
		BlendDestinationAlpha = new unsigned int[FrameBufferCount];
		Blend = new bool[FrameBufferCount];
		ColorMask = new u8[FrameBufferCount];

		// Initial OpenGL values from specification.

		if (feature.BlendOperation)
		{
			Driver->irrGlBlendEquation(GL.FUNC_ADD);
		}

		for (u32 i = 0; i < FrameBufferCount; ++i)
		{
			BlendEquation[i] = GL.FUNC_ADD;

			BlendSourceRGB[i] = GL.ONE;
			BlendDestinationRGB[i] = GL.ZERO;
			BlendSourceAlpha[i] = GL.ONE;
			BlendDestinationAlpha[i] = GL.ZERO;

			Blend[i] = false;
			ColorMask[i] = ECP_ALL;
		}

		GL.BlendFunc(GL.ONE, GL.ZERO);
		GL.Disable(GL.BLEND);

		GL.ColorMask(true, true, true, true);

		GL.CullFace(CullFaceMode);
		GL.Disable(GL.CULL_FACE);

		GL.DepthFunc(DepthFunc);
		GL.DepthMask(true);
		GL.Disable(GL.DEPTH_TEST);

		Driver->irrGlActiveTexture(ActiveTexture);

#if ( defined(IRR_COMPILE_GL_COMMON) || defined(IRR_COMPILE_GLES_COMMON) )
		GL.Disable(GL.TEXTURE_2D);
#endif

		const core::dimension2d<u32> ScreenSize = Driver->getScreenSize();
		ViewportWidth = ScreenSize.Width;
		ViewportHeight = ScreenSize.Height;
		GL.Viewport(ViewportX, ViewportY, ViewportWidth, ViewportHeight);
	}

	virtual ~COpenGLCoreCacheHandler()
	{
		delete[] BlendEquation;
		delete[] BlendSourceRGB;
		delete[] BlendDestinationRGB;
		delete[] BlendSourceAlpha;
		delete[] BlendDestinationAlpha;
		delete[] Blend;

		delete[] ColorMask;
	}

	E_DRIVER_TYPE getDriverType() const
	{
		return Driver->getDriverType();
	}

	STextureCache& getTextureCache()
	{
		return TextureCache;
	}

	// Blending calls.

	void setBlendEquation(unsigned int mode)
	{
		if (BlendEquation[0] != mode || BlendEquationInvalid)
		{
			Driver->irrGlBlendEquation(mode);

			for (unsigned int i = 0; i < FrameBufferCount; ++i)
				BlendEquation[i] = mode;

			BlendEquationInvalid = false;
		}
	}

	void setBlendEquationIndexed(unsigned int index, unsigned int mode)
	{
		if (index < FrameBufferCount && BlendEquation[index] != mode)
		{
			Driver->irrGlBlendEquationIndexed(index, mode);

			BlendEquation[index] = mode;
			BlendEquationInvalid = true;
		}
	}

	void setBlendFunc(unsigned int source, unsigned int destination)
	{
		if (BlendSourceRGB[0] != source || BlendDestinationRGB[0] != destination ||
			BlendSourceAlpha[0] != source || BlendDestinationAlpha[0] != destination ||
			BlendFuncInvalid)
		{
			GL.BlendFunc(source, destination);

			for (unsigned int i = 0; i < FrameBufferCount; ++i)
			{
				BlendSourceRGB[i] = source;
				BlendDestinationRGB[i] = destination;
				BlendSourceAlpha[i] = source;
				BlendDestinationAlpha[i] = destination;
			}

			BlendFuncInvalid = false;
		}
	}

	void setBlendFuncSeparate(unsigned int sourceRGB, unsigned int destinationRGB, unsigned int sourceAlpha, unsigned int destinationAlpha)
	{
		if (sourceRGB != sourceAlpha || destinationRGB != destinationAlpha)
		{
			if (BlendSourceRGB[0] != sourceRGB || BlendDestinationRGB[0] != destinationRGB ||
				BlendSourceAlpha[0] != sourceAlpha || BlendDestinationAlpha[0] != destinationAlpha ||
				BlendFuncInvalid)
			{
				Driver->irrGlBlendFuncSeparate(sourceRGB, destinationRGB, sourceAlpha, destinationAlpha);

				for (unsigned int i = 0; i < FrameBufferCount; ++i)
				{
					BlendSourceRGB[i] = sourceRGB;
					BlendDestinationRGB[i] = destinationRGB;
					BlendSourceAlpha[i] = sourceAlpha;
					BlendDestinationAlpha[i] = destinationAlpha;
				}

				BlendFuncInvalid = false;
			}
		}
		else
		{
			setBlendFunc(sourceRGB, destinationRGB);
		}
	}

	void setBlendFuncIndexed(unsigned int index, unsigned int source, unsigned int destination)
	{
		if (index < FrameBufferCount && (BlendSourceRGB[index] != source || BlendDestinationRGB[index] != destination ||
			BlendSourceAlpha[index] != source || BlendDestinationAlpha[index] != destination))
		{
			Driver->irrGlBlendFuncIndexed(index, source, destination);

			BlendSourceRGB[index] = source;
			BlendDestinationRGB[index] = destination;
			BlendSourceAlpha[index] = source;
			BlendDestinationAlpha[index] = destination;
			BlendFuncInvalid = true;
		}
	}

	void setBlendFuncSeparateIndexed(unsigned int index, unsigned int sourceRGB, unsigned int destinationRGB, unsigned int sourceAlpha, unsigned int destinationAlpha)
	{
		if (sourceRGB != sourceAlpha || destinationRGB != destinationAlpha)
		{
			if (index < FrameBufferCount && (BlendSourceRGB[index] != sourceRGB || BlendDestinationRGB[index] != destinationRGB ||
				BlendSourceAlpha[index] != sourceAlpha || BlendDestinationAlpha[index] != destinationAlpha))
			{
				Driver->irrGlBlendFuncSeparateIndexed(index, sourceRGB, destinationRGB, sourceAlpha, destinationAlpha);

				BlendSourceRGB[index] = sourceRGB;
				BlendDestinationRGB[index] = destinationRGB;
				BlendSourceAlpha[index] = sourceAlpha;
				BlendDestinationAlpha[index] = destinationAlpha;
				BlendFuncInvalid = true;
			}
		}
		else
		{
			setBlendFuncIndexed(index, sourceRGB, destinationRGB);
		}
	}

	void setBlend(bool enable)
	{
		if (Blend[0] != enable || BlendInvalid)
		{
			if (enable)
				GL.Enable(GL.BLEND);
			else
				GL.Disable(GL.BLEND);

			for (unsigned int i = 0; i < FrameBufferCount; ++i)
				Blend[i] = enable;

			BlendInvalid = false;
		}
	}

	void setBlendIndexed(unsigned int index, bool enable)
	{
		if (index < FrameBufferCount && Blend[index] != enable)
		{
			if (enable)
				Driver->irrGlEnableIndexed(GL.BLEND, index);
			else
				Driver->irrGlDisableIndexed(GL.BLEND, index);

			Blend[index] = enable;
			BlendInvalid = true;
		}
	}

	// Color Mask.

	void getColorMask(u8& mask)
	{
		mask = ColorMask[0];
	}

	void setColorMask(u8 mask)
	{
		if (ColorMask[0] != mask || ColorMaskInvalid)
		{
			GL.ColorMask((mask & ECP_RED) ? true : false, (mask & ECP_GREEN) ? true : false, (mask & ECP_BLUE) ? true : false, (mask & ECP_ALPHA) ? true : false);

			for (unsigned int i = 0; i < FrameBufferCount; ++i)
				ColorMask[i] = mask;

			ColorMaskInvalid = false;
		}
	}

	void setColorMaskIndexed(unsigned int index, u8 mask)
	{
		if (index < FrameBufferCount && ColorMask[index] != mask)
		{
			Driver->irrGlColorMaskIndexed(index, (mask & ECP_RED) ? true : false, (mask & ECP_GREEN) ? true : false, (mask & ECP_BLUE) ? true : false, (mask & ECP_ALPHA) ? true : false);

			ColorMask[index] = mask;
			ColorMaskInvalid = true;
		}
	}

	// Cull face calls.

	void setCullFaceFunc(unsigned int mode)
	{
		if (CullFaceMode != mode)
		{
			GL.CullFace(mode);
			CullFaceMode = mode;
		}
	}

	void setCullFace(bool enable)
	{
		if (CullFace != enable)
		{
			if (enable)
				GL.Enable(GL.CULL_FACE);
			else
				GL.Disable(GL.CULL_FACE);

			CullFace = enable;
		}
	}

	// Depth calls.

	void setDepthFunc(unsigned int mode)
	{
		if (DepthFunc != mode)
		{
			GL.DepthFunc(mode);
			DepthFunc = mode;
		}
	}

	void getDepthMask(bool& depth)
	{
		depth = DepthMask;
	}

	void setDepthMask(bool enable)
	{
		if (DepthMask != enable)
		{
			if (enable)
				GL.DepthMask(true);
			else
				GL.DepthMask(false);

			DepthMask = enable;
		}
	}

    void getDepthTest(bool& enable)
    {
        enable = DepthTest;
    }

	void setDepthTest(bool enable)
	{
		if (DepthTest != enable)
		{
			if (enable)
				GL.Enable(GL.DEPTH_TEST);
			else
				GL.Disable(GL.DEPTH_TEST);

			DepthTest = enable;
		}
	}

	// FBO calls.

	void getFBO(unsigned int& frameBufferID) const
	{
		frameBufferID = FrameBufferID;
	}

	void setFBO(unsigned int frameBufferID)
	{
		if (FrameBufferID != frameBufferID)
		{
			Driver->irrGlBindFramebuffer(GL.FRAMEBUFFER, frameBufferID);
			FrameBufferID = frameBufferID;
		}
	}

	// Shaders calls.

	void getProgram(unsigned int& programID) const
	{
		programID = ProgramID;
	}

	void setProgram(unsigned int programID)
	{
		if (ProgramID != programID)
		{
			Driver->irrGlUseProgram(programID);
			ProgramID = programID;
		}
	}

	// Texture calls.

	void getActiveTexture(unsigned int& texture) const
	{
		texture = ActiveTexture;
	}

	void setActiveTexture(unsigned int texture)
	{
		if (ActiveTexture != texture)
		{
			Driver->irrGlActiveTexture(texture);
			ActiveTexture = texture;
		}
	}

	// Viewport calls.

	void getViewport(int& viewportX, int& viewportY, int& viewportWidth, int& viewportHeight) const
	{
		viewportX = ViewportX;
		viewportY = ViewportY;
		viewportWidth = ViewportWidth;
		viewportHeight = ViewportHeight;
	}

	void setViewport(int viewportX, int viewportY, int viewportWidth, int viewportHeight)
	{
		if (ViewportX != viewportX || ViewportY != viewportY || ViewportWidth != viewportWidth || ViewportHeight != viewportHeight)
		{
			GL.Viewport(viewportX, viewportY, viewportWidth, viewportHeight);
			ViewportX = viewportX;
			ViewportY = viewportY;
			ViewportWidth = viewportWidth;
			ViewportHeight = viewportHeight;
		}
	}

	//! Compare material to current cache and update it when there are differences
	// Some material renderers do change the cache beyond the original material settings
	// This corrects the material to represent the current cache state again.
	void correctCacheMaterial(irr::video::SMaterial& material)
	{
		// Fix textures which got removed
		for ( u32 i=0; i < MATERIAL_MAX_TEXTURES; ++i )
		{
			if ( material.TextureLayers[i].Texture && !TextureCache[i] )
			{
				material.TextureLayers[i].Texture = 0;
			}
		}
	}

protected:
	TOpenGLDriver* Driver;

	STextureCache TextureCache;

	unsigned int FrameBufferCount;

	unsigned int* BlendEquation;
	unsigned int* BlendSourceRGB;
	unsigned int* BlendDestinationRGB;
	unsigned int* BlendSourceAlpha;
	unsigned int* BlendDestinationAlpha;
	bool* Blend;
	bool BlendEquationInvalid;
	bool BlendFuncInvalid;
	bool BlendInvalid;


	u8* ColorMask;
	bool ColorMaskInvalid;

	unsigned int CullFaceMode;
	bool CullFace;

	unsigned int DepthFunc;
	bool DepthMask;
	bool DepthTest;

	unsigned int FrameBufferID;

	unsigned int ProgramID;

	unsigned int ActiveTexture;

	int ViewportX;
	int ViewportY;
	int ViewportWidth;
	int ViewportHeight;
};

}
}
