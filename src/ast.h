/* AST */

/* Abstract syntax tree is maximally simplified. There are 2 types of nodes: */
/* - LIST - list of nodes */
/* - ATOM - atom node (leaf) storing a string */


/* [[file:../README.org::*AST][AST:1]] */
#pragma once
#include <stdio.h>
#include <string.h>
#include <malloc.h>

typedef struct s_expr{
    enum{
        LIST,
        ATOM
    } type;
    void * ptr;
} SExpr;

void free_sexpr(SExpr sexp){
    if(sexp.type==LIST)
        for (SExpr *i = (SExpr *) sexp.ptr; i->ptr; i++)
            free_sexpr(*i);
    if(sexp.ptr)
        free(sexp.ptr);
}

#define atomeq(atom, str) (atom.type==ATOM && !strcmp(str,(char *)atom.ptr))
/* AST:1 ends here */

/* Printing */

/* Printing function is simple and prints the AST in a human readable form. */


/* [[file:../README.org::*Printing][Printing:1]] */
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
/* Printing:1 ends here */
