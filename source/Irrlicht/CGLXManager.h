// Copyright (C) 2013 Christian Stehno
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef IRR_C_GLX_MANAGER_H_INCLUDED
#define IRR_C_GLX_MANAGER_H_INCLUDED

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_GLX_MANAGER_

#include "SIrrCreationParameters.h"
#include "SExposedVideoData.h"
#include "IContextManager.h"
#include "SColor.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>

// we can't include glx.h here, because gl.h has incompatible types with ogl es headers and it
// cause redefinition errors, thats why we use ugly trick with void* types and casts.

namespace irr
{
namespace video
{
    // GLX manager.
    class CGLXManager : public IContextManager
    {
    public:
        //! Constructor.
        CGLXManager(const SIrrlichtCreationParameters& params, const SExposedVideoData& videodata, int screennr);

        //! Destructor
        ~CGLXManager();

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

        // Destroy context.
        virtual void destroyContext() IRR_OVERRIDE;

        //! Get current context
        virtual const SExposedVideoData& getContext() const IRR_OVERRIDE;

        //! Change render context, disable old and activate new defined by videoData
        virtual bool activateContext(const SExposedVideoData& videoData, bool restorePrimaryOnZero) IRR_OVERRIDE;

        // Swap buffers.
        virtual bool swapBuffers() IRR_OVERRIDE;

        XVisualInfo* getVisual() const {return VisualInfo;} // return XVisualInfo

    private:
        SIrrlichtCreationParameters Params;
        SExposedVideoData PrimaryContext;
        SExposedVideoData CurrentContext;
        XVisualInfo* VisualInfo;
        void* glxFBConfig; // GLXFBConfig
        XID GlxWin; // GLXWindow
	};
}
}

#endif

#endif
