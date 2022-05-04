// Copyright (C) 2007-2012 Christian Stehno
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_STL_LOADER_

#include "CSTLMeshFileLoader.h"
#include "SMesh.h"
#include "CDynamicMeshBuffer.h"
#include "CMemoryFile.h"
#include "SAnimatedMesh.h"
#include "IReadFile.h"
#include "fast_atof.h"
#include "coreutil.h"
#include "os.h"

namespace irr
{
namespace scene
{


//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".bsp")
bool CSTLMeshFileLoader::isALoadableFileExtension(const io::path& filename) const
{
	return core::hasFileExtension ( filename, "stl" );
}



//! creates/loads an animated mesh from the file.
//! \return Pointer to the created mesh. Returns 0 if loading failed.
//! If you no longer need the mesh, you should call IAnimatedMesh::drop().
//! See IReferenceCounted::drop() for more information.
IAnimatedMesh* CSTLMeshFileLoader::createMesh(io::IReadFile* fileIn)
{
	const long filesize = fileIn->getSize();
	if (filesize < 6) // we need a header
		return 0;

	// We copy the whole file into a memory-read file if it isn't already one.
	io::CMemoryReadFile * memoryFile = 0;
	if ( fileIn->getType() != io::ERFT_MEMORY_READ_FILE )
	{
		u8* fileBuffer = new u8[filesize];
		if ( fileIn->read(fileBuffer, filesize) != (size_t)filesize )
		{
			delete[] fileBuffer;
			return 0;
		}
		memoryFile = new io::CMemoryReadFile(fileBuffer, filesize, io::path(""), true);	// takes over fileBuffer
	}
	io::IReadFile* file = memoryFile ? memoryFile : fileIn;

	SMesh* mesh = new SMesh();
	CDynamicMeshBuffer* meshBuffer = new CDynamicMeshBuffer(video::EVT_STANDARD, video::EIT_16BIT);
	IVertexBuffer& vertBuffer = meshBuffer->getVertexBuffer();
	mesh->addMeshBuffer(meshBuffer);
	meshBuffer->drop();

	core::vector3df vertex[3];
	core::vector3df normal;

	bool binary = false;
	if (getNextToken(file) != "solid")
		binary = true;
	// read/skip header
	u32 binFaceCount = 0;
	if (binary)
	{
		file->seek(80);
		file->read(&binFaceCount, 4);
#ifdef __BIG_ENDIAN__
		binFaceCount = os::Byteswap::byteswap(binFaceCount);
#endif
	}
	else
		goNextLine(file);

	u16 attrib=0;
	Token.reserve(32);
	bool failure = false;

	while (file->getPos() < filesize)
	{
		if (!binary)
		{
			if (getNextToken(file) != "facet")
			{
				if (Token!="endsolid")
					failure = true;
				break;
			}
			if (getNextToken(file) != "normal")
			{
				failure = true;
				break;
			}
		}
		getNextVector(file, normal, binary);
		if (!binary)
		{
			if (getNextToken(file) != "outer")
			{
				failure = true;
				break;
			}
			if (getNextToken(file) != "loop")
			{
				failure = true;
				break;
			}
		}
		for (u32 i=0; i<3; ++i)
		{
			if (!binary)
			{
				if (getNextToken(file) != "vertex")
				{
					failure = true;
					break;
				}
			}
			getNextVector(file, vertex[i], binary);
		}
		if ( failure )
			break;
		if (!binary)
		{
			if (getNextToken(file) != "endloop")
			{
				failure = true;
				break;
			}
			if (getNextToken(file) != "endfacet")
			{
				failure = true;
				break;
			}
		}
		else
		{
			file->read(&attrib, 2);
#ifdef __BIG_ENDIAN__
			attrib = os::Byteswap::byteswap(attrib);
#endif
		}

		video::SColor color(0xffffffff);
		if (attrib & 0x8000)
			color = video::A1R5G5B5toA8R8G8B8(attrib);
		if (normal==core::vector3df())
			normal=core::plane3df(vertex[2],vertex[1],vertex[0]).Normal;
		vertBuffer.push_back(video::S3DVertex(vertex[2],normal,color, core::vector2df()));
		vertBuffer.push_back(video::S3DVertex(vertex[1],normal,color, core::vector2df()));
		vertBuffer.push_back(video::S3DVertex(vertex[0],normal,color, core::vector2df()));
	}	// end while (file->getPos() < filesize)

	// Create the Animated mesh if there's anything in the mesh
	SAnimatedMesh* pAM = 0;
	if ( !failure && mesh->getMeshBufferCount() > 0 )
	{
		IIndexBuffer& indexBuffer = meshBuffer->getIndexBuffer();
		u32 vertCount = vertBuffer.size();
		if (vertCount > 65535 )	// Note 65535 instead of 65536 as it divides by 3
		{
			if ( getIndexTypeHint() != EITH_16BIT )
				indexBuffer.setType(video::EIT_32BIT);
			else
			{
				 // Could split buffer, but probably no one really needs this anymore now with 32-bit support and necessary buffer manipulation functions are not there yet
				vertCount = 65535;
			}
		}

		indexBuffer.reallocate(vertCount);
		for (u32 i=0; i<vertCount; ++i)	//every vertex is unique, so we can just generate the indices
			indexBuffer.push_back(i);

		meshBuffer->recalculateBoundingBox();
		mesh->recalculateBoundingBox();
		pAM = new SAnimatedMesh();
		pAM->Type = EAMT_STATIC;
		pAM->addMesh(mesh);
		pAM->recalculateBoundingBox();
	}

	mesh->drop();
	Token.clear();
	if ( memoryFile )
		memoryFile->drop();

	return pAM;
}


//! Read 3d vector of floats
void CSTLMeshFileLoader::getNextVector(io::IReadFile* file, core::vector3df& vec, bool binary)
{
	if (binary)
	{
		file->read(&vec.X, 4);
		file->read(&vec.Y, 4);
		file->read(&vec.Z, 4);
#ifdef __BIG_ENDIAN__
		vec.X = os::Byteswap::byteswap(vec.X);
		vec.Y = os::Byteswap::byteswap(vec.Y);
		vec.Z = os::Byteswap::byteswap(vec.Z);
#endif
	}
	else
	{
		goNextWord(file);

		getNextToken(file);
		core::fast_atof_move(Token.c_str(), vec.X);
		getNextToken(file);
		core::fast_atof_move(Token.c_str(), vec.Y);
		getNextToken(file);
		core::fast_atof_move(Token.c_str(), vec.Z);
	}
	vec.X=-vec.X;
}


//! Read next word
const core::stringc& CSTLMeshFileLoader::getNextToken(io::IReadFile* file)
{
	goNextWord(file);
	u8 c;
	Token = "";
	while(file->getPos() != file->getSize())
	{
		file->read(&c, 1);
		// found it, so leave
		if (core::isspace(c))
			break;
		Token.append(c);
	}
	return Token;
}


//! skip to next word
void CSTLMeshFileLoader::goNextWord(io::IReadFile* file) const
{
	u8 c;
	while(file->getPos() != file->getSize())
	{
		file->read(&c, 1);
		// found it, so leave
		if (!core::isspace(c))
		{
			file->seek(-1, true);
			break;
		}
	}
}


//! Read until line break is reached and stop at the next non-space character
void CSTLMeshFileLoader::goNextLine(io::IReadFile* file) const
{
	u8 c;
	// look for newline characters
	while(file->getPos() != file->getSize())
	{
		file->read(&c, 1);
		// found it, so leave
		if (c=='\n' || c=='\r')
			break;
	}
}


} // end namespace scene
} // end namespace irr


#endif // _IRR_COMPILE_WITH_STL_LOADER_

