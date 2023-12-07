#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

const char* HOST = "127.0.0.1";
const int PORT = 65456;

using namespace std;

int main() {

	// 소켓 초기화
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	// IPv4 & TCP 기반의 서버 소켓 생성
	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// 서버 주소 설정
	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	// serverAddress.sin_addr.s_addr = INADDR_ANY; : 모든 클라이언트에 대해 연결 허용
	serverAddress.sin_addr.s_addr = inet_addr(HOST);
	serverAddress.sin_port = htons(PORT);

	cout << "> echo-server is activated" << endl;

	// 소켓에 주소 바인딩
	bind(serverSocket, (sockaddr*)(&serverAddress), sizeof(serverAddress));
	// 연결 대기 (listen)
	listen(serverSocket, SOMAXCONN);

	// 클라이언트 연결 요청 수락
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

	// 소켓 정리
	closesocket(clientSocket);
	closesocket(serverSocket);
	WSACleanup();

	return 0;
}