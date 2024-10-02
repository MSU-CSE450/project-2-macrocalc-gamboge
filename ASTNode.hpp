#pragma once

#include <cmath>
#include <sstream>
#include <string>
#include <vector>

#include "SymbolTable.hpp"

class ASTNode {
public:
  enum Type {
    EMPTY = 0,
    STATEMENT_BLOCK,
    ASSIGN,
    PRINT,
    LEAF_VARIABLE, //variable. To execute, pull this variable from symbol table and return
    LEAF_STRING, //literal string. eg, print <literal string>
    LEAF_LITERAL //literal float
  };

private:
  // PLACE AST NODE INFO HERE.

  Type type{EMPTY};
  size_t value{0};  //this is an ID of a variable in the syntax table, if necessary
  std::vector<ASTNode> children{};

  std::string leaf_str_contents; //these two variables used only if the type is leaf_string/leaf_literal
  double leaf_literal_contents;
  
public:
  // CONSTRUCTORS, ETC HERE.
  // CAN SPECIFY NODE TYPE AND ANY NEEDED VALUES HERE OR USING OTHER FUNCTIONS.
  ASTNode(Type thisType) {
    type = thisType;
  }

  // CODE TO ADD CHILDREN AND SETUP AST NODE HERE.
  void AddChild(ASTNode node) {
    //how do we know which child the node should be? 
    ;
  }

  std::vector<ASTNode> GetChildren() {

  }
  
  // CODE TO EXECUTE THIS NODE (AND ITS CHILDREN, AS NEEDED).
  float Run(SymbolTable & symbols) { 
    //recursively call Run on each child, if it exists, unless we have the information
    //on both sides to effectively execute.
    //how do we know information exists/how do we store that?
    switch (type)
    {

    case ASSIGN:
      //assert(GetChildren.size() == 2);
      //assert(GetChild(0).GetType() == ASTNode::LEAF_VARIABLE);

      //size_t var_id = GetChild(0).Run();
      //float assignVal = GetChild(1).Run();
      //return symbols. VarValue(var_id) = assignVal;
      break;
    case STATEMENT_BLOCK:
      break;
    case PRINT:
      //run children; it should return a string (if it's a string) or the result of the calculation
      std::cout << children[0].Run(symbols);
      break;

    case LEAF_STRING:
      //return string contents as string
      //return leaf_str_contents;
      break;
    case LEAF_LITERAL:
      //return literal variable as double
      //return leaf_literal_contents;
      //see example code, they just do this.GetWords
      break;
    case LEAF_VARIABLE:
      //check symbol table for variable.
      //if not there, ??? (can't just error because it might be a declaration, I think?)
      //return symbols.VarValue(GetValue())
      break;

    default: //TODO: make enough cases that default is EMPTY, which shouldn't appear
      break;
    }
  }

};
