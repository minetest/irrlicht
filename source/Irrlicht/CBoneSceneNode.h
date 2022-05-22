// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_BONE_SCENE_NODE_H_INCLUDED__
#define __C_BONE_SCENE_NODE_H_INCLUDED__

// Used with SkinnedMesh and IAnimatedMeshSceneNode, for boned meshes

#include "IBoneSceneNode.h"

namespace irr
{
namespace scene
{

	class CBoneSceneNode : public IBoneSceneNode
	{
	public:

		//! constructor
		CBoneSceneNode(ISceneNode* parent, ISceneManager* mgr,
			s32 id=-1, u32 boneIndex=0, const c8* boneName=0);

		//! Returns the index of the bone
		virtual u32 getBoneIndex() const _IRR_OVERRIDE_;

		//! Sets the animation mode of the bone. Returns true if successful.
		virtual bool setAnimationMode(E_BONE_ANIMATION_MODE mode) _IRR_OVERRIDE_;

		//! Gets the current animation mode of the bone
		virtual E_BONE_ANIMATION_MODE getAnimationMode() const _IRR_OVERRIDE_;

		//! returns the axis aligned bounding box of this node
		virtual const core::aabbox3d<f32>& getBoundingBox() const _IRR_OVERRIDE_;

		/*
		//! Returns the relative transformation of the scene node.
		//virtual core::matrix4 getRelativeTransformation() const _IRR_OVERRIDE_;
		*/

		virtual void OnAnimate(u32 timeMs) _IRR_OVERRIDE_;

		virtual void updateAbsolutePositionOfAllChildren() _IRR_OVERRIDE_;

		//! How the relative transformation of the bone is used
		virtual void setSkinningSpace(E_BONE_SKINNING_SPACE space) _IRR_OVERRIDE_
		{
			SkinningSpace=space;
		}

		virtual E_BONE_SKINNING_SPACE getSkinningSpace() const _IRR_OVERRIDE_
		{
			return SkinningSpace;
		}

	private:
		void helper_updateAbsolutePositionOfAllChildren(ISceneNode *Node);

		u32 BoneIndex;

		core::aabbox3d<f32> Box;

		E_BONE_ANIMATION_MODE AnimationMode;
		E_BONE_SKINNING_SPACE SkinningSpace;
	};


} // end namespace scene
} // end namespace irr

#endif

