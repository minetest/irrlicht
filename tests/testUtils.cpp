// Copyright (C) 2008-2012 Colin MacDonald
// No rights reserved: this software is in the public domain.

#include "testUtils.h"
#include <memory.h>
#include <stdio.h>
#include <string>
#include <assert.h>
#include <stdarg.h>

#if defined(_MSC_VER)  || defined(_IRR_WINDOWS_API_)
#define _CRT_SECURE_NO_WARNINGS 1
#include <windows.h> // For OutputDebugString()
#endif // _MSC_VER || _IRR_WINDOWS_API_

#ifdef _MSC_VER
#pragma warning( disable: 4996)
#endif

using namespace irr;

bool binaryCompareFiles(const char * fileName1, const char * fileName2)
{
	assert(fileName1);
	assert(fileName2);
	if (!fileName1 || !fileName2)
		return false;

	FILE * file1 = fopen(fileName1, "rb");
	if (!file1)
	{
		std::cerr << "binaryCompareFiles: File '"
			<< fileName1 << "' cannot be opened\n";
		assert(file1);
		return false;
	}

	FILE * file2 = fopen(fileName2, "rb");
	if (!file2)
	{
		std::cerr << "binaryCompareFiles: File '"
			<< fileName2 << "' cannot be opened\n";
		(void)fclose(file1);
		assert(file2);
		return false;
	}


	(void)fseek(file1, 0, SEEK_END);
	(void)fseek(file2, 0, SEEK_END);
	const size_t file1Size = ftell(file1);
	const size_t file2Size = ftell(file2);
	if (file1Size != file2Size)
	{
		std::cerr << "binaryCompareFiles: Files are different sizes: "
			<< file1Size << " vs " << file2Size << '\n';
		(void)fclose(file1);
		(void)fclose(file2);
		return false;
	}

	(void)fseek(file1, 0, SEEK_SET);
	(void)fseek(file2, 0, SEEK_SET);

	char file1Buffer[8196];
	char file2Buffer[8196];

	while (!feof(file1))
	{
		if (feof(file2)
			||(fread(file1Buffer, sizeof(file1Buffer), 1, file1) !=
			fread(file2Buffer, sizeof(file2Buffer), 1, file2)))
		{
			std::cerr
				<< "binaryCompareFiles: Error reading file\n";
			break;
		}

		if (memcmp(file1Buffer, file2Buffer, sizeof(file1Buffer)))
		{
			std::cerr << "binaryCompareFiles: files differ\n";
			break;
		}
	}

	bool filesAreIdentical = feof(file1) && feof(file2);
	(void)fclose(file1);
	(void)fclose(file2);

	return filesAreIdentical;
}


//! Compare two images, returning the degree to which they match.
/** \param image1 The first image to compare.
	\param image2 The second image to compare.
	\return The match, from 0.f to 100.f */
static float fuzzyCompareImages(irr::video::IImage * image1,
								irr::video::IImage * image2)
{
	assert(image1);
	assert(image2);
	if (!image1 || !image2)
		return 0.f;

	if (image1->getDimension() != image2->getDimension())
	{
		std::cerr << "fuzzyCompareImages: image sizes differ\n";
		return 0.f;
	}

	video::ECOLOR_FORMAT format1 = image1->getColorFormat();
	if (video::ECF_A8R8G8B8 != format1 && video::ECF_R8G8B8 != format1)
	{
		std::cerr << "fuzzyCompareImages: image 1 must be "\
			"ECF_AR8G8B8 or ECF_R8G8B8\n";
		return 0.f;
	}

	video::ECOLOR_FORMAT format2 = image2->getColorFormat();
	if (video::ECF_A8R8G8B8 != format2 && video::ECF_R8G8B8 != format2)
	{
		std::cerr << "fuzzyCompareImages: image 2 must be "\
			"ECF_AR8G8B8 or ECF_R8G8B8\n";
		return 0.f;
	}

	u8 * image1Data = (u8*)image1->getData();
	u8 * image2Data = (u8*)image2->getData();

	const u32 pixels = (image1->getPitch() * image1->getDimension().Height) / 4;
	u32 mismatchedColours = 0;
	for (u32 pixel = 0; pixel < pixels; ++pixel)
	{
		if (video::ECF_A8R8G8B8 == format1)
			image1Data++;

		const u8 r1 = *(image1Data++);
		const u8 g1 = *(image1Data++);
		const u8 b1 = *(image1Data++);

		if (video::ECF_A8R8G8B8 == format2)
			image2Data++;

		const u8 r2 = *(image2Data++);
		const u8 g2 = *(image2Data++);
		const u8 b2 = *(image2Data++);

		mismatchedColours += abs(r1 - r2) + abs(g1 - g2) + abs(b1 - b2);
	}

	const u32 totalColours = pixels * 255*3;
	return 100.f * (totalColours - mismatchedColours) / totalColours;
}


//! Compare two images, returning the degree to which they match.
/** \param image1 The first image to compare.
	\param image2 The second image to compare.
	\return The match, from 0.f to 100.f */
float fuzzyCompareImages(irr::video::IVideoDriver * driver,
		const char * fileName1, const char * fileName2)
{
	assert(fileName1);
	assert(fileName2);
	irr::video::IImage * img1 = driver->createImageFromFile(fileName1);
	if (!img1)
		return 0;
	irr::video::IImage * img2 = driver->createImageFromFile(fileName2);
	const float result = fuzzyCompareImages(img1, img2);
	std::cerr << "Image match: " << result << '\n';
	img1->drop();
	if (img2)
		img2->drop();
	return result;
}

void stabilizeScreenBackground(irr::video::IVideoDriver * driver,
		irr::video::SColor color)
{
	for(int i = 0; i < 10000; ++i) 
	{
		driver->beginScene(video::ECBF_COLOR | video::ECBF_DEPTH, color);
		driver->endScene();

		irr::video::IImage * screenshot = driver->createScreenShot();
		if (!screenshot)
			return;

		const video::ECOLOR_FORMAT format = screenshot->getColorFormat();
		if (format != video::ECF_R8G8B8)
		{
			irr::video::IImage * fixedScreenshot = driver->createImage(video::ECF_R8G8B8, screenshot->getDimension());
			screenshot->copyTo(fixedScreenshot);
			screenshot->drop();

			if (!fixedScreenshot)
				return;

			screenshot = fixedScreenshot;
		}

		u8 * image1Data = (u8*)screenshot->getData();

		const u32 pixels = (screenshot->getPitch() * screenshot->getDimension().Height) / 4;
		bool status = true;
		for(u32 pixel = 0; pixel < pixels; ++pixel)
		{
			const u8 r = *(image1Data++);
			const u8 g = *(image1Data++);
			const u8 b = *(image1Data++);

			if (r != color.getRed() || g != color.getGreen() || b != color.getBlue())
			{
				status = false;
				break;
			}
		}

		if (status)
		{
			screenshot->drop();
			return;
		}
		screenshot->drop();
	}
	
	std::cerr << "stabilizeScreenBackground failed\n";
}

irr::core::stringc shortDriverName(irr::video::IVideoDriver * driver)
{
	irr::core::stringc driverName = driver->getName();

	// For OpenGL and Burning, chop the version number out. Other drivers have more stable version numbers.
	// TA: Sorry Rogerborg. burnings video also has the version number inside;-)
	//     maybe you sould take the getDriverType Info for this
	if (driverName.find("OpenGL") > -1)
		driverName = "OpenGL";
	else if (driverName.find("Burning's Video") > -1)
		driverName = "Burning's Video";

	return driverName;
}

int runTest(bool (*testFunction)(), const std::string &testName)
{
	std::cerr << "\nStarting test " << testName << '\n';

	bool testPassed = testFunction();
	if (!testPassed)
	{
		std::cerr << "\n******** Test failure ********\n"\
			"Test '" << testName << "' failed\n"\
			"******** Test failure ********\n";
		return 1;
	}
	return 0;
}