#include "util.h"

#include "context.h"
#include "types.h"

#include <vector>
#include <iostream>

Value BuiltinFunctions::print(Context& cxt, Args& args) {
  for (int i=0; i<args.args.size(); i++) {
    Value var = args.args[i];
    if (var.isInt()) {
      std::cout << var.getInt(cxt) << '\t';
    } else if (var.isString()) {
      std::cout << var.getString(cxt) << '\t';
    } else if (var.isBoolean()) {
      if (var.getBoolean(cxt))
        std::cout << "true" << '\t';
      else
        std::cout << "false" << '\t';
    } else if (var.isNil()) {
      std::cout << "nil" << '\t';
    } else {
      cxt.error("Attempt to print unsupported type");
    }
  }
  std::cout << std::endl;
  return Nil();
}