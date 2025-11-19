#include "GameEngine.h"

GameEngine* _gameEngneInstance = NULL;

void GameEngineInit(uint32_t width, uint32_t height, uint8_t fps, uint8_t bytepp) {
	GameEngine* gameinstance = _getGameEngine();
	gameinstance->width = width;
	gameinstance->height = height;
	gameinstance->fps = fps;
	gameinstance->gameIsRuning = true;
	gameinstance->bytepp = bytepp;
	gameinstance->bufferShow = (uint8_t*)malloc(width * height * bytepp);
	gameinstance->bufferOperate = (uint8_t*)malloc(width * height * bytepp);
}

void GameEngineResize(uint32_t w, uint32_t h) {
	printf("GameEngineResize w:%d,h:%d\n", w, h);
	_getGameEngine()->width = w;
	_getGameEngine()->height = h;
}

void EngineClose() {
	_getGameEngine()->gameIsRuning = false;
	printf("GameEngine--EngineClose\n");
}

void  WindowsClose() {
	_getGameEngine()->gameIsRuning = false;
	printf("GameEngine--WindowsClose\n");
}

void onWindowsMax() {
	printf("GameEngine--onWindowsMax\n");
}

void onWindowsMin() {
	printf("GameEngine--onWindowsMin\n");
}

GameEngine* _getGameEngine() {
	if (_gameEngneInstance == NULL) {
		_gameEngneInstance = calloc(1, sizeof(GameEngine));
	}
	return _gameEngneInstance;
}

uint8_t* GameEngine_GetFrameData() {
	return _getGameEngine()->bufferShow;
}

uint32_t GameEngine_GetFrameWidth() {
	return _getGameEngine()->width;
}

uint32_t GameEngine_GetFrameHeight() {
	return _getGameEngine()->height;
}

uint32_t GameEngine_GetFrameBytepp() {
	return _getGameEngine()->bytepp;
}

uint8_t GameEngine_GetFPS() {
	return _getGameEngine()->fps;
}

bool GameEngine_IsRuning() {
	return _getGameEngine()->gameIsRuning;
}

void GameEngin_SceneLoop(float delta) {
	//printf("SceneLoop\n");
	//printf("delta:%f\n",delta);
	GameEngine_Render();
}

void GameEnginRenderLoop() {
	printf("GameEnginRenderLoop\n");
}

void GameEngine_Render() {
	for (size_t y = 0; y < _getGameEngine()->height; y++)
	{
		for (size_t x = 0; x < _getGameEngine()->width; x++)
		{
			size_t index = y * _getGameEngine()->width * _getGameEngine()->bytepp + x * _getGameEngine()->bytepp;
			//bgr
			if (y % 10 == 0 || (x * _getGameEngine()->bytepp) % 10 == 0)
			{
				_getGameEngine()->bufferShow[index + 0] = rand() % 255;
				_getGameEngine()->bufferShow[index + 1] = rand() % 255;
				_getGameEngine()->bufferShow[index + 2] = rand() % 255;
			}
			else
			{
				_getGameEngine()->bufferShow[index + 0] = 255;
				_getGameEngine()->bufferShow[index + 1] = 255;
				_getGameEngine()->bufferShow[index + 2] = 255;
			}
		}
	}
	Sleep(0.02);
}


void GameEngine_ChangeBuffer() {
	/*显示缓冲区*/
	uint8_t* bufferOperate = _getGameEngine()->bufferShow;
	/*操作缓冲区*/
	uint8_t* bufferShow = _getGameEngine()->bufferOperate;
}

void GameEngine_Release() {
	if (_gameEngneInstance)
	{
		free(_gameEngneInstance);
		_gameEngneInstance = NULL;
	}
	printf("GameEngine_Release\n");
}