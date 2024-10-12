#include <assert.h>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

// Below are some suggestions on how you might want to divide up your project.
// You may delete this and divide it up however you like.
#include "ASTNode.hpp"
#include "lexer.hpp"
#include "SymbolTable.hpp"


//in the example code these were in the header file; I don't think we'll be using that here?
size_t token_id{0};
ASTNode root{ASTNode::EMPTY};
ASTNode curr = root;
SymbolTable symbols{};
int lineNumber = 1;

bool ast = false;

bool variable = false; // this indicates that a variable has been identified
std::string var = "";

void UseAST(emplex::Token token){
  if(token.lexeme == "="){
    ASTNode temp{ASTNode::ASSIGN};
    ASTNode child{ASTNode::LEAF_VARIABLE, var};

    temp.AddChild(child);
    temp.AddChild(root);

    curr = root;
    root = temp;
    
  }
  else if((token.id == 252 || token.id == 251 || token.id == 249) && variable == true){ //Int, Double, Variable
    // curr.SetValue(token.lexeme);
    if(token.id == 249){ //variable
      std::cout << symbols.GetValue(token.lexeme) << std::endl;
      // root.GetChild(1).SetValue(std::to_string(symbols.GetValue(token.lexeme)));
    }else{
      root.GetChild(1).SetValue(token.lexeme);
    }
    
  }
  
  // else if(token.lexeme == "**"){
  //   ASTNode temp{ASTNode::STATEMENT_BLOCK, "**"};
  //   root = temp;
  // }
  // else if(token.lexeme == "*"){
  //   ASTNode temp{ASTNode::STATEMENT_BLOCK, "*"};
  //   temp.AddChild(root);
  //   root = temp;
  // }
  // else if(token.lexeme == "/"){
  //   ASTNode temp{ASTNode::STATEMENT_BLOCK, "/"};
  //   temp.AddChild(root);
  //   root = temp;

  // }
  // else if(token.lexeme == "%"){
  //   ASTNode temp{ASTNode::STATEMENT_BLOCK, "%"};
  //   temp.AddChild(root);
  //   root = temp;

  // }
  // else if(token.lexeme == "+"){
  //   ASTNode temp{ASTNode::STATEMENT_BLOCK, "+"};
  //   temp.AddChild(root);
  //   root = temp;
  // }
  // else if(token.lexeme == "-"){
  //   ASTNode temp{ASTNode::STATEMENT_BLOCK, "-"};
  //   temp.AddChild(root);
  //   root = temp;

  // }
}

bool print = false;

void Print(emplex::Token token){
  if(token.lexeme == ")"){
    std::cout << std::endl;
    print = false;
  } else if (token.lexeme != "("){

    //Print out with variables or normal string

    if(token.id == 249){/*strcmp(emplex::Lexer::TokenName(token.id), "VARIABLE") == 0*/ //this line was erroring so I changed it
      std::cout << symbols.GetValue(token.lexeme);
    } else {
      std::cout << token.lexeme;
    }
  }
}

//Parse each token
void Parse(std::vector<emplex::Token> tokens)
{
  for (emplex::Token token: tokens) {
    if(token.id == 248 || token.id == 250){ //Comment Line or whitespace
      continue;
    }

    if(variable && var == ""){
      var = token.lexeme;
      continue;
    }

    UseAST(token);

    //Print
    if(token.id == 255){
      print = true;
      continue;
    }

    //Variable
    if(token.id == 249 && token.lexeme == "var"){
      variable = true;
      continue;
    }

    //Semicolon
    if(token.id == 254){
      //Run AST Node from Root

      if(root.GetChildren().size() > 0){
        root.Run(symbols);

      }
      ASTNode temp{ASTNode::EMPTY};
      var = "";
      root = temp;
      variable = false;
      ast = false;
      continue;
    }
    if(print) Print(token);

    if (token.lexeme.find("\n") != std::string::npos)
      {
        lineNumber++;
      }
  }
}

std::string Fix_string(std::string str,SymbolTable symbols)
{
    size_t open_brace_pos = str.find('{');
    size_t close_brace_pos = str.find('}');

    // Check if both open and close braces exist
    if (open_brace_pos != std::string::npos && close_brace_pos != std::string::npos && open_brace_pos < close_brace_pos) {
        // Extract the substring between the braces
        std::string key = str.substr(open_brace_pos + 1, close_brace_pos - open_brace_pos - 1);
        auto value = symbols.GetValue(key);

        // Check if the key exists in the unordered_map
        
        if (symbols.HasVar(key)) {
            // Replace the substring inside the braces with the corresponding value from the map
            str.replace(open_brace_pos, close_brace_pos - open_brace_pos + 1, std::to_string(value));
        } else {
            std::cerr << "Key '" << key << "' not found in the map!" << std::endl;
        }
    }

    return str;
}
int main(int argc, char * argv[])
{
  // if (argc != 2) {
  //   std::cout << "Format: " << argv[0] << " [filename]" << std::endl;
  //   exit(1);
  // }

  // std::string filename = argv[1];
  
  // std::ifstream in_file(filename);
  // if (in_file.fail()) {
  //   std::cout << "ERROR: Unable to open file '" << filename << "'." << std::endl;
  //   exit(1);
  // }

  // //Initialize Tokens
  // emplex::Lexer lexer;
  // std::vector<emplex::Token> tokens = lexer.Tokenize(in_file);

  // //Go through each token within file

  // Parse(tokens);
  
  // //Starts execution of AST Node which grew during Parse
  // // root.Run(symbols);

  symbols.AddVar("Value",lineNumber,5.0);
  lineNumber++;
  symbols.AddVar("Answer",lineNumber,6.0);
  std::string str = Fix_string("this is the answer: {Answer}",symbols);
  std::cout<<str;

}



/// @brief creates an AST for an expression.
/// @param tokens The vector of tokens
/// @param curr_token The first token to parse. Will be changed; after running, this will refer to the first token after the expression
/// @param symbols The symbol table.
/// @return an ASTNode, ready to be run
ASTNode ParseExpression(std::vector<emplex::Token> tokens, size_t & curr_index, SymbolTable & symbols) {

  return ParseTerm(tokens, curr_index, symbols);
  //return ParseAssign(tokens, curr_index, symbols);

}


ASTNode ParseTerm(std::vector<emplex::Token> tokens, size_t & curr_index, SymbolTable & symbols){
  if  (LexemeIsUnOp(tokens[curr_index])) {
    ASTNode out = ASTNode(ASTNode::UNARY_EXPRESSION, tokens[curr_index].lexeme);
    if (tokens[curr_index+1].id == 249) { //variable
      ASTNode leaf = ASTNode(ASTNode::LEAF_VARIABLE, tokens[curr_index+1].lexeme);
      out.AddChild(leaf);

      curr_index += 2;
      return out;
    }
    else if (tokens[curr_index+1].id == 252 || tokens[curr_index+1].id == 251) {//double; int. Same difference.
      ASTNode leaf = ASTNode(std::stof(tokens[curr_index+1].lexeme));
      out.AddChild(leaf);
      
      curr_index += 2;
      return out;
    }
  }

  else if (tokens[curr_index].id == 249) { //variable
    ASTNode leaf = ASTNode(ASTNode::LEAF_VARIABLE, tokens[curr_index].lexeme);
    
    curr_index += 1;
    return leaf;
  }
  else if (tokens[curr_index].id == 252 || tokens[curr_index].id == 251) {//double; int. Same difference.
    ASTNode leaf = ASTNode(std::stof(tokens[curr_index].lexeme));
    
    curr_index += 1;
    return leaf;
  }

  else if (tokens[curr_index].lexeme == "(") {
    curr_index++;//use (
    ASTNode out = ParseExpression(tokens, curr_index, symbols);
    if (tokens[curr_index].lexeme != ")") {
      //error
    }
    curr_index++;//use )
    return out;
  }

}
void ParseExp(std::vector<emplex::Token> tokens, SymbolTable & symbols){}
void ParseMult(std::vector<emplex::Token> tokens, SymbolTable & symbols){}
void ParseAdd(std::vector<emplex::Token> tokens, SymbolTable & symbols){}
void ParseInequal(std::vector<emplex::Token> tokens, SymbolTable & symbols){}
void ParseIsEqual(std::vector<emplex::Token> tokens, SymbolTable & symbols){}
void ParseLogicAnd(std::vector<emplex::Token> tokens, SymbolTable & symbols){}
void ParseLogicOr(std::vector<emplex::Token> tokens, SymbolTable & symbols){}
ASTNode ParseAssign(std::vector<emplex::Token> tokens, size_t & curr_index, SymbolTable & symbols){
  
  //ASTNode lhs = ParseLogicOr(tokens, curr_index, symbols);
  //if (tokens[curr_index].lexeme == "=") {
  //}
  //return lhs;
}

bool LexemeIsVarOrLit(emplex::Token token) {
  return token.id == 249 || token.id == 252 || token.id == 251;
}
bool LexemeIsUnOp(emplex::Token token) {
  std::string lexeme = token.lexeme;
  return lexeme == "!" || lexeme == "-"; //note the repeated lexeme between these
}
bool LexemeIsBinOp(emplex::Token token) {
  std::string lexeme = token.lexeme;
  return lexeme == "**" || lexeme == "*" || lexeme == "/" || lexeme == "+" || lexeme == "-" || lexeme == "%" || lexeme == "<" || lexeme == "<=" || lexeme == ">" || lexeme == ">=" || lexeme == "==" || lexeme == "!=" || lexeme == "=" || lexeme == "&&" || lexeme == "||";
}