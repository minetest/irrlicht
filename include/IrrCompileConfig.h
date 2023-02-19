// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_COMPILE_CONFIG_H_INCLUDED__
#define __IRR_COMPILE_CONFIG_H_INCLUDED__

//! Identifies the IrrlichtMt fork customized for the Minetest engine
#define IRRLICHT_VERSION_MT_REVISION 9
#define IRRLICHT_VERSION_MT "mt9"

//! Irrlicht SDK Version
#define IRRLICHT_VERSION_MAJOR 1
#define IRRLICHT_VERSION_MINOR 9
#define IRRLICHT_VERSION_REVISION 0
// This flag will be defined only in SVN, the official release code will have
// it undefined
#define IRRLICHT_VERSION_SVN alpha
#define IRRLICHT_SDK_VERSION "1.9.0" IRRLICHT_VERSION_MT

#include <stdio.h> // TODO: Although included elsewhere this is required at least for mingw

//! The defines for different operating system are:
//! _IRR_WINDOWS_ for all irrlicht supported Windows versions
//! _IRR_WINDOWS_API_ for Windows or XBox
//! _IRR_LINUX_PLATFORM_ for Linux (it is defined here if no other os is defined)
//! _IRR_SOLARIS_PLATFORM_ for Solaris
//! _IRR_OSX_PLATFORM_ for Apple systems running OSX
//! _IRR_IOS_PLATFORM_ for Apple devices running iOS
//! _IRR_ANDROID_PLATFORM_ for devices running Android
//! _IRR_POSIX_API_ for Posix compatible systems
//! Note: PLATFORM defines the OS specific layer, API can group several platforms

//! DEVICE is the windowing system used, several PLATFORMs support more than one DEVICE
//! Irrlicht can be compiled with more than one device
//! _IRR_COMPILE_WITH_WINDOWS_DEVICE_ for Windows API based device
//! _IRR_COMPILE_WITH_OSX_DEVICE_ for Cocoa native windowing on OSX
//! _IRR_COMPILE_WITH_X11_DEVICE_ for Linux X11 based device
//! _IRR_COMPILE_WITH_SDL_DEVICE_ for platform independent SDL framework

//! Passing defines to the compiler which have NO in front of the _IRR definename is an alternative
//! way which can be used to disable defines (instead of outcommenting them in this header).
//! So defines can be controlled from Makefiles or Projectfiles which allows building
//! different library versions without having to change the sources.
//! Example: NO_IRR_COMPILE_WITH_X11_ would disable X11

//! Uncomment this line to compile with the SDL device
//#define _IRR_COMPILE_WITH_SDL_DEVICE_
#ifdef NO_IRR_COMPILE_WITH_SDL_DEVICE_
#undef _IRR_COMPILE_WITH_SDL_DEVICE_
#endif

//! WIN32 for Windows32
//! WIN64 for Windows64
// The windows platform and API support SDL and WINDOW device
#if defined(_WIN32) || defined(_WIN64)
#define _IRR_WINDOWS_
#define _IRR_WINDOWS_API_
#ifndef _IRR_COMPILE_WITH_SDL_DEVICE_
#define _IRR_COMPILE_WITH_WINDOWS_DEVICE_
#endif
#endif

#if defined(__APPLE__)
#if defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__) || defined(__IPHONE_OS_VERSION_MIN_REQUIRED)
#define _IRR_IOS_PLATFORM_
#define _IRR_COMPILE_WITH_IOS_DEVICE_
#define NO_IRR_COMPILE_WITH_OPENGL_
// The application state events and following methods: IrrlichtDevice::isWindowActive, IrrlichtDevice::isWindowFocused,
// IrrlichtDevice::isWindowMinimized works out of box only if you'll use built-in CIrrDelegateiOS,
// so _IRR_COMPILE_WITH_IOS_BUILTIN_MAIN_ must be enabled in this case. If you need a custom UIApplicationDelegate
// you must disable _IRR_COMPILE_WITH_IOS_BUILTIN_MAIN_ definition and handle all application events yourself.
#define _IRR_COMPILE_WITH_IOS_BUILTIN_MAIN_
#else
#define _IRR_OSX_PLATFORM_
#ifndef _IRR_COMPILE_WITH_SDL_DEVICE_
#define _IRR_COMPILE_WITH_OSX_DEVICE_
#endif
#define NO_IRR_COMPILE_WITH_OGLES1_
#define NO_IRR_COMPILE_WITH_OGLES2_
#define NO_IRR_COMPILE_WITH_WEBGL1_
#endif
#endif

#if defined(__EMSCRIPTEN__)
#define _IRR_EMSCRIPTEN_PLATFORM_
#define NO_IRR_COMPILE_WITH_JOYSTICK_EVENTS_
#define NO_IRR_COMPILE_WITH_OPENGL_
#define NO_IRR_COMPILE_WITH_OGLES1_
#define _IRR_COMPILE_WITH_OGLES2_
#define _IRR_COMPILE_WITH_WEBGL1_
#define _IRR_COMPILE_WITH_EGL_MANAGER_
#define _IRR_COMPILE_WITH_SDL_DEVICE_
#define NO_IRR_COMPILE_WITH_X11_DEVICE_
#define _IRR_LINUX_PLATFORM_	// emscripten basically working like a unix
#endif // __EMSCRIPTEN__

#if defined(__ANDROID__)
#define _IRR_ANDROID_PLATFORM_
#define _IRR_COMPILE_WITH_ANDROID_DEVICE_
#define _IRR_COMPILE_ANDROID_ASSET_READER_
#define NO_IRR_COMPILE_WITH_OPENGL_
#endif

#if defined(__SVR4) && defined(__sun)
#define _IRR_SOLARIS_PLATFORM_
#if defined(__sparc)
	#define __BIG_ENDIAN__
#endif
#endif

#if !defined(_IRR_WINDOWS_API_) && !defined(_IRR_OSX_PLATFORM_) && !defined(_IRR_IOS_PLATFORM_) && !defined(_IRR_ANDROID_PLATFORM_) && !defined(_IRR_EMSCRIPTEN_PLATFORM_)
#ifndef _IRR_SOLARIS_PLATFORM_
#define _IRR_LINUX_PLATFORM_
#endif
#define _IRR_POSIX_API_
#ifndef _IRR_COMPILE_WITH_SDL_DEVICE_
#define _IRR_COMPILE_WITH_X11_DEVICE_
#endif
#endif


//! Define _IRR_COMPILE_WITH_JOYSTICK_SUPPORT_ if you want joystick events.
#if !(defined(__FreeBSD__) || defined(__OpenBSD__))
#define _IRR_COMPILE_WITH_JOYSTICK_EVENTS_
#endif
#ifdef NO_IRR_COMPILE_WITH_JOYSTICK_EVENTS_
#undef _IRR_COMPILE_WITH_JOYSTICK_EVENTS_
#endif


//! Maximum number of texture an SMaterial can have, up to 8 are supported by Irrlicht.
#define _IRR_MATERIAL_MAX_TEXTURES_ 4

//! Define _IRR_COMPILE_WITH_OPENGL_ to compile the Irrlicht engine with OpenGL.
/** If you do not wish the engine to be compiled with OpenGL, comment this
define out. */
#define _IRR_COMPILE_WITH_OPENGL_
#ifdef NO_IRR_COMPILE_WITH_OPENGL_
#undef _IRR_COMPILE_WITH_OPENGL_
#endif

//! Define required options for OpenGL drivers.
#if defined(_IRR_COMPILE_WITH_OPENGL_)
	#if defined(_IRR_COMPILE_WITH_WINDOWS_DEVICE_)
		#define _IRR_OPENGL_USE_EXTPOINTER_
		#define _IRR_COMPILE_WITH_WGL_MANAGER_
	#elif defined(_IRR_COMPILE_WITH_X11_DEVICE_)
		#define _IRR_OPENGL_USE_EXTPOINTER_
		#define _IRR_COMPILE_WITH_GLX_MANAGER_
	#elif defined(_IRR_COMPILE_WITH_OSX_DEVICE_)
		#define _IRR_COMPILE_WITH_NSOGL_MANAGER_
	#elif defined(_IRR_SOLARIS_PLATFORM_)
		#define _IRR_COMPILE_WITH_GLX_MANAGER_
	#elif defined(_IRR_COMPILE_WITH_SDL_DEVICE_)
		#define _IRR_OPENGL_USE_EXTPOINTER_
	#endif
#endif

//! Define _IRR_COMPILE_WITH_OGLES1_ to compile the Irrlicht engine with OpenGL ES 1.1.
/** If you do not wish the engine to be compiled with OpenGL ES 1.1, comment this
define out.
Depending on platform you may have to enable _IRR_OGLES1_USE_KHRONOS_API_HEADERS_ as well when using it.
*/
#if defined(_IRR_ANDROID_PLATFORM_) || defined(_IRR_IOS_PLATFORM_)
#define _IRR_COMPILE_WITH_OGLES1_
#endif
#ifdef NO_IRR_COMPILE_WITH_OGLES1_
#undef _IRR_COMPILE_WITH_OGLES1_
#endif

#ifdef _IRR_COMPILE_WITH_OGLES1_
//! Define _IRR_OGLES1_USE_KHRONOS_API_HEADERS_ to use the OpenGL ES headers from the Debian Khronos-api package
//#define _IRR_OGLES1_USE_KHRONOS_API_HEADERS_
#endif

//! Define required options for OpenGL ES 1.1 drivers.
#if defined(_IRR_COMPILE_WITH_OGLES1_)
#if defined(_IRR_COMPILE_WITH_WINDOWS_DEVICE_) || defined(_IRR_COMPILE_WITH_X11_DEVICE_) || defined(_IRR_COMPILE_WITH_ANDROID_DEVICE_)
#define _IRR_OGLES1_USE_EXTPOINTER_
#ifndef _IRR_COMPILE_WITH_EGL_MANAGER_
#define _IRR_COMPILE_WITH_EGL_MANAGER_
#endif
#elif defined(_IRR_COMPILE_WITH_IOS_DEVICE_)
#ifndef _IRR_COMPILE_WITH_EAGL_MANAGER_
#define _IRR_COMPILE_WITH_EAGL_MANAGER_
#endif
#endif
#endif

//! Define _IRR_COMPILE_WITH_OGLES2_ to compile the Irrlicht engine with OpenGL ES 2.0.
/** If you do not wish the engine to be compiled with OpenGL ES 2.0, comment this
define out. */
#if defined(_IRR_ANDROID_PLATFORM_) || defined(_IRR_IOS_PLATFORM_)
#define _IRR_COMPILE_WITH_OGLES2_
#else
//#define _IRR_COMPILE_WITH_OGLES2_
#endif
#ifdef NO_IRR_COMPILE_WITH_OGLES2_
#undef _IRR_COMPILE_WITH_OGLES2_
#endif

//! Define _IRR_COMPILE_WITH_WEBGL1_ to compile Irrlicht engine with a WebGL friendly
//! subset of the OpenGL ES 2.0 driver.
//#define _IRR_COMPILE_WITH_WEBGL1_
#ifdef NO_IRR_COMPILE_WITH_WEBGL1_
#undef _IRR_COMPILE_WITH_WEBGL1_
#endif
#ifdef _IRR_COMPILE_WITH_WEBGL1_
#define _IRR_COMPILE_WITH_OGLES2_ //  it's a subset of OGL ES2, so always needed when using WebGL
#endif

//! Define required options for OpenGL ES 2.0 drivers.
#if defined(_IRR_COMPILE_WITH_OGLES2_)
#if defined(_IRR_COMPILE_WITH_WINDOWS_DEVICE_) || defined(_IRR_COMPILE_WITH_X11_DEVICE_) || defined(_IRR_COMPILE_WITH_ANDROID_DEVICE_) || defined(__EMSCRIPTEN__)
#define _IRR_OGLES2_USE_EXTPOINTER_
#ifndef _IRR_COMPILE_WITH_EGL_MANAGER_
#define _IRR_COMPILE_WITH_EGL_MANAGER_
#endif
#elif defined(_IRR_COMPILE_WITH_IOS_DEVICE_)
#ifndef _IRR_COMPILE_WITH_EAGL_MANAGER_
#define _IRR_COMPILE_WITH_EAGL_MANAGER_
#endif
#endif
#endif



//! Define _IRR_COMPILE_WITH_X11_ to compile the Irrlicht engine with X11 support.
/** If you do not wish the engine to be compiled with X11, comment this
define out. */
// Only used in LinuxDevice.
#define _IRR_COMPILE_WITH_X11_
#ifdef NO_IRR_COMPILE_WITH_X11_
#undef _IRR_COMPILE_WITH_X11_
#endif

#if defined(_IRR_LINUX_PLATFORM_) && defined(_IRR_COMPILE_WITH_X11_)
//! XInput2 (library called Xi) is currently only used to support touch-input.
#define _IRR_LINUX_X11_XINPUT2_
#ifdef NO_IRR_LINUX_X11_XINPUT2_
#undef _IRR_LINUX_X11_XINPUT2_
#endif

//! X11 has by default only monochrome cursors, but using the Xcursor library we can also get color cursor support.
//! If you have the need for custom color cursors on X11 then enable this and make sure you also link
//! to the Xcursor library in your Makefile/Projectfile.
//#define _IRR_LINUX_XCURSOR_
#ifdef NO_IRR_LINUX_XCURSOR_
#undef _IRR_LINUX_XCURSOR_
#endif

#endif


#if defined(_IRR_SOLARIS_PLATFORM_)
	#undef _IRR_COMPILE_WITH_JOYSTICK_EVENTS_
#endif


#ifdef _IRR_WINDOWS_API_

// To build Irrlicht as a static library, you must define _IRR_STATIC_LIB_ in both the
// Irrlicht build, *and* in the user application, before #including <irrlicht.h>
#ifndef _IRR_STATIC_LIB_
#ifdef IRRLICHT_EXPORTS
#define IRRLICHT_API __declspec(dllexport)
#else
#define IRRLICHT_API __declspec(dllimport)
#endif // IRRLICHT_EXPORT
#else
#define IRRLICHT_API
#endif // _IRR_STATIC_LIB_

// Declare the calling convention.
#if defined(_STDCALL_SUPPORTED)
#define IRRCALLCONV __stdcall
#else
#define IRRCALLCONV __cdecl
#endif // STDCALL_SUPPORTED

#else // _IRR_WINDOWS_API_

// Force symbol export in shared libraries built with gcc.
#if defined(__GNUC__) && !defined(_IRR_STATIC_LIB_) && defined(IRRLICHT_EXPORTS)
#define IRRLICHT_API __attribute__ ((visibility("default")))
#else
#define IRRLICHT_API
#endif

#define IRRCALLCONV

#endif // _IRR_WINDOWS_API_


#define _IRR_COMPILE_WITH_GUI_
#define _IRR_COMPILE_WITH_LIBJPEG_
#define _IRR_COMPILE_WITH_LIBPNG_
#define _IRR_COMPILE_WITH_BILLBOARD_SCENENODE_
#define _IRR_COMPILE_WITH_SKINNED_MESH_SUPPORT_
#define _IRR_COMPILE_WITH_B3D_LOADER_
#define _IRR_COMPILE_WITH_X_LOADER_
#define _IRR_COMPILE_WITH_OBJ_LOADER_
#define _IRR_COMPILE_WITH_BMP_LOADER_
#define _IRR_COMPILE_WITH_JPG_LOADER_
#define _IRR_COMPILE_WITH_PNG_LOADER_
#define _IRR_COMPILE_WITH_TGA_LOADER_
#define _IRR_COMPILE_WITH_JPG_WRITER_
#define _IRR_COMPILE_WITH_PNG_WRITER_
#define __IRR_COMPILE_WITH_ZIP_ARCHIVE_LOADER_
#define _IRR_COMPILE_WITH_ZLIB_
#define __IRR_HAS_S64


#ifdef _DEBUG
	//! A few attributes are written in CSceneManager when _IRR_SCENEMANAGER_DEBUG is enabled
	// NOTE: Those attributes were used always until 1.8.0 and became a global define for 1.8.1
	// which is only enabled in debug because it had a large (sometimes >5%) impact on speed.
	// A better solution in the long run is to break the interface and remove _all_ attribute
	// access in functions like CSceneManager::drawAll and instead put that information in some
	// own struct/class or in CSceneManager.
	// See http://irrlicht.sourceforge.net/forum/viewtopic.php?f=2&t=48211 for the discussion.
	#define _IRR_SCENEMANAGER_DEBUG
	#ifdef NO_IRR_SCENEMANAGER_DEBUG
		#undef _IRR_SCENEMANAGER_DEBUG
	#endif
#endif

#endif // __IRR_COMPILE_CONFIG_H_INCLUDED__
