#pragma once

#include "IAnimatedMesh.h"
#include "IMeshLoader.h"
#include "CSkinnedMesh.h"
#include "IReadFile.h"
#include "SB3DStructs.h"
#include "path.h"
#include "json/json.hpp"
#include <tuple>

using json = nlohmann::json;

namespace irr
{

namespace scene
{

class CB3DJSONMeshFileLoader : public IMeshLoader
{
private:
  // Fields.
  CSkinnedMesh*	AnimatedMesh;
  core::array<SB3dTexture> Textures;
  core::array<SB3dMaterial> Materials;
  /*
  Quick note about JSON.
  This is static memory, it's a static memory address so I do not think
  that this needs to be freed.
  So once this model goes out of scope, I'm pretty sure that this will
  be pointing to the next model. Or maybe the last model loaded.
  */
  json JSONDataContainer;

  // Methods.
  CSkinnedMesh* cleanUp(std::string);
  std::tuple<bool, std::string> parseJSONFile(io::IReadFile* file);
  std::tuple<bool, std::string> load();
  std::tuple<bool, std::string> readChunkTEXS();
  std::tuple<bool, std::string> readChunkBRUS();

public:

  CB3DJSONMeshFileLoader();

  bool isALoadableFileExtension(const io::path& fileName) const override;

  IAnimatedMesh* createMesh(io::IReadFile* file) override;

};// class CB3DJSONMeshFileLoader
} // namespace scene
} // namespace irr