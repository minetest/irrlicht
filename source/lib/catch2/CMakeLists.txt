cmake_minimum_required(VERSION 3.5)

project(Catch2
  LANGUAGES CXX
)

add_library(Catch2 INTERFACE)
add_library(Catch2::Catch ALIAS Catch2)

target_include_directories(Catch2
  INTERFACE
    "${PROJECT_SOURCE_DIR}"
)
