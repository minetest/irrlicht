// Copyright (C) 2008-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_C_DYNAMIC_MESHBUFFER_H_INCLUDED
#define IRR_C_DYNAMIC_MESHBUFFER_H_INCLUDED

#include "IDynamicMeshBuffer.h"

#include "CVertexBuffer.h"
#include "CIndexBuffer.h"

namespace irr
{
namespace scene
{

	class CDynamicMeshBuffer: public IDynamicMeshBuffer
	{
	public:
		//! constructor
		CDynamicMeshBuffer(video::E_VERTEX_TYPE vertexType, video::E_INDEX_TYPE indexType)
		: PrimitiveType(EPT_TRIANGLES)
		{
			VertexBuffer=new CVertexBuffer(vertexType);
			IndexBuffer=new CIndexBuffer(indexType);
		}

		//! destructor
		virtual ~CDynamicMeshBuffer()
		{
			if (VertexBuffer)
				VertexBuffer->drop();
			if (IndexBuffer)
				IndexBuffer->drop();
		}

		virtual IVertexBuffer& getVertexBuffer() const IRR_OVERRIDE
		{
			return *VertexBuffer;
		}

		virtual IIndexBuffer& getIndexBuffer() const IRR_OVERRIDE
		{
			return *IndexBuffer;
		}

		virtual void setVertexBuffer(IVertexBuffer *newVertexBuffer) IRR_OVERRIDE
		{
			if (newVertexBuffer)
				newVertexBuffer->grab();
			if (VertexBuffer)
				VertexBuffer->drop();

			VertexBuffer=newVertexBuffer;
		}

		virtual void setIndexBuffer(IIndexBuffer *newIndexBuffer) IRR_OVERRIDE
		{
			if (newIndexBuffer)
				newIndexBuffer->grab();
			if (IndexBuffer)
				IndexBuffer->drop();

			IndexBuffer=newIndexBuffer;
		}

		//! Get Material of this buffer.
		virtual const video::SMaterial& getMaterial() const IRR_OVERRIDE
		{
			return Material;
		}

		//! Get Material of this buffer.
		virtual video::SMaterial& getMaterial() IRR_OVERRIDE
		{
			return Material;
		}

		//! Get bounding box
		virtual const core::aabbox3d<f32>& getBoundingBox() const IRR_OVERRIDE
		{
			return BoundingBox;
		}

		//! Set bounding box
		virtual void setBoundingBox( const core::aabbox3df& box) IRR_OVERRIDE
		{
			BoundingBox = box;
		}

		//! Recalculate bounding box
		virtual void recalculateBoundingBox() IRR_OVERRIDE
		{
			if (!getVertexBuffer().size())
				BoundingBox.reset(0,0,0);
			else
			{
				BoundingBox.reset(getVertexBuffer()[0].Pos);
				for (u32 i=1; i<getVertexBuffer().size(); ++i)
					BoundingBox.addInternalPoint(getVertexBuffer()[i].Pos);
			}
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

		video::SMaterial Material;
		core::aabbox3d<f32> BoundingBox;
		//! Primitive type used for rendering (triangles, lines, ...)
		E_PRIMITIVE_TYPE PrimitiveType;
	private:
		CDynamicMeshBuffer(const CDynamicMeshBuffer&); // = delete in c++11, prevent copying

		IVertexBuffer *VertexBuffer;
		IIndexBuffer *IndexBuffer;
	};


} // end namespace scene
} // end namespace irr

#endif
