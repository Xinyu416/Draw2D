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

	//先测试
	GameEngine_Render();
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

	GameIns_Tick(delta);
	GameEngine_DrawBg();
	GameEngine_Render();
}

void GameEnginRenderLoop() {
	printf("GameEnginRenderLoop\n");
}
void GameEngine_Render() {
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
	Color4 randColor = MakeColor4(rand() % 255, rand() % 255, rand() % 255, 255);

	//后期根据BoudingBox大小调整像素渲染区
	Vect2 vertices[4] = { 0 };
	for (size_t v = 0; v < pmesh->geo.numOfQuad; v++)
	{
		uint32_t vi = v * 8;
		//顶点从模型空间转换成世界空间
		vertices[0] = Vect2MultMatrix(MakeVect2(pmesh->geo.vertices[vi + 0], pmesh->geo.vertices[vi + 1]), pmesh->tm.m);
		vertices[1] = Vect2MultMatrix(MakeVect2(pmesh->geo.vertices[vi + 2], pmesh->geo.vertices[vi + 3]), pmesh->tm.m);
		vertices[2] = Vect2MultMatrix(MakeVect2(pmesh->geo.vertices[vi + 4], pmesh->geo.vertices[vi + 5]), pmesh->tm.m);
		vertices[3] = Vect2MultMatrix(MakeVect2(pmesh->geo.vertices[vi + 6], pmesh->geo.vertices[vi + 7]), pmesh->tm.m);
		printf("顶点世界空间\n");
		printf("vertices0:");
		PrintVect2(vertices[0]);
		printf("vertices1:");
		PrintVect2(vertices[1]);
		printf("vertices2:");
		PrintVect2(vertices[2]);
		printf("vertices3:");
		PrintVect2(vertices[3]);

		Vect2 half = MakeVect2((float)_getGameEngine()->width / 2.f, (float)_getGameEngine()->height / 2.f);
		//将mesh的顶点转换到相机空间 *相机的逆矩阵
		Vect2 p0 = Vect2MultMatrix(vertices[0], _getGameIns()->pCam->tm.m);
		Vect2 p1 = Vect2MultMatrix(vertices[1], _getGameIns()->pCam->tm.m);
		Vect2 p2 = Vect2MultMatrix(vertices[2], _getGameIns()->pCam->tm.m);
		Vect2 p3 = Vect2MultMatrix(vertices[3], _getGameIns()->pCam->tm.m);

		//计算画幅空间位置 （需要考虑偏移值）
		Vect2 np0 = AddVect2( MakeVect2((p0.x / _getGameIns()->pCam->width) * (float)_getGameEngine()->width, (p0.y / _getGameIns()->pCam->height) * (float)_getGameEngine()->height), half);
		Vect2 np1 = AddVect2(MakeVect2((p1.x / _getGameIns()->pCam->width) * (float)_getGameEngine()->width, (p1.y / _getGameIns()->pCam->height) * (float)_getGameEngine()->height), half);
		Vect2 np2 = AddVect2(MakeVect2((p2.x / _getGameIns()->pCam->width) * (float)_getGameEngine()->width, (p2.y / _getGameIns()->pCam->height) * (float)_getGameEngine()->height), half);
		Vect2 np3 = AddVect2(MakeVect2((p3.x / _getGameIns()->pCam->width) * (float)_getGameEngine()->width, (p3.y / _getGameIns()->pCam->height) * (float)_getGameEngine()->height), half );

		vertices[0] = np0;
		vertices[1] = np1;
		vertices[2] = np2;
		vertices[3] = np3;
		printf("顶点相机空间\n");
		printf("p0:");
		PrintVect2(p0);
		printf("p1:");
		PrintVect2(p1);
		printf("p2:");
		PrintVect2(p2);
		printf("p3:");
		PrintVect2(p3);
		
		printf("顶点画幅空间\n");
		printf("np0:");
		PrintVect2(np0);
		printf("np1:");
		PrintVect2(np1);
		printf("np2:");
		PrintVect2(np2);
		printf("np3:");
		PrintVect2(np3);

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