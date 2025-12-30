#include "Shader.h"
#include "GameEngine.h"
#include "GameInstance.h"

uint8_t GetNumberOfProcessors() {
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	uint8_t proNum = (uint8_t)sysInfo.dwNumberOfProcessors;
	return proNum;
}

DWORD CALLBACK Shader_ThreadMain(ShaderThread* thread) {
	DWORD threadId = thread->id;
	printf("[Shader_ThreadMain]::id = %d\n",thread->id);
	//TODO
	//分配任务 顶点变换


	//片元裁切


	//片元着色
}
