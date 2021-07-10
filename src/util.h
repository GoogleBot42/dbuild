#pragma once

#include <cpp-peglib/peglib.h>

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
  
  std::shared_ptr<peg::Ast> ast = nullptr;
  std::vector<Identifier> argNames;

  std::function<std::any(Context&,Args&)> nativeFunction;

public:
  /**
   * Create a native function
   */
  Func(std::function<std::any(Context&,Args&)> nativeFunction) 
    : capturedScope(nullptr), nativeFunction(nativeFunction) {}

  /**
   * Create an interpreted function
   */
  Func(std::shared_ptr<peg::Ast> ast, std::vector<Identifier> argNames)
    : ast(ast), argNames(argNames) {}

  std::any execute(Context& cxt, Args& args);
};

class BuiltinFunctions {
public:
  static std::any print(Context& cxt, Args& args);
};