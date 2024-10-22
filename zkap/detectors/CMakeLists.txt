cmake_minimum_required(VERSION 3.10)
project(CDGPass)

set(CMAKE_CXX_STANDARD 14)

find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_library(CDGPass MODULE
    utils_arrayshapes.cpp
    utils_basicinfo.cpp
    utils.cpp
    ProtocolFlowGraph.cpp
    PrintGraphInfo.cpp
    All.cpp
    PrintGraphviz.cpp
)

target_link_libraries(CDGPass PRIVATE LLVMCore LLVMSupport LLVMIRReader LLVMInstCombine LLVMScalarOpts)

