#include <iostream>

// C++线程库
#include <thread> 
//#include <atomic>

// 跨平台通信
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

//std::atomic<bool> running(true); //运行标志

// 客户端处理函数
void handleClient(int client_fd, sockaddr_in client_addr) {
	char ip_str[INET_ADDRSTRLEN];
#ifdef _WIN32
	InetNtopA(AF_INET, &client_addr.sin_addr, ip_str, sizeof(ip_str));
#else
	inet_ntop(AF_INET, &client_addr.sin_addr, ip_str, sizeof(ip_str));
#endif
	int port = ntohs(client_addr.sin_port);

	std::cout << "New Client Connected: [" << ip_str << ":" << port << "]" << std::endl;

// 接收数据
	char buffer[1024]; 
	int bytesReceived;

	// recv()用来接收客户端发来的数据，保存到buffer
	while ((bytesReceived = recv(client_fd, buffer, sizeof(buffer) - 1, 0)) > 0)
	{
		buffer[bytesReceived] = '\0'; 
		std::cout << "[" << ip_str << ":" << port << "] Send > " << buffer << "\n";
		// 回显数据
		send(client_fd, buffer, bytesReceived, 0);

	}

	if (bytesReceived == 0)
	{
		std::cout << "Client [" << ip_str << ":" << port << "] Disconnected\n";
	}
	else
	{
		std::cerr << "ERROR: Data Reception Error\n";
	}

	// 关闭连接
#ifdef _WIN32
	closesocket(client_fd);
#else
	close(client_fd);
#endif
}

/*
// 输入命令线程
void commandListener() {
	std::string cmd;
	while (running)
	{
		std::cin >> cmd;
		if (cmd == "quit")
		{
			running = false;
			break;
		}
	}
}
*/

int main() {
#ifdef _WIN32
	WSADATA wsaData; 
	WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

	// 1.创建套接字
	int server_fd = socket(AF_INET, SOCK_STREAM, 0); // socket()创建套接字，AF_INET使用IPv4，SOCK_STREAM使用TCP，默认TCP
	if (server_fd < 0)
	{
		std::cerr << "Socket Creation Failed\n";
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
	std::cout << "Server Started, Waiting for Connection...\n";
	//std::cout << "(type 'quit' to stop)\n";
	//std::thread cmdThread(commandListener); // 启动命令监听线程
	//while (running)
	while(true)
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
			//if (running) std::cerr << "Accept failed\n";
			std::cerr << "Accept failed\n";
			continue;
		}
		std::cout << "Client conneted!\n";

		// 为每个客户端创建线程
		std::thread t(handleClient, client_fd, client_addr);
		t.detach();
	}
	//std::cout << "Server Shut Down\n";

		

#ifdef _WIN32
	closesocket(server_fd);
	WSACleanup();
#else
	close(server_fd);
#endif

	//cmdThread.join();
	return 0;
}
