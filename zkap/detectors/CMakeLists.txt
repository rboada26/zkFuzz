cmake_minimum_required(VERSION 3.10)
project(DETECTORPass)

set(CMAKE_CXX_STANDARD 14)

find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_library(DETECTORSPass MODULE PrintGraphInfo.cpp ProtocolFlowGraph.cpp utils_arrayshapes.cpp utils_basicinfo.cpp utils_funcs.cpp)

#add_library(detectors MODULE
#    #utils_arrayshapes.cpp
#    #utils_basicinfo.cpp
#    #utils_funcs.cpp
#    #ProtocolFlowGraph.cpp
#   PrintGraphInfo.cpp
#    #All.cpp
#    #PrintGraphviz.cpp
#)

# Get proper shared-library behavior on all systems
#if(APPLE)
#    set_target_properties(detectors PROPERTIES
#        LINK_FLAGS "-undefined dynamic_lookup"
#    )
#endif(APPLE)

#target_compile_features(DETECTORSPass PRIVATE cxx_std_14)
#target_compile_options(DETECTORSPass PRIVATE -fno-rtti)

target_link_libraries(DETECTORSPass PRIVATE LLVMCore LLVMSupport LLVMIRReader LLVMInstCombine LLVMScalarOpts)
#target_link_libraries(DETECTORSPass PRIVATE ${LLVM_AVAILABLE_LIBS})

# target_compile_options(detectors PRIVATE -fno-rtti)
# target_link_libraries(DETECTORSPass PRIVATE LLVMCore LLVMSupport LLVMIRReader LLVMInstCombine LLVMScalarOpts)
# target_link_libraries(DETECTORSPass ${llvm_libs})
