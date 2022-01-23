#include <process.h>
#include <Windows.h>
#include <cstdio>

unsigned WINAPI NumberOfA(void*);
unsigned WINAPI NumberOfOthers(void*);

constexpr int str_len = 100;
char buff[str_len];

int main(int argc, char* argv[]) {

	fputs("input string: ", stdout);
	fgets(buff, str_len, stdin);

	HANDLE eHandle = CreateEvent(nullptr, true, false, nullptr);
	HANDLE tHandle1 = (HANDLE)_beginthreadex(nullptr, 0, NumberOfA, (void*)(&eHandle), 0, nullptr);
	HANDLE tHandle2 = (HANDLE)_beginthreadex(nullptr, 0, NumberOfOthers, (void*)(&eHandle), 0, nullptr);
	//threads wait(in thread's main function, has a event requirement), set event to signaled
	SetEvent(eHandle);

	WaitForSingleObject(tHandle1, INFINITE);
	WaitForSingleObject(tHandle2, INFINITE);
	ResetEvent(eHandle);
	CloseHandle(eHandle);
	return 0;
}

unsigned WINAPI NumberOfA(void* arg) {
	HANDLE eHandle = *((HANDLE*)arg);
	int cnt = 0;
	WaitForSingleObject(eHandle, INFINITE);
	for (int i = 0; buff[i] != 0; i++)
	{
		if (buff[i] == 'A') {
			cnt++;
		}
	}
	//manual-reset mode, if reset stat here, the other thread may be blocked.
	//another view point: both thread do not change the critical session's value, the lock is not necessary
	printf("Number of A: %d\n", cnt);
	return 0;
}

unsigned WINAPI NumberOfOthers(void* arg) {
	HANDLE eHandle = *((HANDLE*)arg);
	int cnt = 0;
	WaitForSingleObject(eHandle, INFINITE);
	for (int i = 0; buff[i] != 0; i++)
	{
		if (buff[i] != 'A')cnt++;

	}
	//manual-reset mode, if reset stat here, the other thread may be blocked.
	//another view point: both thread do not change the critical session's value, the lock is not necessaryk
	printf("Number of others: %d\n", cnt);
	return 0;
}