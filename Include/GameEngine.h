#ifndef __GAMEENGINE__
#define __GAMEENGINE__

#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "GameInstance.h"
#include "Define.h"
#include "Mesh.h"
#include "Texture.h"
//#include "Renderer.h"


#define MESSAGE_NONE 0
#define MESSAGE_TYPE1 1
#define MESSAGE_TYPE2 2
#define MESSAGE_TYPE3 3


typedef struct message{
	uint8_t type;
	uint8_t data[7];
}Message;

typedef struct messageAssistant {
	CRITICAL_SECTION* pToLock;
	CRITICAL_SECTION* pFromLock;
	Message toThreadMessage;
	Message fromThreadMessage;
}MessageAssistant;

typedef struct {
	/*显示缓冲区*/
	uint8_t fps;
	bool gameIsRuning;
	Array texture;
	CRITICAL_SECTION* criticalSection_render; //锁
	MessageAssistant* msgAssist; //消息助手

}GameEngine;

/*场景循环*/
void GameEngin_SceneLoop(float delta);

/*渲染循环*/
void GameEnginRenderLoop();

/*引擎关闭*/
void EngineClose();

/*窗口关闭*/
void  WindowsClose();

/*窗口最大化*/
void onWindowsMax();

/*窗口最小化*/
void onWindowsMin();

/*引擎初始化*/
void GameEngineInit(uint8_t fps);

/*获取单例对象*/
GameEngine* _getGameEngine();

/*获取帧率*/
uint8_t GameEngine_GetFPS();

bool GameEngine_IsRuning();

void GameEngine_Render();

void GameEngine_DrawBg();

void GameEngine_Release();

Array GameEngine_GetTextureArr();

Texture GameEngine_LoadTexture(const char* path, uint32_t textureID);

Color4 UVTextureSample(float u,float v, uint32_t textureID);

void GameEngine_MouseKeyEvent(VMEVENT type,void* key);

#endif // !__GAMEENGINE__
