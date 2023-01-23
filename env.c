/* Environment */


/* [[file:README.org::*Environment][Environment:1]] */
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
