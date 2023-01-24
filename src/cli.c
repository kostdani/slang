/* CLI */

/* Command line interface only supports dumping the LLVM IR. */


/* [[file:../README.org::*CLI][CLI:1]] */
#include "compiler.c"
#include "parser.c"

int cli(int argc,char **argv){
    char *infile=0;
    char *outfile=0;
    int dump=0;
    for(int i=1;i<argc;i++){
        if(!strcmp(argv[i],"-o")){
            outfile=argv[++i];
        }else if(!strcmp(argv[i],"-d")){
            dump=1;
        }else{
            infile=argv[i];
        }
    }
    if(!infile){
        printf("usage: %s [args] <file>\n",argv[0]);
        printf("supported args: \n"
               "-o <file> : output file\n"
               "-d : dump llvm ir\n");
        return 1;
    }
    // read file
    FILE *f=fopen(infile,"r");
    if(!f){
        printf("could not open file %s\n",infile);
        return 1;
    }
    fseek(f,0,SEEK_END);
    size_t size=ftell(f);
    fseek(f,0,SEEK_SET);
    char *buf=malloc(size+1);
    fread(buf,1,size,f);
    buf[size]=0;
    fclose(f);
    // parse file
    SExpr sexp=parse(buf);
    free(buf);
    // compile
    LLVMModuleRef module=compile_module(sexp);
    free_sexpr(sexp);
    // dump ir
    if(dump){
        LLVMDumpModule(module);
    }
    // compile to file
    if(outfile){
        char *error=0;
        LLVMVerifyModule(module,LLVMReturnStatusAction,&error);
        if(error && strcmp(error,"")){
            printf("error1: %s\n",error);
            return 1;
        }
        LLVMInitializeNativeTarget();
        LLVMTargetRef target;
        char *triple=LLVMGetDefaultTargetTriple();
        LLVMGetTargetFromTriple(triple,&target,&error);
        if(error && strcmp(error,"")){
            printf("error2: %s\n",error);
            return 1;
        }
        char *cpu=LLVMGetHostCPUName();
        char *features=LLVMGetHostCPUFeatures();
        LLVMTargetMachineRef machine=LLVMCreateTargetMachine(target,triple,cpu,features,LLVMCodeGenLevelDefault,LLVMRelocDefault,LLVMCodeModelDefault);
        LLVMTargetDataRef data=LLVMCreateTargetDataLayout(machine);
        LLVMSetModuleDataLayout(module,data);
        LLVMDisposeTargetData(data);
        LLVMDisposeTargetMachine(machine);
        free(cpu);
        free(features);
        free(triple);
        if(0){
            LLVMInitializeNativeTarget();
            LLVMInitializeNativeAsmPrinter();
            LLVMInitializeNativeAsmParser();
            LLVMExecutionEngineRef engine;
            if(LLVMCreateJITCompilerForModule(&engine,module,0,&error)){
                printf("error: %s\n",error);
                return 1;
            }
            LLVMRunFunction(engine,LLVMGetNamedFunction(module,"main"),0,0);
            LLVMDisposeExecutionEngine(engine);
        }else{
            LLVMTargetMachineEmitToFile(machine,module,outfile,LLVMObjectFile,0);
        }
    }
    // cleanup
    LLVMDisposeModule(module);
    free_env();
    return 0;

}
/* CLI:1 ends here */
