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
#include <vector>

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

// A helper function to disable tinygltf embedded image loading
bool dummyImageLoader(tinygltf::Image *a, const int b, std::string *c,
	std::string *d, int e, int f, const unsigned char * g,
	int h, void *user_pointer)
{
	return false;
};

namespace irr
{

namespace scene
{

static bool tryParseGLTF(io::IReadFile* file, tinygltf::Model& model)
{
	tinygltf::TinyGLTF loader {};

	// Stop embedded textures from making model fail to load
	loader.SetImageLoader(dummyImageLoader, nullptr);

	std::string err {};
	std::string warn {};

	auto buf = std::make_unique<char[]>(file->getSize());
	file->read(buf.get(), file->getSize());

	if (err != "" || warn != "") {
		return false;
	}

	return loader.LoadASCIIFromString(&model, &err, &warn, buf.get(),
		file->getSize(), "", 1);
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
	return core::vector2df(readPrimitive<float>(readFrom),
		readPrimitive<float>(BufferOffset( readFrom, sizeof(float))));

}

static core::vector3df readVec3DF(const BufferOffset& readFrom,
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

float getScale(const tinygltf::Model& model)
{
	if (model.nodes[0].scale.size() > 0) {
		return static_cast<float>(model.nodes[0].scale[0]);
	}
	return 1.0f;
}

static void copyPositions(const tinygltf::Model& model,
	const Span<video::S3DVertex> vertices, const std::size_t accessorId)
{

	const auto& view = model.bufferViews[
		model.accessors[accessorId].bufferView];
	const auto& buffer = model.buffers[view.buffer];
	const auto count = model.accessors[accessorId].count;

	float scale = getScale(model);

	for (std::size_t i = 0; i < count; i++) {
		const auto v = readVec3DF(BufferOffset(
			buffer.data, view.byteOffset + (3 * sizeof(float) * i)),scale);
		vertices.buffer[i].Pos = v;
	}
}

static void copyNormals(const tinygltf::Model& model,
const Span<video::S3DVertex> vertices,const std::size_t accessorId)
{
	const auto& view = model.bufferViews[
		model.accessors[accessorId].bufferView];
	const auto& buffer = model.buffers[view.buffer];
	const auto count = model.accessors[accessorId].count;
	
	for (std::size_t i = 0; i < count; i++) {
		const auto n = readVec3DF(BufferOffset( buffer.data,
			view.byteOffset + 3 * sizeof(float) * i ));
		vertices.buffer[i].Normal = n;
	}
}

static void copyTCoords(const tinygltf::Model& model,
	const Span<video::S3DVertex> vertices, const std::size_t accessorId)
{
	const auto& view = model.bufferViews[
		model.accessors[accessorId].bufferView];
	const auto& buffer = model.buffers[view.buffer];
	const auto count = model.accessors[accessorId].count;

	for (std::size_t i = 0; i < count; ++i) {
		const auto t = readVec2DF(BufferOffset(
			buffer.data, view.byteOffset + 2 * sizeof(float) * i));
		vertices.buffer[i].TCoords = t;
	}
}

static void getIndices(const tinygltf::Model& model,
	const std::size_t accessorId, std::vector<u16>& indicesBuffer)
{
	const auto& view = model.bufferViews[
		model.accessors[accessorId].bufferView];
	const auto& modelIndices = model.buffers[view.buffer];

	auto buffOffset = BufferOffset(modelIndices.data, view.byteOffset);
	auto count = model.accessors[accessorId].count;

	for (std::size_t i = 0; i < count; i++) {
		indicesBuffer[i] = readPrimitive<u16>(BufferOffset(
			buffOffset, i * sizeof(u16)));
	}
}

//Returns a tuple of the current counts (current_vertex_index,
//	current_normals_index, current_tcoords_index)
static void getVertices
( const tinygltf::Model& model, const std::size_t accessorId,
	Span<video::S3DVertex>& verticesBuffer, 
	std::size_t mesh_index, std::size_t primitive_index )
{
	copyPositions(model, verticesBuffer, accessorId);

	const auto normalsField = model.meshes[mesh_index]
		.primitives[primitive_index].attributes.find("NORMAL");

	if (normalsField != model.meshes[mesh_index]
		.primitives[primitive_index].attributes.end()) {
		copyNormals(model, verticesBuffer, normalsField->second);
	}
	
	const auto tCoordsField = model.meshes[mesh_index]
		.primitives[primitive_index].attributes.find("TEXCOORD_0");

	if (tCoordsField != model.meshes[mesh_index]
		.primitives[primitive_index].attributes.end()) {
		copyTCoords(model, verticesBuffer, tCoordsField->second);
	}
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

	// Create the base mesh
	SMesh* mesh { new SMesh {} };

	// Iterate models
	for (std::size_t mesh_index = 0;
		mesh_index < model.meshes.size(); mesh_index++) {
	// Iterate primitives
	for (std::size_t primitive_index = 0; primitive_index < model
		.meshes[mesh_index].primitives.size(); primitive_index++) {

		const auto positionAccessorId = model.meshes[mesh_index]
			.primitives[primitive_index].attributes["POSITION"];
		const auto indicesAccessorId = model.meshes[mesh_index]
			.primitives[primitive_index].indices;

		// Creates counts for preallocation
		std::size_t vertices_count = model.accessors[positionAccessorId].count;

		// We must count to create containers for the data	
		// Create new buffer for vertices, positions, and normals
		auto* vertexBuffer = new video::S3DVertex[vertices_count]();
		// This is used to copy data into the vertexBuffer
		Span<video::S3DVertex> verticesBuffer{vertexBuffer,vertices_count};
		// Create dynamic indices buffer so it's easier to work with.
        // Preallocate needed resources to boost game startup speed
		std::vector<u16> indicesBuffer(model.accessors[
            indicesAccessorId].count);

		getIndices(model, indicesAccessorId, indicesBuffer);
		getVertices(model, positionAccessorId, verticesBuffer,
			mesh_index, primitive_index);

        // Inverse the order of indices due to the axis of the model being
		// inverted when going from left handed to right handed coordinates
        std::reverse(indicesBuffer.begin(),indicesBuffer.end()); 

		// Create the mesh buffer
		SMeshBuffer* meshbuf { new SMeshBuffer {} };
		meshbuf->append(vertexBuffer, vertices_count, indicesBuffer.data(),
			indicesBuffer.size());
		
		mesh->addMeshBuffer(meshbuf);

	}
	}

	// Create the mesh animations
	SAnimatedMesh* animatedMesh { new SAnimatedMesh {} };
	animatedMesh->addMesh(mesh);	

	return animatedMesh;
}

} // namespace scene

} // namespace irr

