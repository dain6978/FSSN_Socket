#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

const char* HOST = "127.0.0.1";
const int PORT = 65456;

using namespace std;

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // 서버
    SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(HOST);
    serverAddress.sin_port = htons(PORT);

    cout << "> echo-server is activated" << endl;

    bind(serverSocket, (sockaddr*)(&serverAddress), sizeof(serverAddress));


    // 클라이언트
    sockaddr_in clientAddress;
    int clientAddrSize = sizeof(clientAddress);

    char recvData[1024] = {};

    while (true) {
        recvfrom(serverSocket, recvData, sizeof(recvData), 0, (sockaddr*)(&clientAddress), &clientAddrSize);

        cout << "> client connected by IP address " << inet_ntoa(clientAddress.sin_addr) << " with Port number " << ntohs(clientAddress.sin_port) << endl;
        cout << "> echoded: " << recvData << endl;

        sendto(serverSocket, recvData, sizeof(recvData), 0, (sockaddr*)(&clientAddress), clientAddrSize);

        if (strcmp(recvData, "quit") == 0) {
            break;
        }
    }

    cout << "> echo-server is de-activated" << endl;

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
