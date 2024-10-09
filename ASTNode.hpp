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
    LEAF_LITERAL,
    LEAF_VARIABLE,
    IF_STATEMENT,
    WHILE_STATEMENT
  };

private:
  Type type{EMPTY};
  std::string value{0};  //this is an ID of a variable in the syntax table, if necessary
  std::vector<ASTNode> children{};

  //ASTNode* parent{nullptr};
  
  //only used for leaf nodes storing rote floats
  double leaf_literal_contents;
  
public:
  //Specify the node type on construction
  ASTNode(Type type/*, ASTNode* parent*/) : type(type)/*, parent(parent)*/ {}
  ASTNode(Type type, /*ASTNode* parent,*/ std::string value) : type(type), value(value)/*, parent(parent)*/ {}
  ASTNode(float leaf_literal_contents) : type(LEAF_LITERAL), leaf_literal_contents(leaf_literal_contents)/*, parent(parent)*/ {}


  ASTNode(const ASTNode &) = default;
  ASTNode(ASTNode &&) = default;
  ASTNode & operator=(const ASTNode &) = default;
  ASTNode & operator=(ASTNode &&) = default;
  ~ASTNode() {}

  //Values
  void SetValue(std::string in) { 
    value = in;
  }
  std::string GetValue() const {return value;}
  
  //Get Info
  Type GetType() const {return type;}

  void AddChild(ASTNode node) {
    children.push_back(node);
    //node.parent = this;
  }

  //Get Child Info
  ASTNode & GetChild(size_t id){
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


  //Debug Helper
  std::string TypeToString() const {
    switch (type) {
      case EMPTY: return "EMPTY";
      case STATEMENT_BLOCK: return "STATEMENT_BLOCK";
      case ASSIGN: return "ASSIGN";
      case UNARY_STATEMENT: return "UNARY_STATEMENT";
      case LEAF_LITERAL: return "LEAF_LITERAL";
      case LEAF_VARIABLE: return "LEAF_VARIABLE";
      default: return "UNKNOWN";
    }
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

      symbols.AddVar(var_id, 1, assignVal);
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
        // std::cout << "ERROR! AST node expression has the operation " << value << ", which it can't understand" << std::endl;
        // std::cout << "dunno how to tell things to stop" << std::endl;
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
      case LEAF_LITERAL:
      {
        //check symbol table for variable.
        return leaf_literal_contents;
        break;
      }
      case IF_STATEMENT:
      {
        assert(GetChildren().size() == 2 || GetChildren().size() == 3);
        float expression = RunChild(0, symbols);
        if (expression != 0) {
          RunChild(1, symbols);
        }
        else if (GetChildren().size() == 3) {
          RunChild(2, symbols);
        }
        return 0; //nothing should ever be able to use the return value of an if statement
        break;
      }
      case WHILE_STATEMENT:
      {
        assert(GetChildren().size() == 2);
        float expression = RunChild(0, symbols);
        while (expression != 0) {
          RunChild(1, symbols);
        }
        return 0; //nothing should ever be able to use the return value of a while loop
        break;
      }
    case EMPTY:
      {
        //check symbol table for variable.
        const std::string& valueStr = GetValue();
        return std::stof(valueStr);
        break;
      }
    default:
      //empty
        // std::cout << "ERROR! AST node with EMPTY type. This shouldn't happen!" << std::endl;
        // std::cout << "dunno how to tell things to stop" << std::endl;

      break;
    }
  }

};