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
#include <tuple>
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

// A helper function to disable tinygltf embedded image loading
static bool dummyImageLoader(tinygltf::Image *a,
		const int b, std::string *c,
		std::string *d,
		int e,
		int f,
		const unsigned char * g,
		int h,
		void *userPointer)
{
	return false;
};

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

	ModelParser parser(std::move(model));
	SMesh* baseMesh(new SMesh {});

	for (std::size_t i = 0; i < parser.getMeshCount(); ++i) {
		for (std::size_t j = 0; j < parser.getPrimitiveCount(i); ++j) {
			auto indices = parser.getIndices(i, j);
			auto vertices = parser.getVertices(i, j);

			SMeshBuffer* meshbuf(new SMeshBuffer {});
			meshbuf->append(vertices.data(), vertices.size(),
				indices.data(), indices.size());
			baseMesh->addMeshBuffer(meshbuf);
		}
	}

	SAnimatedMesh* animatedMesh(new SAnimatedMesh {});
	animatedMesh->addMesh(baseMesh);

	return animatedMesh;
}

CGLTFMeshFileLoader::ModelParser::ModelParser(
		const tinygltf::Model& model)
	: m_model(model)
{
}

CGLTFMeshFileLoader::ModelParser::ModelParser(
		const tinygltf::Model&& model)
	: m_model(model)
{
}

std::vector<u16> CGLTFMeshFileLoader::ModelParser::getIndices(
		const std::size_t meshIdx,
		const std::size_t primitiveIdx) const
{
	auto accessorIdx = getIndicesAccessorIdx(meshIdx, primitiveIdx);
	auto buf = getBuffer(meshIdx, primitiveIdx, accessorIdx);

	std::vector<u16> indices{};
	const auto count = getElemCount(accessorIdx);
	for (std::size_t i = 0; i < count; ++i) {
		std::size_t elemIdx = count - i - 1;
		indices.push_back(readPrimitive<u16>(
			BufferOffset(buf, elemIdx * sizeof(u16))));
	}

	return indices;
}

std::vector<video::S3DVertex> CGLTFMeshFileLoader::ModelParser::getVertices(
		const std::size_t meshIdx,
		const std::size_t primitiveIdx) const
{
	auto positionAccessorIdx = getPositionAccessorIdx(meshIdx, primitiveIdx);
	auto vertexCount = getElemCount(positionAccessorIdx);
	std::vector<video::S3DVertex> vertices{};
	vertices.resize(vertexCount);

	Span<video::S3DVertex> outVertices {vertices.data(), vertexCount};

	copyPositions(outVertices, positionAccessorIdx);

	const auto normalsField = m_model.meshes[meshIdx]
		.primitives[primitiveIdx].attributes.find("NORMAL");

	if (normalsField != m_model.meshes[meshIdx]
		.primitives[primitiveIdx].attributes.end()) {
		copyNormals(outVertices, normalsField->second);
	}

	const auto tCoordsField = m_model.meshes[meshIdx]
		.primitives[primitiveIdx].attributes.find("TEXCOORD_0");

	if (tCoordsField != m_model.meshes[meshIdx]
		.primitives[primitiveIdx].attributes.end()) {
		copyTCoords(outVertices, tCoordsField->second);
	}

	return vertices;
}

std::size_t CGLTFMeshFileLoader::ModelParser::getMeshCount() const
{
	return m_model.meshes.size();
}

std::size_t CGLTFMeshFileLoader::ModelParser::getPrimitiveCount(
		const std::size_t meshIdx) const
{
	return m_model.meshes[meshIdx].primitives.size();
}


template <typename T>
T CGLTFMeshFileLoader::ModelParser::readPrimitive(
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

core::vector2df CGLTFMeshFileLoader::ModelParser::readVec2DF(
		const CGLTFMeshFileLoader::BufferOffset& readFrom)
{
	return core::vector2df(readPrimitive<float>(readFrom),
		readPrimitive<float>(BufferOffset(readFrom, sizeof(float))));

}

core::vector3df CGLTFMeshFileLoader::ModelParser::readVec3DF(
		const BufferOffset& readFrom,
		const float scale = 1.0f)
{
	return core::vector3df(
		scale * readPrimitive<float>(readFrom),
		scale * readPrimitive<float>(BufferOffset(readFrom, sizeof(float))),
		-scale * readPrimitive<float>(BufferOffset(readFrom, 2 *
		sizeof(float))));
}

void CGLTFMeshFileLoader::ModelParser::copyPositions(
		const Span<video::S3DVertex> vertices,
		const std::size_t accessorId) const
{

	const auto& view = m_model.bufferViews[
		m_model.accessors[accessorId].bufferView];
	const auto& buffer = m_model.buffers[view.buffer];
	const auto count = m_model.accessors[accessorId].count;

	for (std::size_t i = 0; i < count; i++) {
		const auto v = readVec3DF(BufferOffset(
			buffer.data,
			view.byteOffset + (3 * sizeof(float) * i)),
			getScale());
		vertices.buffer[i].Pos = v;
	}
}

void CGLTFMeshFileLoader::ModelParser::copyNormals(
		const Span<video::S3DVertex> vertices,
		const std::size_t accessorId) const
{
	const auto& view = m_model.bufferViews[
		m_model.accessors[accessorId].bufferView];
	const auto& buffer = m_model.buffers[view.buffer];
	const auto count = m_model.accessors[accessorId].count;
	
	for (std::size_t i = 0; i < count; i++) {
		const auto n = readVec3DF(BufferOffset(buffer.data,
			view.byteOffset + 3 * sizeof(float) * i ));
		vertices.buffer[i].Normal = n;
	}
}

void CGLTFMeshFileLoader::ModelParser::copyTCoords(
		const Span<video::S3DVertex> vertices,
		const std::size_t accessorId) const
{
	const auto& view = m_model.bufferViews[
		m_model.accessors[accessorId].bufferView];
	const auto& buffer = m_model.buffers[view.buffer];
	const auto count = m_model.accessors[accessorId].count;

	for (std::size_t i = 0; i < count; ++i) {
		const auto t = readVec2DF(BufferOffset(
			buffer.data, view.byteOffset + 2 * sizeof(float) * i));
		vertices.buffer[i].TCoords = t;
	}
}

float CGLTFMeshFileLoader::ModelParser::getScale() const
{
	if (m_model.nodes[0].scale.size() > 0) {
		return static_cast<float>(m_model.nodes[0].scale[0]);
	}

	return 1.0f;
}

std::size_t CGLTFMeshFileLoader::ModelParser::getElemCount(
		const std::size_t accessorIdx) const
{
	return m_model.accessors[accessorIdx].count;
}

CGLTFMeshFileLoader::BufferOffset CGLTFMeshFileLoader::ModelParser::getBuffer(
		const std::size_t meshIdx,
		const std::size_t primitiveIdx,
		const std::size_t accessorIdx) const
{
	const auto& accessor = m_model.accessors[accessorIdx];
	const auto& view = m_model.bufferViews[accessor.bufferView];
	const auto& buffer = m_model.buffers[view.buffer];

	return BufferOffset(buffer.data, view.byteOffset);
}

std::size_t CGLTFMeshFileLoader::ModelParser::getIndicesAccessorIdx(
		const std::size_t meshIdx,
		const std::size_t primitiveIdx) const
{
	return m_model.meshes[meshIdx].primitives[primitiveIdx].indices;
}

std::size_t CGLTFMeshFileLoader::ModelParser::getPositionAccessorIdx(
		const std::size_t meshIdx,
		const std::size_t primitiveIdx) const
{
	return m_model.meshes[meshIdx].primitives[primitiveIdx]
		.attributes.find("POSITION")->second;
}

bool CGLTFMeshFileLoader::tryParseGLTF(io::IReadFile* file,
		tinygltf::Model& model)
{
	tinygltf::TinyGLTF loader {};

	// Stop embedded textures from making model fail to load
	loader.SetImageLoader(dummyImageLoader, nullptr);

	std::string err {};
	std::string warn {};

	auto buf = std::make_unique<char[]>(file->getSize());
	file->read(buf.get(), file->getSize());

	if (err != "") {
		return false;
	}

	return loader.LoadASCIIFromString(&model, &err, &warn, buf.get(),
		file->getSize(), "", 1);
}

} // namespace scene

} // namespace irr

