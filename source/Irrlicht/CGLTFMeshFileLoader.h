#ifndef __C_GLTF_MESH_FILE_LOADER_INCLUDED__
#define __C_GLTF_MESH_FILE_LOADER_INCLUDED__

#include "IAnimatedMesh.h"
#include "IMeshLoader.h"
#include "IReadFile.h"
#include "path.h"
#include "S3DVertex.h"
#include "vector2d.h"
#include "vector3d.h"

#include <tiny_gltf.h>

#include <cstddef>
#include <vector>

namespace irr
{

namespace scene
{

class CGLTFMeshFileLoader : public IMeshLoader
{
public:
	CGLTFMeshFileLoader() noexcept;

	bool isALoadableFileExtension(const io::path& filename) const override;

	IAnimatedMesh* createMesh(io::IReadFile* file) override;

private:
	template <typename T>
	struct Span
	{
		T* buffer = nullptr;
		std::size_t size = 0;
	};

	class BufferOffset
	{
	public:
		BufferOffset(const std::vector<unsigned char>& buf,
				const std::size_t offset);

		BufferOffset(const BufferOffset& other,
				const std::size_t fromOffset);

		unsigned char at(const std::size_t fromOffset) const;
	private:
		const std::vector<unsigned char>& m_buf;
		std::size_t m_offset;
		int m_filesize;
	};

	template <typename T>
	static T readPrimitive(const BufferOffset& readFrom);

	static core::vector2df readVec2DF(
			const BufferOffset& readFrom);

	static core::vector3df readVec3DF(
			const BufferOffset& readFrom,
			const float scale);

	static void copyPositions(const tinygltf::Model& model,
			const Span<video::S3DVertex> vertices,
			const std::size_t accessorId);

	static void copyNormals(const tinygltf::Model& model,
			const Span<video::S3DVertex> vertices,
			const std::size_t accessorId);

	static void copyTCoords(const tinygltf::Model& model,
			const Span<video::S3DVertex> vertices,
			const std::size_t accessorId);

	static void getIndices(const tinygltf::Model& model,
			const std::size_t accessorId,
			std::vector<u16>& indicesBuffer);

	static void getVertices(const tinygltf::Model& model,
			const std::size_t accessorId,
			Span<video::S3DVertex>& outVertices,
			std::size_t meshIndex,
			std::size_t primitiveIndex);

	static bool tryParseGLTF(io::IReadFile* file,
			tinygltf::Model& model);

	static float getScale(const tinygltf::Model& model);
};

} // namespace scene

} // namespace irr

#endif // __C_GLTF_MESH_FILE_LOADER_INCLUDED__
