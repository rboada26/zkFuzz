mkdir build

circom test/matrix.t.circom --r1cs --wasm --sym --c -o build
node build/matrix.t_js/generate_witness.js build/matrix.t_js/matrix.t.wasm test/matrix.t.json build/matrix.t.wtns