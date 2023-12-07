#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <WinSock2.h>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

int activeThreadCount = 0;

class ThreadedTCPRequestHandler {
public:
    ThreadedTCPRequestHandler(SOCKET clientSocket, sockaddr_in clientAddress) : clientSocket(clientSocket), clientAddress(clientAddress) {};

    void handle() {
        cout << "> client connected by IP address " << inet_ntoa(clientAddress.sin_addr) << " with Port number " << ntohs(clientAddress.sin_port) << endl;
        activeThreadCount++;

        while (true) {
            char recvData[1024] = {};
            recv(clientSocket, recvData, sizeof(recvData), 0);
            cout << "> echoed: " << recvData << " by Thread-" << activeThreadCount << endl;
            send(clientSocket, recvData, sizeof(recvData), 0);

            if (strcmp(recvData, "quit") == 0) {
                activeThreadCount--;
                return;
            }
        }
    }

private:
    SOCKET clientSocket;
    sockaddr_in clientAddress;
};

void clientHandler(SOCKET clientSocket, sockaddr_in clientAddress) {
    ThreadedTCPRequestHandler handler(clientSocket, clientAddress);
    handler.handle();

    cout << "> client disconnected" << endl;
    closesocket(clientSocket);
}

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // 서버
    const char* HOST = "127.0.0.1";
    int PORT = 65456;

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    activeThreadCount++; 

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(HOST);
    serverAddress.sin_port = htons(PORT);

    cout << "> echo-server is activated" << endl;

    bind(serverSocket, (sockaddr*)(&serverAddress), sizeof(serverAddress));
    listen(serverSocket, SOMAXCONN);

    // 클라이언트
    sockaddr_in clientAddress;
    int clientAddrSize = sizeof(clientAddress);

    cout << "> server loop running in thread (main thread): Thread-" << activeThreadCount << endl;

    thread([&]() {
        while (true) {
            SOCKET clientSocket = accept(serverSocket, (sockaddr*)(&clientAddress), &(clientAddrSize));
            if (clientSocket != INVALID_SOCKET) {
                thread(clientHandler, clientSocket, clientAddress).detach();
            }
        }
    }).detach();

    int baseThreadNumber = activeThreadCount;

    while (true) {
        char msg[1024] = {};
        cin >> msg;
        if (strcmp(msg, "quit") == 0) {
            if (baseThreadNumber == activeThreadCount) {
                cout << "> stop procedure started" << endl;
                break;
            }
        else
                cout << "> active threads are remained : " << activeThreadCount - baseThreadNumber << " threads" << endl;
        }
    }

    cout << "> echo-server is de-activated" << endl;

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
