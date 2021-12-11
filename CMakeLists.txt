cmake_minimum_required(VERSION 3.5)

# Set policies up to 3.9 since we want to enable the IPO option
if(${CMAKE_VERSION} VERSION_LESS 3.9)
	cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
	cmake_policy(VERSION 3.9)
endif()

project(Irrlicht
	VERSION 1.9.0
	LANGUAGES CXX
)

message(STATUS "*** Building IrrlichtMt ${PROJECT_VERSION} ***")

include(GNUInstallDirs)

if(ANDROID)
	set(sysname Android)
elseif(APPLE)
	set(sysname OSX)
elseif(MSVC)
	set(sysname Win32-VisualStudio)
elseif(WIN32)
	set(sysname Win32-gcc)
else()
	set(sysname Linux)
endif()
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib/${sysname})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin/${sysname})

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type: Debug or Release" FORCE)
endif()

# use SSE for floating-point operations to avoid issues with improper fp-rounding and loss of precision
# when moving fp-data to incompatible or less-precise registers/storage locations
# see https://gcc.gnu.org/wiki/FloatingPointMath and https://gcc.gnu.org/wiki/x87note
add_compile_options(-mfpmath=sse -msse2)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
add_subdirectory(source/Irrlicht)

option(BUILD_EXAMPLES "Build example applications" FALSE)
if(BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()

# Export a file that describes the targets that IrrlichtMt creates.
# The file is placed in the location FILE points to, where CMake can easily
# locate it by pointing CMAKE_PREFIX_PATH to this project root.
export(EXPORT IrrlichtMt-export
	FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/IrrlichtMtTargets.cmake"
	NAMESPACE IrrlichtMt::
)

# Installation of headers.
install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/"
	DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/irrlichtmt"
)

# Installation of CMake target and configuration files.
install(EXPORT IrrlichtMt-export
	FILE IrrlichtMtTargets.cmake
	NAMESPACE IrrlichtMt::
	DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/IrrlichtMt"
)

include(CMakePackageConfigHelpers)
configure_package_config_file("${PROJECT_SOURCE_DIR}/Config.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/cmake/IrrlichtMtConfig.cmake"
	INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/IrrlichtMt"
	NO_SET_AND_CHECK_MACRO
	NO_CHECK_REQUIRED_COMPONENTS_MACRO
)
write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/cmake/IrrlichtMtConfigVersion.cmake"
	COMPATIBILITY AnyNewerVersion
)

install(FILES
	"${CMAKE_CURRENT_BINARY_DIR}/cmake/IrrlichtMtConfig.cmake"
	"${CMAKE_CURRENT_BINARY_DIR}/cmake/IrrlichtMtConfigVersion.cmake"
	DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/IrrlichtMt"
)
