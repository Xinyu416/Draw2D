#define _CRT_SECURE_NO_WARNINGS
#include "GameEngine.h"


GameEngine* _gameEngne = NULL;
void GameEngineInit(uint8_t fps) {
	GameEngine* gameengine = _getGameEngine();
	gameengine->fps = fps;
	gameengine->gameIsRuning = true;
	gameengine->criticalSection_render = (CRITICAL_SECTION*)malloc(sizeof(CRITICAL_SECTION));
	gameengine->msgAssist = CreateMessageAssistant();

	//创建线程
	//RendererThread* thread = (RendererThread*)malloc(sizeof(RendererThread));
	//thread->handle = CreateThread(NULL, 0, Renderer_ThreadMain, thread, 0, &thread->id);
	//
	//Message send = { .type = MESSAGE_TYPE1,.data[0] = 1 };
	//Message back = SendMessageToThread(gameengine->msgAssist, send, true);

	InitializeCriticalSection(gameengine->criticalSection_render);

	//创建贴图数据
	const char* path1 = "C:\\Users\\DRF\\Desktop\\Temp\\bg.bmp";
	const char* path2 = "C:\\Users\\DRF\\Desktop\\Temp\\bean.bmp";
	const char* path3 = "C:\\Users\\DRF\\Desktop\\Temp\\item.bmp";
	const char* path4 = "C:\\Users\\DRF\\Desktop\\Temp\\role03.bmp";
	Texture tex1 = GameEngine_LoadTexture(path1, 1);
	Texture tex2 = GameEngine_LoadTexture(path2, 2);
	Texture tex3 = GameEngine_LoadTexture(path3, 3);
	Texture tex4 = GameEngine_LoadTexture(path4, 4);

	//提交贴图数据
	Renderer_SubmitTexture(tex1.data, tex1.width, tex1.height, tex1.bpp);
	Renderer_SubmitTexture(tex2.data, tex2.width, tex2.height, tex2.bpp);
	Renderer_SubmitTexture(tex3.data, tex3.width, tex3.height, tex3.bpp);
	Renderer_SubmitTexture(tex4.data, tex4.width, tex4.height, tex4.bpp);

	Array arr = ArrayCreate(sizeof(Texture));
	ArrayPush(&arr, &tex1);
	ArrayPush(&arr, &tex2);
	ArrayPush(&arr, &tex3);
	ArrayPush(&arr, &tex4);
	gameengine->texture = arr;

	//Instance初始化
	GameIns_Init();

	//创建渲染子线程
	//Renderer_TaskMain();
}

void GameEngin_SceneLoop(float delta) {

	GameIns_Tick(delta);

	//拿锁
	EnterCriticalSection(_getGameEngine()->criticalSection_render);
	//每帧提交相机和mesh信息
	Renderer_SubmitCamera(*(_getGameIns()->pCam));
	for (size_t i = 0; i < _getGameIns()->meshs.length; i++)
	{
		Mesh* m = GetArrayElementByIndex(&_getGameIns()->meshs, i);
		Renderer_SubmitMesh(m->pos, m->tm, m->rot, m->scale, m->mat,m->id);
	}
	//解锁
	LeaveCriticalSection(_getGameEngine()->criticalSection_render);

	GameEngine_DrawBg();
	
	//Renderer_Tick(delta);

	//每帧清除动态内存
	Renderer_Release(2);
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

}

void GameEngine_DrawBg() {

	Renderer_DrawBg();
}

void GameEngine_Release() {

	//删除锁
	DeleteCriticalSection(_getGameEngine()->criticalSection_render);

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
	//printf("path:%s\n", path);
	if (rbmp == NULL)
	{
		printf("File is NULL\n");
		Texture texture = { .bpp = 0,.data = NULL,.height = 0,.width = 0 };
		return texture;
	}
	struct tagBITMAPFILEHEADER head;
	struct tagBITMAPINFOHEADER info;
	fread(&head, 1, sizeof(struct tagBITMAPFILEHEADER), rbmp);
	fread(&info, 1, sizeof(struct tagBITMAPINFOHEADER), rbmp);

	//一行像素补齐到DWORD对齐的值
	uint32_t stride = ((((info.biWidth * info.biBitCount) + 31) & ~31) >> 3);
	uint32_t biSizeImage = abs(info.biHeight) * stride;

	uint8_t bytepp = info.biBitCount / 8;
	uint8_t* bgrcolors = (uint8_t*)malloc(info.biWidth * info.biHeight * bytepp);
	for (size_t y = 0; y < info.biHeight; y++)
	{
		fread(bgrcolors + y * info.biWidth * bytepp, 1, stride, rbmp);
	}
	return TextureCreate(info.biWidth, info.biHeight, info.biBitCount, bgrcolors, textureID);

}

Color4 UVTextureSample(float u, float v, uint32_t tID) {
	Color4 out = MakeColor4(0, 0, 0, 0);
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

	uint8_t a = bytepp > 3 ? texture->data[index + 3] : 255;

	out = MakeColor4(r, g, b, a);
	return out;

}

void GameEngine_MouseKeyEvent(VMEVENT eventType, void* key) {
	GameIns_MouseKeyEvent(eventType, key);
	//float moveStep = 12.5f;
	//float preMove = 0.f;
	//int mapIndex = 0;
	//switch ((char)key)
	//{
	//case 'W':

	//	printf("向上移动\n");
	//	preMove = _getGameIns()->cMesh->pos.y - moveStep;
	//	PrintVect2(_getGameIns()->cMesh->pos);
	//	mapIndex = getMapDataByPos(MakeVect2(_getGameIns()->cMesh->pos.x, preMove));
	//	if (mapIndex > -1)
	//	{
	//		_getGameIns()->cMesh->pos.y = preMove;
	//		_getGameIns()->cMesh->rot = -90.f;
	//		//ChangeBeanColor(mapIndex);
	//	}
	//	break;
	//case 'A':
	//	printf("向左移动\n");
	//	preMove = _getGameIns()->cMesh->pos.x - moveStep;
	//	mapIndex = getMapDataByPos(MakeVect2(preMove, _getGameIns()->cMesh->pos.y));
	//	if (mapIndex > -1)
	//	{
	//		_getGameIns()->cMesh->pos.x = preMove;
	//		_getGameIns()->cMesh->rot = 180.f;
	//		//ChangeBeanColor(mapIndex);
	//	}

	//	break;
	//case 'S':
	//	printf("向下移动\n");
	//	preMove = _getGameIns()->cMesh->pos.y + moveStep;
	//	mapIndex = getMapDataByPos(MakeVect2(_getGameIns()->cMesh->pos.x, preMove));
	//	if (mapIndex > -1)
	//	{
	//		_getGameIns()->cMesh->pos.y = preMove;
	//		_getGameIns()->cMesh->rot = 90.f;
	//		//ChangeBeanColor(mapIndex);
	//	}
	//	break;
	//case 'D':
	//	printf("向右移动\n");
	//	preMove = _getGameIns()->cMesh->pos.x + moveStep;
	//	mapIndex = getMapDataByPos(MakeVect2(preMove, _getGameIns()->cMesh->pos.y));
	//	if (mapIndex > -1)
	//	{
	//		_getGameIns()->cMesh->pos.x = preMove;
	//		_getGameIns()->cMesh->rot = 0.f;
	//		//ChangeBeanColor(mapIndex);
	//	}
	//	break;
	//default:
	//	break;
	//}
}
