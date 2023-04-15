// Copyright (C) 2023 Vitaliy Lobachevskiy
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "Driver.h"

namespace irr {
namespace video {

	E_DRIVER_TYPE COpenGL3Driver::getDriverType() const {
		return EDT_OPENGL3;
	}

	OpenGLVersion COpenGL3Driver::getVersionFromOpenGL() const {
		GLint major, minor, profile;
		glGetIntegerv(GL_MAJOR_VERSION, &major);
		glGetIntegerv(GL_MINOR_VERSION, &minor);
		glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profile);
		// The spec is clear a context can’t be both core and compatibility at the same time.
		// However, the returned value is a mask. Ask Khronos why. -- numzero
		if (profile & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT)
			return {OpenGLSpec::Compat, (u8)major, (u8)minor, 0};
		if (profile & GL_CONTEXT_CORE_PROFILE_BIT)
			return {OpenGLSpec::Core, (u8)major, (u8)minor, 0};
		os::Printer::log("Got unrecognized OpenGL profile", ELL_ERROR);
		return {OpenGLSpec::Core, (u8)major, (u8)minor, 0};
	}

	IVideoDriver* createOpenGL3Driver(const SIrrlichtCreationParameters& params, io::IFileSystem* io, IContextManager* contextManager)
	{
		os::Printer::log("Using COpenGL3Driver", ELL_INFORMATION);
		COpenGL3Driver* driver = new COpenGL3Driver(params, io, contextManager);
		driver->genericDriverInit(params.WindowSize, params.Stencilbuffer);	// don't call in constructor, it uses virtual function calls of driver
		return driver;
	}

}
}
