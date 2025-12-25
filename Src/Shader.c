#include "Shader.h"

uint8_t GetNumberOfProcessors() {
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	uint8_t proNum = (uint8_t)sysInfo.dwNumberOfProcessors;
	return proNum;
}