#define _CRT_SECURE_NO_WARNINGS
#include "Thread.h"
#include "FileOperate.h"
#include "Hash.h"

#define BLOCKSIZE 1048576

void fileOperate() {
	const char* dir = "D:\\DS";
	uint32_t filesCount = getDirFileCount(dir);
	printf("fileOperate::filesNum:%d\n", filesCount);
	char* filesName = (char*)malloc(MAX_PATH * filesCount);
	getDirFileNames(dir, filesName);

	//打印文件名称
	/*for (size_t i = 0; i < filesCount; i++)
	{
		printf("%s\n", (filesName + i * MAX_PATH));
	}*/

	//读取文件给文件内容hash
	//const char* path = "D:\\DS\\Ellen Whitaker's Horse Life (Europe) (En,Fr,De,Es,It,Nl)(s.)(v.1.0).zip";
	const char* path = "D:\\DS\\Fashion Dogz (Europe)(s.)(v.1.0).zip";
	FILE* f = fopen(path, "rb");
	if (f == NULL)
	{
		printf("FILE IS NULL\n");
		return;
	}
	if (fseek(f, 0, SEEK_END) != 0) {
		return;
	}
	long originSize = ftell(f);
	printf("originSize:%d\n", originSize);

	rewind(f);
	uint8_t* data = (uint8_t*)malloc(originSize);
	fread(data, 1, originSize, f);
	fclose(f);

	uint8_t hash[20];
	sha1(data, originSize, hash);
	for (uint32_t i = 0; i < 20; i++) {
		printf("%02x", hash[i]);
	}
	printf("\n");
}

void doTaskStream(Thread* thread, uint8_t* data) {
	const char* path = (thread->fileNameArr + (thread->toThreadMessage.taskIndex * MAX_PATH));
	FILE* f = fopen(path, "rb");
	if (f == NULL)
	{
		printf("FILE IS NULL\n");
		return;
	}

	SHA1_CTX ctx;
	sha1_init(&ctx);

	uint32_t readLen = 0;
	while (feof(f) == 0)
	{
		readLen = fread(data, 1, BLOCKSIZE, f);
		sha1_update(&ctx, data, readLen);
	}
	fclose(f);

	//printf("[%d]:path:%s -- ", thread->id, path);
	uint8_t hash[20];
	sha1_final(&ctx, hash);

	/*for (uint32_t i = 0; i < 20; i++) {
		printf("%02x", hash[i]);
	}
	printf("\n");*/
}

/// <summary>
/// 任务内容
/// </summary>
/// <param name="thread"></param>
void doTask(Thread* thread) {
	const char* path = (thread->fileNameArr + (thread->toThreadMessage.taskIndex * MAX_PATH));
	FILE* f = fopen(path, "rb");
	if (f == NULL)
	{
		printf("FILE IS NULL\n");
		return;
	}
	if (fseek(f, 0, SEEK_END) != 0) {
		return;
	}
	long originSize = ftell(f);
	rewind(f);
	uint8_t* data = (uint8_t*)malloc(originSize);
	fread(data, 1, originSize, f);
	fclose(f);

	printf("\n");
	printf("[%d]:path:%s -- ", thread->id, path);
	uint8_t hash[20];
	sha1(data, originSize, hash);
	for (uint32_t i = 0; i < 20; i++) {
		printf("%02x", hash[i]);
	}
	printf("\n");
	free(data);
}

DWORD CALLBACK threadMain(LPVOID lpParam) {
	Thread* thread = (Thread*)lpParam;
	DWORD lpThreadId = 0;
	lpThreadId = GetCurrentThreadId();
	//线程准备
	thread->fromThreadMessage = 1;
	bool isRunning = true;
	bool hasTask = false;
	uint8_t* data = (uint8_t*)malloc(BLOCKSIZE);
	while (isRunning) {
		if (thread->toThreadMessage.type != 0)
		{
			//收到主线程做任务指令
			if (thread->toThreadMessage.type == 11)
			{
				hasTask = true;

			}
			//收到主线程关闭指令
			if (thread->toThreadMessage.type == 12)
			{
				isRunning = false;
			}
			thread->toThreadMessage.type = 0;
		}

		if (hasTask)
		{
			//做任务 读取文件内容加hash 整文件加载
			//doTask(thread);
			//做任务 读取文件内容加hash 流式加载
			doTaskStream(thread, data);

			hasTask = false;
			thread->fromThreadMessage = 2;
		}
		Sleep(20);
	}

	//任务结束
	thread->fromThreadMessage = 3;
	free(data);
	printf("[%d]:thread main close!\n", thread->id);
}

void Task_Main() {

	//获取文件夹文件名得到文件名数组
	const char* dir = "D:\\DS";
	uint32_t filesCount = getDirFileCount(dir);
	printf("getDirFileCount:%d\n", filesCount);
	char* filesNameArr = (char*)malloc(MAX_PATH * filesCount);
	getDirFileNames(dir, filesNameArr);
	uint32_t taskIndex = 0;
	/*for (size_t i = 0; i < filesCount; i++)
	{
		//打印文件名
		printf("%s\n", (filesNameArr + i * MAX_PATH));
	}*/

	//任务数量
	uint32_t taskNum = filesCount;
	//最大线程数
	const uint32_t MAX_THREADS = 24;
	Thread* threadArray = (Thread*)malloc(sizeof(Thread) * MAX_THREADS);
	//活跃线程数量
	uint32_t activeThreadCount = 0;
	bool isRunning = true;

	Thread* thread = NULL;
	for (int i = 0; i < MAX_THREADS; i++)
	{
		thread = threadArray + i;
		thread->isActive = false;
		thread->handle = CreateThread(NULL, 0, threadMain, thread, 0, &thread->id);
		printf("[main]:thread->id:%d\n", thread->id);
		if (thread->handle != NULL)
		{
			thread->fileNameArr = filesNameArr;
			thread->toThreadMessage.taskIndex = 0;
			thread->isActive = true;
			activeThreadCount++;
		}
	}

	printf("threadCompleteCount:%d\n", activeThreadCount);

	while (isRunning) {
		for (size_t i = 0; i < MAX_THREADS; i++)
		{
			thread = threadArray + i;
			if (thread->isActive != false && thread->fromThreadMessage != 0)
			{
				//子线程准备
				if (thread->fromThreadMessage == 1)
				{
					printf("[main]:thread ready, id:%d\n", thread->id);
					//给子线程发任务
					thread->toThreadMessage.taskIndex = taskIndex;
					taskIndex++;
					taskNum--;
					printf("[main]:taskIndex:%d\n", taskIndex);
					thread->toThreadMessage.type = 11;
				}
				//子线程完成任务
				if (thread->fromThreadMessage == 2)
				{
					printf("[main]:thread done,id:%d\n", thread->id);
					if (taskNum > 0)
					{
						//下发新任务
						thread->toThreadMessage.taskIndex = taskIndex;
						taskNum--;
						taskIndex++;
						printf("[Main]:done/remain/filesCount:%d/%d/%d\n", taskIndex, taskNum, filesCount);
						thread->toThreadMessage.type = 11;
					}
					else {
						//关闭任务
						thread->toThreadMessage.type = 12;
					}
				}
				//子线程关闭
				if (thread->fromThreadMessage == 3)
				{
					thread->isActive = false;
					activeThreadCount--;
					printf("[main]:thread[%d] closed\n", thread->id);
				}

				thread->fromThreadMessage = 0;
			}
		}

		if (activeThreadCount == 0)
		{
			isRunning = false;
		}
		Sleep(200);
	}


	for (size_t i = 0; i < MAX_THREADS; i++)
	{
		thread = threadArray + i;
		if (thread != NULL)
		{
			CloseHandle(thread->handle);
		}
	}

	free(threadArray);
	free(filesNameArr);
	return 0;
}