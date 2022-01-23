#include <process.h>
#include <Windows.h>
#include <cstdio>

unsigned WINAPI threadIns(void*);
unsigned WINAPI threadDes(void*);

long long sum = 0;
HANDLE hMutex;

int main(int argc, char* argv[]) {
	constexpr int num_thread = 50;
	HANDLE tHandles[num_thread];

	hMutex = CreateMutex(nullptr, FALSE, nullptr);
	for (int i = 0; i < num_thread; i++)
	{
		if (i % 2) {
			tHandles[i] = (HANDLE)_beginthreadex(nullptr, 0, threadIns, nullptr, 0, nullptr);
		}
		else {
			tHandles[i] = (HANDLE)_beginthreadex(nullptr, 0, threadDes, nullptr, 0, nullptr);
		}
	}
	auto wrs = WaitForMultipleObjects(num_thread, tHandles, TRUE, INFINITE);
	if (wrs == WAIT_FAILED) {
		fputs("wait thread error", stdout);
		fputc('\n', stdout);
		exit(EXIT_FAILURE);
	}

	CloseHandle(hMutex);
	printf("result: %lld\n", sum);
	return 0;
}

unsigned WINAPI threadIns(void* arg) {
	WaitForSingleObject(hMutex, INFINITE);
	for (int i = 0; i < 50000; i++)
	{
		++sum;
	}
	ReleaseMutex(hMutex);
	return 0;
}

unsigned WINAPI threadDes(void* arg) {
	WaitForSingleObject(hMutex, INFINITE);
	for (int i = 0; i < 50000; i++)
	{
		--sum;
	}
	ReleaseMutex(hMutex);
	return 0;
}