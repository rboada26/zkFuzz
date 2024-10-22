# ZKAP

## Build

```bash
$  cd detectors
$  sh ./build.sh
```

## Example

```bash
$  opt -enable-new-pm=0 -load ./build/libDETECTORSPass.so --PrintGraphInfo -S ../../benchmark/sample/iszero_safe.ll -o /dev/null
{"IsZero":{"graph":{"Edges":["out --> mul6","add --> out","in --> mul6","mul6 === 0","out === add","in --> utils_switch","inv --> add","utils_switch --> inv","in --> add"],"Nodes":["utils_switch; inst:  %utils_switch = call i128 @fn_intrinsic_utils_switch(i1 %ne, i128 %sdiv, i128 0)","add; inst:  %add = add i128 %mul, 1","mul6; inst:  %mul6 = mul i128 %read.in.input4, %read.out.output5","out","inv","in"]},"info":{"Component Input Signals":"{}","Component Output Signals":"{}","Input Signals":"{in, }","Inter Signals":"{inv, }","Output Signals":"{out, }","Variables":"{}"},"reports":{}}}
```