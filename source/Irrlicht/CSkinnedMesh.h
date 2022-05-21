// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

//New skinned mesh

#ifndef __C_SKINNED_MESH_H_INCLUDED__
#define __C_SKINNED_MESH_H_INCLUDED__

#include "ISkinnedMesh.h"
#include "SMeshBuffer.h"
#include "S3DVertex.h"
#include "irrString.h"
#include "matrix4.h"
#include "quaternion.h"

namespace irr
{
namespace scene
{

	class IAnimatedMeshSceneNode;
	class IBoneSceneNode;

	class CSkinnedMesh: public ISkinnedMesh
	{
	public:

		//! constructor
		CSkinnedMesh();

		//! destructor
		virtual ~CSkinnedMesh();

		//! returns the amount of frames. If the amount is 1, it is a static (=non animated) mesh.
		virtual u32 getFrameCount() const _IRR_OVERRIDE_;

		//! Gets the default animation speed of the animated mesh.
		/** \return Amount of frames per second. If the amount is 0, it is a static, non animated mesh. */
		virtual f32 getAnimationSpeed() const _IRR_OVERRIDE_;

		//! Gets the frame count of the animated mesh.
		/** \param fps Frames per second to play the animation with. If the amount is 0, it is not animated.
		The actual speed is set in the scene node the mesh is instantiated in.*/
		virtual void setAnimationSpeed(f32 fps) _IRR_OVERRIDE_;

		//! returns the animated mesh based on a detail level (which is ignored)
		virtual IMesh* getMesh(s32 frame, s32 detailLevel=255, s32 startFrameLoop=-1, s32 endFrameLoop=-1) _IRR_OVERRIDE_;

		//! Animates this mesh's joints based on frame input
		//! blend: {0-old position, 1-New position}
		virtual void animateMesh(f32 frame, f32 blend) _IRR_OVERRIDE_;

		//! Preforms a software skin on this mesh based of joint positions
		virtual void skinMesh() _IRR_OVERRIDE_;

		//! returns amount of mesh buffers.
		virtual u32 getMeshBufferCount() const _IRR_OVERRIDE_;

		//! returns pointer to a mesh buffer
		virtual IMeshBuffer* getMeshBuffer(u32 nr) const _IRR_OVERRIDE_;

		//! Returns pointer to a mesh buffer which fits a material
		/** \param material: material to search for
		\return Returns the pointer to the mesh buffer or
		NULL if there is no such mesh buffer. */
		virtual IMeshBuffer* getMeshBuffer( const video::SMaterial &material) const _IRR_OVERRIDE_;

		//! returns an axis aligned bounding box
		virtual const core::aabbox3d<f32>& getBoundingBox() const _IRR_OVERRIDE_;

		//! set user axis aligned bounding box
		virtual void setBoundingBox( const core::aabbox3df& box) _IRR_OVERRIDE_;

		//! sets a flag of all contained materials to a new value
		virtual void setMaterialFlag(video::E_MATERIAL_FLAG flag, bool newvalue) _IRR_OVERRIDE_;

		//! set the hardware mapping hint, for driver
		virtual void setHardwareMappingHint(E_HARDWARE_MAPPING newMappingHint, E_BUFFER_TYPE buffer=EBT_VERTEX_AND_INDEX) _IRR_OVERRIDE_;

		//! flags the meshbuffer as changed, reloads hardware buffers
		virtual void setDirty(E_BUFFER_TYPE buffer=EBT_VERTEX_AND_INDEX) _IRR_OVERRIDE_;

		//! Returns the type of the animated mesh.
		virtual E_ANIMATED_MESH_TYPE getMeshType() const _IRR_OVERRIDE_;

		//! Gets joint count.
		virtual u32 getJointCount() const _IRR_OVERRIDE_;

		//! Gets the name of a joint.
		virtual const c8* getJointName(u32 number) const _IRR_OVERRIDE_;

		//! Gets a joint number from its name
		virtual s32 getJointNumber(const c8* name) const _IRR_OVERRIDE_;

		//! uses animation from another mesh
		virtual bool useAnimationFrom(const ISkinnedMesh *mesh) _IRR_OVERRIDE_;

		//! Update Normals when Animating
		//! False= Don't (default)
		//! True = Update normals, slower
		virtual void updateNormalsWhenAnimating(bool on) _IRR_OVERRIDE_;

		//! Sets Interpolation Mode
		virtual void setInterpolationMode(E_INTERPOLATION_MODE mode) _IRR_OVERRIDE_;

		//! Convertes the mesh to contain tangent information
		virtual void convertMeshToTangents() _IRR_OVERRIDE_;

		//! Does the mesh have no animation
		virtual bool isStatic() _IRR_OVERRIDE_;

		//! (This feature is not implemented in irrlicht yet)
		virtual bool setHardwareSkinning(bool on) _IRR_OVERRIDE_;

		//! Refreshes vertex data cached in joints such as positions and normals
		virtual void refreshJointCache() _IRR_OVERRIDE_;

		//! Moves the mesh into static position.
		virtual void resetAnimation() _IRR_OVERRIDE_;

		//Interface for the mesh loaders (finalize should lock these functions, and they should have some prefix like loader_
		//these functions will use the needed arrays, set values, etc to help the loaders

		//! exposed for loaders to add mesh buffers
		virtual core::array<SSkinMeshBuffer*> &getMeshBuffers() _IRR_OVERRIDE_;

		//! alternative method for adding joints
		virtual core::array<SJoint*> &getAllJoints() _IRR_OVERRIDE_;

		//! alternative method for adding joints
		virtual const core::array<SJoint*> &getAllJoints() const _IRR_OVERRIDE_;

		//! loaders should call this after populating the mesh
		virtual void finalize() _IRR_OVERRIDE_;

		//! Adds a new meshbuffer to the mesh, access it as last one
		virtual SSkinMeshBuffer *addMeshBuffer() _IRR_OVERRIDE_;

		//! Adds a new joint to the mesh, access it as last one
		virtual SJoint *addJoint(SJoint *parent=0) _IRR_OVERRIDE_;

		//! Adds a new position key to the mesh, access it as last one
		virtual SPositionKey *addPositionKey(SJoint *joint) _IRR_OVERRIDE_;
		//! Adds a new rotation key to the mesh, access it as last one
		virtual SRotationKey *addRotationKey(SJoint *joint) _IRR_OVERRIDE_;
		//! Adds a new scale key to the mesh, access it as last one
		virtual SScaleKey *addScaleKey(SJoint *joint) _IRR_OVERRIDE_;

		//! Adds a new weight to the mesh, access it as last one
		virtual SWeight *addWeight(SJoint *joint) _IRR_OVERRIDE_;

		virtual void updateBoundingBox(void);

		//! Recovers the joints from the mesh
		void recoverJointsFromMesh(core::array<IBoneSceneNode*> &jointChildSceneNodes);

		//! Tranfers the joint data to the mesh
		void transferJointsToMesh(const core::array<IBoneSceneNode*> &jointChildSceneNodes);

		//! Tranfers the joint hints to the mesh
		void transferOnlyJointsHintsToMesh(const core::array<IBoneSceneNode*> &jointChildSceneNodes);

		//! Creates an array of joints from this mesh as children of node
		void addJoints(core::array<IBoneSceneNode*> &jointChildSceneNodes,
				IAnimatedMeshSceneNode* node,
				ISceneManager* smgr);

private:
		void checkForAnimation();

		void normalizeWeights();

		void buildAllLocalAnimatedMatrices();

		void buildAllGlobalAnimatedMatrices(SJoint *Joint=0, SJoint *ParentJoint=0);

		void getFrameData(f32 frame, SJoint *Node,
				core::vector3df &position, s32 &positionHint,
				core::vector3df &scale, s32 &scaleHint,
				core::quaternion &rotation, s32 &rotationHint);

		void calculateGlobalMatrices(SJoint *Joint,SJoint *ParentJoint);

		void skinJoint(SJoint *Joint, SJoint *ParentJoint);

		void calculateTangents(core::vector3df& normal,
			core::vector3df& tangent, core::vector3df& binormal,
			const core::vector3df& vt1, const core::vector3df& vt2, const core::vector3df& vt3,
			const core::vector2df& tc1, const core::vector2df& tc2, const core::vector2df& tc3);

		core::array<SSkinMeshBuffer*> *SkinningBuffers; //Meshbuffer to skin, default is to skin localBuffers

		core::array<SSkinMeshBuffer*> LocalBuffers;

		core::array<SJoint*> AllJoints;
		core::array<SJoint*> RootJoints;

		// bool can't be used here because std::vector<bool>
		// doesn't allow taking a reference to individual elements.
		core::array< core::array<char> > Vertices_Moved;

		core::aabbox3d<f32> BoundingBox;

		f32 EndFrame;
		f32 FramesPerSecond;

		f32 LastAnimatedFrame;
		bool SkinnedLastFrame;

		E_INTERPOLATION_MODE InterpolationMode:8;

		bool HasAnimation;
		bool PreparedForSkinning;
		bool AnimateNormals;
		bool HardwareSkinning;
	};

} // end namespace scene
} // end namespace irr

#endif

