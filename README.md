# ProoFuzz

- [Doc](./doc/)
- [Meeting Notes](./NOTE.md)

## Build

- circom2llvm

```bash
cargo build --bin=circom2llvm --package=circom2llvm --release
# sudo cp ./target/release/circom2llvm /usr/local/bin/circom2llvm
```

## Example

```bash
circom2llvm --input path/to/circomfile_or_dir --output path/to/output
```