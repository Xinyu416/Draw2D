#ifndef __GAMEENGINE__
#define __GAMEENGINE__

#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>


/*引擎关闭事件*/
typedef void (*GameEngineCloseEvent)();

/*窗口关闭事件*/
typedef void (*GameEngineWindowsCloseEvent)();

/*窗口大小变化事件*/
typedef void (*GameEngineResizeEvent)(uint32_t w, uint32_t h);

/*窗口最大化事件*/
typedef void (*GameEnginMaximizeEvent)();

/*窗口最小化事件*/
typedef void (*GameEnginMinimizeEvent)();

/*场景循环事件*/
typedef void(*GameEnginSceneLoopEvent)();

/*渲染循环事件*/
typedef void(*GameEnginRenderLoopEvent)();


typedef struct {
	GameEngineResizeEvent onResize;
	GameEngineCloseEvent engineClose;
	GameEngineWindowsCloseEvent windowsClose;

	/*显示缓冲区*/
	uint8_t* bufferShow;
	uint8_t fps;
	uint32_t width;
	uint32_t height;
	uint8_t bytepp;
	bool gameIsRuning;
}GameEngine;


/*场景循环*/
void GameEngin_SceneLoop(float delta);

/*渲染循环*/
void GameEnginRenderLoop();

/*窗口大小变化*/
void GameEngineResize(uint32_t w, uint32_t h);

/*引擎关闭*/
void EngineClose();

/*窗口关闭*/
void  WindowsClose();

/*窗口最大化*/
void onWindowsMax();

/*窗口最小化*/
void onWindowsMin();

/*引擎初始化*/
void GameEngineInit(uint32_t width, uint32_t height,uint8_t fps, uint8_t bytepp);

/*获取单例对象*/
GameEngine* _getGameEngine();

/*获取帧率*/
uint8_t GameEngine_GetFPS();

/*获取缓冲区数据*/
uint8_t* GameEngine_GetFrameData();

/*获取buffer宽度*/
uint32_t GameEngine_GetFrameWidth();

/*获取buffer高度*/
uint32_t GameEngine_GetFrameHeight();

/*获取buffer高度*/
uint32_t GameEngine_GetFrameBytepp();

bool GameEngine_IsRuning();

void GameEngine_Render();

void GameEngine_Release();


#endif // !__GAMEENGINE__
