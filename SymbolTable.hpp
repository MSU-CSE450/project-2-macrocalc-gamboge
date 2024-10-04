#pragma once

#include <assert.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>

class SymbolTable {
private:
  // CODE TO STORE SCOPES AND VARIABLES HERE.
  
  // HINT: YOU CAN CONVERT EACH VARIABLE NAME TO A UNIQUE ID TO CLEANLY DEAL
  //       WITH SHADOWING AND LOOKING UP VARIABLES LATER.
  std::vector<std::unordered_map<std::string, double>> Table {
        {{"1", 1.90}, {"2", 2.3}}
    };

public:
  // CONSTRUCTOR, ETC HERE
  SymbolTable(){}

  // FUNCTIONS TO MANAGE SCOPES (NEED BODIES FOR THESE IF YOU WANT TO USE THEM)
  void PushScope() { 
    Table.emplace_back();
  }
  void PopScope() { 
    Table.pop_back(); 
  }

  // FUNCTIONS TO MANAGE VARIABLES - LOTS MORE NEEDED
  // (NEED REAL FUNCTION BODIES FOR THESE IF YOU WANT TO USE THEM)
  bool HasVar(std::string name) const { 
    //Checks if a variable is present in the Symbol Table

    for (auto it = Table.rbegin(); it != Table.rend(); ++it)
    {
      if (it->find(name) != it->end()) {
          return true;
      }
    }
    
    return false; 
  }

  size_t AddVar(std::string name, size_t line_num) { 
    //Adds a variable to the highest scope
    auto& currScope = Table.back();
    currScope[name] = 5;
    return 0; 
  }

  double GetValue(std::string name) const {
   //Gets a key value pair in the highest scope

    assert(HasVar(name));

    for (auto it = Table.rbegin(); it != Table.rend(); ++it)
    {
      auto found = it->find(name);
      if (found != it->end()) {
        return found->second;
      }
    }

    return 0.0;
  }

  void SetValue(std::string name, double value) {
    //Updates a key value pair in the highest scope that variable is present
    assert(HasVar(name));

    for (auto it = Table.rbegin(); it != Table.rend(); ++it)
    {
      if (it->find(name) != it->end()) {
        (*it)[name] = value;
        break;
      }
    }
  }

};
