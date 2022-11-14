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
#include <cstring>
#include <memory>
#include <string>

template <class T>
struct Span
{
	T* buffer = nullptr;
	std::size_t size = 0;
};

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
static T readPrimitive(const BufferOffset& readFrom)
{
	unsigned char d[sizeof(T)]{};
	for (std::size_t i = 0; i < sizeof(T); ++i) {
		d[i] = readFrom.at(i);
	}
	T dest;
	std::memcpy(&dest, d, sizeof(dest));
	return dest;
}

static core::vector2df readVec2DF(const BufferOffset& readFrom)
{
	return core::vector2df(
		readPrimitive<float>(readFrom),
		readPrimitive<float>(BufferOffset(readFrom, sizeof(float))));

}

static core::vector3df readVec3DF(const BufferOffset& readFrom,
		const float scale = 1.0f)
{
	// glTF's coordinate system is right-handed, Irrlicht's is left-handed
	// glTF's +Z axis corresponds to Irrlicht's -Z axis
	return core::vector3df(
		scale * readPrimitive<float>(readFrom),
		scale * readPrimitive<float>(BufferOffset(readFrom, sizeof(float))),
		-scale * readPrimitive<float>(BufferOffset(readFrom, 2 * sizeof(float))));
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

static void copyPositions(const tinygltf::Model& model,
		const Span<video::S3DVertex> vertices,
		const std::size_t accessorId)
{
	const auto& view = model.bufferViews[
		model.accessors[accessorId].bufferView];
	const auto& buffer = model.buffers[view.buffer];
	for (std::size_t i = 0; i < model.accessors[accessorId].count; ++i) {
		const auto v = readVec3DF(BufferOffset(
			buffer.data, view.byteOffset + 3 * sizeof(float) * i),
			getScale(model));
		vertices.buffer[i].Pos = v;
	}
}

static void copyNormals(const tinygltf::Model& model,
		const Span<video::S3DVertex> vertices,
		const std::size_t accessorId)
{
	const auto& view = model.bufferViews[
		model.accessors[accessorId].bufferView];
	const auto& buffer = model.buffers[view.buffer];
	for (std::size_t i = 0; i < model.accessors[accessorId].count; ++i) {
		const auto n = readVec3DF(BufferOffset(
			buffer.data, view.byteOffset + 3 * sizeof(float) * i));
		vertices.buffer[i].Normal = n;
	}
}

static void copyTCoords(const tinygltf::Model& model,
		const Span<video::S3DVertex> vertices,
		const std::size_t accessorId)
{
	const auto& view = model.bufferViews[
		model.accessors[accessorId].bufferView];
	const auto& buffer = model.buffers[view.buffer];
	for (std::size_t i = 0; i < model.accessors[accessorId].count; ++i) {
		const auto t = readVec2DF(BufferOffset(
			buffer.data, view.byteOffset + 2 * sizeof(float) * i));
		vertices.buffer[i].TCoords = t;
	}
}

static video::S3DVertex* getVertices(const tinygltf::Model& model,
		const std::size_t accessorId)
{
	auto* vertexBuffer = new video::S3DVertex[
		model.accessors[accessorId].count]{};
	Span<video::S3DVertex> vertices{
		vertexBuffer, model.accessors[accessorId].count};
	copyPositions(model, vertices, accessorId);

	const auto normalsField
		= model.meshes[0].primitives[0].attributes.find("NORMAL");
	if (normalsField != model.meshes[0].primitives[0].attributes.end()) {
		copyNormals(model, vertices, normalsField->second);
	}

	const auto tCoordsField
		= model.meshes[0].primitives[0].attributes.find("TEXCOORD_0");
	if (tCoordsField != model.meshes[0].primitives[0].attributes.end()) {
		copyTCoords(model, vertices, tCoordsField->second);
	}

	return vertexBuffer;
}

static u16* getIndices(const tinygltf::Model& model,
		const std::size_t accessorId)
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

