// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_C_EMPTY_SCENE_NODE_H_INCLUDED
#define IRR_C_EMPTY_SCENE_NODE_H_INCLUDED

#include "ISceneNode.h"

namespace irr
{
namespace scene
{

	class CEmptySceneNode : public ISceneNode
	{
	public:

		//! constructor
		CEmptySceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id);

		//! returns the axis aligned bounding box of this node
		virtual const core::aabbox3d<f32>& getBoundingBox() const IRR_OVERRIDE;

		//! This method is called just before the rendering process of the whole scene.
		virtual void OnRegisterSceneNode() IRR_OVERRIDE;

		//! does nothing.
		virtual void render() IRR_OVERRIDE;

		//! Returns type of the scene node
		virtual ESCENE_NODE_TYPE getType() const IRR_OVERRIDE { return ESNT_EMPTY; }

		//! Creates a clone of this scene node and its children.
		virtual ISceneNode* clone(ISceneNode* newParent=0, ISceneManager* newManager=0) IRR_OVERRIDE;

	private:

		core::aabbox3d<f32> Box;
	};

} // end namespace scene
} // end namespace irr

#endif
