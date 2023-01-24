/* Parser */

/* Parser is implemented using recursive descent parser. It is as simple as */
/* possible and does not support any error recovery. It should be able to parse correct */
/* programs, but it is not guaranteed to parse incorrect programs. */

/* Context free grammar used to implement parser: */

/* [[file:../README.org::*Parser][Parser:1]] */
#include <wctype.h>
#include "ast.h"

SExpr parse_expr(char ** str);
SExpr parse_list(char ** str);
SExpr parse_atom(char ** str);
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

#define LISP(str) parse(#str)
/* Parser:1 ends here */


/* - Expression = Atom | List */

/* [[file:../README.org::*Parser][Parser:2]] */
SExpr parse_expr(char ** str){
    for(;iswspace(**str);(*str)++);
    if(**str=='(')
        return parse_list(str);
    else
        return parse_atom(str);
}
/* Parser:2 ends here */


/* - Atom = string | symbol */
/*   - string = '"' {any character except '"' } | '\\' {any character} '"' */
/*   - symbol = {any character except '(', ')', ' ', '\t', '\n', '\r'} */

/* [[file:../README.org::*Parser][Parser:3]] */
SExpr parse_atom(char ** str){
    size_t i;
    SExpr atom;
    atom.type=ATOM;
    switch (**str) {
    case '"':
        for(i=1;(*str)[i]!='"';i++)
            if((*str)[i]=='\\')
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
/* Parser:3 ends here */


/* List = "(" Expression* ")" */

/* [[file:../README.org::*Parser][Parser:4]] */
SExpr parse_list(char **str){
    (*str)++;
    SExpr list;
    list.type=LIST;
    size_t a_len=0;
    size_t m_len=1;
    list.ptr= malloc(m_len*sizeof(SExpr)+1);
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
        ((SExpr *)list.ptr)[a_len]= parse_expr(str);
        a_len++;
        if(a_len==m_len){
            m_len*=2;
            list.ptr= realloc(list.ptr,m_len*sizeof(SExpr)+1);
        }
    }
}
/* Parser:4 ends here */
