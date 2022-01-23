#include <process.h>
#include <Windows.h>
#include <cstdio>

unsigned WINAPI threadIns(void*);
unsigned WINAPI threadDes(void*);
CRITICAL_SECTION cs;
long long num = 0;

int main(int arc, char* argv[]) {
	constexpr int num_thread = 50;
	HANDLE threads[num_thread];

	printf("expected result: %lld\n", num);
	InitializeCriticalSection(&cs);
	for (int i = 0; i < num_thread; i++)
	{
		if (i % 2) {
			threads[i] = (HANDLE)_beginthreadex(nullptr, 0, threadIns, nullptr, 0, nullptr);
		}
		else {
			threads[i] = (HANDLE)_beginthreadex(nullptr, 0, threadDes, nullptr, 0, nullptr);
		}
	}
	auto wrs = WaitForMultipleObjects(num_thread, threads, TRUE, INFINITE);
	if (wrs == WAIT_FAILED) {
		fputs("wait thread error", stdout);
		fputc('\n', stdout);
		exit(EXIT_FAILURE);
	}
	DeleteCriticalSection(&cs);
	//the value must be 1125000 (25* 50000 - 25* 5000)
	printf("result: %lld\n", num);
	return 0;
}

unsigned WINAPI threadIns(void* arg) {
	EnterCriticalSection(&cs);
	for (int i = 0; i < 50000; i++) {
		++num;
	}
	LeaveCriticalSection(&cs);
	return 0;
}

unsigned WINAPI threadDes(void* arg) {
	EnterCriticalSection(&cs);
	for (int i = 0; i < 5000; i++)
	{
		--num;
	}
	LeaveCriticalSection(&cs);
	return 0;
}