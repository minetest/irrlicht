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
static T readPrimative(const BufferOffset& readFrom) {
	unsigned char d[sizeof(T)]{};
	for (std::size_t i = 0; i < sizeof(T); ++i) {
		d[i] = readFrom.at(i);
	}
	return *reinterpret_cast<T*>(d);
}

static core::vector3df readVector(const BufferOffset& readFrom) {
	// glTF coordinates are right-handed, minetest ones are left-handed
	// 1 glTF coordinate is equivalent to 10 Irrlicht coordinates
	return core::vector3df(
		-1.0 * readPrimative<float>(readFrom),
		1.0 * readPrimative<float>(BufferOffset(readFrom, sizeof(float))),
		1.0 * readPrimative<float>(BufferOffset(readFrom, 2 * sizeof(float))));
}

static u16* readIndices(const BufferOffset& readFrom, const std::size_t count)
{
	auto* indices = new u16[count]{};
	for (std::size_t i = 0; i < count; ++i) {
		indices[i] = readPrimative<u16>(BufferOffset(readFrom, i * sizeof(u16)));
	}
	return indices;
}

video::S3DVertex* getVertices(const tinygltf::Model& model, const std::size_t accessorId)
{
	const auto& view = model.bufferViews[
		model.accessors[accessorId].bufferView];
	const auto& buffer = model.buffers[view.buffer];
	const auto v1 = readVector(BufferOffset(
		buffer.data, view.byteOffset));
	const auto v2 = readVector(BufferOffset(
		buffer.data, view.byteOffset + (3 * sizeof(float))));
	const auto v3 = readVector(BufferOffset(
		buffer.data, view.byteOffset + (6 * sizeof(float))));

	return (new video::S3DVertex[3] {
		{v1, {0.0f, 0.0f, 1.0f}, {}, {0.0f, 0.0f}},
		{v2, {0.0f, 0.0f, 1.0f}, {}, {1.0f, 0.0f}},
		{v3, {0.0f, 0.0f, 1.0f}, {}, {0.0f, 1.0f}} } );
}

u16* getIndices(const tinygltf::Model& model, const std::size_t accessorId)
{
	const auto& indicesView = model.bufferViews[
		model.accessors[accessorId].bufferView];
	const auto& indicesBuffer = model.buffers[indicesView.buffer];
	return readIndices(
		BufferOffset(indicesBuffer.data, indicesView.byteOffset),
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

	auto* indices = getIndices(model, 0);
	auto* vertices = getVertices(model, 1);

	SMeshBuffer* meshbuf { new SMeshBuffer {} };

	meshbuf->append(vertices, model.accessors[1].count,
		indices, model.accessors[0].count);

	SMesh* mesh { new SMesh {} };
	mesh->addMeshBuffer(meshbuf);

	SAnimatedMesh* animatedMesh { new SAnimatedMesh {} };
	animatedMesh->addMesh(mesh);

	return animatedMesh;
}

} // namespace scene

} // namespace irr

