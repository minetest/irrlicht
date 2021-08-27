// Copyright (C) 2009-2012 Gaz Davidson
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_PLY_MESH_WRITER_H_INCLUDED
#define IRR_PLY_MESH_WRITER_H_INCLUDED

#include "IMeshWriter.h"

namespace irr
{

namespace scene
{
	class IMeshBuffer;

	//! class to write PLY mesh files
	class CPLYMeshWriter : public IMeshWriter
	{
	public:

		CPLYMeshWriter();

		//! Returns the type of the mesh writer
		virtual EMESH_WRITER_TYPE getType() const IRR_OVERRIDE;

		//! writes a mesh
		virtual bool writeMesh(io::IWriteFile* file, scene::IMesh* mesh, s32 flags=EMWF_NONE) IRR_OVERRIDE;

	};

} // end namespace
} // end namespace

#endif
