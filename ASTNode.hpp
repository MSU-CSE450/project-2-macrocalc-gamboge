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
    //PRINT,
    UNARY_STATEMENT,
    LEAF_VARIABLE
    //LEAF_STRING,
    //LEAF_LITERAL,
  };

private:
  Type type{EMPTY};
  std::string value{0};  //this is an ID of a variable in the syntax table, if necessary
  std::vector<ASTNode> children{};

  ASTNode* parent{nullptr};
  //ASTNode* left{nullptr};
  //ASTNode* right{nullptr};
  
  
  std::string leaf_str_contents; //these two variables used only if the type is leaf_string/leaf_literal
  //double leaf_literal_contents;
  
public:
  //Specify the node type on construction
  ASTNode(Type type, ASTNode* parent) : type(type), parent(parent) {}
  ASTNode(Type type, ASTNode* parent, std::string value) : type(type), value(value), parent(parent) {}
  // ASTNode(Type type, double leaf_literal_contents) 
  //       : type(type), leaf_literal_contents(leaf_literal_contents) {
  //       assert(type == LEAF_LITERAL && "Type must be LEAF_LITERAL for this constructor");
  //   }
  // ASTNode(Type type, const std::string& leaf_str_contents) 
  //     : type(type), leaf_str_contents(leaf_str_contents) {
  //     assert(type == LEAF_STRING && "Type must be LEAF_STRING for this constructor");
  // }

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
  //const double & GetLitValue() const {return leaf_literal_contents;}
  //const std::string & GetLitString() const {return leaf_str_contents;}


  void AddChild(ASTNode node) {
    children.push_back(node);
    node.parent = this;
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
    return children[id].Run(symbols);
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
          //err
        }
        return leftSide / rightSide;
      }
      else if(value == "%"){
        if (rightSide == 0) {
          //err
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
        //error
      }
      break;
    }
    // case PRINT:
    // {
    //   if (GetChild(0).GetType() == LEAF_STRING) { //in case we're printing a string, and not an expression
    //     std::cout << GetChild(0).GetLitString();
    //   }
    //   else {
    //   std::cout << RunChild(0, symbols);
    //   }
    //   break;
    // }
    // case LEAF_STRING:
    //   //this code won't be run; instead, the string's value is retrieved from the parent
    //   break;
    // case LEAF_LITERAL:
    //   //return literal variable as double
    //   //return leaf_literal_contents;
    //   //see example code, they just do this.GetWords
    //   break;
    // case LEAF_VARIABLE:
    //   {
    //     //check symbol table for variable.
    //     return symbols.GetValue(value); //TODO: err if not there? if not handled by table itself
    //     break;
    //   }
    default:
      //empty
      //ERROR.

      break;
    }
  }

};
