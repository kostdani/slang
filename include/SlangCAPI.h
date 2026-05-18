// [[file:../slang.org::*include][include:5]]
//===- SlangCAPI.h - CAPI for slang dialect -------------------*- C -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef SLANG_C_DIALECTS_H
#define SLANG_C_DIALECTS_H

#include "mlir-c/IR.h"

#ifdef __cplusplus
extern "C" {
#endif

MLIR_DECLARE_CAPI_DIALECT_REGISTRATION(Slang, slang);

//===---------------------------------------------------------------------===//
// CustomType
//===---------------------------------------------------------------------===//

MLIR_CAPI_EXPORTED bool mlirTypeIsASlangCustomType(MlirType type);

MLIR_CAPI_EXPORTED MlirType mlirSlangCustomTypeGet(MlirContext ctx, MlirStringRef value);

#ifdef __cplusplus
}
#endif

#endif // SLANG_C_DIALECTS_H
// include:5 ends here
