#include <iostream>

//��ƽ̨ʵ��socket֧��
#ifdef _WIN32
//Windowsʵ��socket
#include <winsock2.h>
//Windows����APIʵ��
#pragma comment(lib, "ws2_32.lib")
#else
//Linux/Macʵ��socket
#include <sys/socket.h>
//IP��ַת������
#include <arpa/inet.h>
#include <unistd.h>
#endif

using namespace std;

int main() {
#ifdef _WIN32
	WSADATA wsaData; // �����ṹ�壬�洢Windows�����(Winsock)��Ϣ
	/*
	��ʼ��Windows��Socket�⣨Winsock���������ڵ����κ�Socket����ǰִ��
	����MAKEWORD(2,2)��ʾʹ��Winsock 2.2�汾
	&wsaData������������᷵�ص�ǰ����֧�ֵ��������Ϣ
	*/
	WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

	// 1.�����׽���
	int server_fd = socket(AF_INET, SOCK_STREAM, 0); // socket()�����׽��֣�AF_INETʹ��IPv4��SOCK_STREAMʹ��TCP��Ĭ��TCP
	if (server_fd < 0)
	{
		std::cerr << "Socket creation failed\n";
		return -1;
	}

	// 2.���÷�������ַ�ṹ
	sockaddr_in server_addr{};
	server_addr.sin_family = AF_INET; //Э���IPv4
	server_addr.sin_port = htons(8888); // �����˿ڣ�8888����htons() ת��Ϊ�����ֽ���
	server_addr.sin_addr.s_addr = INADDR_ANY; // �󶨷����������п��õ�IP��ַ

	// 3.�󶨶˿�
	  // bind()�Ѹմ������׽���server_fd���趨��IP/�˿ڣ�server_addr��������
	if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
	{
		std::cerr << "Bind failed\n";
		return -1;
	}

	// 4.��ʼ����
	  // listen()���߲���ϵͳ����׽���Ҫ��ɱ�������״̬���ȴ��ͻ�������
	  // ���õȴ����е���󳤶�5�����5��δ��������������Ŷӣ�
	if (listen(server_fd, 5) < 0)
	{
		std::cerr << "Listen failed\n";
		return -1;
	}

	// 5.�ȴ��ͻ�������
	while (true)
	{
		sockaddr_in client_addr{};

		// ��ȡ�ͻ�������
#ifdef _WIN32
		int addr_len = sizeof(client_addr);
#else
		socklen_t addr_len = sizeof(client_addr);
#endif
		// accept()��������ֱ���пͻ�����������
		int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
		if (client_fd < 0)
		{
			std::cerr << "Accept failed\n";
			continue;
		}
		std::cout << "Client conneted!\n";

		// ��������
		char buffer[1024] = { 0 }; // ������1024�ֽ�
		int bytesReceived;
		// recv()�������տͻ��˷��������ݣ����浽buffer
		while ((bytesReceived = recv(client_fd, buffer, sizeof(buffer) - 1, 0)) > 0)
		{
			buffer[bytesReceived] = '\0'; // ��β�����ַ���������
			std::cout << "Received:" << buffer << "\n";
			// ��������
			send(client_fd, buffer, bytesReceived, 0);

		}

		if (bytesReceived == 0)
		{
			std::cout << "�ͻ����������ر�����\n";
		}
		else
		{
			std::cerr << "��������ʱ��������\n";
		}

	// �ر�����
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