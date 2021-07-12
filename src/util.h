#pragma once

class Context;
class Value;
class Args;

class BuiltinFunctions {
public:
  static Value print(Context& cxt, Args& args);
};