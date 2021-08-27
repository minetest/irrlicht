// Copyright (C) 2002-2012 Nikolaus Gebhardt / Thomas Alten
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_C_Z_BUFFER_H_INCLUDED
#define IRR_C_Z_BUFFER_H_INCLUDED

#include "IDepthBuffer.h"

namespace irr
{
namespace video
{

	class CDepthBuffer : public IDepthBuffer
	{
	public:

		//! constructor
		CDepthBuffer(const core::dimension2d<u32>& size);

		//! destructor
		virtual ~CDepthBuffer();

		//! clears the zbuffer
		virtual void clear(f32 value, const interlaced_control interlaced) IRR_OVERRIDE;

		//! sets the new size of the zbuffer
		virtual void setSize(const core::dimension2d<u32>& size) IRR_OVERRIDE;

		//! returns the size of the zbuffer
		virtual const core::dimension2d<u32>& getSize() const IRR_OVERRIDE;

		//! locks the zbuffer
		virtual void* lock() IRR_OVERRIDE { return (void*) Buffer; }

		//! unlocks the zbuffer
		virtual void unlock() IRR_OVERRIDE {}

		//! returns pitch of depthbuffer (in bytes)
		virtual u32 getPitch() const IRR_OVERRIDE { return Pitch; }


	private:

		u8* Buffer;
		core::dimension2d<u32> Size;
		u32 Pitch;
	};


	class CStencilBuffer : public IStencilBuffer
	{
	public:

		//! constructor
		CStencilBuffer(const core::dimension2d<u32>& size, unsigned bit);

		//! destructor
		virtual ~CStencilBuffer();

		//! clears the zbuffer
		virtual void clear(u32 value, const interlaced_control interlaced) IRR_OVERRIDE;

		//! sets the new size of the zbuffer
		virtual void setSize(const core::dimension2d<u32>& size) IRR_OVERRIDE;

		//! returns the size of the zbuffer
		virtual const core::dimension2d<u32>& getSize() const IRR_OVERRIDE;

		//! locks the zbuffer
		virtual void* lock() IRR_OVERRIDE { return (void*) Buffer; }

		//! unlocks the zbuffer
		virtual void unlock() IRR_OVERRIDE {}

		//! returns pitch of depthbuffer (in bytes)
		virtual u32 getPitch() const IRR_OVERRIDE { return Pitch; }


	private:
		u8* Buffer;
		core::dimension2d<u32> Size;
		u32 Pitch;
		u32 Bit;
	};

} // end namespace video
} // end namespace irr

#endif
