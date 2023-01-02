option(BUILD_SHARED_LIBS "Build shared library" TRUE)

# Compiler flags

add_definitions(-DIRRLICHT_EXPORTS)
if(NOT BUILD_SHARED_LIBS)
	add_definitions(-D_IRR_STATIC_LIB_)
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_definitions(-D_DEBUG)
endif()
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3")
	set(CMAKE_CXX_FLAGS_DEBUG "-g")

	add_compile_options(-Wall -pipe -fno-exceptions -fno-rtti)

	# Enable SSE for floating point math on 32-bit x86 by default
	# reasoning see minetest issue #11810 and https://gcc.gnu.org/wiki/FloatingPointMath
	if(CMAKE_SIZEOF_VOID_P EQUAL 4)
		include(CheckCXXSourceCompiles)
		check_cxx_source_compiles("#ifndef __i686__\n#error\n#endif\nint main(){}" IS_I686)
		if(IS_I686)
			message(STATUS "Detected Intel x86: using SSE instead of x87 FPU")
			add_compile_options(-mfpmath=sse -msse)
		endif()
	endif()

elseif(MSVC)
	string(APPEND CMAKE_CXX_STANDARD_LIBRARIES " msvcrt.lib") # ???? fuck off

	add_compile_options(/GR- /Zl)

	# Enable SSE for floating point math on 32-bit x86 by default
	# reasoning see minetest issue #11810 and https://gcc.gnu.org/wiki/FloatingPointMath
	if(CMAKE_SIZEOF_VOID_P EQUAL 4)
		add_compile_options(/arch:SSE)
	endif()
endif()

# Sanity-check version

include(CheckCXXSourceCompiles)
set(CMAKE_REQUIRED_INCLUDES ${PROJECT_SOURCE_DIR}/include)
unset(REVISION_SANITY_CHECK CACHE)
check_cxx_source_compiles("#include <IrrCompileConfig.h>\n\
#if IRRLICHT_VERSION_MT_REVISION != ${IRRLICHTMT_REVISION}\n\
#error\n\
#endif\n\
int main() {}" REVISION_SANITY_CHECK)
if(NOT REVISION_SANITY_CHECK)
	message(FATAL_ERROR "IrrlichtMt revision number mismatches between CMake and headers.")
endif()

# Required libs

find_package(ZLIB REQUIRED)
find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)

# To configure the features available in this Irrlicht build please edit include/IrrCompileConfig.h.
include(CheckSymbolExists)
set(CMAKE_REQUIRED_INCLUDES ${PROJECT_SOURCE_DIR}/include)
unset(OGLES1_ENABLED CACHE)
unset(OGLES2_ENABLED CACHE)
unset(OGL_ENABLED CACHE)
unset(XINPUT2_ENABLED CACHE)
unset(SDL_ENABLED CACHE)

# tell cmake about the dependency
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${CMAKE_REQUIRED_INCLUDES}/IrrCompileConfig.h)

check_symbol_exists(_IRR_COMPILE_WITH_OGLES1_ "IrrCompileConfig.h" OGLES1_ENABLED)
if(OGLES1_ENABLED)
	# only tested on Android, probably works on Linux (is this needed anywhere else?)
	find_library(OPENGLES_LIBRARY NAMES GLESv1_CM REQUIRED)
	find_library(EGL_LIBRARY NAMES EGL REQUIRED)

	message(STATUS "Found OpenGLES: ${OPENGLES_LIBRARY}")
endif()
check_symbol_exists(_IRR_COMPILE_WITH_OGLES2_ "IrrCompileConfig.h" OGLES2_ENABLED)
if(OGLES2_ENABLED)
	find_package(OpenGLES2 REQUIRED)
endif()
check_symbol_exists(_IRR_COMPILE_WITH_OPENGL_ "IrrCompileConfig.h" OGL_ENABLED)
if(OGL_ENABLED)
	set(OpenGL_GL_PREFERENCE "LEGACY")
	find_package(OpenGL REQUIRED)
endif()
if(UNIX AND NOT ANDROID AND NOT APPLE)
	check_symbol_exists(_IRR_LINUX_X11_XINPUT2_ "IrrCompileConfig.h" XINPUT2_ENABLED)
endif()
check_symbol_exists(_IRR_COMPILE_WITH_SDL_DEVICE_ "IrrCompileConfig.h" SDL_ENABLED)
if(SDL_ENABLED)
	find_package(SDL2 CONFIG REQUIRED)
	message(STATUS "Found SDL2: ${SDL2_LIBRARIES}")
endif()

# Platform-specific libs

if(ANDROID)
	enable_language(C)
	add_library(native_app_glue STATIC ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
elseif(APPLE)
	find_library(COCOA_LIB Cocoa REQUIRED)
	find_library(IOKIT_LIB IOKit REQUIRED)

	add_definitions(-DGL_SILENCE_DEPRECATION)
else()
	# Unix probably
	find_package(X11 REQUIRED)
	if(XINPUT2_ENABLED AND NOT X11_Xi_FOUND)
		message(FATAL_ERROR "XInput not found")
	endif()
endif()

set(link_includes
	"${PROJECT_SOURCE_DIR}/include"
	"${CMAKE_CURRENT_SOURCE_DIR}"

	"${ZLIB_INCLUDE_DIR}"
	"${JPEG_INCLUDE_DIR}"
	"${PNG_INCLUDE_DIR}"
	"${SDL2_INCLUDE_DIRS}"

	${OPENGL_INCLUDE_DIR}
	${OPENGLES2_INCLUDE_DIR}
	${EGL_INCLUDE_DIR}

	"$<$<PLATFORM_ID:Android>:${ANDROID_NDK}/sources/android/native_app_glue>"
	${X11_INCLUDE_DIR}
)

set(link_libs
	"${ZLIB_LIBRARY}"
	"${JPEG_LIBRARY}"
	"${PNG_LIBRARY}"
	"${SDL2_LIBRARIES}"

	${OPENGL_LIBRARIES}
	${OPENGLES_LIBRARY}
	${OPENGLES2_LIBRARIES}
	${EGL_LIBRARY}

	"$<$<PLATFORM_ID:Android>:native_app_glue -landroid -llog>"
	${COCOA_LIB}
	${IOKIT_LIB}
	"$<$<PLATFORM_ID:Windows>:gdi32>"
	"$<$<PLATFORM_ID:Windows>:winmm>"
	${X11_X11_LIB}
	${X11_Xi_LIB}
)

# Source files

set(IRRMESHLOADER
	CB3DMeshFileLoader.cpp
	COBJMeshFileLoader.cpp
	CXMeshFileLoader.cpp
)

add_library(IRRMESHOBJ OBJECT
	CSkinnedMesh.cpp
	CBoneSceneNode.cpp
	CMeshSceneNode.cpp
	CAnimatedMeshSceneNode.cpp
	${IRRMESHLOADER}
)

add_library(IRROBJ OBJECT
	CBillboardSceneNode.cpp
	CCameraSceneNode.cpp
	CDummyTransformationSceneNode.cpp
	CEmptySceneNode.cpp
	CMeshManipulator.cpp
	CSceneCollisionManager.cpp
	CSceneManager.cpp
	CMeshCache.cpp
)

set(IRRDRVROBJ
	CNullDriver.cpp
	COpenGLCacheHandler.cpp
	COpenGLDriver.cpp
	COpenGLShaderMaterialRenderer.cpp
	COpenGLSLMaterialRenderer.cpp
	COpenGLExtensionHandler.cpp
	COGLESDriver.cpp
	COGLESExtensionHandler.cpp
	COGLES2Driver.cpp
	COGLES2ExtensionHandler.cpp
	COGLES2FixedPipelineRenderer.cpp
	COGLES2MaterialRenderer.cpp
	COGLES2Renderer2D.cpp
	CWebGL1Driver.cpp
	CGLXManager.cpp
	CWGLManager.cpp
	CEGLManager.cpp
	CSDLManager.cpp
	mt_opengl_loader.cpp
)

set(IRRIMAGEOBJ
	CColorConverter.cpp
	CImage.cpp
	CImageLoaderBMP.cpp
	CImageLoaderJPG.cpp
	CImageLoaderPNG.cpp
	CImageLoaderTGA.cpp
	CImageWriterJPG.cpp
	CImageWriterPNG.cpp
)

add_library(IRRVIDEOOBJ OBJECT
	CFPSCounter.cpp
	${IRRDRVROBJ}
	${IRRIMAGEOBJ}
)

add_library(IRRIOOBJ OBJECT
	CFileList.cpp
	CFileSystem.cpp
	CLimitReadFile.cpp
	CMemoryFile.cpp
	CReadFile.cpp
	CWriteFile.cpp
	CZipReader.cpp
	CAttributes.cpp
)

add_library(IRROTHEROBJ OBJECT
	CIrrDeviceSDL.cpp
	CIrrDeviceLinux.cpp
	CIrrDeviceStub.cpp
	CIrrDeviceWin32.cpp
	CLogger.cpp
	COSOperator.cpp
	Irrlicht.cpp
	os.cpp
)

if(ANDROID)
	target_sources(IRROTHEROBJ PRIVATE
		Android/CIrrDeviceAndroid.cpp
		Android/CAndroidAssetReader.cpp
		Android/CAndroidAssetFileArchive.cpp
		Android/CKeyEventWrapper.cpp
	)
elseif(APPLE)
	# Build all IRROTHEROBJ sources as objc++, including the .cpp's
	set_target_properties(IRROTHEROBJ PROPERTIES COMPILE_OPTIONS "-xobjective-c++")
	target_sources(IRROTHEROBJ PRIVATE
		CIrrDeviceOSX.mm
		CNSOGLManager.mm
	)
endif()

add_library(IRRGUIOBJ OBJECT
	CGUIButton.cpp
	CGUICheckBox.cpp
	CGUIComboBox.cpp
	CGUIEditBox.cpp
	CGUIEnvironment.cpp
	CGUIFileOpenDialog.cpp
	CGUIFont.cpp
	CGUIImage.cpp
	CGUIListBox.cpp
	CGUIScrollBar.cpp
	CGUISkin.cpp
	CGUIStaticText.cpp
	CGUITabControl.cpp
	CGUISpriteBank.cpp
	CGUIImageList.cpp
)

# Library

add_library(IrrlichtMt)
foreach(object_lib
	IRRMESHOBJ IRROBJ IRRVIDEOOBJ
	IRRIOOBJ IRROTHEROBJ IRRGUIOBJ)
	# Set include directories for object library compilation
	target_include_directories(${object_lib} PRIVATE ${link_includes})
	# Add objects from object library to main library
	target_sources(IrrlichtMt PRIVATE $<TARGET_OBJECTS:${object_lib}>)
endforeach()

# Alias target provides add_submodule compatibility
add_library(IrrlichtMt::IrrlichtMt ALIAS IrrlichtMt)

target_include_directories(IrrlichtMt
	PUBLIC
		"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/>"
		"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
		"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/irrlichtmt>"
	PRIVATE
		${link_includes}
)

target_link_libraries(IrrlichtMt PRIVATE ${link_libs})

# Propagate static library flag to lib users, only needed for Windows
if(NOT BUILD_SHARED_LIBS)
	target_compile_definitions(IrrlichtMt INTERFACE _IRR_STATIC_LIB_)
endif()

set_target_properties(IrrlichtMt PROPERTIES
	VERSION ${PROJECT_VERSION}
)

if(WIN32)
	set_target_properties(IrrlichtMt PROPERTIES PREFIX "") # for DLL name
endif()

# Installation of library
if(ANDROID)
	set(INSTALL_TARGETS IrrlichtMt native_app_glue)
else()
	set(INSTALL_TARGETS IrrlichtMt)
endif()

install(TARGETS ${INSTALL_TARGETS}
	EXPORT IrrlichtMt-export
	DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)
