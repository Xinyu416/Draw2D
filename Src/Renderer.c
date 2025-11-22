#include "Renderer.h"

DWORD CALLBACK MyThreadFunction(LPVOID lpParam);
CRITICAL_SECTION criticalSection;

DWORD CALLBACK MyThreadFunction(LPVOID lpParam) {
	DWORD lpThreadId = 0;
	lpThreadId = GetCurrentThreadId();
	Sleep(3000);
	for (size_t i = 0; i < 5; i++)
	{
		// 访问共享资源
		EnterCriticalSection(&criticalSection);
		// 修改共享资源...
		printf("[%d]:sleep\n", lpThreadId);
		LeaveCriticalSection(&criticalSection);
		Sleep(2000);
	}
	return 0;
}

DWORD CALLBACK MyThreadFunction2(LPVOID lpParam) {
	DWORD lpThreadId = 0;
	lpThreadId = GetCurrentThreadId();
	Sleep(3000);
	for (size_t i = 0; i < 5; i++)
	{
		EnterCriticalSection(&criticalSection);
		printf("[%d]:function2\n", lpThreadId);
		LeaveCriticalSection(&criticalSection);
		Sleep(2000);
	}
	return 0;
}

void ThreadTest() {

	DWORD lpThreadId1 = 0;
	DWORD lpThreadId2 = 0;
	InitializeCriticalSection(&criticalSection);
	HANDLE  hThread1 = CreateThread(NULL, 0, MyThreadFunction, NULL, 0, lpThreadId1);
	HANDLE  hThread2 = CreateThread(NULL, 0, MyThreadFunction2, NULL, 0, lpThreadId2);
	if (hThread1 != NULL)
	{
		WaitForSingleObject(hThread1, INFINITE);
		CloseHandle(hThread1);
	}

	if (hThread2 != NULL)
	{
		WaitForSingleObject(hThread2, INFINITE);
		CloseHandle(hThread2);
	}
	DeleteCriticalSection(&criticalSection);
	return;
}

void MakeTask() {
	const uint32_t MAX_THREADS = 10;
	DWORD* dwThreadIdArray = (DWORD*)malloc(sizeof(DWORD) * MAX_THREADS);
	HANDLE* hThreadArray = (HANDLE*)malloc(sizeof(HANDLE) * MAX_THREADS);

	for (int i = 0; i < MAX_THREADS; i++)
	{
		hThreadArray[i] = CreateThread(NULL, 0, MyThreadFunction, NULL, 0, &dwThreadIdArray[i]);
		if (hThreadArray[i] == NULL)
		{
			ExitProcess(3);
		}
	}

	WaitForMultipleObjects(MAX_THREADS, hThreadArray, TRUE, INFINITE);
	for (int i = 0; i < MAX_THREADS; i++)
	{
		CloseHandle(hThreadArray[i]);
	}
	return 0;
}