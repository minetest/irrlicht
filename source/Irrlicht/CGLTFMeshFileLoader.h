#ifndef __C_GLTF_MESH_FILE_LOADER_INCLUDED__
#define __C_GLTF_MESH_FILE_LOADER_INCLUDED__

#include "IAnimatedMesh.h"
#include "IMeshLoader.h"
#include "IReadFile.h"
#include "irrTypes.h"
#include "path.h"
#include "S3DVertex.h"
#include "SMesh.h"
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

	class MeshExtractor {
	public:
		MeshExtractor(const tinygltf::Model& model) noexcept;

		MeshExtractor(const tinygltf::Model&& model) noexcept;

		/* Gets indices for the given mesh/primitive.
		 *
		 * Values are return in Irrlicht winding order.
		 */
		std::vector<u16> getIndices(const std::size_t meshIdx,
				const std::size_t primitiveIdx) const;

		std::vector<video::S3DVertex> getVertices(std::size_t meshIdx,
				const std::size_t primitiveIdx) const;

		std::size_t getMeshCount() const;

		std::size_t getPrimitiveCount(const std::size_t meshIdx) const;

	private:
		tinygltf::Model m_model;

		template <typename T>
		static T readPrimitive(const BufferOffset& readFrom);

		static core::vector2df readVec2DF(
				const BufferOffset& readFrom);

		/* Read a vec3df from a buffer with transformations applied.
		 *
		 * Values are returned in Irrlicht coordinates.
		 */
		static core::vector3df readVec3DF(
				const BufferOffset& readFrom,
				const float scale = 1.0f);

		void copyPositions(const Span<video::S3DVertex> vertices,
				const std::size_t accessorId) const;

		void copyNormals(const Span<video::S3DVertex> vertices,
				const std::size_t accessorId) const;

		void copyTCoords(const Span<video::S3DVertex> vertices,
				const std::size_t accessorId) const;

		/* Get the scale factor from the glTF mesh information.
		 *
		 * Returns 1.0f if no scale factor is present.
		 */
		float getScale() const;

		std::size_t getElemCount(const std::size_t accessorIdx) const;

		BufferOffset getBuffer(const std::size_t meshIdx,
				const std::size_t primitiveIdx,
				const std::size_t accessorIdx) const;

		std::size_t getIndicesAccessorIdx(const std::size_t meshIdx,
				const std::size_t primitiveIdx) const;

		std::size_t getPositionAccessorIdx(const std::size_t meshIdx,
				const std::size_t primitiveIdx) const;
	};

	void loadPrimitives(const MeshExtractor& parser, SMesh* mesh);

	static bool tryParseGLTF(io::IReadFile* file,
			tinygltf::Model& model);
};

} // namespace scene

} // namespace irr

#endif // __C_GLTF_MESH_FILE_LOADER_INCLUDED__

