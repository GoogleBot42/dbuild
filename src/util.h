#pragma once

#include <string>
#include <any>
#include <vector>
#include <functional>
#include <iostream>
#include <memory>

class Context;
class Scope;

// represents the 'nil' type
class Nil {
};

class Identifier {
public:
  std::string identifier;
  Identifier(std::string id) : identifier(id) {} 
};

/**
 * Arguments passed to a function and also results returned from a function
 */
class Args {
public:
  std::vector<std::any> args;
  static Args toArgs(Context& cxt, std::any input);
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
  static void print(Context& cxt, Args& args);
};