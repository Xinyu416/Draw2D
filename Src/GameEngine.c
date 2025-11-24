#include "GameEngine.h"


GameEngine* _gameEngne = NULL;

void GameEngineInit(uint32_t width, uint32_t height, uint8_t fps, uint8_t bytepp) {
	GameEngine* gameengine = _getGameEngine();
	gameengine->width = width;
	gameengine->height = height;
	gameengine->fps = fps;
	gameengine->gameIsRuning = true;
	gameengine->bytepp = bytepp;
	gameengine->backgroudColor = MakeColor4(0, 0, 0, 255);
	gameengine->bufferShow = (uint8_t*)malloc(width * height * bytepp);
	GameIns_Init();
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
	if (_gameEngne == NULL) {
		_gameEngne = calloc(1, sizeof(GameEngine));
	}
	return _gameEngne;
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
	GameEngine_DrawBg();
	GameEngine_Render();
	GameIns_Tick(delta);
}

void GameEnginRenderLoop() {
	printf("GameEnginRenderLoop\n");
}
void GameEngine_Render() {
	//Mesh* pmesh = (Mesh*)GetArrayElementByIndex(&_getGameIns()->meshs, 0);
	Mesh* pmesh = _getGameIns()->cMesh;
	//缩放旋转结果矩阵
	Matrix srm = CreateStandardMatrix();
	//缩放旋转位移结果矩阵
	Matrix srtm = CreateStandardMatrix();
	//缩放
	Matrix ms = MakeScaMatrix(pmesh->scale.x, pmesh->scale.y);
	//旋转
	Matrix mr = MakeRotMatrix(Deg2Rad(pmesh->rot));
	//位移
	Matrix mt = MakeTranslataMatrix(pmesh->pos.x, pmesh->pos.y);
	//一起算
	Multi2Matrix(mr.m, ms.m, srm.m);
	Multi2Matrix(mt.m, srm.m, srtm.m);
	pmesh->tm = srtm;
	Color4 randColor =  MakeColor4(rand() % 255, rand() % 255, rand() % 255, 255);

	//后期根据BoudingBox大小调整像素渲染区
	Vect2 vertices[4] = { 0 };
	for (size_t v = 0; v < pmesh->geo.numOfQuad; v++)
	{
		uint32_t vi = v * 8;
		vertices[0] = Vect2MultMatrix(MakeVect2(pmesh->geo.vertices[vi + 0], pmesh->geo.vertices[vi + 1]), pmesh->tm.m);
		vertices[1] = Vect2MultMatrix(MakeVect2(pmesh->geo.vertices[vi + 2], pmesh->geo.vertices[vi + 3]), pmesh->tm.m);
		vertices[2] = Vect2MultMatrix(MakeVect2(pmesh->geo.vertices[vi + 4], pmesh->geo.vertices[vi + 5]), pmesh->tm.m);
		vertices[3] = Vect2MultMatrix(MakeVect2(pmesh->geo.vertices[vi + 6], pmesh->geo.vertices[vi + 7]), pmesh->tm.m);

		//遍历屏幕空间像素
		for (size_t y = 0; y < _getGameEngine()->height; y++)
		{
			for (size_t x = 0; x < _getGameEngine()->width; x++)
			{
				size_t index = y * _getGameEngine()->width * _getGameEngine()->bytepp + x * _getGameEngine()->bytepp;
				//bgr
				Vect2 pixel = MakeVect2((float)x, (float)y);
				if (IsPointInQuadDotSign(pixel, vertices))
				{
					_getGameEngine()->bufferShow[index + 0] = /*pmesh->mat.color.b*/ randColor.b;
					_getGameEngine()->bufferShow[index + 1] = /*pmesh->mat.color.g*/ randColor.g;
					_getGameEngine()->bufferShow[index + 2] = /*pmesh->mat.color.r*/ randColor.r;
				}
				else
				{
					_getGameEngine()->bufferShow[index + 0] = _getGameEngine()->bufferShow[index + 0] + _getGameEngine()->backgroudColor.b;
					_getGameEngine()->bufferShow[index + 1] = _getGameEngine()->bufferShow[index + 1] + _getGameEngine()->backgroudColor.g;
					_getGameEngine()->bufferShow[index + 2] = _getGameEngine()->bufferShow[index + 2] + _getGameEngine()->backgroudColor.r;
				}
			}
		}
	}
	Sleep(0.02);
}

void GameEngine_DrawBg() {

	for (size_t y = 0; y < _getGameEngine()->height; y++)
	{
		for (size_t x = 0; x < _getGameEngine()->width; x++)
		{
			size_t index = y * _getGameEngine()->width * _getGameEngine()->bytepp + x * _getGameEngine()->bytepp;

			_getGameEngine()->bufferShow[index + 0] = _getGameEngine()->backgroudColor.b;
			_getGameEngine()->bufferShow[index + 1] = _getGameEngine()->backgroudColor.g;
			_getGameEngine()->bufferShow[index + 2] = _getGameEngine()->backgroudColor.r;
		}
	}
}

void GameEngine_Release() {
	if (_gameEngne)
	{
		free(_gameEngne);
		_gameEngne = NULL;
	}
	printf("GameEngine_Release\n");
}