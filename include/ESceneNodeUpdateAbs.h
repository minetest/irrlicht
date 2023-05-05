// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_E_SCENE_NODE_UPDATE_ABS_H_INCLUDED
#define IRR_E_SCENE_NODE_UPDATE_ABS_H_INCLUDED

namespace irr
{
namespace scene
{
	//! Options how ISceneNode::updateAbsolutePosition calculates the AbsoluteTransformation
	enum ESCENE_NODE_UPDATE_ABS
	{
		//! Node and parent transformation matrices are multiplied (default)
		ESNUA_TRANSFORM_MATRIX,

		//! Only transform the position of the node transformation matrix 
		//! by the parent transformation matrix.
		//! Parent will not affect the rotation/scale of the node transformation.
		ESNUA_TRANSFORM_POSITION,

		//! Use the relative matrix as absolute transformation matrix
		//! Parent node transformation is ignored just like when the parent is set to 0
		ESNUA_RELATIVE
	};

	//! Names for culling type
	const c8* const SceneNodeUpdateAbsNames[] =
	{
		"matrix",
		"pos",
		"relative",
		0
	};

} // end namespace scene
} // end namespace irr

#endif
