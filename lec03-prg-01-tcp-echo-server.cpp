#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

const char* HOST = "127.0.0.1";
const int PORT = 65456;

using namespace std;

int main() {

	// ���� �ʱ�ȭ
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	// IPv4 & TCP ����� ���� ���� ����
	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// ���� �ּ� ����
	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	// serverAddress.sin_addr.s_addr = INADDR_ANY; : ��� Ŭ���̾�Ʈ�� ���� ���� ���
	serverAddress.sin_addr.s_addr = inet_addr(HOST);
	serverAddress.sin_port = htons(PORT);

	cout << "> echo-server is activated" << endl;

	// ���Ͽ� �ּ� ���ε�
	bind(serverSocket, (sockaddr*)(&serverAddress), sizeof(serverAddress));
	// ���� ��� (listen)
	listen(serverSocket, SOMAXCONN);

	// Ŭ���̾�Ʈ ���� ��û ����
	sockaddr_in clientAddress;
	int clientAddrSize = sizeof(clientAddress);
	SOCKET clientSocket = accept(serverSocket, (sockaddr*)(&clientAddress), &(clientAddrSize));

	cout << "> client connected by IP address " << inet_ntoa(clientAddress.sin_addr) 
		<< " with Port number " << ntohs(clientAddress.sin_port) << endl;
	
	char recvData[1024] = {};

	// echo
	while (true) {
		// start
		recv(clientSocket, recvData, sizeof(recvData), 0);
		cout << "> echoded: " << recvData << endl;
		send(clientSocket, recvData, sizeof(recvData), 0);

		// end
		if (strcmp(recvData, "quit") == 0) {
			break;
		}
	}

	cout << "> echo-server is de-activated" << endl;

	// ���� ����
	closesocket(clientSocket);
	closesocket(serverSocket);
	WSACleanup();

	return 0;
}