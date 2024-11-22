#include "ece250_socket.h"
#include <iostream>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#endif

std::string sendPostRequest(const std::string& host, const std::string& path, const std::string& data, int port) {
    std::string request;
    char buffer[4096];
    std::string response;

#ifdef _WIN32
    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;
    struct addrinfo *result = NULL, hints;
    int bytesReceived;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        return "";
    }

    // Create a socket
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed." << std::endl;
        WSACleanup();
        return "";
    }

    // Resolve the server address and port
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if (getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, &result) != 0) {
        std::cerr << "getaddrinfo failed." << std::endl;
        closesocket(sock);
        WSACleanup();
        return "";
    }

    // Connect to the server
    if (connect(sock, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR) {
        std::cerr << "Connect failed." << std::endl;
        freeaddrinfo(result);
        closesocket(sock);
        WSACleanup();
        return "";
    }

    freeaddrinfo(result);

    // Create HTTP POST request
    request = "POST " + path + " HTTP/1.1\r\n";
    request += "Host: " + host + ":" + std::to_string(port) + "\r\n";
    request += "Content-Type: application/json\r\n";
    request += "Content-Length: " + std::to_string(data.length()) + "\r\n";
    request += "Connection: close\r\n\r\n";
    request += data;

    // Send request
    if (send(sock, request.c_str(), request.length(), 0) == SOCKET_ERROR) {
        std::cerr << "Send failed." << std::endl;
        closesocket(sock);
        WSACleanup();
        return "";
    }

    // Receive response
    int received;
    while ((received = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[received] = '\0';
        response += buffer;
    }

    if (received == SOCKET_ERROR) {
        std::cerr << "Receive failed." << std::endl;
    }

    // Cleanup
    closesocket(sock);
    WSACleanup();
#else
    int sock;
    struct sockaddr_in server;
    struct hostent* he;
    int received;

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return "";
    }

    // Set socket timeout
    struct timeval timeout;
    timeout.tv_sec = 10; // 10 seconds timeout
    timeout.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

    // Get host IP
    if ((he = gethostbyname(host.c_str())) == NULL) {
        std::cerr << "gethostbyname failed." << std::endl;
        close(sock);
        return "";
    }

    // Setup server structure
    server.sin_family = AF_INET;
    server.sin_port = htons(port); // Use the specified port
    server.sin_addr = *((struct in_addr*)he->h_addr);

    // Connect to server
    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        std::cerr << "Connection failed." << std::endl;
        close(sock);
        return "";
    }

    // Create HTTP POST request
    request = "POST " + path + " HTTP/1.1\r\n";
    request += "Host: " + host + ":" + std::to_string(port) + "\r\n";
    request += "Content-Type: application/json\r\n";
    request += "Content-Length: " + std::to_string(data.length()) + "\r\n";
    request += "Connection: close\r\n\r\n";
    request += data;

    // Send request
    if (send(sock, request.c_str(), request.length(), 0) < 0) {
        std::cerr << "Send failed." << std::endl;
        close(sock);
        return "";
    }

    // Receive response
    while ((received = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[received] = '\0';
        response += buffer;
    }

    if (received < 0) {
        std::cerr << "Receive failed or timed out." << std::endl;
    }

    // Cleanup
    close(sock);
#endif

    return response;
}

std::string extractFirstLabel(const std::string& response) {
    // Define the substring patterns we're looking for
    const std::string labelsPrefix = "===";//"\"labels\":[";
    const std::string comma = "+++";//"\",";

    // Find the position of the "labels" array
    size_t labelsStart = response.find(labelsPrefix);
    if (labelsStart == std::string::npos) {
        return "";  // "labels" array not found
    }

    // Move the start position to the start of the first element in the array
    labelsStart += labelsPrefix.length();

    // Find the position of the comma after the first element
    size_t commaPos = response.find(comma, labelsStart);
    if (commaPos == std::string::npos) {
        return "";  // No comma found, invalid format
    }

    // Extract the first label
    size_t labelStart = labelsStart;  // Skip the opening quote
    size_t labelEnd = commaPos - 1;  // Skip the closing quote

    // Ensure the indices are valid and extract the substring
    if (labelStart < labelEnd) {
        return response.substr(labelStart, labelEnd - labelStart + 1);
    } else {
        return "";  // Invalid indices
    }
}

std::string labelText(const std::string& textToClassify, const std::string& candidateLabels) {
   
    std::string host = "ece-nebula01.eng.uwaterloo.ca";
    std::string path = "/classify/";
    
    std::string data = "{\"text\":\"" + textToClassify + "\",\"labels\":\"" + candidateLabels + "\"}";
    std::string response = sendPostRequest(host, path, data, 8086); // Specify port 8086
    //std::cout << extractFirstLabel(response) << std::endl;
    return extractFirstLabel(response);
    //return response;
    
}
