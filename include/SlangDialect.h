//===- SlangDialect.h - Slang dialect -----------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef SLANG_SLANGDIALECT_H
#define SLANG_SLANGDIALECT_H

#include "mlir/Bytecode/BytecodeOpInterface.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/Dialect.h"
#include "mlir/IR/OpDefinition.h"
#include "mlir/Interfaces/InferTypeOpInterface.h"
#include "mlir/Interfaces/SideEffectInterfaces.h"
#include "mlir/Pass/Pass.h"

#include <memory>

#include "SlangDialect.h.inc"

#define GET_OP_CLASSES
#include "SlangOps.h.inc"

namespace mlir::slang {
#define GEN_PASS_DECL
#include "SlangPasses.h.inc"

#define GEN_PASS_REGISTRATION
#include "SlangPasses.h.inc"
} // namespace mlir::slang

#define GET_TYPEDEF_CLASSES
#include "SlangTypes.h.inc"

#endif // SLANG_SLANGDIALECT_H
