#include "GameEngine.h"

GameEngine* _gameEngneInstance = NULL;

void GameEngineInit(uint32_t width, uint32_t height, uint8_t fps, GameEngineResizeEvent cb) {
	GameEngine* gameinstance = _getGameEngine();
	gameinstance->width = width;
	gameinstance->height = height;
	gameinstance->fps = fps;
	gameinstance->onResize = cb;
	gameinstance->gameIsRuning = true;
}

void GameEngineResize(uint32_t w, uint32_t h) {
	printf("GameEngineResize w:%d,h:%d\n", w, h);
	_gameEngneInstance->width = w;
	_gameEngneInstance->height = h;
}

void EngineClose() {
	_gameEngneInstance->gameIsRuning = false;
	printf("GameEngine--EngineClose\n");
}

void  WindowsClose() {
	_gameEngneInstance->gameIsRuning = false;
	printf("GameEngine--WindowsClose\n");
}

void onWindowsMax() {
	printf("GameEngine--onWindowsMax\n");
}

void onWindowsMin() {
	printf("GameEngine--onWindowsMin\n");
}

GameEngine* _getGameEngine() {
	if (_gameEngneInstance) {
		return _gameEngneInstance;
	}
	_gameEngneInstance = calloc(1,sizeof(GameEngine));
	return _gameEngneInstance;
}

uint8_t GameEngine_GetFPS() {
	return _gameEngneInstance->fps;
}

bool GameEngine_IsRuning() {
	return _gameEngneInstance->gameIsRuning;
}

void GameEnginSceneLoop() {
	//printf("SceneLoop\n");
}

void GameEnginRenderLoop() {
	printf("GameEnginRenderLoop\n");
}