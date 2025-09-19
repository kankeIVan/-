#include <iostream>
#include <string>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

int main() {
#ifdef _WIN32
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

	// 1.�����׽���
	int client_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (client_fd < 0)
	{
		std::cerr << "Socket creation failed\n";
		return -1;
	}

	// 2.��������ַ
	sockaddr_in serverAddr{};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(8888); // ���ö˿ں�
	// ��������
#ifdef _WIN32
	InetPton(AF_INET, L"127.0.0.1", &serverAddr.sin_addr);
#else
	inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
#endif


	// 3.���ӷ�����
	if (connect(client_fd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
	{
		std::cerr << "Connect failed\n";
		return -1;
	}
	std::cout << "Connected to server\n";

	// 4.ѭ��������Ϣ
	std::string msg;
	char buffer[1024];
	while (true)
	{
		std::cout << "Enter message:";
		std::getline(std::cin, msg);

		// ����"exit"�˳�
		if (msg == "exit")
		{
			break;
		}

		// ������Ϣ
		send(client_fd, msg.c_str(), msg.size(), 0);

		// ���ջ���
		int bytesReceived = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
		if (bytesReceived > 0)
		{
			buffer[bytesReceived] = '\0';
			std::cout << "Server echoed:" << buffer << "\n";
		}

	}

	// �رտͻ���
#ifdef _WIN32
	closesocket(client_fd);
	WSACleanup();
#else
	close(client_fd);
#endif

	return 0;
}