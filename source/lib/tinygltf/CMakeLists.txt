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
    "${CMAKE_CURRENT_SOURCE_DIR}"
)
