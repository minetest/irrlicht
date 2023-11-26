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

//! Remember to remove this function. It's very dumb.
void println(const char* data) {
  printf(data, "\n");
}


// Class methods.
CB3DJSONMeshFileLoader::CB3DJSONMeshFileLoader()
{

}

bool CB3DJSONMeshFileLoader::isALoadableFileExtension(
  const io::path& fileName) const
{
  //* we could do multiple accepted extensions, like b3dj jb3d, j3d, etc. Talk with core devs about this.
  //? For now, just JSON because it has vscode linting.
  return core::hasFileExtension(fileName, "json");
}

IAnimatedMesh* CB3DJSONMeshFileLoader::createMesh(io::IReadFile* file) {
  println("I am loading your cool file, yay");

  printf("the file is called: ");
  println(file->getFileName().c_str());

  // Less than zero? What is this file a black hole?
  if (file->getSize() <= 0) {
    return nullptr;
  }
  //! I'm sure this isn't a horrible idea!
  return nullptr;
}

} // namespace scene
} // namespace irr