#include <WinSock2.h>
#include <Windows.h>
#include <ws2tcpip.h>
#include <iostream>
#include <process.h>

#pragma comment(lib, "ws2_32.lib")

void ErrorHandling(const char*);
unsigned WINAPI tRead(void*);
unsigned WINAPI tWrite(void*);

constexpr const char* address = "127.0.0.1";
constexpr int BuffSize = 0x01 << 8;
constexpr WORD port = 9091;

int main(int argc, char* argv[]) {
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) {
		ErrorHandling("socket() error");
	}

	SOCKADDR_IN serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	inet_pton(AF_INET, address, &serv_addr.sin_addr.s_addr);
	serv_addr.sin_port = htons(port);

	if (connect(sock, reinterpret_cast<sockaddr*>(&serv_addr), sizeof(serv_addr)) == -1) {
		ErrorHandling("connect() error");
	}
	
	HANDLE rHandle, wHandle;
	rHandle = (HANDLE)_beginthreadex(nullptr, 0, tRead, (void*)(&sock), 0, nullptr);
	wHandle = (HANDLE)_beginthreadex(nullptr, 0, tWrite, (void*)(&sock), 0, nullptr);

	WaitForSingleObject(rHandle, INFINITE);
	WaitForSingleObject(wHandle, INFINITE);
	closesocket(sock);
	WSACleanup();
	return 0;
}

unsigned WINAPI tRead(void* arg) {
	SOCKET sock = *((SOCKET*)arg);
	char msg[BuffSize];
	int str_len = 0;
	while (true)
	{
		str_len = recv(sock, msg, BuffSize - 1, 0);
		if (str_len == -1) {
			return -1;
		}
		msg[str_len] = 0;
		puts(msg);
	}
	shutdown(sock, SD_RECEIVE);
	return 0;
}

unsigned WINAPI tWrite(void* arg) {
	SOCKET sock = *((SOCKET*)arg);
	char msg[BuffSize];
	while (true)
	{
		fputs("input message (Q to quit): ", stdout);
		fgets(msg, BuffSize, stdin);
		if (!strcmp(msg, "Q\n") || !strcmp(msg, "q\n")) {
			break;
		}
		send(sock, msg, strlen(msg), 0);

	}
	shutdown(sock, SD_SEND);
	return 0;
}

void ErrorHandling(const char* msg) {
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(EXIT_FAILURE);
}