//===- Dialects.cpp - CAPI for dialects -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "SlangCAPI.h"

#include "SlangDialect.h"

#include "mlir/CAPI/Registration.h"

MLIR_DEFINE_CAPI_DIALECT_REGISTRATION(Slang, slang,
                                      mlir::slang::SlangDialect)

//===---------------------------------------------------------------------===//
// CustomType
//===---------------------------------------------------------------------===//

bool mlirTypeIsASlangCustomType(MlirType type) {
  return llvm::isa<mlir::slang::CustomType>(unwrap(type));
}

MlirType mlirSlangCustomTypeGet(MlirContext ctx, MlirStringRef value) {
  return wrap(mlir::slang::CustomType::get(unwrap(ctx), unwrap(value)));
}
