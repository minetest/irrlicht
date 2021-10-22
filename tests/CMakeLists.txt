set(UNITTEST_FLAGS "" CACHE STRING "Additional compile flags for unit tests.")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Registers a test case TEST_NAME. Source files are taken up by ARGN variable.
# testUtils.cpp is added to sources by default.
macro(REGISTER_TEST TEST_NAME)
	add_executable(${TEST_NAME} testUtils.cpp testUtils.h ${ARGN})

	# Based on Makefile as used in original Irrlicht.
	target_compile_options(${TEST_NAME}
		PRIVATE
			-pipe
			-Wall
			-ansi
			-pedantic
			-fno-exceptions
			${UNITTEST_FLAGS}
	)
	target_link_libraries(${TEST_NAME}
		PRIVATE
			IrrlichtMt
	)
	add_test(
		NAME ${TEST_NAME}
		COMMAND ${TEST_NAME}
		WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/tests"
	)
endmacro()

message(STATUS "Generating unit tests.")

# Validates working directory.
register_test(testDisambiguateTextures disambiguateTextures.cpp)

# Simple tests without device.
register_test(testIrrArray irrArray.cpp)
register_test(testIrrMap irrMap.cpp)
register_test(testIrrList irrList.cpp)
register_test(testExports exports.cpp)
register_test(testIrrCoreEquals irrCoreEquals.cpp)
register_test(testIrrString irrString.cpp)
register_test(testLine2DSimple testLine2d.cpp)
register_test(testMatrixOps matrixOps.cpp)
register_test(testDimension2D testDimension2d.cpp)
register_test(testVector2D testVector2d.cpp)
register_test(testVector3D testVector3d.cpp)
register_test(testQuaternion testQuaternion.cpp)
register_test(testS3DVertex testS3DVertex.cpp)
register_test(testaabbox testaabbox.cpp)
register_test(testColor color.cpp)
register_test(testTriangle3D triangle3d.cpp)
register_test(testVectorPositionDimension2D vectorPositionDimension2d.cpp)

# File system checks.
register_test(testFilesystem filesystem.cpp)
#register_test(testArchiveReader archiveReader.cpp)
register_test(testSerializeAttributes serializeAttributes.cpp)

# Null driver tests.
register_test(testFastAtof fast_atof.cpp)
# Yay, "Fast" Atof is only faster in Release mode, thank you for the confusion.
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_compile_definitions(testFastAtof PRIVATE _DEBUG)
endif()
register_test(testLoadTextures loadTextures.cpp)
register_test(testEnumerateImageManipulators enumerateImageManipulators.cpp)
register_test(testMeshLoaders meshLoaders.cpp)
register_test(testTimer timer.cpp)
register_test(testCoreutil coreutil.cpp)

# Software driver tests.
register_test(testCreateImage createImage.cpp)
#register_test(testCursorSetVisible cursorSetVisible.cpp)

# All driver checks.
register_test(testVideoDriver videoDriver.cpp)
register_test(testDrawRectOutline drawRectOutline.cpp)
register_test(testDrawVertexPrimitive drawVertexPrimitive.cpp)
register_test(testDraw2DImage draw2DImage.cpp)
register_test(testProjectionMatrix projectionMatrix.cpp)