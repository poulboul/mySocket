// Sockets.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <sstream>
#include <iostream>

#define _WIN32_WINNT 0x501

#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;
using std::cerr;

int main()
{
	WSADATA wsaData;

	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (result != 0)
	{
		cerr << "WSAStartup failed: " << result << endl;
		return result;
	}

	struct addrinfo* addr = NULL;

	struct addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	result = getaddrinfo("127.0.0.1", "8000", &hints, &addr);

	if (result != 0)
	{
		cerr << "getaddrinfo failed: " << result << endl;
		WSACleanup();
		return 1;
	}

	int listen_socket = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
	if (listen_socket == INVALID_SOCKET)
	{
		cerr << "Error at socket: " << WSAGetLastError() << endl;
		freeaddrinfo(addr);
		WSACleanup();
		return 1;
	}

	result = bind(listen_socket, addr->ai_addr, (int)addr->ai_addrlen);
	if (result == SOCKET_ERROR)
	{
		cerr << "bind failed with error: " << WSAGetLastError() << endl;
		freeaddrinfo(addr);
		closesocket(listen_socket);
		WSACleanup();
		return 1;
	}

	if (listen(listen_socket, SOMAXCONN) == SOCKET_ERROR)
	{
		cerr << "listen failed with error: " << WSAGetLastError() << endl;
		closesocket(listen_socket);
		WSACleanup();
		return 1;
	}

	int client_socket = accept(listen_socket, NULL, NULL);
	if (client_socket == INVALID_SOCKET)
	{
		cerr << "accept failed: " << WSAGetLastError() << endl;
		closesocket(listen_socket);
		WSACleanup();
		return 1;
	}

	const int max_client_buffer_size = 1024;
	char buf[max_client_buffer_size];

	result = recv(client_socket, buf, max_client_buffer_size, 0);

	stringstream response;
	stringstream response_body;

	if (result == SOCKET_ERROR)
	{
		cerr << "recv failed: " << result << endl;
		closesocket(client_socket);
	}
	else if (result == 0)
	{
		cerr << "connection closed ..." << endl;
	}
	else if (result > 0)
	{
		buf[result] = '\0';
		response_body << "<title>Test C++ HTTP Server </title>\n"
			<< "<h1>Test page</h1>\n"
			<< "<p>This is body of the test page...</p>\n"
			<< "<h2>Request headers</h2>\n"
			<< "<pre>" << buf << "</buf>\n"
			<< "<em><small>Test C++ HTTP Serber</small></em>\n";

		response << "HTTP/1.1 200 OK\r\n"
			<< "Version: HTTP/1.1\r\n"
			<< "Content-Type: text/html; charset=utf-8\r\n"
			<< "Content-Length: " << response_body.str().length()
			<< "\r\n\r\n"
			<< response_body.str();

		result = send(client_socket, response.str().c_str(), response.str().length(), 0);

		if (result == SOCKET_ERROR)
		{
			cerr << "send failed: " << WSAGetLastError() << endl;
		}
		closesocket(client_socket);
	}	
	closesocket(listen_socket);
	freeaddrinfo(addr);
	WSACleanup();
	return 0;

}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
