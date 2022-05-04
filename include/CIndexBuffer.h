// Copyright (C) 2008-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_C_INDEX_BUFFER_H_INCLUDED
#define IRR_C_INDEX_BUFFER_H_INCLUDED

#include "IIndexBuffer.h"

namespace irr
{
namespace scene
{

	class CIndexBuffer : public IIndexBuffer
	{
		// Virtual function wrapper around irr::core::array
		class IIndexList
		{
		public:
			virtual ~IIndexList(){};

			virtual u32 stride() const =0;
			virtual u32 size() const =0;
			virtual void push_back(u32 value) =0;
			virtual u32 operator [](u32 index) const =0;
			virtual u32 getLast() =0;
			virtual void setValue(u32 index, u32 value) =0;
			virtual void set_used(u32 usedNow) =0;
			virtual void reallocate(u32 new_size, bool canShrink=true) =0;
			virtual u32 allocated_size() const =0;
			virtual void* pointer() =0;
			virtual const void* const_pointer() const =0;
			virtual video::E_INDEX_TYPE getType() const =0;
		};

		template <class T>
		class CSpecificIndexList : public IIndexList
		{
		public:
			core::array<T> Indices;

			virtual u32 stride() const IRR_OVERRIDE {return sizeof(T);}

			virtual u32 size() const IRR_OVERRIDE {return Indices.size();}

			virtual void push_back(u32 value) IRR_OVERRIDE
			{
				Indices.push_back((T)value);
			}

			virtual u32 operator [](u32 index) const IRR_OVERRIDE
			{
				return (u32)(Indices[index]);
			}

			virtual u32 getLast() IRR_OVERRIDE {return (u32)Indices.getLast();}

			virtual void setValue(u32 index, u32 value) IRR_OVERRIDE
			{
				Indices[index]=(T)value;
			}

			virtual void set_used(u32 usedNow) IRR_OVERRIDE
			{
				Indices.set_used(usedNow);
			}

			virtual void reallocate(u32 new_size, bool canShrink) IRR_OVERRIDE
			{
				Indices.reallocate(new_size, canShrink);
			}

			virtual u32 allocated_size() const IRR_OVERRIDE
			{
				return Indices.allocated_size();
			}

			virtual void* pointer() IRR_OVERRIDE  { return Indices.pointer(); }
			virtual const void* const_pointer() const IRR_OVERRIDE  { return Indices.const_pointer(); }

			virtual video::E_INDEX_TYPE getType() const IRR_OVERRIDE
			{
				if (sizeof(T)==sizeof(u16))
					return video::EIT_16BIT;
				else
					return video::EIT_32BIT;
			}
		};

	public:
		IIndexList *Indices;

		CIndexBuffer(video::E_INDEX_TYPE IndexType) :Indices(0), MappingHint(EHM_NEVER), ChangedID(1)
		{
			setType(IndexType);
		}

		CIndexBuffer(const IIndexBuffer &IndexBufferCopy) :Indices(0), MappingHint(EHM_NEVER), ChangedID(1)
		{
			setType(IndexBufferCopy.getType());
			reallocate(IndexBufferCopy.size());

			for (u32 n=0;n<IndexBufferCopy.size();++n)
				push_back(IndexBufferCopy[n]);
		}

		virtual ~CIndexBuffer()
		{
			delete Indices;
		}

		virtual void setType(video::E_INDEX_TYPE indexType) IRR_OVERRIDE
		{
			if ( Indices && Indices->getType() == indexType )
				return;

			IIndexList *NewIndices=0;

			switch (indexType)
			{
				case video::EIT_16BIT:
				{
					NewIndices=new CSpecificIndexList<u16>;
					break;
				}
				case video::EIT_32BIT:
				{
					NewIndices=new CSpecificIndexList<u32>;
					break;
				}
			}

			if (Indices)
			{
				NewIndices->reallocate( Indices->size() );

				for(u32 n=0;n<Indices->size();++n)
					NewIndices->push_back((*Indices)[n]);

				delete Indices;
			}

			Indices=NewIndices;
		}

		virtual void* getData() IRR_OVERRIDE {return Indices->pointer();}
		virtual const void* getData() const IRR_OVERRIDE { return Indices->const_pointer(); }

		virtual video::E_INDEX_TYPE getType() const IRR_OVERRIDE {return Indices->getType();}

		virtual u32 stride() const IRR_OVERRIDE {return Indices->stride();}

		virtual u32 size() const IRR_OVERRIDE
		{
			return Indices->size();
		}

		virtual void push_back(u32 value) IRR_OVERRIDE
		{
			Indices->push_back(value);
		}

		virtual u32 operator [](u32 index) const IRR_OVERRIDE
		{
			return (*Indices)[index];
		}

		virtual u32 getLast() IRR_OVERRIDE
		{
			return Indices->getLast();
		}

		virtual void setValue(u32 index, u32 value) IRR_OVERRIDE
		{
			Indices->setValue(index, value);
		}

		virtual void set_used(u32 usedNow) IRR_OVERRIDE
		{
			Indices->set_used(usedNow);
		}

		virtual void reallocate(u32 new_size, bool canShrink=true) IRR_OVERRIDE
		{
			Indices->reallocate(new_size, canShrink);
		}

		virtual u32 allocated_size() const IRR_OVERRIDE
		{
			return Indices->allocated_size();
		}

		//! get the current hardware mapping hint
		virtual E_HARDWARE_MAPPING getHardwareMappingHint() const IRR_OVERRIDE
		{
			return MappingHint;
		}

		//! set the hardware mapping hint, for driver
		virtual void setHardwareMappingHint( E_HARDWARE_MAPPING NewMappingHint ) IRR_OVERRIDE
		{
			MappingHint=NewMappingHint;
		}

		//! flags the mesh as changed, reloads hardware buffers
		virtual void setDirty() IRR_OVERRIDE
		{
			++ChangedID;
		}

		//! Get the currently used ID for identification of changes.
		/** This shouldn't be used for anything outside the VideoDriver. */
		virtual u32 getChangedID() const IRR_OVERRIDE {return ChangedID;}

		E_HARDWARE_MAPPING MappingHint;
		u32 ChangedID;
	};


} // end namespace scene
} // end namespace irr

#endif
