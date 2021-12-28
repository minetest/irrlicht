// Copyright (C) 2002-2012 Nikolaus Gebhardt / Thomas Alten
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __S_VIDEO_2_SOFTWARE_COMPILE_CONFIG_H_INCLUDED__
#define __S_VIDEO_2_SOFTWARE_COMPILE_CONFIG_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifndef REALINLINE
#ifdef _MSC_VER
#define REALINLINE __forceinline
#else
#define REALINLINE inline
#endif
#endif

//! Size of a static C-style array.
#define array_size(_arr)  ((sizeof(_arr)/sizeof(*_arr)))

//! Compiler Align
#if defined(_MSC_VER)
#define ALIGN(x) __declspec(align(x))
#elif defined(__GNUC__)
#define ALIGN(x) __attribute__ ((aligned(x)))
#else
#define ALIGN(x)
#endif

#endif // __S_VIDEO_2_SOFTWARE_COMPILE_CONFIG_H_INCLUDED__
