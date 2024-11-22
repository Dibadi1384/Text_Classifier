#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include "Node.hpp"
#include "Illegal_exception.hpp"
#include "Functions.hpp"
#include "ece250_socket.h"

int main() {
    std::string input;

    while (std::cin >> input) {
        if (input == "LOAD") {
            std::string filename;
            std::cin >> filename;
            LOAD(filename);
        } 
        else if (input == "INSERT") {
            std::string classification;
            std::cin.ignore(); 
            std::getline(std::cin, classification);  
            INSERT(classification);
        } 
        else if (input == "CLASSIFY") {
            std::string inputText;
            std::cin.ignore(); 
            std::getline(std::cin, inputText); 
            CLASSIFY(inputText);
        } 
        else if (input == "ERASE") {
            std::string classification;
            std::cin.ignore(); 
            std::getline(std::cin, classification);  
            ERASE(classification);
        } 
        else if (input == "PRINT") {
            PRINT();
        } 
        else if (input == "EMPTY") {
            EMPTY();
        } 
        else if (input == "CLEAR") {
            CLEAR();
        } 
        else if (input == "SIZE") {
            SIZE();
        } 
        else if (input == "EXIT") {
            EXIT();
            break; 
        }
    }

    return 0;
}
