#include <Windows.h>
#include <process.h>
#include <cstdio>

unsigned WINAPI tRead(void*);
unsigned WINAPI tAccu(void*);

int sum = 0;
int num;

int main(int argc, char* argv[]) {
	HANDLE sems[2];
	sems[0] = CreateSemaphore(nullptr, 0, 1, nullptr);
	sems[1] = CreateSemaphore(nullptr, 1, 1, nullptr);
	HANDLE threads[2];
	threads[0] = (HANDLE)_beginthreadex(nullptr, 0, tRead, (void*)sems, 0, nullptr);
	threads[1] = (HANDLE)_beginthreadex(nullptr, 0, tAccu, (void*)sems, 0, nullptr);
	WaitForMultipleObjects(2, threads, true, INFINITE);
	CloseHandle(sems[0]);
	CloseHandle(sems[1]);
	printf("result: %d\n", sum);
	return 0;
}

unsigned WINAPI tRead(void* arg) {
	HANDLE* sems = (HANDLE*)arg;
	for (int i = 0; i < 5; i++)
	{
		WaitForSingleObject(sems[1], INFINITE);
		fputs("input number: ", stdout);
		scanf_s("%d", &num);
		ReleaseSemaphore(sems[0], 1, nullptr);
	}
	return 0;
}

unsigned WINAPI tAccu(void* arg) {
	//if the read function uses a loop, here also need a loop with same times, otherwise the other thread
	//will always waiting for semaphore from this thread with this function executed which is already returned
	HANDLE* sems = (HANDLE*)arg;
	for (int i = 0; i < 5; i++)
	{
		WaitForSingleObject(sems[0], INFINITE);
		sum += num;
		ReleaseSemaphore(sems[1], 1, nullptr);
	}
	return 0;
}