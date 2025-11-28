#include "GameInstance.h"
#include "Mesh.h"


static GameInstance* _gameIns = NULL;
void GameIns_Init() {
	GameInstance* gameIns = _getGameIns();
	gameIns->meshs = ArrayCreate(sizeof(Mesh));
	gameIns->cMesh = NULL;
	//CreateMeshTest();
	Camera* cam = CreateCamera(700, 700.0f / 775.0f, 0, MakeVect2(100.f,100.f), MakeVect2(1, 1));
	gameIns->pCam = cam;
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

void GameIns_Release() {
	ArrayRelease(&_getGameIns()->meshs);
}

void GameIns_Tick(float delta) {
	//Ðý×ªmesh
	//for (size_t i = 0; i < _getGameIns()->meshs.length; i++)
	//{
	//	Mesh* pmesh = (Mesh*)GetArrayElementByIndex(&_getGameIns()->meshs, i);
	//	pmesh->rot += i * 2.f + 2.f;
	//}
	//_getGameIns()->cMesh->rot += 2.f;
}

GameInstance* _getGameIns() {
	if (_gameIns == NULL) {
		_gameIns = calloc(1, sizeof(GameInstance));
	}
	return _gameIns;
}