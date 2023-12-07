#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

const char* HOST = "127.0.0.1";
const int PORT = 65456;

using namespace std;


class MyTCPSocketHandler {
public:
	// 생성자
	MyTCPSocketHandler(SOCKET clientSocket, sockaddr_in clientAddress) : clientSocket(clientSocket), clientAddress(clientAddress) {};

	// handle 함수
	void handle() {
		
		cout << "> client connected by IP adress " << inet_ntoa(clientAddress.sin_addr) << " with Port number " << ntohs(clientAddress.sin_port) << endl;

		char recvData[1024] = {};
		
		while (true) 
		{
			recv(clientSocket, recvData, sizeof(recvData), 0);
			cout << "> echoded: " << recvData << endl;
			send(clientSocket, recvData, sizeof(recvData), 0);

			if (strcmp(recvData, "quit") == 0) {
				return;
			}
		}
	}
private:
	SOCKET clientSocket;
	sockaddr_in clientAddress;
};


int main() {

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	// 서버
	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

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
	SOCKET clientSocket = accept(serverSocket, (sockaddr*)(&clientAddress), &(clientAddrSize));

	MyTCPSocketHandler handler(clientSocket, clientAddress); 
	handler.handle();

	cout << "> echo-server is de-activated" << endl;

	closesocket(clientSocket);
	closesocket(serverSocket);
	WSACleanup();

	return 0;

}