// Copyright (C) 2008-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_C_VERTEX_BUFFER_H_INCLUDED
#define IRR_C_VERTEX_BUFFER_H_INCLUDED

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
			virtual void push_back(const video::S3DVertex2TCoords &element) =0;
			virtual void push_back(const video::S3DVertexTangents &element) =0;
			virtual void setValue(u32 index, const video::S3DVertex &value) =0;
			virtual void setValue(u32 index, const video::S3DVertex2TCoords &value) =0;
			virtual void setValue(u32 index, const video::S3DVertexTangents &value) =0;

			virtual video::S3DVertex& operator [](u32 index) = 0;
			virtual video::S3DVertex& operator [](const u32 index) const =0;
			virtual video::S3DVertex& getLast() =0;
			virtual void set_used(u32 usedNow) =0;
			virtual void reallocate(u32 new_size, bool canShrink=true) =0;
			virtual u32 allocated_size() const =0;
			virtual void* pointer() =0;
			virtual const void* const_pointer() const =0;
			virtual video::E_VERTEX_TYPE getType() const =0;
		};

		template <class T>
		class CSpecificVertexList : public IVertexList
		{
		public:
			core::array<T> Vertices;

			virtual u32 stride() const IRR_OVERRIDE {return sizeof(T);}

			virtual u32 size() const IRR_OVERRIDE {return Vertices.size();}

			virtual void push_back (const video::S3DVertex &element) IRR_OVERRIDE
			{Vertices.push_back(element);}
			virtual void push_back(const video::S3DVertex2TCoords &element) IRR_OVERRIDE
			{Vertices.push_back(element);}
			virtual void push_back(const video::S3DVertexTangents &element) IRR_OVERRIDE
			{Vertices.push_back(element);}

			virtual void setValue(u32 index, const video::S3DVertex &value) IRR_OVERRIDE
			{Vertices[index] = value;}
			virtual void setValue(u32 index, const video::S3DVertex2TCoords &value) IRR_OVERRIDE
			{Vertices[index] = value;}
			virtual void setValue(u32 index, const video::S3DVertexTangents &value) IRR_OVERRIDE
			{Vertices[index] = value;}

			virtual video::S3DVertex& operator [](u32 index) IRR_OVERRIDE
			{return (video::S3DVertex&)Vertices[index];}

			virtual video::S3DVertex& operator [](const u32 index) const IRR_OVERRIDE
			{return (video::S3DVertex&)Vertices[index];}

			virtual video::S3DVertex& getLast() IRR_OVERRIDE
			{return (video::S3DVertex&)Vertices.getLast();}

			virtual void set_used(u32 usedNow) IRR_OVERRIDE
			{Vertices.set_used(usedNow);}

			virtual void reallocate(u32 new_size, bool canShrink) IRR_OVERRIDE
			{Vertices.reallocate(new_size, canShrink);}

			virtual u32 allocated_size() const IRR_OVERRIDE
			{
				return Vertices.allocated_size();
			}

			virtual void* pointer() IRR_OVERRIDE {return Vertices.pointer();}
			virtual const void* const_pointer() const IRR_OVERRIDE {return Vertices.const_pointer();}

			virtual video::E_VERTEX_TYPE getType() const IRR_OVERRIDE {return T::getType();}
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

		virtual void setType(video::E_VERTEX_TYPE vertexType) IRR_OVERRIDE
		{
			if ( Vertices && Vertices->getType() == vertexType )
				return;

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

				switch (Vertices->getType())	// old type
				{
					case video::EVT_STANDARD:
					{
						for(u32 n=0;n<Vertices->size();++n)
							NewVertices->push_back((*Vertices)[n]);
						break;
					}
					case video::EVT_2TCOORDS:
					{
						for(u32 n=0;n<Vertices->size();++n)
							NewVertices->push_back((video::S3DVertex2TCoords&)(*Vertices)[n]);
						break;
					}
					case video::EVT_TANGENTS:
					{
						for(u32 n=0;n<Vertices->size();++n)
							NewVertices->push_back((video::S3DVertexTangents&)(*Vertices)[n]);
						break;
					}
				}

				delete Vertices;
			}

			Vertices=NewVertices;
		}

		virtual void* getData() IRR_OVERRIDE {return Vertices->pointer();}
		virtual const void* getData() const IRR_OVERRIDE {return Vertices->const_pointer();}

		virtual video::E_VERTEX_TYPE getType() const IRR_OVERRIDE {return Vertices->getType();}

		virtual u32 stride() const IRR_OVERRIDE {return Vertices->stride();}

		virtual u32 size() const IRR_OVERRIDE
		{
			return Vertices->size();
		}

		virtual void push_back (const video::S3DVertex &element) IRR_OVERRIDE
		{
			Vertices->push_back(element);
		}

		virtual void push_back(const video::S3DVertex2TCoords &element) IRR_OVERRIDE
		{
			Vertices->push_back(element);
		}

		virtual void push_back(const video::S3DVertexTangents &element) IRR_OVERRIDE
		{
			Vertices->push_back(element);
		}

		virtual void setValue(u32 index, const video::S3DVertex &value) IRR_OVERRIDE
		{
			Vertices->setValue(index, value);
		}

		virtual void setValue(u32 index, const video::S3DVertex2TCoords &value) IRR_OVERRIDE
		{
			Vertices->setValue(index, value);
		}

		virtual void setValue(u32 index, const video::S3DVertexTangents &value) IRR_OVERRIDE
		{
			Vertices->setValue(index, value);
		}

		virtual video::S3DVertex& operator [](u32 index) IRR_OVERRIDE
		{
			return (*Vertices)[index];
		}

		virtual video::S3DVertex& operator [](const u32 index) const IRR_OVERRIDE
		{
			return (*Vertices)[index];
		}

		virtual video::S3DVertex& getLast() IRR_OVERRIDE
		{
			return Vertices->getLast();
		}

		virtual void set_used(u32 usedNow) IRR_OVERRIDE
		{
			Vertices->set_used(usedNow);
		}

		virtual void reallocate(u32 new_size, bool canShrink=true) IRR_OVERRIDE
		{
			Vertices->reallocate(new_size, canShrink);
		}

		virtual u32 allocated_size() const IRR_OVERRIDE
		{
			return Vertices->allocated_size();
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
		virtual u32 getChangedID() const IRR_OVERRIDE  {return ChangedID;}

		E_HARDWARE_MAPPING MappingHint;
		u32 ChangedID;
	};


} // end namespace scene
} // end namespace irr

#endif
