cmake_minimum_required(VERSION 3.5)

project(tinygltf
  VERSION 2.6.3
  DESCRIPTION "Header only C++11 tiny glTF 2.0 library"
  LANGUAGES CXX
)

add_library(tinygltf INTERFACE)
add_library(tinygltf::tinygltf ALIAS tinygltf)

target_compile_definitions(tinygltf
  INTERFACE
    TINYGLTF_NO_EXTERNAL_IMAGE 
    TINYGLTF_NO_STB_IMAGE
    TINYGLTF_NO_STB_IMAGE_WRITE
)

target_include_directories(tinygltf
  INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
    "$<INSTALL_INTERFACE:./>"
)

include(GNUInstallDirs)

install(TARGETS tinygltf
  EXPORT tinygltf-export
  DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)

export(EXPORT tinygltf-export
  FILE "${CMAKE_BINARY_DIR}/cmake/tinygltfTargets.cmake"
  NAMESPACE tinygltf::
)

install(
  DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/tinygltf"
)

install(EXPORT tinygltf-export
  FILE tinygltfTargets.cmake
  NAMESPACE tinygltf::
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/tinygltf"
)

include(CMakePackageConfigHelpers)
configure_package_config_file("${PROJECT_SOURCE_DIR}/tinygltfConfig.cmake.in"
  "${CMAKE_BINARY_DIR}/cmake/tinygltfConfig.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/tinygltf"
  NO_SET_AND_CHECK_MACRO
  NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

write_basic_package_version_file(
  "${CMAKE_BINARY_DIR}/cmake/tinygltfConfigVersion.cmake"
  COMPATIBILITY AnyNewerVersion
)

install(
  FILES
    "${CMAKE_BINARY_DIR}/cmake/tinygltfConfig.cmake"
    "${CMAKE_BINARY_DIR}/cmake/tinygltfConfigVersion.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/tinygltf"
)
