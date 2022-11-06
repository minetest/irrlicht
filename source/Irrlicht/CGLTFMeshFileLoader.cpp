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

namespace irr
{

namespace scene
{

class BufferOffset
{
public:
	BufferOffset(const std::vector<unsigned char>& buf,
			const std::size_t offset) noexcept
		: m_buf(buf), m_offset(offset)
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

static constexpr float readFloat(const BufferOffset& readFrom) {
	unsigned char d[4]{};
	for (int i = 0; i < 4; ++i) {
		d[i] = readFrom.at(i);
	}
	return *reinterpret_cast<float*>(d);
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

	const auto& view = model.bufferViews[model.accessors[1].bufferView];
	const auto& buffer = model.buffers[view.buffer];
	const core::vector3df v1(
		-readFloat(BufferOffset(buffer.data, view.byteOffset)),
		readFloat(BufferOffset(buffer.data, view.byteOffset + 4)),
		readFloat(BufferOffset(buffer.data, view.byteOffset + 8)));
	const core::vector3df v2(
		-readFloat(BufferOffset(buffer.data, view.byteOffset + 12)),
		readFloat(BufferOffset(buffer.data, view.byteOffset + 16)),
		readFloat(BufferOffset(buffer.data, view.byteOffset + 20)));
	const core::vector3df v3(
		-readFloat(BufferOffset(buffer.data, view.byteOffset + 24)),
		readFloat(BufferOffset(buffer.data, view.byteOffset + 28)),
		readFloat(BufferOffset(buffer.data, view.byteOffset + 32)));

	// sorry Bjarne
	SMeshBuffer* meshbuf { new SMeshBuffer {} };

	const video::S3DVertex* vertices { new video::S3DVertex[3] {
		{v1, {0.0f, 0.0f, 1.0f}, {}, {0.0f, 0.0f}},
		{v2, {0.0f, 0.0f, 1.0f}, {}, {1.0f, 0.0f}},
		{v3, {0.0f, 0.0f, 1.0f}, {}, {0.0f, 1.0f}} } };
	const u16* indices { new u16[3] {0, 2, 1} };
	meshbuf->append(vertices, 3, indices, 3);

	SMesh* mesh { new SMesh {} };
	mesh->addMeshBuffer(meshbuf);

	SAnimatedMesh* animatedMesh { new SAnimatedMesh {} };
	animatedMesh->addMesh(mesh);

	return animatedMesh;
}

bool CGLTFMeshFileLoader::tryParseGLTF(io::IReadFile* file,
		tinygltf::Model& model) const
{
	tinygltf::TinyGLTF loader {};
	std::string err {};
	std::string warn {};

	auto buf = std::make_unique<char[]>(file->getSize());
	file->read(buf.get(), file->getSize());
	return loader.LoadASCIIFromString(
		&model, &err, &warn, buf.get(), file->getSize(), "", 1);
}

} // namespace irr

} // namespace scene

