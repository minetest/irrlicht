// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __E_DRIVER_TYPES_H_INCLUDED__
#define __E_DRIVER_TYPES_H_INCLUDED__

#include "irrTypes.h"

namespace irr
{
namespace video
{

	//! An enum for all types of drivers the Irrlicht Engine supports.
	enum E_DRIVER_TYPE
	{
		//! Null driver, useful for applications to run the engine without visualization.
		/** The null device is able to load textures, but does not
		render and display any graphics. */
		EDT_NULL,

		//! The Irrlicht Engine Software renderer.
		/** Runs on all platforms, with every hardware. It should only
		be used for 2d graphics, but it can also perform some primitive
		3d functions. These 3d drawing functions are quite fast, but
		very inaccurate, and don't even support clipping in 3D mode. */
		EDT_SOFTWARE,

		//! The Burning's Software Renderer, an alternative software renderer
		/** Basically it can be described as the Irrlicht Software
		renderer on steroids. It rasterizes 3D geometry perfectly: It
		is able to perform correct 3d clipping, perspective correct
		texture mapping, perspective correct color mapping, and renders
		sub pixel correct, sub texel correct primitives. In addition,
		it does bilinear texel filtering and supports more materials
		than the EDT_SOFTWARE driver. This renderer has been written
		entirely by Thomas Alten, thanks a lot for this huge
		contribution. */
		EDT_BURNINGSVIDEO,

		//! Direct3D8 device is longer supported in Irrlicht. You have to go back to Irrlicht 1.8 if you still need that.
		DEPRECATED_EDT_DIRECT3D8_NO_LONGER_EXISTS, // keep enum to avoid breaking enumeration order (might be used in ini-files, serialization, etc)

		//! Direct3D 9 device, only available on Win32 platforms.
		/** Performs hardware accelerated rendering of 3D and 2D
		primitives. */
		EDT_DIRECT3D9,

		//! OpenGL device, available on most platforms.
		/** Performs hardware accelerated rendering of 3D and 2D
		primitives. */
		EDT_OPENGL,

        //! OpenGL-ES 1.x driver, for embedded and mobile systems
		EDT_OGLES1,

		//! OpenGL-ES 2.x driver, for embedded and mobile systems
		/** Supports shaders etc. */
		EDT_OGLES2,

		//! WebGL1 friendly subset of OpenGL-ES 2.x driver for Emscripten
		EDT_WEBGL1,

		//! No driver, just for counting the elements
		EDT_COUNT
	};

	const c8* const DRIVER_TYPE_NAMES[] =
	{
		"NullDriver",
		"Software Renderer",
		"Burning's Video",
		"Direct3D 8.1",
		"Direct3D 9.0c",
		"OpenGL 1.x/2.x/3.x",
		"OpenGL ES1",
		"OpenGL ES2",
		"WebGL 1",
		0
	};

	const c8* const DRIVER_TYPE_NAMES_SHORT[] =
	{
		"null",
		"software",
		"burning",
		"d3d8",
		"d3d9",
		"opengl",
		"ogles1",
		"ogles2",
		"webgl1",
		0
	};

} // end namespace video
} // end namespace irr


#endif
