/* main */


/* [[file:README.org::*main][main:1]] */
#include "compiler.c"
int main(){
    SExpr sexp=parse("(module test (function ((main <i32>)) (block entry (ret 0))))");
    prints(sexp);
    //
    LLVMModuleRef module=compile_module(sexp);
    // dump ir
    LLVMDumpModule(module);
    //cleanup
    LLVMDisposeModule(module);
    free_env();
    free_sexpr(sexp);
    return 0;
}
/* main:1 ends here */
