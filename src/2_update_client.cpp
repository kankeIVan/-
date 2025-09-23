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

// 接收命令行参数(元素空格分割)
int main(int argc, char* argv[]) {
#ifdef _WIN32
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

	//默认服务器信息
	const char* server_ip = "127.0.0.1";
	int server_port = 8888;
	std::string client_name = "Client"; // 默认名称

	// 命令行参数设置
	if (argc >= 2) server_ip = argv[1]; // IP地址
	if (argc >= 3) server_port = std::stoi(argv[2]); // 端口号
	if (argc >= 4) client_name = argv[3]; // 客户端名称

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
	serverAddr.sin_port = htons(server_port); // 设置端口号
	// 本机测试
#ifdef _WIN32
	InetPton(AF_INET, std::wstring(server_ip, server_ip+strlen(server_ip)).c_str(), &serverAddr.sin_addr); // 将server_ip（char*类型）转换为std::wstring（宽字符），.c_str()宽字符串指针
#else
	inet_pton(AF_INET, server_ip, &serverAddr.sin_addr);
#endif


	// 3.连接服务器
	if (connect(client_fd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
	{
		std::cerr << "Connect Failed\n";
		return -1;
	}
	std::cout << client_name << " Connected to Server " << server_ip << ":" << server_port << "\n(type 'exit' to disconnect)\n";

	// 4.循环发送信息
	std::string msg;
	char buffer[1024];
	while (true)
	{
		std::cout << client_name <<" > ";
		std::getline(std::cin, msg);

		// 输入"exit"退出
		if (msg == "exit") break;

		// 发送消息
		std::string send_msg = client_name + ":" + msg;
		send(client_fd, send_msg.c_str(), send_msg.size(), 0);

		// 接收回显
		int bytesReceived = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
		if (bytesReceived > 0)
		{
			buffer[bytesReceived] = '\0';
			std::cout << "Server Echoed > " << buffer << "\n";
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
