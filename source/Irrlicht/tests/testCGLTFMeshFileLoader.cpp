#include "inMemoryFile.h"

#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <irrlicht.h>

TEST_CASE("load empty gltf file") {
	irr::IrrlichtDevice* device { irr::createDevice(irr::video::EDT_NULL) };
	irr::scene::ISceneManager* smgr { device->getSceneManager() };

	irr::io::InMemoryFile filebuf{"test.gltf", ""};

	auto* mesh { smgr->getMesh(&filebuf) };
	REQUIRE(mesh == nullptr);

	device->drop();
}

