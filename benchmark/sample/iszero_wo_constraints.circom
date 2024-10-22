pragma circom 2.0.0;

template IsZero() {
    signal input in;    // Input signal to check if it's zero or non-zero.
    signal output out;  // Output signal: 1 if `in == 0`, 0 if `in != 0`.
    signal inv;         // Inverse of the input when `in != 0`, or 0 when `in == 0`.
    
    inv <-- in!=0 ? 1/in : 0;

    out <-- -in*inv +1;
}

component main = IsZero();