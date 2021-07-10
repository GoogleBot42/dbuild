#include "context.h"

#include <iostream>

void Context::error(std::string_view msg) {
    std::cout << "Error! " << msg << std::endl;
}

using std::any_cast;

std::any Context::readIdentifier(std::any val) {
  assert(val.type() == typeid(Identifier));
  return currentScope->getVar(*this, any_cast<Identifier>(val).identifier);
}

/**
 * Returns the value of the input (reads the value if it is a refers to a variable)
 */
std::any Context::getValue(std::any input) {
  if (input.type() == typeid(Identifier))
    return readIdentifier(input);
  else
    return input;
}

std::any Context::eval(peg::Ast& ast) {
  const auto &nodes = ast.nodes;
  if (ast.name == "Root") {
    for (int i=0; i<ast.nodes.size(); i++) {
      eval(*nodes[i]);
    }
    return Nil();
  } else if (ast.name == "VarDecl") {
    auto identifier = any_cast<Identifier>(eval(*nodes[0]));
    auto value = eval(*nodes[1]);
    currentScope->writeVar(*this, identifier.identifier, value);
    return Nil();
  } else if (ast.name == "IDENTIFIER" || ast.name == "BUILTINIDENTIFIER") {
    return Identifier(std::string(ast.token_to_string()));
  } else if (ast.name == "INTEGER") {
    // TODO parse hex, oct, and binary
    return ast.token_to_number<long>();
  } else if (ast.name == "BUILTINCALL" ) {
    auto identifier = any_cast<Identifier>(eval(*nodes[0]));
    auto args = Args::toArgs(*this, eval(*nodes[1]));
    builtins.at(identifier.identifier).execute(*this, args);
    // TODO: handle returns
    return Nil();
  } else if (ast.name == "STRINGLITERALSINGLE")
  {
    return ast.token_to_string();
  } else if (ast.name == "ExprList") {
    std::vector<std::any> exprs;
    for (int i=0; i<ast.nodes.size(); i++) {
      exprs.push_back(getValue(eval(*nodes[i])));
    }
    return exprs;
  } else if (ast.name == "AdditionExpr") {
    auto value = getValue(eval(*nodes[0]));
    for (int i=1; i<nodes.size(); i+=2) {
      int op = (*nodes[i]).choice;
      auto v2 = getValue(eval(*nodes[i+1]));
      // TODO: floats
      if (op == 0) {
        value = any_cast<long>(value) + any_cast<long>(v2);
      } else {
        value = any_cast<long>(value) - any_cast<long>(v2);
      }
    }
    return value;
  } else if (ast.name == "MultiplyExpr") {
    auto value = getValue(eval(*nodes[0]));
    for (int i=1; i<nodes.size(); i+=2) {
      int op = (*nodes[i]).choice;
      auto v2 = getValue(eval(*nodes[i+1]));
      // TODO: floats
      if (op == 0) {
        value = any_cast<long>(value) * any_cast<long>(v2);
      } else {
        value = any_cast<long>(value) / any_cast<long>(v2);
      }
    }
    return value;
  } else if (ast.name == "AssignExpr") {
    std::string name = any_cast<Identifier>(eval(*nodes[0])).identifier;
    // TODO arrays, dot operator, etc.
    auto v1 = currentScope->getVar(*this, name);
    int op = (*nodes[1]).choice;
    auto v2 = getValue(eval(*nodes[2]));
    std::any result;
    // TODO: floats
    if (op == 0) {
      result = any_cast<long>(v1) * any_cast<long>(v2);
    } else if (op == 1) {
      result = any_cast<long>(v1) % any_cast<long>(v2);
    } else if (op == 2) {
      result = any_cast<long>(v1) + any_cast<long>(v2);
    } else if (op == 3) {
      result = any_cast<long>(v1) - any_cast<long>(v2);
    } else if (op == 4) {
      result = any_cast<long>(v1) & any_cast<long>(v2);
    } else if (op == 5) {
      result = any_cast<long>(v1) ^ any_cast<long>(v2);
    } else if (op == 6) {
      result = any_cast<long>(v1) | any_cast<long>(v2);
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
    auto v1 = getValue(eval(*nodes[0]));
    int op = (*nodes[1]).choice;
    auto v2 = getValue(eval(*nodes[2]));
    // TODO: floats, bool, etc.
    if (op == 0) {
      return (bool)(any_cast<long>(v1) == any_cast<long>(v2));
    } else if (op == 1) {
      return (bool)(any_cast<long>(v1) != any_cast<long>(v2));
    } else if (op == 2) {
      return (bool)(any_cast<long>(v1) < any_cast<long>(v2));
    } else if (op == 3) {
      return (bool)(any_cast<long>(v1) > any_cast<long>(v2));
    } else if (op == 4) {
      return (bool)(any_cast<long>(v1) <= any_cast<long>(v2));
    } else if (op == 5) {
      return (bool)(any_cast<long>(v1) >= any_cast<long>(v2));
    }
  } else if (ast.name == "IfStatement") {
    auto val = getValue(eval(*nodes[0]));
    if (any_cast<bool>(val)) {
      return eval(*nodes[1]);
    } else {
      return eval(*nodes[2]);
    }
  } else if (ast.name == "Block") {
    // TODO create new scope
    std::any result = Nil();
    for (int i=0; i<ast.nodes.size(); i++) {
      result = eval(*nodes[i]);
    }
    return result;
  } else if (ast.name == "WhileStatement") {
    std::any result = Nil();
    while (any_cast<bool>(getValue(eval(*nodes[0])))) {
      result = eval(*nodes[1]);
    }
    return result;
  } else if (ast.name == "Fn") {
    auto identifiers = any_cast<std::vector<Identifier>>(eval(*nodes[0]));
    return Func(nodes[1], identifiers);
  } else if (ast.name == "ParamDeclList") {
    std::vector<Identifier> identifiers;
    for (int i=0; i<nodes.size(); i++) {
      identifiers.push_back(any_cast<Identifier>(eval(*nodes[i])));
    }
    return identifiers;
  } else if (ast.name == "SuffixExpr") {
    // TODO handle other PrimaryTypeExpr types
    auto val = getValue(eval(*nodes[0]));
    auto func = any_cast<Func>(val);
    for (int i=1; i<nodes.size(); i++) {
      auto args = Args::toArgs(*this, eval(*nodes[i]));
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