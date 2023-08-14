// Copyright (C) 2008-2012 Christian Stehno
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_OBJ_WRITER_

#include "COBJMeshWriter.h"
#include "os.h"
#include "IMesh.h"
#include "IMeshBuffer.h"
#include "ISceneManager.h"
#include "IWriteFile.h"
#include "IFileSystem.h"
#include "ITexture.h"

namespace irr
{
namespace scene
{

COBJMeshWriter::COBJMeshWriter(scene::ISceneManager* smgr, io::IFileSystem* fs)
	: SceneManager(smgr), FileSystem(fs)
{
	#ifdef _DEBUG
	setDebugName("COBJMeshWriter");
	#endif

	if (SceneManager)
		SceneManager->grab();

	if (FileSystem)
		FileSystem->grab();
}


COBJMeshWriter::~COBJMeshWriter()
{
	if (SceneManager)
		SceneManager->drop();

	if (FileSystem)
		FileSystem->drop();
}


//! Returns the type of the mesh writer
EMESH_WRITER_TYPE COBJMeshWriter::getType() const
{
	return EMWT_OBJ;
}


//! writes a mesh
bool COBJMeshWriter::writeMesh(io::IWriteFile* file, scene::IMesh* mesh, s32 flags)
{
	if (!file)
		return false;

	os::Printer::log("Writing mesh", file->getFileName());

	// write OBJ MESH header

	io::path name;
	core::cutFilenameExtension(name,file->getFileName()) += ".mtl";
	file->write("# exported by Irrlicht\n",23);
	file->write("mtllib ",7);
	file->write(name.c_str(),name.size());
	file->write("\n\n",2);

	// write mesh buffers

	core::array<video::SMaterial*> mat;

	u32 allVertexCount=1; // count vertices over the whole file
	for (u32 i=0; i<mesh->getMeshBufferCount(); ++i)
	{
		core::stringc num(i+1);
		IMeshBuffer* buffer = mesh->getMeshBuffer(i);
		if (buffer && buffer->getVertexCount())
		{
			file->write("g grp", 5);
			file->write(num.c_str(), num.size());
			file->write("\n",1);

			u32 j;
			const u32 vertexCount = buffer->getVertexCount();
			for (j=0; j<vertexCount; ++j)
			{
				file->write("v ",2);
				getVectorAsStringLine(buffer->getPosition(j), num);
				file->write(num.c_str(), num.size());
			}

			for (j=0; j<vertexCount; ++j)
			{
				file->write("vt ",3);
				getVectorAsStringLine(buffer->getTCoords(j), num);
				file->write(num.c_str(), num.size());
			}

			for (j=0; j<vertexCount; ++j)
			{
				file->write("vn ",3);
				getVectorAsStringLine(buffer->getNormal(j), num);
				file->write(num.c_str(), num.size());
			}

			file->write("usemtl mat",10);
			num = "";
			for (j=0; j<mat.size(); ++j)
			{
				if (*mat[j]==buffer->getMaterial())
				{
					num = core::stringc(j);
					break;
				}
			}
			if (num == "")
			{
				num = core::stringc(mat.size());
				mat.push_back(&buffer->getMaterial());
			}
			file->write(num.c_str(), num.size());
			file->write("\n",1);

			unsigned int idx2=0, idx1=0, idx0 = 0;
			const u32 indexCount = buffer->getIndexCount();
			for (j=0; j<indexCount; j+=3)
			{
				switch(buffer->getIndexType())
				{
					case video::EIT_16BIT:
						idx2 = buffer->getIndices()[j+2]+allVertexCount;
						idx1 = buffer->getIndices()[j+1]+allVertexCount;
						idx0 = buffer->getIndices()[j+0]+allVertexCount;
						break;
					case video::EIT_32BIT:
						idx2 = ((u32*)buffer->getIndices())[j+2]+allVertexCount;
						idx1 = ((u32*)buffer->getIndices())[j+1]+allVertexCount;
						idx0 = ((u32*)buffer->getIndices())[j+0]+allVertexCount;
						break;
				}

				file->write("f ",2);
				num = core::stringc(idx2);
				file->write(num.c_str(), num.size());
				file->write("/",1);
				file->write(num.c_str(), num.size());
				file->write("/",1);
				file->write(num.c_str(), num.size());
				file->write(" ",1);

				num = core::stringc(idx1);
				file->write(num.c_str(), num.size());
				file->write("/",1);
				file->write(num.c_str(), num.size());
				file->write("/",1);
				file->write(num.c_str(), num.size());
				file->write(" ",1);

				num = core::stringc(idx0);
				file->write(num.c_str(), num.size());
				file->write("/",1);
				file->write(num.c_str(), num.size());
				file->write("/",1);
				file->write(num.c_str(), num.size());
				file->write(" ",1);

				file->write("\n",1);
			}
			file->write("\n",1);
			allVertexCount += vertexCount;
		}
	}

	if (mat.size() == 0)
		return true;

	file = FileSystem->createAndWriteFile( name );
	if (file)
	{
		os::Printer::log("Writing material", file->getFileName());

		file->write("# exported by Irrlicht\n\n",24);
		for (u32 i=0; i<mat.size(); ++i)
		{
			core::stringc num(i);
			file->write("newmtl mat",10);
			file->write(num.c_str(),num.size());
			file->write("\n",1);

			getColorAsStringLine(mat[i]->AmbientColor, "Ka", num);
			file->write(num.c_str(),num.size());
			getColorAsStringLine(mat[i]->DiffuseColor, "Kd", num);
			file->write(num.c_str(),num.size());
			getColorAsStringLine(mat[i]->SpecularColor, "Ks", num);
			file->write(num.c_str(),num.size());
			getColorAsStringLine(mat[i]->EmissiveColor, "Ke", num);
			file->write(num.c_str(),num.size());
			num = core::stringc((double)(mat[i]->Shininess/0.128f));
			file->write("Ns ", 3);
			file->write(num.c_str(),num.size());
			file->write("\n", 1);
			if (mat[i]->getTexture(0))
			{
				file->write("map_Kd ", 7);

				f32 tposX, tposY, tscaleX, tscaleY;
				const core::matrix4& textureMatrix =  mat[i]->getTextureMatrix(0);
				textureMatrix.getTextureTranslate(tposX, tposY);
				textureMatrix.getTextureScale(tscaleX, tscaleY);

			   //Write texture translation values
				if ( !core::equals(tposX, 0.f) || !core::equals(tposY, 0.f) )
				{
					file->write("-o ", 3);
			   		core::stringc tx(tposX);
			   		core::stringc ty(tposY);

					file->write(tx.c_str(), tx.size());
					file->write(" ", 1);
					file->write(ty.c_str(), ty.size());
					file->write(" ", 1);
				}

				//Write texture scaling values
				if ( !core::equals(tscaleX, 1.f) || !core::equals(tscaleY, 1.f) )
				{
					file->write("-s ", 3);

					core::stringc sx(tscaleX);
					core::stringc sy(tscaleY);

					file->write(sx.c_str(), sx.size());
					file->write(" ", 1);
					file->write(sy.c_str(), sy.size());
					file->write(" ", 1);
				}

				io::path tname = FileSystem->getRelativeFilename(mat[i]->getTexture(0)->getName(),
						FileSystem->getFileDir(file->getFileName()));
				// avoid blanks as .obj cannot handle strings with spaces
				if (tname.findFirst(' ') != -1)
					tname = FileSystem->getFileBasename(tname);
				file->write(tname.c_str(), tname.size());
				file->write("\n",1);
			}
			file->write("\n",1);
		}
		file->drop();
	}
	return true;
}


void COBJMeshWriter::getVectorAsStringLine(const core::vector3df& v, core::stringc& s) const
{
	c8 tmpbuf[255];
	snprintf_irr(tmpbuf, 255, "%f %f %f\n", -v.X, v.Y, v.Z);
	s = tmpbuf;
}


void COBJMeshWriter::getVectorAsStringLine(const core::vector2df& v, core::stringc& s) const
{
	c8 tmpbuf[255];
	snprintf_irr(tmpbuf, 255, "%f %f\n", v.X, 1.f-v.Y);
	s = tmpbuf;
}


void COBJMeshWriter::getColorAsStringLine(const video::SColor& color, const c8* const prefix, core::stringc& s) const
{
	c8 tmpbuf[255];
	snprintf_irr(tmpbuf, 255, "%s %f %f %f\n", prefix, (float)(color.getRed()/255.f), (float)(color.getGreen()/255.f), (float)(color.getBlue()/255.f));
	s = tmpbuf;
}


} // end namespace
} // end namespace

#endif

