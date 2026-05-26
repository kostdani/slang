#pragma once

#include "mlir/IR/MLIRContext.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/Pass/PassManager.h"

namespace slang {

/// Registers dialects + ALL LLVM translation layers (heavy mode)
void registerDialectAndLLVMTranslations(mlir::MLIRContext &context);

/// Builds full lowering pipeline (Func → LLVM, Arith → LLVM, etc.)
void buildDefaultLoweringPipeline(mlir::PassManager &pm);

}
