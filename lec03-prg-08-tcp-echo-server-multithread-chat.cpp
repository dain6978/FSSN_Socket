#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <WinSock2.h>
#include <thread>
#include <vector>
#include <algorithm> // find_if

#pragma comment(lib, "ws2_32.lib")

using namespace std;

int activeThreadCount = 0;

struct Client {
    SOCKET clientSocket;
    sockaddr_in clientAddress;
};

vector<Client> clients;

class ThreadedTCPRequestHandler {
public:
    ThreadedTCPRequestHandler(Client client) : client(client) {};

    void handle() {
        clients.push_back(client);
        activeThreadCount++;

        SOCKET clientSocket = client.clientSocket;
        sockaddr_in clientAddress = client.clientAddress;

        cout << "> client connected by IP address " << inet_ntoa(clientAddress.sin_addr) << " with Port number " << ntohs(clientAddress.sin_port) << endl;

        while (true) {
            char recvData[1024] = {};
            recv(clientSocket, recvData, sizeof(recvData), 0);

            if (strcmp(recvData, "quit") == 0) {
                auto it = find_if(clients.begin(), clients.end(), [&](const Client& c) {
                    return c.clientSocket == clientSocket;
                    });
                if (it != clients.end()) {
                    clients.erase(it);
                }
                activeThreadCount--;
                return;
            }
            else {
                cout << "> echoed: " << recvData << " by Thread-" << activeThreadCount << endl;
                for (int i = 0; i < clients.size(); i++) {
                    send(clients[i].clientSocket, recvData, sizeof(recvData), 0);
                }

            }
        }
    }

private:
    Client client;
};

void clientHandler(Client client) {
    ThreadedTCPRequestHandler handler(client);
    handler.handle();

    cout << "> client disconnected" << endl;
    closesocket(client.clientSocket);
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

    cout << "> server loop running in thread (main thread): Thread-" << activeThreadCount << endl;

    // 클라이언트
    sockaddr_in clientAddress;
    int clientAddrSize = sizeof(clientAddress);

    thread([&]() {
        while (true) {
            SOCKET clientSocket = accept(serverSocket, (sockaddr*)(&clientAddress), &(clientAddrSize));
            if (clientSocket != INVALID_SOCKET) {
                Client client = { clientSocket, clientAddress };
                thread(clientHandler, client).detach();
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
