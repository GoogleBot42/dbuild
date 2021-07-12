#include "types.h"

#include "context.h"

// Args Args::toArgs(Context& cxt, Value input) {
//   Args args;
//   if (input.type() == typeid(std::vector<std::any>)) {
//     args.args = std::any_cast<std::vector<std::any>>(input);
//   } else {
//     cxt.error("Unimplemented toArgs");
//   }
//   return args;
// }

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
  type = val.type;
  if (isNil()) {
    value.nil = Nil();
  } else if (isInt()) {
    value.int_ = val.value.int_;
  } else if (isFloat()) {
    value.float_ = val.value.float_;
  } else if (isBoolean()) {
    value.bool_ = val.value.bool_;
  } else if (isString()) {
    value.string = new std::string(*val.value.string);
  } else if (isFunction()) {
    value.function = new Func(*val.value.function);
  } else if (isArgs()) {
    value.args = new Args(*val.value.args);
  } else if (isIdentifier()) {
    value.identifier = new Identifier(*val.value.identifier);
  }
}

Value& Value::operator=(const Value &rhs) {
  type = rhs.type;
  if (isNil()) {
    value.nil = Nil();
  } else if (isInt()) {
    value.int_ = rhs.value.int_;
  } else if (isFloat()) {
    value.float_ = rhs.value.float_;
  } else if (isBoolean()) {
    value.bool_ = rhs.value.bool_;
  } else if (isString()) {
    value.string = new std::string(*rhs.value.string);
  } else if (isFunction()) {
    value.function = new Func(*rhs.value.function);
  } else if (isArgs()) {
    value.args = new Args(*rhs.value.args);
  } else if (isIdentifier()) {
    value.identifier = new Identifier(*rhs.value.identifier);
  }
  return *this;
}

Value::~Value() {
  if (isString()) {
    delete value.string;
  } else if (isFunction()) {
    delete value.function;
  } else if (isArgs()) {
    delete value.args;
  } else if (isIdentifier()) {
    delete value.identifier;
  }
}

bool Value::isNil() {
  return type == TypeNil;
}
bool Value::isInt() {
  return type == TypeInt;
}
bool Value::isFloat() {
  return type == TypeFloat;
}
bool Value::isNumber() {
  return isInt() || isFloat();
}
bool Value::isBoolean() {
  return type == TypeBoolean;
}
bool Value::isString() {
  return type == TypeString;
}
bool Value::isFunction() {
  return type == TypeFunction;
}
bool Value::isArgs() {
  return type == TypeArgs;
}
bool Value::isIdentifier() {
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