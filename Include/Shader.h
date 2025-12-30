#pragma once
#include "Define.h"

typedef struct {
	DWORD id;
	HANDLE handle;
}ShaderThread;

typedef struct {
	uint8_t stage;
}Shader;

uint8_t GetNumberOfProcessors();

DWORD CALLBACK Shader_ThreadMain(ShaderThread* thread);