// Copyright (C) 2008-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_I_INDEX_BUFFER_H_INCLUDED
#define IRR_I_INDEX_BUFFER_H_INCLUDED

#include "IReferenceCounted.h"
#include "EHardwareBufferFlags.h"
#include "SVertexIndex.h"

namespace irr
{

namespace scene
{

	class IIndexBuffer : public virtual IReferenceCounted
	{
	public:

		//! Pointer to first element
		virtual void* getData() =0;

		//! Const pointer to first element
		virtual const void* getData() const =0;

		//! Same as getData(), just closer to core::array interface
		void* pointer() { return getData(); }

		virtual video::E_INDEX_TYPE getType() const =0;

		//! Change between 16 and 32 bit indices.
		/** This copies all indices to a new buffer of corresponding type.
		Be careful - going from 32 to 16 bit will only  work correctly 
		if none of your indices is larger than 16 bit. */
		virtual void setType(video::E_INDEX_TYPE IndexType) =0;

		//! Number of bytes per element
		virtual u32 stride() const =0;

		//! Number of elements
		virtual u32 size() const =0;

		//! Add value to end. Note that for 16 bit index types values shouldn't be larger than u16
		virtual void push_back(u32 value) =0;

		//! Set value at index. Note that for 16 bit index types values shouldn't be larger than u16
		/** Buffer must be already large enough. This is basically the non const version of operator [] */
		virtual void setValue(u32 index, u32 value) =0;

		//! Access element value at given index
		virtual u32 operator [](u32 index) const =0;
		virtual u32 getLast() =0;

		virtual void set_used(u32 usedNow) =0;
		virtual void reallocate(u32 new_size, bool canShrink=true) =0;
		virtual u32 allocated_size() const=0;

		//! get the current hardware mapping hint
		virtual E_HARDWARE_MAPPING getHardwareMappingHint() const =0;

		//! set the hardware mapping hint, for driver
		virtual void setHardwareMappingHint( E_HARDWARE_MAPPING NewMappingHint ) =0;

		//! flags the meshbuffer as changed, reloads hardware buffers
		virtual void setDirty() = 0;

		//! Get the currently used ID for identification of changes.
		/** This shouldn't be used for anything outside the VideoDriver. */
		virtual u32 getChangedID() const = 0;
	};


} // end namespace scene
} // end namespace irr

#endif
