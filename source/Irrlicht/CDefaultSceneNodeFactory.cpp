// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CDefaultSceneNodeFactory.h"
#include "ISceneManager.h"
#include "IDummyTransformationSceneNode.h"
#include "ICameraSceneNode.h"
#include "IBillboardSceneNode.h"
#include "IAnimatedMeshSceneNode.h"
#include "IMeshSceneNode.h"

namespace irr
{
namespace scene
{


CDefaultSceneNodeFactory::CDefaultSceneNodeFactory(ISceneManager* mgr)
: Manager(mgr)
{

	#ifdef _DEBUG
	setDebugName("CDefaultSceneNodeFactory");
	#endif

	// don't grab the scene manager here to prevent cyclic references

	SupportedSceneNodeTypes.push_back(SSceneNodeTypePair(ESNT_MESH, "mesh"));
	SupportedSceneNodeTypes.push_back(SSceneNodeTypePair(ESNT_EMPTY, "empty"));
	SupportedSceneNodeTypes.push_back(SSceneNodeTypePair(ESNT_DUMMY_TRANSFORMATION, "dummyTransformation"));
	SupportedSceneNodeTypes.push_back(SSceneNodeTypePair(ESNT_CAMERA, "camera"));
	SupportedSceneNodeTypes.push_back(SSceneNodeTypePair(ESNT_BILLBOARD, "billBoard"));
	SupportedSceneNodeTypes.push_back(SSceneNodeTypePair(ESNT_ANIMATED_MESH, "animatedMesh"));
}


//! adds a scene node to the scene graph based on its type id
ISceneNode* CDefaultSceneNodeFactory::addSceneNode(ESCENE_NODE_TYPE type, ISceneNode* parent)
{
	switch(type)
	{
	case ESNT_MESH:
		return Manager->addMeshSceneNode(0, parent, -1, core::vector3df(),
										 core::vector3df(), core::vector3df(1,1,1), true);
	case ESNT_EMPTY:
		return Manager->addEmptySceneNode(parent);
	case ESNT_DUMMY_TRANSFORMATION:
		return Manager->addDummyTransformationSceneNode(parent);
	case ESNT_CAMERA:
		return Manager->addCameraSceneNode(parent);
	case ESNT_BILLBOARD:
		return Manager->addBillboardSceneNode(parent);
	case ESNT_ANIMATED_MESH:
		return Manager->addAnimatedMeshSceneNode(0, parent, -1, core::vector3df(),
												 core::vector3df(), core::vector3df(1,1,1), true);
	default:
		break;
	}

	return 0;
}


//! adds a scene node to the scene graph based on its type name
ISceneNode* CDefaultSceneNodeFactory::addSceneNode(const c8* typeName, ISceneNode* parent)
{
	return addSceneNode( getTypeFromName(typeName), parent );
}


//! returns amount of scene node types this factory is able to create
u32 CDefaultSceneNodeFactory::getCreatableSceneNodeTypeCount() const
{
	return SupportedSceneNodeTypes.size();
}


//! returns type of a creatable scene node type
ESCENE_NODE_TYPE CDefaultSceneNodeFactory::getCreateableSceneNodeType(u32 idx) const
{
	if (idx<SupportedSceneNodeTypes.size())
		return SupportedSceneNodeTypes[idx].Type;
	else
		return ESNT_UNKNOWN;
}


//! returns type name of a creatable scene node type
const c8* CDefaultSceneNodeFactory::getCreateableSceneNodeTypeName(u32 idx) const
{
	if (idx<SupportedSceneNodeTypes.size())
		return SupportedSceneNodeTypes[idx].TypeName.c_str();
	else
		return 0;
}


//! returns type name of a creatable scene node type
const c8* CDefaultSceneNodeFactory::getCreateableSceneNodeTypeName(ESCENE_NODE_TYPE type) const
{
	for (u32 i=0; i<SupportedSceneNodeTypes.size(); ++i)
		if (SupportedSceneNodeTypes[i].Type == type)
			return SupportedSceneNodeTypes[i].TypeName.c_str();

	return 0;
}


ESCENE_NODE_TYPE CDefaultSceneNodeFactory::getTypeFromName(const c8* name) const
{
	for (u32 i=0; i<SupportedSceneNodeTypes.size(); ++i)
		if (SupportedSceneNodeTypes[i].TypeName == name)
			return SupportedSceneNodeTypes[i].Type;

	return ESNT_UNKNOWN;
}


} // end namespace scene
} // end namespace irr

