set(UNITTEST_FLAGS "" CACHE STRING "Additional compile flags for unit tests.")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Yeah, not recompiling testUtils for every test is probably a good idea.
add_library(testUtils OBJECT testUtils.cpp testUtils.h)
target_link_libraries(testUtils PUBLIC IrrlichtMt)

# Registers a test case TEST_NAME.
macro(REGISTER_TEST TEST_NAME)
	add_executable(${TEST_NAME} ${ARGN})

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
			testUtils
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
register_test(testVector3D testVector3d.cpp)
register_test(testS3DVertex testS3DVertex.cpp)
register_test(testColor color.cpp)

# File system checks.
register_test(testFilesystem filesystem.cpp)

# Null driver tests.
register_test(testMeshLoaders meshLoaders.cpp)
register_test(testTimer timer.cpp)
register_test(testCoreutil coreutil.cpp)
