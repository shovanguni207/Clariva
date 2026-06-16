#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>

#pragma comment(lib, "ws2_32.lib")

int main() {
    // Initializing Winsock
    WSADATA wsaData;
if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
    std::cout << "Winsock initialization failed" << std::endl;
    return 1;
}
std::cout << "Winsock initialized" << std::endl;
    
    // Creating a socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

if (sock == INVALID_SOCKET) {
    std::cout << "Socket creation failed" << std::endl;
    std::cout << "Error code: " << WSAGetLastError() << std::endl; 
    WSACleanup();
    return 1;
}

    std::cout << "Socket created" << std::endl;
  
  // DNS Resolution
std::string host;
std::cout << "Enter domain (without http://): ";
std::cin >> host;
struct addrinfo hints, *result;
memset(&hints, 0, sizeof(hints));
hints.ai_family = AF_INET;
hints.ai_socktype = SOCK_STREAM;

if (getaddrinfo(host.c_str(), "80", &hints, &result) != 0) {
    std::cout << "DNS resolution failed" << std::endl;
    closesocket(sock);
    WSACleanup();
    return 1;
}
    
//Timeout
int timeout = 5000;
setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));


// Extracting IP from result
std::cout << "DNS resolved" << std::endl;  

   // Connecting to server
if (connect(sock, result->ai_addr, result->ai_addrlen) == SOCKET_ERROR){
    std::cout << "Connection failed" << std::endl;
    std::cout << "Error code: " << WSAGetLastError() << std::endl;
    freeaddrinfo(result);
    closesocket(sock);
    WSACleanup();
    return 1;
}
std::cout << "Connected to server" << std::endl;


// Sending HTTP request
std::string requestStr = "GET / HTTP/1.1\r\n"
                         "Host: " + host + "\r\n"
                         "User-Agent: Clariva/0.1\r\n"
                         "Accept: text/html\r\n"
                         "Connection: close\r\n\r\n";
const char* request = requestStr.c_str();
send(sock, request, strlen(request), 0);
std::cout << "Request sent" << std::endl;

// Receive response
char buffer[4096];
std::string response = "";
int bytesReceived;

int recvTimeout = 3000;
setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&recvTimeout, sizeof(recvTimeout));

while ((bytesReceived = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
    buffer[bytesReceived] = '\0';
    response += buffer;
}

std::cout << "Response received:" << std::endl;
std::cout << response << std::endl;

closesocket(sock);
WSACleanup();
return 0;
}
