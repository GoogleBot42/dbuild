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

  Type type = TypeNil;
  TypeValues value;
  Context &cxt;

public:
  Value(Context &cxt, Nil nil);
  Value(Context &cxt, long int_);
  Value(Context &cxt, double float_);
  Value(Context &cxt, bool bool_);
  Value(Context &cxt, const std::string &string);
  Value(Context &cxt, const Func &function);
  Value(Context &cxt, const Args &args);
  Value(Context &cxt, const Identifier &identifier);

  Value(const Value &value);
  Value& operator=(const Value &rhs);
  ~Value();

  void cleanup();

  Value& operator=(Nil rhs);
  Value& operator=(long rhs);
  Value& operator=(double rhs);
  Value& operator=(bool rhs);
  Value& operator=(const std::string &rhs);
  Value& operator=(const Func &rhs);
  Value& operator=(const Args &rhs);
  Value& operator=(const Identifier &rhs);

  bool isNil() const;
  bool isInt() const;
  bool isFloat() const;
  bool isNumber() const;
  bool isBoolean() const;
  bool isString() const;
  bool isFunction() const;
  bool isArgs() const;
  bool isIdentifier() const;

  void assertNil(Context &cxt) const;
  void assertInt(Context &cxt) const;
  void assertFloat(Context &cxt) const;
  void assertNumber(Context &cxt) const;
  void assertBoolean(Context &cxt) const;
  void assertString(Context &cxt) const;
  void assertFunction(Context &cxt) const;
  void assertArgs(Context &cxt) const;
  void assertIdentifier(Context &cxt) const;

  Nil getNil(Context &cxt) const;
  long getInt(Context &cxt) const;
  double getFloat(Context &cxt) const;
  bool getBoolean(Context &cxt) const;
  std::string getString(Context &cxt) const;
  Func getFunction(Context &cxt) const;
  Args getArgs(Context &cxt) const;
  Identifier getIdentifier(Context &cxt) const;

  Value operator+(const Value &rhs) const;
  Value operator-(const Value &rhs) const;
  Value operator*(const Value &rhs) const;
  Value operator/(const Value &rhs) const;
  Value operator%(const Value &rhs) const;
  Value operator&(const Value &rhs) const;
  Value operator^(const Value &rhs) const;
  Value operator|(const Value &rhs) const;
  Value operator==(const Value &rhs) const;
  Value operator!=(const Value &rhs) const;
  Value operator<(const Value &rhs) const;
  Value operator>(const Value &rhs) const;
  Value operator<=(const Value &rhs) const;
  Value operator>=(const Value &rhs) const;
};