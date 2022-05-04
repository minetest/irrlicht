// Copyright (C) 2008-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_I_VERTEX_BUFFER_H_INCLUDED
#define IRR_I_VERTEX_BUFFER_H_INCLUDED

#include "IReferenceCounted.h"
#include "irrArray.h"
#include "EHardwareBufferFlags.h"
#include "S3DVertex.h"

namespace irr
{
namespace scene
{

	class IVertexBuffer : public virtual IReferenceCounted
	{
	public:
		//! Pointer to first element of vertex data
		virtual void* getData() =0;

		//! Const pointer to first element
		virtual const void* getData() const =0;

		//! Same as getData.
		virtual void* pointer() { return getData(); }

		virtual video::E_VERTEX_TYPE getType() const =0;
		virtual void setType(video::E_VERTEX_TYPE vertexType) =0;

		//! Number of bytes per element
		virtual u32 stride() const =0;

		//! Number of elements
		virtual u32 size() const =0;

		//! Add vertex to end. 
		//* Note that if you pass another type than the currently used vertex type then information can be lost */
		virtual void push_back(const video::S3DVertex &element) =0;
		virtual void push_back(const video::S3DVertex2TCoords &element) =0;
		virtual void push_back(const video::S3DVertexTangents &element) =0;

		//! Set value at index. Buffer must be already large enough that element exists.
		//* Note that if you pass another type than the currently used vertex type then information can be lost */
		virtual void setValue(u32 index, const video::S3DVertex &value) =0;
		virtual void setValue(u32 index, const video::S3DVertex2TCoords &value) =0;
		virtual void setValue(u32 index, const video::S3DVertexTangents &value) =0;

		//! Direct access to elements. Risky to use!
		/** The reference _must_ be cast to the correct type before use. It's only video::S3DVertex if getType is EVT_STANDARD.
		    otherwise cast it first to a reference type derived from S3DVertex like S3DVertex2TCoords& or S3DVertexTangents&. */
		virtual video::S3DVertex& operator [](u32 index) = 0;
		virtual video::S3DVertex& operator [](const u32 index) const =0;
		virtual video::S3DVertex& getLast() =0;

		virtual void set_used(u32 usedNow) =0;
		virtual void reallocate(u32 new_size, bool canShrink=true) =0;
		virtual u32 allocated_size() const =0;

		//! get the current hardware mapping hint
		virtual E_HARDWARE_MAPPING getHardwareMappingHint() const =0;

		//! set the hardware mapping hint, for driver
		virtual void setHardwareMappingHint( E_HARDWARE_MAPPING NewMappingHint ) =0;

		//! flags the meshbuffer as changed, reloads hardware buffers
		virtual void setDirty() =0;

		//! Get the currently used ID for identification of changes.
		/** This shouldn't be used for anything outside the VideoDriver. */
		virtual u32 getChangedID() const = 0;
	};


} // end namespace scene
} // end namespace irr

#endif
