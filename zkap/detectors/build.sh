mkdir build
cd build
cmake ..
make

ls

# clang++ -shared -fPIC -o libdetectors.so -I /usr/include/llvm -L /usr/include/llvm -lLLVM ../All.cpp