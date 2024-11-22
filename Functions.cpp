#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include "Node.hpp"
#include "Functions.hpp"
#include "Illegal_exception.hpp"
#include "ece250_socket.h"


//Global Variables
Node *root=new Node("root"); 
int numClassifications=0;

void LOAD(const std::string& filename) {

    //read each line from the texfile
    std::ifstream file(filename);
    std::string classification;

    while (std::getline(file, classification)) {

        //read each class in the line
        std::stringstream ss(classification);
        std::string Class;
        Node* currentNode = root; //assign the current node to root

        while (std::getline(ss, Class, ',')) {

            bool classFound = false;
            for (auto& child : currentNode->Children) { //check if class already exists as a child of current node
                if (child.first == Class) {
                    currentNode = child.second; //update current node, traverse to next level of trie
                    classFound = true; 
                    break;
                }
            }

            //if class was not in trie, insert it at the currect level, traverse to the next level of trie
            if (!classFound) {
                Node* newNode = new Node(Class);
                currentNode->Children.push_back({Class, newNode});
                currentNode = newNode;
            }            
        }

        numClassifications+=1;   //update num classifications
    }

    std::cout<<"success"<<std::endl;
}

void INSERT(const std::string& classification) {
    try {
        std::stringstream ss(classification);
        std::string Class;

        Node* currentNode = root;
        int pathlen = 0; //number of the classes that already exists in classification path
        int numclasses = 0; //total number of classes in given classfifcation

        //parse through each class in the classification path
        while (std::getline(ss, Class, ',')) {
            //check for illegal_exceptions
            for (char c : Class) {
                if (std::isupper(c)) {
                    throw illegal_exception();  //throw an exception if uppercase is found
                }
            }

            //if no expetions, continue with the trie traversal 
            bool classFound = false;
            numclasses += 1; //increment the number of classes in given classification

            for (auto& child : currentNode->Children) { //check if class already exists as a child of current node
                if (child.first == Class) { 
                    currentNode = child.second; //update current node, traverse to next level of trie
                    classFound = true;
                    pathlen += 1; //increment the number of classes already existing in the path
                }
            }


            //if class was not in trie, insert it at the currect level, traverse to the next level of trie
            if (!classFound) {
                Node* newNode = new Node(Class);
                currentNode->Children.push_back({Class, newNode});
                currentNode = newNode;
            }         
        }

        //if classification already existed, number of existing classes==number of given classes
        if (pathlen == numclasses) {
            std::cout << "failure" << std::endl;
            return;
        }

        //otherwise, the classification was inserted
        numClassifications+=1;    //update num classifications
        std::cout << "success" << std::endl;
    }
    
    //if an illegal_excpetion was found
    catch (const illegal_exception& e) {
        std::cout <<"illegal argument"<< std::endl; 
    }
}

void CLASSIFY(const std::string& input){    
    std::string path; //path that will be output

    try {
        //check for illegal_exceptions
        for (char c : input) {
            if (std::isupper(c)) {
                throw illegal_exception();  //throw an exception if uppercase is found
            }
        }

        //traversing the trie
        Node* currentNode = root;

        //if tree is empty
        if(currentNode->Children.empty()){
            std::cout<<"failure"<<std::endl;
            return;
        }

       //else traverse the trie
        while (!currentNode->Children.empty()) {
            std::string candidate_labels;
            bool wasFound=false;

            //make a string of all candidate classes in that level
            for (auto& child : currentNode->Children) { 
                if(!candidate_labels.empty()){
                    candidate_labels += "," + child.first;
                }
                else{
                    candidate_labels += child.first;
                }
            }

            //call classify for that level
            std::string classNode = labelText(input, candidate_labels);

            //update current node based on the returned classification
            for (auto& child : currentNode->Children) {
                if (child.first == classNode) {
                    wasFound=true;
                    if(path.empty()){
                        path+=child.first; 
                    }
                    
                    else{
                        path+=","+child.first;
                    }

                    currentNode = child.second; //updates current node
                    break;
                }
            }

            //if the output label wasn't in tree return that path so far
            if(!wasFound){
                std::cout<<path<<std::endl;      
                return;

            }
        }
        std::cout<<path<<std::endl;      
    }

    //if an illegal_excpetion was found
    catch (const illegal_exception& e) {
        std::cout <<"illegal argument" << std::endl;
    }
}

void ERASE(const std::string& classification) {
    try {
        std::stringstream ss(classification);
        std::string Class;
        std::vector<std::pair<Node*, std::string>> path; //to keep track of nodes and class names

        Node* currentNode = root;

        //parse through each class in 
        while (std::getline(ss, Class, ',')) {
            //check for illegal_exceptions
            for (char c : Class) {
                if (std::isupper(c)) {
                    throw illegal_exception(); //throw an exception if uppercase is found
                }
            }

            bool classFound = false; 

            //search for the current class in the current node's children
            for (auto& child : currentNode->Children) {
                if (child.first == Class) {
                    path.push_back({currentNode, Class}); //record current node and class name
                    currentNode = child.second;          //move to the child node
                    classFound = true;
                    break;
                }
            }

            //if class is not found classifiction doesn't exists
            if (!classFound) {
                std::cout << "failure" << std::endl;
                return;
            }
        }

        //if all classes  in the given classification path for erase existed

        //check if the last class to be erase is NOT a terminal node(trying to erase internal nodes)
        if (!currentNode->Children.empty()) {
            std::cout << "failure" << std::endl;
            return;
        }

        //if the last class was a terminal node, delete the node

        //get the parent node and class name of the last node
        std::pair<Node*, std::string> lastParent = path.back();
        Node* parentNode = lastParent.first;
        std::string className = lastParent.second;

        /*
            CITATION:
            I used chatGPT to give me a code where I could find an efficient way to find the terminal node's position 
            in the children vector in order to be able to use erase. Lines 241-246.
        */

        //find the last node
        auto childIt = std::find_if(
            parentNode->Children.begin(),
            parentNode->Children.end(),
            [&](const std::pair<std::string, Node*>& child) {
                return child.first == className;
            });

        //erase the terminal node from the children of parent node
        if (childIt != parentNode->Children.end()) {
            Node* leafNode = childIt->second;
            delete leafNode;
            parentNode->Children.erase(childIt);

            numClassifications -= 1;  //decrement size
            std::cout << "success" << std::endl;
        } 
        
        // else {
        //     std::cout << "failure" << std::endl; // This case shouldn't occur with correct path validation
        // }
    } 
    
    //if an illegal_excpetion was found
    catch (const illegal_exception& e) {
        std::cout << "illegal argument" << std::endl;
    }
}

void PRINT() {
    std::vector<std::string> classifications;  //to store the output

    //call the recursive function
    printClassifications(root, "", classifications);

    //if tree is empty
    if (classifications.empty()) {
        std::cout << "trie is empty" << std::endl;
    } 
    
    //otherwise print all the classifications, seprated by "_"
    else {
        for (int i = 0; i < classifications.size(); i++) {
            std::cout<< classifications[i]<<"_"; 
        }
        std::cout<< std::endl;
    }
}


void EMPTY(){
    if(root->Children.empty()==true){
        std::cout << "empty 1" << std::endl;
    }

    else{
        std::cout << "empty 0" << std::endl;
    }
}

void CLEAR(){
    //clear all nodes from the root
    clearNode(root);

    //reset root node to empty state
    root->Children.clear();

    numClassifications=0;  //reset the number of classifications
    std::cout << "success" << std::endl;
}

void SIZE(){
    std::cout << "number of classifications is "<<numClassifications<< std::endl;
}

void EXIT(){
    //clear the trie prevent memory leaks
    clearNode(root);

    delete root;
    root = nullptr;
}

/////helper functions
void printClassifications(Node* currentNode, std::string path, std::vector<std::string>& classifications) {
    //if the node has no children, it is a terminal node (end of a classification path)
    if (currentNode->Children.empty()) {
        if (!path.empty()) {
            classifications.push_back(path);  //add the full classification path to the vector
        }
        return;
    }

    //for all the children of the given node
    for (const auto& child : currentNode->Children) {
        std::string newPath;

        //if node is the first node for that classification
        if(path.empty()){
            newPath = child.first; //add the node to the classification path
        }

        //if other nodes already existed in the path
        else{
           newPath = path + "," + child.first; //add the node to the classification path
        }

        printClassifications(child.second, newPath, classifications); //call print again for that node
    }
}

void clearNode(Node* currentNode) {

    //for all the children in the given node
    for (auto& child : currentNode->Children) {

        clearNode(child.second); //call clear node for that node again
        delete child.second;     //delete the node
    }
    currentNode->Children.clear();  //clear the children vector
}

///Class
Node::Node(std::string name){
    className=name;
}

Node::~Node(){}



