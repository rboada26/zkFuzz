cmake_minimum_required(VERSION 3.10)
project(CRCPass)

set(CMAKE_CXX_STANDARD 14)

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_library(detectors MODULE
    utils_arrayshapes.cpp
    utils_basicinfo.cpp
    utils.cpp
    ProtocolFlowGraph.cpp
    All.cpp
    PrintGraphInfo.cpp
    PrintGraphviz.cpp
)

# Get proper shared-library behavior on all systems
if(APPLE)
    set_target_properties(detectors PROPERTIES
        LINK_FLAGS "-undefined dynamic_lookup"
    )
endif(APPLE)

target_link_libraries(detectors ${llvm_libs})
