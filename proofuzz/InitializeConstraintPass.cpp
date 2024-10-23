#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

using namespace llvm;

namespace
{
    /**
     * @brief LLVM ModulePass that initializes the external global variable `@constraint`
     * to `false`. This pass looks for a global variable named `constraint`, which is
     * assumed to be a boolean (i1 type), and sets its value to `false` (0).
     */
    struct InitializeConstraintPass : public ModulePass
    {
        static char ID;
        InitializeConstraintPass() : ModulePass(ID) {}

        /**
         * @brief Runs the transformation pass on the given LLVM module.
         *
         * This method iterates over all global variables in the module and identifies
         * the one named `constraint`. If found and it is a declaration (without an initializer),
         * the pass sets its linkage to `InternalLinkage` and initializes it to `false`.
         *
         * @param M The LLVM module to be transformed.
         * @return true Always returns true to indicate the module was modified.
         */
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
