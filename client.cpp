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

	// 1.创建套接字
	int client_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (client_fd < 0)
	{
		std::cerr << "Socket creation failed\n";
		return -1;
	}

	// 2.服务器地址
	sockaddr_in serverAddr{};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(8888); // 设置端口号
	// 本机测试
#ifdef _WIN32
	InetPton(AF_INET, L"127.0.0.1", &serverAddr.sin_addr);
#else
	inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
#endif


	// 3.连接服务器
	if (connect(client_fd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
	{
		std::cerr << "Connect failed\n";
		return -1;
	}
	std::cout << "Connected to server\n";

	// 4.循环发送信息
	std::string msg;
	char buffer[1024];
	while (true)
	{
		std::cout << "Enter message:";
		std::getline(std::cin, msg);

		// 输入"exit"退出
		if (msg == "exit")
		{
			break;
		}

		// 发送消息
		send(client_fd, msg.c_str(), msg.size(), 0);

		// 接收回显
		int bytesReceived = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
		if (bytesReceived > 0)
		{
			buffer[bytesReceived] = '\0';
			std::cout << "Server echoed:" << buffer << "\n";
		}

	}

	// 关闭客户端
#ifdef _WIN32
	closesocket(client_fd);
	WSACleanup();
#else
	close(client_fd);
#endif

	return 0;
}