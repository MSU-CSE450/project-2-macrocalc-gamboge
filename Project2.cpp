#include <assert.h>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

//Include Files
#include "ASTNode.hpp"
#include "lexer.hpp"
#include "SymbolTable.hpp"


//Initialize Info for AST
size_t token_id{0};
ASTNode root{ASTNode::EMPTY};
ASTNode curr = root;
SymbolTable symbols{};

//Initialize Info for Parsing
int lineNumber = 1;
size_t token_count = 0;
emplex::Token token;
std::vector<emplex::Token> tokens;
std::vector<int> scope; //Used to help restart while loop


bool SampleASTOutput(){
  /*
  Placeholder for AST Node, allows to test Parser prior to functional Abstract Syntax Tree
  */
  return true;
}

void Print(){
  /*
  Print Functionality

  Enters when "print" token lexeme is detected
  Allows building on after '(' and finally runs AST on ')'

  Prints output on ')'
  */

  bool buildAST = false;

  while(token_count < tokens.size()){
    emplex::Token token = tokens[token_count];
    token_count++;

    if(token.lexeme == ")"){
      //Run AST Here and Print

      std::cout << std::endl;
      break;
    } 
    else if (buildAST){
      //Add to AST Node Here
      std::cout << token.lexeme;
    }
    
    if (token.lexeme != "("){
      buildAST = true;
    }
  }
}



void If(){
  /*
  If Functionality

  Enters when "if" token lexeme is detected
  Allows building expression in AST on after '(' and finally runs AST on ')'

  If Expression is true, creates scope and exits function and continues in Parse()
  If Expression is false, Skips to closing '}'
  */

  bool buildAST = false;
  while(token_count < tokens.size()){
    emplex::Token token = tokens[token_count];
    token_count++;

    if(token.lexeme == ")"){
      if(SampleASTOutput() == false){
        //Skips If-Else
        while(token_count < tokens.size()){
          emplex::Token token = tokens[token_count];
          if(token.lexeme == "}" || token.lexeme == ";"){
            token_count++;
            break;
          }
          token_count++;
         }

      } else {
        symbols.PushScope();
      }

      break;
    } 
    else if (buildAST){
      //Add to AST Node Here

    }
    
    if (token.lexeme != "("){
      buildAST = true;
    }

  }
}

void While(){
  /*
  While Functionality

  Enters when "while" token lexeme is detected
  Allows building expression in AST on after '(' and finally runs AST on ')'

  If Expression is true, creates scope and exits function and continues in Parse()
  If Expression is false, Skips to closing '}'
  */

  bool buildAST = false;
  while(token_count < tokens.size()){
    emplex::Token token = tokens[token_count];
    token_count++;

    if(token.lexeme == ")"){
      if(SampleASTOutput() == false){
        //Skips While
        while(token_count < tokens.size()){
          emplex::Token token = tokens[token_count];
          if(token.lexeme == "}" || token.lexeme == ";"){
            token_count++;
            break;
          }

          token_count++;
         }

      } else {
        symbols.PushScope();
      }

      break;
    } 
    else if (buildAST){
      //Add to AST Node Here
    }
    
    if (token.lexeme != "("){
      buildAST = true;

    }

  }
}

void Variable(){
  /*
  Variable Functionality

  Enters when "var" token lexeme is detected
  Allows building expression in AST on after 'var' and finally runs AST on ';'

  Once token reaches semicolon, runs AST allowing variable to be set, then returns to Parser()
  */

  while(token_count < tokens.size()){
    emplex::Token token = tokens[token_count];
    
    token_count++;

    if(token.lexeme == ";"){
      //Run AST Here

      break;
    } 

    //Build AST Here

  }
}


void CheckForKey(emplex::Token token){
  /*
  Runs on Each Token, checks if key word If, While, Print, Var

  If detects key word, runs key word's cooresponding function
  Within Function, functionality to deal with each case

  scope is meant to allow program to deal with nests
  */

  //If-Else
  if(strcmp(emplex::Lexer::TokenName(token.id), "IF")){
    scope.push_back(-1);
    If();
  }
  //While
  if(strcmp(emplex::Lexer::TokenName(token.id), "WHILE")){
    scope.push_back(token_count);
    While();
  }

  //Print
  if(strcmp(emplex::Lexer::TokenName(token.id), "PRINT")){
    Print();
  }

  //Variable
  if(strcmp(emplex::Lexer::TokenName(token.id), "VARIABLE")){
    Variable();
  }
}


void Parse()
{
  /*
  Main Parse Function

  Runs through each token, in special cases (ex. Key Words) uses helper functions
  */

  while(token_count < tokens.size()){
    //Initializes current working token, allows next one in next loop
    emplex::Token token = tokens[token_count];
    token_count++;

    //Comment Line or whitespace - Skips
    if(token.id == 248 || token.id == 250){
      continue;
    }

    //Check for Key Word - If, While, Print, Var
    CheckForKey(token);

    //End of Scope
    if(strcmp(emplex::Lexer::TokenName(token.id), "}")){
      symbols.PopScope(); //Remove top layer of symbol tree

      //If ending while loop, restarts from the top
      int temp = scope.back();
      scope.pop_back(); 

      if(temp != -1){
        token_count = temp - 1;
        token = tokens[token_count];
      }
    }

    //Line Numbers
    if (token.lexeme.find("\n") != std::string::npos)
      {
        lineNumber++;
      }
  }
}

std::string Fix_string(std::string str,SymbolTable symbols)
{
  /*
  Handles special case of internal variable in string - "hello {variable_name}"
  */

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

  //-Commented out example code, use when ready
  // symbols.AddVar("Value",lineNumber,5.0);
  // lineNumber++;
  // symbols.AddVar("Answer",lineNumber,6.0);
  // std::string str = Fix_string("this is the answer: {Answer}",symbols);
  // std::cout<<str;
}



int main(int argc, char * argv[])
{
  //Allow test cases
  if (argc != 2) {
    std::cout << "Format: " << argv[0] << " [filename]" << std::endl;
    exit(1);
  }

  std::string filename = argv[1];
  
  std::ifstream in_file(filename);
  if (in_file.fail()) {
    std::cout << "ERROR: Unable to open file '" << filename << "'." << std::endl;
    exit(1);
  }

  //Initialize Tokens
  emplex::Lexer lexer;
  tokens = lexer.Tokenize(in_file);

  //Begin Parse
  Parse();

}


ASTNode ParseExpression(std::vector<emplex::Token> tokens, size_t & curr_index, SymbolTable & symbols);

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

/// @brief creates an AST for an expression.
/// @param tokens The vector of tokens
/// @param curr_token The first token to parse. Will be changed; after running, this will refer to the first token after the expression
/// @param symbols The symbol table.
/// @return an ASTNode, ready to be run
ASTNode ParseExpression(std::vector<emplex::Token> tokens, size_t & curr_index, SymbolTable & symbols) {

  return ParseTerm(tokens, curr_index, symbols);
  //return ParseAssign(tokens, curr_index, symbols);
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