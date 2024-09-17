pragma circom 2.0.0;

include "./matric.circom";

// Linear Layer
template Linear(nInputs, nOutputs, precision) {
    signal input x[nInputs];
    signal input weights[nInputs][nOutputs];
    signal input bias[nOutputs];
    signal input out[nOutputs];
    signal input remainder[nOutputs];

    component dot[nOutputs];

    for (var i = 0; i < nOutputs; i++) {
        assert (remainder[i] < precision);
        dot[i] = MatrixMul(1, nInputs, 1);

        for (var j = 0; j < nInputs; j++) {
            dot[i].a[0][j] <== x[j];
            dot[i].b[j][0] <== weights[j][i];
        }

        out[i] * n + remainder[i] === dot[i].out[0][0] + bias[i];
    }
}