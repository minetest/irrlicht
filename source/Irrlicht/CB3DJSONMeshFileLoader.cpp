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
#include <tuple>
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

/**
 * Simple error builder for Vec3.
*/
const char* buildVec3fError(std::string key) {
  return std::string("Error, ").append(key).append(" in NODE must be an array of 3 numbers!").c_str();
}

/**
 * Returns true if failure occurs.
*/
bool grabVec3f(json data, std::string key, irr::core::vector3df& refVec) {
  if (data.contains(key) && data[key].is_array() && data[key].size() == 3) {
    auto jsonVec3 = data[key];
    int i = 0;
    for (auto reference = jsonVec3.begin(); reference != jsonVec3.end(); ++reference) {
      auto value = *reference;
      // Can take integer OR float.
      if (!value.is_number()) {
        os::Printer::log(buildVec3fError(key), ELL_WARNING);
        return true;
      }
      switch (i){
        case 0:
          refVec.X = value;
          break;
        case 1:
          refVec.Y = value;
          break;
        case 2:
          refVec.Z = value;
          break;
      }
      i++;
    }
  } else {
    os::Printer::log(buildVec3fError(key), ELL_WARNING);
    return true;  
  }
  return false;
}

/**
 * Simple error builder for Quaternion.
*/
const char* buildQuatError(std::string key) {
  return std::string("Error, ").append(key).append(" in NODE must be an array of 4 numbers!").c_str();
}

/**
 * Returns true if failure occurs.
*/
bool grabQuaternionf(json data, std::string key, irr::core::quaternion& refQuat) {
  if (data.contains(key) && data[key].is_array() && data[key].size() == 4) {
    auto jsonQuat = data[key];
    int i = 0;
    for (auto reference = jsonQuat.begin(); reference != jsonQuat.end(); ++reference) {
      auto value = *reference;
      // Can take integer OR float.
      if (!value.is_number()) {
        os::Printer::log(buildQuatError(key), ELL_WARNING);
        return true;
      }
      switch (i){
        case 0:
          refQuat.X = value;
          break;
        case 1:
          refQuat.Y = value;
          break;
        case 2:
          refQuat.Z = value;
          break;
        case 3:
          refQuat.W = value;
          break;
      }
      i++;
    }
  } else {
    os::Printer::log(buildQuatError(key), ELL_WARNING);
    return true;  
  }
  return false;
}


// Returns if errored.
bool parseNode(json data, SMeshBuffer* meshBuffer) {

  auto position = irr::core::vector3df{0,0,0};
  auto scale = irr::core::vector3df{1,1,1};
  auto rotation = irr::core::quaternion{0,0,0,1};

  if (grabVec3f(data, "position", position)) {
    return true;
  }

  if (grabVec3f(data, "scale", scale)) {
    return true;
  }

  if (grabQuaternionf(data, "rotation", rotation)) {
    return true;
  }
  
  return false;
}

/**
 * Returns (success, failure reason).
*/
std::tuple<bool, std::string> CB3DJSONMeshFileLoader::load() {

  

  // Now check some real basic elements of the JSON file.
  if (!JSONDataContainer.contains("format") || !JSONDataContainer["format"].is_string() || JSONDataContainer["format"] != "BB3D") {
    return {false, "No format in B3D JSON! Expected: BB3D"};
  }
  if (!JSONDataContainer.contains("version") || !JSONDataContainer["version"].is_number_integer() || JSONDataContainer["version"] != 1) {
    return {false, "Wrong version in B3D JSON! Expected: 1"};
  }

  //! Remember: This is basically a linked tree.

  // SMesh* baseMesh(new SMesh {});

  // SMeshBuffer* meshBuffer(new SMeshBuffer {});

  // And now we begin the recursion!

  // std::cout << data["NODE"] << "\n";

  // if (data.contains("NODE") && data["NODE"].is_object()) {
  //   println("Yep, that's a node!");

  //   // If it fails, give up basically.
  //   if (parseNode(data["NODE"], meshBuffer)) {
  //     return nullptr;
  //   }
  // }

  // SAnimatedMesh* animatedMesh(new SAnimatedMesh {});
	// animatedMesh->addMesh(baseMesh);

  // return animatedMesh;
  return true;
}

/**
 * Automatically cleans and returns the nullptr so it can be inlined or chained.
*/
CSkinnedMesh* CB3DJSONMeshFileLoader::cleanUp(std::string failure) {
  os::Printer::log(failure.c_str(), ELL_WARNING);
  AnimatedMesh->drop();
  AnimatedMesh = 0;
  return AnimatedMesh;
}

/**
 * Returns success.
*/
bool CB3DJSONMeshFileLoader::parseJSONFile(io::IReadFile* file) {
  // So here we turn this mangled disaster into a C string.
  // Please consider this the equivalent of duct taping a chainsaw onto a car to cut your lawn.
  auto buffer = std::make_unique<char[]>(file->getSize());

  // Now we read that dang JSON.
  file->read(buffer.get(), file->getSize());

  // This is the same pointer as buffer, but it's easier to read as "output".
  char* output = buffer.get();

  // Irrlicht doesn't null terminate the raw output. So we must doe it ourself.
  output[file->getSize()] = '\0';

  // We have to catch a JSON parse error or else the game will segfault.
  try {
    JSONDataContainer = json::parse(output);
  } catch (const json::parse_error& e) {
    std::cout << "message: " << e.what() << '\n'
          << "exception id: " << e.id << '\n'
          << "byte position of error: " << e.byte << std::endl;
    os::Printer::log("JSON: Failed to parse!", ELL_WARNING);
    return false;
  }

  // I'm not sure if buffer and output gets dropped here.
  return true;
}

IAnimatedMesh* CB3DJSONMeshFileLoader::createMesh(io::IReadFile* file) {

  // Less than zero? What is this file a black hole?
  if (file->getSize() <= 0) {
    return(this->cleanUp("B3D JSON severe error! File size is 0!"));
  }

  // Try to read this file.
  if (!this->parseJSONFile(file)) {
    return(this->cleanUp("B3D JSON severe error! File size is 0!"));
  }

  //? Now JSONDataContainer exists on this object.
  //? So we can commence loading it.

  // Now we can start doing a full parse of the data in the model JSON and build it as an irrlicht model.
  const std::tuple<bool, std::string> loadResult = load();

  // If it failed to load, we give up.
  if (!std::get<0>(loadResult)) {
    // Print the reason, and return a nullptr.
    return(this->cleanUp(std::get<1>(loadResult)));
  }

  println("We got to the end.");
  return nullptr;
}

} // namespace scene
} // namespace irr