/* main */


/* [[file:README.org::*main][main:1]] */
#include "compiler.c"

int main() {

  SExpr sexp=LISP((module "test" (function ((main i32) (argc i32)) (block entry (ret 0)))));
  print_sexpr(sexp);
  printf("\n");
  LLVMModuleRef module=compile_module(sexp);
  LLVMDumpModule(module);
  free_sexpr(sexp);
  return 0;

}
/* main:1 ends here */
