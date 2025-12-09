#pragma once
#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <MemManager.h>
#include <containers.h>
#include "Define.h"

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

void Renderer_FrameBufferCreate(uint32_t width, uint32_t height, uint8_t bytepp);

void Renderer_FrameBufferRelease();

void Renderer_Create();

void Renderer_Release(Renderer* render,uint8_t type);

uint8_t* Renderer_GetFrameBuffer();

/*获取buffer宽度*/
uint32_t Renderer_GetFrameWidth();

/*获取buffer高度*/
uint32_t Renderer_GetFrameHeight();

/*获取buffer高度*/
uint32_t Renderer_GetFrameBytepp();

void Renderer_SubmitTexture(uint8_t* inPixels, uint32_t inWidth, uint32_t inHeight, uint8_t bytepp);

void Renderer_SubmitObject(float* inVertices, float* inUvs, uint32_t inNumOfVetices);

void Renderer_Tick(float delta);

void Renderer_Render();

void Renderer_DrawBg();