#include "util.h"

#include "context.h"

#include <vector>

Args Args::toArgs(Context& cxt, std::any input) {
  Args args;
  if (input.type() == typeid(Identifier)) {
    auto id = std::any_cast<Identifier>(input);
    auto value = cxt.currentScope->getVar(cxt, id.identifier);
    args.args.push_back(value);
  }
  return args;
}

void BuiltinFunctions::print(Context& cxt, Args& args) {
  for (int i=0; i<args.args.size(); i++) {
    std::any var = args.args[i];
    if ( var.type() == typeid(long) ) {
      std::cout << std::any_cast<long>(var) << '\t';
    } else {
      cxt.error("Attempt to print unsupported type");
    }
  }
  std::cout << std::endl;
}