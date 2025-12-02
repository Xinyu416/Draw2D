#include "Scene.h"


static MapData* mapData = NULL;

MapData* _getMapData() {
	if (mapData == NULL) {
		mapData = calloc(1, sizeof(MapData));
	}
	return mapData;
}

void Scene_BeginPlay() {
	printf("Scene_BeginPlay(),ID:%d\n", _getGameIns()->pLevel->levelID);
	_getGameIns()->meshs = ArrayCreate(sizeof(Mesh));
	Scene_Init();
}

void Scene_EndPlay() {
	printf("Scene_EndPlay(),ID:%d\n", _getGameIns()->pLevel->levelID);
	ArrayRelease(&_getGameIns()->meshs);
}

void Scene_Tick(float delta) {

}

void Scene_Init() {

	if (_getGameIns()->meshs.length > 0)
	{
		ArrayRelease(&_getGameIns()->meshs);
	}

	SetMapData();
	CreateMeshData(BG, 3, 3, 1);	//0
	CreateMeshData(BEAN, 1, 1, 2);	//1
	CreateMeshData(ITEM, 1, 1, 3);	//2
	GenerateRoleData();				//3
	//设置角色数据
	Mesh* pmesh = (Mesh*)GetArrayElementByIndex(&_getGameIns()->meshs, 3);
	_getGameIns()->cMesh = pmesh;
}

void SetMapData() {
	uint32_t wNum = _getGameEngine()->width / GridSize;
	uint32_t hNum = _getGameEngine()->height / GridSize;
	uint32_t* typeData = malloc(sizeof(uint32_t) * wNum * hNum);
	//7 豆子 8 特殊道具 1-6 地形
	uint32_t data[28 * 31] = {
		3,1,1,1,1,1,1,1,1,1,1,1,1,5,3,1,1,1,1,1,1,1,1,1,1,1,1,5,
		4,7,7,7,7,7,7,7,7,7,7,7,7,4,4,7,7,7,7,7,7,7,7,7,7,7,7,4,
		4,7,3,1,1,5,7,3,1,1,1,5,7,4,4,7,3,1,1,1,5,7,3,1,1,5,7,4,
		4,8,4,6,6,4,7,4,6,6,6,4,7,4,4,7,4,6,6,6,4,7,4,6,6,4,8,4,
		4,7,0,1,1,2,7,0,1,1,1,2,7,0,2,7,0,1,1,1,2,7,0,1,1,2,7,4,
		4,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,4,
		4,7,3,1,1,5,7,3,5,7,3,1,1,1,1,1,1,5,7,3,5,7,3,1,1,5,7,4,
		4,7,0,1,1,2,7,4,4,7,0,1,1,5,3,1,1,2,7,4,4,7,0,1,1,2,7,4,
		4,7,7,7,7,7,7,4,4,7,7,7,7,4,4,7,7,7,7,4,4,7,7,7,7,7,7,4,
		0,1,1,1,1,5,7,4,0,1,1,5,7,4,4,7,3,1,1,2,4,7,3,1,1,1,1,2,
		6,6,6,6,6,4,7,4,3,1,1,2,7,0,2,7,0,1,1,5,4,7,4,6,6,6,6,6,
		6,6,6,6,6,4,7,4,4,7,7,7,7,7,7,7,7,7,7,4,4,7,4,6,6,6,6,6,

		6,6,6,6,6,4,7,4,4,7,3,1,1,6,6,1,1,5,7,4,4,7,4,6,6,6,6,6,
		1,1,1,1,1,2,7,0,2,7,4,6,6,6,6,6,6,4,7,0,2,7,0,1,1,1,1,1,
		7,7,7,7,7,7,7,7,7,7,4,6,6,6,6,6,6,4,7,7,7,7,7,7,7,7,7,7,
		1,1,1,1,1,5,7,3,5,7,4,6,6,6,6,6,6,4,7,3,5,7,3,1,1,1,1,1,
		6,6,6,6,6,4,7,4,4,7,0,1,1,1,1,1,1,2,7,4,4,7,4,6,6,6,6,6,
		6,6,6,6,6,4,7,4,4,7,7,7,7,7,7,7,7,7,7,4,4,7,4,6,6,6,6,6,
		6,6,6,6,6,4,7,4,4,7,3,1,1,1,1,1,1,5,7,4,4,7,4,6,6,6,6,6,
		3,1,1,1,1,2,7,0,2,7,0,1,1,5,3,1,1,2,7,0,2,7,0,1,1,1,1,5,
		4,7,7,7,7,7,7,7,7,7,7,7,7,4,4,7,7,7,7,7,7,7,7,7,7,7,7,4,
		4,7,3,1,1,5,7,3,1,1,1,5,7,4,4,7,3,1,1,1,5,7,3,1,1,5,7,4,
		4,7,0,1,5,4,7,0,1,1,1,2,7,0,2,7,0,1,1,1,2,7,4,3,1,2,7,4,
		4,8,7,7,4,4,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,4,4,7,7,8,4,
		0,1,5,7,4,4,7,3,5,7,3,1,1,1,1,1,1,5,7,3,5,7,4,4,7,3,1,2,
		3,1,2,7,0,2,7,4,4,7,0,1,1,5,3,1,1,2,7,4,4,7,0,2,7,0,1,5,
		4,7,7,7,7,7,7,4,4,7,7,7,7,4,4,7,7,7,7,4,4,7,7,7,7,7,7,4,
		4,7,3,1,1,1,1,2,0,1,1,5,7,4,4,7,3,1,1,2,0,1,1,1,1,5,7,4,
		4,7,0,1,1,1,1,1,1,1,1,2,7,0,2,7,0,1,1,1,1,1,1,1,1,2,7,4,
		4,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,4,
		0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2
	};
	for (size_t i = 0; i < wNum * hNum; i++)
	{
		typeData[i] = data[i];
	}
	MapData* mapData = _getMapData();
	mapData->wNum = wNum;
	mapData->hNum = hNum;
	mapData->typeData = typeData;
}

void CreateMeshData(MESHTYPE meshType, uint32_t texWidth, uint32_t texHeight, uint8_t textureId) {
	uint32_t wNum = _getMapData()->wNum;
	uint32_t hNum = _getMapData()->hNum;
	Geometry geo = { 0 };
	Vect2 offset = MakeVect2(_getGameIns()->pCam->width / 2, _getGameIns()->pCam->height / 2);

	//quad 计数
	uint32_t count = 0;
	switch (meshType)
	{
	case BG:
		geo = CreateGeometry(wNum * hNum);
		break;
	case BEAN:
		count = 0;
		for (size_t i = 0; i < wNum * hNum; i++)
		{
			if (_getMapData()->typeData[i] == 7) {
				count++;
			}
		}
		geo = CreateGeometry(count);
		break;
	case ITEM:
		count = 0;
		for (size_t i = 0; i < wNum * hNum; i++)
		{
			if (_getMapData()->typeData[i] == 8) {
				count++;
			}
		}
		geo = CreateGeometry(count);
		break;
	case ROLE:
		break;
	default:
		break;
	}
	//坐标原点 根据Camera位置和窗口设计换算来的
	uint8_t texType = 0;
	uint32_t index = 0;
	//以中心对齐的方式建mesh
	for (size_t y = 0; y < hNum; y++)
	{
		for (size_t x = 0; x < wNum; x++)
		{
			//根据地图数据提取用的贴图类型 再根据贴图类型换取贴图坐标
			texType = _getMapData()->typeData[index];
			Vect2* uvs = getUVbyType((uint8_t)meshType, texType, texWidth, texHeight);
			Vect2* vertices = getVeticesbyCoord(x, y, offset, GridSize);
			switch (meshType)
			{
			case BG:
				Quad quad = {
					.vertices = {vertices[0],vertices[1],vertices[2],vertices[3],vertices[4],vertices[5]},
					 .uvs = {uvs[0],uvs[1],uvs[2],uvs[3],uvs[4],uvs[5]},
					 .color = {255,255,255,255, 255,255,255,255 ,255,255,255,255, 255,255,255,255, 255,255,255,255, 255,255,255,255},
					.type = texType
				};
				GeometryAddQuad(&geo, quad);
				break;
			case BEAN:
				if (texType == 7)
				{
					Quad quad = {
					.vertices = {vertices[0],vertices[1],vertices[2],vertices[3],vertices[4],vertices[5]},
					 .uvs = {uvs[0],uvs[1],uvs[2],uvs[3],uvs[4],uvs[5]},
					  .color = {255,255,255,255, 255,255,255,255 ,255,255,255,255, 255,255,255,255, 255,255,255,255, 255,255,255,255},
					.type = texType
					};
					GeometryAddQuad(&geo, quad);
				}
				break;
			case ITEM:
				if (texType == 8)
				{
					Quad quad = {
					.vertices = {vertices[0],vertices[1],vertices[2],vertices[3],vertices[4],vertices[5]},
					 .uvs = {uvs[0],uvs[1],uvs[2],uvs[3],uvs[4],uvs[5]},
					  .color = {255,255,255,255, 255,255,255,255 ,255,255,255,255, 255,255,255,255, 255,255,255,255, 255,255,255,255},
					.type = texType
					};
					GeometryAddQuad(&geo, quad);
				}
				break;
			default:
				break;
			}
			index++;
		}
	}
	Matrix tm = CreateStandardMatrix();
	Material mat = { .color = MakeColor4(10,255,255,255),.textureId = textureId };
	Mesh mesh = CreateMesh(textureId, _getGameIns()->pCam->pos, 0, MakeVect2(1.f, 1.f), geo, tm, mat);
	ArrayPush(&_getGameIns()->meshs, &mesh);
}

void GenerateRoleData() {
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
	Vect2 pos = MakeVect2(0, 200);
	//printf("RolePos:[%f,%f]\n", pos.x, pos.y);
	Mesh mesh = CreateMesh(4, pos, -90, MakeVect2(1.f, 1.f), geo, tm, mat);
	ArrayPush(&_getGameIns()->meshs, &mesh);
}

//通过坐标（格子）得到顶点数据
Vect2* getVeticesbyCoord(uint32_t x, uint32_t y, Vect2 offset, float gridSize) {
	Vect2 vert[6];
	float posx = offset.x;
	float posy = offset.y;
	vert[0] = MakeVect2(x * gridSize - posx, (y + 1) * gridSize - posy);
	vert[1] = MakeVect2((x + 1) * gridSize - posx, (y + 1) * gridSize - posy);
	vert[2] = MakeVect2((x + 1) * gridSize - posx, y * gridSize - posy);
	vert[3] = MakeVect2((x + 1) * gridSize - posx, y * gridSize - posy);
	vert[4] = MakeVect2(x * gridSize - posx, y * gridSize - posy);
	vert[5] = MakeVect2(x * gridSize - posx, (y + 1) * gridSize - posy);
	return vert;
}

//通过角色行进的下一个位置判断当前地图格子类型是否可以行动
int getMapDataByPos(Vect2 pos) {
	//需要偏移一个格子的距离
	Vect2 offset = MakeVect2(_getGameIns()->pCam->width / 2, _getGameIns()->pCam->height / 2);
	Vect2 newPos = AddVect2(pos, offset);
	printf("inpos:[%f,%f],newPos:[%f,%f]\n", pos.x, pos.y, newPos.x, newPos.y);
	float gridLocX = (newPos.x / _getGameIns()->pCam->width) * (float)_getMapData()->wNum;
	float gridLocY = (newPos.y / _getGameIns()->pCam->height) * (float)_getMapData()->hNum;
	uint32_t mapIndex = (uint32_t)gridLocY * _getMapData()->wNum + (uint32_t)gridLocX;
	printf("gridLocX:%f,gridLocY:%f,mapIndex:%d\n", gridLocX, gridLocY, mapIndex);

	uint8_t mapType = _getMapData()->typeData[mapIndex];
	if (mapType == 7)
	{
		//豆子路径 可以行动
		return mapIndex;
	}
	return -1;
}

//吃豆子 改变豆子颜色
void ChangeBeanColor(uint32_t mapIndex) {
	//提取豆子mesh
	Mesh* pmeshBean = (Mesh*)GetArrayElementByIndex(&_getGameIns()->meshs, 1);
	uint32_t count = 0;
	//通过mapIndex得到bean的index
	for (size_t i = 0; i < mapIndex; i++)
	{
		if (_getMapData()->typeData[i] == 7)
		{
			count++;
		};
	}
	//printf("ChangeBeanColor::count:%d\n", count);
	for (size_t i = 0; i < 24; i++)
	{
		pmeshBean->geo.colors[count * 24 + i] = 0;
	}
}

void Scene_MouseKeyEvent(VMEVENT eventType, void* key) {
	float moveStep = 12.5f;
	float preMove = 0.f;
	int mapIndex = 0;
	switch (eventType)
	{
	case KEYDOWN:
		printf("Scene_MouseKeyEvent type:%d,key:%c\n", eventType, (char)key);
		switch ((char)key)
		{
		case 'W':
			printf("向上移动\n");
			preMove = _getGameIns()->cMesh->pos.y - moveStep;
			PrintVect2(_getGameIns()->cMesh->pos);
			mapIndex = getMapDataByPos(MakeVect2(_getGameIns()->cMesh->pos.x, preMove));
			if (mapIndex > -1)
			{
				_getGameIns()->cMesh->pos.y = preMove;
				_getGameIns()->cMesh->rot = -90.f;
				ChangeBeanColor(mapIndex);
			}
			break;
		case 'A':
			printf("向左移动\n");
			preMove = _getGameIns()->cMesh->pos.x - moveStep;
			mapIndex = getMapDataByPos(MakeVect2(preMove, _getGameIns()->cMesh->pos.y));
			if (mapIndex > -1)
			{
				_getGameIns()->cMesh->pos.x = preMove;
				_getGameIns()->cMesh->rot = 180.f;
				ChangeBeanColor(mapIndex);
			}
			break;
		case 'S':
			printf("向下移动\n");
			preMove = _getGameIns()->cMesh->pos.y + moveStep;
			mapIndex = getMapDataByPos(MakeVect2(_getGameIns()->cMesh->pos.x, preMove));
			if (mapIndex > -1)
			{
				_getGameIns()->cMesh->pos.y = preMove;
				_getGameIns()->cMesh->rot = 90.f;
				ChangeBeanColor(mapIndex);
			}
			break;
		case 'D':
			printf("向右移动\n");
			preMove = _getGameIns()->cMesh->pos.x + moveStep;
			mapIndex = getMapDataByPos(MakeVect2(preMove, _getGameIns()->cMesh->pos.y));
			if (mapIndex > -1)
			{
				_getGameIns()->cMesh->pos.x = preMove;
				_getGameIns()->cMesh->rot = 0.f;
				ChangeBeanColor(mapIndex);
			}
			break;
		default:
			break;
		}
		break;
	case LMBD:
		//获取窗口中的鼠标位置
		int x = GET_X_LPARAM(key);
		int y = GET_Y_LPARAM(key);
		printf("Scene_MouseKeyEvent LeftMouseButtonDown: (%d, %d)\n", x, y);
		//判断点击屏幕中心按元素大小的区域
		if (x > (_getGameIns()->pCam->width / 2 - 25) && x < (_getGameIns()->pCam->width / 2 + 25) && y>(_getGameIns()->pCam->height / 2 - 25) && y < (_getGameIns()->pCam->height / 2 + 25))
		{
			printf("Scene_MouseKeyEvent -- Switch Level\n");
			GameIns_OpenLevel(2);
		}
		break;
	default:
		break;
	}
}
