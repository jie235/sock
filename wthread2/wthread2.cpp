#include <process.h>
#include <Windows.h>
#include <cstdio>

unsigned WINAPI ThreadFunc(void*);

int main(int argc, char* argv[]) {
	int param = 5;
	unsigned threadId;
	HANDLE hThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadFunc, (void*)&param, 0, &threadId);
	if (hThread == 0) {
		puts("_beginthreadex() error");
		return -1;
	}
	DWORD wd;
	if ((wd = WaitForSingleObject(hThread, INFINITE)) == WAIT_FAILED) {
		puts("thread wait error");
		return -1;
	}
	printf("wait result: %s\n", (wd == WAIT_FAILED) ? "signaled" : "time-out");
	puts("end of main");
	return 0;
}

unsigned WINAPI ThreadFunc(void* arg) {
	int i;
	int cnt = *((int*)arg);
	for (int  i = 0; i < cnt; i++)
	{
		Sleep(1000);
		puts("running thread");
	}
	return 0;
}