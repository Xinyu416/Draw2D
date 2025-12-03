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
	//gameIns->cMesh = NULL;
	/*创建Mesh数组，场景初始化创建*/
	//GameIns_CreateMesh();
	/*创建Camera，场景初始创建*/
	//GameIns_CreateCamera(700, 700.0f / 775.0f, 0, MakeVect2(0, 0), MakeVect2(1, 1));
	/*OpenStartLevel*/
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

void CreateMeshTest() {
	Quad quad1 = {
		.vertices = {-110, -10, 110, -10, 110, -100,110, -105, -110, -110,-110, -15},
		.uvs = {
			0.0f, 0.f, 1.0f, 0.0f, 1.f, 1.f ,
			1.f, 1.f, 0.0f, 1.0f, 0.f, 0.f},
		.color = {255,0,0,255, 0,255,0,255, 0,0,255,255, 255,255,0,255, 0,255,255,255, 255,0,255,255}
	};

	Quad quad2 = { .vertices = {-50, 50,-20, 50,-20, 20,-20, 20,-50, 20,-50, 50},
		.uvs = {
			0.0f, 0.f, 1.0f, 0.0f, 1.f, 1.f ,
			1.f, 1.f, 0.0f, 1.0f, 0.f, 0.f},
				.color = {255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255,0,255,255,255,255,255,255,255}
	};

	Quad quad3 = { .vertices = {30, 60,60, 60,60, 30,60, 30, 30, 30,30, 60},
				.color = {255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255,0,255,255,255,255,255,255,255}
	};

	Quad quad4 = { .vertices = { -4, 6, 6, 6, 6, -4, 6, -6, -6, -6,-6, 4},
				.color = {255,0,0,255, 0,255,0,255, 0,0,255,255, 255,255,0,255, 0,255,255,255, 255,255,255,255}
	};

	Geometry geo = CreateGeometry(4);
	GeometryAddQuad(&geo, quad1);
	Geometry geo2 = CreateGeometry(1);
	GeometryAddQuad(&geo2, quad2);
	//GeometryAddQuad(&geo, quad3);
	//GeometryAddQuad(&geo, quad4);
	printf("numOfQuad:%d\n", geo.numOfQuad);

	Matrix tm = CreateStandardMatrix();
	Material mat = { .color = MakeColor4(10,255,255,255),.textureId = 2 };
	Material mat2 = { .color = MakeColor4(10,255,255,255),.textureId = 1 };
	Mesh mesh = CreateMesh(1, MakeVect2(400.f, 400.f), 0, MakeVect2(1.5f, 1.5f), geo, tm, mat);
	Mesh mesh2 = CreateMesh(2, MakeVect2(450.f, 450.f), 0, MakeVect2(1.5f, 1.5f), geo2, tm, mat2);
	ArrayPush(&_getGameIns()->meshs, &mesh);
	ArrayPush(&_getGameIns()->meshs, &mesh2);
	Mesh* pmesh = (Mesh*)GetArrayElementByIndex(&_getGameIns()->meshs, 0);
	_getGameIns()->cMesh = pmesh;

}
