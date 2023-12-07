#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

const char* HOST = "127.0.0.1";
const int PORT = 65456;

using namespace std;

int main() {

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		cerr << "> Failed to start up WSA" << endl;
		return 1;
	}

	SOCKET clientSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET) {
		cerr << "Failed to create client socket" << endl;
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}

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

	while (true) {
		cout << "> ";
		char sendMsg[1024] = {};
		cin >> sendMsg;
		send(clientSocket, sendMsg, sizeof(sendMsg), 0);

		char recvData[1024] = {};
		recv(clientSocket, recvData, sizeof(recvData), 0);
		cout << "> received: " << recvData << endl;

		if (strcmp(sendMsg, "quit") == 0) {
			break;
		}
	}

	closesocket(clientSocket);
	WSACleanup();

	cout << "> echo-client is de-activated" << endl;

	return 0;
}