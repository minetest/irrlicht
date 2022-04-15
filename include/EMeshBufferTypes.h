// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_E_MESH_BUFFER_TYPES_H_INCLUDED
#define IRR_E_MESH_BUFFER_TYPES_H_INCLUDED

#include "irrTypes.h"

namespace irr
{
namespace scene
{

	//! An enumeration for all types of built-in mesh buffers
	/** Types are represented by a four character code instead 
	of simple numbers (easier to add external classes). */
	enum EMESH_BUFFER_TYPE
	{
		//! SMeshBuffer (16 bit buffers)
		EMBT_STANDARD = MAKE_IRR_ID('s','t','a','n'),

		//! SMeshBufferLightMap (16 bit buffers)
		EMBT_LIGHTMAP = MAKE_IRR_ID('l','i','g','h'),

		//! SMeshBufferTangents (16 bit buffers)
		EMBT_TANGENTS = MAKE_IRR_ID('t','a','n','g'),

		//! CDynamicMeshBuffer (16 or 32 bit buffers)
		EMBT_DYNAMIC  = MAKE_IRR_ID('d','y','n','a'),

		// SSharedMeshBuffer
		EMBT_SHARED   = MAKE_IRR_ID('s','h','a','r'),

		// SSkinMeshBuffer
		EMBT_SKIN     = MAKE_IRR_ID('s','k','i','n'),

		//! Unknown class type
		EMBT_UNKNOWN  = MAKE_IRR_ID('u','n','k','n')
	};

} // end namespace scene
} // end namespace irr

#endif
