// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_C_DUMMY_TRANSFORMATION_SCENE_NODE_H_INCLUDED
#define IRR_C_DUMMY_TRANSFORMATION_SCENE_NODE_H_INCLUDED

#include "IDummyTransformationSceneNode.h"

namespace irr
{
namespace scene
{

	class CDummyTransformationSceneNode : public IDummyTransformationSceneNode
	{
	public:

		//! constructor
		CDummyTransformationSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id);

		//! returns the axis aligned bounding box of this node
		virtual const core::aabbox3d<f32>& getBoundingBox() const IRR_OVERRIDE;

		//! Returns a reference to the current relative transformation matrix.
		//! This is the matrix, this scene node uses instead of scale, translation
		//! and rotation.
		virtual core::matrix4& getRelativeTransformationMatrix() IRR_OVERRIDE;

		//! Returns the relative transformation of the scene node.
		virtual core::matrix4 getRelativeTransformation() const IRR_OVERRIDE;

		//! does nothing.
		virtual void render() IRR_OVERRIDE {}

		//! Returns type of the scene node
		virtual ESCENE_NODE_TYPE getType() const IRR_OVERRIDE { return ESNT_DUMMY_TRANSFORMATION; }

		//! Creates a clone of this scene node and its children.
		virtual ISceneNode* clone(ISceneNode* newParent=0, ISceneManager* newManager=0) IRR_OVERRIDE;


	private:

		// TODO: We can add least add some warnings to find troubles faster until we have
		// fixed bug id 2318691.
		virtual const core::vector3df& getScale() const IRR_OVERRIDE;
		virtual void setScale(const core::vector3df& scale) IRR_OVERRIDE;
		virtual const core::vector3df& getRotation() const IRR_OVERRIDE;
		virtual void setRotation(const core::vector3df& rotation) IRR_OVERRIDE;
		virtual const core::vector3df& getPosition() const IRR_OVERRIDE;
		virtual void setPosition(const core::vector3df& newpos) IRR_OVERRIDE;

		core::matrix4 RelativeTransformationMatrix;
		core::aabbox3d<f32> Box;
	};

} // end namespace scene
} // end namespace irr

#endif
