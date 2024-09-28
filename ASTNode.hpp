#pragma once

#include <cmath>
#include <sstream>
#include <string>
#include <vector>

#include "SymbolTable.hpp"

class ASTNode {
private:
  // PLACE AST NODE INFO HERE.
  enum Type {
    EMPTY = 0,
    STATEMENT_BLOCK,
    ASSIGN
  };
  Type type{EMPTY};
  size_t value{0};
  std::vector<ASTNode> children{};
  
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
  
  // CODE TO EXECUTE THIS NODE (AND ITS CHILDREN, AS NEEDED).
  double Run(SymbolTable & symbols) { 
    //recursively call Run on each chilld, if it exists, unless we have the information
    //on both sides to effectively execute.
    //how do we know information exists/how do we store that?
    ; 
  }

};
