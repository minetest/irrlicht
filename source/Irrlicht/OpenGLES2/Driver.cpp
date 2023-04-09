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
		bool passed = true;

		auto Size = texture->getSize();

		// TODO: on ES2 we can likely also work with glCopyTexImage2D instead of rendering which should be faster.
		COpenGL3Texture* tmpTexture = new COpenGL3Texture("OGL_CORE_LOCK_TEXTURE", Size, ETT_2D, texture->getColorFormat(), this);

		GLuint tmpFBO = 0;
		irrGlGenFramebuffers(1, &tmpFBO);

		GLint prevViewportX = 0;
		GLint prevViewportY = 0;
		GLsizei prevViewportWidth = 0;
		GLsizei prevViewportHeight = 0;
		getCacheHandler()->getViewport(prevViewportX, prevViewportY, prevViewportWidth, prevViewportHeight);
		getCacheHandler()->setViewport(0, 0, Size.Width, Size.Height);

		GLuint prevFBO = 0;
		getCacheHandler()->getFBO(prevFBO);
		getCacheHandler()->setFBO(tmpFBO);

		irrGlFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tmpTexture->getOpenGLTextureName(), 0);

		glClear(GL_COLOR_BUFFER_BIT);

		draw2DImage(texture, 0, true);

		IImage* tmpImage = createImage(ECF_A8R8G8B8, Size);
		glReadPixels(0, 0, Size.Width, Size.Height, GL_RGBA, GL_UNSIGNED_BYTE, tmpImage->getData());

		getCacheHandler()->setFBO(prevFBO);
		getCacheHandler()->setViewport(prevViewportX, prevViewportY, prevViewportWidth, prevViewportHeight);

		irrGlDeleteFramebuffers(1, &tmpFBO);
		delete tmpTexture;

		void* src = tmpImage->getData();
		void* dest = into->getData();

		switch (texture->getColorFormat())
		{
		case ECF_A1R5G5B5:
			CColorConverter::convert_A8R8G8B8toA1B5G5R5(src, tmpImage->getDimension().getArea(), dest);
			break;
		case ECF_R5G6B5:
			CColorConverter::convert_A8R8G8B8toR5G6B5(src, tmpImage->getDimension().getArea(), dest);
			break;
		case ECF_R8G8B8:
			CColorConverter::convert_A8R8G8B8toB8G8R8(src, tmpImage->getDimension().getArea(), dest);
			break;
		case ECF_A8R8G8B8:
			CColorConverter::convert_A8R8G8B8toA8B8G8R8(src, tmpImage->getDimension().getArea(), dest);
			break;
		default:
			passed = false;
			break;
		}
		tmpImage->drop();

		return passed;
	}
}
}
