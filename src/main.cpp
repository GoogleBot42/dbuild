#include <cpp-peglib/peglib.h>
#include <assert.h>
#include <iostream>
#include <unordered_map>
#include <exception>

using peg::SemanticValues;
using std::any_cast;

int main_old(void) {
  // (2) Make a parser
  peg::parser parser(R"(
    # Grammar for Calculator...
    Additive    <- Multitive '+' Additive / Multitive
    Multitive   <- Primary '*' Multitive / Primary
    Primary     <- '(' Additive ')' / Number
    Number      <- < [0-9]+ >
    %whitespace <- [ \t]*
  )");

  assert(static_cast<bool>(parser) == true);

  // (3) Setup actions
  parser["Additive"] = [](const SemanticValues &vs) {
    switch (vs.choice()) {
    case 0: // "Multitive '+' Additive"
      return any_cast<int>(vs[0]) + any_cast<int>(vs[1]);
    default: // "Multitive"
      return any_cast<int>(vs[0]);
    }
  };

  parser["Multitive"] = [](const SemanticValues &vs) {
    switch (vs.choice()) {
    case 0: // "Primary '*' Multitive"
      return any_cast<int>(vs[0]) * any_cast<int>(vs[1]);
    default: // "Primary"
      return any_cast<int>(vs[0]);
    }
  };

  parser["Number"] = [](const SemanticValues &vs) {
    return vs.token_to_number<int>();
  };

  // (4) Parse
  parser.enable_packrat_parsing(); // Enable packrat parsing.

  int val;
  parser.parse(" (1 + 2) * 3 ", val);

  assert(val == 9);

  return 0;
}

class Func;
class Scope;
class Identifier;

// represents the 'nil' type
class Nil {
};

class Context {
public:
  std::unordered_map<std::string_view, Func> builtins;
  std::shared_ptr<Scope> currentScope;
public:
  void error(std::string_view msg) {
    std::cout << "Error! " << msg << std::endl;
  }
};

class Identifier {
public:
  std::string identifier;
  Identifier(std::string id) : identifier(id) {} 
};

// table, function, number, string
// enum Type {
//   Type_Table,
//   Type_Function,
//   Type_Number,
//   Type_String,
// };

// Scope - parent scope, local variables, return handler, continue handler, break handler
class Scope {
public:
  std::shared_ptr<Scope> parentScope;
  std::unordered_map<std::string_view, std::any&> vars;
  // std::function<bool(Context&,Args&)> returnHandler;
  // std::function<bool(Context&)> breakHandler;
  // std::function<bool(Context&)> continueHandler;

public:
  Scope(std::shared_ptr<Scope> parentScope) : parentScope(parentScope) {
  }

  bool varExists(Context& ctx, std::string_view name) {
    if (vars.find(name) == vars.end()) {
      if (parentScope.get() == nullptr) {
        return false;
      } else {
        return parentScope.get()->varExists(ctx, name);
      }
    } else {
      return true;
    }
  }

  /**
   * Looks up var and creates it in the current scope if it doesn't exist
   */
  std::any& getVar(Context& cxt, std::string_view name) {
    if (vars.find(name) == vars.end()) {
      if (varExists(cxt, name)) {
        return parentScope.get()->getVar(cxt, name);
      } else {
        // doesn't exist at all
        throw new std::exception();
      }
    }
    return vars.at(name);
  }

  void writeVar(Context& cxt, std::string_view name, std::any& var) {
    if (vars.find(name) == vars.end()) {
      if (varExists(cxt, name)) {
        parentScope.get()->writeVar(cxt, name, var);
      }
    }
    vars.insert_or_assign(name, var);
  }
};

/**
 * Arguments passed to a function and also results returned from a function
 */
class Args {
public:
  std::vector<std::any> args;
  static Args toArgs(Context& cxt, std::any input) {
    Args args;
    if (input.type() == typeid(Identifier)) {
      auto id = any_cast<Identifier>(input);
      auto value = cxt.currentScope->getVar(cxt, id.identifier);
      args.args.push_back(value);
    }
    return args;
  }
};

// Function - captured scope, ast ptr OR native function
class Func {
public:
  std::shared_ptr<Scope> capturedScope;
  // ast ptr
  std::function<void(Context&,Args&)> nativeFunction;

public:
  Func(std::function<void(Context&,Args&)> nativeFunction) 
    : capturedScope(nullptr), nativeFunction(nativeFunction) {}

  void execute(Context& cxt, Args& args) {
    // TODO implement AST executer
    nativeFunction(cxt, args);
  }
};

class BuiltinFunctions {
public:
  static void print(Context& cxt, Args& args) {
    for (int i=0; i<args.args.size(); i++) {
      std::any& var = args.args[i];
      if ( var.type() == typeid(long) ) {
        std::cout << any_cast<long>(var) << '\t';
      } else {
        cxt.error("Attempt to print unsupported type");
      }
    }
    std::cout << std::endl;
  }
};

std::any eval(Context &cxt, peg::Ast& ast) {
  const auto &nodes = ast.nodes;
  if (ast.name == "Root") {
    for (int i=0; i<ast.nodes.size(); i++) {
      eval(cxt, *nodes[i]);
    }
    return Nil();
  } else if (ast.name == "VarDecl") {
    auto identifier = any_cast<Identifier>(eval(cxt, *nodes[0]));
    auto value = eval(cxt, *nodes[1]);
    cxt.currentScope->writeVar(cxt, identifier.identifier, value);
    return Nil();
  } else if (ast.name == "IDENTIFIER" || ast.name == "BUILTINIDENTIFIER") {
    return Identifier(ast.token_to_string());
  } else if (ast.name == "INTEGER") {
    // TODO parse hex, oct, and binary
    return ast.token_to_number<long>();
  } else if (ast.name == "BUILTINCALL" ) {
    auto identifier = any_cast<Identifier>(eval(cxt, *nodes[0]));
    auto args = Args::toArgs(cxt, eval(cxt, *nodes[1]));
    cxt.builtins.at(identifier.identifier).execute(cxt, args);
  }

  std::cout << "AST Name: " << ast.name << std::endl;
  for (int i=0; i<ast.nodes.size(); i++) {
    std::cout << "AST child: " << ast.name << " : ";
    eval(cxt, *nodes[i]);
  }
  return Nil();
}

int main(void) {
  peg::parser parser(
    #include "grammar.peg"
  );
  std::string s = R"(
    var a = 10;
    @print(a);
  )";

  parser.enable_ast();
  std::shared_ptr<peg::Ast> ast;

  if (parser.parse(s, ast)) {
    ast = parser.optimize_ast(ast);
    std::cout << ast_to_s(ast) << std::endl;

    Context cxt;

    // init new scope
    std::shared_ptr<Scope> scope(new Scope(nullptr));

    // init builtins
    cxt.builtins.emplace("print", Func(BuiltinFunctions::print));
    cxt.currentScope = scope;

    eval(cxt, *ast);

    return 0;
  }

  std::cout << "syntax error..." << std::endl;

  return 1;
}