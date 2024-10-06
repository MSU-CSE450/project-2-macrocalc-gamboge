#include <assert.h>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

// Below are some suggestions on how you might want to divide up your project.
// You may delete this and divide it up however you like.
// #include "ASTNode.hpp"
#include "lexer.hpp"
#include "SymbolTable.hpp"


//in the example code these were in the header file; I don't think we'll be using that here?
size_t token_id{0};
// ASTNode root{ASTNode::STATEMENT_BLOCK};
SymbolTable symbols{};

//Parse each token
void Parse(std::vector<emplex::Token> tokens)
{
  for (emplex::Token token: tokens) {
    if(token.id == 248 || token.id == 250){ //Comment Line or whitespace
      continue;
    }
  }
}

int main(int argc, char * argv[])
{
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
  std::vector<emplex::Token> tokens = lexer.Tokenize(in_file);

  //Go through each token within file
  Parse(tokens);
  
  //Starts execution of AST Node which grew during Parse
  // root.Run(symbols);
}



// ASTNode ParseDeclare() {
//   /*auto type_token = tokens[token_id++];
  
//   std::string var_name{};

//   switch (type_token) {
//   using namespace emplex;
//   case Lexer::ID_TYPE:  // @CAO This should be called LIST.
//     if (tokens[token_id] != Lexer::ID_ID) {
//       Error(tokens[token_id].line_id, "Expected identifier in variable declaration.");
//     }
//     var_name = tokens[token_id];
//     size_t var_id = symbols.AddVar(tokens[token_id].line_id, var_name);
//     ++token_id;
//     if (tokens[token_id] == ';') return ASTNode{};

//     if (tokens[token_id] != '=') {
//       Error(line tokens[token_id].line_id, "Expected ';' or '='.");
//     }
//     ++token_id;

//     auto rhs_node = ParseExpression();

//     ASTNode out_node(ASTNode::ASSIGN);
//     out_node.children.push_back(MakeVarNode(var_id));
//     out_node.children.push_back(rhs_node);
    
//     return out_node;
//   }*/

//   return ASTNode{ASTNode::EMPTY}; //I think?
// }