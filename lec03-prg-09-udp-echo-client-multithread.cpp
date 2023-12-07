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
		// UDP -> Connect X
		// recv & send 대신 recvfrom & sendto 사용
		sockaddr_in serverAddress;
		int serverAddrSize = sizeof(serverAddress);

		char recvData[maxBufferSize] = {};
		if (recvfrom(clientSocket, recvData, sizeof(recvData), 0, (sockaddr*)&serverAddress, &serverAddrSize) <= 0)
			return;

		cout << "> received: " << recvData << endl;

		if (strcmp(recvData, "quit") == 0)
			return;
	}
}


int main() {

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);


	SOCKET clientSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP); // UDP

	sockaddr_in clientAddress;
	clientAddress.sin_family = AF_INET;
	clientAddress.sin_addr.s_addr = inet_addr(HOST);
	clientAddress.sin_port = htons(PORT);

	cout << "> echo-client is activated" << endl;


	thread([&]() {
		RecvHandler(clientSocket);
		}).detach();


	while (true) {
		char sendMsg[maxBufferSize] = {};
		cout << "> ";
		cin >> sendMsg;
		sendto(clientSocket, sendMsg, sizeof(sendMsg), 0, (sockaddr*)&clientAddress, sizeof(clientAddress));

		if (strcmp(sendMsg, "quit") == 0)
			break;
	}


	cout << "> echo-client is de-activated" << endl;

	closesocket(clientSocket);
	WSACleanup();
	
	return 0;
}