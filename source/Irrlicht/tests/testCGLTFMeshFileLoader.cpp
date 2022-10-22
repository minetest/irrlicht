#include "inMemoryFile.h"

#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <irrlicht.h>

class ScopedMesh
{
public:
	ScopedMesh(irr::io::IReadFile* file)
		: m_device { irr::createDevice(irr::video::EDT_NULL) }
		, m_mesh { nullptr }
	{
		auto* smgr = m_device->getSceneManager();
		m_mesh = smgr->getMesh(file);
	}

	ScopedMesh(const irr::io::path& filepath)
		: m_device { irr::createDevice(irr::video::EDT_NULL) }
		, m_mesh { nullptr }
	{
		auto* smgr = m_device->getSceneManager();
		m_mesh = smgr->getMesh(filepath, "");
	}

	~ScopedMesh()
	{
		m_device->drop();
		m_mesh = nullptr;
	}

	const irr::scene::IAnimatedMesh* getMesh() const
	{
		return m_mesh;
	}

private:
	irr::IrrlichtDevice* m_device;
	irr::scene::IAnimatedMesh* m_mesh;
};

TEST_CASE("load empty gltf file") {
	irr::io::InMemoryFile filebuf {"test.gltf", ""};
	ScopedMesh sm { &filebuf };
	CHECK(sm.getMesh() == nullptr);
}

TEST_CASE("minimal triangle has correct vertices") {
	ScopedMesh sm { "source/Irrlicht/tests/assets/minimal_triangle.gltf" };

	auto* mesh = sm.getMesh();
	REQUIRE(mesh != nullptr);
	REQUIRE(mesh->getMeshBufferCount() == 1);
	auto* meshbuf = mesh->getMeshBuffer(0);
	REQUIRE(meshbuf->getVertexCount() == 3);
	auto* vertices = reinterpret_cast<irr::video::S3DVertex*>(
		meshbuf->getVertices());
	CHECK(vertices[0].Pos == irr::core::vector3df {0.0f, 0.0f, 0.0f});
	CHECK(vertices[1].Pos == irr::core::vector3df {0.0f, 1.0f, 0.0f});
	CHECK(vertices[2].Pos == irr::core::vector3df {-1.0f, 0.0f, 0.0f});
}

TEST_CASE("mesh loader returns nullptr when given null file pointer") {
	ScopedMesh sm { nullptr };
	CHECK(sm.getMesh() == nullptr);
}

TEST_CASE("invalid JSON returns nullptr") {
	SECTION("missing closing brace") {
		irr::io::InMemoryFile filebuf {"test.gltf", "{"};
		ScopedMesh sm { &filebuf };
		CHECK(sm.getMesh() == nullptr);
	}

	SECTION("missing colon") {
		irr::io::InMemoryFile filebuf {"test.gltf", "{\"a\" \"b\"}"};
		ScopedMesh sm { &filebuf };
		CHECK(sm.getMesh() == nullptr);
	}
}

