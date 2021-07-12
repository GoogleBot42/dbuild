#include "context.h"

#include <cpp-peglib/peglib.h>
#include <iostream>

void Context::error(std::string_view msg) {
    std::cout << "Error! " << msg << std::endl;
}

Value Context::readIdentifier(Value val) {
  return currentScope->getVar(*this, val.getIdentifier(*this).identifier);
}

/**
 * Returns the value of the input (reads the value if it is a refers to a variable)
 */
Value Context::getValue(Value input) {
  if (input.isIdentifier())
    return readIdentifier(input);
  else
    return input;
}

Value Context::eval(peg::Ast& ast) {
  const auto &nodes = ast.nodes;
  Context &cxt = *this;
  if (ast.name == "Root") {
    for (int i=0; i<ast.nodes.size(); i++) {
      eval(*nodes[i]);
    }
    return Nil();
  } else if (ast.name == "VarDecl") {
    std::string identifier = eval(*nodes[0]).getIdentifier(cxt).identifier;
    Value value = eval(*nodes[1]);
    currentScope->writeVar(cxt, identifier, value);
    return Nil();
  } else if (ast.name == "IDENTIFIER" || ast.name == "BUILTINIDENTIFIER") {
    return Identifier(std::string(ast.token_to_string()));
  } else if (ast.name == "INTEGER") {
    // TODO parse hex, oct, and binary
    return ast.token_to_number<long>();
  } else if (ast.name == "BUILTINCALL" ) {
    std::string identifier = eval(*nodes[0]).getIdentifier(cxt).identifier;
    Args args = eval(*nodes[1]).getArgs(cxt);
    return builtins.at(identifier).execute(cxt, args);
  } else if (ast.name == "STRINGLITERALSINGLE")
  {
    return ast.token_to_string();
  } else if (ast.name == "ExprList") {
    Args exprs;
    for (int i=0; i<ast.nodes.size(); i++) {
      exprs.args.push_back(getValue(eval(*nodes[i])));
    }
    return exprs;
  } else if (ast.name == "AdditionExpr") {
    Value value = getValue(eval(*nodes[0]));
    for (int i=1; i<nodes.size(); i+=2) {
      int op = (*nodes[i]).choice;
      Value v2 = getValue(eval(*nodes[i+1]));
      // TODO: floats
      if (op == 0) {
        value = value.getInt(cxt) + v2.getInt(cxt);
      } else {
        value = value.getInt(cxt) - v2.getInt(cxt);
      }
    }
    return value;
  } else if (ast.name == "MultiplyExpr") {
    Value value = getValue(eval(*nodes[0]));
    for (int i=1; i<nodes.size(); i+=2) {
      int op = (*nodes[i]).choice;
      Value v2 = getValue(eval(*nodes[i+1]));
      // TODO: floats
      if (op == 0) {
        value = value.getInt(cxt) * v2.getInt(cxt);
      } else {
        value = value.getInt(cxt) / v2.getInt(cxt);
      }
    }
    return value;
  } else if (ast.name == "AssignExpr") {
    std::string name = eval(*nodes[0]).getIdentifier(cxt).identifier;
    // TODO arrays, dot operator, etc.
    auto v1 = currentScope->getVar(*this, name);
    int op = (*nodes[1]).choice;
    auto v2 = getValue(eval(*nodes[2]));
    Value result = Nil();
    // TODO: floats
    if (op == 0) {
      result = v1.getInt(cxt) * v2.getInt(cxt);
    } else if (op == 1) {
      result = v1.getInt(cxt) % v2.getInt(cxt);
    } else if (op == 2) {
      result = v1.getInt(cxt) + v2.getInt(cxt);
    } else if (op == 3) {
      result = v1.getInt(cxt) - v2.getInt(cxt);
    } else if (op == 4) {
      result = v1.getInt(cxt) & v2.getInt(cxt);
    } else if (op == 5) {
      result = v1.getInt(cxt) ^ v2.getInt(cxt);
    } else if (op == 6) {
      result = v1.getInt(cxt) | v2.getInt(cxt);
    } else if (op == 7) {
      result = v2;
    }
    currentScope->writeVar(*this, name, result);
    return result;
  } else if (ast.name == "PrimaryTypeExpr") {
    int op = ast.choice;
    if (op == 8) {
      return false;
    } else if (op == 9) {
      return Nil();
    } else if (op == 10) {
      return true;
    }
  } else if (ast.name == "CompareExpr") {
    Value v1 = getValue(eval(*nodes[0]));
    int op = (*nodes[1]).choice;
    Value v2 = getValue(eval(*nodes[2]));
    // TODO: floats, bool, etc.
    if (op == 0) {
      return (bool)(v1.getInt(cxt) == v2.getInt(cxt));
    } else if (op == 1) {
      return (bool)(v1.getInt(cxt) != v2.getInt(cxt));
    } else if (op == 2) {
      return (bool)(v1.getInt(cxt) < v2.getInt(cxt));
    } else if (op == 3) {
      return (bool)(v1.getInt(cxt) > v2.getInt(cxt));
    } else if (op == 4) {
      return (bool)(v1.getInt(cxt) <= v2.getInt(cxt));
    } else if (op == 5) {
      return (bool)(v1.getInt(cxt) >= v2.getInt(cxt));
    }
  } else if (ast.name == "IfStatement") {
    Value val = getValue(eval(*nodes[0]));
    if (val.getBoolean(cxt)) {
      return eval(*nodes[1]);
    } else {
      return eval(*nodes[2]);
    }
  } else if (ast.name == "Block") {
    // TODO create new scope
    Value result = Nil();
    for (int i=0; i<ast.nodes.size(); i++) {
      result = eval(*nodes[i]);
    }
    return result;
  } else if (ast.name == "WhileStatement") {
    Value result = Nil();
    while (getValue(eval(*nodes[0])).getBoolean(cxt)) {
      result = eval(*nodes[1]);
    }
    return result;
  } else if (ast.name == "Fn") {
    Args args = eval(*nodes[0]).getArgs(cxt);
    std::vector<Identifier> identifiers;
    for (int i=0; i<args.args.size(); i++) {
      identifiers.push_back(args.args[i].getIdentifier(cxt));
    }
    return Func(nodes[1], identifiers);
  } else if (ast.name == "ParamDeclList") {
    Args identifiers;
    for (int i=0; i<nodes.size(); i++) {
      identifiers.args.push_back(Value(eval(*nodes[i]).getIdentifier(cxt)));
    }
    return identifiers;
  } else if (ast.name == "SuffixExpr") {
    // TODO handle other PrimaryTypeExpr types
    Value val = getValue(eval(*nodes[0]));
    Func func = val.getFunction(cxt);
    for (int i=1; i<nodes.size(); i++) {
      Args args = eval(*nodes[i]).getArgs(cxt);
      return func.execute(*this, args);
    }
  }

  std::cout << "AST Name: " << ast.name << std::endl;
  for (int i=0; i<ast.nodes.size(); i++) {
    std::cout << "AST child: " << ast.name << " : ";
    eval(*nodes[i]);
  }
  return Nil();
}