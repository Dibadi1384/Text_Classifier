#ifndef MY_SOCKET_H
#define MY_SOCKET_H

#include <string>

// Function to send a POST request and return the response
std::string sendPostRequest(const std::string& host, const std::string& path, const std::string& data, int port);

// Function to extract the first label from a JSON response
std::string extractFirstLabel(const std::string& response);

// Function to classify text and return the first label
std::string labelText(const std::string& textToClassify, const std::string& candidateLabels);

#endif // MY_SOCKET_H
