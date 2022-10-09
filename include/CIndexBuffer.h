// Copyright (C) 2008-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_INDEX_BUFFER_H_INCLUDED__
#define __C_INDEX_BUFFER_H_INCLUDED__

#include "IIndexBuffer.h"

namespace irr
{
namespace scene
{

	class CIndexBuffer : public IIndexBuffer
	{

		class IIndexList
		{
		public:
			virtual ~IIndexList(){};

			virtual u32 stride() const =0;
			virtual u32 size() const =0;
			virtual void push_back(const u32 &element) =0;
			virtual u32 operator [](u32 index) const =0;
			virtual u32 getLast() =0;
			virtual void setValue(u32 index, u32 value) =0;
			virtual void set_used(u32 usedNow) =0;
			virtual void reallocate(u32 new_size) =0;
			virtual u32 allocated_size() const =0;
			virtual void* pointer() =0;
			virtual video::E_INDEX_TYPE getType() const =0;
		};

		template <class T>
		class CSpecificIndexList : public IIndexList
		{
		public:
			core::array<T> Indices;

			u32 stride() const override {return sizeof(T);}

			u32 size() const override {return Indices.size();}

			void push_back(const u32 &element) override
			{
				// push const ref due to compiler problem with gcc 4.6, big endian
				Indices.push_back((const T&)element);
			}

			u32 operator [](u32 index) const override
			{
				return (u32)(Indices[index]);
			}

			u32 getLast() override {return (u32)Indices.getLast();}

			void setValue(u32 index, u32 value) override
			{
				Indices[index]=(T)value;
			}

			void set_used(u32 usedNow) override
			{
				Indices.set_used(usedNow);
			}

			void reallocate(u32 new_size) override
			{
				Indices.reallocate(new_size);
			}

			u32 allocated_size() const override
			{
				return Indices.allocated_size();
			}

			void* pointer() override  {return Indices.pointer();}

			video::E_INDEX_TYPE getType() const override
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

		//virtual void setType(video::E_INDEX_TYPE IndexType);
		void setType(video::E_INDEX_TYPE IndexType) override
		{
			IIndexList *NewIndices=0;

			switch (IndexType)
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

		void* getData() override {return Indices->pointer();}

		video::E_INDEX_TYPE getType() const override {return Indices->getType();}

		u32 stride() const override {return Indices->stride();}

		u32 size() const override
		{
			return Indices->size();
		}

		void push_back(const u32 &element) override
		{
			Indices->push_back(element);
		}

		u32 operator [](u32 index) const override
		{
			return (*Indices)[index];
		}

		u32 getLast() override
		{
			return Indices->getLast();
		}

		void setValue(u32 index, u32 value) override
		{
			Indices->setValue(index, value);
		}

		void set_used(u32 usedNow) override
		{
			Indices->set_used(usedNow);
		}

		void reallocate(u32 new_size) override
		{
			Indices->reallocate(new_size);
		}

		u32 allocated_size() const override
		{
			return Indices->allocated_size();
		}

		void* pointer() override
		{
			return Indices->pointer();
		}

		//! get the current hardware mapping hint
		E_HARDWARE_MAPPING getHardwareMappingHint() const override
		{
			return MappingHint;
		}

		//! set the hardware mapping hint, for driver
		void setHardwareMappingHint( E_HARDWARE_MAPPING NewMappingHint ) override
		{
			MappingHint=NewMappingHint;
		}

		//! flags the mesh as changed, reloads hardware buffers
		void setDirty() override
		{
			++ChangedID;
		}

		//! Get the currently used ID for identification of changes.
		/** This shouldn't be used for anything outside the VideoDriver. */
		u32 getChangedID() const override {return ChangedID;}

		E_HARDWARE_MAPPING MappingHint;
		u32 ChangedID;
	};


} // end namespace scene
} // end namespace irr

#endif

