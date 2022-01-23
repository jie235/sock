#include <Winsock2.h>
#include <Windows.h>
//Winsock2.h must included before Windows.h if want to use Winsock2.h instead of winsock.h
#include <process.h>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

void ErrorHandling(const char*);
unsigned WINAPI HandleClnt(void*);
void SendMsg(char[], int);

constexpr int QueueSize = 5;
constexpr WORD port = 9091;
constexpr int BuffSize = 0x01 << 8;
constexpr int MaxClnt = 50;
HANDLE hMutex;
SOCKET clnt_socks[MaxClnt];
int clntCnt = 0;

int main(int argc, char* argv[]) {

	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	SOCKET serv_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serv_sock == INVALID_SOCKET) {
		ErrorHandling("socket() error");
	}

	SOCKADDR_IN serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);

	if (bind(serv_sock, reinterpret_cast<sockaddr*>(&serv_addr), sizeof(serv_addr)) == SOCKET_ERROR) {
		ErrorHandling("bind() error");
	}
	if (listen(serv_sock, QueueSize) == SOCKET_ERROR) {
		ErrorHandling("listen() error");
	}

	HANDLE tHandle;
	SOCKET clnt_sock;
	SOCKADDR_IN clnt_addr;
	int clnt_addr_len = sizeof(clnt_addr);
	while (true)
	{
		clnt_sock = accept(serv_sock, reinterpret_cast<SOCKADDR*>(&clnt_addr), &clnt_addr_len);
		if (clnt_sock == INVALID_SOCKET) {
			continue;
		}
		WaitForSingleObject(hMutex, INFINITE);
		clnt_socks[clntCnt++] = clnt_sock;
		ReleaseMutex(hMutex);
		tHandle = (HANDLE)_beginthreadex(nullptr, 0, HandleClnt, (void*)(clnt_sock), 0, nullptr);
		printf("Connected client: %d\n", tHandle);
	}
	closesocket(serv_sock);
	WSACleanup();
	return 0;
}

unsigned WINAPI HandleClnt(void* arg) {
	SOCKET clnt_sock = (SOCKET)arg;
	int strLen = 0;
	char message[BuffSize];
	//read data, and send data to all clients
	while ((strLen = recv(clnt_sock, message, BuffSize, 0)) != 0)
	{
		SendMsg(message, strLen);
	}
	//close current client
	WaitForSingleObject(hMutex, INFINITE);
	for (int i = 0; i < clntCnt; i++)
	{
		if (clnt_sock == clnt_socks[i]) {
			//first add one, go over the equivalent value of 'clnt_sock', pay attention to the array boundry
			while (i++<clntCnt-1)
			{
				//maybe out of the bountry
				clnt_socks[i] = clnt_socks[i + 1];
			}
			break;
		}
	}
	--clntCnt;
	ReleaseMutex(hMutex);
	closesocket(clnt_sock);
	return 0;
}

void SendMsg(char msg[], int strLen) {
	//function 'send' uses critical session: clnt_socks, so here need a mutex
	WaitForSingleObject(hMutex, INFINITE);
	for (int i = 0; i < clntCnt; i++)
	{
		send(clnt_socks[i], msg, strLen, 0);
	}
	ReleaseMutex(hMutex);
}

void ErrorHandling(const char* msg) {
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(EXIT_FAILURE);
}