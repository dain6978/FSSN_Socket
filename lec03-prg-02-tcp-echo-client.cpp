#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

const char* HOST = "127.0.0.1";
const int PORT = 65456;

using namespace std;

int main() {

	cout << "> echo-client is activated" << endl;

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET clientSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	sockaddr_in clientAddress;
	clientAddress.sin_family = AF_INET;
	clientAddress.sin_addr.s_addr = inet_addr(HOST);
	clientAddress.sin_port = htons(PORT);

	connect(clientSocket, (sockaddr*)&clientAddress, sizeof(clientAddress));

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