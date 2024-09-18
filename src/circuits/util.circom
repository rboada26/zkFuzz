pragma circom 2.0.0;

include "../circomlib/sign.circom";
include "../circomlib/bitify.circom";
include "../circomlib/comparators.circom";
include "../circomlib/switcher.circom";

template isNegative() {
    signal input x;
    signal output out;

    component num2Bits = Num2Bits(254);
    num2Bits.in <== in;
    component sign = Sign();
    
    for (var i = 0; i < 254; i++) {
        sign.in[i] <== num2Bits.out[i];
    }

    out <== sign.sign;
}

template isPositive() {
    signal input x;
    signal output out;

    component num2Bits = Num2Bits(254);
    num2Bits.in <== in;
    component sign = Sign();
    
    for (var i = 0; i < 254; i++) {
        sign.in[i] <== num2Bits.out[i];
    }

    out <== 1 - sign.sign;
}

template Sum(nInputs) {
    signal input x[nInputs]:
    signal output out;

    signal partialSum[nInputs];
    partialSum[0] <== in[0];

    for (var i = 1; i < nInputs; i++) {
        partialSum[i] <== partialSum[i-1] + x[i];
    }

    out <== partialSum[nInputs-1];
}