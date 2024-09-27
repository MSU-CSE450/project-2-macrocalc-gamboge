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


int main(int argc, char * argv[])
{
  if (argc != 2) {
    std::cout << "Format: " << argv[0] << " [filename]" << std::endl;
    exit(1);
  }

  std::string filename = argv[1];
  
  std::ifstream in_file(filename);              // Load the input file
  if (in_file.fail()) {
    std::cout << "ERROR: Unable to open file '" << filename << "'." << std::endl;
    exit(1);
  }

  // TO DO:  
  // PARSE input file to create Abstract Syntax Tree (AST).
  // Look through each token at a time. Branch to different sections of code based on the token type.
  // We should never have to look at a previous token ever, if we do it right
  // Each helper funtion returns an Abstract Syntax Tree Node (I think)
  // Start with SET and PRINT, as seen in test cases
  // EXECUTE the AST to run your program.
  
}
