#pragma once

#include <memory>
#include <vector>
#include <cpp-peglib/peglib.h>

class Context;
class Identifier;
class Value;
class Scope;

/**
 * Arguments passed to a function and also results returned from a function
 */
class Args {
public:
  std::vector<Value> args;
  // static Args toArgs(Context& cxt, Value input);
};

// represents the 'nil' type
class Nil {
};

class Identifier {
public:
  std::string identifier;
  Identifier(std::string id) : identifier(id) {} 
};

// Function - captured scope, ast ptr OR native function
class Func {
public:
  std::shared_ptr<Scope> capturedScope;
  
  std::shared_ptr<peg::Ast> ast = nullptr;
  std::vector<Identifier> argNames;

  std::function<Value(Context&,Args&)> nativeFunction;

public:
  /**
   * Create a native function
   */
  Func(std::function<Value(Context&,Args&)> nativeFunction) 
    : capturedScope(nullptr), nativeFunction(nativeFunction) {}

  /**
   * Create an interpreted function
   */
  Func(std::shared_ptr<peg::Ast> ast, std::vector<Identifier> argNames)
    : ast(ast), argNames(argNames) {}

  Value execute(Context& cxt, Args& args);
};

class Value {
protected:
  enum Type {
    TypeNil,
    TypeInt,
    TypeFloat,
    TypeBoolean,
    TypeString,
    TypeFunction,

    // Internal types
    TypeArgs,
    TypeIdentifier,
  };
  union TypeValues {
    Nil nil;
    long int_;
    double float_;
    bool bool_;
    std::string *string;
    Func *function;
    Args *args;
    Identifier *identifier;
  };

  Type type;
  TypeValues value;

public:
  Value(Nil nil);
  Value(long int_);
  Value(double float_);
  Value(bool bool_);
  Value(const std::string &string);
  Value(const Func &function);
  Value(const Args &args);
  Value(const Identifier &identifier);

  Value(const Value &value);
  Value& operator=(const Value &rhs);
  ~Value();

  bool isNil();
  bool isInt();
  bool isFloat();
  bool isNumber();
  bool isBoolean();
  bool isString();
  bool isFunction();
  bool isArgs();
  bool isIdentifier();

  void assertNil(Context &cxt);
  void assertInt(Context &cxt);
  void assertFloat(Context &cxt);
  void assertNumber(Context &cxt);
  void assertBoolean(Context &cxt);
  void assertString(Context &cxt);
  void assertFunction(Context &cxt);
  void assertArgs(Context &cxt);
  void assertIdentifier(Context &cxt);

  Nil getNil(Context &cxt);
  long getInt(Context &cxt);
  double getFloat(Context &cxt);
  bool getBoolean(Context &cxt);
  std::string getString(Context &cxt);
  Func getFunction(Context &cxt);
  Args getArgs(Context &cxt);
  Identifier getIdentifier(Context &cxt);
};