// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_C_CUBE_SCENE_NODE_H_INCLUDED
#define IRR_C_CUBE_SCENE_NODE_H_INCLUDED

#include "IMeshSceneNode.h"
#include "SMesh.h"
#include "IGeometryCreator.h"

namespace irr
{
namespace scene
{
	class CCubeSceneNode : public IMeshSceneNode
	{
	public:

		//! constructor
		CCubeSceneNode(f32 size, ISceneNode* parent, ISceneManager* mgr, s32 id,
			const core::vector3df& position = core::vector3df(0,0,0),
			const core::vector3df& rotation = core::vector3df(0,0,0),
			const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f),
			ECUBE_MESH_TYPE type=ECMT_1BUF_12VTX_NA);

		virtual ~CCubeSceneNode();

		virtual void OnRegisterSceneNode() IRR_OVERRIDE;

		//! renders the node.
		virtual void render() IRR_OVERRIDE;

		//! returns the axis aligned bounding box of this node
		virtual const core::aabbox3d<f32>& getBoundingBox() const IRR_OVERRIDE;

		//! returns the material based on the zero based index i. To get the amount
		//! of materials used by this scene node, use getMaterialCount().
		//! This function is needed for inserting the node into the scene hierarchy on a
		//! optimal position for minimizing renderstate changes, but can also be used
		//! to directly modify the material of a scene node.
		virtual video::SMaterial& getMaterial(u32 i) IRR_OVERRIDE;

		//! returns amount of materials used by this scene node.
		virtual u32 getMaterialCount() const IRR_OVERRIDE;

		//! Returns type of the scene node
		virtual ESCENE_NODE_TYPE getType() const IRR_OVERRIDE { return ESNT_CUBE; }

		//! Creates shadow volume scene node as child of this node
		//! and returns a pointer to it.
		virtual IShadowVolumeSceneNode* addShadowVolumeSceneNode(const IMesh* shadowMesh,
			s32 id, bool zfailmethod=true, f32 infinity=10000.0f) IRR_OVERRIDE;

		//! Writes attributes of the scene node.
		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const IRR_OVERRIDE;

		//! Reads attributes of the scene node.
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0) IRR_OVERRIDE;

		//! Creates a clone of this scene node and its children.
		virtual ISceneNode* clone(ISceneNode* newParent=0, ISceneManager* newManager=0) IRR_OVERRIDE;

		//! Sets a new mesh to display
		virtual void setMesh(IMesh* mesh) IRR_OVERRIDE {}

		//! Returns the current mesh
		virtual IMesh* getMesh(void) IRR_OVERRIDE { return Mesh; }

		//! Sets if the scene node should not copy the materials of the mesh but use them in a read only style.
		/* In this way it is possible to change the materials a mesh causing all mesh scene nodes
		referencing this mesh to change too. */
		virtual void setReadOnlyMaterials(bool readonly) IRR_OVERRIDE {}

		//! Returns if the scene node should not copy the materials of the mesh but use them in a read only style
		virtual bool isReadOnlyMaterials() const IRR_OVERRIDE { return false; }

		//! Removes a child from this scene node.
		//! Implemented here, to be able to remove the shadow properly, if there is one,
		//! or to remove attached child.
		virtual bool removeChild(ISceneNode* child) IRR_OVERRIDE;

	private:
		void setSize();

		IMesh* Mesh;
		IShadowVolumeSceneNode* Shadow;
		f32 Size;
		ECUBE_MESH_TYPE MeshType;
	};

} // end namespace scene
} // end namespace irr

#endif
