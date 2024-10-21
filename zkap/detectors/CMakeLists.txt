cmake_minimum_required(VERSION 3.10)
project(CRCPass)

set(CMAKE_CXX_STANDARD 14)

find_package(LLVM REQUIRED CONFIG)
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
