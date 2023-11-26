#include "CB3DJSONMeshFileLoader.h"
#include "coreutil.h"
#include "IAnimatedMesh.h"
#include "S3DVertex.h"
#include "SAnimatedMesh.h"
#include "SColor.h"
#include "CMeshBuffer.h"
#include "SMesh.h"
#include "vector3d.h"
#include "quaternion.h"
#include "IReadFile.h"
#include "path.h"
#include  <iostream>
#include "os.h"
#include <string>
#include "json/json.hpp"
#include <array>
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

// Returns if errored.
bool parseNode(json data, SMeshBuffer* meshBuffer) {

  auto position = irr::core::vector3df{0,0,0};
  auto scale = irr::core::vector3df{1,1,1};
  auto rotation = irr::core::quaternion{0,0,0,1};

  if (data.contains("position") && data["position"].is_array() && data["position"].size() == 3) {
    auto positionJSON = data["position"];
    int i = 0;
    for (auto reference = positionJSON.begin(); reference != positionJSON.end(); ++reference) {
      auto value = *reference;
      if (!value.is_number_integer()) {
        os::Printer::log("Error, position in NODE must be an array of 3 integers!", ELL_WARNING);
        return true;
      }
      switch (i){
        case 0:
          position.X = value;
          println(std::string("x is: ").append(std::to_string(position.X)).c_str());
          break;
        case 1:
          position.Y = value;
          break;
        case 2:
          position.Z = value;
          break;
      }
      i++;
    }
  } else {
    os::Printer::log("Error, position in NODE must be an array of 3 integers!", ELL_WARNING);
    return true;  
  }
  return false;
}

IAnimatedMesh* parseModel(json data) {

  //! Remember: This is basically a linked tree.

  SMesh* baseMesh(new SMesh {});

  SMeshBuffer* meshBuffer(new SMeshBuffer {});

  // And now we begin the recursion!

  std::cout << data["NODE"] << "\n";

  if (data.contains("NODE") && data["NODE"].is_object()) {
    println("Yep, that's a node!");

    // If it fails, give up basically.
    if (parseNode(data["NODE"], meshBuffer)) {
      return nullptr;
    }
  }

  SAnimatedMesh* animatedMesh(new SAnimatedMesh {});
	animatedMesh->addMesh(baseMesh);

  return animatedMesh;
}

IAnimatedMesh* CB3DJSONMeshFileLoader::createMesh(io::IReadFile* file) {


  // Less than zero? What is this file a black hole?
  if (file->getSize() <= 0) {
    os::Printer::log("B3D JSON severe error! File size is 0!", ELL_WARNING);
    return nullptr;
  }

  // So here we turn this mangled disaster into a C string.
  // Please consider this the equivalent of duct taping a chainsaw onto a car to cut your lawn.
  // char* buffer = new char[file->getSize()];
  auto buffer = std::make_unique<char[]>(file->getSize());

  // Now we read that dang JSON.
  file->read(buffer.get(), file->getSize());

  char* output = buffer.get();

  // Irrlicht doesn't null terminate the raw output. So we must doe it ourself.
  output[file->getSize()] = '\0';

  // We have to catch a JSON parse error or else the game will segfault.
  json data;

  try {
    data = json::parse(output);
  } catch (const json::parse_error& e) {
    std::cout << "message: " << e.what() << '\n'
          << "exception id: " << e.id << '\n'
          << "byte position of error: " << e.byte << std::endl;
    os::Printer::log("JSON: Failed to parse!", ELL_WARNING);
    return nullptr;
  }

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