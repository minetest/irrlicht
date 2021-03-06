include_directories(${CMAKE_SOURCE_DIR}/include)
add_definitions(-DIRRLICHT_EXPORTS=1)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_definitions(-D_DEBUG)
endif()
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

if(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3")
	set(CMAKE_C_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
	set(CMAKE_CXX_FLAGS_DEBUG "-g")
	set(CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})

	add_compile_options(-Wall -pipe -fno-exceptions -fno-rtti)
elseif(MSVC)
	# entirely untested
	set(CMAKE_CXX_FLAGS_RELEASE "/GL /MT /Ox")
	set(CMAKE_C_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
	set(CMAKE_CXX_FLAGS_DEBUG "/MTd /RTC1 /Zi")
	set(CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})

	set(CMAKE_CXX_FLAGS "/Gd /GF /GR- /GS /Gy /Zl")
	set(CMAKE_EXE_LINKER_FLAGS "/nologo")
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /OPT:REF /LTCG")
endif()

find_package(ZLIB REQUIRED)
find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)
include_directories(${ZLIB_INCLUDE_DIR} ${JPEG_INCLUDE_DIR} ${PNG_INCLUDE_DIR})
link_libraries(${ZLIB_LIBRARY} ${JPEG_LIBRARY} ${PNG_LIBRARY})

# To configure the features available in this Irrlicht build please edit
# include/IrrCompileConfig.h and re-run CMake from a clean state
include(CheckSymbolExists)
set(CMAKE_REQUIRED_INCLUDES ${CMAKE_SOURCE_DIR}/include)

#check_symbol_exists(_IRR_COMPILE_WITH_OGLES1_ "IrrCompileConfig.h" OGLES1_ENABLED)
#if(OGLES1_ENABLED)
#endif()
check_symbol_exists(_IRR_COMPILE_WITH_OGLES2_ "IrrCompileConfig.h" OGLES2_ENABLED)
if(OGLES2_ENABLED)
	find_package(OpenGLES2 REQUIRED)
	include_directories(${OPENGLES2_INCLUDE_DIR} ${EGL_INCLUDE_DIR})
	link_libraries(${OPENGLES2_LIBRARIES})
endif()
check_symbol_exists(_IRR_COMPILE_WITH_OPENGL_ "IrrCompileConfig.h" OGL_ENABLED)
if(OGL_ENABLED)
	set(OpenGL_GL_PREFERENCE "LEGACY")
	find_package(OpenGL REQUIRED)
	include_directories(${OPENGL_INCLUDE_DIR})
	link_libraries(${OPENGL_LIBRARIES})
endif()

# Platform-specific libs

if(ANDROID)
	# TODO
elseif(APPLE)
	# TODO probably these:
	#find_library(COCOA_LIB Cocoa REQUIRED)
	#find_library(IOKIT_LIB IOKit REQUIRED)
elseif(WIN32)
	# -lgdi32 -lwinmm?
else()
	# Unix probably
	find_package(X11 REQUIRED)
	include_directories(${X11_INCLUDE_DIR})
	link_libraries(${X11_X11_LIB} ${X11_Xxf86vm_LIB})
endif()

# Source files

set(IRRMESHLOADER
	CBSPMeshFileLoader.cpp
	CMD2MeshFileLoader.cpp
	CMD3MeshFileLoader.cpp
	CMS3DMeshFileLoader.cpp
	CB3DMeshFileLoader.cpp
	C3DSMeshFileLoader.cpp
	COgreMeshFileLoader.cpp
	COBJMeshFileLoader.cpp
	CColladaFileLoader.cpp
	CCSMLoader.cpp
	CDMFLoader.cpp
	CLMTSMeshFileLoader.cpp
	CMY3DMeshFileLoader.cpp
	COCTLoader.cpp
	CXMeshFileLoader.cpp
	CIrrMeshFileLoader.cpp
	CSTLMeshFileLoader.cpp
	CLWOMeshFileLoader.cpp
	CPLYMeshFileLoader.cpp
	CSMFMeshFileLoader.cpp
	CMeshTextureLoader.cpp
)

set(IRRMESHWRITER
	CColladaMeshWriter.cpp
	CIrrMeshWriter.cpp
	CSTLMeshWriter.cpp
	COBJMeshWriter.cpp
	CPLYMeshWriter.cpp
	CB3DMeshWriter.cpp
)

add_library(IRRMESHOBJ OBJECT
	CSkinnedMesh.cpp
	CBoneSceneNode.cpp
	CMeshSceneNode.cpp
	CAnimatedMeshSceneNode.cpp
	CAnimatedMeshMD2.cpp
	CAnimatedMeshMD3.cpp
	CQ3LevelMesh.cpp
	CQuake3ShaderSceneNode.cpp
	CAnimatedMeshHalfLife.cpp
	${IRRMESHLOADER}
	${IRRMESHWRITER}
)

add_library(IRROBJ OBJECT
	CBillboardSceneNode.cpp
	CCameraSceneNode.cpp
	CDummyTransformationSceneNode.cpp
	CEmptySceneNode.cpp
	CGeometryCreator.cpp
	CLightSceneNode.cpp
	CMeshManipulator.cpp
	CMetaTriangleSelector.cpp
	COctreeSceneNode.cpp
	COctreeTriangleSelector.cpp
	CSceneCollisionManager.cpp
	CSceneManager.cpp
	CShadowVolumeSceneNode.cpp
	CSkyBoxSceneNode.cpp
	CSkyDomeSceneNode.cpp
	CTerrainSceneNode.cpp
	CTerrainTriangleSelector.cpp
	CVolumeLightSceneNode.cpp
	CCubeSceneNode.cpp
	CSphereSceneNode.cpp
	CTextSceneNode.cpp
	CTriangleBBSelector.cpp
	CTriangleSelector.cpp
	CWaterSurfaceSceneNode.cpp
	CMeshCache.cpp
	CDefaultSceneNodeAnimatorFactory.cpp
	CDefaultSceneNodeFactory.cpp
	CSceneLoaderIrr.cpp
)

add_library(IRRPARTICLEOBJ OBJECT
	CParticleAnimatedMeshSceneNodeEmitter.cpp
	CParticleBoxEmitter.cpp
	CParticleCylinderEmitter.cpp
	CParticleMeshEmitter.cpp
	CParticlePointEmitter.cpp
	CParticleRingEmitter.cpp
	CParticleSphereEmitter.cpp
	CParticleAttractionAffector.cpp
	CParticleFadeOutAffector.cpp
	CParticleGravityAffector.cpp
	CParticleRotationAffector.cpp
	CParticleSystemSceneNode.cpp
	CParticleScaleAffector.cpp
)

add_library(IRRANIMOBJ OBJECT
	CSceneNodeAnimatorCameraFPS.cpp
	CSceneNodeAnimatorCameraMaya.cpp
	CSceneNodeAnimatorCollisionResponse.cpp
	CSceneNodeAnimatorDelete.cpp
	CSceneNodeAnimatorFlyCircle.cpp
	CSceneNodeAnimatorFlyStraight.cpp
	CSceneNodeAnimatorFollowSpline.cpp
	CSceneNodeAnimatorRotation.cpp
	CSceneNodeAnimatorTexture.cpp
)

set(IRRDRVROBJ
	CNullDriver.cpp
	COpenGLCacheHandler.cpp
	COpenGLDriver.cpp
	COpenGLNormalMapRenderer.cpp
	COpenGLParallaxMapRenderer.cpp
	COpenGLShaderMaterialRenderer.cpp
	COpenGLSLMaterialRenderer.cpp
	COpenGLExtensionHandler.cpp
	CD3D9Driver.cpp
	CD3D9HLSLMaterialRenderer.cpp
	CD3D9NormalMapRenderer.cpp
	CD3D9ParallaxMapRenderer.cpp
	CD3D9ShaderMaterialRenderer.cpp
	CD3D9Texture.cpp
	COGLESDriver.cpp
	COGLESExtensionHandler.cpp
	COGLES2Driver.cpp
	COGLES2ExtensionHandler.cpp
	COGLES2FixedPipelineRenderer.cpp
	COGLES2MaterialRenderer.cpp
	COGLES2NormalMapRenderer.cpp
	COGLES2ParallaxMapRenderer.cpp
	COGLES2Renderer2D.cpp
	CWebGL1Driver.cpp
	CGLXManager.cpp
	CWGLManager.cpp
	CEGLManager.cpp
)

set(IRRIMAGEOBJ
	CColorConverter.cpp
	CImage.cpp
	CImageLoaderBMP.cpp
	CImageLoaderDDS.cpp
	CImageLoaderJPG.cpp
	CImageLoaderPCX.cpp
	CImageLoaderPNG.cpp
	CImageLoaderPSD.cpp
	CImageLoaderPVR.cpp
	CImageLoaderTGA.cpp
	CImageLoaderPPM.cpp
	CImageLoaderWAL.cpp
	CImageLoaderRGB.cpp
	CImageWriterBMP.cpp
	CImageWriterJPG.cpp
	CImageWriterPCX.cpp
	CImageWriterPNG.cpp
	CImageWriterPPM.cpp
	CImageWriterPSD.cpp
	CImageWriterTGA.cpp
)

add_library(IRRVIDEOOBJ OBJECT
	CVideoModeList.cpp
	CFPSCounter.cpp
	${IRRDRVROBJ}
	${IRRIMAGEOBJ}
)

add_library(IRRSWRENDEROBJ OBJECT
	CSoftwareDriver.cpp
	CSoftwareTexture.cpp
	CTRFlat.cpp
	CTRFlatWire.cpp
	CTRGouraud.cpp
	CTRGouraudWire.cpp
	CTRNormalMap.cpp
	CTRStencilShadow.cpp
	CTRTextureFlat.cpp
	CTRTextureFlatWire.cpp
	CTRTextureGouraud.cpp
	CTRTextureGouraudAdd.cpp
	CTRTextureGouraudNoZ.cpp
	CTRTextureGouraudWire.cpp
	CZBuffer.cpp
	CTRTextureGouraudVertexAlpha2.cpp
	CTRTextureGouraudNoZ2.cpp
	CTRTextureLightMap2_M2.cpp
	CTRTextureLightMap2_M4.cpp
	CTRTextureLightMap2_M1.cpp
	CSoftwareDriver2.cpp
	CSoftwareTexture2.cpp
	CTRTextureGouraud2.cpp
	CTRGouraud2.cpp
	CTRGouraudAlpha2.cpp
	CTRGouraudAlphaNoZ2.cpp
	CTRTextureDetailMap2.cpp
	CTRTextureGouraudAdd2.cpp
	CTRTextureGouraudAddNoZ2.cpp
	CTRTextureWire2.cpp
	CTRTextureLightMap2_Add.cpp
	CTRTextureLightMapGouraud2_M4.cpp
	IBurningShader.cpp
	CTRTextureBlend.cpp
	CTRTextureGouraudAlpha.cpp
	CTRTextureGouraudAlphaNoZ.cpp
	CDepthBuffer.cpp
	CBurningShader_Raster_Reference.cpp
	CTR_transparent_reflection_2_layer.cpp
	CTRGouraudNoZ2.cpp
	burning_shader_color.cpp
)

add_library(IRRIOOBJ OBJECT
	CFileList.cpp
	CFileSystem.cpp
	CLimitReadFile.cpp
	CMemoryFile.cpp
	CReadFile.cpp
	CWriteFile.cpp
	CXMLReader.cpp
	CXMLWriter.cpp
	CWADReader.cpp
	CZipReader.cpp
	CPakReader.cpp
	CNPKReader.cpp
	CTarReader.cpp
	CMountPointReader.cpp
	irrXML.cpp
	CAttributes.cpp
)

add_library(IRROTHEROBJ OBJECT
	CIrrDeviceSDL.cpp
	CIrrDeviceLinux.cpp
	CIrrDeviceConsole.cpp
	CIrrDeviceStub.cpp
	CIrrDeviceWin32.cpp
	CIrrDeviceFB.cpp
	CLogger.cpp
	COSOperator.cpp
	Irrlicht.cpp
	os.cpp
	leakHunter.cpp
	CProfiler.cpp
	utf8.cpp
)

add_library(IRRGUIOBJ OBJECT
	CGUIButton.cpp
	CGUICheckBox.cpp
	CGUIComboBox.cpp
	CGUIContextMenu.cpp
	CGUIEditBox.cpp
	CGUIEnvironment.cpp
	CGUIFileOpenDialog.cpp
	CGUIFont.cpp
	CGUIImage.cpp
	CGUIInOutFader.cpp
	CGUIListBox.cpp
	CGUIMenu.cpp
	CGUIMeshViewer.cpp
	CGUIMessageBox.cpp
	CGUIModalScreen.cpp
	CGUIScrollBar.cpp
	CGUISpinBox.cpp
	CGUISkin.cpp
	CGUIStaticText.cpp
	CGUITabControl.cpp
	CGUITable.cpp
	CGUIToolBar.cpp
	CGUIWindow.cpp
	CGUIColorSelectDialog.cpp
	CDefaultGUIElementFactory.cpp
	CGUISpriteBank.cpp
	CGUIImageList.cpp
	CGUITreeView.cpp
	CGUIProfiler.cpp
)

# Library

option(BUILD_SHARED_LIBS "Build shared library" TRUE)

add_library(Irrlicht
	$<TARGET_OBJECTS:IRRMESHOBJ>
	$<TARGET_OBJECTS:IRROBJ>
	$<TARGET_OBJECTS:IRRPARTICLEOBJ>
	$<TARGET_OBJECTS:IRRANIMOBJ>
	$<TARGET_OBJECTS:IRRVIDEOOBJ>
	$<TARGET_OBJECTS:IRRSWRENDEROBJ>
	$<TARGET_OBJECTS:IRRIOOBJ>
	$<TARGET_OBJECTS:IRROTHEROBJ>
	$<TARGET_OBJECTS:IRRGUIOBJ>
)

set_target_properties(Irrlicht PROPERTIES
	VERSION ${VERSION}
	SOVERSION ${IRRLICHT_VERSION_MAJOR}.${IRRLICHT_VERSION_MINOR}
)
