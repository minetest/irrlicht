#include "COpenGLWaylandDriver.h"

#ifdef _IRR_COMPILE_WITH_WAYLAND_DEVICE_

namespace irr
{
namespace video
{

bool COpenGLWaylandDriver::initDriver(CIrrDeviceWayland *device)
{
	ExposedData.OpenGLWayland.eglContext = device->getCurrentContext();
	ExposedData.OpenGLWayland.eglDisplay = device->getCurrentDisplay();
	ExposedData.OpenGLWayland.eglSurface = device->getCurrentSurface();
	mDrawable = eglGetCurrentSurface(EGL_DRAW);
	mDisplay = ExposedData.OpenGLWayland.eglDisplay;

	genericDriverInit();

	// set vsync
	eglSwapInterval(mDisplay, Params.Vsync ? 1 : 0);
	return true;
}

bool COpenGLWaylandDriver::changeRenderContext(const SExposedVideoData& videoData, CIrrDeviceLinux* device)
{
	if (videoData.OpenGLWayland.eglSurface)
	{
		if (videoData.OpenGLWayland.eglDisplay && videoData.OpenGLWayland.eglDisplay) {
			if (!eglMakeCurrent(videoData.OpenGLWayland.eglDisplay, videoData.OpenGLWayland.eglSurface, videoData.OpenGLWayland.eglSurface, videoData.OpenGLWayland.eglContext))
			{
				os::Printer::log("Render Context switch failed.");
				return false;
			}
			else
			{
				mDrawable = videoData.OpenGLWayland.eglSurface;
				mDisplay = videoData.OpenGLWayland.eglDisplay;
			}
		}
		else
		{
			// in case we only got a window ID, try with the existing values for display and context
			if (!eglMakeCurrent(ExposedData.OpenGLWayland.eglDisplay, videoData.OpenGLWayland.eglSurface, videoData.OpenGLWayland.eglSurface, ExposedData.OpenGLWayland.eglContext))
			{
				os::Printer::log("Render Context switch failed.");
				return false;
			}
			else
			{
				mDrawable = videoData.OpenGLWayland.eglSurface;
				mDisplay = ExposedData.OpenGLWayland.eglDisplay;
			}
		}
	}
	// set back to main context
	else if (mDisplay != ExposedData.OpenGLWayland.eglDisplay)
	{
		if (!eglMakeCurrent(ExposedData.OpenGLWayland.eglDisplay, ExposedData.OpenGLWayland.eglSurface, ExposedData.OpenGLWayland.eglSurface, ExposedData.OpenGLWayland.eglContext))
		{
			os::Printer::log("Render Context switch failed.");
			return false;
		}
		else
		{
			mDrawable = ExposedData.OpenGLWayland.eglSurface;
			mDisplay = videoData.OpenGLWayland.eglDisplay;
		}
	}
	return true;
}

//! init call for rendering start
bool COpenGLWaylandDriver::beginScene(bool backBuffer, bool zBuffer, SColor color,
		const SExposedVideoData& videoData, core::rect<s32>* sourceRect)
{
	CNullDriver::beginScene(backBuffer, zBuffer, color, videoData, sourceRect);

	switch (DeviceType)
	{
	case EIDT_X11:
		changeRenderContext(videoData, X11Device);
		break;
	default:
		changeRenderContext(videoData, nullptr);
		break;
	}

	clearBuffers(backBuffer, zBuffer, false, color);
	return true;
}

bool COpenGLWaylandDriver::endScene()
{
	CNullDriver::endScene();

	if (DeviceType == EIDT_X11)
	{
		eglSwapBuffers(mDisplay, mDrawable);
		return true;
	}

	return false;
}

}
}
#endif