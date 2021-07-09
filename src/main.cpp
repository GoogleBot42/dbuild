#include <cpp-peglib/peglib.h>
#include <assert.h>
#include <iostream>
#include <cassert>
#include <unordered_map>

#include "context.h"
#include "util.h"

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

std::any readIdentifier(Context &cxt, std::any val) {
  assert(val.type() == typeid(Identifier));
  return cxt.currentScope->getVar(cxt, any_cast<Identifier>(val).identifier);
}

/**
 * Returns the value of the input (reads the value if it is a refers to a variable)
 */
std::any getValue(Context& cxt, std::any input) {
  if (input.type() == typeid(Identifier))
    return readIdentifier(cxt, input);
  else
    return input;
}

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
    return Identifier(std::string(ast.token_to_string()));
  } else if (ast.name == "INTEGER") {
    // TODO parse hex, oct, and binary
    return ast.token_to_number<long>();
  } else if (ast.name == "BUILTINCALL" ) {
    auto identifier = any_cast<Identifier>(eval(cxt, *nodes[0]));
    auto args = Args::toArgs(cxt, eval(cxt, *nodes[1]));
    cxt.builtins.at(identifier.identifier).execute(cxt, args);
    // TODO: handle returns
    return Nil();
  } else if (ast.name == "STRINGLITERALSINGLE")
  {
    return ast.token_to_string();
  } else if (ast.name == "ExprList") {
    std::vector<std::any> exprs;
    for (int i=0; i<ast.nodes.size(); i++) {
      exprs.push_back(getValue(cxt, eval(cxt, *nodes[i])));
    }
    return exprs;
  } else if (ast.name == "AdditionExpr") {
    auto value = getValue(cxt, eval(cxt, *nodes[0]));
    for (int i=1; i<nodes.size(); i+=2) {
      int op = (*nodes[i]).choice;
      auto v2 = getValue(cxt, eval(cxt, *nodes[i+1]));
      // TODO: floats
      if (op == 0) {
        value = any_cast<long>(value) + any_cast<long>(v2);
      } else {
        value = any_cast<long>(value) - any_cast<long>(v2);
      }
    }
    return value;
  } else if (ast.name == "MultiplyExpr") {
    auto value = getValue(cxt, eval(cxt, *nodes[0]));
    for (int i=1; i<nodes.size(); i+=2) {
      int op = (*nodes[i]).choice;
      auto v2 = getValue(cxt, eval(cxt, *nodes[i+1]));
      // TODO: floats
      if (op == 0) {
        value = any_cast<long>(value) * any_cast<long>(v2);
      } else {
        value = any_cast<long>(value) / any_cast<long>(v2);
      }
    }
    return value;
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
    var a = "hello world";
    var b = 20 + 10 + 5 - 10 * 2 / 2;
    @print(a,b);
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