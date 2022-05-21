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

		virtual bool initialize(const SIrrlichtCreationParameters& params, const SExposedVideoData& data) _IRR_OVERRIDE_;

		virtual void terminate() _IRR_OVERRIDE_ {};
		virtual bool generateSurface() _IRR_OVERRIDE_ { return true; };
		virtual void destroySurface() _IRR_OVERRIDE_ {};
		virtual bool generateContext() _IRR_OVERRIDE_ { return true; };
		virtual void destroyContext() _IRR_OVERRIDE_ {};

		virtual const SExposedVideoData& getContext() const _IRR_OVERRIDE_;

		virtual bool activateContext(const SExposedVideoData& videoData, bool restorePrimaryOnZero=false) _IRR_OVERRIDE_;

		virtual void* getProcAddress(const std::string &procName) _IRR_OVERRIDE_;

		virtual bool swapBuffers() _IRR_OVERRIDE_;

	private:
		SExposedVideoData Data;
		CIrrDeviceSDL *SDLDevice;
	};
}
}

#endif
#endif
