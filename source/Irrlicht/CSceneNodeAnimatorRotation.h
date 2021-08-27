// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_C_SCENE_NODE_ANIMATOR_ROTATION_H_INCLUDED
#define IRR_C_SCENE_NODE_ANIMATOR_ROTATION_H_INCLUDED

#include "ISceneNode.h"

namespace irr
{
namespace scene
{
	class CSceneNodeAnimatorRotation : public ISceneNodeAnimator
	{
	public:

		//! constructor
		CSceneNodeAnimatorRotation(u32 time, const core::vector3df& rotation);

		//! animates a scene node
		virtual void animateNode(ISceneNode* node, u32 timeMs) IRR_OVERRIDE;

		//! Writes attributes of the scene node animator.
		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const IRR_OVERRIDE;

		//! Reads attributes of the scene node animator.
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0) IRR_OVERRIDE;

		//! Returns type of the scene node animator
		virtual ESCENE_NODE_ANIMATOR_TYPE getType() const IRR_OVERRIDE { return ESNAT_ROTATION; }

		//! Creates a clone of this animator.
		/** Please note that you will have to drop
		(IReferenceCounted::drop()) the returned pointer after calling this. */
		virtual ISceneNodeAnimator* createClone(ISceneNode* node, ISceneManager* newManager=0) IRR_OVERRIDE;

	private:

		core::vector3df Rotation;
	};


} // end namespace scene
} // end namespace irr

#endif
