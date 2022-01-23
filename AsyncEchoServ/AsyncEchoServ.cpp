#include <WinSock2.h>
#include <Windows.h>
#include <WS2tcpip.h>
#include <process.h>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

constexpr int BuffSize = 0x01 << 8;
constexpr int QueueSize = 5;
constexpr WORD port = 9091;

void CompressSockets(SOCKET[], DWORD, DWORD);
void CompressEvents(WSAEVENT[], DWORD, DWORD);
void ErrorHandling(const char*);

int main(int argc, char* argv[]) {
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2, ), &wsaData) != 0) {
		ErrorHandling("WSAStartup() error");
	}
	SOCKET serv_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serv_sock == INVALID_SOCKET) {
		ErrorHandling("socket() error");
	}
	SOCKADDR_IN serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);
	if (bind(serv_sock, reinterpret_cast<SOCKADDR*>(&serv_addr), sizeof(serv_addr)) == SOCKET_ERROR) {
		ErrorHandling("bind() error");
	}
	if (listen(serv_sock, QueueSize) == SOCKET_ERROR) {
		ErrorHandling("listen() error");
	}

	//to store network select notification result
	WSANETWORKEVENTS netEvent;
	//temp notice event
	WSAEVENT hEvent;
	//to store events
	WSAEVENT hEvents[WSA_MAXIMUM_WAIT_EVENTS];
	//to store socket connections
	SOCKET hSocks[WSA_MAXIMUM_WAIT_EVENTS];
	DWORD numOfClntSocks(0);

	hEvent = WSACreateEvent();
	//asynchronize call, return immediately
	if (WSAEventSelect(serv_sock, hEvent, FD_ACCEPT) == SOCKET_ERROR) {
		ErrorHandling("WSAEventSelect() error");
	}
	//include server socket
	hEvents[numOfClntSocks] = hEvent;
	hSocks[numOfClntSocks] = serv_sock;
	++numOfClntSocks;

	DWORD posInfo, startIdx;
	SOCKADDR_IN clnt_addr;
	int clnt_addr_len;
	SOCKET clnt_sock;
	char msg[BuffSize];
	while (true)
	{
		posInfo = WSAWaitForMultipleEvents(numOfClntSocks, hEvents, TRUE, WSA_INFINITE, FALSE);
		startIdx = posInfo + WSA_WAIT_EVENT_0;

		//unfortunately, you need to loop every element starting from idx startIdx
		for (DWORD i = startIdx; i < numOfClntSocks; ++i) {
			DWORD sigEventIdx = WSAWaitForMultipleEvents(1, &hEvents[i], TRUE, WSA_INFINITE, FALSE);
			if ((sigEventIdx == WSA_WAIT_FAILED) || (sigEventIdx == WSA_WAIT_TIMEOUT)) {
				continue;
			}
			else
			{
				sigEventIdx = i;
				WSAEnumNetworkEvents(hSocks[sigEventIdx], hEvents[sigEventIdx], &netEvent);
				if (netEvent.lNetworkEvents == FD_ACCEPT) {
					if (netEvent.iErrorCode[FD_ACCEPT_BIT] != 0) {
						puts("Accept error");
						continue;
					}
					clnt_addr_len = sizeof(clnt_addr);
					clnt_sock = accept(hSocks[sigEventIdx], reinterpret_cast<SOCKADDR*>(&clnt_addr), &clnt_addr_len);
					if (clnt_sock == INVALID_SOCKET) {
						puts("accept() error");
						continue;
					}
					hEvent = WSACreateEvent();
					WSAEventSelect(clnt_sock, hEvent, FD_READ | FD_CLOSE);
					//update events list
					hEvents[numOfClntSocks] = hEvent;
					hSocks[numOfClntSocks] = clnt_sock;
					++numOfClntSocks;
					puts("connected new client...");
				}
				if (netEvent.lNetworkEvents & FD_READ) {
					if (netEvent.iErrorCode[FD_READ_BIT] != 0) {
						puts("Read error");
						continue;
					}
					int strLen = recv(hSocks[sigEventIdx], msg, sizeof(msg), 0);
					send(hSocks[sigEventIdx], msg, strLen, 0);
				}
				if (netEvent.lNetworkEvents & FD_CLOSE) {
					if (netEvent.iErrorCode[FD_CLOSE_BIT] != 0) {
						puts("Close error");
						continue;
					}
					WSACloseEvent(hEvents[sigEventIdx]);
					closesocket(hSocks[sigEventIdx]);
					--numOfClntSocks;
					CompressEvents(hEvents, sigEventIdx, numOfClntSocks);
					CompressSockets(hSocks, sigEventIdx, numOfClntSocks);
				}

			}
		}
	}
	WSACleanup();
	return 0;
}

void ErrorHandling(const char* msg) {
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(EXIT_FAILURE);
}

void CompressEvents(WSAEVENT hEvents[], DWORD startIdx, DWORD targetNums) {
	for (DWORD i = startIdx; i < targetNums; ++i)
	{
		hEvents[i] = hEvents[i+1];
	}
}

void CompressSockets(SOCKET hSocks[], DWORD startIdx, DWORD targetNums) {
	for (DWORD i = startIdx; i < targetNums; ++i)
	{
		hSocks[i] = hSocks[i + 1];
	}
}