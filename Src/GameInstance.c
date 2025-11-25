#include "GameInstance.h"
#include "Mesh.h"


static GameInstance* _gameIns = NULL;
void GameIns_Init() {
	GameInstance* gameIns = _getGameIns();
	gameIns->meshs = ArrayCreate(sizeof(Mesh));
	gameIns->cMesh = NULL;
	CreateMeshTest();
	Camera* cam = CreateCamera(400, 4.0f / 3.0f, 0, MakeVect2(400, 300), MakeVect2(1, 1));
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
	/*GeometryAddQuad(&geo, quad2);
	GeometryAddQuad(&geo, quad3);
	GeometryAddQuad(&geo, quad4);*/
	printf("numOfQuad:%d\n", geo.numOfQuad);

	Matrix tm = CreateStandardMatrix();
	Material mat = { .color = MakeColor4(10,255,255,255),.textureId = 1 };
	Mesh mesh = CreateMesh(1, MakeVect2(400.f, 400.f), 0, MakeVect2(1.5f, 1.5f), geo, tm, mat);
	ArrayPush(&_getGameIns()->meshs, &mesh);

	Mesh* pmesh = (Mesh*)GetArrayElementByIndex(&_getGameIns()->meshs, 0);
	_getGameIns()->cMesh = pmesh;

}

void GameIns_Release() {
	ArrayRelease(&_getGameIns()->meshs);
}

void GameIns_Tick(float delta) {
	//Ðý×ªmesh
	_getGameIns()->cMesh->rot += 2.f;
}

GameInstance* _getGameIns() {
	if (_gameIns == NULL) {
		_gameIns = calloc(1, sizeof(GameInstance));
	}
	return _gameIns;
}