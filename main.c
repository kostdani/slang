/* main */


/* [[file:README.org::*main][main:1]] */
#include "parser.c"

int main() {
    SExpr ast = LISP((function ((main :i32) (argc :i32) (argv :pointer (:pointer :i8))) (return :i32 0)));
    print_sexpr(ast);
    free_sexpr(ast);
    return 0;
}
/* main:1 ends here */
