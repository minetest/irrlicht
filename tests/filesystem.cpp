#include "testUtils.h"
#include <iostream>

using namespace irr;
using namespace core;
using namespace io;

static bool testgetAbsoluteFilename(io::IFileSystem *fs)
{
	bool result = true;
	io::path apath = fs->getAbsolutePath("media");
	io::path cwd = fs->getWorkingDirectory();
	if (!assertLog(apath == (cwd + "/media")))
	{
		result = false;
		std::cerr << "getAbsolutePath failed on existing dir "
			<< apath.c_str() << '\n';
	}

	apath = fs->getAbsolutePath("../media/");
	core::deletePathFromPath(cwd, 1);
	if (!assertLog(apath == (cwd + "media/")))
	{
		result = false;
		std::cerr << "getAbsolutePath failed on dir with postfix / "
			<< apath.c_str() << '\n';
	}

	apath = fs->getAbsolutePath ("../nothere.txt"); // file does not exist
	if (!assertLog(apath == (cwd + "nothere.txt")))
	{
		result = false;
		std::cerr << "getAbsolutePath failed on non-existing file "
			<< apath.c_str() << '\n';
	}

	return result;
}

struct FlattenFilenameCase
{
	io::path tmp;
	io::path expected;
};

constexpr const char test[]{ "hello there" };
FlattenFilenameCase flattenCases[]{
	{"../tmp", "../tmp/"},
	{"tmp/tmp/../", "tmp/"},
	{"tmp/tmp/..", "tmp/"},
	{"tmp/next/../third", "tmp/third/"},
	{"this/tmp/next/../../my/fourth", "this/my/fourth/"},
	{"this/is/../../../a/fifth/test", "../a/fifth/test/"},
	{"this/../is/../../a/sixth/test/", "../a/sixth/test/"}
};

static bool testFlattenFilename(io::IFileSystem* fs)
{
	for (FlattenFilenameCase &testCase : flattenCases)
	{
		const io::path &expected = testCase.expected;
		if (!assertLog(fs->flattenFilename(testCase.tmp) == expected))
		{
			std::cerr << "flattening destroys path.\n"
				<< testCase.tmp.c_str() << "!="
				<< expected.c_str() << '\n';
			return false;
		}
	}

	return true;
}

static bool testgetRelativeFilename(io::IFileSystem* fs)
{
	bool result = true;
	io::path apath = fs->getAbsolutePath("media");
	io::path cwd = fs->getWorkingDirectory();
	if (!assertLog(fs->getRelativeFilename(apath, cwd) == "media"))
	{
		result = false;
		std::cerr << "getRelativePath failed on "
			<< apath.c_str() << '\n';
	}

	apath = fs->getAbsolutePath("../media/");
	if (!assertLog(fs->getRelativeFilename(apath, cwd) == "../media/"))
	{
		result = false;
		std::cerr << "getRelativePath failed on "
			<< apath.c_str() << '\n';
	}

	return result;
}

bool filesystem(void)
{
	IrrlichtDevice * device = irr::createDevice(video::EDT_NULL, dimension2d<u32>(1, 1));
	assertLog(device);
	if(!device)
		return false;

	io::IFileSystem * fs = device->getFileSystem ();
	if ( !fs )
		return false;

	bool result = true;

	io::path workingDir = device->getFileSystem()->getWorkingDirectory();

	io::path empty;
	if (!assertLog(!fs->existFile(empty)))
	{
		result = false;
		std::cerr << "Empty filename should not exist.\n";
	}

	io::path newWd = workingDir + "/media";
	bool changed = device->getFileSystem()->changeWorkingDirectoryTo(newWd);
	assertLog(changed);

	if (!assertLog(!fs->existFile(empty)))
	{
		result = false;
		std::cerr << "Epmyt filename should not exist even in " \
			"another working directory\n";
	}

	// The working directory must be restored for the other tests to work.
	changed = device->getFileSystem()->changeWorkingDirectoryTo(workingDir.c_str());
	assertLog(changed);

	// adding  a folder archive which just should not really change anything
	device->getFileSystem()->addFileArchive( "./" );

	if (!assertLog(!fs->existFile(empty)))
	{
		result = false;
		std::cerr << "Empty filename should not exist in " \
			"folder file archive\n";
	}

	// remove it again to not affect other tests
	device->getFileSystem()->removeFileArchive( device->getFileSystem()->getFileArchiveCount() );

	result &= testFlattenFilename(fs);
	result &= testgetAbsoluteFilename(fs);
	result &= testgetRelativeFilename(fs);

	device->closeDevice();
	device->run();
	device->drop();

	return result;
}

int main()
{
	return runTest(filesystem, "testFilesystem");
}
