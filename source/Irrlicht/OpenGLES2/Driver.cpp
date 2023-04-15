// Copyright (C) 2023 Vitaliy Lobachevskiy
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "Driver.h"

namespace irr {
namespace video {

	E_DRIVER_TYPE COpenGLES2Driver::getDriverType() const {
		return EDT_OGLES2;
	}

	OpenGLVersion COpenGLES2Driver::getVersionFromOpenGL() const {
		auto version_string = reinterpret_cast<const char *>(glGetString(GL_VERSION));
		int major, minor;
		if (sscanf(version_string, "OpenGL ES %d.%d", &major, &minor) != 2) {
			os::Printer::log("Failed to parse OpenGL ES version string", version_string, ELL_ERROR);
			return {OpenGLSpec::ES, 0, 0, 0};
		}
		return {OpenGLSpec::ES, (u8)major, (u8)minor, 0};
	}

	IVideoDriver* createOGLES2Driver(const SIrrlichtCreationParameters& params, io::IFileSystem* io, IContextManager* contextManager)
	{
		os::Printer::log("Using COpenGLES2Driver", ELL_INFORMATION);
		COpenGLES2Driver* driver = new COpenGLES2Driver(params, io, contextManager);
		driver->genericDriverInit(params.WindowSize, params.Stencilbuffer);	// don't call in constructor, it uses virtual function calls of driver
		return driver;
	}

}
}
