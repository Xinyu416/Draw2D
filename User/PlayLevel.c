#include "PlayLevel.h"
#include "GameInstance.h"

void PlayLevel_BeginPlay() {
	printf("PlayLevel_BeginPlay(),ID:%d\n", _getGameIns()->pLevel->levelID);
	_getGameIns()->meshs = ArrayCreate(sizeof(Mesh));
	PlayLevelInit();
}

void PlayLevel_EndPlay() {
	/*for (size_t i = 0; i < _getGameIns()->meshs.length; i++)
	{
		ArrayDelete(&_getGameIns()->meshs, 0);
	}*/
	ArrayRelease(&_getGameIns()->meshs);
	printf("PlayLevel_EndPlay(),ID:%d\n", _getGameIns()->pLevel->levelID);
}


void PlayLevel_Tick(float delta) {
	if (_getGameIns()->cMesh)
	{
		_getGameIns()->cMesh->rot += 2.f;
	}
	//printf("PlayLevel_Tick(),ID:%d,delta:%f\n", _getGameIns()->pLevel->levelID, delta);
}

void PlayLevel_MouseKeyEvent(VMEVENT eventType, void* key) {
	//接收处理键盘事件
	switch (eventType)
	{
	case KEYDOWN:
		printf("PlayLevel_MouseKeyEvent type:%d, key:%c\n", eventType, (char)key);
		break;
	case LMBD:
		//获取窗口中的鼠标位置
		int x = GET_X_LPARAM(key);
		int y = GET_Y_LPARAM(key);
		printf("LeftMouseButtonDown: (%d, %d)\n", x, y);
		//判断点击屏幕中心按元素大小的区域
		if (x > (_getGameIns()->pCam->width / 2 - 25) && x < (_getGameIns()->pCam->width / 2 + 25) && y>(_getGameIns()->pCam->height / 2 - 25) && y < (_getGameIns()->pCam->height / 2 + 25))
		{
			GameIns_OpenLevel(3);
		}
		break;
	default:
		break;
	}
}

void PlayLevelInit() {
	Vect2* uvs = getUVbyType(0, 0, 1, 1);
	Geometry geo = CreateGeometry(1);
	//Vect2* vertices = getVeticesbyCoord(x, y, offset, GridSize);
	Vect2 offset = MakeVect2(_getGameIns()->pCam->width / 2, _getGameIns()->pCam->height / 2);
	Quad quad = {
				.vertices = {-25.f,25.f, 25.f,25.f, 25.f,-25.f, 25.f,-25.f, -25.f,-25.f, -25.f,25.f},
				.uvs = {uvs[0],uvs[1],uvs[2],uvs[3],uvs[4],uvs[5]},
				 .color = {255,255,255,255, 255,255,255,255 ,255,255,255,255, 255,255,255,255, 255,255,255,255, 255,255,255,255},
			   .type = 0
	};
	GeometryAddQuad(&geo, quad);
	Matrix tm = CreateStandardMatrix();
	Material mat = { .color = MakeColor4(10,255,255,255),.textureId = 4 };
	//Vect2 pos = MakeVect2(_getGameIns()->pCam->pos.x - GridSize * 4, _getGameIns()->pCam->pos.y - GridSize * 4);
	Vect2 pos = MakeVect2(0, 0);
	Mesh mesh = CreateMesh(4, pos, -90, MakeVect2(1.f, 1.f), geo, tm, mat);
	ArrayPush(&_getGameIns()->meshs, &mesh);

	Mesh* pmesh = (Mesh*)GetArrayElementByIndex(&_getGameIns()->meshs, 0);
	_getGameIns()->cMesh = pmesh;
}