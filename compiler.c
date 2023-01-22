/* Compiler */


/* [[file:README.org::*Compiler][Compiler:1]] */
#include <llvm-c/Core.h>
  #include <llvm-c/ExecutionEngine.h>
  #include <llvm-c/Target.h>
  #include <llvm-c/Analysis.h>
  #include <llvm-c/BitWriter.h>
  #include "parser.c"

// environment is linked list of variable names and their values
typedef struct env{
  char *name;
  LLVMValueRef value;
  struct env *next;
} Env;

Env *global_env=0;
// add variable to environment
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

  int atomeq(SExpr atom,char *str){
    if(atom.type==ATOM)
      return !strcmp(str,(char *)atom.ptr);
    return 0;
  }

  LLVMTypeRef compile_type(SExpr sexp){
    if(sexp.type==ATOM){
      if(atomeq(sexp,"i8"))
	return LLVMInt8Type();
      else if(atomeq(sexp,"i16"))
	return LLVMInt16Type();
      else if(atomeq(sexp,"i32"))
	return LLVMInt32Type();
      else if(atomeq(sexp,"i64"))
	return LLVMInt64Type();
      else if(atomeq(sexp,"f32"))
	return LLVMFloatType();
      else if(atomeq(sexp,"f64"))
	return LLVMDoubleType();
      else if(atomeq(sexp,"void"))
	return LLVMVoidType();
      return 0;
    }
}
  LLVMValueRef compile_value(LLVMBuilderRef builder,SExpr sexp){
    if(sexp.type==ATOM){
      // if atom is a number, return constant
      if(isdigit(*(char *)sexp.ptr))
	return LLVMConstInt(LLVMInt32Type(),atoi((char *)sexp.ptr),0);
      // if atom is a string, return constant
      else if(*(char *)sexp.ptr=='"')
	return LLVMConstString((char *)sexp.ptr+1,strlen((char *)sexp.ptr)-2,0);
      // if atom is a variable, return load instruction
      // get current function
      LLVMValueRef func=LLVMGetBasicBlockParent(LLVMGetInsertBlock(builder));
      // get module
      LLVMModuleRef mod=LLVMGetGlobalParent(func);
      // get value allocated in module by name
      LLVMValueRef val=get_var((char *)sexp.ptr);
      return LLVMBuildLoad(builder,val,(char *)sexp.ptr);

    }
  }



  void compile_instr(LLVMBuilderRef builder,SExpr sexp){
    printf("\ncompile_instr: ");
    print_sexpr(sexp);
    printf("\n");
    // get current module
    LLVMModuleRef module=LLVMGetGlobalParent(LLVMGetBasicBlockParent(LLVMGetInsertBlock(builder)));
     LLVMValueRef function =LLVMGetBasicBlockParent(LLVMGetInsertBlock(builder));
    if(sexp.type==LIST){
      SExpr *list=(SExpr *)sexp.ptr;
      if(atomeq(list[0],"ret")){
	printf("ret\n");
	LLVMValueRef ret=compile_value(builder,list[1]);
	LLVMBuildRet(builder,ret);
	printf("ret done\n");
      }else if(atomeq(list[0],"br")){
	// br <label> | br <cond> <label> <label>
	if(list[1].type==ATOM){
	  LLVMBuildBr(builder,get_var((char *)list[1].ptr));
	}else{
	  LLVMValueRef cond=compile_value(builder,list[1]);
	  LLVMBuildCondBr(builder,cond,get_var((char *)list[2].ptr),get_var((char *)list[3].ptr));
	}
      }else if(atomeq(list[0],"add")){
	// second elem is type
	LLVMTypeRef type=compile_type(list[1]);
	// third elem is name of variable to store result
	char *name=(char *)list[2].ptr;
	// fourth elem is first operand
	LLVMValueRef op1=compile_value(builder,list[3]);
	// fifth elem is second operand
	LLVMValueRef op2=compile_value(builder,list[4]);
	// build add instruction
	LLVMValueRef add=LLVMBuildAdd(builder,op1,op2,name);
	// store result in variable
	LLVMBuildStore(builder,add,LLVMBuildAlloca(builder,type,name));
      }else if(atomeq(list[0],"sub")){
	LLVMTypeRef type=compile_type(list[1]);
	char *name=(char *)list[2].ptr;
	LLVMValueRef op1=compile_value(builder,list[3]);
	LLVMValueRef op2=compile_value(builder,list[4]);
	LLVMValueRef sub=LLVMBuildSub(builder,op1,op2,name);
	LLVMBuildStore(builder,sub,LLVMBuildAlloca(builder,type,name));
      }else if(atomeq(list[0],"mul")){
	LLVMTypeRef type=compile_type(list[1]);
	char *name=(char *)list[2].ptr;
	LLVMValueRef op1=compile_value(builder,list[3]);
	LLVMValueRef op2=compile_value(builder,list[4]);
	LLVMValueRef mul=LLVMBuildMul(builder,op1,op2,name);
	LLVMBuildStore(builder,mul,LLVMBuildAlloca(builder,type,name));
      }else if(atomeq(list[0],"div")){
	LLVMTypeRef type=compile_type(list[1]);
	char *name=(char *)list[2].ptr;
	LLVMValueRef op1=compile_value(builder,list[3]);
	LLVMValueRef op2=compile_value(builder,list[4]);
	LLVMValueRef div=LLVMBuildSDiv(builder,op1,op2,name);
	LLVMBuildStore(builder,div,LLVMBuildAlloca(builder,type,name));
      }else if(atomeq(list[0],"call")){
	// second elem is name of variable to store result
	char *name=(char *)list[1].ptr;
	// third elem is name of function to call
	char *fname=(char *)list[2].ptr;
	// rest of list is arguments
	size_t n_args=0;
	for(SExpr *i=list+3;i->ptr;i++)
	  n_args++;
	LLVMValueRef *args=malloc(n_args*sizeof(LLVMValueRef));
	for(size_t i=0;i<n_args;i++)
	  args[i]=compile_value(builder,list[i+2]);
	LLVMValueRef call=LLVMBuildCall(builder,LLVMGetNamedFunction(module,fname),args,n_args,(char *)list[1].ptr);
	LLVMBuildStore(builder,call,LLVMBuildAlloca(builder,LLVMTypeOf(call),name));
      }else if(atomeq(list[0],"load")){
	// second elem is name of variable to store result
	char *rname=(char *)list[1].ptr;
	// third elem is name of variable to load
	char *name=(char *)list[2].ptr;
	LLVMValueRef load=LLVMBuildLoad(builder,get_var(name),name);
	LLVMBuildStore(builder,load,LLVMBuildAlloca(builder,LLVMTypeOf(load),rname));
      }else if(atomeq(list[0],"store")){
	// second elem is name of variable to store
	char *name=(char *)list[1].ptr;
	// third elem is value to store
	LLVMValueRef value=compile_value(builder,list[2]);
	LLVMBuildStore(builder,value,get_var(name));
      }
    }
  }

  void compile_block(LLVMValueRef function,SExpr sexp){
    if(sexp.type==LIST){
      SExpr *list=(SExpr *)sexp.ptr;
      if(atomeq(list[0],"block")){
	// create builder
	LLVMBuilderRef builder=LLVMCreateBuilder();
	// second elem in list is name of block
	LLVMBasicBlockRef block=LLVMAppendBasicBlock(function,(char *)list[1].ptr);
	LLVMPositionBuilderAtEnd(builder,block);
	print_sexpr(sexp);
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
	// signatures is list of ((name type) ...)
	//first signature is return type and name of function
	SExpr *namesig=(SExpr *)signatures[0].ptr;
	char *name=(char *)namesig[0].ptr;
	LLVMTypeRef rettype=compile_type(namesig[1]);
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