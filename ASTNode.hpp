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
    UNARY_STATEMENT,
    LEAF_VARIABLE
  };

private:
  Type type{EMPTY};
  std::string value{0};  //this is an ID of a variable in the syntax table, if necessary
  std::vector<ASTNode> children{};

  //ASTNode* parent{nullptr};
  
  std::string leaf_str_contents; //these two variables used only if the type is leaf_string/leaf_literal
  //double leaf_literal_contents;
  
public:
  //Specify the node type on construction
  ASTNode(Type type/*, ASTNode* parent*/) : type(type)/*, parent(parent)*/ {}
  ASTNode(Type type, /*ASTNode* parent,*/ std::string value) : type(type), value(value)/*, parent(parent)*/ {}


  ASTNode(const ASTNode &) = default;
  ASTNode(ASTNode &&) = default;
  ASTNode & operator=(const ASTNode &) = default;
  ASTNode & operator=(ASTNode &&) = default;
  ~ASTNode() {}

  //Values
  void SetValue(std::string in) { value = in;}
  std::string GetValue() const {return value;}
  
  //Get Info
  Type GetType() const {return type;}


  void AddChild(ASTNode node) {
    children.push_back(node);
    //node.parent = this;
  }

  //Get Child Info
  const ASTNode & GetChild(size_t id) const {
    assert(id < children.size());
    return children[id];
  }
  const std::vector<ASTNode> & GetChildren() const {
    return children;
  }

  // Executes recursive tree from current Node
  float RunChild(size_t id, SymbolTable & symbols) {
    assert(id < children.size());
    // return children[id].Run(symbols);
  }



  
  // CODE TO EXECUTE THIS NODE (AND ITS CHILDREN, AS NEEDED).
  float Run(SymbolTable & symbols) { 
    // recursively call Run on each child, if it exists, unless we have the information
    // on both sides to effectively execute.
    // how do we know information exists/how do we store that?
    switch (type)
    {

    case ASSIGN: {
      assert(GetChildren().size() == 2);
      assert(GetChild(0).GetType() == ASTNode::LEAF_VARIABLE);

      std::string var_id = GetChild(0).GetValue();
      float assignVal = RunChild(1, symbols);
      symbols.SetValue(var_id, assignVal);
      return assignVal;
      break;
    }
    case UNARY_STATEMENT: {
      assert(GetChildren().size() == 1);

      double operand = RunChild(0, symbols);
      if(value == "!"){
        return !operand;
      }
      else if(value == "-"){
        return -operand;
      }
    }
    case STATEMENT_BLOCK: {
      assert(GetChildren().size() == 2);

      double leftSide = RunChild(0, symbols);
      double rightSide = RunChild(1, symbols);
      double result;
      if(value == "**"){
        return std::pow(leftSide, rightSide);
      }
      else if(value == "*"){
        return leftSide * rightSide;
      }
      else if(value == "/"){
        if (rightSide == 0) {
        std::cout << "ERROR! division by 0 :(" << std::endl;
        std::cout << "dunno how to tell things to stop" << std::endl;
        }
        return leftSide / rightSide;
      }
      else if(value == "%"){
        if (rightSide == 0) {
        std::cout << "ERROR! modulo by 0 :(" << std::endl;
        std::cout << "dunno how to tell things to stop" << std::endl;
        }
        return std::fmod(leftSide, rightSide);
      }
      else if(value == "+"){
        return leftSide + rightSide;
      }
      else if(value == "-"){
        return leftSide - rightSide;
      }
      else if(value == "<"){
        return leftSide < rightSide;
      }
      else if(value == "<="){
        return leftSide <= rightSide;
      }
      else if(value == ">"){
        return leftSide > rightSide;
      }
      else if(value == ">="){
        return leftSide >= rightSide;
      }
      else if(value == "=="){
        return leftSide == rightSide;
      }
      else if(value == "!="){
        return leftSide != rightSide;
      }
      else if(value == "&&"){
        return leftSide && rightSide;
      }
      else if(value == "||"){
        return leftSide || rightSide;
      }
      else {
        std::cout << "ERROR! AST node expression has the operation " << value << ", which it can't understand" << std::endl;
        std::cout << "dunno how to tell things to stop" << std::endl;
        //error
      }
      break;
    }
    case LEAF_VARIABLE:
      {
        //check symbol table for variable.
        return symbols.GetValue(value); //TODO: err if not there? if not handled by table itself
        break;
      }
    default:
      //empty
        std::cout << "ERROR! AST node with EMPTY type. This shouldn't happen!" << std::endl;
        std::cout << "dunno how to tell things to stop" << std::endl;

      break;
    }
  }

};
