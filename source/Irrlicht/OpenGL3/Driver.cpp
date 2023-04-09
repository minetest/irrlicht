// Copyright (C) 2023 Vitaliy Lobachevskiy
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "Driver.h"
#include <memory>
#include <cstring>
#include "OpenGL/Texture.h"
#include "COpenGLCoreCacheHandler.h"

namespace irr {
namespace video {

	E_DRIVER_TYPE COpenGL3Driver::getDriverType() const {
		return EDT_OPENGL3;
	}

	IVideoDriver* createOpenGL3Driver(const SIrrlichtCreationParameters& params, io::IFileSystem* io, IContextManager* contextManager)
	{
		COpenGL3Driver* driver = new COpenGL3Driver(params, io, contextManager);
		driver->genericDriverInit(params.WindowSize, params.Stencilbuffer);	// don't call in constructor, it uses virtual function calls of driver
		return driver;
	}

	static void flipImageY(IImage *image) {
		const s32 pitch = image->getPitch();
		const u32 height = image->getDimension().Height;

		u8* srcA = static_cast<u8*>(image->getData());
		u8* srcB = srcA + (height - 1) * pitch;

		std::unique_ptr<u8[]> tmpBuffer{new u8[pitch]};

		for (u32 i = 0; i < height / 2; i++) {
			memcpy(tmpBuffer.get(), srcA, pitch);
			memcpy(srcA, srcB, pitch);
			memcpy(srcB, tmpBuffer.get(), pitch);
			srcA += pitch;
			srcB -= pitch;
		}
	}

	bool COpenGL3Driver::readTexture(IImage *into, COpenGL3Texture *texture) {
		CacheHandler->getTextureCache().set(0, texture);
		testGLError(__LINE__);

		GLenum tmpTextureType = texture->getOpenGLTextureType();
		if (tmpTextureType == GL_TEXTURE_CUBE_MAP)
			tmpTextureType = GL_TEXTURE_CUBE_MAP_POSITIVE_X;

		glGetTexImage(tmpTextureType, 0, texture->getPixelFormat(), texture->getPixelType(), into->getData());
		testGLError(__LINE__);

		if (texture->isRenderTarget()) // FIXME this is due to ETLF_FLIP_Y_UP_RTT being forced on
			flipImageY(into);

		return true;
	}
}
}
