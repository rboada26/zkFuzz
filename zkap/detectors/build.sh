mkdir build
cd build
# cmake -DENABLE_LLVM_SHARED=1 ..
cmake ..
make

ls

clang++ -shared -fPIC -o libDETECTORSPass.so \
    $(llvm-config --cxxflags) \
    $(llvm-config --ldflags) \
    $(llvm-config --libs) \
    -Wl,--no-undefined \
    -I /usr/include/llvm \
    -L /usr/include/llvm \
    ../PrintGraphInfo.cpp ../ProtocolFlowGraph.cpp ../utils_arrayshapes.cpp ../utils_basicinfo.cpp ../utils_funcs.cpp

# clang++ -shared -fPIC -o libDETECTORSPass.so -I /usr/include/llvm -L /usr/include/llvm -lLLVM ../PrintGraphInfo.cpp ../ProtocolFlowGraph.cpp ../utils_arrayshapes.cpp ../utils_basicinfo.cpp ../utils_funcs.cpp