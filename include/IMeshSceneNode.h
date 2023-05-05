// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_I_MESH_SCENE_NODE_H_INCLUDED
#define IRR_I_MESH_SCENE_NODE_H_INCLUDED

#include "ISceneNode.h"

namespace irr
{
namespace scene
{

class IShadowVolumeSceneNode;
class IMesh;

//! Option for nodes how to register themeselves at the SceneManager
enum ENodeRegistration
{
	//! Each node registers once and renders all it's mesh-buffers
	ENR_DEFAULT,

	//! Register a new node per mesh-buffer at the SceneManager
	//! It allows the SceneManager to sort in each render stage per buffer instead of per node.
	//! This can be useful when having several transparent buffers in a mesh.
	//! Depending on the scene (and hardware) this can have a positive or negative effect on performance.
	//! It can avoid texture-switches, but adds nodes to sort and more matrix transformations are set.
	ENR_PER_MESH_BUFFER
};


//! A scene node displaying a static mesh
class IMeshSceneNode : public ISceneNode
{
public:

	//! Constructor
	/** Use setMesh() to set the mesh to display.
	*/
	IMeshSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id,
			const core::vector3df& position = core::vector3df(0,0,0),
			const core::vector3df& rotation = core::vector3df(0,0,0),
			const core::vector3df& scale = core::vector3df(1,1,1))
		: ISceneNode(parent, mgr, id, position, rotation, scale) 
		, NodeRegistration(ENR_DEFAULT)
	{}

	//! Sets a new mesh to display or update mesh when it changed
	/** \param mesh Mesh to display. */
	virtual void setMesh(IMesh* mesh) = 0;

	//! Get the currently defined mesh for display.
	/** \return Pointer to mesh which is displayed by this node. */
	virtual IMesh* getMesh(void) = 0;

	/** The shadow can be rendered using the ZPass or the zfail
	method. ZPass is a little bit faster because the shadow volume
	creation is easier, but with this method there occur ugly
	looking artifacts when the camera is inside the shadow volume.
	These error do not occur with the ZFail method, but it can 
	have trouble with clipping to the far-plane (it usually works 
	well in OpenGL and fails with other drivers).
	\param shadowMesh: Optional custom mesh for shadow volume.
	\param id: Id of the shadow scene node. This id can be used to
	identify the node later.
	\param zfailmethod: If set to true, the shadow will use the
	zfail method, if not, zpass is used.
	\param infinity: Value used by the shadow volume algorithm to
	scale the shadow volume. For zfail shadow volumes on some drivers 
	only support finite shadows, so camera zfar must be larger than 
	shadow back cap,which is depending on the infinity parameter).
	Infinity value also scales by the scaling factors of the model.
	If shadows don't show up with zfail then try reducing infinity.
	If shadows are cut-off then try increasing infinity.
	\return Pointer to the created shadow scene node. This pointer
	should not be dropped. See IReferenceCounted::drop() for more
	information. */
	virtual IShadowVolumeSceneNode* addShadowVolumeSceneNode(const IMesh* shadowMesh=0,
		s32 id=-1, bool zfailmethod=true, f32 infinity=1000.0f) = 0;

	//! Sets if the scene node should not copy the materials of the mesh but use them in a read only style.
	/** In this way it is possible to change the materials of a mesh
	causing all mesh scene nodes referencing this mesh to change, too.
	\param readonly Flag if the materials shall be read-only. */
	virtual void setReadOnlyMaterials(bool readonly) = 0;

	//! Check if the scene node should not copy the materials of the mesh but use them in a read only style
	/** This flag can be set by setReadOnlyMaterials().
	\return Whether the materials are read-only. */
	virtual bool isReadOnlyMaterials() const = 0;

	//! Set how the node registers itself to the SceneManager
	/** Note: Derived classes can ignore this flag, so think of it as a hint. */
	virtual void setNodeRegistration(ENodeRegistration nodeRegistration) 
	{
		NodeRegistration = nodeRegistration;
	}

	//! How does a node register itself to the SceneManager
	/** Note: Derived classes may ignore this flag */
	virtual ENodeRegistration getNodeRegistration() const
	{
		return NodeRegistration;
	}

protected:
	ENodeRegistration NodeRegistration;
};

} // end namespace scene
} // end namespace irr

#endif
