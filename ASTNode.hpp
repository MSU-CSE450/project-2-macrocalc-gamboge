#pragma once

#include <cmath>
#include <sstream>
#include <string>
#include <vector>

#include "SymbolTable.hpp"
#include "lexer.hpp"

class ASTNode {
public:
  enum Type {
    EMPTY = 0,
    EXPRESSION_BLOCK,
    ASSIGN,
    UNARY_EXPRESSION,
    LEAF_LITERAL,
    LEAF_VARIABLE,
    IF_STATEMENT,
    WHILE_STATEMENT,
    STATEMENT_LIST
  };

private:
  Type type{EMPTY};
  std::string value{0};  //this is an ID of a variable in the syntax table, if necessary
  std::vector<ASTNode> children{};

  //ASTNode* parent{nullptr};
  
  //only used for leaf nodes storing rote floats
  double leaf_literal_contents;

/*
/// @brief swaps around two of the children of this node
/// @param lowChild 
/// @param highChild 
void Rebalance(ASTNode oldChild, ASTNode newChild) {

}

/// @brief Replaces a child with a new child, where the old one is a child of the new
/// @param newChild new node to add
/// @param index index of the old node
void SwapInNewChild(ASTNode newChild, size_t index) {
  ASTNode oldChild = GetChild(index);
  children[index] = newChild;
  newChild.AddChild(oldChild);
}*/


  
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
      case EXPRESSION_BLOCK: return "EXPRESSION_BLOCK";
      case ASSIGN: return "ASSIGN";
      case UNARY_EXPRESSION: return "UNARY_EXPRESSION";
      case LEAF_LITERAL: return "LEAF_LITERAL";
      case LEAF_VARIABLE: return "LEAF_VARIABLE";
      case IF_STATEMENT: return "IF_STATEMENT";
      case WHILE_STATEMENT: return "WHILE_STATEMENT";
      case STATEMENT_LIST: return "STATEMENT_LIST";
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
      //assert(GetChild(0).GetType() == ASTNode::LEAF_VARIABLE);
      if (GetChild(0).GetType() != ASTNode::LEAF_VARIABLE)
      {
        std::cout<<"ERROR: Assignment error"<<std::endl;
        exit(1);
      }

      std::string var_id = GetChild(0).GetValue();

      float assignVal = RunChild(1, symbols);

      symbols.AddVar(var_id, 1, assignVal);
      return assignVal;
      break;
    }
    case UNARY_EXPRESSION: {
      assert(GetChildren().size() == 1);

      double operand = RunChild(0, symbols);
      if(value == "!"){
        return !operand;
      }
      else if(value == "-"){
        return -operand;
      }
    }
    case EXPRESSION_BLOCK: {
      assert(GetChildren().size() == 2);

      double leftSide = RunChild(0, symbols);
      double rightSide;
      if (value != "||" && value != "&&") {
        rightSide = RunChild(1, symbols);
      }
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
      else if(value == "==" ){
        return leftSide == rightSide;
      }
      else if(value == "!="){
        return leftSide != rightSide;
      }

      else if(value == "&&"){
        if (!leftSide) {
          return false;
        }
        rightSide = RunChild(1, symbols);
        return leftSide && rightSide;
      }
      else if(value == "||"){
        if (leftSide) {
          return true;
        }
        rightSide = RunChild(1, symbols);
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
      /*case LEAF_LITERAL:
      {
          // if (symbols.HasVarInScope(value) == false)
          // {
            
          //     std::cout<<"ERROR: Unknown Identifier at line: "<<std::endl;
          //     exit(1);
          // }
        return symbols.GetValue(value); //TODO: err if not there? if not handled by table itself
        break;
      }*/
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
      case STATEMENT_LIST:
      {
        for (int i = 0; i < GetChildren().size(); i++) {
          RunChild(i, symbols);
        }
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






/*
use of AST

When you make an AST node, you'll define its type, and sometimes a value and children.
It'll always return a float; you could imagine it ricocheting values up from the leaves to the root as the tree executes.
The Value variable depends on the type, and so it'll be described with those.


the types work like this:
The EMPTY type should not be used. If an AST node with empty type is run, that's a problem


nodes with the LEAF_VARIABLE type hold a variable name. Right now, variables in the symbol table are identified with strings (note 1)
so the string representing the variable is held in the Value parameter in this case. When run, this node will return the current value
of the variable it holds.

(note 1: in class on wednesday the professor discussed using unique integer ids, not string ids, for symbol table entries. I
think we could make do with string ids, but it might be a little harder.)


nodes of the LEAF_LITERAL type are the only type which uses the leaf_literal_contents variable. To create one, call the consructor
for ASTNode that takes only a float. When run, this returns that float. 


nodes with the ASSIGN type should have two children. The left child should be an AST node of type LEAF_VARIABLE.
when run, this node will set the variable held by that left child to whatever the right child returns when run.
after doing this, it'll return whatever the variable was set to. (this type doesn't use the Value parameter)


nodes of the type EXPRESSION_BLOCK are the most complex. They expect to have two children, and a Value. The value is the operator;
stuff like +, *, **, >=, ect., just as a string. There's a big if/else block in this 
file you can check if you want the list of possibilities.
When run, this node type will preform the operation listed in Value on its left and right children, and return the result


nodes of type UNARY_EXPRESSION are just like the above, but much simpler; less operations are supported and there's only one operator.


nodes of type IF_STATEMENT have 2 or 3 children. given the expression: if (A) {B} else {C}, the children of this node are A, B, C in
that order. Child C can be missing without consequence. This type doesn't return anything because I don't think that would make sense


nodes of type WHILE_STATEMENT have 2 children. As above, for the statement: while (A) {B} the children are A, B in that order.
Also no return value for the same reason.


nodes of type STATEMENT_LIST have any number of children, where each child is some statement (probably a full line.) It runs each
child in order. Normally, nodes of the IF_STATEMENT type can only run one statement if the condition is true. Using this, 
multiple lines can be run in that case. This won't return anything either.
*/