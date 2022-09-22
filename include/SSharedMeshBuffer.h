// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef S_SHARED_MESH_BUFFER_H_INCLUDED
#define S_SHARED_MESH_BUFFER_H_INCLUDED

#include "irrArray.h"
#include "IMeshBuffer.h"

namespace irr
{
namespace scene
{
	//! Implementation of the IMeshBuffer interface with shared vertex list
	struct SSharedMeshBuffer : public IMeshBuffer
	{
		//! constructor
		SSharedMeshBuffer() 
			: IMeshBuffer()
			, Vertices(0), ChangedID_Vertex(1), ChangedID_Index(1)
			, MappingHintVertex(EHM_NEVER), MappingHintIndex(EHM_NEVER)
			, PrimitiveType(EPT_TRIANGLES)
		{
			#ifdef _DEBUG
			setDebugName("SSharedMeshBuffer");
			#endif
		}

		//! constructor
		SSharedMeshBuffer(core::array<video::S3DVertex> *vertices) : IMeshBuffer(), Vertices(vertices), ChangedID_Vertex(1), ChangedID_Index(1), MappingHintVertex(EHM_NEVER), MappingHintIndex(EHM_NEVER)
		{
			#ifdef _DEBUG
			setDebugName("SSharedMeshBuffer");
			#endif
		}

		//! returns the material of this meshbuffer
		virtual const video::SMaterial& getMaterial() const IRR_OVERRIDE
		{
			return Material;
		}

		//! returns the material of this meshbuffer
		virtual video::SMaterial& getMaterial() IRR_OVERRIDE
		{
			return Material;
		}

		//! returns pointer to vertices
		virtual const void* getVertices() const IRR_OVERRIDE
		{
			if (Vertices)
				return Vertices->const_pointer();
			else
				return 0;
		}

		//! returns pointer to vertices
		virtual void* getVertices() IRR_OVERRIDE
		{
			if (Vertices)
				return Vertices->pointer();
			else
				return 0;
		}

		//! returns amount of vertices
		virtual u32 getVertexCount() const IRR_OVERRIDE
		{
			if (Vertices)
				return Vertices->size();
			else
				return 0;
		}

		//! returns pointer to indices
		virtual const u16* getIndices() const IRR_OVERRIDE
		{
			return Indices.const_pointer();
		}

		//! returns pointer to indices
		virtual u16* getIndices() IRR_OVERRIDE
		{
			return Indices.pointer();
		}

		//! returns amount of indices
		virtual u32 getIndexCount() const IRR_OVERRIDE
		{
			return Indices.size();
		}

		//! Get type of index data which is stored in this meshbuffer.
		virtual video::E_INDEX_TYPE getIndexType() const IRR_OVERRIDE
		{
			return video::EIT_16BIT;
		}

		//! returns an axis aligned bounding box
		virtual const core::aabbox3d<f32>& getBoundingBox() const IRR_OVERRIDE
		{
			return BoundingBox;
		}

		//! set user axis aligned bounding box
		virtual void setBoundingBox( const core::aabbox3df& box) IRR_OVERRIDE
		{
			BoundingBox = box;
		}

		//! returns which type of vertex data is stored.
		virtual video::E_VERTEX_TYPE getVertexType() const IRR_OVERRIDE
		{
			return video::EVT_STANDARD;
		}

		//! recalculates the bounding box. should be called if the mesh changed.
		virtual void recalculateBoundingBox() IRR_OVERRIDE
		{
			if (!Vertices || Vertices->empty() || Indices.empty())
				BoundingBox.reset(0,0,0);
			else
			{
				BoundingBox.reset((*Vertices)[Indices[0]].Pos);
				for (u32 i=1; i<Indices.size(); ++i)
					BoundingBox.addInternalPoint((*Vertices)[Indices[i]].Pos);
			}
		}

		//! returns position of vertex i
		virtual const core::vector3df& getPosition(u32 i) const IRR_OVERRIDE
		{
			IRR_DEBUG_BREAK_IF(!Vertices);
			return (*Vertices)[Indices[i]].Pos;
		}

		//! returns position of vertex i
		virtual core::vector3df& getPosition(u32 i) IRR_OVERRIDE
		{
			IRR_DEBUG_BREAK_IF(!Vertices);
			return (*Vertices)[Indices[i]].Pos;
		}

		//! returns normal of vertex i
		virtual const core::vector3df& getNormal(u32 i) const IRR_OVERRIDE
		{
			IRR_DEBUG_BREAK_IF(!Vertices);
			return (*Vertices)[Indices[i]].Normal;
		}

		//! returns normal of vertex i
		virtual core::vector3df& getNormal(u32 i) IRR_OVERRIDE
		{
			IRR_DEBUG_BREAK_IF(!Vertices);
			return (*Vertices)[Indices[i]].Normal;
		}

		//! returns texture coord of vertex i
		virtual const core::vector2df& getTCoords(u32 i) const IRR_OVERRIDE
		{
			IRR_DEBUG_BREAK_IF(!Vertices);
			return (*Vertices)[Indices[i]].TCoords;
		}

		//! returns texture coord of vertex i
		virtual core::vector2df& getTCoords(u32 i) IRR_OVERRIDE
		{
			IRR_DEBUG_BREAK_IF(!Vertices);
			return (*Vertices)[Indices[i]].TCoords;
		}

		//! returns color of vertex i
		virtual video::SColor& getColor(u32 i) IRR_OVERRIDE
		{
			IRR_DEBUG_BREAK_IF(!Vertices);
			return (*Vertices)[Indices[i]].Color;
		}

		//! returns color of vertex i
		virtual const video::SColor& getColor(u32 i) const IRR_OVERRIDE
		{
			IRR_DEBUG_BREAK_IF(!Vertices);
			return (*Vertices)[Indices[i]].Color;
		}


		//! append the vertices and indices to the current buffer
		virtual void append(const void* const vertices, u32 numVertices, const u16* const indices, u32 numIndices)  IRR_OVERRIDE 
		{
			// can't do that as it doesn't own the vertex memory
		}

		//! append the meshbuffer to the current buffer
		virtual void append(const IMeshBuffer* const other) IRR_OVERRIDE 
		{
			// can't do that as it doesn't own the vertex memory
		}

		//! get the current hardware mapping hint
		virtual E_HARDWARE_MAPPING getHardwareMappingHint_Vertex() const IRR_OVERRIDE
		{
			return MappingHintVertex;
		}

		//! get the current hardware mapping hint
		virtual E_HARDWARE_MAPPING getHardwareMappingHint_Index() const IRR_OVERRIDE
		{
			return MappingHintIndex;
		}

		//! set the hardware mapping hint, for driver
		virtual void setHardwareMappingHint( E_HARDWARE_MAPPING NewMappingHint, E_BUFFER_TYPE buffer=EBT_VERTEX_AND_INDEX ) IRR_OVERRIDE
		{
			if (buffer==EBT_VERTEX_AND_INDEX || buffer==EBT_VERTEX)
				MappingHintVertex=NewMappingHint;
			if (buffer==EBT_VERTEX_AND_INDEX || buffer==EBT_INDEX)
				MappingHintIndex=NewMappingHint;
		}

		//! Describe what kind of primitive geometry is used by the meshbuffer
		virtual void setPrimitiveType(E_PRIMITIVE_TYPE type) IRR_OVERRIDE
		{
			PrimitiveType = type;
		}

		//! Get the kind of primitive geometry which is used by the meshbuffer
		virtual E_PRIMITIVE_TYPE getPrimitiveType() const IRR_OVERRIDE
		{
			return PrimitiveType;
		}

		//! flags the mesh as changed, reloads hardware buffers
		virtual void setDirty(E_BUFFER_TYPE buffer=EBT_VERTEX_AND_INDEX) IRR_OVERRIDE
		{
			if (buffer==EBT_VERTEX_AND_INDEX || buffer==EBT_VERTEX)
				++ChangedID_Vertex;
			if (buffer==EBT_VERTEX_AND_INDEX || buffer==EBT_INDEX)
				++ChangedID_Index;
		}

		//! Get the currently used ID for identification of changes.
		/** This shouldn't be used for anything outside the VideoDriver. */
		virtual u32 getChangedID_Vertex() const IRR_OVERRIDE {return ChangedID_Vertex;}

		//! Get the currently used ID for identification of changes.
		/** This shouldn't be used for anything outside the VideoDriver. */
		virtual u32 getChangedID_Index() const IRR_OVERRIDE {return ChangedID_Index;}

		//! Returns type of the class implementing the IMeshBuffer
		virtual EMESH_BUFFER_TYPE getType() const  IRR_OVERRIDE
		{
			return EMBT_SHARED;
		}

		//! Create copy of the meshbuffer
		virtual IMeshBuffer* createClone(int cloneFlags) const IRR_OVERRIDE
		{
			SSharedMeshBuffer * clone = new SSharedMeshBuffer();

			if (cloneFlags & ECF_VERTICES)
			{
				clone->Vertices = Vertices;
				clone->BoundingBox = BoundingBox;
			}

			if (cloneFlags & ECF_INDICES)
			{
				clone->Indices = Indices;
			}

			clone->Material = Material;
			clone->MappingHintVertex = MappingHintVertex;
			clone->MappingHintIndex = MappingHintIndex;
			clone->PrimitiveType = PrimitiveType;

			return clone;
		}

		//! Material of this meshBuffer
		video::SMaterial Material;

		//! Shared Array of vertices
		core::array<video::S3DVertex> *Vertices;

		//! Array of indices
		core::array<u16> Indices;

		//! ID used for hardware buffer management
		u32 ChangedID_Vertex;

		//! ID used for hardware buffer management
		u32 ChangedID_Index;

		//! Bounding box
		core::aabbox3df BoundingBox;

		//! hardware mapping hint
		E_HARDWARE_MAPPING MappingHintVertex;
		E_HARDWARE_MAPPING MappingHintIndex;

		//! Primitive type used for rendering (triangles, lines, ...)
		E_PRIMITIVE_TYPE PrimitiveType;
	};


} // end namespace scene
} // end namespace irr

#endif
