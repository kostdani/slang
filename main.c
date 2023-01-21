#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <wctype.h>


typedef struct s_expr{
    enum{
        LIST,
        ATOM
    } type;
    void * ptr;
} SExpr;

SExpr make_atom(char *str,size_t n){
    char *atom=(char *)malloc(n+1);
    strncpy(atom,str,n);
    atom[n]=0;
    SExpr a;
    a.type=ATOM;
    a.ptr=(void *)atom;
}

SExpr parse_expr(char ** str);

SExpr parse_atom(char ** str){
    size_t i;
    SExpr atom;
    atom.type=ATOM;
    switch (**str) {
        case '"':
            for(i=1;(*str)[i]!='"';i++)
                if(str[i]=='\\')
                    i++;
            i++;
            atom.ptr=malloc(i+1);
            strncpy((char *)atom.ptr,*str,i);
            ((char *)atom.ptr)[i]=0;
            (*str)+=i;
            return atom;
        default:
            for (i=0; !iswspace((*str)[i])&&(*str)[i]!=')'&&(*str)[i]!='('; i++);
            atom.ptr=malloc(i+1);
            strncpy((char *)atom.ptr,*str,i);
            ((char *)atom.ptr)[i]=0;
            (*str)+=i;
            return atom;
    }
}

SExpr parse_list(char **str){
    (*str)++;
    SExpr list;
    list.type=LIST;
    size_t a_len=0;
    size_t m_len=1;
    list.ptr= malloc(m_len*sizeof(SExpr));
    while(1){
        for(;iswspace(**str);(*str)++);
        if(**str==')'){
            (*str)++;
            SExpr nil;
            nil.type=LIST;
            nil.ptr=0;
            ((SExpr *)list.ptr)[a_len]=nil;
            return list;
        }
        if(a_len==m_len){
            m_len*=2;
            list.ptr= realloc(list.ptr,m_len*sizeof(SExpr));
        }
        ((SExpr *)list.ptr)[a_len]= parse_expr(str);
        a_len++;
    }
}

SExpr parse_expr(char ** str){
    for(;iswspace(**str);(*str)++);
    if(**str=='(')
        return parse_list(str);
    else
        return parse_atom(str);

}

SExpr parse(char *s){
    size_t l=strlen(s);
    char *ostr=(char *)malloc(l+1);
    char *str=ostr;
    strcpy(str,s);
    str[l]=0;
    SExpr ast= parse_expr(&str);
    free(ostr);
    return ast;
}

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

void free_sexpr(SExpr sexp){
    if(sexp.type==LIST)
        for (SExpr *i = (SExpr *) sexp.ptr; i->ptr; i++)
            free_sexpr(*i);
    char *c=(char *)sexp.ptr;
    if(sexp.ptr)
        free(c);
}

#define LISP(str) parse(#str)

int main() {
    SExpr ast=LISP((a "ds" 3));
    print_sexpr(ast);
    free_sexpr(ast);

    return 0;
}