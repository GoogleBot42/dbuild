#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <cpp-peglib/peglib.h>

#include "Scope.h"
#include "util.h"

class Context {
public:
  std::unordered_map<std::string_view, Func> builtins;
  std::shared_ptr<Scope> currentScope;
public:
  void error(std::string_view msg);
  Value eval(peg::Ast& ast);
protected:
  Value readIdentifier(Value val);
  Value getValue(Value input);
};