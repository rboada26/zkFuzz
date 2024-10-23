mkdir build
cd build
# cmake -DENABLE_LLVM_SHARED=1 ..
cmake ..
make

ls

clang++ -shared -fPIC -o libProoFuzzPass.so \
    $(llvm-config --cxxflags) \
    $(llvm-config --ldflags) \
    $(llvm-config --libs) \
    -Wl,--no-undefined \
    -I /usr/include/llvm \
    -L /usr/include/llvm \
    ../MainAdderPass.cpp
