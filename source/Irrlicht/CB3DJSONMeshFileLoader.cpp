#include "IAnimatedMesh.h"
#include "IMeshLoader.h"
#include "IReadFile.h"
#include "path.h"
#include "json/json.hpp"

namespace irr
{

namespace scene
{

class CB3DJSONMeshFileLoader : public IMeshLoader
{
public:

CB3DJSONMeshFileLoader::CB3DJSONMeshFileLoader()
{
}

bool CB3DJSONMeshFileLoader::isALoadableFileExtension(
  const io::path& fileName) const
{
  return core::hasFileExtension(fileName, "b3djson");
}

};// class CB3DJSONMeshFileLoader
} // namespace scene
} // namespace irr