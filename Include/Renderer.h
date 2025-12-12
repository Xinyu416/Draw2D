#pragma once
#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <MemManager.h>
//#include <containers.h>
#include "Define.h"
#include "GameInstance.h"


typedef struct {
	uint8_t type;
	uint32_t taskIndex;
}Message;

typedef struct {
	DWORD id;
	HANDLE handle;
	bool isActive;
	uint8_t fromThreadMessage;
	Message toThreadMessage;
}RendererThread;

typedef struct {
	uint8_t* buffer;
	uint32_t width;
	uint32_t height;
	uint8_t bytepp;
	Color4 backgroudColor;
}FrameBuffer;

typedef struct {
	MemManager memM;
	FrameBuffer frameBuffer;
	Array textures;
	Array objcects;
}Renderer;

Renderer* _getRenderer();

/*创建缓冲区*/
void Renderer_FrameBufferCreate(uint32_t width, uint32_t height, uint8_t bytepp);

/*释放缓冲区*/
void Renderer_FrameBufferRelease();

/*创建渲染器*/
void Renderer_Create();

/*释放渲染器*/
void Renderer_Release(Renderer* render,uint8_t type);

/*获取buffer*/
uint8_t* Renderer_GetFrameBuffer();

/*获取buffer宽度*/
uint32_t Renderer_GetFrameWidth();

/*获取buffer高度*/
uint32_t Renderer_GetFrameHeight();

/*获取buffer高度*/
uint32_t Renderer_GetFrameBytepp();

/*提交贴图数据*/
void Renderer_SubmitTexture(uint8_t* inPixels, uint32_t inWidth, uint32_t inHeight, uint8_t bpp);

/*提交点数据*/
uint32_t Renderer_SubmitObject(float* inVertices, float* inUvs, uint32_t inNumOfVetices, uint32_t objID);

/*每帧执行*/
void Renderer_Tick(float delta);

void Renderer_Render();

/*每帧渲染背景*/
void Renderer_DrawBg();

/*贴图采样器*/
Color4 Renderer_UVTextureSample(float u, float v, uint32_t tID);

/*渲染主线程*/
void Renderer_TaskMain();

void Renderer_ThreadMain(RendererThread* thread);