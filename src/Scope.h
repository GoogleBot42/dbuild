#pragma once

#include "types.h"

#include <unordered_map>
#include <memory>
#include <string>

class Context;

// Scope - parent scope, local variables, return handler, continue handler, break handler
class Scope {
public:
  std::shared_ptr<Scope> parentScope;
  std::unordered_map<std::string, Value> vars;
  // std::function<bool(Context&,Args&)> returnHandler;
  // std::function<bool(Context&)> breakHandler;
  // std::function<bool(Context&)> continueHandler;

public:
  Scope(std::shared_ptr<Scope> parentScope) : parentScope(parentScope) {
  }

  bool varExists(Context& ctx, std::string name);

  /**
   * Looks up var and creates it in the current scope if it doesn't exist
   */
  Value& getVar(Context& cxt, std::string name);

  void writeVar(Context& cxt, std::string name, const Value& var);
};