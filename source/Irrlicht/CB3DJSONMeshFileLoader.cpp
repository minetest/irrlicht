#include "CB3DJSONMeshFileLoader.h"
#include "coreutil.h"
#include "IAnimatedMesh.h"
#include "IReadFile.h"
#include "path.h"
#include  <iostream>
#include "os.h"
#include "string.h"
#include "json/json.hpp"
#include <array>
using json = nlohmann::json;
using namespace nlohmann::literals;

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

IAnimatedMesh* parseModel(json model) {


  return nullptr;
}

IAnimatedMesh* CB3DJSONMeshFileLoader::createMesh(io::IReadFile* file) {

  // Less than zero? What is this file a black hole?
  if (file->getSize() <= 0) {
    os::Printer::log("B3D JSON severe error! File size is 0!", ELL_WARNING);
    return nullptr;
  }

  // So here we turn this mangled disaster into a C string.
  // Please consider this the equivalent of duct taping a chainsaw onto a car to cut your lawn.

  // auto buffer = std::make_unique<char[]>(file->getSize());
  char* buffer = new char[file->getSize()];

  // Now we read that dang JSON.
  file->read(buffer, file->getSize());

  char* clone = strdup(buffer);

  // Dereference then borrow it.
  json data = json::parse(&*clone);

  // Now check some real basic elements of the JSON file.
  if (!data.contains("format") || !data["format"].is_string() || data["format"] != "BB3DJSON") {
    os::Printer::log("No format in B3D JSON! Expected: BB3DJSON", ELL_WARNING);
    return nullptr;
  }
  if (!data.contains("version") || !data["version"].is_number_integer() || data["version"] != 1) {
    os::Printer::log("Wrong version in B3D JSON! Expected: 1", ELL_WARNING);
    return nullptr;
  }

  // Now we can start doing things with it.
  IAnimatedMesh* finalizedModel = parseModel(data);


  return finalizedModel;
}

} // namespace scene
} // namespace irr