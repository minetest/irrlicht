get_filename_component(IrrlichtMt_CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}" PATH)
include(CMakeFindDependencyMacro)

find_dependency(ZLIB REQUIRED)

if(NOT TARGET irr::IrrlichtMt)
    include("${IrrlichtMt_CMAKE_DIR}/irrlichtmt/irrlichtmtTargets.cmake")
endif()
