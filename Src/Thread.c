#include "Thread.h"


DWORD CALLBACK MyThreadTestFunction(LPVOID lpParam) {
	Thread* thread = (Thread*)lpParam;
	DWORD lpThreadId = 0;
	lpThreadId = GetCurrentThreadId();
	//线程准备
	thread->fromThreadMessage = 1;
	bool isRunning = true;
	bool hasTask = false;

	while (isRunning) {
		if (thread->toThreadMessage != 0)
		{
			//收到主线程做任务指令
			if (thread->toThreadMessage == 11)
			{
				hasTask = true;
			}
			//收到主线程关闭指令
			if (thread->toThreadMessage == 12)
			{
				isRunning = false;
			}
			thread->toThreadMessage = 0;
		}

		if (hasTask)
		{
			Sleep(1000);
			//任务完成
			//printf("[%d]:done\n", thread->id);
			hasTask = false;
			thread->fromThreadMessage = 2;
		}
		//Sleep(100);
	}

	//任务结束
	thread->fromThreadMessage = 3;
	printf("[%d]:thread main close!\n", thread->id);
}

void Task_Main() {
	uint32_t taskNum = 100;
	const uint32_t MAX_THREADS = 10;
	Thread* threadArray = (Thread*)malloc(sizeof(Thread) * MAX_THREADS);
	uint32_t threadCount = 0;
	bool isRunning = true;

	Thread* thread = NULL;
	for (int i = 0; i < MAX_THREADS; i++)
	{
		thread = threadArray + i;
		thread->isActive = false;
		thread->handle = CreateThread(NULL, 0, MyThreadTestFunction, thread, 0, &thread->id);
		printf("[main]:thread->id:%d\n", thread->id);
		if (thread->handle != NULL)
		{
			thread->isActive = true;
			threadCount++;
		}
	}

	printf("threadCompleteCount:%d\n", threadCount);
	uint32_t taskCount = 0;
	Thread* tmpThread = NULL;
	while (isRunning) {
		for (size_t i = 0; i < MAX_THREADS; i++)
		{
			tmpThread = threadArray + i;
			if (tmpThread->handle != NULL && tmpThread->fromThreadMessage != 0)
			{
				//子线程准备
				if (tmpThread->fromThreadMessage == 1)
				{
					printf("[main]:thread ready, id:%d\n", tmpThread->id);
					//给子线程发任务
					tmpThread->toThreadMessage = 11;
					if (taskNum > 0)taskNum--;
					taskCount++;
				}
				//子线程完成任务
				if (tmpThread->fromThreadMessage == 2)
				{
					printf("[main]:thread done,id:%d\n", tmpThread->id);
					if (taskNum > 0)
					{
						taskCount++;
						//下发新任务
						tmpThread->toThreadMessage = 11;
						taskNum--;
						printf("[main]:taskNum:%d\n", taskNum);
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
					printf("[main]:thread[%d] closed\n", tmpThread->id);
				}

				tmpThread->fromThreadMessage = 0;
			}
		}

		if (threadCount == 0)
		{
			isRunning = false;
		}
		Sleep(200);
	}
	printf("taskCount:%d\n",taskCount);

	for (size_t i = 0; i < MAX_THREADS; i++)
	{
		thread = threadArray + i;
		if (thread != NULL)
		{
			CloseHandle(thread->handle);
		}
	}

	free(threadArray);
	return 0;
}