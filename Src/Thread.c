#include "Thread.h"


DWORD CALLBACK MyThreadTestFunction(LPVOID lpParam) {
	Thread* thread = (Thread*)lpParam;
	DWORD lpThreadId = 0;
	lpThreadId = GetCurrentThreadId();
	//线程准备
	thread->fromThreadMessage = 1;
	bool isRuning = true;
	bool hasTask = false;

	while (isRuning) {
		if (thread->toThreadMessage != 0)
		{
			//收到主线程做任务指令
			if (thread->toThreadMessage = 11)
			{
				hasTask = true;
			}
			//收到主线程关闭指令
			if (thread->toThreadMessage = 12)
			{
				isRuning = false;
			}
			thread->toThreadMessage = 0;
		}

		if (hasTask)
		{
			Sleep(1000);
			//任务完成
			printf("[%d]:done\n", thread->id);
			hasTask = false;
			thread->fromThreadMessage = 2;
		}
	}

	Sleep(1000);
	//任务结束
	thread->fromThreadMessage = 3;
	printf("[%d]:thread main close!\n", thread->id);
	return 0;
}

void Task_Main() {
	uint32_t taskNum = 100;
	const uint32_t MAX_THREADS = 3;
	Thread* threadArray = (Thread*)malloc(sizeof(Thread) * MAX_THREADS);
	uint32_t threadCount = 0;
	bool isRuning = true;

	for (int i = 0; i < MAX_THREADS; i++)
	{
		Thread* thread = NULL;
		thread = threadArray + i;
		thread->handle = CreateThread(NULL, 0, MyThreadTestFunction, thread, 0, &thread->id);
		printf("[main]:thread->id:%d\n", thread->id);
		if (thread->handle != NULL)
		{
			thread->isActive = true;
			threadCount++;
		}
	}

	printf("threadCompleteCount:%d\n", threadCount);
	while (isRuning) {
		for (size_t i = 0; i < MAX_THREADS; i++)
		{
			Thread* tmpThread = threadArray + i;
			if (tmpThread->isActive && tmpThread->fromThreadMessage != 0)
			{
				//子线程准备
				if (tmpThread->fromThreadMessage == 1)
				{
					printf("[main]:thread ready, id:%d\n", tmpThread->id);
					//给子线程发任务
					tmpThread->toThreadMessage = 11;
				}
				//子线程完成任务
				if (tmpThread->fromThreadMessage == 2)
				{
					printf("[main]:thread done,id:%d\n", tmpThread->id);
					if (taskNum > 0)
					{
						//下发新任务
						tmpThread->toThreadMessage = 11;
						taskNum--;
						printf("[main]:taskNum:%d\n",taskNum);
					}
					else {
						//关闭任务
						tmpThread->toThreadMessage = 12;
					}
				}
				//子线程关闭
				if (tmpThread->fromThreadMessage == 3)
				{
					tmpThread->isActive = false;
					threadCount--;
					printf("[main]:threadCount:%d\n", threadCount);
				}
				tmpThread->fromThreadMessage = 0;
			}
		}

		if (threadCount == 0)
		{
			isRuning = false;
		}
		Sleep(200);
	}

	for (size_t i = 0; i < MAX_THREADS; i++)
	{
		CloseHandle(threadArray[i].handle);
	}
	return 0;
}