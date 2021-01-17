include_directories(${CMAKE_SOURCE_DIR}/include)
add_definitions(-DIRRLICHT_EXPORTS=1)
add_compile_options(-Wall -pipe -fno-exceptions -fno-rtti -fstrict-aliasing)
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

add_library(MESHLOADER OBJECT
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
	)

add_library(MESHWRITER OBJECT
	CColladaMeshWriter.cpp
	CIrrMeshWriter.cpp
	CSTLMeshWriter.cpp
	COBJMeshWriter.cpp
	CPLYMeshWriter.cpp
	)

add_library(MESHOBJ OBJECT
	CSkinnedMesh.cpp
	CBoneSceneNode.cpp
	CMeshSceneNode.cpp
	CAnimatedMeshSceneNode.cpp
	CAnimatedMeshMD2.cpp
	CAnimatedMeshMD3.cpp
	CQ3LevelMesh.cpp
	CQuake3ShaderSceneNode.cpp
	CAnimatedMeshHalfLife.cpp
	)

add_library(SCENEOBJ OBJECT
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

add_library(PARTICLEOBJ OBJECT
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

add_library(ANIMOBJ OBJECT
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

add_library(DRVROBJ OBJECT
	CNullDriver.cpp
	COpenGLDriver.cpp
	COpenGLNormalMapRenderer.cpp
	COpenGLParallaxMapRenderer.cpp
	COpenGLShaderMaterialRenderer.cpp
	COpenGLTexture.cpp
	COpenGLSLMaterialRenderer.cpp
	COpenGLExtensionHandler.cpp
	CD3D8Driver.cpp
	CD3D8NormalMapRenderer.cpp
	CD3D8ParallaxMapRenderer.cpp
	CD3D8ShaderMaterialRenderer.cpp
	CD3D8Texture.cpp
	CD3D9Driver.cpp
	CD3D9HLSLMaterialRenderer.cpp
	CD3D9NormalMapRenderer.cpp
	CD3D9ParallaxMapRenderer.cpp
	CD3D9ShaderMaterialRenderer.cpp
	CD3D9Texture.cpp
	)

add_library(IMAGEOBJ OBJECT
	CColorConverter.cpp
	CImage.cpp
	CImageLoaderBMP.cpp
	CImageLoaderDDS.cpp
	CImageLoaderJPG.cpp
	CImageLoaderPCX.cpp
	CImageLoaderPNG.cpp
	CImageLoaderPSD.cpp
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

add_library(VIDEOOBJ OBJECT
	CVideoModeList.cpp
	CFPSCounter.cpp
	)

add_library(SWRENDEROBJ OBJECT
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
	)

add_library(IOOBJ OBJECT
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
	lzma/LzmaDec.c
	)

add_library(GUIOBJ OBJECT
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
	)

add_library(AESGM STATIC
	aesGladman/aescrypt.cpp
	aesGladman/aeskey.cpp
	aesGladman/aestab.cpp
	aesGladman/fileenc.cpp
	aesGladman/hmac.cpp
	aesGladman/prng.cpp
	aesGladman/pwd2key.cpp
	aesGladman/sha1.cpp
	aesGladman/sha2.cpp
	)

add_library(Irrlicht SHARED
	CIrrDeviceSDL2.cpp
	CIrrDeviceStub.cpp
	CLogger.cpp
	COSOperator.cpp
	Irrlicht.cpp
	os.cpp
	)

target_sources(Irrlicht PRIVATE
	$<TARGET_OBJECTS:MESHOBJ>
		$<TARGET_OBJECTS:MESHLOADER>
		$<TARGET_OBJECTS:MESHWRITER>
	$<TARGET_OBJECTS:SCENEOBJ>
	$<TARGET_OBJECTS:PARTICLEOBJ>
	$<TARGET_OBJECTS:ANIMOBJ>
	$<TARGET_OBJECTS:VIDEOOBJ>
		$<TARGET_OBJECTS:DRVROBJ>
		$<TARGET_OBJECTS:IMAGEOBJ>
	$<TARGET_OBJECTS:SWRENDEROBJ>
	$<TARGET_OBJECTS:IOOBJ>
	$<TARGET_OBJECTS:GUIOBJ>
	)

target_link_libraries(Irrlicht PRIVATE
	AESGM
	z
	jpeg
	png
	bz2
	SDL2
)

set(VERSION "${IRRLICHT_VERSION_MAJOR}.${IRRLICHT_VERSION_MINOR}.${IRRLICHT_VERSION_RELEASE}")
set_target_properties(Irrlicht PROPERTIES
	VERSION ${VERSION}
	SOVERSION ${IRRLICHT_VERSION_MAJOR}.${IRRLICHT_VERSION_MINOR}
)
