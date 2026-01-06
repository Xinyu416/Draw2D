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

/*顶点变换*/
void VertexTranfrom(uint32_t inTaskIndex);

/*片元裁切*/
void FragmentClip(uint32_t inTaskIndex);

/*片元着色*/
void FragmentShading(uint32_t inTaskIndex);

DWORD CALLBACK Shader_ThreadMain(ShaderThread* thread);