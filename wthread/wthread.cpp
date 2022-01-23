#include <process.h>
#include <Windows.h>
#include <cstdio>

unsigned WINAPI ThreadFunc(void* arg);

int main() {
	HANDLE hThread;
	unsigned threadId;
	int param = 5;
	hThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadFunc, (void*)&param, 0, &threadId);
	if (hThread == 0) {
		puts("_beginghreadex() error");
		return -1;
	}
	Sleep(3000);
	puts("end of main");
	return 0;
}

//thread destroyed after the main function returns
unsigned WINAPI ThreadFunc(void* arg) {
	int cnt = *((int*)arg);
	for (int i = 0; i < cnt; i++)
	{
		Sleep(1000);
		puts("running thread");
	}
	return 0;
}