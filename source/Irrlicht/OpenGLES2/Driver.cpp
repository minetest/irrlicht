// Copyright (C) 2023 Vitaliy Lobachevskiy
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "Driver.h"
#include "OpenGL/Texture.h"
#include "COpenGLCoreCacheHandler.h"

namespace irr {
namespace video {

	E_DRIVER_TYPE COpenGLES2Driver::getDriverType() const {
		return EDT_OGLES2;
	}

	IVideoDriver* createOGLES2Driver(const SIrrlichtCreationParameters& params, io::IFileSystem* io, IContextManager* contextManager)
	{
		COpenGLES2Driver* driver = new COpenGLES2Driver(params, io, contextManager);
		driver->genericDriverInit(params.WindowSize, params.Stencilbuffer);	// don't call in constructor, it uses virtual function calls of driver
		return driver;
	}

	bool COpenGLES2Driver::readTexture(IImage *into, COpenGL3Texture *texture) {
		// GLES (even 3.2) can’t read textures to RAM, but it can read framebuffers to RAM. Use that.

		auto Size = texture->getSize();
		auto ColorFormat = texture->getColorFormat();
		void (*Converter)(const void* sP, s32 sN, void* dP);

		switch (ColorFormat)
		{
		case ECF_A1R5G5B5: Converter = CColorConverter::convert_A8R8G8B8toA1B5G5R5; break;
		case ECF_R5G6B5:   Converter = CColorConverter::convert_A8R8G8B8toR5G6B5;   break;
		case ECF_R8G8B8:   Converter = CColorConverter::convert_A8R8G8B8toB8G8R8;   break;
		case ECF_A8R8G8B8: Converter = CColorConverter::convert_A8R8G8B8toA8B8G8R8; break;
		default: return false;
		}

		GLuint tmpFBO = 0;
		irrGlGenFramebuffers(1, &tmpFBO);
		GLuint prevFBO = 0;
		CacheHandler->getFBO(prevFBO);
		CacheHandler->setFBO(tmpFBO);
		irrGlFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->getOpenGLTextureName(), 0);
		GLenum status = irrGlCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE) {
			CacheHandler->setFBO(prevFBO);
			irrGlDeleteFramebuffers(1, &tmpFBO);
			os::Printer::log("readTexture: Texture format is not supported", ColorFormatNames[ColorFormat < ECF_UNKNOWN?ColorFormat:ECF_UNKNOWN], ELL_ERROR);
			return false;
		}

		IImage* tmpImage = createImage(ECF_A8R8G8B8, Size);
		glReadPixels(0, 0, Size.Width, Size.Height, GL_RGBA, GL_UNSIGNED_BYTE, tmpImage->getData());

		CacheHandler->setFBO(prevFBO);
		irrGlDeleteFramebuffers(1, &tmpFBO);

		Converter(tmpImage->getData(), tmpImage->getDimension().getArea(), into->getData());

		tmpImage->drop();

		return true;
	}
}
}
