cmake_minimum_required(VERSION 3.5)

project(Irrlicht
	VERSION 1.9.0
	LANGUAGES CXX
)

message(STATUS "*** Building IrrlichtMt ${PROJECT_VERSION} ***")

if(ANDROID)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/Android)
elseif(APPLE)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/OSX)
elseif(WIN32)
	# good enough
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/Win32-gcc)
else()
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/Linux)
endif()
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type: Debug or Release" FORCE)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
add_subdirectory(source/Irrlicht)

# Installation of library and headers.
include(GNUInstallDirs)
install(TARGETS ${INSTALL_TARGETS}
	EXPORT IrrlichtMt-export
	DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)

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
configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/IrrlichtMtConfig.cmake"
	INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/IrrlichtMt"
	NO_SET_AND_CHECK_MACRO
	NO_CHECK_REQUIRED_COMPONENTS_MACRO
)
write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/IrrlichtMtConfigVersion.cmake"
	COMPATIBILITY AnyNewerVersion
)

install(FILES
	"${CMAKE_CURRENT_BINARY_DIR}/IrrlichtMtConfig.cmake"
	"${CMAKE_CURRENT_BINARY_DIR}/IrrlichtMtConfigVersion.cmake"
	DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/IrrlichtMt"
)
