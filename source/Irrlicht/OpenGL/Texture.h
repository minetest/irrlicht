// Copyright (C) 2023 Vitaliy Lobachevskiy
// Copyright (C) 2015 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include "irrArray.h"
#include "SMaterialLayer.h"
#include "ITexture.h"
#include "EDriverFeatures.h"
#include "os.h"
#include "CImage.h"
#include "CColorConverter.h"
#include "Driver.h"
#include "SDL_opengl.h"

namespace irr
{
namespace video
{

class COpenGL3Texture : public ITexture
{
public:
	struct SStatesCache
	{
		SStatesCache() : WrapU(ETC_REPEAT), WrapV(ETC_REPEAT), WrapW(ETC_REPEAT),
			LODBias(0), AnisotropicFilter(0), BilinearFilter(false), TrilinearFilter(false),
			MipMapStatus(false), IsCached(false)
		{
		}

		u8 WrapU;
		u8 WrapV;
		u8 WrapW;
		s8 LODBias;
		u8 AnisotropicFilter;
		bool BilinearFilter;
		bool TrilinearFilter;
		bool MipMapStatus;
		bool IsCached;
	};

	COpenGL3Texture(const io::path& name, const core::array<IImage*>& images, E_TEXTURE_TYPE type, COpenGL3DriverBase* driver);
	COpenGL3Texture(const io::path& name, const core::dimension2d<u32>& size, E_TEXTURE_TYPE type, ECOLOR_FORMAT format, COpenGL3DriverBase* driver);
	virtual ~COpenGL3Texture();

	void *lock(E_TEXTURE_LOCK_MODE mode = ETLM_READ_WRITE, u32 mipmapLevel=0, u32 layer = 0, E_TEXTURE_LOCK_FLAGS lockFlags = ETLF_FLIP_Y_UP_RTT) override;
	void unlock() override;

	void regenerateMipMapLevels(void *data = nullptr, u32 layer = 0) override;

	GLenum getOpenGLTextureType() const
	{
		return TextureType;
	}

	GLuint getOpenGLTextureName() const
	{
		return TextureName;
	}

	SStatesCache& getStatesCache() const
	{
		return StatesCache;
	}

protected:
	void *getLockImageData(irr::u32 miplevel) const;
	void getImageValues(const IImage* image);
	void uploadTexture(bool initTexture, u32 layer, u32 level, void* data);

	ECOLOR_FORMAT getBestColorFormat(ECOLOR_FORMAT format) const; // depends on the driver thus not static
	static GLenum TextureTypeIrrToGL(E_TEXTURE_TYPE type);

	COpenGL3DriverBase* Driver;

	GLenum TextureType;
	GLuint TextureName;
	GLint InternalFormat;
	GLenum PixelFormat;
	GLenum PixelType;
	void (*Converter)(const void*, s32, void*);

	bool LockReadOnly;
	IImage* LockImage;
	u32 LockLayer;

	bool KeepImage;
	core::array<IImage*> Images;

	u8 MipLevelStored;
	bool LegacyAutoGenerateMipMaps;

	mutable SStatesCache StatesCache;
};

}
}
