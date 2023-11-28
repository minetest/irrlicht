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
void println(std::string data) {
  printf((data + "\n").c_str());
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
  return {true, ""};
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
  return {true, ""};
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
  return {true, ""};
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
  
  return {true, ""};
}

/**
 * Returns (success, failure reason).
 * [optional]
*/
std::tuple<bool, std::string> CB3DJSONMeshFileLoader::readChunkBRUS() {

  //? Can skip number of BRUS because JSON. :D
  //? note: BRUS is an array of objects.

  json brus;

  // We're checking the root of the JSON structure here. So we can just peak directly into the root.
  if (JSONDataContainer.contains("BRUS") && JSONDataContainer["BRUS"].is_array()) {

    brus = JSONDataContainer["BRUS"];

  } else {

    // We also want to warn developers if they're using {} instead of [].
    if (JSONDataContainer.contains("BRUS") && !JSONDataContainer["BRUS"].is_array()) {
      return {false, "BRUS is not an array."};
    }

    // Since it's optional, it succeeds if it's not there.
    return {true, ""};
  }

  int index = 0;
  for (auto reference = brus.begin(); reference != brus.end(); ++reference) {

    // b stands for brush.
    json b = *reference;

    if (!b.is_object()) {

      return {false, "BRUS: Element " + std::to_string(index) + " is not an object."};
    }

    // This is still quite strange. Maybe just make this a reference because it would be pointing to the same memory address.
    Materials.push_back(SB3dMaterial());
    SB3dMaterial& B3DMaterial = Materials.getLast();

    // This part should probably be it's own function.
    //todo: look into making this it's own function.

    //* Red.
    if (b.contains("red") && b["red"].is_number()) {

      B3DMaterial.red = b["red"];

    } else {

      if (!b.contains("red")) {
        return {false, "BRUS: Element (" + std::to_string(index) + ") is missing \"red\"."};
      }

      return {false, "BRUS: Element (" + std::to_string(index) + ") \"red\" is not a number."};
    }

    //* Green.
    if (b.contains("green") && b["green"].is_number()) {
      B3DMaterial.green = b["green"];
    } else {

      if (!b.contains("green")) {
        return {false, "BRUS: Element (" + std::to_string(index) + ") is missing \"green\"."};
      }

      return {false, "BRUS: Element (" + std::to_string(index) + ") \"green\" is not a number."};
    }

    //* Blue.
    if (b.contains("blue") && b["blue"].is_number()) {

      B3DMaterial.blue = b["blue"];

    } else {

      if (!b.contains("blue")) {
        return {false, "BRUS: Element (" + std::to_string(index) + ") is missing \"blue\"."};
      }

      return {false, "BRUS: Element (" + std::to_string(index) + ") \"blue\" is not a number."};
    }

    //* Alpha.
    if (b.contains("alpha") && b["alpha"].is_number()) {
      B3DMaterial.alpha = b["alpha"];
    } else {

      if (!b.contains("alpha")) {
        return {false, "BRUS: Element (" + std::to_string(index) + ") is missing \"alpha\"."};
      }

      return {false, "BRUS: Element (" + std::to_string(index) + ") \"alpha\" is not a number."};
    }

    //* Blend.
    if (b.contains("blend") && b["blend"].is_number_integer()) {

      B3DMaterial.blend = b["blend"];

    } else {

      if (!b.contains("blend")) {
        return {false, "BRUS: Element (" + std::to_string(index) + ") is missing \"blend\"."};
      }

      return {false, "BRUS: Element (" + std::to_string(index) + ") \"blend\" is not an integer."};
    }

    //* FX.
    if (b.contains("fx") && b["fx"].is_number_integer()) {

      B3DMaterial.fx = b["fx"];

    } else {

      if (!b.contains("fx")) {
        return {false, "BRUS: Element (" + std::to_string(index) + ") is missing \"fx\"."};
      }

      return {false, "BRUS: Element (" + std::to_string(index) + ") \"fx\" is not an integer."};
    }

    //* Textures Array.
    if (b.contains("textures") && b["textures"].is_array()) {

      json internalArray = b["textures"];

      int internalIndex = 0;
      for (auto internalReference = internalArray.begin(); internalReference != internalArray.end(); ++internalReference) {

        json texName = *internalReference;

        if (!texName.is_string()) {
          return {false, "BRUS: Element (" + std::to_string(index) + ") texture element (" + std::to_string(internalIndex) + ") is not a string."};
        }

        std::string finalizedName = texName;

        println("this is what we got: " + finalizedName);

        internalIndex++;
      }
      
    } else {

      if (!b.contains("textures")) {
        return {false, "BRUS: Element (" + std::to_string(index) + ") is missing \"textures\"."};
      }

      return {false, "BRUS: Element (" + std::to_string(index) + ") is not an array."};
    }

    index++;
  }
  
  return {true, ""};
}

/**
 * Returns (success, failure reason).
 * [optional]
*/
std::tuple<bool, std::string> CB3DJSONMeshFileLoader::readChunkTEXS() {

  //? note: TEXS is an array of objects.

  // We're checking the root of the JSON structure here. So we can just peak directly into the root.
  json texs;
  if (JSONDataContainer.contains("TEXS") && JSONDataContainer["TEXS"].is_array()) {
    // We're referencing static memory.
    texs = JSONDataContainer["TEXS"];
  } else {

    // We also want to warn developers if they're using {} instead of [].
    if (JSONDataContainer.contains("TEXS") && !JSONDataContainer["TEXS"].is_array()) {
      return {false, "TEXS is not an array."};
    }

    // Since it's optional, it succeeds if it's not there.
    return {true, ""};
  }

  int index = 0;
  for (auto reference = texs.begin(); reference != texs.end(); ++reference) {

    // t stands for texture.
    json t = *reference;
    
    if (!t.is_object()) {
      return {false, "TEXS: Element " + std::to_string(index) + " is not an object."};
    }
    
    // This is a very strange way to do this but I won't complain.
    // Create the texture object inside of the Mesh.
    Textures.push_back(SB3dTexture());
    SB3dTexture& B3DTexture = Textures.getLast();

    
    // This part should probably be it's own function.
    //todo: look into making this it's own function.

    // This part is a bit complex, for ease of use for the plugin dev/modders advantage.

    //* Name.
    if (t.contains("name") && t["name"].is_string()) {

      // Strings are implicitly converted.
      std::string textureName = t["name"];

      B3DTexture.TextureName = textureName.c_str();

      // Convert Windows '\' to Posix '/'.
      B3DTexture.TextureName.replace('\\','/');

    } else {

      if (t.contains("name") && !t["name"].is_string()) {
        return {false, "\"name\" in TEXS block index (" + std::to_string(index) +") is not a string."};
      }

      return {false, "Missing \"name\" in TEXS block index (" + std::to_string(index) +")."};
    }

    //* Flags.
    if (t.contains("flags") && t["flags"].is_number_integer()) {

      B3DTexture.Flags = t["flags"];

    } else {

      if (t.contains("flags") && !t["flags"].is_number_integer()) {
        return {false, "\"flags\" in TEXS block index(" + std::to_string(index)+") is not an integer."};
      }

      return {false, "Missing \"flags\" in TEXS block index(" + std::to_string(index)+")."};
    }

    //* Blend.
    if (t.contains("blend") && t["blend"].is_number_integer()) {

      B3DTexture.Blend = t["blend"];

    } else {

      if (t.contains("blend") && !t["blend"].is_number_integer()) {
        return {false, "\"blend\" in TEXS block index(" + std::to_string(index)+") is not an integer."};
      }
            
      return {false, "Missing \"blend\" in TEXS block index(" + std::to_string(index)+")."};
    }

    //* Position.
    if (t.contains("pos") && t["pos"].is_array()) {

      irr::core::vector2df pos {0,0};

      auto posSuccess = grabVec2f(t, "pos", pos);

      // Something went horribly wrong.
      if (!std::get<0>(posSuccess)) {

        return {false, "TEXS: " + std::get<1>(posSuccess)};
      }

      // Success.
      B3DTexture.Xpos = pos.X;
      B3DTexture.Ypos = pos.Y;

    } else {

      if (t.contains("pos") && !t["pos"].is_array()) {
        return {false, "\"pos\" in TEXS block index(" + std::to_string(index)+") is not an array."};
      }

      if (!t.contains("pos") ) {
        return {false, "\"pos\" in TEXS block index(" + std::to_string(index)+") is missing."};
      }

      return {false, "Malformed \"pos\" in TEXS block index (" + std::to_string(index) + "). Must be an array with 2 numbers."};
    }

    //* Scale.
    if (t.contains("scale") && t["scale"].is_array()) {

      irr::core::vector2df scale {0,0};

      auto scaleSuccess = grabVec2f(t, "scale", scale);

      // Something went horribly wrong.
      if (!std::get<0>(scaleSuccess)) {

        return {false, "TEXS: " + std::get<1>(scaleSuccess)};
      }

      // Success.
      B3DTexture.Xscale = scale.X;
      B3DTexture.Yscale = scale.Y;

      // println(std::to_string(B3DTexture.Xscale) + " " + std::to_string(B3DTexture.Yscale));

    } else {

      if (t.contains("scale") && !t["scale"].is_array()) {
        return {false, "\"scale\" in TEXS block index(" + std::to_string(index)+") is not an array."};
      }

      if (!t.contains("scale") ) {
        return {false, "\"scale\" in TEXS block index(" + std::to_string(index)+") is missing."};
      }

      return {false, "Malformed \"scale\" in TEXS block index (" + std::to_string(index) + "). Must be an array with 2 numbers."};
    }

    //* Angle.
    if (t.contains("angle") && t["angle"].is_number()) {

      B3DTexture.Angle = t["angle"];

    } else {

      if (t.contains("angle")) {
        return {false, "\"angle\" in TEXS block index (" + std::to_string(index) + ") is not a number."};
      }

      return {false, "\"angle\" in TEXS block index (" + std::to_string(index) + ") is missing."};
    }


    index++;
  }
  // } else {
  //   // it is malformed.
  //   //todo: return 
  //   return {false, "Malformed TEXS block. \"textures\" is not an array."} ;
  // }

  // Everything succeeds, yay!
  return {true, ""};
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

  // Grab TEXS (textures).
  std::tuple<bool, std::string> texturesSuccess = this->readChunkTEXS();

  if (!std::get<0>(texturesSuccess)) {
    return {false, std::get<1>(texturesSuccess)};
  }

  // Grab BRUS (brushes). Defines where materials go.
  std::tuple<bool, std::string> brushesSuccess = this->readChunkBRUS();

  if (!std::get<0>(brushesSuccess)) {
    return {false, std::get<1>(brushesSuccess)};
  }



  

  // return animatedMesh;
  return {true, ""};
}

/**
 * Automatically cleans and returns the nullptr so it can be inlined or chained.
*/
CSkinnedMesh* CB3DJSONMeshFileLoader::cleanUp(std::string failure) {
  os::Printer::log(failure.c_str(), ELL_WARNING);
  //! This causes a segmentation fault.
  if (AnimatedMesh != nullptr) {
    AnimatedMesh->drop();
    AnimatedMesh = 0;
  }
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
    return {false, failureReason};
  }

  // I'm not sure if buffer and output gets dropped here.
  return {true, ""};
}

IAnimatedMesh* CB3DJSONMeshFileLoader::createMesh(io::IReadFile* file) {
  
  // Create the initial model. (if you delete this line, you will have random segfaults.)
  AnimatedMesh = new scene::CSkinnedMesh();

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