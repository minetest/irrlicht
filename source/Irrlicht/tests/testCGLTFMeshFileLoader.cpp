#include "inMemoryFile.h"

#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <irrlicht.h>

TEST_CASE("load empty gltf file") {
	irr::IrrlichtDevice* device { irr::createDevice(irr::video::EDT_NULL) };
	irr::scene::ISceneManager* smgr { device->getSceneManager() };

	irr::io::InMemoryFile filebuf {"test.gltf", ""};

	auto* mesh { smgr->getMesh(&filebuf) };
	CHECK(mesh == nullptr);

	device->drop();
}

TEST_CASE("load minimal triangle") {
	irr::IrrlichtDevice* device { irr::createDevice(irr::video::EDT_NULL) };
	irr::scene::ISceneManager* smgr { device->getSceneManager() };

	auto* mesh { smgr->getMesh(
		"source/Irrlicht/tests/assets/minimal_triangle.gltf", "") };
	REQUIRE(mesh != nullptr);

	device->drop();
}

