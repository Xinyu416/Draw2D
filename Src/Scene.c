#include "Scene.h"

static MapData* mapData = NULL;

MapData* _getMapData() {
	if (mapData == NULL) {
		mapData = calloc(1, sizeof(MapData));
	}
	return mapData;
}

void Scene_Init() {
	SetMapData();
	CreateMeshData(BG, 3, 3, 1);
	CreateMeshData(BEAN, 1, 1, 2);
	CreateMeshData(ITEM, 1, 1, 3);
	GenerateRoleData();
	//GenerateGridData();
	//GenerateBeansData();
	//GenerateItemData();
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
		6,6,6,6,6,4,7,4,4,7,3,1,1,1,1,1,1,5,7,4,4,7,4,6,6,6,6,6,
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
	Mesh mesh = CreateMesh(1, _getGameIns()->pCam->pos, 0, MakeVect2(1.f, 1.f), geo, tm, mat);
	ArrayPush(&_getGameIns()->meshs, &mesh);
}

void GenerateRoleData() {
	Vect2* uvs = getUVbyType(0, 0, 1, 1);
	Geometry geo = CreateGeometry(1);
	//Vect2* vertices = getVeticesbyCoord(x, y, offset, GridSize);
	Quad quad = {
				.vertices = {-25.f,25.f, 25.f,25.f, 25.f,-25.f, 25.f,-25.f, -25.f,-25.f, -25.f,25.f},
				.uvs = {uvs[0],uvs[1],uvs[2],uvs[3],uvs[4],uvs[5]},
				//.uvs = {0,0,0.33f,0,0.33f, 1,0.33f,1,0,1,0,0},
			   .type = 0
	};
	GeometryAddQuad(&geo, quad);
	Matrix tm = CreateStandardMatrix();
	Material mat = { .color = MakeColor4(10,255,255,255),.textureId = 4 };
	Mesh mesh = CreateMesh(1, _getGameIns()->pCam->pos, 0, MakeVect2(1.f, 1.f), geo, tm, mat);
	ArrayPush(&_getGameIns()->meshs, &mesh);
}

void GenerateGridData() {

	CreateMeshData(BG, 3, 3, 1);
	//uint32_t wNum = _getMapData()->wNum;
	//uint32_t hNum = _getMapData()->hNum;
	//uint32_t texWidth = 3;
	//uint32_t texHeight = 3;
	//Geometry geo = CreateGeometry(wNum * hNum);
	////坐标原点 根据Camera位置和窗口设计换算来的
	//Vect2 offset = MakeVect2(_getGameIns()->pCam->width / 2, _getGameIns()->pCam->height / 2);
	//uint8_t type = 0;
	//uint32_t index = 0;
	////以中心对齐的方式建mesh
	//for (size_t y = 0; y < hNum; y++)
	//{
	//	for (size_t x = 0; x < wNum; x++)
	//	{
	//		type = _getMapData()->typeData[index];
	//		Vect2* uvs = getUVbyType(type, texWidth, texHeight);
	//		Vect2* vertices = getVeticesbyCoord(x, y, offset, GridSize);
	//		Quad quad = {
	//					.vertices = {vertices[0],vertices[1],vertices[2],vertices[3],vertices[4],vertices[5]},
	//					 .uvs = {uvs[0],uvs[1],uvs[2],uvs[3],uvs[4],uvs[5]},
	//					.type = type
	//		};
	//		GeometryAddQuad(&geo, quad);
	//		index++;
	//	}
	//}
	//Matrix tm = CreateStandardMatrix();
	//Material mat = { .color = MakeColor4(10,255,255,255),.textureId = 1 };
	//Mesh mesh = CreateMesh(1, _getGameIns()->pCam->pos, 0, MakeVect2(1.f, 1.f), geo, tm, mat);
	//ArrayPush(&_getGameIns()->meshs, &mesh);
}

void GenerateBeansData() {
	CreateMeshData(BEAN, 1, 1, 2);
	//uint32_t wNum = _getMapData()->wNum;
	//uint32_t hNum = _getMapData()->hNum;
	//uint8_t type = 0;
	//uint32_t count = 0;
	////统计quad数量
	//for (size_t i = 0; i < wNum * hNum; i++)
	//{
	//	if (_getMapData()->typeData[i] == 7) {
	//		count++;
	//	}
	//}
	//Geometry geo = CreateGeometry(count);
	//Vect2 offset = MakeVect2(_getGameIns()->pCam->width / 2, _getGameIns()->pCam->height / 2);
	////以中心对齐的方式建mesh
	//uint32_t index = 0;

	//for (size_t y = 0; y < hNum; y++)
	//{
	//	for (size_t x = 0; x < wNum; x++)
	//	{
	//		type = _getMapData()->typeData[index];

	//		if (type == 7)
	//		{
	//			Vect2* vertices = getVeticesbyCoord(x, y, offset, GridSize);
	//			Vect2* uvs = getUVbyType(0, 1, 1);
	//			Quad quad = {
	//				.vertices = {vertices[0],vertices[1],vertices[2],vertices[3],vertices[4],vertices[5]},
	//						 .uvs = {uvs[0],uvs[1],uvs[2],uvs[3],uvs[4],uvs[5]},
	//						.type = type
	//			};
	//			GeometryAddQuad(&geo, quad);
	//		}
	//		index++;
	//	}
	//}
	//Matrix tm = CreateStandardMatrix();
	//Material mat = { .color = MakeColor4(10,255,255,255),.textureId = 2 };
	//Mesh mesh = CreateMesh(2, _getGameIns()->pCam->pos, 0, MakeVect2(1.f, 1.f), geo, tm, mat);
	//ArrayPush(&_getGameIns()->meshs, &mesh);
}

void GenerateItemData() {
	CreateMeshData(ITEM, 1, 1, 3);
	//uint32_t wNum = _getMapData()->wNum;
	//uint32_t hNum = _getMapData()->hNum;
	//printf("GenerateBeansData:: wNum:%d,hNum%d\n", wNum, hNum);
	//uint32_t count = 0;
	//Vect2 offset = MakeVect2(_getGameIns()->pCam->width / 2, _getGameIns()->pCam->height / 2);
	////统计quad数量
	//for (size_t i = 0; i < wNum * hNum; i++)
	//{
	//	if (_getMapData()->typeData[i] == 8) {
	//		count++;
	//	}
	//}
	//Geometry geo = CreateGeometry(count);
	//uint8_t type = 0;

	////以中心对齐的方式建mesh
	//uint32_t index = 0;
	//for (size_t y = 0; y < hNum; y++)
	//{
	//	for (size_t x = 0; x < wNum; x++)
	//	{
	//		type = _getMapData()->typeData[index];

	//		if (type == 8)
	//		{
	//			Vect2* vertices = getVeticesbyCoord(x, y, offset, GridSize);
	//			Vect2* uvs = getUVbyType(0, 1, 1);
	//			Quad quad = {
	//				.vertices = {vertices[0],vertices[1],vertices[2],vertices[3],vertices[4],vertices[5]},
	//						 .uvs = {uvs[0],uvs[1],uvs[2],uvs[3],uvs[4],uvs[5]},
	//						.type = type
	//			};
	//			GeometryAddQuad(&geo, quad);
	//		}
	//		index++;
	//	}
	//}
	//Matrix tm = CreateStandardMatrix();
	//Material mat = { .color = MakeColor4(10,255,255,255),.textureId = 3 };
	//Mesh mesh = CreateMesh(3, _getGameIns()->pCam->pos, 0, MakeVect2(1.f, 1.f), geo, tm, mat);
	//ArrayPush(&_getGameIns()->meshs, &mesh);
}