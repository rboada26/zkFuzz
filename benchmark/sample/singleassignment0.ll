; ModuleID = 'singleassignment0.circom'
source_filename = "../../benchmark/sample/singleassignment0.circom"

%struct_template_SingleAssignment0 = type { i128, i128, i128 }

; @constraint = external global i1
@constraint = global i1 false

define void @fn_intrinsic_utils_constraint(i128 %0, i128 %1, i1* %2) {
entry:
  %constraint = icmp eq i128 %0, %1
  store i1 %constraint, i1* %2, align 1
  ret void
}

define void @fn_intrinsic_utils_constraint_array([256 x i128]* %0, [256 x i128]* %1, i1* %2) {
entry:
  ret void
}

define i128 @fn_intrinsic_utils_switch(i1 %0, i128 %1, i128 %2) {
entry:
  br i1 %0, label %if.true, label %if.false

if.true:                                          ; preds = %entry
  ret i128 %1

if.false:                                         ; preds = %entry
  ret i128 %2
}

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare fp128 @llvm.powi.f128.i32(fp128, i32) #0

define i128 @fn_intrinsic_utils_powi(i128 %0, i128 %1) {
entry:
  %utils_powi.base = uitofp i128 %0 to fp128
  %utils_powi.power = trunc i128 %1 to i32
  %utils_powi.cal = call fp128 @llvm.powi.f128.i32(fp128 %utils_powi.base, i32 %utils_powi.power)
  %utils_powi.ret = fptoui fp128 %utils_powi.cal to i128
  ret i128 %utils_powi.ret
}

define i128 @fn_intrinsic_utils_init() {
entry:
  ret i128 0
}

define void @fn_intrinsic_utils_assert(i1 %0) {
entry:
  ret void
}

define void @fn_intrinsic_utils_arraydim(i128* %0, ...) {
entry:
  ret void
}

define %struct_template_SingleAssignment0* @fn_template_build_SingleAssignment0() {
entry:
  %malloccall = tail call i8* @malloc(i32 ptrtoint (%struct_template_SingleAssignment0* getelementptr (%struct_template_SingleAssignment0, %struct_template_SingleAssignment0* null, i32 1) to i32))
  %struct_template_SingleAssignment0 = bitcast i8* %malloccall to %struct_template_SingleAssignment0*
  ret %struct_template_SingleAssignment0* %struct_template_SingleAssignment0
}

declare noalias i8* @malloc(i32)

define void @fn_template_init_SingleAssignment0(%struct_template_SingleAssignment0* %0) {
entry:
  %initial.out.output = alloca i128, align 8
  %initial.a.input = alloca i128, align 8
  %"gep.SingleAssignment0|a.input" = getelementptr inbounds %struct_template_SingleAssignment0, %struct_template_SingleAssignment0* %0, i32 0, i32 0
  %read.a.input = load i128, i128* %"gep.SingleAssignment0|a.input", align 4
  store i128 %read.a.input, i128* %initial.a.input, align 4
  %initial.b.input = alloca i128, align 8
  %"gep.SingleAssignment0|b.input" = getelementptr inbounds %struct_template_SingleAssignment0, %struct_template_SingleAssignment0* %0, i32 0, i32 1
  %read.b.input = load i128, i128* %"gep.SingleAssignment0|b.input", align 4
  store i128 %read.b.input, i128* %initial.b.input, align 4
  br label %body

body:                                             ; preds = %entry
  %read.a.input1 = load i128, i128* %initial.a.input, align 4
  %add = add i128 %read.a.input1, 1
  store i128 %add, i128* %initial.out.output, align 4
  %read.out.output = load i128, i128* %initial.out.output, align 4
  %read.b.input2 = load i128, i128* %initial.b.input, align 4
  %add3 = add i128 %read.b.input2, 1
  call void @fn_intrinsic_utils_constraint(i128 %read.out.output, i128 %add3, i1* @constraint)
  br label %exit

exit:                                             ; preds = %body
  %read.out.output4 = load i128, i128* %initial.out.output, align 4
  %"gep.SingleAssignment0|out.output" = getelementptr inbounds %struct_template_SingleAssignment0, %struct_template_SingleAssignment0* %0, i32 0, i32 2
  store i128 %read.out.output4, i128* %"gep.SingleAssignment0|out.output", align 4
  ret void
}

attributes #0 = { nofree nosync nounwind readnone speculatable willreturn }

define i32 @main() {
entry:
  ; Create an instance of the SingleAssignment0 struct
  %instance = call %struct_template_SingleAssignment0* @fn_template_build_SingleAssignment0()

  ; Get pointer to a.input and store value
  %"gep.SingleAssignment0|a.input" = getelementptr inbounds %struct_template_SingleAssignment0, %struct_template_SingleAssignment0* %instance, i32 0, i32 0
  store i128 5, i128* %"gep.SingleAssignment0|a.input", align 4  ; Set 'a' = 5

  ; Get pointer to b.input and store value
  %"gep.SingleAssignment0|b.input" = getelementptr inbounds %struct_template_SingleAssignment0, %struct_template_SingleAssignment0* %instance, i32 0, i32 1
  store i128 7, i128* %"gep.SingleAssignment0|b.input", align 4  ; Set 'b' = 7

  ; Initialize the instance
  call void @fn_template_init_SingleAssignment0(%struct_template_SingleAssignment0* %instance)

  ; Call the print function to display the result
  ; call void @print_constraint()

  ; Load the value of the constraint
  %constraint_val = load i1, i1* @constraint

  ; Zero-extend the i1 value to an i32 (since main returns i32)
  %constraint_i32 = zext i1 %constraint_val to i32

  ; Return 0 for successful execution
  ret i32 12
}
