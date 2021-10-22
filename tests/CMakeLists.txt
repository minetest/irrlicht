# Do not GLOB because otherwise new tests will not trigger rebuilding!
add_executable(tests
	main.cpp
	testUtils.cpp
	testUtils.h
	# Does workspace validation.
	disambiguateTextures.cpp
	# Simple tests without device.
	irrArray.cpp
	irrMap.cpp
	irrList.cpp
	exports.cpp
	irrCoreEquals.cpp
	irrString.cpp
	testLine2d.cpp
	matrixOps.cpp
	testDimension2d.cpp
	testVector2d.cpp
	testVector3d.cpp
	testQuaternion.cpp
	testS3DVertex.cpp
	testaabbox.cpp
	color.cpp
	triangle3d.cpp
	vectorPositionDimension2d.cpp
#[[
	2dmaterial.cpp
	anti-aliasing.cpp
	archiveReader.cpp
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
	filesystem.cpp
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
	serializeAttributes.cpp
	skinnedMesh.cpp
	softwareDevice.cpp
	stencilshadow.cpp
	terrainSceneNode.cpp
	testGeometryCreator.cpp
	testXML.cpp
	textureFeatures.cpp
	textureRenderStates.cpp
	timer.cpp
	transparentMaterials.cpp
	triangleSelector.cpp
	userClipPlane.cpp
	videoDriver.cpp
	viewPort.cpp
	writeImageToFile.cpp
#]]
)

# Based on Makefile as used in original Irrlicht.
target_compile_options(tests
	PRIVATE
		-pipe
		-Wall
		-ansi
		-pedantic
		-fno-exceptions
)

target_link_libraries(tests
	PRIVATE
		IrrlichtMt
)

add_test(
	NAME IrrlichtMtTests
	COMMAND tests
	WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/tests"
)