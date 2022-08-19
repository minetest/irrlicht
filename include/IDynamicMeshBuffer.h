// Copyright (C) 2008-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_I_DYNAMIC_MESH_BUFFER_H_INCLUDED
#define IRR_I_DYNAMIC_MESH_BUFFER_H_INCLUDED

#include "IMeshBuffer.h"
#include "IVertexBuffer.h"
#include "IIndexBuffer.h"

namespace irr
{
namespace scene
{

	/** a dynamic meshBuffer */
	class IDynamicMeshBuffer : public IMeshBuffer
	{
	public:
		virtual IVertexBuffer &getVertexBuffer() const =0;
		virtual IIndexBuffer &getIndexBuffer() const =0;

		virtual void setVertexBuffer(IVertexBuffer *vertexBuffer) =0;
		virtual void setIndexBuffer(IIndexBuffer *indexBuffer) =0;


		// ------------------- Old interface -------------------  //
		// That stuff could also be in CDynamicMeshBuffer
		// I suppose it was put here to show that the information 
		// is now basically handled by the vertex/index buffers instead
		// of the meshbuffer itself.


		//! get the current hardware mapping hint
		virtual E_HARDWARE_MAPPING getHardwareMappingHint_Vertex() const IRR_OVERRIDE
		{
			return getVertexBuffer().getHardwareMappingHint();
		}

		//! get the current hardware mapping hint
		virtual E_HARDWARE_MAPPING getHardwareMappingHint_Index() const IRR_OVERRIDE
		{
			return getIndexBuffer().getHardwareMappingHint();
		}

		//! set the hardware mapping hint, for driver
		virtual void setHardwareMappingHint( E_HARDWARE_MAPPING NewMappingHint, E_BUFFER_TYPE Buffer=EBT_VERTEX_AND_INDEX ) IRR_OVERRIDE
		{
			if (Buffer==EBT_VERTEX_AND_INDEX || Buffer==EBT_VERTEX)
				getVertexBuffer().setHardwareMappingHint(NewMappingHint);
			if (Buffer==EBT_VERTEX_AND_INDEX || Buffer==EBT_INDEX)
				getIndexBuffer().setHardwareMappingHint(NewMappingHint);
		}

		//! flags the mesh as changed, reloads hardware buffers
		virtual void setDirty(E_BUFFER_TYPE Buffer=EBT_VERTEX_AND_INDEX) IRR_OVERRIDE
		{
			if (Buffer==EBT_VERTEX_AND_INDEX || Buffer==EBT_VERTEX)
				getVertexBuffer().setDirty();
			if (Buffer==EBT_VERTEX_AND_INDEX || Buffer==EBT_INDEX)
				getIndexBuffer().setDirty();
		}

		virtual u32 getChangedID_Vertex() const IRR_OVERRIDE
		{
			return getVertexBuffer().getChangedID();
		}

		virtual u32 getChangedID_Index() const IRR_OVERRIDE
		{
			return getIndexBuffer().getChangedID();
		}


		//! Get type of vertex data which is stored in this meshbuffer.
		/** \return Vertex type of this buffer. */
		virtual video::E_VERTEX_TYPE getVertexType() const IRR_OVERRIDE
		{
			return getVertexBuffer().getType();
		}

		//! Get access to vertex data. The data is an array of vertices.
		/** Which vertex type is used can be determined by getVertexType().
		\return Pointer to array of vertices. */
		virtual const void* getVertices() const IRR_OVERRIDE
		{
			return getVertexBuffer().getData();
		}

		//! Get access to vertex data. The data is an array of vertices.
		/** Which vertex type is used can be determined by getVertexType().
		\return Pointer to array of vertices. */
		virtual void* getVertices() IRR_OVERRIDE
		{
			return getVertexBuffer().getData();
		}

		//! Get amount of vertices in meshbuffer.
		/** \return Number of vertices in this buffer. */
		virtual u32 getVertexCount() const IRR_OVERRIDE
		{
			return getVertexBuffer().size();
		}

		//! Get type of index data which is stored in this meshbuffer.
		/** \return Index type of this buffer. */
		virtual video::E_INDEX_TYPE getIndexType() const IRR_OVERRIDE
		{
			return getIndexBuffer().getType();
		}

		//! Get access to indices.
		/** \return Pointer to indices array. */
		virtual const u16* getIndices() const IRR_OVERRIDE
		{
			return (u16*)getIndexBuffer().getData();
		}

		//! Get access to indices.
		/** \return Pointer to indices array. */
		virtual u16* getIndices() IRR_OVERRIDE
		{
			return (u16*)getIndexBuffer().getData();
		}

		//! Get amount of indices in this meshbuffer.
		/** \return Number of indices in this buffer. */
		virtual u32 getIndexCount() const IRR_OVERRIDE
		{ 
			return getIndexBuffer().size();
		}

		//! returns position of vertex i
		virtual const core::vector3df& getPosition(u32 i) const IRR_OVERRIDE
		{
			return getVertexBuffer()[i].Pos;
		}

		//! returns position of vertex i
		virtual core::vector3df& getPosition(u32 i) IRR_OVERRIDE
		{
			return getVertexBuffer()[i].Pos;
		}

		//! returns texture coords of vertex i
		virtual const core::vector2df& getTCoords(u32 i) const IRR_OVERRIDE
		{
			return getVertexBuffer()[i].TCoords;
		}

		//! returns texture coords of vertex i
		virtual core::vector2df& getTCoords(u32 i) IRR_OVERRIDE
		{
			return getVertexBuffer()[i].TCoords;
		}

		//! returns normal of vertex i
		virtual const core::vector3df& getNormal(u32 i) const IRR_OVERRIDE
		{
			return getVertexBuffer()[i].Normal;
		}

		//! returns normal of vertex i
		virtual core::vector3df& getNormal(u32 i) IRR_OVERRIDE
		{
			return getVertexBuffer()[i].Normal;
		}

		//! returns color of vertex i
		virtual video::SColor& getColor(u32 i) IRR_OVERRIDE
		{
			return getVertexBuffer()[i].Color;
		}

		//! returns color of vertex i
		virtual const video::SColor& getColor(u32 i) const IRR_OVERRIDE
		{
			return getVertexBuffer()[i].Color;
		}
	};


} // end namespace scene
} // end namespace irr

#endif
