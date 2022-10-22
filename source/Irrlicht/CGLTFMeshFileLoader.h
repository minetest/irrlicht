#ifndef __C_GLTF_MESH_FILE_LOADER_INCLUDED__
#define __C_GLTF_MESH_FILE_LOADER_INCLUDED__

#include "IAnimatedMesh.h"
#include "IMeshLoader.h"
#include "IReadFile.h"
#include "path.h"

namespace irr
{

namespace scene
{

class CGLTFMeshFileLoader : public IMeshLoader
{
public:
	CGLTFMeshFileLoader();

	bool isALoadableFileExtension(const io::path& filename) const override;

	IAnimatedMesh* createMesh(io::IReadFile* file) override;

private:
	bool tryParseGLTF(io::IReadFile* file) const;
};

} // namespace scene

} // namespace irr

#endif // __C_GLTF_MESH_FILE_LOADER_INCLUDED__

