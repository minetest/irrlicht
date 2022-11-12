#include "CGLTFMeshFileLoader.h"
#include "CMeshBuffer.h"
#include "coreutil.h"
#include "IAnimatedMesh.h"
#include "IReadFile.h"
#include "irrTypes.h"
#include "path.h"
#include "S3DVertex.h"
#include "SAnimatedMesh.h"
#include "SColor.h"
#include "SMesh.h"
#include "vector3d.h"

#define TINYGLTF_IMPLEMENTATION
#include <tiny_gltf.h>

#include <cstddef>
#include <memory>
#include <string>

class BufferOffset
{
public:
	BufferOffset(const std::vector<unsigned char>& buf,
			const std::size_t offset)
		: m_buf(buf)
		, m_offset(offset)
	{
	}

	BufferOffset(const BufferOffset& other, const std::size_t fromOffset)
		: m_buf(other.m_buf)
		, m_offset(other.m_offset + fromOffset)
	{
	}

	unsigned char at(const std::size_t fromOffset) const
	{
		return m_buf.at(m_offset + fromOffset);
	}

private:
	const std::vector<unsigned char>& m_buf;
	std::size_t m_offset;
};

namespace irr
{

namespace scene
{

static bool tryParseGLTF(io::IReadFile* file, tinygltf::Model& model)
{
	tinygltf::TinyGLTF loader {};
	std::string err {};
	std::string warn {};

	auto buf = std::make_unique<char[]>(file->getSize());
	file->read(buf.get(), file->getSize());
	return loader.LoadASCIIFromString(
		&model, &err, &warn, buf.get(), file->getSize(), "", 1);
}

template <class T>
static T readPrimitive(const BufferOffset& readFrom) {
	unsigned char d[sizeof(T)]{};
	for (std::size_t i = 0; i < sizeof(T); ++i) {
		d[i] = readFrom.at(i);
	}
	return *reinterpret_cast<T*>(d);
}

static core::vector3df readVector(const BufferOffset& readFrom, const float scale) {
	// glTF coordinates are right-handed, minetest ones are left-handed
	// 1 glTF coordinate is equivalent to 10 Irrlicht coordinates
	return core::vector3df(
		-scale * readPrimitive<float>(readFrom),
		scale * readPrimitive<float>(BufferOffset(readFrom, sizeof(float))),
		scale * readPrimitive<float>(BufferOffset(readFrom, 2 * sizeof(float))));
}

static u16* readIndices(const BufferOffset& readFrom, const std::size_t count)
{
	auto* indices = new u16[count]{};
	for (std::size_t i = 0; i < count; ++i) {
		indices[i] = readPrimitive<u16>(BufferOffset(readFrom, i * sizeof(u16)));
	}
	return indices;
}


float getScale(const tinygltf::Model& model)
{
	if (model.nodes[0].scale.size() > 0) {
		return static_cast<float>(model.nodes[0].scale[0]);
	}
	return 1.0f;
}

video::S3DVertex* getVertices(const tinygltf::Model& model, const std::size_t accessorId)
{
	const auto& view = model.bufferViews[
		model.accessors[accessorId].bufferView];
	const auto& buffer = model.buffers[view.buffer];

	auto* vertices = new video::S3DVertex[model.accessors[accessorId].count];

	for (std::size_t i = 0; i < model.accessors[accessorId].count; ++i) {
		const auto v = readVector(BufferOffset(
			buffer.data, view.byteOffset + 3 * sizeof(float) * i), getScale(model));
		vertices[i] = {v, {0.0f, 0.0f, 0.0f}, {}, {0.0f, 0.0f}};
	}

	return vertices;
}

u16* getIndices(const tinygltf::Model& model, const std::size_t accessorId)
{
	const auto& view = model.bufferViews[
		model.accessors[accessorId].bufferView];
	const auto& indicesBuffer = model.buffers[view.buffer];
	return readIndices(
		BufferOffset(indicesBuffer.data, view.byteOffset),
		model.accessors[0].count);
}

CGLTFMeshFileLoader::CGLTFMeshFileLoader()
{
}

bool CGLTFMeshFileLoader::isALoadableFileExtension(
	const io::path& filename) const
{
	return core::hasFileExtension(filename, "gltf");
}

IAnimatedMesh* CGLTFMeshFileLoader::createMesh(io::IReadFile* file)
{
	tinygltf::Model model{};

	if (file->getSize() == 0 || !tryParseGLTF(file, model)) {
		return nullptr;
	}

	const auto indicesAccessorId =
		model.meshes[0].primitives[0].indices;
	const auto positionAccessorId =
		model.meshes[0].primitives[0].attributes["POSITION"];

	auto* indices = getIndices(model, indicesAccessorId);
	auto* vertices = getVertices(model, positionAccessorId);

	SMeshBuffer* meshbuf { new SMeshBuffer {} };

	meshbuf->append(vertices, model.accessors[positionAccessorId].count,
		indices, model.accessors[indicesAccessorId].count);

	SMesh* mesh { new SMesh {} };
	mesh->addMeshBuffer(meshbuf);

	SAnimatedMesh* animatedMesh { new SAnimatedMesh {} };
	animatedMesh->addMesh(mesh);

	return animatedMesh;
}

} // namespace scene

} // namespace irr

