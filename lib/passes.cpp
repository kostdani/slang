#include "passes.h"

#include "mlir/InitAllDialects.h"
#include "mlir/InitAllPasses.h"

#include "mlir/Conversion/Passes.h"
#include "mlir/Conversion/FuncToLLVM/ConvertFuncToLLVM.h"
#include "mlir/Conversion/ArithToLLVM/ArithToLLVM.h"

#include "mlir/Target/LLVMIR/Dialect/All.h"

#include "mlir/Transforms/Passes.h"
#include "mlir/Pass/PassManager.h"
#include "mlir/Transforms/Passes.h"

namespace slang {

void registerDialectAndLLVMTranslations(mlir::MLIRContext &context) {

    // ------------------------------------------------------------
    // 1. Dialects (heavy full registry mode)
    // ------------------------------------------------------------
    mlir::DialectRegistry registry;

    registry.insert<
        mlir::func::FuncDialect,
        mlir::arith::ArithDialect,
        mlir::LLVM::LLVMDialect
    >();

    // Optional heavy dialect ecosystem (future-proof for slang)
    mlir::registerAllDialects(registry);

    // ------------------------------------------------------------
    // 2. LLVM IR translation (THIS IS THE HEAVY PART YOU WANTED)
    // ------------------------------------------------------------
    mlir::registerAllToLLVMIRTranslations(registry);

    context.appendDialectRegistry(registry);
    context.loadAllAvailableDialects();
}

void buildDefaultLoweringPipeline(mlir::PassManager &pm) {

    // canonical cleanup
    pm.addPass(mlir::createCanonicalizerPass());
    pm.addPass(mlir::createCSEPass());

    // Func → LLVM
    pm.addPass(mlir::createConvertFuncToLLVMPass());

    // Arith → LLVM
    pm.addPass(mlir::createArithToLLVMConversionPass());

    // cleanup again after lowering
    pm.addPass(mlir::createCanonicalizerPass());
    pm.addPass(mlir::createCSEPass());
}

}
