#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <WinSock2.h>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

const char* HOST = "127.0.0.1";
const int PORT = 65456;
const int maxBufferSize = 1024;

using namespace std;


// {CHAT#1} Create a separate receive handler
void RecvHandler(SOCKET clientSocket) {

    while (true) {
		char recvData[maxBufferSize] = {};
		if (recv(clientSocket, recvData, sizeof(recvData), 0) <= 0)
			return;

        cout << "> received: " << recvData << endl;

		if (strcmp(recvData, "quit") == 0)
			return;
    }
}

int main() {
   
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);


    SOCKET clientSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	sockaddr_in clientAddress;
	clientAddress.sin_family = AF_INET;
	clientAddress.sin_addr.s_addr = inet_addr(HOST);
	clientAddress.sin_port = htons(PORT);

	cout << "> echo-client is activated" << endl;

	try
	{
		if (connect(clientSocket, (sockaddr*)&clientAddress, sizeof(clientAddress)) == SOCKET_ERROR) {
			cerr << "> connect() failed ad program terminated" << endl;
			closesocket(clientSocket);
			WSACleanup();
			return 1;
		}
	}
	catch (const exception& error) {
		cerr << "> connect() failed by exception: " << error.what() << endl;
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}


	// {CHAT#2} Create a receive handler and set to exit the thread, then execute the thread
	thread([&]() {
		RecvHandler(clientSocket);
		}).detach();


    while (true) {

		char sendMsg[maxBufferSize] = {};

		cout << "> ";
		cin >> sendMsg;
		
		send(clientSocket, sendMsg, sizeof(sendMsg), 0);

        if (strcmp(sendMsg, "quit") == 0) {
            break;
        }
    }


	cout << "> echo-client is de-activated" << endl;

	closesocket(clientSocket);
	WSACleanup();

	return 0;
}