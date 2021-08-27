// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_C_SOFTWARE_TEXTURE_H_INCLUDED
#define IRR_C_SOFTWARE_TEXTURE_H_INCLUDED

#include "ITexture.h"
#include "IRenderTarget.h"
#include "CImage.h"

namespace irr
{
namespace video
{

class CSoftwareDriver;

/*!
	interface for a Video Driver dependent Texture.
*/
class CSoftwareTexture : public ITexture
{
public:

	//! constructor
	CSoftwareTexture(IImage* surface, const io::path& name, bool renderTarget=false);

	//! destructor
	virtual ~CSoftwareTexture();

	//! lock function
	virtual void* lock(E_TEXTURE_LOCK_MODE mode = ETLM_READ_WRITE, u32 mipmapLevel=0, u32 layer = 0, E_TEXTURE_LOCK_FLAGS lockFlags = ETLF_FLIP_Y_UP_RTT) IRR_OVERRIDE;

	//! unlock function
	virtual void unlock() IRR_OVERRIDE;

	//! returns unoptimized surface
	virtual CImage* getImage();

	//! returns texture surface
	virtual CImage* getTexture();

	virtual void regenerateMipMapLevels(void* data = 0, u32 layer = 0) IRR_OVERRIDE;

private:
	CImage* Image;
	CImage* Texture;
};

/*!
	interface for a Video Driver dependent render target.
*/
class CSoftwareRenderTarget : public IRenderTarget
{
public:
	CSoftwareRenderTarget(CSoftwareDriver* driver);
	virtual ~CSoftwareRenderTarget();

	virtual void setTextures(ITexture* const * textures, u32 numTextures, ITexture* depthStencil, const E_CUBE_SURFACE* cubeSurfaces, u32 numCubeSurfaces) IRR_OVERRIDE;

	ITexture* getTexture() const;

protected:
	CSoftwareDriver* Driver;
};


} // end namespace video
} // end namespace irr

#endif
