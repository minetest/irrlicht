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
	tinygltf::Model model{};

	if (file->getSize() == 0 || !tryParseGLTF(file, model)) {
		return nullptr;
	}

	// Create the base mesh
	SMesh* mesh { new SMesh {} };

	// Iterate models
	for (std::size_t meshIndex = 0;
		meshIndex < model.meshes.size(); meshIndex++) {
	// Iterate primitives
	for (std::size_t primitiveIndex = 0; primitiveIndex < model
		.meshes[meshIndex].primitives.size(); primitiveIndex++) {

		const auto positionAccessorId = model.meshes[meshIndex]
			.primitives[primitiveIndex].attributes["POSITION"];
		const auto indicesAccessorId = model.meshes[meshIndex]
			.primitives[primitiveIndex].indices;

		// Creates counts for preallocation
		std::size_t vertexCount = model.accessors[positionAccessorId].count;

		// We must count to create containers for the data
		// Create new buffer for vertices, positions, and normals
		auto* vertexBuffer = new video::S3DVertex[vertexCount]();
		// This is used to copy data into the vertexBuffer
		Span<video::S3DVertex> verticesBuffer{vertexBuffer,vertexCount};
		// Create dynamic indices buffer so it's easier to work with.
		// Preallocate needed resources to boost game startup speed
		std::vector<u16> indicesBuffer(model.accessors[
			indicesAccessorId].count);

		ModelParser parser(std::move(model));

		parser.getIndices(indicesAccessorId, indicesBuffer);
		parser.getVertices(positionAccessorId,
			verticesBuffer,
			meshIndex,
			primitiveIndex);

		// Inverse the order of indices due to the axis of the model being
		// inverted when going from left handed to right handed coordinates
		std::reverse(indicesBuffer.begin(),indicesBuffer.end());

		// Create the mesh buffer
		SMeshBuffer* meshbuf { new SMeshBuffer {} };
		meshbuf->append(vertexBuffer, vertexCount, indicesBuffer.data(),
			indicesBuffer.size());

		mesh->addMeshBuffer(meshbuf);

	}
	}

	// Create the mesh animations
	SAnimatedMesh* animatedMesh { new SAnimatedMesh {} };
	animatedMesh->addMesh(mesh);

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

void CGLTFMeshFileLoader::ModelParser::getIndices(
		const std::size_t accessorId,
		std::vector<u16>& outIndices) const
{
	const auto& view = m_model.bufferViews[
		m_model.accessors[accessorId].bufferView];
	const auto& modelIndices = m_model.buffers[view.buffer];

	auto buffOffset = BufferOffset(modelIndices.data, view.byteOffset);
	auto count = m_model.accessors[accessorId].count;

	for (std::size_t i = 0; i < count; i++) {
		outIndices[i] = readPrimitive<u16>(BufferOffset(
			buffOffset, i * sizeof(u16)));
	}
}

//Returns a tuple of the current counts (current_vertex_index,
//	current_normals_index, current_tcoords_index)
void CGLTFMeshFileLoader::ModelParser::getVertices(
		const std::size_t accessorId,
		Span<video::S3DVertex>& outVertices,
		std::size_t meshIndex,
		std::size_t primitiveIndex) const
{
	copyPositions(outVertices, accessorId);

	const auto normalsField = m_model.meshes[meshIndex]
		.primitives[primitiveIndex].attributes.find("NORMAL");

	if (normalsField != m_model.meshes[meshIndex]
		.primitives[primitiveIndex].attributes.end()) {
		copyNormals(outVertices, normalsField->second);
	}

	const auto tCoordsField = m_model.meshes[meshIndex]
		.primitives[primitiveIndex].attributes.find("TEXCOORD_0");

	if (tCoordsField != m_model.meshes[meshIndex]
		.primitives[primitiveIndex].attributes.end()) {
		copyTCoords(outVertices, tCoordsField->second);
	}
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
	// glTF's coordinate system is right-handed, Irrlicht's is left-handed
	// glTF's +Z axis corresponds to Irrlicht's -Z axis
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
		const auto n = readVec3DF(BufferOffset( buffer.data,
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

