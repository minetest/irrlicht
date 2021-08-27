// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_C_FILE_SYSTEM_H_INCLUDED
#define IRR_C_FILE_SYSTEM_H_INCLUDED

#include "IFileSystem.h"
#include "irrArray.h"

namespace irr
{
namespace io
{

	class CZipReader;
	class CPakReader;
	class CMountPointReader;

/*!
	FileSystem which uses normal files and one zipfile
*/
class CFileSystem : public IFileSystem
{
public:

	//! constructor
	CFileSystem();

	//! destructor
	virtual ~CFileSystem();

	//! opens a file for read access
	virtual IReadFile* createAndOpenFile(const io::path& filename) IRR_OVERRIDE;

	//! Creates an IReadFile interface for accessing memory like a file.
	virtual IReadFile* createMemoryReadFile(const void* memory, s32 len, const io::path& fileName, bool deleteMemoryWhenDropped = false) IRR_OVERRIDE;

	//! Creates an IReadFile interface for accessing files inside files
	virtual IReadFile* createLimitReadFile(const io::path& fileName, IReadFile* alreadyOpenedFile, long pos, long areaSize) IRR_OVERRIDE;

	//! Creates an IWriteFile interface for accessing memory like a file.
	virtual IWriteFile* createMemoryWriteFile(void* memory, s32 len, const io::path& fileName, bool deleteMemoryWhenDropped=false) IRR_OVERRIDE;

	//! Opens a file for write access.
	virtual IWriteFile* createAndWriteFile(const io::path& filename, bool append=false) IRR_OVERRIDE;

	//! Adds an archive to the file system.
	virtual bool addFileArchive(const io::path& filename,
			bool ignoreCase = true, bool ignorePaths = true,
			E_FILE_ARCHIVE_TYPE archiveType = EFAT_UNKNOWN,
			const core::stringc& password="",
			IFileArchive** retArchive = 0) IRR_OVERRIDE;

	//! Adds an archive to the file system.
	virtual bool addFileArchive(IReadFile* file, bool ignoreCase=true,
			bool ignorePaths=true,
			E_FILE_ARCHIVE_TYPE archiveType=EFAT_UNKNOWN,
			const core::stringc& password="",
			IFileArchive** retArchive = 0) IRR_OVERRIDE;

	//! Adds an archive to the file system.
	virtual bool addFileArchive(IFileArchive* archive) IRR_OVERRIDE;

	//! move the hirarchy of the filesystem. moves sourceIndex relative up or down
	virtual bool moveFileArchive(u32 sourceIndex, s32 relative) IRR_OVERRIDE;

	//! Adds an external archive loader to the engine.
	virtual void addArchiveLoader(IArchiveLoader* loader) IRR_OVERRIDE;

	//! Returns the total number of archive loaders added.
	virtual u32 getArchiveLoaderCount() const IRR_OVERRIDE;

	//! Gets the archive loader by index.
	virtual IArchiveLoader* getArchiveLoader(u32 index) const IRR_OVERRIDE;

	//! gets the file archive count
	virtual u32 getFileArchiveCount() const IRR_OVERRIDE;

	//! gets an archive
	virtual IFileArchive* getFileArchive(u32 index) IRR_OVERRIDE;

	//! removes an archive from the file system.
	virtual bool removeFileArchive(u32 index) IRR_OVERRIDE;

	//! removes an archive from the file system.
	virtual bool removeFileArchive(const io::path& filename) IRR_OVERRIDE;

	//! Removes an archive from the file system.
	virtual bool removeFileArchive(const IFileArchive* archive) IRR_OVERRIDE;

	//! Returns the string of the current working directory
	virtual const io::path& getWorkingDirectory() IRR_OVERRIDE;

	//! Changes the current Working Directory to the string given.
	//! The string is operating system dependent. Under Windows it will look
	//! like this: "drive:\directory\sudirectory\"
	virtual bool changeWorkingDirectoryTo(const io::path& newDirectory) IRR_OVERRIDE;

	//! Converts a relative path to an absolute (unique) path, resolving symbolic links
	virtual io::path getAbsolutePath(const io::path& filename) const IRR_OVERRIDE;

	//! Returns the directory a file is located in.
	/** \param filename: The file to get the directory from */
	virtual io::path getFileDir(const io::path& filename) const IRR_OVERRIDE;

	//! Returns the base part of a filename, i.e. the name without the directory
	//! part. If no directory is prefixed, the full name is returned.
	/** \param filename: The file to get the basename from */
	virtual io::path getFileBasename(const io::path& filename, bool keepExtension=true) const IRR_OVERRIDE;

	//! flatten a path and file name for example: "/you/me/../." becomes "/you"
	virtual io::path& flattenFilename( io::path& directory, const io::path& root = "/" ) const IRR_OVERRIDE;

	//! Get the relative filename, relative to the given directory
	virtual path getRelativeFilename(const path& filename, const path& directory) const IRR_OVERRIDE;

	virtual EFileSystemType setFileListSystem(EFileSystemType listType) IRR_OVERRIDE;

	//! Creates a list of files and directories in the current working directory
	//! and returns it.
	virtual IFileList* createFileList() IRR_OVERRIDE;

	//! Creates an empty filelist
	virtual IFileList* createEmptyFileList(const io::path& path, bool ignoreCase, bool ignorePaths) IRR_OVERRIDE;

	//! determines if a file exists and would be able to be opened.
	virtual bool existFile(const io::path& filename) const IRR_OVERRIDE;

	//! Creates a XML Reader from a file.
	virtual IXMLReader* createXMLReader(const io::path& filename) IRR_OVERRIDE;

	//! Creates a XML Reader from a file.
	virtual IXMLReader* createXMLReader(IReadFile* file) IRR_OVERRIDE;

	//! Creates a XML Reader from a file.
	virtual IXMLReaderUTF8* createXMLReaderUTF8(const io::path& filename) IRR_OVERRIDE;

	//! Creates a XML Reader from a file.
	virtual IXMLReaderUTF8* createXMLReaderUTF8(IReadFile* file) IRR_OVERRIDE;

	//! Creates a XML Writer from a file.
	virtual IXMLWriter* createXMLWriter(const io::path& filename) IRR_OVERRIDE;

	//! Creates a XML Writer from a file.
	virtual IXMLWriter* createXMLWriter(IWriteFile* file) IRR_OVERRIDE;

	//! Creates a XML Writer from a file which will write ASCII/UTF-8 characters (char*).
	virtual IXMLWriterUTF8* createXMLWriterUTF8(const path& filename) IRR_OVERRIDE;

	//! Creates a XML Writer from a file which will write ASCII/UTF-8 characters (char*).
	virtual IXMLWriterUTF8* createXMLWriterUTF8(IWriteFile* file) IRR_OVERRIDE;

	//! Creates a new empty collection of attributes, usable for serialization and more.
	virtual IAttributes* createEmptyAttributes(video::IVideoDriver* driver) IRR_OVERRIDE;

private:

	// don't expose, needs refactoring
	bool changeArchivePassword(const path& filename,
			const core::stringc& password,
			IFileArchive** archive = 0);

	//! Currently used FileSystemType
	EFileSystemType FileSystemType;
	//! WorkingDirectory for Native and Virtual filesystems
	io::path WorkingDirectory [2];
	//! currently attached ArchiveLoaders
	core::array<IArchiveLoader*> ArchiveLoader;
	//! currently attached Archives
	core::array<IFileArchive*> FileArchives;
};


} // end namespace irr
} // end namespace io

#endif
