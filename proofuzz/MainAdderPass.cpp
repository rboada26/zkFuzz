#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include <regex>

using namespace llvm;

namespace
{
    struct MainAdderPass : public ModulePass
    {
        static char ID;
        MainAdderPass() : ModulePass(ID) {}

        bool runOnModule(Module &M) override
        {
            // Create a `main` function that initializes an instance of the target circuit.
            createMainFunction(M);

            return true;
        }

        FunctionCallee declarePrintfFunction(Module &M)
        {
            LLVMContext &Ctx = M.getContext();
            Type *PrintfArgType = Type::getInt8PtrTy(Ctx);
            FunctionType *PrintfType = FunctionType::get(Type::getInt32Ty(Ctx), PrintfArgType, true);
            return M.getOrInsertFunction("printf", PrintfType);
        }

        std::vector<Instruction *> findAllocas(Function *F, const std::string &pattern)
        {
            std::vector<Instruction *> allocas;
            std::regex regexPattern(pattern);

            for (auto &BB : *F)
            {
                for (auto &I : BB)
                {
                    if (AllocaInst *AI = dyn_cast<AllocaInst>(&I))
                    {
                        if (std::regex_search(AI->getName().str(), regexPattern))
                        {
                            allocas.push_back(AI);
                        }
                    }
                }
            }
            return allocas;
        }

        void createMainFunction(Module &M)
        {
            LLVMContext &Context = M.getContext();
            IRBuilder<> Builder(Context);

            // Define the `main` function type and create the function
            FunctionType *mainType = FunctionType::get(Builder.getInt32Ty(), false);
            Function *mainFunc = Function::Create(mainType, Function::ExternalLinkage, "main", M);

            // Create the basic block for the `main` function
            BasicBlock *entry = BasicBlock::Create(Context, "entry", mainFunc);
            Builder.SetInsertPoint(entry);

            std::vector<Instruction *> inputs, outputs;
            std::string circuitName;

            // ########### Extract all inputs and outputs using pattern matching #############
            for (Function &F : M)
            {
                if (F.getName().contains("fn_template_init"))
                {
                    circuitName = F.getName().substr(17).str();
                    for (auto &BB : F)
                    {
                        if (BB.getName() == "entry")
                        {
                            inputs = findAllocas(&F, "initial.*.input");
                            outputs = findAllocas(&F, "initial.*.output");
                            break;
                        }
                    }
                    break;
                }
            }

            // ############ Print Outputs #####################

            // Declare the printf function
            FunctionCallee printfFunc = declarePrintfFunction(M);
            // Create format strings for printf
            Constant *formatStr = ConstantDataArray::getString(Context, "%ld\n", true);
            GlobalVariable *formatStrVar = new GlobalVariable(
                M, formatStr->getType(), true, GlobalValue::PrivateLinkage, formatStr, ".str");
            for (Function &F : M)
            {
                if (F.getName().contains("fn_template_build"))
                {
                    Value *instance = Builder.CreateCall(&F, {}, "instance");
                    unsigned index = 0;

                    for (auto &v : inputs)
                    {
                        Value *inputPtr = getGEP(Context, Builder, instance, index++, ("gep." + circuitName + "|" + v->getName().substr(8).str()).c_str());
                        Builder.CreateStore(ConstantInt::get(Builder.getInt128Ty(), 123), inputPtr); // 123 is the example values
                    }

                    std::string initFuncName = "fn_template_init_" + circuitName;
                    Function *initFunc = M.getFunction(initFuncName);
                    if (initFunc)
                    {
                        Builder.CreateCall(initFunc, {instance});
                    }

                    for (auto &v : outputs)
                    {
                        Value *outputPtr = getGEP(Context, Builder, instance, index++, ("gep." + circuitName + "|" + v->getName().substr(8).str()).c_str());
                        Value *outputVal = Builder.CreateLoad(Builder.getInt128Ty(), outputPtr, ("val." + circuitName + "|" + v->getName().substr(8).str()).c_str());
                        
                        Value *lowPart = Builder.CreateTrunc(outputVal, Type::getInt64Ty(Context));
                        Value *shifted = Builder.CreateLShr(outputVal, ConstantInt::get(Type::getInt128Ty(Context), 64));
                        Value *highPart = Builder.CreateTrunc(shifted, Type::getInt64Ty(Context));

                        Value *formatStrPtr = Builder.CreatePointerCast(formatStrVar, Type::getInt8PtrTy(Context));
                        Builder.CreateCall(printfFunc, {formatStrPtr, highPart});
                        Builder.CreateCall(printfFunc, {formatStrPtr, lowPart});
                    }

                    break;
                }
            }

            // Load the value of `constraint` and zero-extend it to i32
            GlobalVariable *constraintVar = M.getGlobalVariable("constraint");
            Value *constraintVal = Builder.CreateLoad(Builder.getInt1Ty(), constraintVar, "constraint_val");
            Value *constraintI32 = Builder.CreateZExt(constraintVal, Builder.getInt32Ty(), "constraint_i32");
            // Return 0 or 1 based on the constraint value
            Builder.CreateRet(constraintI32);

            // Finalize the function by setting up the entry point
            Builder.SetInsertPoint(&entry->back());
        }

        Value *getGEP(LLVMContext &Context, IRBuilder<> &Builder, Value *instance, unsigned index, const char *name)
        {
            return Builder.CreateGEP(instance->getType()->getPointerElementType(), instance,
                                     {Builder.getInt32(0), Builder.getInt32(index)}, name);
        }
    };
}

char MainAdderPass::ID = 0;
static RegisterPass<MainAdderPass> X("MainAdderPass", "Circom Transformation Pass", false, false);
