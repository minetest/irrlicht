#include "CB3DJSONMeshFileLoader.h"
#include "coreutil.h"
#include "IAnimatedMesh.h"
#include "IReadFile.h"
#include "path.h"
#include "json/json.hpp"

namespace irr
{

namespace scene
{

// Class methods.
CB3DJSONMeshFileLoader::CB3DJSONMeshFileLoader()
{

}

bool CB3DJSONMeshFileLoader::isALoadableFileExtension(
  const io::path& fileName) const
{
  //* we could do multiple accepted extensions, like b3dj jb3d, j3d, etc. Talk with core devs about this.
  return core::hasFileExtension(fileName, "b3djson");
}

IAnimatedMesh* CB3DJSONMeshFileLoader::createMesh(io::IReadFile* file) {
  //! I'm sure this isn't a horrible idea!
  return nullptr;
}

} // namespace scene
} // namespace irr