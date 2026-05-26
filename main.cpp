#include <alloca.h>
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cwctype>
#include <fstream>
#include <istream>
#include <mlir/IR/MLIRContext.h>
#include <mlir/IR/BuiltinOps.h>
#include "mlir/IR/Builders.h"
#include <mlir/IR/Location.h>
#include <mlir/IR/OperationSupport.h>
#include <llvm/Support/raw_ostream.h>
#include <iostream>
#include <stack>
#include <string>
#include <vector>


static inline bool isnewline(char c) { return c == '\n' || c == '\r'; }
static inline bool isquote(char c) { return c == '"' || c == '|'; }
static inline bool isleft(char c) { return c == '(' || c == '[' || c == '{';}
static inline bool isright(char c) { return c == ')' || c == ']' || c == '}';}
static inline bool isprefix(char c) { return c == '\'' || c == '`' || c == ',';}
static inline bool issyntactic(char c) { return isleft(c) || isright(c) || isquote(c) || isspace(c) || isprefix(c);}

class Reader {
  std::string src_name;
  std::istream& src_stream;
  size_t line = 1, column = 1;

  void incLine() {
    column = 1;
    line++;
  }
  void incColumn() {
    column++;
  }

  mlir::MLIRContext context;
  mlir::ModuleOp module = mlir::ModuleOp::create(getLoc());
  mlir::Block *block = module.getBody();
  // mlir::Operation* operation = 0;
  char c;
  std::string accum;
  mlir::OperationState opState= mlir::OperationState(getLoc(), "sexp");
public:
  Reader(std::string name, std::istream &is) : src_name(name), src_stream(is) {}

  mlir::Location getLoc() {
    auto attr = mlir::StringAttr::get(&context, src_name);
    auto loc = mlir::FileLineColLoc::get(attr, line, column);
    return loc;
  }


  char read_char() {
    src_stream >> c;
    if (isnewline(c))
      incLine();
    else
      incColumn();
    return c;
  }
  std::string read_line(bool (*isdelim)(char)) {
    std::string res;
    for (res=""; !isdelim(read_char()); res+=c);
    return res;
  }
  std::string read_line(char delim) {
    std::string res;
    for (res = ""; read_char() && c != delim; res += c) {
      if (c == '\\') {
        switch (read_char()) {
	case 'n': c='\n'; break;
	case 'r': c='\r'; break;
	case 't': c='\t'; break;
	case 'b': c='\b'; break;
	case 'f': c='\f'; break;
	case 'v': c='\v'; break;
	case '0': c='\0'; break;
	}          
      }        
    }      
    return res;
  }
  void skip_while(bool (*pred)(char)) {
    while(pred(read_char()));
  }
  void read_atom() {
    std::string val;
    char ch = c;
    if (isquote(ch)) {     
      val=read_line(ch);
      o->setAttr("quotation_mark", mlir::StringAttr::get(&context, llvm::StringRef(&ch,1)));
    } else {
      src_stream.unget();
      c=' ';
      val=read_line(issyntactic);
    }
    o->setAttr("atom_str", mlir::StringAttr::get(&context, val));
  }
  void read
};  


mlir::ModuleOp read_syntax(std::istream is) {
  mlir::MLIRContext context;
  auto attr = mlir::StringAttr::get(&context, "idk");
  auto loc = mlir::FileLineColLoc::get(attr, 1, 1);
  auto module = mlir::ModuleOp::create(loc);
  mlir::Block * block = module.getBody();
  mlir::Operation* operation = 0; module.getOperation();
  mlir::OpBuilder builder(&context);
  builder.setInsertionPointToEnd(block);

  
  std::string attr_val="";  
  while (is >> c) {
    if (isnewline(c)) {
      line++; column=1;
    } else {
      column++;
    }

    if (!operation) {
      builder.setInsertionPointToStart(block);
      if (isspace(c))
        continue;
      if (isquote(c)) {
        opName = "sexp.atom";
	
        mlir::OperationState sexpQuoteState(loc, "sexp.atom");
	sexpQuoteState.addAttribute("quotation mark", mlir::StringAttr::get(&context, llvm::StringRef(&c,1)));
        operation = builder.create(sexpQuoteState);
        // operation->setAttr(
	str="";
	continue;
      }
      if (isleft(c)) {
	opName="sexp.list";
        mlir::OperationState sexpQuoteState(loc, "sexp.atom");
	auto attr = mlir::StringAttr::get(&context, llvm::StringRef(&c,1));
	sexpQuoteState.addAttribute("left_bracket", attr);
        mlir::Operation *sexpQuoteOp = builder.create(sexpQuoteState);
	continue;
      }        
      builder.setInsertionPointToStart(block);
      mlir::OperationState sexpListState(loc, "sexp.quote");
      sexpListState.regions.reserve(1);          
      sexpListState.addRegion();                 
      mlir::Operation* sexpListOp = builder.create(sexpListState);
    }      
      
  }    
}
static  std::string unknown_filename="*unknown*";
static  std::string stdin_filename = "*stdin*";

static inline bool is_newline(char c){return c=='\n'||c=='\r';}

class Parser {
  llvm::Twine &file_p;
  std::istream &is_p;
  static mlir::MLIRContext ctx_p;
  char peek;
  mlir::Block *curblock;
  mlir::Operation *curop;
  

  struct nesting_t {
    char open, close;
    mlir::Block *block;
  };
  std::stack<struct nesting_t> depth;
  size_t line,column;//,depth=1;  
  inline size_t getLine(){return line;}
  inline size_t getColumn(){return column;}
  inline size_t getDepth(){return depth.size();}
  inline mlir::StringAttr getFilenameAttr(){return mlir::StringAttr::get(&ctx_p, file_p);}
  mlir::Location getLocation(){return mlir::FileLineColLoc::get(getFilenameAttr(), getLine(), getColumn());}
  
  inline void incColumn(){ ++column;}
  inline void incLine(){column=1; ++line;}
  inline void incDepth(char close = 0) {
    
    //if (getDepth()==0)
    auto inner = depth.top();
    mlir::Block *b = inner.block;
    mlir::OpBuilder builder(&ctx_p);
    builder.setInsertionPointToEnd(b);
    depth.emplace(peek,close); }
  inline void decDepth() { auto t = depth.top(); }

  mlir::ModuleOp module;  
public:
  Parser(std::string &file_a, std::istream &is_a)
      : file_p(*new llvm::Twine(file_a)), is_p(is_a), line(1), column(1),
        module(mlir::ModuleOp::create(getLocation())), depth() {
    depth.emplace(0,module.getBody());
    depth.top().builder.setInsertionPointToStart(module.getBody()); 
  }
  Parser( std::string &file_a)
    : Parser(file_a,*new std::ifstream(file_a)) {}
  Parser(std::istream &is_a)
    : Parser(unknown_filename,is_a) {}
  Parser() : Parser(stdin_filename, std::cin) {}

  void consume_whitespace() {
    char c;
    while((c = is_p.peek())){
      if (c1 == '\n' || c1 == '\r') {
	is_p.get();
	char c2 = is_p.peek();
	if (c1 != c2 && (c2 == '\n' || c2 == '\r'))
	  is_p.get();        
	y++;
      }
    }
  }   
  bool is_quote(char c) { return c == '"' || c == '|';}
  
  mlir::ModuleOp read_syntax() {
    char c;
    size_t line = 1, column = 1;
    char quoting_mode=0;
    while (is_p >> c) {
      if (is_nl(c))
        line++;
      else
        column++;
      
      if (is_cr(c))
	column = 1;
      
      if (c=='\n'){
        line++;
	column=1;
      } else {
	column++;
      }

      if(c=='"') quoting_mode = c;

      
      if (iswspace(c)) {
	
      }      
  }    
} ; 
  




mlir::Operation *parse(std::istream &is) {
    mlir::MLIRContext context;  
    context.allowUnregisteredDialects(true);
    mlir::OpBuilder builder(&context);
    mlir::Location loc = mlir::UnknownLoc::get(&context);
    mlir::ModuleOp module = mlir::ModuleOp::create(loc);
    builder.setInsertionPointToStart(module.getBody());

    
    
  //std::cin >> std::noskipws;
    char c;
    std::string tok;
    depth d = (depth)alloca(sizeof(struct depth_t));    
    struct nesting_t * new_nesting = alloca(sizeof(struct nesting_t));
    

    while (is >> c) {
      if (std::isspace(c)) {
	continue;
      }else if (c == '(') {
	
      } else if (c == ')') {
            flush();
            Node* finished = cur;
            if (!st.empty()) {
                cur = st.back();
                st.pop_back();
                cur->list.push_back(finished);
            } else {
                cur = finished;
            }
        } else if (isspace(c)) {
            flush();
        } else {
            tok.push_back(c);
        }
    }

    flush();
    return cur;
}

int main() {
    mlir::MLIRContext context;  
    // Allow unregistered dialects to use custom operation names
    context.allowUnregisteredDialects(true);

    mlir::OpBuilder builder(&context);
    mlir::Location loc = mlir::UnknownLoc::get(&context);

    // Create a top-level module
    mlir::ModuleOp module = mlir::ModuleOp::create(loc);
    builder.setInsertionPointToStart(module.getBody());

    // Step 1: Create the outer operation "foo.foo" with a single region
    mlir::OperationState fooFooState(loc, "foo.foo");
    fooFooState.regions.reserve(1);          // Reserve one region
    fooFooState.addRegion();                 // Add an empty region

    mlir::Operation* fooFooOp = builder.create(fooFooState);

    // Step 2: Go into the region of foo.foo and create the inner op "foo.bar"
    mlir::Region& region = fooFooOp->getRegion(0);
    region.emplaceBlock();                   // Create a block inside the region
    mlir::OpBuilder innerBuilder(&context);
    innerBuilder.setInsertionPointToStart(&region.front());

    mlir::OperationState fooBarState(loc, "foo.bar");
    mlir::Operation *fooBarOp = innerBuilder.create(fooBarState);


    mlir::OperationState fooBazState(loc, "foo.baz");
    mlir::Operation* fooBazOp = builder.create(fooBazState);    

    // (Optional) you can also add results/operands/attributes as needed

    // Print the entire module
    module.print(llvm::outs());
    llvm::outs() << "\n";

    return 0;
}
