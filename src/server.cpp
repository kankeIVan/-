#include <iostream>

//跨平台实现socket支持
#ifdef _WIN32
//Windows实现socket
#include <winsock2.h>
//Windows网络API实现
#pragma comment(lib, "ws2_32.lib")
#else
//Linux/Mac实现socket
#include <sys/socket.h>
//IP地址转换函数
#include <arpa/inet.h>
#include <unistd.h>
#endif

int main() {
#ifdef _WIN32
	WSADATA wsaData; // 声明结构体，存储Windows网络库(Winsock)信息
	/*
	初始化Windows的Socket库（Winsock），必须在调用任何Socket函数前执行
	参数MAKEWORD(2,2)表示使用Winsock 2.2版本
	&wsaData是输出参数，会返回当前环境支持的网络库信息
	*/
	WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

	// 1.创建套接字
	int server_fd = socket(AF_INET, SOCK_STREAM, 0); // socket()创建套接字，AF_INET使用IPv4，SOCK_STREAM使用TCP，默认TCP
	if (server_fd < 0)
	{
		std::cerr << "Socket creation failed\n";
		return -1;
	}

	// 2.设置服务器地址结构
	sockaddr_in server_addr{};
	server_addr.sin_family = AF_INET; //协议簇IPv4
	server_addr.sin_port = htons(8888); // 监听端口（8888），htons() 转换为网络字节序
	server_addr.sin_addr.s_addr = INADDR_ANY; // 绑定服务器上所有可用的IP地址

	// 3.绑定端口
	  // bind()把刚创建的套接字server_fd和设定的IP/端口（server_addr）绑定起来
	if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
	{
		std::cerr << "Bind failed\n";
		return -1;
	}

	// 4.开始监听
	  // listen()告诉操作系统这个套接字要变成被动监听状态，等待客户端连接
	  // 设置等待队列的最大长度5（最多5个未处理的连接请求排队）
	if (listen(server_fd, 5) < 0)
	{
		std::cerr << "Listen failed\n";
		return -1;
	}

	// 5.等待客户端连接
	while (true)
	{
		sockaddr_in client_addr{};

		// 获取客户端连接
#ifdef _WIN32
		int addr_len = sizeof(client_addr);
#else
		socklen_t addr_len = sizeof(client_addr);
#endif
		// accept()会阻塞，直到有客户端请求连接
		int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
		if (client_fd < 0)
		{
			std::cerr << "Accept failed\n";
			continue;
		}
		std::cout << "Client conneted!\n";

		// 接收数据
		char buffer[1024] = { 0 }; // 最大接收1024字节
		int bytesReceived;
		// recv()用来接收客户端发来的数据，保存到buffer
		while ((bytesReceived = recv(client_fd, buffer, sizeof(buffer) - 1, 0)) > 0)
		{
			buffer[bytesReceived] = '\0'; // 结尾补上字符串结束符
			std::cout << "Received:" << buffer << "\n";
			// 回显数据
			send(client_fd, buffer, bytesReceived, 0);

		}

		if (bytesReceived == 0)
		{
			std::cout << "客户端已正常关闭连接\n";
		}
		else
		{
			std::cerr << "接收数据时发生错误\n";
		}

	// 关闭连接
#ifdef _WIN32
	closesocket(client_fd);
#else
	close(client_fd);
#endif
	}

#ifdef _WIN32
	closesocket(server_fd);
	WSACleanup();
#else
	close(server_fd);
#endif

	return 0;
}
