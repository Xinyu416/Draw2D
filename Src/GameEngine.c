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

	//创建贴图数据
	
	const char* path1 = "C:\\Users\\DRF\\Desktop\\Temp\\bg.bmp";
	Texture tex1 = GameEngine_LoadTexture(path1, 1);
	const char* path2 = "C:\\Users\\DRF\\Desktop\\Temp\\bean_alpha.bmp";
	Texture tex2 = GameEngine_LoadTexture(path2, 1);
	const char* path3 = "C:\\Users\\DRF\\Desktop\\Temp\\item.bmp";
	Texture tex3 = GameEngine_LoadTexture(path3, 2);
	const char* path4 = "C:\\Users\\DRF\\Desktop\\Temp\\role_alpha.bmp";
	Texture tex4 = GameEngine_LoadTexture(path4, 2);


	Array arr = ArrayCreate(sizeof(Texture));
	ArrayPush(&arr, &tex1);
	ArrayPush(&arr, &tex2);
	ArrayPush(&arr, &tex3);
	ArrayPush(&arr, &tex4);
	gameengine->texture = arr;

	GameIns_Init();

	Scene_Init();
}

void GameEngin_SceneLoop(float delta) {

	GameIns_Tick(delta);
	GameEngine_DrawBg();
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

Array GameEngine_GetTextureArr() {
	return _getGameEngine()->texture;
}


bool GameEngine_IsRuning() {
	return _getGameEngine()->gameIsRuning;
}

void GameEnginRenderLoop() {
	printf("GameEnginRenderLoop\n");
}
void GameEngine_Render() {

	for (size_t i = 0; i < _getGameIns()->meshs.length; i++)
	{
		Mesh* pmesh = (Mesh*)GetArrayElementByIndex(&_getGameIns()->meshs, i);

		//Mesh* pmesh = _getGameIns()->cMesh;
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

		//后期根据BoudingBox大小调整像素渲染区
		Vect2 vp[3] = { 0 };
		Vect2 uv[3] = { 0 };
		for (size_t v = 0; v < pmesh->geo.numOfQuad * 2; v++)
		{
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
						float uv_u = alpha * uv[0].x + beta * uv[1].x + gama * uv[2].x;
						float uv_v = alpha * uv[0].y + beta * uv[1].y + gama * uv[2].y;

						//贴图颜色采样
						Color4 colPick = UVTextureSample(uv_u, uv_v, pmesh->mat.textureId);

						//设置颜色值
						_getGameEngine()->bufferShow[index + 0] = colPick.b;
						_getGameEngine()->bufferShow[index + 1] = colPick.g;
						_getGameEngine()->bufferShow[index + 2] = colPick.r;

						/*if ((v / 2) % 2 == 0)
						{
							_getGameEngine()->bufferShow[index + 0] = 255;
							_getGameEngine()->bufferShow[index + 1] = 255;
							_getGameEngine()->bufferShow[index + 2] = 255;
						}
						else
						{
							_getGameEngine()->bufferShow[index + 0] = 0;
							_getGameEngine()->bufferShow[index + 1] = 255;
							_getGameEngine()->bufferShow[index + 2] = 255;
						}*/

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
			_getGameEngine()->bufferShow[index + 3] = _getGameEngine()->backgroudColor.a;
		}
	}
}

void GameEngine_Release() {
	if (_gameEngne)
	{
		uint32_t length = GetArrayElementCount(&_getGameEngine()->texture);
		for (size_t i = 0; i < length; i++)
		{
			Texture* t = (Texture*)GetArrayElementByIndex(&_getGameEngine()->texture, i);
			TextureRelease(t);
		}
		free(_gameEngne);
		_gameEngne = NULL;
	}
	printf("GameEngine_Release\n");
}

Texture GameEngine_LoadTexture(const char* path, uint32_t textureID) {
	FILE* rbmp = fopen(path, "rb");
	if (rbmp == NULL)
	{
		printf("path:%s\n",path);
		printf("File is NULL\n");
		Texture texture = { .bpp = 0,.data = NULL,.height = 0,.width = 0 };
		return texture;
	}
	struct tagBITMAPFILEHEADER head;
	struct tagBITMAPINFOHEADER info;
	fread(&head, 1, sizeof(struct tagBITMAPFILEHEADER), rbmp);
	//printf("bitmap: bfSize:%d,bfOffBits:%d\n", head.bfSize, head.bfOffBits);

	fread(&info, 1, sizeof(struct tagBITMAPINFOHEADER), rbmp);
	//printf("bitmap: biSize:%d,biWidth:%d,biHeight:%d,biBitCount:%d,biClrUsed:%d,biCompression:%d\n", info.biSize, info.biWidth, info.biHeight, info.biBitCount, info.biClrUsed, info.biCompression);

	//printf("sizeof head:%d\n", sizeof(struct tagBITMAPFILEHEADER));
	//printf("sizeof info:%d\n", sizeof(struct tagBITMAPINFOHEADER));

	uint32_t stride = ((((info.biWidth * info.biBitCount) + 31) & ~31) >> 3);
	uint32_t biSizeImage = abs(info.biHeight) * stride;


	uint8_t count = info.biBitCount / 8;
	printf("GameEngine_LoadTexture:: count:%d\n", count);
	uint8_t* bgrcolors = (uint8_t*)malloc(info.biWidth * info.biHeight * count);
	for (size_t y = 0; y < info.biHeight; y++)
	{
		fread(bgrcolors + y * info.biWidth * count, 1, stride, rbmp);
	}

	return TextureCreate(info.biWidth, info.biHeight, info.biBitCount, bgrcolors, textureID);

}

Color4 UVTextureSample(float u, float v, uint32_t tID) {
	Color4 out = MakeColor4(0, 0, 0, 0);
	if (_getGameEngine()->texture.length == 0)
	{
		printf("texture len == 0 \n");
		return out;
	}
	Texture* texture = (Texture*)GetArrayElementByIndex(&_getGameEngine()->texture, tID - 1);

	u = fmaxf(0.0f, fminf(1.0f, u));
	v = fmaxf(0.0f, fminf(1.0f, v));
	uint32_t tw = texture->width;
	uint32_t th = texture->height;

	uint32_t x = (uint32_t)((float)(tw - 1) * u + 0.5f);
	uint32_t y = (uint32_t)((float)(th - 1) * v + 0.5f);
	uint8_t bytepp = texture->bpp / 8;

	uint32_t index = y * tw * bytepp + x * bytepp;
	if (index + bytepp > tw * th * bytepp)
	{
		return out;
	}
	uint8_t r = texture->data[index + 2];
	uint8_t g = texture->data[index + 1];
	uint8_t b = texture->data[index + 0];
	
	uint8_t a = 255;
	a = bytepp > 3 ? texture->data[index + 3] : 255;

	out = MakeColor4(r, g, b, a);
	return out;

}