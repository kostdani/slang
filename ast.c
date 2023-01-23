/* AST */


/* [[file:README.org::*AST][AST:1]] */
#include <stdio.h>
#include <malloc.h>

typedef struct s_expr{
    enum{
        LIST,
        ATOM
    } type;
    void * ptr;
} SExpr;

void print_sexpr(SExpr sexp){
    if(sexp.type==ATOM)
        printf("%s",(char *)sexp.ptr);
    else if(sexp.type==LIST) {
        printf("(");
        for (SExpr *i = (SExpr *) sexp.ptr; i->ptr; i++) {
            if(i!=sexp.ptr)
                printf(" ");
            print_sexpr(*i);
        }
        printf(")");
    }
}

void prints(SExpr sexp){
    print_sexpr(sexp);
    printf("\n");
}

void free_sexpr(SExpr sexp){
    if(sexp.type==LIST)
        for (SExpr *i = (SExpr *) sexp.ptr; i->ptr; i++)
            free_sexpr(*i);
    char *c=(char *)sexp.ptr;
    if(sexp.ptr)
        free(c);
}


int atomeq(SExpr atom,char *str){
    if(atom.type==ATOM)
        return !strcmp(str,(char *)atom.ptr);
    return 0;
}
/* AST:1 ends here */
