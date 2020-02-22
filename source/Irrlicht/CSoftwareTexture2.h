// Copyright (C) 2002-2012 Nikolaus Gebhardt / Thomas Alten
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_SOFTWARE_2_TEXTURE_H_INCLUDED__
#define __C_SOFTWARE_2_TEXTURE_H_INCLUDED__

#include "SoftwareDriver2_compile_config.h"

#include "ITexture.h"
#include "IRenderTarget.h"
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
		GEN_MIPMAP	     = 1,
		IS_RENDERTARGET  = 2,
		ALLOW_NPOT		= 4,		//allow non power of two
		IMAGE_IS_LINEAR	 = 8,
		TEXTURE_IS_LINEAR = 16,
	};
	CSoftwareTexture2(IImage* surface, const io::path& name, u32 flags /*eTex2Flags*/, CBurningVideoDriver* driver);

	//! destructor
	virtual ~CSoftwareTexture2();

	u32 getMipmapLevel(s32 newLevel) const
	{
		if ( newLevel < 0 ) newLevel = 0;
		else if ( newLevel >= SOFTWARE_DRIVER_2_MIPMAPPING_MAX ) newLevel = SOFTWARE_DRIVER_2_MIPMAPPING_MAX - 1;

		while ( newLevel > 0 && MipMap[newLevel] == 0 ) newLevel -= 1;
		return newLevel;
	}

	//! lock function
	virtual void* lock(E_TEXTURE_LOCK_MODE mode, u32 mipmapLevel, u32 layer, E_TEXTURE_LOCK_FLAGS lockFlags = ETLF_FLIP_Y_UP_RTT) _IRR_OVERRIDE_
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
	virtual void unlock() _IRR_OVERRIDE_
	{
	}

	//! compare the area drawn with the area of the texture
	f32 getLODFactor( const f32 texArea ) const
	{
		return MipMap0_Area[0]* MipMap0_Area[1] * 0.5f * texArea;
		//return MipMap[0]->getImageDataSizeInPixels () * texArea;
	}

	const u32* getMipMap0_Area() const
	{
		return MipMap0_Area;
	}

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

	virtual void regenerateMipMapLevels(void* data = 0, u32 layer = 0) _IRR_OVERRIDE_;

private:
	void calcDerivative();

	//! controls MipmapSelection. relation between drawn area and image size
	u32 MipMapLOD; // 0 .. original Texture pot -SOFTWARE_DRIVER_2_MIPMAPPING_MAX
	u32 Flags; //eTex2Flags
	ECOLOR_FORMAT OriginalFormat;
	CBurningVideoDriver* Driver;

	CImage* MipMap[SOFTWARE_DRIVER_2_MIPMAPPING_MAX];
	CSoftwareTexture2_Bound TexBound[SOFTWARE_DRIVER_2_MIPMAPPING_MAX];
	u32 MipMap0_Area[2];
};

/*!
interface for a Video Driver dependent render target.
*/
class CSoftwareRenderTarget2 : public IRenderTarget
{
public:
	CSoftwareRenderTarget2(CBurningVideoDriver* driver);
	virtual ~CSoftwareRenderTarget2();

	virtual void setTexture(const core::array<ITexture*>& texture, ITexture* depthStencil, const core::array<E_CUBE_SURFACE>& cubeSurfaces) _IRR_OVERRIDE_;

	ITexture* getTexture() const;

protected:
	CBurningVideoDriver* Driver;
};

} // end namespace video
} // end namespace irr

#endif // __C_SOFTWARE_2_TEXTURE_H_INCLUDED__


