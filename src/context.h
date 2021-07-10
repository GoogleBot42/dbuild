#pragma once

#include <string>
#include <unordered_map>
#include <memory>

#include "Scope.h"
#include "util.h"

class Context {
public:
  std::unordered_map<std::string_view, Func> builtins;
  std::shared_ptr<Scope> currentScope;
public:
  void error(std::string_view msg);
  std::any eval(peg::Ast& ast);
protected:
  std::any readIdentifier(std::any val);
  std::any getValue(std::any input);
};