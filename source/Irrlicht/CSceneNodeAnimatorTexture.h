// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_C_SCENE_NODE_ANIMATOR_TEXTURE_H_INCLUDED
#define IRR_C_SCENE_NODE_ANIMATOR_TEXTURE_H_INCLUDED

#include "irrArray.h"
#include "ISceneNodeAnimatorFinishing.h"

namespace irr
{
namespace scene
{
	class CSceneNodeAnimatorTexture : public ISceneNodeAnimatorFinishing
	{
	public:

		//! constructor
		CSceneNodeAnimatorTexture(const core::array<video::ITexture*>& textures,
			s32 timePerFrame, bool loop, u32 now);

		//! destructor
		virtual ~CSceneNodeAnimatorTexture();

		//! animates a scene node
		virtual void animateNode(ISceneNode* node, u32 timeMs) IRR_OVERRIDE;

		//! Writes attributes of the scene node animator.
		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const IRR_OVERRIDE;

		//! Reads attributes of the scene node animator.
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0) IRR_OVERRIDE;

		//! Returns type of the scene node animator
		virtual ESCENE_NODE_ANIMATOR_TYPE getType() const IRR_OVERRIDE { return ESNAT_TEXTURE; }

		//! Creates a clone of this animator.
		/** Please note that you will have to drop
		(IReferenceCounted::drop()) the returned pointer after calling
		this. */
		virtual ISceneNodeAnimator* createClone(ISceneNode* node, ISceneManager* newManager=0) IRR_OVERRIDE;


	private:

		void clearTextures();

		core::array<video::ITexture*> Textures;
		u32 TimePerFrame;
		bool Loop;
	};


} // end namespace scene
} // end namespace irr

#endif
