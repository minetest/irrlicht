#include "CB3DJSONMeshFileLoader.h"
#include "coreutil.h"
#include "IAnimatedMesh.h"
#include "SB3DStructs.h"
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
 * Simple error builder for Vec2, Vec3, and Quaternion.
*/
std::string buildVectorError(std::string key, int index, int width) {

  if (index == -1) {
    return std::string("Error, " + key + " is missing!");
  }

  std::string component;
  switch (index) {
    case 0:
      component = "X";
      break;
    case 1:
      component = "Y";
      break;
    case 2:
      component = "Z";
      break;
    case 3:
      component = "W";
      break;
  }
  return std::string("Error, " + component + " in " + key + "is not a number! " + key + " must be an array of " + std::to_string(width) + " numbers!");
}


/**
 * Returns (success, failure reason).
*/
std::tuple<bool, std::string> grabVec2f(json data, std::string key, irr::core::vector2df& refVec) {

  // todo: make a CurrentElement thing in the class header so that we can print nice debug info.

  if (data.contains(key) && data[key].is_array() && data[key].size() == 2) {
    auto jsonVec2 = data[key];
    int i = 0;
    for (auto reference = jsonVec2.begin(); reference != jsonVec2.end(); ++reference) {
      auto value = *reference;
      // Can take integer OR float.
      if (!value.is_number()) {
        return {false, buildVectorError(key, i, 2)};
      }
      switch (i){
        case 0:
          refVec.X = value;
          break;
        case 1:
          refVec.Y = value;
          break;
      }
      i++;
    }
  } else {
    return {false, buildVectorError(key, -1, 2)};  
  }
  return {true, nullptr};
}

/**
 * Returns (success, failure reason).
*/
std::tuple<bool, std::string> grabVec3f(json data, std::string key, irr::core::vector3df& refVec) {

  // todo: make a CurrentElement thing in the class header so that we can print nice debug info.

  if (data.contains(key) && data[key].is_array() && data[key].size() == 3) {
    auto jsonVec3 = data[key];
    int i = 0;
    for (auto reference = jsonVec3.begin(); reference != jsonVec3.end(); ++reference) {
      auto value = *reference;
      // Can take integer OR float.
      if (!value.is_number()) {
        return {false, buildVectorError(key, i, 3)};
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
    return {false, buildVectorError(key, -1, 3)};
  }
  return {true, nullptr};
}

/**
 * Returns (success, failure reason).
*/
std::tuple<bool, std::string> grabQuaternionf(json data, std::string key, irr::core::quaternion& refQuat) {
  if (data.contains(key) && data[key].is_array() && data[key].size() == 4) {
    auto jsonQuat = data[key];
    int i = 0;
    for (auto reference = jsonQuat.begin(); reference != jsonQuat.end(); ++reference) {
      auto value = *reference;
      // Can take integer OR float.
      if (!value.is_number()) {
        return {false, buildVectorError(key, i, 4)};
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
    return {false, buildVectorError(key, -1, 4)};
  }
  return {true, nullptr};
}


// Returns (success, failure reason).
std::tuple<bool, std::string> parseNode(json data, SMeshBuffer* meshBuffer) {

  auto position = irr::core::vector3df{0,0,0};
  auto scale = irr::core::vector3df{1,1,1};
  auto rotation = irr::core::quaternion{0,0,0,1};

  auto positionResult = grabVec3f(data, "position", position);
  if (!std::get<0>(positionResult)) {
    return {false, std::get<1>(positionResult)};
  }

  auto scaleResult = grabVec3f(data, "scale", scale);
  if (!std::get<0>(scaleResult)) {
    return {false, std::get<1>(scaleResult)};
  }

  auto rotationResult = grabQuaternionf(data, "rotation", rotation);
  if (!std::get<0>(rotationResult)) {
    return {false, std::get<1>(rotationResult)};
  }
  
  return {true, nullptr};
}

/**
 * Returns (success, failure reason).
 * This is an optional component of B3D.
*/
std::tuple<bool, std::string> CB3DJSONMeshFileLoader::readChunkTEXS() {

  // We're checking the root of the JSON structure here. So we can just peak directly into the root.
  json texs;
  if (JSONDataContainer.contains("TEXS")) {
    // We're referencing static memory.
    texs = JSONDataContainer["TEXS"];
  } {
    // Since it's optional, it succeeds if it's not there.
    return {true, nullptr};
  }

  if (texs.contains("textures") && texs["textures"].is_array()) {

    json textureArray = texs["textures"];

    for (auto reference = textureArray.begin(); reference != textureArray.end(); ++reference) {

      // t stands for texture. :D Come back next week for more knowledge.
      auto t = *reference;
                  
      // This is a very strange way to do this but I won't complain.
      Textures.push_back(SB3dTexture());
      SB3dTexture& B3DTexture = Textures.getLast();

      // This part should probably be it's own function.
      //todo: look into making this it's own function.

      if (t.contains("pos") && t["pos"].is_array()) {

        irr::core::vector2df pos {0,0};
        

        auto posSuccess = grabVec2f(t, "pos", pos);
        if (!std::get<0>(posSuccess)) {
          return {false, "TEXS: " + std::get<1>(posSuccess)};
        }



      } else {
        return {false, "Malformed \"pos\" in TEXS block. Must be an array with 2 numbers."};
      }
    }
  } else {
    // it is malformed.
    //todo: return 
    return {false, "Malformed TEXS block. \"textures\" is not an array."} ;
  }

  // Everything succeeds, yay!
  return {true, nullptr};
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

  // Success, failure reason
  std::tuple<bool, std::string> texturesSuccess = this->readChunkTEXS();

  

  // return animatedMesh;
  return {true, nullptr};
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
 * Returns (success, failure reason).
*/
std::tuple<bool, std::string> CB3DJSONMeshFileLoader::parseJSONFile(io::IReadFile* file) {
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
    //! This is pointing to static memory. It will only be valid during the loading of this model.
    JSONDataContainer = json::parse(output);
  } catch (const json::parse_error& e) {
    std::string failureReason = "message: " + std::string(e.what()) + '\n' +
          "exception id: " + std::to_string(e.id) + '\n' +
          "byte position of error: " + std::to_string(e.byte) + '\n';
    os::Printer::log("JSON: Failed to parse!", ELL_WARNING);
    return {false, failureReason};
  }

  // I'm not sure if buffer and output gets dropped here.
  return {true, nullptr};
}

IAnimatedMesh* CB3DJSONMeshFileLoader::createMesh(io::IReadFile* file) {

  // Less than zero? What is this file a black hole?
  if (file->getSize() <= 0) {
    return(this->cleanUp("B3D JSON severe error! File size is 0!"));
  }

  // Try to read this file.
  const std::tuple<bool, std::string> parseResult = this->parseJSONFile(file);

  // If it failed to parse, we give up.
  if (!std::get<0>(parseResult)) {
    // Print the reason, and return a nullptr.
    return(this->cleanUp(std::get<1>(parseResult)));
  }

  //? Now JSONDataContainer exists on this object.
  //? So we can commence loading it.

  // Now we can start doing a full parse of the data in the model JSON and build it as an irrlicht model.
  const std::tuple<bool, std::string> loadResult = load();

  // If it failed to load, we give up.
  if (!std::get<0>(loadResult)) {
    // Print the reason, and return a nullptr.
    return(this->cleanUp(std::get<1>(loadResult)));
  } else {
    //todo: We finalize the model here.
  }

  println("We got to the end.");
  //todo: We return the object's AnimatedMesh.
  return nullptr;
}

} // namespace scene
} // namespace irr

/*

! deprecated

? This is old reference material. Remember to remove this before PR is made.

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

*/