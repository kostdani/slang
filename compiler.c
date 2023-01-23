/* Compiler */


/* [[file:README.org::*Compiler][Compiler:1]] */
#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Target.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/BitWriter.h>
#include <ctype.h>
#include <stdlib.h>
#include "parser.c"
#include "env.c"

LLVMTypeRef compile_type(SExpr sexp){
    if(sexp.ptr==0)
        return LLVMVoidType();
    if(sexp.type==ATOM){
        if(atomeq(sexp,"<i8>"))
            return LLVMInt8Type();
        else if(atomeq(sexp,"<i16>"))
            return LLVMInt16Type();
        else if(atomeq(sexp,"<i32>"))
            return LLVMInt32Type();
        else if(atomeq(sexp,"<i64>"))
            return LLVMInt64Type();
        else if(atomeq(sexp,"<f32>"))
            return LLVMFloatType();
        else if(atomeq(sexp,"<f64>"))
            return LLVMDoubleType();
        else if(atomeq(sexp,"<void>"))
            return LLVMVoidType();
        return 0;
    }else if(sexp.type==LIST){
        SExpr *p=(SExpr *)sexp.ptr;
        if(atomeq(p[0],"<ptr>")){
            LLVMTypeRef t=compile_type(p[1]);
            if(!t)
                t=LLVMVoidType();
            return LLVMPointerType(t,0);
        }else if(atomeq(p[0],"<fn>")){
            LLVMTypeRef ret=compile_type(p[1]);
            if(!ret)
                return LLVMFunctionType(LLVMVoidType(),0,0,0);
            LLVMTypeRef *args=0;
            size_t argn;
            for(argn=0;p[argn+2].ptr;argn++);
            args=malloc(argn*sizeof(LLVMTypeRef));
            for(size_t i=0;i<argn;i++)
                args[i]=compile_type(p[i+2]);
            LLVMTypeRef fn=LLVMFunctionType(ret,args,argn,0);
            free(args);
            return fn;
        }
    }
    printf("unknown type: \n");
    prints(sexp);
    return 0;
}
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

void compile_instr(LLVMBuilderRef builder,SExpr sexp){
    LLVMModuleRef module=LLVMGetGlobalParent(LLVMGetBasicBlockParent(LLVMGetInsertBlock(builder)));
    if(sexp.type==LIST){
        SExpr *list=(SExpr *)sexp.ptr;
        if(atomeq(list[0],"ret")){
            LLVMValueRef ret=compile_value(list[1]);
            LLVMBuildRet(builder,ret);
        }else if(atomeq(list[0],"br")){
            // br <label> | br <cond> <label> <label>
            if(list[1].type==ATOM){
                LLVMBuildBr(builder,(LLVMBasicBlockRef)get_var((char *)list[1].ptr));
            }else{
                LLVMValueRef cond=compile_value(list[1]);
                LLVMBuildCondBr(builder,cond,(LLVMBasicBlockRef)get_var((char *)list[2].ptr),(LLVMBasicBlockRef)get_var((char *)list[3].ptr));
            }
        }else if(atomeq(list[0],"add")){
            char *name=(char *)list[1].ptr;
            LLVMValueRef op1=compile_value(list[2]);
            LLVMValueRef op2=compile_value(list[3]);
            LLVMValueRef add=LLVMBuildAdd(builder,op1,op2,name);
            add_var(name,add);
        }else if(atomeq(list[0],"sub")){
            char *name=(char *)list[1].ptr;
            LLVMValueRef op1=compile_value(list[2]);
            LLVMValueRef op2=compile_value(list[3]);
            LLVMValueRef sub=LLVMBuildSub(builder,op1,op2,name);
            add_var(name,sub);
        }else if(atomeq(list[0],"mul")){
            char *name=(char *)list[1].ptr;
            LLVMValueRef op1=compile_value(list[2]);
            LLVMValueRef op2=compile_value(list[3]);
            LLVMValueRef mul=LLVMBuildMul(builder,op1,op2,name);
            add_var(name,mul);
        }else if(atomeq(list[0],"div")){
            char *name=(char *)list[1].ptr;
            LLVMValueRef op1=compile_value(list[2]);
            LLVMValueRef op2=compile_value(list[3]);
            LLVMValueRef div=LLVMBuildUDiv(builder,op1,op2,name);
            add_var(name,div);
        }else if(atomeq(list[0],"call")){
            char *name=(char *)list[1].ptr;
            // third elem is name of function to call
            char *fname=(char *)list[2].ptr;
            // rest of list is arguments
            size_t n_args=0;
            for(SExpr *i=list+3;i->ptr;i++)
                n_args++;
            LLVMValueRef *args=malloc(n_args*sizeof(LLVMValueRef));
            for(size_t i=0;i<n_args;i++)
                args[i]=compile_value(list[i+2]);
            LLVMValueRef func=LLVMGetNamedFunction(module,fname);
            LLVMTypeRef type=LLVMGetReturnType(LLVMGetElementType(LLVMTypeOf(func)));
            LLVMValueRef call=LLVMBuildCall2(builder,type,func,args,n_args,(char *)list[1].ptr);
            add_var(name,call);
            free(args);
        }else if(atomeq(list[0],"load")){
            // second elem is name of variable to store result
            char *rname=(char *)list[1].ptr;
            // third elem is name of variable to load
            char *name=(char *)list[2].ptr;
            LLVMValueRef load=LLVMBuildLoad2(builder, LLVMTypeOf(get_var(name)),get_var(name),name);
            add_var(rname,load);
        }else if(atomeq(list[0],"store")){
            // second elem is name of variable to store
            char *name=(char *)list[1].ptr;
            // third elem is value to store
            LLVMValueRef value=compile_value(list[2]);
            LLVMBuildStore(builder,value,get_var(name));
        }
    }
}

void compile_block(LLVMValueRef function,SExpr sexp){
    if(sexp.type==LIST){
        SExpr *list=(SExpr *)sexp.ptr;
        if(atomeq(list[0],"block")){
            LLVMBuilderRef builder=LLVMCreateBuilder();
            LLVMBasicBlockRef block=LLVMAppendBasicBlock(function,(char *)list[1].ptr);
            LLVMPositionBuilderAtEnd(builder,block);
            for(SExpr *i=list+2;i->ptr;i++)
                compile_instr(builder,*i);

        }
    }
}

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
/* Compiler:1 ends here */
