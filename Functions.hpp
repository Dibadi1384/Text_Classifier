#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include <string>
#include "Node.hpp"
#include "Illegal_exception.hpp"

// Global Variables
extern Node* root;
extern int numClassifications;


// Main Function Declarations
void LOAD(const std::string& filename);
void INSERT(const std::string& classification);
void CLASSIFY(const std::string& input);
void ERASE(const std::string& classification);
void PRINT();
void EMPTY();
void CLEAR();
void SIZE();
void EXIT();



// Helper Function Declarations
void printClassifications(Node* currentNode, std::string path, std::vector<std::string>& output); 
void clearNode(Node* currentNode);

#endif 
