#include "CB3DJSONMeshFileLoader.h"
#include "coreutil.h"
#include "IAnimatedMesh.h"
#include "IReadFile.h"
#include "path.h"
#include "json/json.hpp"
#include "os.h"
using json = nlohmann::json;

namespace irr
{

namespace scene
{

//! Remember to remove this function. It's very dumb.
void println(const char* data) {
  printf(data);
  printf("\n");
}
const char * const boolToString(bool b)
{
  return b ? "true" : "false";
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

  // Less than zero? What is this file a black hole?
  if (file->getSize() <= 0) {
    os::Printer::log("B3D JSON severe error! File size is 0!", ELL_WARNING);
    return nullptr;
  }

  println("I am loading your cool file, yay");

  printf("the file is called: ");
  println(file->getFileName().c_str());

  // So here we turn this mangled disaster into a C string.

  // These two hold error message pointers, basically.
  // std::string err {};
  // std::string warn {};

  // Try changing the type from auto to see why it's auto.
  auto buffer = std::make_unique<char[]>(file->getSize());

  // Now we read that dang JSON.
  file->read(buffer.get(), file->getSize());

  const json data = json::parse("{\"hi\": 1}");

  const auto test = data["hi"];
  printf("is this a number? ");
  println(boolToString(test.is_number()));
  println(test.dump().c_str());



  //! I'm sure this isn't a horrible idea!
  return nullptr;
}

} // namespace scene
} // namespace irr