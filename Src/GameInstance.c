#include "GameInstance.h"
#include "Mesh.h"
#include "UserDefine.h"

static GameInstance* _gameIns = NULL;

GameInstance* _getGameIns() {
	if (_gameIns == NULL) {
		_gameIns = calloc(1, sizeof(GameInstance));
	}
	return _gameIns;
}

void GameIns_Init() {
	GameInstance* gameIns = _getGameIns();

	GameIns_OpenLevel(2);
}

void GameIns_Release() {
	ArrayRelease(&_getGameIns()->meshs);
}

void GameIns_Tick(float delta) {
	if (_getGameIns()->pLevel != NULL)
	{
		_getGameIns()->pLevel->Tick(delta);
	}
}

void GameIns_MouseKeyEvent(VMEVENT eventType, void* key) {
	//键盘事件分发给Level
	_getGameIns()->pLevel->MouseKeyEvent(eventType, key);
}

void GameIns_OpenLevel(uint8_t levelID) {
	GameInstance* gameIns = _getGameIns();
	if (gameIns->pLevel != NULL)
	{
		gameIns->pLevel->EndPlay();
	}
	OPENLEVEL(gameIns, levelID);
	gameIns->pLevel->BeginPlay();
}

void GameIns_CreateMesh() {
	_getGameIns()->meshs = ArrayCreate(sizeof(Mesh));
}

void GameIns_ReleaseMesh() {
	ArrayRelease(&_getGameIns()->meshs);
	_getGameIns()->cMesh = NULL;
}

void GameIns_CreateCamera(float width, float ratio, float rot, Vect2 pos, Vect2 scale) {
	//Camera* cam = CreateCamera(700, 700.0f / 775.0f, 0, MakeVect2(0, 0), MakeVect2(1, 1));
	Camera* cam = CreateCamera(width, ratio, rot, pos, scale);
	_getGameIns()->pCam = cam;
}

void GameIns_ReleaseCamera() {
	free(_getGameIns()->pCam);
	_getGameIns()->pCam = NULL;
}
