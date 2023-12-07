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

	// ����
	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket == INVALID_SOCKET) {
		cerr << "Failed to create server socket" << endl;
		WSACleanup();
		return 1;
	}

	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr(HOST);
	serverAddress.sin_port = htons(PORT);

	cout << "> echo-server is activated" << endl;

	try 
	{
		if (bind(serverSocket, (sockaddr*)(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR) {
			cerr << "> bind() failed and program terminated" << endl;
			closesocket(serverSocket);
			WSACleanup();
			return 1;
		}
	}
	catch (const exception& error) 
	{
		cerr << "> bind() failed by exception: " << error.what() << endl;
		closesocket(serverSocket);
		WSACleanup();
		return 1;
	}
	
	if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
		cerr << "> listen() failed and program terminated" << endl;
		closesocket(serverSocket);
		WSACleanup();
		return 1;
	}
	

	// Ŭ���̾�Ʈ
	sockaddr_in clientAddress;
	int clientAddrSize = sizeof(clientAddress);
	SOCKET clientSocket = accept(serverSocket, (sockaddr*)(&clientAddress), &(clientAddrSize));

	if (clientSocket == INVALID_SOCKET) {
		cerr << "Failed to connect client socket" << endl;
		closesocket(serverSocket);
		WSACleanup();
		return 1;
	}


	cout << "> client connected by IP address " << inet_ntoa(clientAddress.sin_addr)
		<< " with Port number " << ntohs(clientAddress.sin_port) << endl;

	char recvData[1024] = {};

	while (true) {
		recv(clientSocket, recvData, sizeof(recvData), 0);
		cout << "> echoded: " << recvData << endl;
		send(clientSocket, recvData, sizeof(recvData), 0);

		if (strcmp(recvData, "quit") == 0) {
			break;
		}
	}

	cout << "> echo-server is de-activated" << endl;

	closesocket(clientSocket);
	closesocket(serverSocket);
	WSACleanup();

	return 0;
}