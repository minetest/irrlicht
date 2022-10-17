#include "CGLTFMeshFileLoader.h"
#include "coreutil.h"
#include "IAnimatedMesh.h"
#include "IReadFile.h"
#include "path.h"
#include "SAnimatedMesh.h"

namespace irr
{

namespace scene
{

CGLTFMeshFileLoader::CGLTFMeshFileLoader()
{
}

bool CGLTFMeshFileLoader::isALoadableFileExtension(
	const io::path& filename) const
{
	return core::hasFileExtension(filename, "gltf");
}

IAnimatedMesh* CGLTFMeshFileLoader::createMesh(io::IReadFile* file)
{
	if (file->getSize() == 0) {
		return nullptr;
	}

	// sorry Bjarne
	SAnimatedMesh* mesh { new SAnimatedMesh {} };

	return mesh;
}

} // namespace irr

} // namespace scene

