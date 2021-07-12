#include "Scope.h"

#include <iostream>

bool Scope::varExists(Context& ctx, std::string name) {
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

Value& Scope::getVar(Context& cxt, std::string name) {
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

void Scope::writeVar(Context& cxt, std::string name, const Value& var) {
  if (vars.find(name) == vars.end()) {
    if (varExists(cxt, name)) {
      parentScope.get()->writeVar(cxt, name, var);
    }
  }
  vars.insert_or_assign(name, var);
}