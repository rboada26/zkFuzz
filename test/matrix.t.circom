pragma circom 2.0.0;

include "../src/circuits/matrix.circom";

template MatrixMulTest() {
    // Define matrix dimensions
    var m = 3;
    var n = 2;
    var p = 4;

    // Input matrices
    signal input a[3][2];
    signal input b[2][4];
    
    // Expected output
    signal input expected[3][4];
    
    // Actual output
    signal output actual[3][4];

    // Instantiate the MatrixMul component
    component matMul = MatrixMul(m, n, p);

    // Connect inputs
    for (var i = 0; i < m; i++) {
        for (var j = 0; j < n; j++) {
            matMul.a[i][j] <== a[i][j];
        }
    }
    for (var i = 0; i < n; i++) {
        for (var j = 0; j < p; j++) {
            matMul.b[i][j] <== b[i][j];
        }
    }

    // Connect outputs and verify
    for (var i = 0; i < m; i++) {
        for (var j = 0; j < p; j++) {
            actual[i][j] <== matMul.out[i][j];
            actual[i][j] === expected[i][j];
        }
    }
}

component main = MatrixMulTest();