// Copyright (C) 2023 Vitaliy Lobachevskiy
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#pragma once
#include "OpenGL/Driver.h"

namespace irr {
namespace video {

	class COpenGLES2Driver : public COpenGL3DriverBase {
		friend IVideoDriver* createOGLES2Driver(const SIrrlichtCreationParameters& params, io::IFileSystem* io, IContextManager* contextManager);
	public:
		using COpenGL3DriverBase::COpenGL3DriverBase;
		E_DRIVER_TYPE getDriverType() const override;
	};

}
}
