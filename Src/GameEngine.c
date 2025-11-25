#define _CRT_SECURE_NO_WARNINGS
#include "GameEngine.h"


GameEngine* _gameEngne = NULL;
//UV UVTexture;
void GameEngineInit(uint32_t width, uint32_t height, uint8_t fps, uint8_t bytepp) {
	GameEngine* gameengine = _getGameEngine();
	gameengine->width = width;
	gameengine->height = height;
	gameengine->fps = fps;
	gameengine->gameIsRuning = true;
	gameengine->bytepp = bytepp;
	gameengine->backgroudColor = MakeColor4(0, 0, 0, 255);
	gameengine->bufferShow = (uint8_t*)malloc(width * height * bytepp);

	const char* path = "C:\\Users\\DRF\\Desktop\\Temp\\uv.bmp";
	GameEngine_LoadTexture(path);
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

	//Color4 randColor = MakeColor4(rand() % 255, rand() % 255, rand() % 255, 255);
	Color4 CA = MakeColor4(255, 0, 0, 255);
	Color4 CB = MakeColor4(0, 255, 0, 255);
	Color4 CC = MakeColor4(0, 0, 255, 255);


	//后期根据BoudingBox大小调整像素渲染区
	Vect2 vp[3] = { 0 };
	Vect2 uv[3] = { 0 };
	for (size_t v = 0; v < pmesh->geo.numOfQuad * 2; v++)
	{
		//c = v * 12;
		//CA = MakeColor4(pmesh->geo.colors[0 + c], pmesh->geo.colors[1 + c], pmesh->geo.colors[2 + c], pmesh->geo.colors[3 + c]);
		//CB = MakeColor4(pmesh->geo.colors[4 + c], pmesh->geo.colors[5 + c], pmesh->geo.colors[6 + c], pmesh->geo.colors[7 + c]);
		//CC = MakeColor4(pmesh->geo.colors[8 + c], pmesh->geo.colors[9 + c], pmesh->geo.colors[10 + c], pmesh->geo.colors[11 + c]);
		uint32_t vi = v * 6;

		uv[0] = MakeVect2(pmesh->geo.uvs[vi + 0], pmesh->geo.uvs[vi + 1]);
		uv[1] = MakeVect2(pmesh->geo.uvs[vi + 2], pmesh->geo.uvs[vi + 3]);
		uv[2] = MakeVect2(pmesh->geo.uvs[vi + 4], pmesh->geo.uvs[vi + 5]);

		//顶点从模型空间转换成世界空间
		vp[0] = Vect2MultMatrix(MakeVect2(pmesh->geo.vertices[vi + 0], pmesh->geo.vertices[vi + 1]), pmesh->tm.m);
		vp[1] = Vect2MultMatrix(MakeVect2(pmesh->geo.vertices[vi + 2], pmesh->geo.vertices[vi + 3]), pmesh->tm.m);
		vp[2] = Vect2MultMatrix(MakeVect2(pmesh->geo.vertices[vi + 4], pmesh->geo.vertices[vi + 5]), pmesh->tm.m);

		Vect2 half = MakeVect2((float)_getGameEngine()->width / 2.f, (float)_getGameEngine()->height / 2.f);

		//将mesh的顶点转换到相机空间 *相机的逆矩阵
		Vect2 p0 = Vect2MultMatrix(vp[0], _getGameIns()->pCam->tm.m);
		Vect2 p1 = Vect2MultMatrix(vp[1], _getGameIns()->pCam->tm.m);
		Vect2 p2 = Vect2MultMatrix(vp[2], _getGameIns()->pCam->tm.m);

		//计算画幅空间位置 （需要考虑偏移值）
		Vect2 A = AddVect2(MakeVect2((p0.x / _getGameIns()->pCam->width) * (float)_getGameEngine()->width, (p0.y / _getGameIns()->pCam->height) * (float)_getGameEngine()->height), half);
		Vect2 B = AddVect2(MakeVect2((p1.x / _getGameIns()->pCam->width) * (float)_getGameEngine()->width, (p1.y / _getGameIns()->pCam->height) * (float)_getGameEngine()->height), half);
		Vect2 C = AddVect2(MakeVect2((p2.x / _getGameIns()->pCam->width) * (float)_getGameEngine()->width, (p2.y / _getGameIns()->pCam->height) * (float)_getGameEngine()->height), half);

		//Color4 uvA = UVTextureSample(uv[0].x, uv[0].y);
		//Color4 uvB = UVTextureSample(uv[1].x, uv[1].y);
		//Color4 uvC = UVTextureSample(uv[2].x, uv[2].y);

		/*PrintColor(uvA);
		PrintColor(uvB);
		PrintColor(uvC);*/

		//遍历屏幕空间像素
		for (size_t y = 0; y < _getGameEngine()->height; y++)
		{
			for (size_t x = 0; x < _getGameEngine()->width; x++)
			{
				size_t index = y * _getGameEngine()->width * _getGameEngine()->bytepp + x * _getGameEngine()->bytepp;
				//bgr buffer像素坐标 偏移到每个像素中心去除锯齿
				Vect2 pix = MakeVect2((float)x + 0.5f, (float)y + 0.5f);

				//重心坐标值
				float alpha = (-(pix.x - B.x) * (C.y - B.y) + (pix.y - B.y) * (C.x - B.x)) / (-(A.x - B.x) * (C.y - B.y) + (A.y - B.y) * (C.x - B.x));
				float beta = (-(pix.x - C.x) * (A.y - C.y) + (pix.y - C.y) * (A.x - C.x)) / (-(B.x - C.x) * (A.y - C.y) + (B.y - C.y) * (A.x - C.x));
				float gama = 1.f - alpha - beta;

				//判断点在三角形内还是外
				if (alpha >= 0 && beta >= 0 && gama >= 0)
				{
					//通过顶点的uv值算出每个点的uv值
					float u = alpha * uv[0].x + beta * uv[1].x + gama * uv[2].x;
					float v = alpha * uv[0].y + beta * uv[1].y + gama * uv[2].y;
					Color4 pure = MakeColor4(255, 255, 255, 255);

					/*_getGameEngine()->bufferShow[index + 0] = alpha * CA.b + beta * CB.b + gama * CC.b;
					_getGameEngine()->bufferShow[index + 1] = alpha * CA.g + beta * CB.g + gama * CC.g;
					_getGameEngine()->bufferShow[index + 2] = alpha * CA.r + beta * CB.r + gama * CC.r;*/

					//贴图颜色采样
					Color4 colPick = UVTextureSample(u, v);

					//设置颜色值
					_getGameEngine()->bufferShow[index + 0] = colPick.b;
					_getGameEngine()->bufferShow[index + 1] = colPick.g;
					_getGameEngine()->bufferShow[index + 2] = colPick.r;

					//uv验证
					_getGameEngine()->bufferShow[index + 0] = pure.b * v;
					_getGameEngine()->bufferShow[index + 1] = pure.g * u;
					_getGameEngine()->bufferShow[index + 2] = 0;
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

void GameEngine_LoadTexture(const char* path) {
	FILE* rbmp = fopen(path, "rb");
	if (rbmp == NULL)
	{
		printf("file is NULL");
		return;
	}
	struct tagBITMAPFILEHEADER head;
	struct tagBITMAPINFOHEADER info;
	fread(&head, 1, sizeof(struct tagBITMAPFILEHEADER), rbmp);
	fread(&info, 1, sizeof(struct tagBITMAPINFOHEADER), rbmp);
	uint8_t count = info.biBitCount / 8;
	uint8_t* bgrcolors = (uint8_t*)malloc(info.biWidth * info.biHeight * count);
	fread(bgrcolors, 1, info.biWidth * info.biHeight * count, rbmp);

	Texture texture = { .data = bgrcolors,.width = info.biWidth,.height = info.biHeight,.bpp = info.biBitCount };

	_getGameEngine()->texture = &texture;
}

Color4 UVTextureSample(float u, float v) {
	/*u = fmodf(u, 1.0f);
	v = fmodf(v, 1.0f);*/
	//Color4 out = MakeColor4(255, 255, 255, 255);



	uint32_t width = (float)_getGameEngine()->texture->width * u;
	uint32_t height = (float)_getGameEngine()->texture->height * v;

	uint32_t index = _getGameEngine()->texture->width * height * _getGameEngine()->texture->bpp / 8
		+ width * _getGameEngine()->texture->bpp / 8;
	uint8_t r = _getGameEngine()->texture->data[index + 2];
	uint8_t g = _getGameEngine()->texture->data[index + 1];
	uint8_t b = _getGameEngine()->texture->data[index + 0];
	uint8_t a = 255;
	Color4 out = MakeColor4(r, g, b, a);

	return out;

}