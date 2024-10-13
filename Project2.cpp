#include <assert.h>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <set>

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
size_t token_count = 1;
emplex::Token token;
std::vector<emplex::Token> tokens;
std::vector<int> scope; //Used to help restart while loop

std::set<int> ids = {240, 241, 242, 255};


//String Helper
std::string RemoveTrailingZeros(std::string str) {
    // Remove trailing zeros
    if (str.find('.') != std::string::npos) {
        while (str.back() == '0') {
            str.pop_back();
        }
        if (!str.empty() && str.back() == '.') {
            str.pop_back();
        }
    }
    return str;
}

std::string Fix_string(std::string str, SymbolTable symbols)
{
    /*
    Handles special case of internal variable in string - "hello {variable_name}"
    */

    size_t open_brace_pos = 0;

    // Loop until no more open braces are found
    while ((open_brace_pos = str.find('{', open_brace_pos)) != std::string::npos) {
        size_t close_brace_pos = str.find('}', open_brace_pos);

        // Check if the closing brace exists and is after the opening brace
        if (close_brace_pos != std::string::npos) {
            // Extract the substring between the braces
            std::string key = str.substr(open_brace_pos + 1, close_brace_pos - open_brace_pos - 1);
            auto value = symbols.GetValue(key);

            // Check if the key exists in the unordered_map
            if (symbols.HasVar(key)) {
                std::string value_str = RemoveTrailingZeros(std::to_string(value));
                str.replace(open_brace_pos, close_brace_pos - open_brace_pos + 1, value_str);
                close_brace_pos = open_brace_pos + value_str.length(); 
            } else {
                std::cerr << "Key '" << key << "' not found in the map!" << std::endl;
            }

            open_brace_pos = close_brace_pos; 
        } else {
            break;
        }
    }

    // Remove leading and trailing quotes
    if (!str.empty() && (str.front() == '"' || str.front() == '\'')) {
        str.erase(str.begin());
    }

    if (!str.empty() && (str.back() == '"' || str.back() == '\'')) {
        str.pop_back();
    }

    return str;

    //-Commented out example code, use when ready
    // symbols.AddVar("Value",lineNumber,5.0);
    // lineNumber++;
    // symbols.AddVar("Answer",lineNumber,6.0);
    // std::string str = Fix_string("this is the answer: {Answer}",symbols);
    // std::cout<<str;
}


//AST Node stuff
//Forward declerations to resolve errors
ASTNode ParseAssign(std::vector<emplex::Token> tokens, size_t & curr_index, SymbolTable & symbols);
ASTNode ParseAdd(std::vector<emplex::Token> tokens, size_t & curr_index, SymbolTable & symbols);


/// @brief creates an AST for an expression.
/// @param tokens The vector of tokens.
/// @param curr_token The first token to parse. Will be changed; after running, this will refer to the first token after the expression
/// @param symbols The symbol table.
/// @return an ASTNode, ready to be run
ASTNode ParseExpression(std::vector<emplex::Token> tokens, size_t & curr_index, SymbolTable & symbols) {
  // Pass the filtered tokens to ParseAssign
  return ParseAssign(tokens, curr_index, symbols);

}

ASTNode ParseTerm(std::vector<emplex::Token> tokens, size_t & curr_index, SymbolTable & symbols){
  if  (tokens[curr_index].lexeme == "!" || tokens[curr_index].lexeme == "-") { //do we have a unary operator, eg negation
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

ASTNode ParseExp(std::vector<emplex::Token> tokens, size_t & curr_index, SymbolTable & symbols){
  ASTNode lhs = ParseTerm(tokens, curr_index, symbols);
  if (tokens[curr_index].lexeme == "**") {
    curr_index++; //use **
    ASTNode rhs = ParseExp(tokens, curr_index, symbols);
    ASTNode out = ASTNode(ASTNode::EXPRESSION_BLOCK, "**");
    out.AddChild(lhs);
    out.AddChild(rhs);
    return out;
  }
  return lhs;
}
ASTNode ParseMult(std::vector<emplex::Token> tokens, size_t & curr_index, SymbolTable & symbols){
  ASTNode lhs = ParseExp(tokens, curr_index, symbols);

  while (tokens[curr_index].lexeme == "*" || tokens[curr_index].lexeme == "/" || tokens[curr_index].lexeme == "%") {
    std::string op = tokens[curr_index].lexeme;
    curr_index++; //use token
    ASTNode rhs = ParseAdd(tokens, curr_index, symbols);
    ASTNode out = ASTNode(ASTNode::EXPRESSION_BLOCK, op);
    out.AddChild(lhs);
    out.AddChild(rhs);
    lhs = out;  
  }
  return lhs;
}
ASTNode ParseAdd(std::vector<emplex::Token> tokens, size_t & curr_index, SymbolTable & symbols){
  ASTNode lhs = ParseMult(tokens, curr_index, symbols);
  while (tokens[curr_index].lexeme == "+" || tokens[curr_index].lexeme == "-") {
    std::string op = tokens[curr_index].lexeme;
    curr_index++; //use token
    ASTNode rhs = ParseAdd(tokens, curr_index, symbols);
    ASTNode out = ASTNode(ASTNode::EXPRESSION_BLOCK, op);
    out.AddChild(lhs);
    out.AddChild(rhs);
    lhs = out;  
  }
  return lhs;
}
ASTNode ParseInequal(std::vector<emplex::Token> tokens, size_t & curr_index, SymbolTable & symbols){
  ASTNode lhs = ParseAdd(tokens, curr_index, symbols);
  if (tokens[curr_index].lexeme == "<" || tokens[curr_index].lexeme == "<=" || tokens[curr_index].lexeme == ">" || tokens[curr_index].lexeme == ">=") {
    std::string op = tokens[curr_index].lexeme;
    curr_index++; //use token
    ASTNode rhs = ParseAdd(tokens, curr_index, symbols);
    ASTNode out = ASTNode(ASTNode::EXPRESSION_BLOCK, op);
    out.AddChild(lhs);
    out.AddChild(rhs);
    return out;
  }
  return lhs;
}
ASTNode ParseIsEqual(std::vector<emplex::Token> tokens, size_t & curr_index, SymbolTable & symbols){
  ASTNode lhs = ParseInequal(tokens, curr_index, symbols);
  if (tokens[curr_index].lexeme == "== " || tokens[curr_index].lexeme == " != ") {
    std::string op = tokens[curr_index].lexeme;
    curr_index++; //use == or !=
    ASTNode rhs = ParseInequal(tokens, curr_index, symbols);
    ASTNode out = ASTNode(ASTNode::EXPRESSION_BLOCK, op);
    out.AddChild(lhs);
    out.AddChild(rhs);
    
    return out;
  }
  return lhs;
}
ASTNode ParseLogicAnd(std::vector<emplex::Token> tokens, size_t & curr_index, SymbolTable & symbols){
  
  ASTNode lhs = ParseIsEqual(tokens, curr_index, symbols);

  size_t temp = curr_index;

  if(lhs.Run(symbols) < 1){
    curr_index = temp;
    return lhs;
  }

  curr_index = temp;

  while (tokens[curr_index].lexeme == "&&") {
    curr_index++; //use &&
    ASTNode rhs = ParseLogicAnd(tokens, curr_index, symbols);
    ASTNode out = ASTNode(ASTNode::EXPRESSION_BLOCK, "&&");
    out.AddChild(lhs);
    out.AddChild(rhs);
    lhs = out;  
  }
  return lhs;
}
ASTNode ParseLogicOr(std::vector<emplex::Token> tokens, size_t & curr_index, SymbolTable & symbols){
  ASTNode lhs = ParseLogicAnd(tokens, curr_index, symbols);

  size_t temp = curr_index;

  if(lhs.Run(symbols) != 0){
    curr_index = temp;
    return lhs;
  }

  curr_index = temp;


  while (tokens[curr_index].lexeme == "||") {
    curr_index++; //use ||
    ASTNode rhs = ParseLogicOr(tokens, curr_index, symbols);
    ASTNode out = ASTNode(ASTNode::EXPRESSION_BLOCK, "||");
    out.AddChild(lhs);
    out.AddChild(rhs);
    lhs = out;  //if this is somehow a reference and breaks the node I just made I might end up on the news
  }
  return lhs;
}
ASTNode ParseAssign(std::vector<emplex::Token> tokens, size_t & curr_index, SymbolTable & symbols){
  ASTNode lhs = ParseLogicOr(tokens, curr_index, symbols);
  if (tokens[curr_index].lexeme == "=") {
    curr_index++; //use =
    ASTNode rhs = ParseAssign(tokens, curr_index, symbols);
    ASTNode out = ASTNode(ASTNode::ASSIGN);
    out.AddChild(lhs);
    out.AddChild(rhs);
    return out;
  }
  return lhs;
}


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

    token_count++;
    emplex::Token token = tokens[token_count];


    if (ids.find(token.id) != ids.end()) {
        break;
    }

    if(token.lexeme == ")"){
      //Run AST Here and Print
      
      break;
    } 
    else if (token.lexeme == "("){
      buildAST = true;
      if(tokens[token_count + 1].id == 253){
        std::cout << Fix_string(tokens[token_count + 1].lexeme, symbols) << std::endl;
      } else {
        std::cout << ParseExpression(tokens, token_count, symbols).Run(symbols) << std::endl;
      }
      

      break;
      
    }

    token_count++;
      

    
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

    if (ids.find(token.id) != ids.end()) {
        break;
    }

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

      token_count = token_count - 1;
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

    if (ids.find(token.id) != ids.end()) {
        break;
    }

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

      token_count = token_count + 1;
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
  bool set = false;;
  std::vector<emplex::Token> rhs;

  while(token_count < tokens.size()){
    token = tokens[token_count + 1];


    if (ids.find(token.id) != ids.end()) {
        break;
    }

    
    if(token.lexeme == ";"){
      break;
    }

    //Begin AST Build Here
    if(token.id == 249 && set == false){
      std::string variable_name = "";
      variable_name = token.lexeme;

      token_count = token_count + 1;
      symbols.AddVar(variable_name, 1, ParseExpression(tokens, token_count, symbols).Run(symbols));

      break;

      set = true;
    }

    token_count++;

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
  if(token.lexeme == "if"){
    scope.push_back(-1);
    If();
  }
  //While
  if(token.lexeme ==  "while"){
    scope.push_back(token_count);
    While();
  }

  //Print
  if(token.lexeme == "print"){
    Print();
  }

  //Variable
  if(token.lexeme == "var"){
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


    //Comment Line or whitespace - Skips
    if(token.id == 248 || token.id == 250){
      token_count++;
      continue;
    }

    //Check for Key Word - If, While, Print, Var
    CheckForKey(token);

    //End of Scope
    if(token.lexeme == "}"){
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


    token_count++;
  }

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

  std::vector<emplex::Token> filtered_tokens;

  for (size_t i = 0; i < tokens.size(); ++i) {
      if (tokens[i].id != 250) {
          filtered_tokens.push_back(tokens[i]);
      }
  }

  tokens = filtered_tokens;

  //Begin Parse
  Parse();
}
