// Copyright (C) 2010-2012 Gaz Davidson
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_C_SMF_MESH_LOADER_H_INCLUDED
#define IRR_C_SMF_MESH_LOADER_H_INCLUDED

#include "IMeshLoader.h"
#include "SMesh.h"

namespace irr
{

namespace video
{
	class IVideoDriver;
}
namespace io
{
	class IFileSystem;
}

namespace scene
{

//! Class which can load
class CSMFMeshFileLoader : public virtual IMeshLoader
{
public:

	CSMFMeshFileLoader(irr::io::IFileSystem* fs, video::IVideoDriver* driver);

	//! Returns true if the file might be loaded by this class.
	virtual bool isALoadableFileExtension(const io::path& filename) const IRR_OVERRIDE;

	//! Creates/loads an animated mesh from the file.
	virtual IAnimatedMesh* createMesh(io::IReadFile* file) IRR_OVERRIDE;
private:

	void loadLimb(io::IReadFile* file, scene::SMesh* mesh, const core::matrix4 &parentTransformation);
};

} // end namespace scene

namespace io
{
	class BinaryFile
	{
	public:
		//! reads most types from the given file, moving the file pointer along
		template <class T>
		static void read(io::IReadFile* file, T &out, bool bigEndian=false);

		//! reads a 3d vector from the file, moving the file pointer along
		static void read(io::IReadFile* file, core::vector3df &outVector2d, bool bigEndian=false);

		//! reads a 2d vector from the file, moving the file pointer along
		static void read(io::IReadFile* file, core::vector2df &outVector2d, bool bigEndian=false);

		//! reads a null terminated string from the file, moving the file pointer along
		static void read(io::IReadFile* file, core::stringc &outString, bool bigEndian=false);

	};
}

} // end namespace irr

#endif // IRR_C_SMF_MESH_LOADER_H_INCLUDED
