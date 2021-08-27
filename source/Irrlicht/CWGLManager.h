// Copyright (C) 2013 Christian Stehno
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef IRR_C_WGL_MANAGER_H_INCLUDED
#define IRR_C_WGL_MANAGER_H_INCLUDED

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_WGL_MANAGER_

#include "SIrrCreationParameters.h"
#include "SExposedVideoData.h"
#include "IContextManager.h"
#include "SColor.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace irr
{
namespace video
{
    // WGL manager.
    class CWGLManager : public IContextManager
    {
    public:
        //! Constructor.
        CWGLManager();

		//! Destructor
		~CWGLManager();

        // Initialize
        virtual bool initialize(const SIrrlichtCreationParameters& params, const SExposedVideoData& data) IRR_OVERRIDE;

        // Terminate
        virtual void terminate() IRR_OVERRIDE;

        // Create surface.
        virtual bool generateSurface() IRR_OVERRIDE;

        // Destroy surface.
        virtual void destroySurface() IRR_OVERRIDE;

        // Create context.
        virtual bool generateContext() IRR_OVERRIDE;

        // Destroy EGL context.
        virtual void destroyContext() IRR_OVERRIDE;

		//! Get current context
		virtual const SExposedVideoData& getContext() const IRR_OVERRIDE;

		//! Change render context, disable old and activate new defined by videoData
		virtual bool activateContext(const SExposedVideoData& videoData, bool restorePrimaryOnZero) IRR_OVERRIDE;

        // Swap buffers.
        virtual bool swapBuffers() IRR_OVERRIDE;

    private:
        SIrrlichtCreationParameters Params;
		SExposedVideoData PrimaryContext;
        SExposedVideoData CurrentContext;
		s32 PixelFormat;
		PIXELFORMATDESCRIPTOR pfd;
		ECOLOR_FORMAT ColorFormat;
	};
}
}

#endif

#endif
