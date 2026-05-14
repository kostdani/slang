add_mlir_dialect_library(
  MLIRSlang
  SlangDialect.cpp

  # to silence LLVMProcessSources.cmake:116
  PARTIAL_SOURCES_INTENDED

  ADDITIONAL_HEADER_DIRS
  ${PROJECT_SOURCE_DIR}/include

  DEPENDS
  MLIRSlangIncGen
  copy_generated_td_to_src_include

  LINK_LIBS PUBLIC
  MLIRIR
  MLIRInferTypeOpInterface
  MLIRFuncDialect
#  MLIRRegisterAllDialects
#  MLIRRegisterAllPasses
)

add_mlir_public_c_api_library(SlangCAPI
  SlangCAPI.cpp
  # to silence LLVMProcessSources.cmake:116
  PARTIAL_SOURCES_INTENDED

  LINK_LIBS PUBLIC
  MLIRSlang
)

get_property(dialect_libs GLOBAL PROPERTY MLIR_DIALECT_LIBS)
get_property(conversion_libs GLOBAL PROPERTY MLIR_CONVERSION_LIBS)
set(LIBS ${dialect_libs} ${conversion_libs} MLIROptLib MLIRSlang)

add_llvm_executable(slang-opt slang-opt.cpp)
llvm_update_compile_flags(slang-opt)
target_link_libraries(slang-opt PRIVATE ${LIBS})
add_dependencies(slang-opt MLIRSlangIncGen)

mlir_check_all_link_libraries(slang-opt)
