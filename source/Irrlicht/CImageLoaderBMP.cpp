// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CImageLoaderBMP.h"

#ifdef _IRR_COMPILE_WITH_BMP_LOADER_

#include "IReadFile.h"
#include "SColor.h"
#include "CColorConverter.h"
#include "CImage.h"
#include "os.h"

namespace irr
{
namespace video
{


//! constructor
CImageLoaderBMP::CImageLoaderBMP()
{
	#ifdef _DEBUG
	setDebugName("CImageLoaderBMP");
	#endif
}


//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".tga")
bool CImageLoaderBMP::isALoadableFileExtension(const io::path& filename) const
{
	return core::hasFileExtension ( filename, "bmp" );
}


//! returns true if the file maybe is able to be loaded by this class
bool CImageLoaderBMP::isALoadableFileFormat(io::IReadFile* file) const
{
	u16 headerID;
	file->read(&headerID, sizeof(u16));
#ifdef __BIG_ENDIAN__
	headerID = os::Byteswap::byteswap(headerID);
#endif
	return headerID == 0x4d42;
}


// UB-safe overflow check
static inline bool doesOverflow(const void *base, size_t numElements, const void *end)
{
	// TODO: uintptr_t (as in original patch from sfan5) would be nicer than size_t, use once we allow c++11
	size_t baseI = reinterpret_cast<size_t>(base);
	size_t endI = reinterpret_cast<size_t>(end);
	return baseI > endI || numElements >= (endI - baseI);
}

// check whether &p[0] to &p[_off - 1] can be accessed
#define EXIT_P_OVERFLOW(_off) if (doesOverflow(p, _off, pEnd)) goto exit
// same for d
#define EXIT_D_OVERFLOW(_off) if (doesOverflow(d, _off, destEnd)) goto exit

void CImageLoaderBMP::decompress8BitRLE(u8*& bmpData, s32 size, s32 width, s32 height, s32 pitch) const
{
	u8* p = bmpData;
	const u8* pEnd = bmpData + size;
	u8* newBmp = new u8[(width+pitch)*height];
	u8* d = newBmp;
	const u8* destEnd = newBmp + (width+pitch)*height;
	s32 line = 0;

	while (p < pEnd && d < destEnd)
	{
		if (*p == 0)
		{
			++p;
			EXIT_P_OVERFLOW(1);

			switch(*p)
			{
			case 0: // end of line
				++p;
				++line;
				d = newBmp + (line*(width+pitch));
				break;
			case 1: // end of bmp
				goto exit;
			case 2:
				++p;
				EXIT_P_OVERFLOW(2);
				d += (u8)*p; // delta
				++p;
				d += ((u8)*p)*(width+pitch);
				++p;
				break;
			default:
				{
					// absolute mode
					const s32 count = (u8)*p;
					++p;

					EXIT_P_OVERFLOW(count);
					EXIT_D_OVERFLOW(count);
					for (s32 i=0; i<count; ++i)
					{
						*d = *p;
						++p;
						++d;
					}

					const s32 readAdditional = ((2-(count%2))%2);
					EXIT_P_OVERFLOW(readAdditional);
					for (s32 i=0; i<readAdditional; ++i)
						++p;
				}
			}
		}
		else
		{
			const s32 count = (u8)*p; ++p;
			EXIT_P_OVERFLOW(1);
			u8 color = *p; ++p;
			EXIT_D_OVERFLOW(count);
			for (s32 i=0; i<count; ++i)
			{
				*d = color;
				++d;
			}
		}
	}

exit:
	delete [] bmpData;
	bmpData = newBmp;
}


// how many new bytes will be touched given the current state of decompress4BitRLE
static inline u32 shiftedCount(s32 count, s32 shift)
{
	IRR_DEBUG_BREAK_IF(count <= 0)

	u32 ret = 0;
	if ( shift == 0 )	// using up half of an old byte
	{
		--count;
	}
	ret += (count / 2) + (count % 2);
	return ret;
}

// Ensure current row/line are inside width/height
// Didn't find any documentation how BMP's are supposed to handle this
// I think in general "good" bmp's are supposed to not go over line-ends
#define KEEP_ROW_LINE_INSIDE \
	if ( row >= width ) \
	{ \
		line += row/width; \
		row %= width; \
	} \
	if ( line >= height ) \
	{ \
		line = 0; /* bug anyway, this way more visible maybe */ \
	}


void CImageLoaderBMP::decompress4BitRLE(u8*& bmpData, s32 size, s32 width, s32 height, s32 pitch) const
{
	const s32 lineWidth = (width+1)/2+pitch;
	u8* p = bmpData;
	const u8* pEnd = bmpData + size;
	u8* newBmp = new u8[lineWidth*height];
	memset(newBmp, 0, lineWidth*height);	// Extra cost, but otherwise we have to code pixel skipping stuff
	const u8* destEnd = newBmp + lineWidth*height;
	s32 line = 0;
	s32 row = 0;

	while (p < pEnd)
	{
		if (*p == 0)
		{
			++p;
			EXIT_P_OVERFLOW(1);

			switch(*p)
			{
			case 0: // end of line
				++p;
				++line;
				row = 0;
				break;
			case 1: // end of bmp
				goto exit;
			case 2: // delta
				{
					++p;
					EXIT_P_OVERFLOW(2);
					s32 x = (u8)*p; ++p;
					s32 y = (u8)*p; ++p;
					row += x;
					line += y;
				}
				break;
			default:
				{
					// absolute mode
					const u32 count = (u8)*p; ++p;
					s32 readShift = 4;

					KEEP_ROW_LINE_INSIDE;
					s32 shift = row%2 == 0 ? 4 : 0;
					u8* d = newBmp + (lineWidth*line + row/2);

					EXIT_P_OVERFLOW(shiftedCount(count, readShift));
					EXIT_D_OVERFLOW(shiftedCount(count, shift));
					for (u32 i=0; i<count; ++i)
					{
						s32 color = (((u8)*p) >> readShift) & 0x0f;
						readShift -= 4;
						if (readShift < 0)
						{
							++p;
							readShift = 4;
						}

						u8 mask = 0x0f << shift;
						*d = (*d & (~mask)) | ((color << shift) & mask);

						shift -= 4;
						if (shift < 0)
						{
							shift = 4;
							++d;
						}
					}
					row += count;

					// pixels always come in 2-byte packages with unused half-bytes filled with zeroes
					const u32 modCount = (count%4);
					const u32 readAdditional = modCount == 1 ? 2 : (modCount==0?0:1); // jump 2,1,1,0 for count 1,2,3,4
					EXIT_P_OVERFLOW(readAdditional);
					for (u32 i=0; i<readAdditional; ++i)
						++p;
				}
			}
		}
		else // encoded mode
		{
			const s32 count = (u8)*p;	// pixels to draw
			++p;
			EXIT_P_OVERFLOW(1);
			s32 color1 = (u8)*p; color1 = color1 & 0x0f;		// lo bit (2nd,4th,... pixel)
			s32 color2 = (u8)*p; color2 = (color2 >> 4) & 0x0f;	// hi bits (1st,3rd,... pixel)
			++p;

			KEEP_ROW_LINE_INSIDE;
			s32 shift = row%2 == 0 ? 4 : 0;
			u8* d = newBmp + (lineWidth*line + row/2);

			EXIT_D_OVERFLOW(shiftedCount(count, shift));
			for (s32 i=0; i<count; ++i)
			{
				u8 mask = 0x0f << shift;
				u8 toSet = ((i%2==0) ? color2 : color1) << shift;
				*d = (*d & (~mask)) | toSet;

				shift -= 4;
				if (shift < 0)
				{
					shift = 4;
					++d;
				}
			}
			row += count;
		}
	}

exit:
	delete [] bmpData;
	bmpData = newBmp;
}

#undef EXIT_P_OVERFLOW
#undef EXIT_D_OVERFLOW
#undef KEEP_ROW_LINE_INSIDE

//! creates a surface from the file
IImage* CImageLoaderBMP::loadImage(io::IReadFile* file) const
{
	SBMPHeader header;

	file->read(&header, sizeof(header));

#ifdef __BIG_ENDIAN__
	header.Id = os::Byteswap::byteswap(header.Id);
	header.FileSize = os::Byteswap::byteswap(header.FileSize);
	header.BitmapDataOffset = os::Byteswap::byteswap(header.BitmapDataOffset);
	header.BitmapHeaderSize = os::Byteswap::byteswap(header.BitmapHeaderSize);
	header.Width = os::Byteswap::byteswap(header.Width);
	header.Height = os::Byteswap::byteswap(header.Height);
	header.Planes = os::Byteswap::byteswap(header.Planes);
	header.BPP = os::Byteswap::byteswap(header.BPP);
	header.Compression = os::Byteswap::byteswap(header.Compression);
	header.BitmapDataSize = os::Byteswap::byteswap(header.BitmapDataSize);
	header.PixelPerMeterX = os::Byteswap::byteswap(header.PixelPerMeterX);
	header.PixelPerMeterY = os::Byteswap::byteswap(header.PixelPerMeterY);
	header.Colors = os::Byteswap::byteswap(header.Colors);
	header.ImportantColors = os::Byteswap::byteswap(header.ImportantColors);
#endif

	//! return if the header is false

	bool topDown = false;
	if (header.Id == 0x4d42) // BM = Windows header
	{
		// Sizes are signed integer with Windows header (unsigned with OS/2)
		// Not sure if negative Width has any meaning, but negative height is for upside-down
		header.Width = core::abs_((s32)header.Width);
		if ( (s32)header.Height < 0 )
		{
			topDown = true;
			header.Height = core::abs_((s32)header.Height);
		}
	}
	else
	{
		os::Printer::log("BMP files with OS/2 Headers not supported.", ELL_ERROR);
		return 0;
	}

	if (header.Compression > 2) // we'll only handle RLE-Compression
	{
		os::Printer::log("Compression mode not supported.", ELL_ERROR);
		return 0;
	}

	// adjust bitmap data size to dword boundary
	header.BitmapDataSize += (4-(header.BitmapDataSize%4))%4;

	// read palette

	long pos = file->getPos();
	s32 paletteSize = (header.BitmapDataOffset - pos) / 4;

	s32* paletteData = 0;
	if (paletteSize)
	{
		paletteData = new s32[paletteSize];
		file->read(paletteData, paletteSize * sizeof(s32));
#ifdef __BIG_ENDIAN__
		for (s32 i=0; i<paletteSize; ++i)
			paletteData[i] = os::Byteswap::byteswap(paletteData[i]);
#endif
	}

	// read image data

	if (!header.BitmapDataSize)
	{
		// okay, lets guess the size
		// some tools simply don't set it
		header.BitmapDataSize = static_cast<u32>(file->getSize()) - header.BitmapDataOffset;
	}

	file->seek(header.BitmapDataOffset);

	const s32 widthInBytes = core::ceil32(header.Width * (header.BPP / 8.0f));
	const s32 lineSize = widthInBytes + ((4-(widthInBytes%4)))%4;
	const s32 pitch = lineSize - widthInBytes;

	u8* bmpData = new u8[header.BitmapDataSize];
	file->read(bmpData, header.BitmapDataSize);

	// decompress data if needed
	switch(header.Compression)
	{
	case 1: // 8 bit RLE
		decompress8BitRLE(bmpData, header.BitmapDataSize, header.Width, header.Height, pitch);
		header.BitmapDataSize = (header.Width + pitch) * header.Height;
		break;
	case 2: // 4 bit RLE
		decompress4BitRLE(bmpData, header.BitmapDataSize, header.Width, header.Height, pitch);
		header.BitmapDataSize = ((header.Width+1)/2 + pitch) * header.Height;
		break;
	}

	if (header.BitmapDataSize < lineSize * header.Height)
	{
		os::Printer::log("Bitmap data is cut off.", ELL_ERROR);

		delete [] paletteData;
		delete [] bmpData;
		return 0;
	}

	// create surface

	// no default constructor from packed area! ARM problem!
	core::dimension2d<u32> dim;
	dim.Width = header.Width;
	dim.Height = header.Height;

	IImage* image = 0;

	if ( !IImage::checkDataSizeLimit( (size_t)header.Width*header.Height*header.BPP) )
	{
		os::Printer::log("Image dimensions too large for file", file->getFileName(), ELL_WARNING);
		goto cleanup;
	}

	switch(header.BPP)
	{
	case 1:
		image = new CImage(ECF_A1R5G5B5, dim);
		if (image)
			CColorConverter::convert1BitTo16Bit(bmpData, (s16*)image->getData(), header.Width, header.Height, pitch, true);
		break;
	case 4:
		image = new CImage(ECF_A1R5G5B5, dim);
		if (image)
			CColorConverter::convert4BitTo16Bit(bmpData, (s16*)image->getData(), header.Width, header.Height, paletteData, pitch, true);
		break;
	case 8:
		image = new CImage(ECF_A1R5G5B5, dim);
		if (image)
			CColorConverter::convert8BitTo16Bit(bmpData, (s16*)image->getData(), header.Width, header.Height, paletteData, pitch, true);
		break;
	case 16:
		image = new CImage(ECF_A1R5G5B5, dim);
		if (image)
			CColorConverter::convert16BitTo16Bit((s16*)bmpData, (s16*)image->getData(), header.Width, header.Height, pitch, true);
		break;
	case 24:
		image = new CImage(ECF_R8G8B8, dim);
		if (image)
			CColorConverter::convert24BitTo24Bit(bmpData, (u8*)image->getData(), header.Width, header.Height, pitch, true, true);
		break;
	case 32: // thx to Reinhard Ostermeier
		image = new CImage(ECF_A8R8G8B8, dim);
		if (image)
			CColorConverter::convert32BitTo32Bit((s32*)bmpData, (s32*)image->getData(), header.Width, header.Height, pitch, true);
		break;
	};

	// clean up
cleanup:
	delete [] paletteData;
	delete [] bmpData;

	if ( image && topDown )
		image->flip(true, false);

	return image;
}


//! creates a loader which is able to load windows bitmaps
IImageLoader* createImageLoaderBMP()
{
	return new CImageLoaderBMP;
}


} // end namespace video
} // end namespace irr

#endif

