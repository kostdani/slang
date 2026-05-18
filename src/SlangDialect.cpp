// [[file:../slang.org::*src][src:2]]
//===- SlangDialect.cpp - Slang dialect ---------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "SlangDialect.h"

#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/DialectImplementation.h"
#include "mlir/IR/PatternMatch.h"
#include "mlir/Rewrite/FrozenRewritePatternSet.h"
#include "mlir/Support/LogicalResult.h"
#include "mlir/Tools/mlir-opt/MlirOptMain.h"
#include "mlir/Transforms/GreedyPatternRewriteDriver.h"
#include "llvm/ADT/TypeSwitch.h"

using namespace mlir;
using namespace mlir::slang;

#include "SlangDialect.cpp.inc"

//===----------------------------------------------------------------------===//
// Slang dialect.
//===----------------------------------------------------------------------===//

void SlangDialect::initialize() {
  addOperations<
#define GET_OP_LIST
#include "SlangOps.cpp.inc"

      >();
  registerTypes();
}

//===----------------------------------------------------------------------===//
// Slang ops
//===----------------------------------------------------------------------===//

#define GET_OP_CLASSES
#include "SlangOps.cpp.inc"

namespace mlir::slang {
#define GEN_PASS_DEF_SLANGSWITCHBARFOO
#include "SlangPasses.h.inc"

//===----------------------------------------------------------------------===//
// Slang passes
//===----------------------------------------------------------------------===//

namespace {
class SlangSwitchBarFooRewriter : public OpRewritePattern<func::FuncOp> {
public:
  using OpRewritePattern<func::FuncOp>::OpRewritePattern;
  LogicalResult matchAndRewrite(func::FuncOp op,
                                PatternRewriter &rewriter) const final {
    if (op.getSymName() == "bar") {
      rewriter.modifyOpInPlace(op, [&op]() { op.setSymName("foo"); });
      return success();
    }
    return failure();
  }
};

class SlangSwitchBarFoo
    : public impl::SlangSwitchBarFooBase<SlangSwitchBarFoo> {
public:
  using impl::SlangSwitchBarFooBase<
      SlangSwitchBarFoo>::SlangSwitchBarFooBase;
  void runOnOperation() final {
    RewritePatternSet patterns(&getContext());
    patterns.add<SlangSwitchBarFooRewriter>(&getContext());
    FrozenRewritePatternSet patternSet(std::move(patterns));
    if (failed(applyPatternsAndFoldGreedily(getOperation(), patternSet)))
      signalPassFailure();
  }
};
} // namespace
} // namespace mlir::slang

//===----------------------------------------------------------------------===//
// Slang types
//===----------------------------------------------------------------------===//

#define GET_TYPEDEF_CLASSES
#include "SlangTypes.cpp.inc"

void SlangDialect::registerTypes() {
  addTypes<
#define GET_TYPEDEF_LIST
#include "SlangTypes.cpp.inc"

      >();
}
// src:2 ends here
