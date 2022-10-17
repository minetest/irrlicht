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

TEST_CASE("minimal triangle has correct vertices") {
	irr::IrrlichtDevice* device { irr::createDevice(irr::video::EDT_NULL) };
	irr::scene::ISceneManager* smgr { device->getSceneManager() };
	auto* mesh { smgr->getMesh(
		"source/Irrlicht/tests/assets/minimal_triangle.gltf", "") };

	REQUIRE(mesh != nullptr);
	REQUIRE(mesh->getMeshBufferCount() == 1);
	auto* meshbuf { mesh->getMeshBuffer(0) };
	REQUIRE(meshbuf->getVertexCount() == 3);
	irr::video::S3DVertex* vertices {
		reinterpret_cast<irr::video::S3DVertex*>(
			meshbuf->getVertices()) };
	CHECK(vertices[0].Pos == irr::core::vector3df {0.0f, 0.0f, 0.0f});
	CHECK(vertices[1].Pos == irr::core::vector3df {1.0f, 0.0f, 0.0f});
	CHECK(vertices[2].Pos == irr::core::vector3df {1.0f, 1.0f, 0.0f});

	device->drop();
}

