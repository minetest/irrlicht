#include "CGLTFMeshFileLoader.h"
#include "CMeshBuffer.h"
#include "coreutil.h"
#include "IAnimatedMesh.h"
#include "ILogger.h"
#include "IReadFile.h"
#include "irrTypes.h"
#include "os.h"
#include "path.h"
#include "S3DVertex.h"
#include "SAnimatedMesh.h"
#include "SColor.h"
#include "SMesh.h"
#include "vector3d.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION
#include "tinygltf/tiny_gltf.h"

#include <cstddef>
#include <cstring>
#include <memory>
#include <string>
#include <utility>
#include <vector>

/* Notes on the coordinate system.
 *
 * glTF uses a right-handed coordinate system where +Z is the
 * front-facing axis, and Irrlicht uses a left-handed coordinate
 * system where -Z is the front-facing axis.
 * We convert between them by reflecting the mesh across the X axis.
 * Doing this correctly requires negating the Z coordinate on
 * vertex positions and normals, and reversing the winding order
 * of the vertex indices.
 */


namespace irr {
namespace scene {

CGLTFMeshFileLoader::BufferOffset::BufferOffset(
		const std::vector<unsigned char>& buf,
		const std::size_t offset)
	: m_buf(buf)
	, m_offset(offset)
{
}

CGLTFMeshFileLoader::BufferOffset::BufferOffset(
		const CGLTFMeshFileLoader::BufferOffset& other,
		const std::size_t fromOffset)
	: m_buf(other.m_buf)
	, m_offset(other.m_offset + fromOffset)
{
}

unsigned char CGLTFMeshFileLoader::BufferOffset::at(
		const std::size_t fromOffset) const
{
	return m_buf.at(m_offset + fromOffset);
}

CGLTFMeshFileLoader::CGLTFMeshFileLoader() noexcept
{
}

bool CGLTFMeshFileLoader::isALoadableFileExtension(
		const io::path& filename) const
{
	return core::hasFileExtension(filename, "gltf");
}

IAnimatedMesh* CGLTFMeshFileLoader::createMesh(io::IReadFile* file)
{
	tinygltf::Model model {};
	if (file->getSize() <= 0 || !tryParseGLTF(file, model)) {
		return nullptr;
	}

	MeshExtractor parser(std::move(model));
	SMesh* baseMesh(new SMesh {});
	loadPrimitives(parser, baseMesh);

	SAnimatedMesh* animatedMesh(new SAnimatedMesh {});
	animatedMesh->addMesh(baseMesh);

	return animatedMesh;
}

void CGLTFMeshFileLoader::loadPrimitives(
		const MeshExtractor& parser,
		SMesh* mesh)
{
	for (std::size_t i = 0; i < parser.getMeshCount(); ++i) {
		for (std::size_t j = 0; j < parser.getPrimitiveCount(i); ++j) {
			auto indices = parser.getIndices(i, j);
			auto vertices = parser.getVertices(i, j);

			SMeshBuffer* meshbuf(new SMeshBuffer {});
			meshbuf->append(vertices.data(), vertices.size(),
				indices.data(), indices.size());
			mesh->addMeshBuffer(meshbuf);
		}
	}
}

CGLTFMeshFileLoader::MeshExtractor::MeshExtractor(
		const tinygltf::Model& model) noexcept
	: m_model(model)
{
}

CGLTFMeshFileLoader::MeshExtractor::MeshExtractor(
		const tinygltf::Model&& model) noexcept
	: m_model(model)
{
}

std::vector<u16> CGLTFMeshFileLoader::MeshExtractor::getIndices(
		const std::size_t meshIdx,
		const std::size_t primitiveIdx) const
{
	const auto accessorIdx = getIndicesAccessorIdx(meshIdx, primitiveIdx);
	const auto& buf = getBuffer(accessorIdx);

	std::vector<u16> indices{};
	const auto count = getElemCount(accessorIdx);
	for (std::size_t i = 0; i < count; ++i) {
		std::size_t elemIdx = count - i - 1;
		indices.push_back(readPrimitive<u16>(
			BufferOffset(buf, elemIdx * sizeof(u16))));
	}

	return indices;
}

std::vector<video::S3DVertex> CGLTFMeshFileLoader::MeshExtractor::getVertices(
		const std::size_t meshIdx,
		const std::size_t primitiveIdx) const
{
	const auto positionAccessorIdx = getPositionAccessorIdx(
			meshIdx, primitiveIdx);
	std::vector<vertex_t> vertices{};
	vertices.resize(getElemCount(positionAccessorIdx));
	copyPositions(positionAccessorIdx, vertices);

	const auto normalAccessorIdx = getNormalAccessorIdx(
			meshIdx, primitiveIdx);
	if (normalAccessorIdx != static_cast<std::size_t>(-1)) {
		copyNormals(normalAccessorIdx, vertices);
	}

	const auto tCoordAccessorIdx = getTCoordAccessorIdx(
			meshIdx, primitiveIdx);
	if (tCoordAccessorIdx != static_cast<std::size_t>(-1)) {
		copyTCoords(tCoordAccessorIdx, vertices);
	}

	return vertices;
}

std::size_t CGLTFMeshFileLoader::MeshExtractor::getMeshCount() const
{
	return m_model.meshes.size();
}

std::size_t CGLTFMeshFileLoader::MeshExtractor::getPrimitiveCount(
		const std::size_t meshIdx) const
{
	return m_model.meshes[meshIdx].primitives.size();
}


template <typename T>
T CGLTFMeshFileLoader::MeshExtractor::readPrimitive(
		const BufferOffset& readFrom)
{
	unsigned char d[sizeof(T)]{};
	for (std::size_t i = 0; i < sizeof(T); ++i) {
		d[i] = readFrom.at(i);
	}
	T dest;
	std::memcpy(&dest, d, sizeof(dest));
	return dest;
}

core::vector2df CGLTFMeshFileLoader::MeshExtractor::readVec2DF(
		const CGLTFMeshFileLoader::BufferOffset& readFrom)
{
	return core::vector2df(readPrimitive<float>(readFrom),
		readPrimitive<float>(BufferOffset(readFrom, sizeof(float))));

}

core::vector3df CGLTFMeshFileLoader::MeshExtractor::readVec3DF(
		const BufferOffset& readFrom,
		const float scale)
{
	return core::vector3df(
		scale * readPrimitive<float>(readFrom),
		scale * readPrimitive<float>(BufferOffset(readFrom, sizeof(float))),
		-scale * readPrimitive<float>(BufferOffset(readFrom, 2 *
		sizeof(float))));
}

void CGLTFMeshFileLoader::MeshExtractor::copyPositions(
		const std::size_t accessorIdx,
		std::vector<vertex_t>& vertices) const
{

	const auto& buffer = getBuffer(accessorIdx);
	const auto count = getElemCount(accessorIdx);

	for (std::size_t i = 0; i < count; i++) {
		const auto v = readVec3DF(BufferOffset(buffer,
			3 * sizeof(float) * i), getScale());
		vertices[i].Pos = v;
	}
}

void CGLTFMeshFileLoader::MeshExtractor::copyNormals(
		const std::size_t accessorIdx,
		std::vector<vertex_t>& vertices) const
{
	const auto& buffer = getBuffer(accessorIdx);
	const auto count = getElemCount(accessorIdx);

	for (std::size_t i = 0; i < count; i++) {
		const auto n = readVec3DF(BufferOffset(buffer,
			3 * sizeof(float) * i));
		vertices[i].Normal = n;
	}
}

void CGLTFMeshFileLoader::MeshExtractor::copyTCoords(
		const std::size_t accessorIdx,
		std::vector<vertex_t>& vertices) const
{
	const auto& buffer = getBuffer(accessorIdx);
	const auto count = getElemCount(accessorIdx);

	for (std::size_t i = 0; i < count; ++i) {
		const auto t = readVec2DF(BufferOffset(buffer,
			2 * sizeof(float) * i));
		vertices[i].TCoords = t;
	}
}

float CGLTFMeshFileLoader::MeshExtractor::getScale() const
{
	if (m_model.nodes[0].scale.size() > 0) {
		return static_cast<float>(m_model.nodes[0].scale[0]);
	}

	return 1.0f;
}

std::size_t CGLTFMeshFileLoader::MeshExtractor::getElemCount(
		const std::size_t accessorIdx) const
{
	return m_model.accessors[accessorIdx].count;
}

CGLTFMeshFileLoader::BufferOffset CGLTFMeshFileLoader::MeshExtractor::getBuffer(
		const std::size_t accessorIdx) const
{
	const auto& accessor = m_model.accessors[accessorIdx];
	const auto& view = m_model.bufferViews[accessor.bufferView];
	const auto& buffer = m_model.buffers[view.buffer];

	return BufferOffset(buffer.data, view.byteOffset);
}

std::size_t CGLTFMeshFileLoader::MeshExtractor::getIndicesAccessorIdx(
		const std::size_t meshIdx,
		const std::size_t primitiveIdx) const
{
	return m_model.meshes[meshIdx].primitives[primitiveIdx].indices;
}

std::size_t CGLTFMeshFileLoader::MeshExtractor::getPositionAccessorIdx(
		const std::size_t meshIdx,
		const std::size_t primitiveIdx) const
{
	return m_model.meshes[meshIdx].primitives[primitiveIdx]
		.attributes.find("POSITION")->second;
}

std::size_t CGLTFMeshFileLoader::MeshExtractor::getNormalAccessorIdx(
		const std::size_t meshIdx,
		const std::size_t primitiveIdx) const
{
	const auto& attributes = m_model.meshes[meshIdx]
		.primitives[primitiveIdx].attributes;
	const auto result = attributes.find("NORMAL");

	if (result == attributes.end()) {
		return -1;
	} else {
		return result->second;
	}
}

std::size_t CGLTFMeshFileLoader::MeshExtractor::getTCoordAccessorIdx(
		const std::size_t meshIdx,
		const std::size_t primitiveIdx) const
{
	const auto& attributes = m_model.meshes[meshIdx]
		.primitives[primitiveIdx].attributes;
	const auto result = attributes.find("TEXCOORD_0");

	if (result == attributes.end()) {
		return -1;
	} else {
		return result->second;
	}
}

bool CGLTFMeshFileLoader::tryParseGLTF(io::IReadFile* file,
		tinygltf::Model& model)
{
	tinygltf::TinyGLTF loader {};

	// Stop embedded textures from making model fail to load
	loader.SetImageLoader(nullptr, nullptr);

	std::string err {};
	std::string warn {};

	auto buf = std::make_unique<char[]>(file->getSize());
	file->read(buf.get(), file->getSize());

        if (warn != "") {
                os::Printer::log(warn.c_str(), ELL_WARNING);
        }

	if (err != "") {
                os::Printer::log(err.c_str(), ELL_ERROR);
		return false;
	}

	return loader.LoadASCIIFromString(&model, &err, &warn, buf.get(),
		file->getSize(), "", 1);
}

} // namespace scene

} // namespace irr