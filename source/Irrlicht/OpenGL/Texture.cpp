// Copyright (C) 2023 Vitaliy Lobachevskiy
// Copyright (C) 2015 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "Texture.h"
#include "COpenGLCoreCacheHandler.h"

namespace irr
{
namespace video
{

COpenGL3Texture::COpenGL3Texture(const io::path& name, const core::array<IImage*>& images, E_TEXTURE_TYPE type, COpenGL3DriverBase* driver) : ITexture(name, type), Driver(driver), TextureType(GL_TEXTURE_2D),
	TextureName(0), InternalFormat(GL_RGBA), PixelFormat(GL_RGBA), PixelType(GL_UNSIGNED_BYTE), Converter(0), LockReadOnly(false), LockImage(0),
	KeepImage(false), LegacyAutoGenerateMipMaps(false)
{
	_IRR_DEBUG_BREAK_IF(images.size() == 0)

	DriverType = Driver->getDriverType();
	TextureType = TextureTypeIrrToGL(Type);
	HasMipMaps = Driver->getTextureCreationFlag(ETCF_CREATE_MIP_MAPS);
	KeepImage = Driver->getTextureCreationFlag(ETCF_ALLOW_MEMORY_COPY);

	getImageValues(images[0]);

	const core::array<IImage*>* tmpImages = &images;

	if (KeepImage || OriginalSize != Size || OriginalColorFormat != ColorFormat)
	{
		Images.set_used(images.size());

		for (u32 i = 0; i < images.size(); ++i)
		{
			Images[i] = Driver->createImage(ColorFormat, Size);

			if (images[i]->getDimension() == Size)
				images[i]->copyTo(Images[i]);
			else
				images[i]->copyToScaling(Images[i]);

			if ( images[i]->getMipMapsData() )
			{
				if ( OriginalSize == Size && OriginalColorFormat == ColorFormat )
				{
					Images[i]->setMipMapsData( images[i]->getMipMapsData(), false);
				}
				else
				{
					// TODO: handle at least mipmap with changing color format
					os::Printer::log("COpenGL3Texture: Can't handle format changes for mipmap data. Mipmap data dropped", ELL_WARNING);
				}
			}
		}

		tmpImages = &Images;
	}

	glGenTextures(1, &TextureName);

	const COpenGL3Texture* prevTexture = Driver->getCacheHandler()->getTextureCache().get(0);
	Driver->getCacheHandler()->getTextureCache().set(0, this);

	glTexParameteri(TextureType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(TextureType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	for (u32 i = 0; i < (*tmpImages).size(); ++i)
		uploadTexture(true, i, 0, (*tmpImages)[i]->getData());

	if (HasMipMaps && !LegacyAutoGenerateMipMaps)
	{
		// Create mipmaps (either from image mipmaps or generate them)
		for (u32 i = 0; i < (*tmpImages).size(); ++i)
		{
			void* mipmapsData = (*tmpImages)[i]->getMipMapsData();
			regenerateMipMapLevels(mipmapsData, i);
		}
	}

	if (!KeepImage)
	{
		for (u32 i = 0; i < Images.size(); ++i)
			Images[i]->drop();

		Images.clear();
	}


	Driver->getCacheHandler()->getTextureCache().set(0, prevTexture);

	Driver->testGLError(__LINE__);
}

COpenGL3Texture::COpenGL3Texture(const io::path& name, const core::dimension2d<u32>& size, E_TEXTURE_TYPE type, ECOLOR_FORMAT format, COpenGL3DriverBase* driver)
	: ITexture(name, type),
	Driver(driver), TextureType(GL_TEXTURE_2D),
	TextureName(0), InternalFormat(GL_RGBA), PixelFormat(GL_RGBA), PixelType(GL_UNSIGNED_BYTE), Converter(0), LockReadOnly(false), LockImage(0), KeepImage(false),
	LegacyAutoGenerateMipMaps(false)
{
	DriverType = Driver->getDriverType();
	TextureType = TextureTypeIrrToGL(Type);
	HasMipMaps = false;
	IsRenderTarget = true;

	OriginalColorFormat = format;

	if (ECF_UNKNOWN == OriginalColorFormat)
		ColorFormat = getBestColorFormat(Driver->getColorFormat());
	else
		ColorFormat = OriginalColorFormat;

	OriginalSize = size;
	Size = OriginalSize;

	Pitch = Size.Width * IImage::getBitsPerPixelFromFormat(ColorFormat) / 8;

	if ( !Driver->getColorFormatParameters(ColorFormat, InternalFormat, PixelFormat, PixelType, &Converter) )
	{
		os::Printer::log("COpenGL3Texture: Color format is not supported", ColorFormatNames[ColorFormat < ECF_UNKNOWN?ColorFormat:ECF_UNKNOWN], ELL_ERROR);
	}

	glGenTextures(1, &TextureName);

	const COpenGL3Texture* prevTexture = Driver->getCacheHandler()->getTextureCache().get(0);
	Driver->getCacheHandler()->getTextureCache().set(0, this);


	glTexParameteri(TextureType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(TextureType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(TextureType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(TextureType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(TextureType, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	StatesCache.WrapU = ETC_CLAMP_TO_EDGE;
	StatesCache.WrapV = ETC_CLAMP_TO_EDGE;
	StatesCache.WrapW = ETC_CLAMP_TO_EDGE;

	switch (Type)
	{
	case ETT_2D:
		glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, Size.Width, Size.Height, 0, PixelFormat, PixelType, 0);
		break;
	case ETT_CUBEMAP:
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, InternalFormat, Size.Width, Size.Height, 0, PixelFormat, PixelType, 0);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, InternalFormat, Size.Width, Size.Height, 0, PixelFormat, PixelType, 0);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, InternalFormat, Size.Width, Size.Height, 0, PixelFormat, PixelType, 0);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, InternalFormat, Size.Width, Size.Height, 0, PixelFormat, PixelType, 0);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, InternalFormat, Size.Width, Size.Height, 0, PixelFormat, PixelType, 0);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, InternalFormat, Size.Width, Size.Height, 0, PixelFormat, PixelType, 0);
		break;
	}

	Driver->getCacheHandler()->getTextureCache().set(0, prevTexture);
	if ( Driver->testGLError(__LINE__) )
	{
		char msg[256];
		snprintf_irr(msg, 256, "COpenGL3Texture: InternalFormat:0x%04x PixelFormat:0x%04x", (int)InternalFormat, (int)PixelFormat);
		os::Printer::log(msg, ELL_ERROR);
	}
}

COpenGL3Texture::~COpenGL3Texture()
{
	if (TextureName)
		glDeleteTextures(1, &TextureName);

	if (LockImage)
		LockImage->drop();

	for (u32 i = 0; i < Images.size(); ++i)
		Images[i]->drop();
}

void* COpenGL3Texture::lock(E_TEXTURE_LOCK_MODE mode, u32 mipmapLevel, u32 layer, E_TEXTURE_LOCK_FLAGS lockFlags)
{
	assert(mipmapLevel == 0);
	assert(layer == 0);
	assert(lockFlags == ETLF_FLIP_Y_UP_RTT);

	if (LockImage)
		return LockImage->getData();

	if (IImage::isCompressedFormat(ColorFormat))
		return 0;

	LockReadOnly |= (mode == ETLM_READ_ONLY);

	if (KeepImage)
	{
		_IRR_DEBUG_BREAK_IF(0 > Images.size())

		LockImage = Images[0];
		LockImage->grab();
	}

	if ( !LockImage )
	{
		LockImage = Driver->createImage(ColorFormat, Size);

		if (LockImage && mode != ETLM_WRITE_ONLY)
		{
			if (!Driver->readTexture(LockImage, this))
			{
				LockImage->drop();
				LockImage = 0;
			}
		}

		Driver->testGLError(__LINE__);
	}

	return LockImage ? LockImage->getData() : nullptr;
}

void COpenGL3Texture::unlock()
{
	if (!LockImage)
		return;

	if (!LockReadOnly)
	{
		const COpenGL3Texture* prevTexture = Driver->getCacheHandler()->getTextureCache().get(0);
		Driver->getCacheHandler()->getTextureCache().set(0, this);

		uploadTexture(false, 0, 0, LockImage->getData());

		Driver->getCacheHandler()->getTextureCache().set(0, prevTexture);
	}

	LockImage->drop();

	LockReadOnly = false;
	LockImage = 0;
}

void COpenGL3Texture::regenerateMipMapLevels(void *data, u32 layer)
{
	if (!HasMipMaps || LegacyAutoGenerateMipMaps || (Size.Width <= 1 && Size.Height <= 1))
		return;

	const COpenGL3Texture* prevTexture = Driver->getCacheHandler()->getTextureCache().get(0);
	Driver->getCacheHandler()->getTextureCache().set(0, this);

	if (data)
	{
		u32 width = Size.Width;
		u32 height = Size.Height;
		u8* tmpData = static_cast<u8*>(data);
		u32 dataSize = 0;
		u32 level = 0;

		do
		{
			if (width > 1)
				width >>= 1;

			if (height > 1)
				height >>= 1;

			dataSize = IImage::getDataSizeFromFormat(ColorFormat, width, height);
			++level;

			uploadTexture(true, layer, level, tmpData);

			tmpData += dataSize;
		}
		while (width != 1 || height != 1);
	}
	else
	{
		Driver->irrGlGenerateMipmap(TextureType);
	}

	Driver->getCacheHandler()->getTextureCache().set(0, prevTexture);
}

void COpenGL3Texture::getImageValues(const IImage* image)
{
	OriginalColorFormat = image->getColorFormat();
	ColorFormat = getBestColorFormat(OriginalColorFormat);

	if ( !Driver->getColorFormatParameters(ColorFormat, InternalFormat, PixelFormat, PixelType, &Converter) )
	{
		os::Printer::log("getImageValues: Color format is not supported", ColorFormatNames[ColorFormat < ECF_UNKNOWN?ColorFormat:ECF_UNKNOWN], ELL_ERROR);
		// not quitting as it will use some alternative internal format
	}

	if (IImage::isCompressedFormat(image->getColorFormat()))
	{
		KeepImage = false;
	}

	OriginalSize = image->getDimension();
	Size = OriginalSize;

	if (Size.Width == 0 || Size.Height == 0)
	{
		os::Printer::log("Invalid size of image for texture.", ELL_ERROR);
		return;
	}

	const f32 ratio = (f32)Size.Width / (f32)Size.Height;

	if ((Size.Width > Driver->MaxTextureSize) && (ratio >= 1.f))
	{
		Size.Width = Driver->MaxTextureSize;
		Size.Height = (u32)(Driver->MaxTextureSize / ratio);
	}
	else if (Size.Height > Driver->MaxTextureSize)
	{
		Size.Height = Driver->MaxTextureSize;
		Size.Width = (u32)(Driver->MaxTextureSize * ratio);
	}

	bool needSquare = (!Driver->queryFeature(EVDF_TEXTURE_NSQUARE) || Type == ETT_CUBEMAP);

	Size = Size.getOptimalSize(!Driver->queryFeature(EVDF_TEXTURE_NPOT), needSquare, true, Driver->MaxTextureSize);

	Pitch = Size.Width * IImage::getBitsPerPixelFromFormat(ColorFormat) / 8;
}

void COpenGL3Texture::uploadTexture(bool initTexture, u32 layer, u32 level, void* data)
{
	if (!data)
		return;

	u32 width = Size.Width >> level;
	u32 height = Size.Height >> level;

	GLenum tmpTextureType = TextureType;

	if (tmpTextureType == GL_TEXTURE_CUBE_MAP)
	{
		_IRR_DEBUG_BREAK_IF(layer > 5)

		tmpTextureType = GL_TEXTURE_CUBE_MAP_POSITIVE_X + layer;
	}

	if (!IImage::isCompressedFormat(ColorFormat))
	{
		CImage* tmpImage = 0;
		void* tmpData = data;

		if (Converter)
		{
			const core::dimension2d<u32> tmpImageSize(width, height);

			tmpImage = new CImage(ColorFormat, tmpImageSize);
			tmpData = tmpImage->getData();

			Converter(data, tmpImageSize.getArea(), tmpData);
		}

		switch (TextureType)
		{
		case GL_TEXTURE_2D:
		case GL_TEXTURE_CUBE_MAP:
			if (initTexture)
				glTexImage2D(tmpTextureType, level, InternalFormat, width, height, 0, PixelFormat, PixelType, tmpData);
			else
				glTexSubImage2D(tmpTextureType, level, 0, 0, width, height, PixelFormat, PixelType, tmpData);
			Driver->testGLError(__LINE__);
			break;
		default:
			break;
		}

		delete tmpImage;
	}
	else
	{
		u32 dataSize = IImage::getDataSizeFromFormat(ColorFormat, width, height);

		switch (TextureType)
		{
		case GL_TEXTURE_2D:
		case GL_TEXTURE_CUBE_MAP:
			if (initTexture)
				Driver->irrGlCompressedTexImage2D(tmpTextureType, level, InternalFormat, width, height, 0, dataSize, data);
			else
				Driver->irrGlCompressedTexSubImage2D(tmpTextureType, level, 0, 0, width, height, PixelFormat, dataSize, data);
			Driver->testGLError(__LINE__);
			break;
		default:
			break;
		}
	}
}

ECOLOR_FORMAT COpenGL3Texture::getBestColorFormat(ECOLOR_FORMAT format) const
{
	// We only try for to adapt "simple" formats
	return (format <= ECF_A8R8G8B8) ? ECF_A8R8G8B8 : format;
}

GLenum COpenGL3Texture::TextureTypeIrrToGL(E_TEXTURE_TYPE type)
{
	switch ( type)
	{
	case ETT_2D:
		return GL_TEXTURE_2D;
	case ETT_CUBEMAP:
		return GL_TEXTURE_CUBE_MAP;
	}

	os::Printer::log("COpenGL3Texture::TextureTypeIrrToGL unknown texture type", ELL_WARNING);
	return GL_TEXTURE_2D;
}

}
}
