#pragma once

#ifndef __REDERER__
#define __REDERER__

#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <MemManager.h>
#include "Define.h"
#include "GameInstance.h"
#include "Queue.h"
#include "GameEngine.h";

#define RENDERSTAGE_NONE 0
#define RENDERSTAGE_VERTEXTRAS 1
#define RENDERSTAGE_FRAGMENTCLIP 2
#define RENDERSTAGE_FRAGMENTSHADING 3
#define RENDERSTAGE_SHADECOMPLETE 4

typedef struct {
	DWORD id;
	HANDLE handle;
}RendererThread;

typedef struct {
	DWORD id;
	HANDLE handle;
	bool isActive;
	uint8_t fromThreadMessage;
	Message toThreadMessage;
	uint32_t* resultCount;
}RendererChildThread;

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
	Array objcects;

	//渲染线程参数
	bool isRunning;
	//渲染任务数 （三角面index数）  线程加锁访问
	uint32_t taskTriangleIndex; 
	//渲染任务数 （三角面中片元index数） 线程加锁访问
	uint32_t taskFragmentIndex;
	//所有三角面内片元总数
	uint32_t numOfVertex;
	//任务编号
	uint32_t taskIndex;
	//任务总数
	uint32_t taskTotalCount;

	Mesh* currentMesh;

	//渲染阶段（切换到不同阶段做不同任务）
	uint8_t renderStage;

	CRITICAL_SECTION* vertexIndexLock;
	CRITICAL_SECTION* taskTriangleIndexLock;
	CRITICAL_SECTION* taskFragmentIndexLock;
	CRITICAL_SECTION* taskIndexLock;

}Renderer;

Renderer* _getRenderer();

void Renderer_SetCurrentMesh(Mesh* m);

Mesh* Renderer_GetCurrentMesh();

void Renderer_Stop();

/*创建渲染器*/
void Renderer_Initialize(uint32_t width, uint32_t height, uint8_t bytepp);

/*释放渲染器内存*/
void Renderer_ReleaseMEM( uint8_t type);

void Renderer_Release();

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
uint32_t Renderer_SubmitObject(float* inVertices, float* inUvs, uint32_t inNumOfVetices, uint32_t inObjID);

/*提交相机数据*/
void Renderer_SubmitCamera(Camera cam);

/*提交Mesh数据*/
void Renderer_SubmitMesh(Vect2 pos, Matrix tm, float rot, Vect2 scale, Material mat,uint32_t meshID);

void Renderer_Render();

/*每帧渲染背景*/
void Renderer_DrawBg();

void Renderer_Clear();

/*贴图采样器*/
Color4 Renderer_UVTextureSample(float u, float v, uint32_t tID);

void Renderer_ThreadMain(RendererThread* thread);

/*Shader领取任务函数*/
uint32_t Renderer_GetTaskIndex();







//消息机制
MessageAssistant* CreateMessageAssistant();

void ReleaseMessageAssistant(MessageAssistant* assistant);

Message MsgAssistant_SendMsgToThread(MessageAssistant* assistant,Message msg, const bool isblock);

Message MsgAssistant_SendMsgToMain(MessageAssistant* assistant, Message msg, const bool isblock);

Message MsgAssistant_GetMsgFromThread(MessageAssistant* assistant, const bool isblock);

Message MsgAssistant_GetMsgToThread(MessageAssistant* assistant, const bool isblock);

#endif