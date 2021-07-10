#include "util.h"

#include "context.h"

#include <vector>

Args Args::toArgs(Context& cxt, std::any input) {
  Args args;
  if (input.type() == typeid(std::vector<std::any>)) {
    args.args = std::any_cast<std::vector<std::any>>(input);
  } else {
    cxt.error("Unimplemented toArgs");
  }
  return args;
}

std::any Func::execute(Context& cxt, Args& args) {
  if (ast == nullptr) {
    return nativeFunction(cxt, args);
  } else {
    // TODO create scope
    std::any result;
    for (int i=0; i<args.args.size(); i++) {
      cxt.currentScope->writeVar(cxt, argNames[i].identifier, args.args[i]);
    }
    result = cxt.eval(*ast);
    // TODO tear down scope
    return result;
  }
}

std::any BuiltinFunctions::print(Context& cxt, Args& args) {
  for (int i=0; i<args.args.size(); i++) {
    std::any var = args.args[i];
    if (var.type() == typeid(long)) {
      std::cout << std::any_cast<long>(var) << '\t';
    } else if (var.type() == typeid(std::string)) {
      std::cout << std::any_cast<std::string>(var) << '\t';
    } else if (var.type() == typeid(bool)) {
      bool v = std::any_cast<bool>(var);
      if (v)
        std::cout << "true" << '\t';
      else
        std::cout << "false" << '\t';
    } else if (var.type() == typeid(Nil)) {
      std::cout << "nil" << '\t';
    } else {
      cxt.error("Attempt to print unsupported type");
    }
  }
  std::cout << std::endl;
  return Nil();
}