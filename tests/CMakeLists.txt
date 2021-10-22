# Registers a test case TEST_NAME. Source files are taken up by ARGN variable.
# testUtils.cpp is linked by default.
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
# Do not GLOB because otherwise new tests will not trigger rebuilding!
#[[
add_executable(tests
	# File system checks.
	#[[
	filesystem.cpp
	archiveReader.cpp
	serializeAttributes.cpp
	]]#
#[[
	2dmaterial.cpp
	anti-aliasing.cpp
	b3dAnimation.cpp
	billboards.cpp
	burningsVideo.cpp
	collisionResponseAnimator.cpp
	coreutil.cpp
	createImage.cpp
	cursorSetVisible.cpp
	disambiguateTextures.cpp
	draw2DImage.cpp
	drawPixel.cpp
	drawRectOutline.cpp
	drawVertexPrimitive.cpp
	enumerateImageManipulators.cpp
	fast_atof.cpp
	flyCircleAnimator.cpp
	guiDisabledMenu.cpp
	ioScene.cpp
	lightMaps.cpp
	lights.cpp
	loadTextures.cpp
	makeColorKeyTexture.cpp
	material.cpp
	md2Animation.cpp
	meshLoaders.cpp
	meshTransform.cpp
	mrt.cpp
	orthoCam.cpp
	planeMatrix.cpp
	projectionMatrix.cpp
	removeCustomAnimator.cpp
	renderTargetTexture.cpp
	sceneCollisionManager.cpp
	sceneNodeAnimator.cpp
	screenshot.cpp
	skinnedMesh.cpp
	softwareDevice.cpp
	stencilshadow.cpp
	terrainSceneNode.cpp
	testGeometryCreator.cpp
	textureFeatures.cpp
	textureRenderStates.cpp
	timer.cpp
	transparentMaterials.cpp
	triangleSelector.cpp
	userClipPlane.cpp
	videoDriver.cpp
	viewPort.cpp
	writeImageToFile.cpp
)
#]]
