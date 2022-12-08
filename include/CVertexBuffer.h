// Copyright (C) 2008-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_VERTEX_BUFFER_H_INCLUDED__
#define __C_VERTEX_BUFFER_H_INCLUDED__

#include "IVertexBuffer.h"


namespace irr
{
namespace scene
{

	class CVertexBuffer : public IVertexBuffer
	{
		class IVertexList
		{
		public:
			virtual ~IVertexList(){};

			virtual u32 stride() const =0;

			virtual u32 size() const =0;

			virtual void push_back (const video::S3DVertex &element) =0;
			virtual video::S3DVertex& operator [](const u32 index) const =0;
			virtual video::S3DVertex& getLast() =0;
			virtual void set_used(u32 usedNow) =0;
			virtual void reallocate(u32 new_size) =0;
			virtual u32 allocated_size() const =0;
			virtual video::S3DVertex* pointer() =0;
			virtual video::E_VERTEX_TYPE getType() const =0;
		};

		template <class T>
		class CSpecificVertexList : public IVertexList
		{
		public:
			core::array<T> Vertices;

			u32 stride() const override {return sizeof(T);}

			u32 size() const override {return Vertices.size();}

			void push_back (const video::S3DVertex &element) override
			{Vertices.push_back((T&)element);}

			video::S3DVertex& operator [](const u32 index) const override
			{return (video::S3DVertex&)Vertices[index];}

			video::S3DVertex& getLast() override
			{return (video::S3DVertex&)Vertices.getLast();}

			void set_used(u32 usedNow) override
			{Vertices.set_used(usedNow);}

			void reallocate(u32 new_size) override
			{Vertices.reallocate(new_size);}

			u32 allocated_size() const override
			{
				return Vertices.allocated_size();
			}

			video::S3DVertex* pointer() override {return Vertices.pointer();}

			video::E_VERTEX_TYPE getType() const override {return T::getType();}
		};

	public:
		IVertexList *Vertices;

		CVertexBuffer(video::E_VERTEX_TYPE vertexType) : Vertices(0),
				MappingHint(EHM_NEVER), ChangedID(1)
		{
			setType(vertexType);
		}

		CVertexBuffer(const IVertexBuffer &VertexBufferCopy) :
				Vertices(0), MappingHint(EHM_NEVER),
				ChangedID(1)
		{
			setType(VertexBufferCopy.getType());
			reallocate(VertexBufferCopy.size());

			for (u32 n=0;n<VertexBufferCopy.size();++n)
				push_back(VertexBufferCopy[n]);
		}

		virtual ~CVertexBuffer()
		{
			delete Vertices;
		}


		void setType(video::E_VERTEX_TYPE vertexType) override
		{
			IVertexList *NewVertices=0;

			switch (vertexType)
			{
				case video::EVT_STANDARD:
				{
					NewVertices=new CSpecificVertexList<video::S3DVertex>;
					break;
				}
				case video::EVT_2TCOORDS:
				{
					NewVertices=new CSpecificVertexList<video::S3DVertex2TCoords>;
					break;
				}
				case video::EVT_TANGENTS:
				{
					NewVertices=new CSpecificVertexList<video::S3DVertexTangents>;
					break;
				}
			}
			if (Vertices)
			{
				NewVertices->reallocate( Vertices->size() );

				for(u32 n=0;n<Vertices->size();++n)
					NewVertices->push_back((*Vertices)[n]);

				delete Vertices;
			}

			Vertices=NewVertices;
		}

		void* getData() override {return Vertices->pointer();}

		video::E_VERTEX_TYPE getType() const override {return Vertices->getType();}

		u32 stride() const override {return Vertices->stride();}

		u32 size() const override
		{
			return Vertices->size();
		}

		void push_back (const video::S3DVertex &element) override
		{
			Vertices->push_back(element);
		}

		video::S3DVertex& operator [](const u32 index) const override
		{
			return (*Vertices)[index];
		}

		video::S3DVertex& getLast() override
		{
			return Vertices->getLast();
		}

		void set_used(u32 usedNow) override
		{
			Vertices->set_used(usedNow);
		}

		void reallocate(u32 new_size) override
		{
			Vertices->reallocate(new_size);
		}

		u32 allocated_size() const override
		{
			return Vertices->allocated_size();
		}

		video::S3DVertex* pointer() override
		{
			return Vertices->pointer();
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
		u32 getChangedID() const override  {return ChangedID;}

		E_HARDWARE_MAPPING MappingHint;
		u32 ChangedID;
	};


} // end namespace scene
} // end namespace irr

#endif

