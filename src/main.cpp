#include <cpp-peglib/peglib.h>
#include <assert.h>
#include <iostream>
#include <cassert>
#include <unordered_map>

#include "context.h"
#include "util.h"

int main(void) {
  peg::parser parser(
    #include "grammar.peg"
  );
  std::string s = R"(
    var a = "hello world";
    var b = 20 + 10 + 5 - 10 * 2 / 2;
    b *= 2;
    @print("hello world");
    @print(a,b);
    b = 2;
    @print(b);
    a = b != 2;
    @print(a);
    if (a) {
      @print("yay");
    } else {
      @print("neigh");
    }
    a = 10;
    while(a > 0) {
      a -= 1;
      @print(a);
    }
    var hello = fn(abc) {
      @print(abc);
    };
    hello("world");
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

    cxt.eval(*ast);

    return 0;
  }

  std::cout << "syntax error..." << std::endl;

  return 1;
}