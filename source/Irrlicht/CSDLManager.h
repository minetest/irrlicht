// Copyright (C) 2022 sfan5
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __C_SDL_MANAGER_H_INCLUDED__
#define __C_SDL_MANAGER_H_INCLUDED__

#include "IrrCompileConfig.h"

#if defined(_IRR_COMPILE_WITH_SDL_DEVICE_) && defined(_IRR_COMPILE_WITH_OPENGL_)

#include "IContextManager.h"

namespace irr
{
	class CIrrDeviceSDL;

namespace video
{

	// Manager for SDL with OpenGL
	class CSDLManager : public IContextManager
	{
	public:
		CSDLManager(CIrrDeviceSDL* device);

		virtual ~CSDLManager() {}

		bool initialize(const SIrrlichtCreationParameters& params, const SExposedVideoData& data) override;

		void terminate() override {};
		bool generateSurface() override { return true; };
		void destroySurface() override {};
		bool generateContext() override { return true; };
		void destroyContext() override {};

		const SExposedVideoData& getContext() const override;

		bool activateContext(const SExposedVideoData& videoData, bool restorePrimaryOnZero=false) override;

		void* getProcAddress(const std::string &procName) override;

		bool swapBuffers() override;

	private:
		SExposedVideoData Data;
		CIrrDeviceSDL *SDLDevice;
	};
}
}

#endif
#endif
