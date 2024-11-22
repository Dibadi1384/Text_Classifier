#ifndef _Node_HPP_
#define _Node_HPP_

#include <iostream>
#include <vector>

class Node {
  public:
    // Constructor
    Node(std::string name);

    // Destructor
    ~Node();


    //public member functions
    std::string className; 
    std::vector<std::pair<std::string, Node*>> Children; 
};
#endif 

