
add_llvm_executable(test test.cpp)
llvm_update_compile_flags(test)
target_link_libraries(test
    PRIVATE
    MLIRInferTypeOpInterface
    
    MLIRExecutionEngine
    
    MLIRArithToLLVM
    MLIRFuncToLLVM
    MLIRReconcileUnrealizedCasts
    MLIRArithDialect
    MLIRFuncDialect
    MLIRLLVMDialect
    MLIRExecutionEngine
    MLIRLLVMToLLVMIRTranslation
    MLIRTargetLLVMIRExport
    MLIRTransforms
    MLIRPass
    MLIRIR
    MLIRSupport
  )
  
mlir_check_all_link_libraries(test)
