// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef IRR_C_MEMORY_READ_FILE_H_INCLUDED
#define IRR_C_MEMORY_READ_FILE_H_INCLUDED

#include "IMemoryReadFile.h"
#include "IWriteFile.h"
#include "irrString.h"

namespace irr
{

namespace io
{

	/*!
		Class for reading from memory.
	*/
	class CMemoryReadFile : public IMemoryReadFile
	{
	public:

		//! Constructor
		CMemoryReadFile(const void* memory, long len, const io::path& fileName, bool deleteMemoryWhenDropped);

		//! Destructor
		virtual ~CMemoryReadFile();

		//! returns how much was read
		virtual size_t read(void* buffer, size_t sizeToRead) IRR_OVERRIDE;

		//! changes position in file, returns true if successful
		virtual bool seek(long finalPos, bool relativeMovement = false) IRR_OVERRIDE;

		//! returns size of file
		virtual long getSize() const IRR_OVERRIDE;

		//! returns where in the file we are.
		virtual long getPos() const IRR_OVERRIDE;

		//! returns name of file
		virtual const io::path& getFileName() const IRR_OVERRIDE;

		//! Get the type of the class implementing this interface
		virtual EREAD_FILE_TYPE getType() const IRR_OVERRIDE
		{
			return ERFT_MEMORY_READ_FILE;
		}

		//! Get direct access to internal buffer
		virtual const void *getBuffer() const IRR_OVERRIDE
		{
			return Buffer;
		}

	private:

		const void *Buffer;
		long Len;
		long Pos;
		io::path Filename;
		bool deleteMemoryWhenDropped;
	};

	/*!
		Class for writing to memory.
	*/
	class CMemoryWriteFile : public IWriteFile
	{
	public:

		//! Constructor
		CMemoryWriteFile(void* memory, long len, const io::path& fileName, bool deleteMemoryWhenDropped);

		//! Destructor
		virtual ~CMemoryWriteFile();

		//! returns how much was written
		virtual size_t write(const void* buffer, size_t sizeToWrite) IRR_OVERRIDE;

		//! changes position in file, returns true if successful
		virtual bool seek(long finalPos, bool relativeMovement = false) IRR_OVERRIDE;

		//! returns where in the file we are.
		virtual long getPos() const IRR_OVERRIDE;

		//! returns name of file
		virtual const io::path& getFileName() const IRR_OVERRIDE;

		virtual bool flush() IRR_OVERRIDE;

	private:

		void *Buffer;
		long Len;
		long Pos;
		io::path Filename;
		bool deleteMemoryWhenDropped;
	};

} // end namespace io
} // end namespace irr

#endif
