#include "IAnimatedMesh.h"
#include "IMeshLoader.h"
#include "IReadFile.h"
#include "path.h"

namespace irr
{

namespace scene
{

class CB3DJSONMeshFileLoader : public IMeshLoader
{
public:

  CB3DJSONMeshFileLoader();

  bool isALoadableFileExtension(const io::path& fileName) const override;

  IAnimatedMesh* createMesh(io::IReadFile* file) override;

};// class CB3DJSONMeshFileLoader
} // namespace scene
} // namespace irr