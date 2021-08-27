// Copyright (C) 2002-2012 Nikolaus Gebhardt / Thomas Alten
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_C_SOFTWARE_2_TEXTURE_H_INCLUDED
#define IRR_C_SOFTWARE_2_TEXTURE_H_INCLUDED

#include "SoftwareDriver2_compile_config.h"

#include "ITexture.h"
#if defined(PATCH_SUPERTUX_8_0_1_with_1_9_0)
#include "IVideoDriver.h"
#else
#include "IRenderTarget.h"
#endif
#include "CImage.h"

namespace irr
{
namespace video
{

class CBurningVideoDriver;

/*!
	interface for a Video Driver dependent Texture.
*/
struct CSoftwareTexture2_Bound
{
	f32 w;  // width - 0.5f;
	f32 h;  // height- 0.5f;
	f32 cx; // texelcenter x 1.f/width*0.5f
	f32 cy; // texelcenter y 1.f/height*0.5f
};

class CSoftwareTexture2 : public ITexture
{
public:

	//! constructor
	enum eTex2Flags
	{
		GEN_MIPMAP			= 1,		// has mipmaps
		GEN_MIPMAP_AUTO		= 2,		// automatic mipmap generation
		IS_RENDERTARGET		= 4,
		ALLOW_NPOT			= 8,		//allow non power of two
		IMAGE_IS_LINEAR		= 16,
		TEXTURE_IS_LINEAR	= 32,
	};
	CSoftwareTexture2(IImage* surface, const io::path& name, u32 flags /*eTex2Flags*/, CBurningVideoDriver* driver);

	//! destructor
	virtual ~CSoftwareTexture2();

	u32 getMipmapLevel(s32 newLevel) const
	{
		if ( newLevel < 0 ) newLevel = 0;
		else if ( newLevel >= (s32)array_size(MipMap)) newLevel = array_size(MipMap) - 1;

		while ( newLevel > 0 && MipMap[newLevel] == 0 ) newLevel -= 1;
		return newLevel;
	}

	//! lock function
#if defined(PATCH_SUPERTUX_8_0_1_with_1_9_0)
	virtual void* lock(E_TEXTURE_LOCK_MODE mode, u32 mipmapLevel)
#else
	virtual void* lock(E_TEXTURE_LOCK_MODE mode, u32 mipmapLevel, u32 layer, E_TEXTURE_LOCK_FLAGS lockFlags = ETLF_FLIP_Y_UP_RTT) IRR_OVERRIDE
#endif
	{
		if (Flags & GEN_MIPMAP)
		{
			//called from outside. must test
			MipMapLOD = getMipmapLevel(mipmapLevel);
			Size = MipMap[MipMapLOD]->getDimension();
			Pitch = MipMap[MipMapLOD]->getPitch();
		}

		return MipMap[MipMapLOD]->getData();
	}

	//! unlock function
	virtual void unlock() IRR_OVERRIDE
	{
	}
/*
	//! compare the area drawn with the area of the texture
	f32 getLODFactor( const f32 texArea ) const
	{
		return MipMap0_Area[0]* MipMap0_Area[1] * 0.5f * texArea;
		//return MipMap[0]->getImageDataSizeInPixels () * texArea;
	}
*/

	const u32* getMipMap0_Area() const
	{
		return MipMap0_Area;
	}
	f32 get_lod_bias() const { return LodBIAS; }

	//! returns unoptimized surface (misleading name. burning can scale down originalimage)
	virtual CImage* getImage() const
	{
		return MipMap[0];
	}

	//! returns texture surface
	virtual CImage* getTexture() const
	{
		return MipMap[MipMapLOD];
	}

	//precalculated dimx-1/dimx*0.5f
	const CSoftwareTexture2_Bound& getTexBound() const
	{
		return TexBound[MipMapLOD];
	}

#if !defined(PATCH_SUPERTUX_8_0_1_with_1_9_0)
	virtual void regenerateMipMapLevels(void* data = 0, u32 layer = 0) IRR_OVERRIDE;
#else
	virtual void regenerateMipMapLevels(void* data = 0);
#endif


#if defined(PATCH_SUPERTUX_8_0_1_with_1_9_0)
	const core::dimension2d<u32>& getOriginalSize() const { return OriginalSize; };
	const core::dimension2d<u32>& getSize() const { return Size; };
	E_DRIVER_TYPE getDriverType() const { return DriverType; };
	ECOLOR_FORMAT getColorFormat() const { return ColorFormat; };
	ECOLOR_FORMAT getOriginalColorFormat() const { return OriginalColorFormat; };
	u32 getPitch() const { return Pitch; };
	bool hasMipMaps() const { return HasMipMaps; }
	bool isRenderTarget() const { return IsRenderTarget; }

	core::dimension2d<u32> OriginalSize;
	core::dimension2d<u32> Size;
	E_DRIVER_TYPE DriverType;
	ECOLOR_FORMAT OriginalColorFormat;
	ECOLOR_FORMAT ColorFormat;
	u32 Pitch;
	bool HasMipMaps;
	bool IsRenderTarget;
#endif

private:
	void calcDerivative();

	//! controls MipmapSelection. relation between drawn area and image size
	u32 MipMapLOD; // 0 .. original Texture pot -SOFTWARE_DRIVER_2_MIPMAPPING_MAX
	u32 Flags; //eTex2Flags
	CBurningVideoDriver* Driver;

	CImage* MipMap[SOFTWARE_DRIVER_2_MIPMAPPING_MAX];
	CSoftwareTexture2_Bound TexBound[SOFTWARE_DRIVER_2_MIPMAPPING_MAX];
	u32 MipMap0_Area[2];
	f32 LodBIAS;	// Tweak mipmap selection
};

/*!
interface for a Video Driver dependent render target.
*/
class CSoftwareRenderTarget2 : public IRenderTarget
{
public:
	CSoftwareRenderTarget2(CBurningVideoDriver* driver);
	virtual ~CSoftwareRenderTarget2();

	virtual void setTextures(ITexture* const * textures, u32 numTextures, ITexture* depthStencil, const E_CUBE_SURFACE* cubeSurfaces, u32 numCubeSurfaces) IRR_OVERRIDE;

#if defined(PATCH_SUPERTUX_8_0_1_with_1_9_0)
	E_DRIVER_TYPE DriverType;
	core::array<ITexture*> Texture;
#endif

protected:
	CBurningVideoDriver* Driver;
};

} // end namespace video
} // end namespace irr

#endif // IRR_C_SOFTWARE_2_TEXTURE_H_INCLUDED
