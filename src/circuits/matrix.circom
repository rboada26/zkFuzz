pragma circom 2.0.0;

// element-wise matrix multiplication
template MatrixElementwiseMul (m, n) {
    signal input a[m][n];
    signal input b[m][n];
    signal output out[m][n];

    for (var i = 0; i < m; i++) {
        for (var j = 0; j < n; j++) {
            out[i][j] <== a[i][j] * b[i][j];
        }
    }
}

// sum of all elements in a matrix
template MatrixSum (m, n) {
    signal input a[m][n];
    signal output out;

    signal sum[m*n];
    sum[0] <== a[0][0];
    
    var idx = 0;
    for (var i = 0; i < m; i++) {
        for (var j = 0; j < n; j++) {
            if (idx > 0) {
                sum[idx] <== sum[idx-1] + a[i][j];
            }
            idx++;
        }
    }

    out <== sum[m*n-1];
}

// matrix multiplication
template MatrixMul (m,n,p) {
    signal input a[m][n];
    signal input b[n][p];
    signal output out[m][p];

    component mem[m][p];
    component mes[m][p];

    for (var i=0; i < m; i++) {
        for (var j=0; j < p; j++) {
            mem[i][j] = MatrixElementwiseMul(1, n);
            for (var k=0; k<n; k++) {
                mem[i][j].a[0][k] <== a[i][k];
                mem[i][j].b[0][k] <== b[k][j];
            }

            mes[i][j] = MatrixSum(1, n);
            mes[i][j].a[0] <== mem[i][j].out[0];
            out[i][j] <== mes[i][j].out;
        }
    }
}