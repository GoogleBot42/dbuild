#include "types.h"

#include "context.h"

Value Func::execute(Context& cxt, Args& args) {
  if (ast == nullptr) {
    return nativeFunction(cxt, args);
  } else {
    // TODO create scope
    Value result = Value(Nil());
    for (int i=0; i<args.args.size(); i++) {
      cxt.currentScope->writeVar(cxt, argNames[i].identifier, args.args[i]);
    }
    result = cxt.eval(*ast);
    // TODO tear down scope
    return result;
  }
}

Value::Value(Nil nil) : type(TypeNil) {
  value.nil = Nil();
}
Value::Value(long int_) : type(TypeInt) {
  value.int_ = int_;
}
Value::Value(double float_) : type(TypeFloat) {
  value.float_ = float_;
}
Value::Value(bool bool_) : type(TypeBoolean) {
  value.bool_ = bool_;
}
Value::Value(const std::string &string) : type(TypeString) {
  value.string = new std::string(string);
}
Value::Value(const Func &function) : type(TypeFunction) {
  value.function = new Func(function);
}
Value::Value(const Args &args) : type(TypeArgs) {
  value.args = new Args(args);
}
Value::Value(const Identifier &identifier) : type(TypeIdentifier) {
  value.identifier = new Identifier(identifier);
}

Value::Value(const Value &val) {
  if (val.isNil()) {
    *this = Nil();
  } else if (val.isInt()) {
    *this = val.value.int_;
  } else if (val.isFloat()) {
    *this = val.value.float_;
  } else if (val.isBoolean()) {
    *this = val.value.bool_;
  } else if (val.isString()) {
    *this = *val.value.string;
  } else if (val.isFunction()) {
    *this = *val.value.function;
  } else if (val.isArgs()) {
    *this = *val.value.args;
  } else if (val.isIdentifier()) {
    *this = *val.value.identifier;
  }
}

Value& Value::operator=(const Value &rhs) {
  cleanup();
  if (rhs.isNil()) {
    *this = Nil();
  } else if (rhs.isInt()) {
    *this = rhs.value.int_;
  } else if (rhs.isFloat()) {
    *this = rhs.value.float_;
  } else if (rhs.isBoolean()) {
    *this = rhs.value.bool_;
  } else if (rhs.isString()) {
    *this = *rhs.value.string;
  } else if (rhs.isFunction()) {
    *this = *rhs.value.function;
  } else if (rhs.isArgs()) {
    *this = *rhs.value.args;
  } else if (rhs.isIdentifier()) {
    *this = *rhs.value.identifier;
  }
  return *this;
}

Value::~Value() {
  cleanup();
}

void Value::cleanup() {
  if (isString()) {
    delete value.string;
  } else if (isFunction()) {
    delete value.function;
  } else if (isArgs()) {
    delete value.args;
  } else if (isIdentifier()) {
    delete value.identifier;
  }
  type = TypeNil;
}

Value& Value::operator=(Nil rhs) {
  cleanup();
  type = TypeNil;
  value.nil = rhs;
  return *this;
}
Value& Value::operator=(long rhs) {
  cleanup();
  type = TypeInt;
  value.int_ = rhs;
  return *this;
}
Value& Value::operator=(double rhs) {
  cleanup();
  type = TypeFloat;
  value.float_ = rhs;
  return *this;
}
Value& Value::operator=(bool rhs) {
  cleanup();
  type = TypeBoolean;
  value.bool_ = rhs;
  return *this;
}
Value& Value::operator=(const std::string &rhs) {
  cleanup();
  type = TypeString;
  value.string = new std::string(rhs);
  return *this;
}
Value& Value::operator=(const Func &rhs) {
  cleanup();
  type = TypeFunction;
  value.function = new Func(rhs);
  return *this;
}
Value& Value::operator=(const Args &rhs) {
  cleanup();
  type = TypeArgs;
  value.args = new Args(rhs);
  return *this;
}
Value& Value::operator=(const Identifier &rhs) {
  cleanup();
  type = TypeIdentifier;
  value.identifier = new Identifier(rhs);
  return *this;
}

bool Value::isNil() const {
  return type == TypeNil;
}
bool Value::isInt() const {
  return type == TypeInt;
}
bool Value::isFloat() const {
  return type == TypeFloat;
}
bool Value::isNumber() const {
  return isInt() || isFloat();
}
bool Value::isBoolean() const {
  return type == TypeBoolean;
}
bool Value::isString() const {
  return type == TypeString;
}
bool Value::isFunction() const {
  return type == TypeFunction;
}
bool Value::isArgs() const {
  return type == TypeArgs;
}
bool Value::isIdentifier() const {
  return type == TypeIdentifier;
}

void Value::assertNil(Context &cxt) {
  if (!isNil()) {
    cxt.error("value is not nil");
  }
}
void Value::assertInt(Context &cxt) {
  if (!isInt()) {
    cxt.error("value is not an integer");
  }
}
void Value::assertFloat(Context &cxt) {
  if (!isFloat()) {
    cxt.error("value is not a float");
  }
}
void Value::assertNumber(Context &cxt) {
  if (!isNumber()) {
    cxt.error("value is not a number");
  }
}
void Value::assertBoolean(Context &cxt) {
  if (!isBoolean()) {
    cxt.error("value is not a boolean");
  }
}
void Value::assertString(Context &cxt) {
  if (!isString()) {
    cxt.error("value is not a string");
  }
}
void Value::assertFunction(Context &cxt) {
  if (!isFunction()) {
    cxt.error("value is not a function");
  }
}
void Value::assertArgs(Context &cxt) {
  if (!isArgs()) {
    cxt.error("value is not arguments");
  }
}
void Value::assertIdentifier(Context &cxt) {
  if (!isIdentifier()) {
    cxt.error("value is not identifier");
  }
}

Nil Value::getNil(Context &cxt) {
  assertNil(cxt);
  return value.nil;
}
long Value::getInt(Context &cxt) {
  assertNumber(cxt);
  if (isFloat())
    return (long)value.float_;
  else
    return value.int_;
}
double Value::getFloat(Context &cxt) {
  assertNumber(cxt);
  if (isInt())
    return (double)value.int_;
  else
    return value.float_;
}
bool Value::getBoolean(Context &cxt) {
  assertBoolean(cxt);
  return value.bool_;
}
std::string Value::getString(Context &cxt) {
  assertString(cxt);
  return *value.string;
}
Func Value::getFunction(Context &cxt) {
  assertFunction(cxt);
  return *value.function;
}
Args Value::getArgs(Context &cxt) {
  assertArgs(cxt);
  return *value.args;
}
Identifier Value::getIdentifier(Context &cxt) {
  assertIdentifier(cxt);
  return *value.identifier;
}