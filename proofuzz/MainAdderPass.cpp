#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

using namespace llvm;

namespace {
  struct MainAdderPass : public ModulePass {
    static char ID;
    MainAdderPass() : ModulePass(ID) {}

    bool runOnModule(Module &M) override {
      // Step 1: Modify external global variable `@constraint` to initialize with `false`.
      GlobalVariable *constraint = M.getGlobalVariable("constraint");
      if (constraint && constraint->isDeclaration()) {
        // Make it internal and initialize with `false`
        constraint->setLinkage(GlobalValue::InternalLinkage);
        constraint->setInitializer(ConstantInt::get(Type::getInt1Ty(M.getContext()), 0));
      }

      // Step 2: Modify function `fn_template_init_SingleAssignment0` to include `@constraint` and add necessary operations.
      Function *fnInitSingleAssignment = M.getFunction("fn_template_init_SingleAssignment0");
      if (fnInitSingleAssignment) {
        modifyFnTemplateInit(fnInitSingleAssignment, M);
      }

      // Step 3: Create a `main` function that initializes an instance of the `SingleAssignment0` struct.
      createMainFunction(M);

      return true;
    }

    void modifyFnTemplateInit(Function *F, Module &M) {
      LLVMContext &Context = M.getContext();
      IRBuilder<> Builder(Context);

      // Traverse the function and locate the `body` block to insert additional logic
      for (auto &BB : *F) {
        if (BB.getName() == "body") {
          // Insert at the end of the body block
          Builder.SetInsertPoint(BB.getTerminator());

          // Load values from `%initial.out.output` and `%initial.b.input`
          Instruction *outOutput = findAlloca(F, "initial.out.output");
          Instruction *bInput = findAlloca(F, "initial.b.input");

          Value *readOutOutput = Builder.CreateLoad(Builder.getInt128Ty(), outOutput, "read.out.output");
          Value *readBInput2 = Builder.CreateLoad(Builder.getInt128Ty(), bInput, "read.b.input2");

          // Add 1 to readBInput2 (mimicking `%add3`)
          Value *add3 = Builder.CreateAdd(readBInput2, ConstantInt::get(Builder.getInt128Ty(), 1), "add3");

          // Call `fn_intrinsic_utils_constraint`
          Function *constraintFunc = M.getFunction("fn_intrinsic_utils_constraint");
          GlobalVariable *constraintVar = M.getGlobalVariable("constraint");
          Builder.CreateCall(constraintFunc, {readOutOutput, add3, constraintVar});

          break;
        }
      }
    }

    // Helper function to find alloca instruction by its name
    Instruction* findAlloca(Function *F, StringRef name) {
      for (auto &BB : *F) {
        for (auto &I : BB) {
          if (AllocaInst *AI = dyn_cast<AllocaInst>(&I)) {
            if (AI->getName() == name) {
              return AI;
            }
          }
        }
      }
      return nullptr;
    }

    void createMainFunction(Module &M) {
      LLVMContext &Context = M.getContext();
      IRBuilder<> Builder(Context);

      // Define the `main` function type and create the function
      FunctionType *mainType = FunctionType::get(Builder.getInt32Ty(), false);
      Function *mainFunc = Function::Create(mainType, Function::ExternalLinkage, "main", M);

      // Create the basic block for the `main` function
      BasicBlock *entry = BasicBlock::Create(Context, "entry", mainFunc);
      Builder.SetInsertPoint(entry);

      // Call `fn_template_build_SingleAssignment0`
      Function *fnBuildSingleAssignment = M.getFunction("fn_template_build_SingleAssignment0");
      Value *instance = Builder.CreateCall(fnBuildSingleAssignment, {}, "instance");

      // Get pointers to `a.input` and `b.input` and store values
      Value *aInputPtr = getGEP(Context, Builder, instance, 0, "a.input");
      Builder.CreateStore(ConstantInt::get(Builder.getInt128Ty(), 5), aInputPtr);

      Value *bInputPtr = getGEP(Context, Builder, instance, 1, "b.input");
      Builder.CreateStore(ConstantInt::get(Builder.getInt128Ty(), 7), bInputPtr);

      // Call `fn_template_init_SingleAssignment0`
      Function *fnInitSingleAssignment = M.getFunction("fn_template_init_SingleAssignment0");
      Builder.CreateCall(fnInitSingleAssignment, {instance});

      // Load the value of `constraint` and zero-extend it to i32
      GlobalVariable *constraintVar = M.getGlobalVariable("constraint");
      Value *constraintVal = Builder.CreateLoad(Builder.getInt1Ty(), constraintVar, "constraint_val");
      Value *constraintI32 = Builder.CreateZExt(constraintVal, Builder.getInt32Ty(), "constraint_i32");

      // Return 0 or 1 based on the constraint value
      Builder.CreateRet(constraintI32);

      // Finalize the function by setting up the entry point
      Builder.SetInsertPoint(&entry->back());
    }

    Value* getGEP(LLVMContext &Context, IRBuilder<> &Builder, Value *instance, unsigned index, const char *name) {
      return Builder.CreateGEP(instance->getType()->getPointerElementType(), instance, 
                               {Builder.getInt32(0), Builder.getInt32(index)}, name);
    }
  };
}

char MainAdderPass::ID = 0;
static RegisterPass<MainAdderPass> X("CircomMainAddr", "Circom Transformation Pass", false, false);
