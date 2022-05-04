// Copyright (C) 2014 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __C_NSOGL_MANAGER_H_INCLUDED__
#define __C_NSOGL_MANAGER_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_NSOGL_MANAGER_

#include "SIrrCreationParameters.h"
#include "SExposedVideoData.h"
#include "IContextManager.h"
#include "SColor.h"

#import <AppKit/NSOpenGL.h>

namespace irr
{
namespace video
{
    // NSOpenGL manager.
    class CNSOGLManager : public IContextManager
    {
    public:
        //! Constructor.
        CNSOGLManager();

		//! Destructor
		~CNSOGLManager();

        // Initialize
        bool initialize(const SIrrlichtCreationParameters& params, const SExposedVideoData& data) IRR_OVERRIDE;

        // Terminate
        void terminate() IRR_OVERRIDE;

        // Create surface.
        bool generateSurface() IRR_OVERRIDE;

        // Destroy surface.
        void destroySurface() IRR_OVERRIDE;

        // Create context.
        bool generateContext() IRR_OVERRIDE;

        // Destroy EGL context.
        void destroyContext() IRR_OVERRIDE;

		//! Get current context
		const SExposedVideoData& getContext() const IRR_OVERRIDE;

		//! Change render context, disable old and activate new defined by videoData
		bool activateContext(const SExposedVideoData& videoData, bool restorePrimaryOnZero) IRR_OVERRIDE;

        // Swap buffers.
        bool swapBuffers() IRR_OVERRIDE;

    private:
        SIrrlichtCreationParameters Params;
		SExposedVideoData PrimaryContext;
        SExposedVideoData CurrentContext;

        NSOpenGLPixelFormat* PixelFormat;
	};
}
}

#endif

#endif
