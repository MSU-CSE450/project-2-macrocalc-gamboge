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
  ASTNode(Type type = EMPTY) : type(type) {}
  //ASTNode(Type type = EMPTY, size_t value = 1) : type(type), value(value) {}
  //ASTNode(Type type = LEAF_LITERAL, double leaf_literal_contents = 0) : type(type), leaf_literal_contents(leaf_literal_contents) {}
  //ASTNode(Type type = LEAF_STRING, std::string leaf_str_contents = "") : type(type), leaf_str_contents(leaf_str_contents) {}
  //ASTNode(Type type = EMPTY, ASTNode child) : type(type) {AddChild(child);}
  //ASTNode(Type type = EMPTY, ASTNode child1, ASTNode child2) : type(type) {AddChild(child1); AddChild(child2);}

  ASTNode(const ASTNode &) = default;
  ASTNode(ASTNode &&) = default;
  ASTNode & operator=(const ASTNode &) = default;
  ASTNode & operator=(ASTNode &&) = default;
  ~ASTNode() {}

  void SetValue(size_t in) { value = in;}

  size_t GetValue() const {return value;}
  Type GetType() const {return type;}
  const double & GetLitValue() const {return leaf_literal_contents;}
  const std::string & GetLitString() const {return leaf_str_contents;}


  // CODE TO ADD CHILDREN AND SETUP AST NODE HERE.
  void AddChild(ASTNode node) {
    //how do we know which child the node should be? 
    ;
  }

  const ASTNode & GetChild(size_t id) const {
    assert(id < children.size());
    return children[id];
  }
  const std::vector<ASTNode> & GetChildren() const {
    return children;
  }

  float RunChild(size_t id, SymbolTable & symbols) {
    assert(id < children.size());
    return children[id].Run(symbols);
  }


  
  // CODE TO EXECUTE THIS NODE (AND ITS CHILDREN, AS NEEDED).
  float Run(SymbolTable & symbols) { 
    //recursively call Run on each child, if it exists, unless we have the information
    //on both sides to effectively execute.
    //how do we know information exists/how do we store that?
    switch (type)
    {

    case ASSIGN:
      assert(GetChildren().size() == 2);
      assert(GetChild(0).GetType() == ASTNode::LEAF_VARIABLE);

      size_t var_id = RunChild(0, symbols);
      float assignVal = RunChild(1, symbols);
      //return symbols. VarValue(var_id) = assignVal;
      break;
    case STATEMENT_BLOCK:
      break;
    case PRINT:
      if (GetChild(0).GetType() == LEAF_STRING) { //in case we're printing a string, and not an expression
        std::cout << GetChild(0).GetLitString();
      }
      else {
      std::cout << RunChild(0, symbols);
      }
      break;

    case LEAF_STRING:
      //this code won't be run; instead, the string's value is retrieved from the parent
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
