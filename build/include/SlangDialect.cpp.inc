/*===- TableGen'erated file -------------------------------------*- C++ -*-===*\
|*                                                                            *|
|* Dialect Definitions                                                        *|
|*                                                                            *|
|* Automatically generated file, do not edit!                                 *|
|* From: SlangDialect.td                                                      *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

MLIR_DEFINE_EXPLICIT_TYPE_ID(::mlir::slang::SlangDialect)
namespace mlir {
namespace slang {

SlangDialect::SlangDialect(::mlir::MLIRContext *context)
    : ::mlir::Dialect(getDialectNamespace(), context, ::mlir::TypeID::get<SlangDialect>())
    
     {
  
  initialize();
}

SlangDialect::~SlangDialect() = default;

} // namespace slang
} // namespace mlir
