#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

using namespace llvm;

namespace
{
    struct InitializeConstraintPass : public ModulePass
    {
        static char ID;
        InitializeConstraintPass() : ModulePass(ID) {}

        bool runOnModule(Module &M) override
        {
            // Modify external global variable `@constraint` to initialize with `false`.
            for (GlobalVariable &GV : M.globals())
            {
                if (GV.getName().contains("constraint") && GV.isDeclaration())
                {
                    GV.setLinkage(GlobalValue::InternalLinkage);
                    GV.setInitializer(ConstantInt::get(Type::getInt1Ty(M.getContext()), 0));
                }
            }

            return true;
        }
    };
}

char InitializeConstraintPass::ID = 0;
static RegisterPass<InitializeConstraintPass> X("InitializeConstraintPass", "Circom Transformation Pass", false, false);
