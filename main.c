/* main */


/* [[file:README.org::*main][main:1]] */
#include "compiler.c"

int main() {

  SExpr sexp=LISP((module test (function (main (argc <i32>)) (block :entry: (add <i32> n 2 2) (add <i32> m n n) (ret m)))));
  prints(sexp);
  LLVMModuleRef module=compile_module(sexp);
  LLVMDumpModule(module);
  free_sexpr(sexp);
  return 0;

}
/* main:1 ends here */
