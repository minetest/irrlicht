// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CImageLoaderTGA.h"

#include <array>

#include "IReadFile.h"
#include "os.h"
#include "CColorConverter.h"
#include "CImage.h"
#include "irrString.h"


namespace irr
{
namespace video
{


//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".tga")
bool CImageLoaderTGA::isALoadableFileExtension(const io::path& filename) const
{
	return core::hasFileExtension ( filename, "tga" );
}


//! loads a compressed tga.
std::vector<u8> CImageLoaderTGA::loadCompressedImage(io::IReadFile *file, const STGAHeader& header) const
{
	// This was written and sent in by Jon Pry, thank you very much!
	// I only changed the formatting a little bit.

	s32 bytesPerPixel = header.PixelDepth/8;
	s32 imageSize =  header.ImageHeight * header.ImageWidth * bytesPerPixel;
	std::vector<u8> data(imageSize);
	s32 currentByte = 0;

	while(currentByte < imageSize)
	{
		u8 chunkheader = 0;
		file->read(&chunkheader, sizeof(u8)); // Read The Chunk's Header

		if (chunkheader < 128) { // If The Chunk Is A 'RAW' Chunk
			chunkheader++; // Add 1 To The Value To Get Total Number Of Raw Pixels
			s32 byteCount = bytesPerPixel * chunkheader;
			if (byteCount > imageSize - currentByte)
				return {};
			file->read(&data[currentByte], byteCount);
			currentByte += byteCount;
		} else {
			// thnx to neojzs for some fixes with this code

			// If It's An RLE Header
			chunkheader -= 127; // Subtract 127 To Get Rid Of The ID Bit
			// Read one pixel but write it `chunkheader` times

			s32 byteCount = bytesPerPixel * chunkheader;
			if (byteCount > imageSize - currentByte)
				return {};

			s32 dataOffset = currentByte;
			file->read(&data[dataOffset], bytesPerPixel);

			currentByte += bytesPerPixel;

			for(s32 counter = 1; counter < chunkheader; counter++)
			{
				for(s32 elementCounter=0; elementCounter < bytesPerPixel; elementCounter++)
					data[currentByte + elementCounter] = data[dataOffset + elementCounter];

				currentByte += bytesPerPixel;
			}
		}
	}

	return data;
}



//! returns true if the file maybe is able to be loaded by this class
bool CImageLoaderTGA::isALoadableFileFormat(io::IReadFile* file) const
{
	if (!file)
		return false;

	STGAFooter footer;
	memset(&footer, 0, sizeof(STGAFooter));
	file->seek(file->getSize()-sizeof(STGAFooter));
	file->read(&footer, sizeof(STGAFooter));
	return (!strcmp(footer.Signature,"TRUEVISION-XFILE.")); // very old tgas are refused.
}



//! creates a surface from the file
IImage* CImageLoaderTGA::loadImage(io::IReadFile* file) const
{
	STGAHeader header = {};
	std::array<s32, 256> palette = {}; // large enough for all 8bpp images to avoid out-of-bounds access

	file->read(&header, sizeof(STGAHeader));

#ifdef __BIG_ENDIAN__
	header.ColorMapLength = os::Byteswap::byteswap(header.ColorMapLength);
	header.ImageWidth = os::Byteswap::byteswap(header.ImageWidth);
	header.ImageHeight = os::Byteswap::byteswap(header.ImageHeight);
#endif

	const u64 imageSize = (u64)header.ImageHeight * header.ImageWidth * header.PixelDepth / 8;
	if (imageSize >= 0x1000'0000) { // 1/4 GiB
		os::Printer::log("Image dimensions too large in file", file->getFileName(), ELL_ERROR);
		return nullptr;
	}

	// skip image identification field
	if (header.IdLength)
		file->seek(header.IdLength, true);

	if (header.ColorMapType) {
		if (header.ColorMapLength > palette.size()) {
			os::Printer::log("Image palette too large in file", file->getFileName(), ELL_ERROR);
			return nullptr;
		}

		void (*convert_palette)(const void* sP, s32 sN, void* dP);
		switch (header.ColorMapEntrySize) {
			case 16: convert_palette = CColorConverter::convert_A1R5G5B5toA8R8G8B8; break;
			case 24: convert_palette = CColorConverter::convert_B8G8R8toA8R8G8B8; break;
			case 32: convert_palette = CColorConverter::convert_B8G8R8A8toA8R8G8B8; break;
			default:
				os::Printer::log("Image palette has wrong entry size in file", file->getFileName(), ELL_ERROR);
				return nullptr;
		}

		std::vector<u8> original_palette(header.ColorMapEntrySize/8 * header.ColorMapLength);
		file->read(original_palette.data(), original_palette.size());
		convert_palette(original_palette.data(), header.ColorMapLength, palette.data());
	}

	// read image

	std::vector<u8> data;

	if (	header.ImageType == 1 || // Uncompressed, color-mapped images.
			header.ImageType == 2 || // Uncompressed, RGB images
			header.ImageType == 3 // Uncompressed, black and white images
		) {
		data.resize(imageSize);
		if (file->read(data.data(), imageSize) != imageSize) {
			os::Printer::log("Failed to read image data from file", file->getFileName(), ELL_ERROR);
			return nullptr;
		}
	}
	else
	if(header.ImageType == 10)
	{
		// Runlength encoded RGB images
		data = loadCompressedImage(file, header);
		if (data.empty()) {
			os::Printer::log("Not a valid TGA file", file->getFileName(), ELL_ERROR);
			return nullptr;
		}
	}
	else
	{
		os::Printer::log("Unsupported TGA file type", file->getFileName(), ELL_ERROR);
		return 0;
	}

	IImage* image = 0;

	switch(header.PixelDepth)
	{
	case 8:
		{
			if (header.ImageType==3) // grey image
			{
				image = new CImage(ECF_R8G8B8,
					core::dimension2d<u32>(header.ImageWidth, header.ImageHeight));
				if (image)
					CColorConverter::convert8BitTo24Bit((u8*)data.data(),
						(u8*)image->getData(),
						header.ImageWidth,header.ImageHeight,
						0, 0, (header.ImageDescriptor&0x20)==0);
			}
			else
			{
				image = new CImage(ECF_A1R5G5B5,
					core::dimension2d<u32>(header.ImageWidth, header.ImageHeight));
				if (image)
					CColorConverter::convert8BitTo16Bit((u8*)data.data(),
						(s16*)image->getData(),
						header.ImageWidth,header.ImageHeight,
						palette.data(), 0,
						(header.ImageDescriptor&0x20)==0);
			}
		}
		break;
	case 16:
		image = new CImage(ECF_A1R5G5B5,
			core::dimension2d<u32>(header.ImageWidth, header.ImageHeight));
		if (image)
			CColorConverter::convert16BitTo16Bit((s16*)data.data(),
				(s16*)image->getData(), header.ImageWidth,	header.ImageHeight, 0, (header.ImageDescriptor&0x20)==0);
		break;
	case 24:
			image = new CImage(ECF_R8G8B8,
				core::dimension2d<u32>(header.ImageWidth, header.ImageHeight));
			if (image)
				CColorConverter::convert24BitTo24Bit(
					(u8*)data.data(), (u8*)image->getData(), header.ImageWidth, header.ImageHeight, 0, (header.ImageDescriptor&0x20)==0, true);
		break;
	case 32:
			image = new CImage(ECF_A8R8G8B8,
				core::dimension2d<u32>(header.ImageWidth, header.ImageHeight));
			if (image)
				CColorConverter::convert32BitTo32Bit((s32*)data.data(),
					(s32*)image->getData(), header.ImageWidth, header.ImageHeight, 0, (header.ImageDescriptor&0x20)==0);
		break;
	default:
		os::Printer::log("Unsupported TGA format", file->getFileName(), ELL_ERROR);
		break;
	}

	return image;
}


//! creates a loader which is able to load tgas
IImageLoader* createImageLoaderTGA()
{
	return new CImageLoaderTGA();
}


} // end namespace video
} // end namespace irr
