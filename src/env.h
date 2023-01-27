/* Environment */
/* :PROPERTIES: */
/* :HEADER-ARGS: :tangle src/env.h :comments both :exports both :mkdirp yes */
/* :END: */

/* Environment is implemented as a linked list of name and value pairs. It is */
/* possible to define new variables and functions in the environment. It is also */
/* possible to lookup variables and functions in the environment. */


/* [[file:../README.org::*Environment][Environment:1]] */
#pragma once
#include <llvm-c/Core.h>
#include <malloc.h>
#include <string.h>

typedef struct env{
    char *name;
    LLVMValueRef value;
    struct env *next;
} Env;

Env *global_env=0;

void add_var(char *name,LLVMValueRef value){
    Env *e=(Env *)malloc(sizeof(Env));
    e->name=name;
    e->value=value;
    e->next=global_env;
    global_env=e;
}

LLVMValueRef get_var(char *name){
    for(Env *e=global_env;e;e=e->next)
        if(!strcmp(e->name,name))
            return e->value;
    return 0;
}

void free_env(){
    for(Env *e=global_env;e;e=e->next)
        free(e);
}
/* Environment:1 ends here */
