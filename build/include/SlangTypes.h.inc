/*===- TableGen'erated file -------------------------------------*- C++ -*-===*\
|*                                                                            *|
|* TypeDef Declarations                                                       *|
|*                                                                            *|
|* Automatically generated file, do not edit!                                 *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

#ifdef GET_TYPEDEF_CLASSES
#undef GET_TYPEDEF_CLASSES


namespace mlir {
class AsmParser;
class AsmPrinter;
} // namespace mlir
namespace mlir {
namespace slang {
class CustomType;
namespace detail {
struct CustomTypeStorage;
} // namespace detail
class CustomType : public ::mlir::Type::TypeBase<CustomType, ::mlir::Type, detail::CustomTypeStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "slang.custom";
  static constexpr ::llvm::StringLiteral dialectName = "slang";
  static CustomType get(::mlir::MLIRContext *context, ::llvm::StringRef value);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"custom"};
  }

  static ::mlir::Type parse(::mlir::AsmParser &odsParser);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::llvm::StringRef getValue() const;
};
} // namespace slang
} // namespace mlir
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::slang::CustomType)

#endif  // GET_TYPEDEF_CLASSES

