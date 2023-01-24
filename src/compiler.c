/* Compiler */

/* Compiler is implemented using LLVM C API. It is not very efficient, but it is */
/* easy to use and understand. The goal is to bootstrap the compiler, better compiler */
/* will be implemented later in compiled language itself. */


/* [[file:../README.org::*Compiler][Compiler:1]] */
#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Target.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/BitWriter.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "env.h"
/* Compiler:1 ends here */

/* Types */

/* compile type takes SExpr and returns LLVMTypeRef */
/* if SExpr is atom calls compile_atomic_type if SExpr is list calls compile_compound_type */


/* [[file:../README.org::*Types][Types:1]] */
LLVMTypeRef compile_atomic_type(SExpr type);
LLVMTypeRef compile_compound_type(SExpr* type);
LLVMTypeRef compile_type(SExpr sexp){
    if(sexp.type==ATOM)
        return compile_atomic_type(sexp);
    else if(sexp.type==LIST)
        return compile_compound_type((SExpr *)sexp.ptr);
    return NULL;
}
/* Types:1 ends here */

/* supprted atomic types */

/* atomic type is stored as pair of name and LLVMTypeRef */


/* [[file:../README.org::*supprted atomic types][supprted atomic types:1]] */
struct{
    char *name;
     LLVMTypeRef (*fn)();
} types[]={
    {"<i8>",LLVMInt8Type},
    {"<i16>",LLVMInt16Type},
    {"<i32>",LLVMInt32Type},
    {"<i64>",LLVMInt64Type},
    {"<f32>",LLVMFloatType},
    {"<f64>",LLVMDoubleType},
    {"<void>",LLVMVoidType},
    {0,0}
};
LLVMTypeRef compile_atomic_type(SExpr sexp){
    for(int i=0;types[i].name;i++)
        if(atomeq(sexp,types[i].name))
            return types[i].fn();
    return 0;
}
/* supprted atomic types:1 ends here */

/* supported compound types */

/* compound types are stored as pair of name and function that takes list of SExpr and returns LLVMTypeRef */


/* [[file:../README.org::*supported compound types][supported compound types:1]] */
LLVMTypeRef compile_ptr(SExpr* list);
LLVMTypeRef compile_fn(SExpr* list);
struct{
    char *name;
    LLVMTypeRef (*fn)(SExpr *);
} compound_types[]={
    {"<ptr>",compile_ptr},
    {"<fn>",compile_fn},
    {0,0}
};
LLVMTypeRef compile_compound_type(SExpr* list){
    for(int i=0;compound_types[i].name;i++)
        if(atomeq(list[0],compound_types[i].name))
            return compound_types[i].fn(list);
    return 0;
}
/* supported compound types:1 ends here */

/* pointer type */

/* pointers are defined as list of two elements: first element is name of pointer type, second element is type of pointer */
/* (<ptr> <type>) */
/* (<ptr>) stands for (<ptr> <void>) */


/* [[file:../README.org::*pointer type][pointer type:1]] */
LLVMTypeRef compile_ptr(SExpr* list){
      if(list[1].ptr==0)
          return LLVMPointerType(LLVMVoidType(),0);
      LLVMTypeRef t=compile_type(list[1]);
      if(!t)
          t=LLVMVoidType();
      return LLVMPointerType(t,0);
  }
/* pointer type:1 ends here */

/* function type */

/* functions are defined as list of two or more elements: first element is name of function type, second element is return type of function, rest of elements are types of arguments */
/* (<fn> <return type> <arg1 type> <arg2 type> ...) */
/* (<fn>) stands for (<fn> <void>) */


/* [[file:../README.org::*function type][function type:1]] */
LLVMTypeRef compile_fn(SExpr* list){
      LLVMTypeRef ret=compile_type(list[1]);
      if(!ret)
          return LLVMFunctionType(LLVMVoidType(),0,0,0);
      LLVMTypeRef *args=0;
      size_t argn;
      for(argn=0;list[argn+2].ptr;argn++);
      args=malloc(argn*sizeof(LLVMTypeRef));
      for(size_t i=0;i<argn;i++)
          args[i]=compile_type(list[i+2]);
      LLVMTypeRef fn=LLVMFunctionType(ret,args,argn,0);
      free(args);
      return fn;
  }
/* function type:1 ends here */

/* Values */

/* Values are either numbers, strings, or symbols. */
/* numbers or compiled to LLVM constants, strings are compiled to LLVM global strings, and are looked up in the environment. */


/* [[file:../README.org::*Values][Values:1]] */
LLVMValueRef compile_value(SExpr sexp){
    if(sexp.type==ATOM){
        if(isdigit(*(char *)sexp.ptr))
            return LLVMConstInt(LLVMInt32Type(),atoi((char *)sexp.ptr),0);
        else if(*(char *)sexp.ptr=='"')
            return LLVMConstString((char *)sexp.ptr+1,strlen((char *)sexp.ptr)-2,0);
        else
            return get_var((char *)sexp.ptr);
    }
    printf("unknown value: \n");
    prints(sexp);
    return 0;
}
/* Values:1 ends here */

/* Instructions */

/* instruction compiler functions are named compile_<instruction> */
/* implemented as functions that take a pointer to builder and a pointer to the instruction list and return a pointer to the last instruction */


/* [[file:../README.org::*Instructions][Instructions:1]] */
LLVMValueRef compile_add(LLVMBuilderRef builder,SExpr* sexp);
LLVMValueRef compile_sub(LLVMBuilderRef builder,SExpr* sexp);
LLVMValueRef compile_mul(LLVMBuilderRef builder,SExpr* sexp);
LLVMValueRef compile_div(LLVMBuilderRef builder,SExpr* sexp);
LLVMValueRef compile_call(LLVMBuilderRef builder,SExpr* sexp);
LLVMValueRef compile_store(LLVMBuilderRef builder,SExpr* sexp);
LLVMValueRef compile_load(LLVMBuilderRef builder,SExpr* sexp);
LLVMValueRef compile_phi(LLVMBuilderRef builder,SExpr* sexp);
LLVMValueRef compile_cast(LLVMBuilderRef builder,SExpr* sexp);
LLVMValueRef compile_assign(LLVMBuilderRef builder,SExpr* sexp);
struct instruction{
    char *name;
    LLVMValueRef (*compile)(LLVMBuilderRef,SExpr*);
}   instructions[]={
    {"add",compile_add},
    {"sub",compile_sub},
    {"mul",compile_mul},
    {"div",compile_div},
    {"call",compile_call},
    {"store",compile_store},
    {"load",compile_load},
    {"phi",compile_phi},
    {"cast",compile_cast},
    {"assign",compile_assign},
    {0,0}
};
LLVMValueRef compile_instr(LLVMBuilderRef builder,SExpr* sexp){
    for(size_t i=0;instructions[i].name;i++)
        if(!strcmp(sexp[0].ptr,instructions[i].name))
            return instructions[i].compile(builder,sexp);
    return 0;
}
/* Instructions:1 ends here */

/* Return */

/* syntax: (ret value) or (ret) for void */


/* [[file:../README.org::*Return][Return:1]] */
LLVMValueRef compile_ret(LLVMBuilderRef *builder,SExpr *sexp){
    if(sexp[1].ptr==0)
        return LLVMBuildRetVoid(*builder);
    else
        return LLVMBuildRet(*builder,compile_value(sexp[1]));
}
/* Return:1 ends here */

/* Branch */

/* syntax: (br cond label1 label2) or (br label) for unconditional branch */


/* [[file:../README.org::*Branch][Branch:1]] */
LLVMValueRef compile_br(LLVMBuilderRef builder,SExpr* sexp){
    if(sexp[3].ptr)
        return LLVMBuildCondBr(builder,compile_value(sexp[1]),
                               (LLVMBasicBlockRef)compile_value(sexp[2]),
                               (LLVMBasicBlockRef)compile_value(sexp[3]));
    else
        return LLVMBuildBr(builder,(LLVMBasicBlockRef)compile_value(sexp[1]));
}
/* Branch:1 ends here */

/* Add */

/* syntax: (add value1 value2) */


/* [[file:../README.org::*Add][Add:1]] */
LLVMValueRef compile_add(LLVMBuilderRef builder,SExpr* sexp){
    return LLVMBuildAdd(builder,compile_value(sexp[2]),
                               compile_value(sexp[3]),
                               (char *)sexp[0].ptr);
}
/* Add:1 ends here */

/* Sub */

/* syntax: (sub value1 value2) */


/* [[file:../README.org::*Sub][Sub:1]] */
LLVMValueRef compile_sub(LLVMBuilderRef builder,SExpr* sexp){
    return LLVMBuildSub(builder,compile_value(sexp[1]),
                               compile_value(sexp[2]),
                               (char *)sexp[0].ptr);
}
/* Sub:1 ends here */

/* Mul */

/* syntax: (mul value1 value2) */


/* [[file:../README.org::*Mul][Mul:1]] */
LLVMValueRef compile_mul(LLVMBuilderRef builder,SExpr* sexp){
    return LLVMBuildMul(builder,compile_value(sexp[1]),
                               compile_value(sexp[2]),
                               (char *)sexp[0].ptr);
}
/* Mul:1 ends here */

/* Div */

/* syntax: (div value1 value2) */


/* [[file:../README.org::*Div][Div:1]] */
LLVMValueRef compile_sdiv(LLVMBuilderRef builder,SExpr* sexp){
    return LLVMBuildSDiv(builder,compile_value(sexp[1]),
                               compile_value(sexp[2]),
                               (char *)sexp[0].ptr);
}
/* Div:1 ends here */

/* Call */

/* syntax: (call fn arg1 arg2 ...) */
/* fn is a function name or a function pointer */


/* [[file:../README.org::*Call][Call:1]] */
LLVMValueRef compile_call(LLVMBuilderRef builder,SExpr* sexp){
    LLVMValueRef fn=compile_value(sexp[1]);
    LLVMValueRef *args=0;
    size_t argn;
    for(argn=0;sexp[argn+2].ptr;argn++);
    args=malloc(argn*sizeof(LLVMValueRef));
    for(size_t i=0;i<argn;i++)
        args[i]=compile_value(sexp[i+2]);
    // using LLVMBuildCall2 to avoid warning
    LLVMValueRef call=LLVMBuildCall2(builder,LLVMGetElementType(LLVMTypeOf(fn)),fn,args,argn,"call");
    free(args);
    return call;
}
/* Call:1 ends here */

/* Store */

/* syntax: (store value ptr) */


/* [[file:../README.org::*Store][Store:1]] */
LLVMValueRef compile_store(LLVMBuilderRef builder,SExpr* sexp){
    return LLVMBuildStore(builder,compile_value(sexp[1]),compile_value(sexp[2]));
}
/* Store:1 ends here */

/* Load */

/* syntax: (load ptr) */


/* [[file:../README.org::*Load][Load:1]] */
LLVMValueRef compile_load(LLVMBuilderRef builder,SExpr* sexp){
    return LLVMBuildLoad2(builder,LLVMGetElementType(LLVMTypeOf(compile_value(sexp[1]))),compile_value(sexp[1]),"load");
}
/* Load:1 ends here */

/* Phi */

/* syntax: (phi type (value1 label1) (value2 label2) ...) */
/* type is the type of the phi node */


/* [[file:../README.org::*Phi][Phi:1]] */
LLVMValueRef compile_phi(LLVMBuilderRef builder,SExpr* sexp){
    LLVMTypeRef type=compile_type(sexp[1]);
    LLVMValueRef phi=LLVMBuildPhi(builder,type,"phi");
    size_t argn;
    for(argn=0;sexp[argn+2].ptr;argn++);
    LLVMValueRef *values=malloc(argn*sizeof(LLVMValueRef));
    LLVMBasicBlockRef *labels=malloc(argn*sizeof(LLVMBasicBlockRef));
    for(size_t i=0;i<argn;i++){
        values[i]=compile_value(((SExpr *)(sexp[i+2].ptr))[0]);
        labels[i]=(LLVMBasicBlockRef)compile_value(((SExpr *)(sexp[i+2].ptr))[1]);
    }
    LLVMAddIncoming(phi,values,labels,argn);
    free(values);
    free(labels);
    return phi;
}
/* Phi:1 ends here */

/* Cast */

/* syntax: (cast type value) */


/* [[file:../README.org::*Cast][Cast:1]] */
LLVMValueRef compile_cast(LLVMBuilderRef builder,SExpr* sexp){
    return LLVMBuildBitCast(builder,compile_value(sexp[2]),compile_type(sexp[1]),"cast");
}
/* Cast:1 ends here */

/* Define */

/* syntax: (def name value) or (def name (instruction ...)) */


/* [[file:../README.org::*Define][Define:1]] */
LLVMValueRef compile_assign(LLVMBuilderRef builder,SExpr* sexp){
    LLVMValueRef value;
    if(sexp[2].type==ATOM)
        value=compile_value(sexp[2]);
    else
        value=compile_instr(builder,(SExpr *)(sexp[2].ptr));
    add_var((char *)sexp[1].ptr,value);
    return value;
}
/* Define:1 ends here */

/* Blocks */

/* Syntax: (block name (instruction ...) ...) */


/* [[file:../README.org::*Blocks][Blocks:1]] */
void compile_block(LLVMValueRef function,SExpr sexp){
    if(sexp.type==LIST){
        SExpr *list=(SExpr *)sexp.ptr;
        if(atomeq(list[0],"block")){
            LLVMBuilderRef builder=LLVMCreateBuilder();
            LLVMBasicBlockRef block=LLVMAppendBasicBlock(function,(char *)list[1].ptr);
            LLVMPositionBuilderAtEnd(builder,block);
            for(SExpr *i=list+2;i->ptr;i++)
                compile_instr(builder,(SExpr *)i->ptr);
        }
    }
}
/* Blocks:1 ends here */

/* Globals */

/* Only functions are implemented for now. */
/* function syntax: (function ((name <return type>) (arg1 <type>) (arg2 <type>) ...) (block ...) ...) */


/* [[file:../README.org::*Globals][Globals:1]] */
void compile_global(LLVMModuleRef module,SExpr sexp){
    if(sexp.type==LIST){
        SExpr *list=(SExpr *)sexp.ptr;
        if(atomeq(list[0],"function")){
            SExpr *signatures=(SExpr *)list[1].ptr;
            char *name;
            LLVMTypeRef rettype;
            if(signatures[0].type==LIST) {
                SExpr *namesig = (SExpr *) signatures[0].ptr;
                name = (char *) namesig[0].ptr;
                rettype = compile_type(namesig[1]);
            } else{
                name=(char *)signatures[0].ptr;
                rettype=LLVMVoidType();
            }
            size_t n_args=0;
            for(SExpr *i=signatures+1;i->ptr;i++)
                n_args++;
            LLVMTypeRef *argtypes=malloc(n_args*sizeof(LLVMTypeRef));
            for(size_t i=0;i<n_args;i++)
                argtypes[i]=compile_type(((SExpr *)signatures[i+1].ptr)[1]);
            LLVMTypeRef functype=LLVMFunctionType(rettype,argtypes,n_args,0);
            LLVMValueRef function=LLVMAddFunction(module,name,functype);
            for(size_t i=0;i<n_args;i++){
                LLVMValueRef arg=LLVMGetParam(function,i);
                LLVMSetValueName(arg,((char *)(((SExpr *)signatures[i+1].ptr)[0].ptr)));
            }
            for(SExpr *body=list+2;body->ptr;body++)
                compile_block(function,*body);
            free(argtypes);
        }
    }
}
/* Globals:1 ends here */

/* Module */

/* syntax: (module name global ...) */
/* where global is a function or a global variable declarations */


/* [[file:../README.org::*Module][Module:1]] */
LLVMModuleRef compile_module(SExpr sexp){
    if(sexp.type==LIST){
        SExpr *list=(SExpr *)sexp.ptr;
        if(atomeq(list[0],"module")){
            LLVMModuleRef module=LLVMModuleCreateWithName((char *)list[1].ptr);
            for(SExpr *i=list+2;i->ptr;i++)
                compile_global(module,*i);
            return module;
        }
    }
    return 0;
}
/* Module:1 ends here */
