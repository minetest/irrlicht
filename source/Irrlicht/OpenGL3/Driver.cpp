// Copyright (C) 2023 Vitaliy Lobachevskiy
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "Driver.h"
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

	bool COpenGL3Driver::readTexture(IImage *into, COpenGL3Texture *texture) {
		IImage* tmpImage = into;	// not sure yet if the size required by glGetTexImage is always correct, if not we might have to allocate a different tmpImage and convert colors later on.

		getCacheHandler()->getTextureCache().set(0, texture);
		testGLError(__LINE__);

		GLenum tmpTextureType = texture->getOpenGLTextureType();

		if (tmpTextureType == GL_TEXTURE_CUBE_MAP)
		{
			tmpTextureType = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
		}

		glGetTexImage(tmpTextureType, 0, texture->getPixelFormat(), texture->getPixelType(), tmpImage->getData());
		testGLError(__LINE__);

		if (texture->isRenderTarget()) // FIXME this is due to ETLF_FLIP_Y_UP_RTT being forced on
		{
			const s32 pitch = tmpImage->getPitch();

			u8* srcA = static_cast<u8*>(tmpImage->getData());
			u8* srcB = srcA + (tmpImage->getDimension().Height - 1) * pitch;

			u8* tmpBuffer = new u8[pitch];

			for (u32 i = 0; i < tmpImage->getDimension().Height; i += 2)
			{
				memcpy(tmpBuffer, srcA, pitch);
				memcpy(srcA, srcB, pitch);
				memcpy(srcB, tmpBuffer, pitch);
				srcA += pitch;
				srcB -= pitch;
			}

			delete[] tmpBuffer;
		}

		return true;
	}
}
}
