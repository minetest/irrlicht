// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_ANIMATED_MESH_SCENE_NODE_H_INCLUDED__
#define __C_ANIMATED_MESH_SCENE_NODE_H_INCLUDED__

#include "IAnimatedMeshSceneNode.h"
#include "IAnimatedMesh.h"

#include "matrix4.h"


namespace irr
{
namespace scene
{
	class IDummyTransformationSceneNode;

	class CAnimatedMeshSceneNode : public IAnimatedMeshSceneNode
	{
	public:

		//! constructor
		CAnimatedMeshSceneNode(IAnimatedMesh* mesh, ISceneNode* parent, ISceneManager* mgr,	s32 id,
			const core::vector3df& position = core::vector3df(0,0,0),
			const core::vector3df& rotation = core::vector3df(0,0,0),
			const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f));

		//! destructor
		virtual ~CAnimatedMeshSceneNode();

		//! sets the current frame. from now on the animation is played from this frame.
		virtual void setCurrentFrame(f32 frame) _IRR_OVERRIDE_;

		//! frame
		virtual void OnRegisterSceneNode() _IRR_OVERRIDE_;

		//! OnAnimate() is called just before rendering the whole scene.
		virtual void OnAnimate(u32 timeMs) _IRR_OVERRIDE_;

		//! renders the node.
		virtual void render() _IRR_OVERRIDE_;

		//! returns the axis aligned bounding box of this node
		virtual const core::aabbox3d<f32>& getBoundingBox() const _IRR_OVERRIDE_;

		//! sets the frames between the animation is looped.
		//! the default is 0 - MaximalFrameCount of the mesh.
		//! NOTE: setMesh will also change this value and set it to the full range of animations of the mesh
		virtual bool setFrameLoop(s32 begin, s32 end) _IRR_OVERRIDE_;

		//! Sets looping mode which is on by default. If set to false,
		//! animations will not be looped.
		virtual void setLoopMode(bool playAnimationLooped) _IRR_OVERRIDE_;

		//! returns the current loop mode
		virtual bool getLoopMode() const _IRR_OVERRIDE_;

		//! Sets a callback interface which will be called if an animation
		//! playback has ended. Set this to 0 to disable the callback again.
		virtual void setAnimationEndCallback(IAnimationEndCallBack* callback=0) _IRR_OVERRIDE_;

		//! sets the speed with which the animation is played
		//! NOTE: setMesh will also change this value and set it to the default speed of the mesh
		virtual void setAnimationSpeed(f32 framesPerSecond) _IRR_OVERRIDE_;

		//! gets the speed with which the animation is played
		virtual f32 getAnimationSpeed() const _IRR_OVERRIDE_;

		//! returns the material based on the zero based index i. To get the amount
		//! of materials used by this scene node, use getMaterialCount().
		//! This function is needed for inserting the node into the scene hierarchy on a
		//! optimal position for minimizing renderstate changes, but can also be used
		//! to directly modify the material of a scene node.
		virtual video::SMaterial& getMaterial(u32 i) _IRR_OVERRIDE_;

		//! returns amount of materials used by this scene node.
		virtual u32 getMaterialCount() const _IRR_OVERRIDE_;

		//! Returns a pointer to a child node, which has the same transformation as
		//! the corresponding joint, if the mesh in this scene node is a skinned mesh.
		virtual IBoneSceneNode* getJointNode(const c8* jointName) _IRR_OVERRIDE_;

		//! same as getJointNode(const c8* jointName), but based on id
		virtual IBoneSceneNode* getJointNode(u32 jointID) _IRR_OVERRIDE_;

		//! Gets joint count.
		virtual u32 getJointCount() const _IRR_OVERRIDE_;

		//! Removes a child from this scene node.
		//! Implemented here, to be able to remove the shadow properly, if there is one,
		//! or to remove attached child.
		virtual bool removeChild(ISceneNode* child) _IRR_OVERRIDE_;

		//! Returns the current displayed frame number.
		virtual f32 getFrameNr() const _IRR_OVERRIDE_;
		//! Returns the current start frame number.
		virtual s32 getStartFrame() const _IRR_OVERRIDE_;
		//! Returns the current end frame number.
		virtual s32 getEndFrame() const _IRR_OVERRIDE_;

		//! Sets if the scene node should not copy the materials of the mesh but use them in a read only style.
		/* In this way it is possible to change the materials a mesh causing all mesh scene nodes
		referencing this mesh to change too. */
		virtual void setReadOnlyMaterials(bool readonly) _IRR_OVERRIDE_;

		//! Returns if the scene node should not copy the materials of the mesh but use them in a read only style
		virtual bool isReadOnlyMaterials() const _IRR_OVERRIDE_;

		//! Sets a new mesh
		virtual void setMesh(IAnimatedMesh* mesh) _IRR_OVERRIDE_;

		//! Returns the current mesh
		virtual IAnimatedMesh* getMesh(void) _IRR_OVERRIDE_ { return Mesh; }

		//! Returns type of the scene node
		virtual ESCENE_NODE_TYPE getType() const _IRR_OVERRIDE_ { return ESNT_ANIMATED_MESH; }

		//! updates the absolute position based on the relative and the parents position
		virtual void updateAbsolutePosition() _IRR_OVERRIDE_;


		//! Set the joint update mode (0-unused, 1-get joints only, 2-set joints only, 3-move and set)
		virtual void setJointMode(E_JOINT_UPDATE_ON_RENDER mode) _IRR_OVERRIDE_;

		//! Sets the transition time in seconds (note: This needs to enable joints, and setJointmode maybe set to 2)
		//! you must call animateJoints(), or the mesh will not animate
		virtual void setTransitionTime(f32 Time) _IRR_OVERRIDE_;

		//! updates the joint positions of this mesh
		virtual void animateJoints(bool CalculateAbsolutePositions=true) _IRR_OVERRIDE_;

		//! render mesh ignoring its transformation. Used with ragdolls. (culling is unaffected)
		virtual void setRenderFromIdentity( bool On ) _IRR_OVERRIDE_;

		//! Creates a clone of this scene node and its children.
		/** \param newParent An optional new parent.
		\param newManager An optional new scene manager.
		\return The newly created clone of this node. */
		virtual ISceneNode* clone(ISceneNode* newParent=0, ISceneManager* newManager=0) _IRR_OVERRIDE_;

	private:

		//! Get a static mesh for the current frame of this animated mesh
		IMesh* getMeshForCurrentFrame();

		void buildFrameNr(u32 timeMs);
		void checkJoints();
		void beginTransition();

		core::array<video::SMaterial> Materials;
		core::aabbox3d<f32> Box;
		IAnimatedMesh* Mesh;

		s32 StartFrame;
		s32 EndFrame;
		f32 FramesPerSecond;
		f32 CurrentFrameNr;

		u32 LastTimeMs;
		u32 TransitionTime; //Transition time in millisecs
		f32 Transiting; //is mesh transiting (plus cache of TransitionTime)
		f32 TransitingBlend; //0-1, calculated on buildFrameNr

		//0-unused, 1-get joints only, 2-set joints only, 3-move and set
		E_JOINT_UPDATE_ON_RENDER JointMode;
		bool JointsUsed;

		bool Looping;
		bool ReadOnlyMaterials;
		bool RenderFromIdentity;

		IAnimationEndCallBack* LoopCallBack;
		s32 PassCount;

		core::array<IBoneSceneNode* > JointChildSceneNodes;
		core::array<core::matrix4> PretransitingSave;
	};

} // end namespace scene
} // end namespace irr

#endif

