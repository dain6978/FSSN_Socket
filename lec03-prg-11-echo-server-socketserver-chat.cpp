#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <vector>
#include <WinSock2.h>
#include <vector>
#include <algorithm> // find_if

#pragma comment(lib, "ws2_32.lib")

const char* HOST = "127.0.0.1";
const int PORT = 65456;

using namespace std;

vector<SOCKADDR_IN> groupQueue;

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(HOST);
    serverAddress.sin_port = htons(PORT);

    cout << "> echo-server is activated" << endl;
    bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress));

    sockaddr_in clientAddress;
    int clientAddrSize = sizeof(clientAddress);

    char recvData[1024] = {};

    while (true) {
        recvfrom(serverSocket, recvData, sizeof(recvData), 0, reinterpret_cast<sockaddr*>(&clientAddress), &clientAddrSize);

        cout << "> client connected by IP address " << inet_ntoa(clientAddress.sin_addr) << " with Port number " << ntohs(clientAddress.sin_port) << endl;
        cout << "> echoed: " << recvData << endl;

        if (recvData[0] == '#' || strcmp(recvData, "quit") == 0) {
            if (strcmp(recvData, "#REG") == 0) {
                cout << "> client registered" << endl;
                groupQueue.push_back(clientAddress);
            }
            else if (strcmp(recvData, "#DEREG") == 0 || strcmp(recvData, "quit") == 0) {
                auto it = find_if(groupQueue.begin(), groupQueue.end(), [clientAddress](const SOCKADDR_IN& addr) {
                    return inet_ntoa(addr.sin_addr) == inet_ntoa(clientAddress.sin_addr);
                    });

                if (it != groupQueue.end()) {
                    cout << "> client de-registered" << endl;
                    groupQueue.erase(it);
                }
            }
        }
        else {
            if (groupQueue.empty()) {
                cout << "> no clients to echo" << endl;
            }
            else if (find_if(groupQueue.begin(), groupQueue.end(), [clientAddress](const SOCKADDR_IN& addr) {
                return inet_ntoa(addr.sin_addr) == inet_ntoa(clientAddress.sin_addr);
                }) == groupQueue.end()) {
                cout << "> ignores a message from an un-registered client" << endl;
            }
            else {
                cout << "> received (" << recvData << ") and echoed to " << groupQueue.size() << " clients" << endl;
                for (const auto& clientConn : groupQueue) {
                    sendto(serverSocket, recvData, strlen(recvData) + 1, 0, reinterpret_cast<const sockaddr*>(&clientConn), sizeof(clientConn));
                }
            }
        }

        if (strcmp(recvData, "quit") == 0) {
            break;
        }
    }

    cout << "> echo-server is de-activated" << endl;

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
