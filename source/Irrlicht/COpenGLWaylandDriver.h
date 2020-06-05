// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __C_VIDEO_OPEN_GL_WAYLAND_H_INCLUDED__
#define __C_VIDEO_OPEN_GL_WAYLAND_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_WAYLAND_DEVICE_

#include "COpenGLDriver.h"
#include "SIrrCreationParameters.h"
#include "IFileSystem.h"
#include "CIrrDeviceWayland.h"

namespace irr
{

namespace video
{
	class COpenGLWaylandDriver : public COpenGLDriver
	{
	public:
		COpenGLWaylandDriver(const SIrrlichtCreationParameters& params,
		io::IFileSystem* io, CIrrDeviceWayland* device): COpenGLDriver(params, io, device) {}
		virtual ~COpenGLWaylandDriver() {}

		virtual bool initDriver(CIrrDeviceWayland* device);
		virtual bool changeRenderContext(const SExposedVideoData& videoData, CIrrDeviceLinux* device);

		//! clears the zbuffer
		virtual bool beginScene(bool backBuffer=true, bool zBuffer=true,
				SColor color=SColor(255,0,0,0),
				const SExposedVideoData& videoData=SExposedVideoData(),
				core::rect<s32>* sourceRect=0);

		virtual bool endScene();
	protected:
		//! inits the parts of the open gl driver used on all platforms
		virtual bool genericDriverInit();

	private:
		//! inits the parts of the open gl driver used on all platforms
		//virtual bool genericDriverInit();

		EGLContext mContext = nullptr;
		EGLDisplay mDisplay = nullptr;
		EGLSurface mDrawable = nullptr;
	};

} // end namespace video
} // end namespace irr


#endif // _IRR_COMPILE_WITH_WAYLAND_DEVICE_
#endif

