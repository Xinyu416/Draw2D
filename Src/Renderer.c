#include "Renderer.h"

typedef struct {
	float* vertices;		//2*3  顶点数组			8byte(64位操作系统指针是8字节)
	float* uvs;				//2*3  顶点uv数组		8byte	
	float* bboxes;			//1*2  边界盒			8byte
	float* clipVertices;	//2*3  裁切空间点信息		8byte
	uint32_t numOfVetices;  //*3   顶点数			4byte
	uint32_t id;			//	   顶点id			4byte
}Geo;

typedef struct {
	uint8_t* pixels;	//贴图像素数据				8byte
	uint32_t width;		//贴图宽度					4byte	
	uint32_t height;	//贴图高度					4byte
	uint32_t id;		//贴图id						4byte
	uint8_t bpp;		//byte per pixel			1byte
	uint8_t block[3];	//							3byte
}Tex;

static Renderer* _renderer = NULL;

Renderer* _getRenderer() {
	if (_renderer == NULL)
	{
		_renderer = (Renderer*)calloc(1, sizeof(Renderer));
	}
	return _renderer;
}

void Renderer_FrameBufferCreate(uint32_t width, uint32_t height, uint8_t bytepp) {
	uint8_t* data = (uint8_t*)malloc(width * height * bytepp);
	FrameBuffer FB = { .buffer = data,.width = width,.height = height,.bytepp = bytepp };
	_getRenderer()->frameBuffer = FB;
}

void Renderer_FrameBufferRelease() {
	if (_getRenderer()->frameBuffer.buffer == NULL)return NULL;
	free(_getRenderer()->frameBuffer.buffer);
}

uint8_t* Renderer_GetFrameBuffer() {
	return _getRenderer()->frameBuffer.buffer;
}

uint32_t Renderer_GetFrameWidth() {
	return _getRenderer()->frameBuffer.width;
}

uint32_t Renderer_GetFrameHeight() {
	return _getRenderer()->frameBuffer.height;
}

uint32_t Renderer_GetFrameBytepp() {
	return _getRenderer()->frameBuffer.bytepp;
}

void Renderer_Create() {
	//创建内存管理器128MB
	_getRenderer()->memM = Mem_Create(128 * 1024 * 1024);

	//创建texture数组
	_getRenderer()->textures = ArrayCreate(sizeof(Tex));

	printf("Renderer_Create\n");
	PrintArray(&_getRenderer()->textures);
	//创建obj数组
	_getRenderer()->objcects = ArrayCreate(sizeof(Geo));

	//背景色
	_getRenderer()->frameBuffer.backgroudColor = MakeColor4(0, 255, 0, 255);
}

void Renderer_Release(Renderer* render, uint8_t type) {
	if (render == NULL)return;
	switch (type)
	{
	case 0:
		//全部释放
		Mem_Release(&render->memM);
		break;
	case 1:
		//释放静态内存
		Mem_ClearStatic(&render->memM);
		break;
	case 2:
		//释放动态内存
		Mem_ClearDynamic(&render->memM);
	default:
		break;
	}
}

void Renderer_SubmitTexture(uint8_t* inPixels, uint32_t inWidth, uint32_t inHeight, uint8_t bytepp) {
	printf("inWidth:%d,inHeight:%d,bytepp:%d\n", inWidth, inHeight, bytepp);
	Tex* texture = (Tex*)Mem_AllocateStatic(&(_getRenderer()->memM), sizeof(Tex));
	if (texture == NULL)
	{
		printf("texture is NULL\n");
		return;
	}
	uint8_t* data = (uint8_t*)Mem_AllocateStatic(&(_getRenderer()->memM), sizeof(uint8_t) * inWidth * inHeight * bytepp);
	if (data == NULL)
	{
		printf("data is NULL\n");
		return;
	}
	PrintMemManager(&(_getRenderer()->memM));

	memcpy(data, inPixels, inWidth * inHeight * bytepp);

	printf("_getRenderer()->textures.length:%d\n", _getRenderer()->textures.length);

	texture->id = _getRenderer()->textures.length + 1;
	texture->pixels = data;
	texture->bpp = bytepp;
	texture->width = inWidth;
	texture->height = inHeight;

	ArrayPush(&_getRenderer()->textures, texture);
}

uint32_t Renderer_SubmitObject(float* inVertices, float* inUvs, uint32_t inNumOfVetices, uint32_t objID) {

	//几何体结构体数据
	Geo* obj = (Geo*)Mem_AllocateDynamic(&_getRenderer()->memM, sizeof(Geo));
	//顶点数据
	float* vertices = (float*)Mem_AllocateDynamic(&_getRenderer()->memM, sizeof(float) * inNumOfVetices * 2);
	memcpy(vertices, inVertices, sizeof(float) * inNumOfVetices * 2);
	//uv 数据
	float* uvs = (float*)Mem_AllocateDynamic(&_getRenderer()->memM, sizeof(float) * inNumOfVetices * 2);
	memcpy(uvs, inUvs, sizeof(float) * inNumOfVetices * 2);
	//为边界盒数据准备的空间
	float* bboxes = (float*)Mem_AllocateDynamic(&_getRenderer()->memM, sizeof(float) * inNumOfVetices / 3 * 2);
	//为裁切空间点数据准备的空间
	float* clipVertices = (float*)Mem_AllocateDynamic(&_getRenderer()->memM, sizeof(float) * inNumOfVetices * 2);

	obj->vertices = vertices;
	obj->uvs = uvs;
	obj->bboxes = bboxes;
	obj->clipVertices = clipVertices;
	obj->numOfVetices = inNumOfVetices;
	if (objID == 0)
	{
		obj->id = _getRenderer()->objcects.length + 1;
		//创建buffer
	}
	//修改buffer
	obj->id = objID;
	ArrayPush(&_getRenderer()->objcects, obj);
	return objID;
}

void Renderer_Tick(float delta) {
	Renderer_DrawBg();
	Renderer_Render();
}

void Renderer_Render() {
	//for (size_t i = 0; i < GetArrayElementCount(&_getRenderer()->objcects); i++)
	{
		Geo* geo = (Geo*)GetArrayElementByIndex(&_getRenderer()->objcects, 0);
		//缩放旋转结果矩阵
		Matrix srm = CreateStandardMatrix();
		//缩放旋转位移结果矩阵
		Matrix srtm = CreateStandardMatrix();
		//缩放
		Matrix ms = MakeScaMatrix(1, 1);
		//旋转
		Matrix mr = MakeRotMatrix(Deg2Rad(0));
		//位移
		Matrix mt = MakeTranslataMatrix(0, 0);
		//一起算
		Multi2Matrix(mr.m, ms.m, srm.m);
		Multi2Matrix(mt.m, srm.m, srtm.m);


		for (size_t v = 0; v < geo->numOfVetices / 3; v++)
		{
			Vect2 uv[3] = { 0 };
			Vect2 vp[3] = { 0 };
			uint32_t vi = v * 6;

			uv[0] = MakeVect2(geo->uvs[vi + 0], geo->uvs[vi + 1]);
			uv[1] = MakeVect2(geo->uvs[vi + 2], geo->uvs[vi + 3]);
			uv[2] = MakeVect2(geo->uvs[vi + 4], geo->uvs[vi + 5]);

			//顶点从模型空间转换成世界空间
			vp[0] = Vect2MultMatrix(MakeVect2(geo->vertices[vi + 0], geo->vertices[vi + 1]), srtm.m);
			vp[1] = Vect2MultMatrix(MakeVect2(geo->vertices[vi + 2], geo->vertices[vi + 3]), srtm.m);
			vp[2] = Vect2MultMatrix(MakeVect2(geo->vertices[vi + 4], geo->vertices[vi + 5]), srtm.m);

			Vect2 half = MakeVect2((float)Renderer_GetFrameWidth() / 2.f, (float)Renderer_GetFrameHeight() / 2.f);

			//将mesh的顶点转换到相机空间 *相机的逆矩阵
			Vect2 p0 = Vect2MultMatrix(vp[0], _getGameIns()->pCam->tm.m);
			Vect2 p1 = Vect2MultMatrix(vp[1], _getGameIns()->pCam->tm.m);
			Vect2 p2 = Vect2MultMatrix(vp[2], _getGameIns()->pCam->tm.m);

			//顶点转世界计算画幅空间位置 （需要考虑偏移值）
			Vect2 A = AddVect2(MakeVect2((p0.x / _getGameIns()->pCam->width) * (float)Renderer_GetFrameWidth(), (p0.y / _getGameIns()->pCam->height) * (float)Renderer_GetFrameHeight()), half);
			Vect2 B = AddVect2(MakeVect2((p1.x / _getGameIns()->pCam->width) * (float)Renderer_GetFrameWidth(), (p1.y / _getGameIns()->pCam->height) * (float)Renderer_GetFrameHeight()), half);
			Vect2 C = AddVect2(MakeVect2((p2.x / _getGameIns()->pCam->width) * (float)Renderer_GetFrameWidth(), (p2.y / _getGameIns()->pCam->height) * (float)Renderer_GetFrameHeight()), half);

			//遍历屏幕空间像素
			for (size_t y = 0; y < Renderer_GetFrameHeight(); y++)
			{
				for (size_t x = 0; x < Renderer_GetFrameWidth(); x++)
				{
					size_t index = y * Renderer_GetFrameWidth() * Renderer_GetFrameBytepp() + x * Renderer_GetFrameBytepp();
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
						Color4 colPick = Renderer_UVTextureSample(uv_u, uv_v, 3);

						//颜色混合 color*alpha + bg*(1-alpha)
						float colorAlpha = ((float)colPick.a / 255.f);
						_getRenderer()->frameBuffer.buffer[index + 0] = colPick.b * colorAlpha + _getRenderer()->frameBuffer.buffer[index + 0] * (1.f - colorAlpha);
						_getRenderer()->frameBuffer.buffer[index + 1] = colPick.g * colorAlpha + _getRenderer()->frameBuffer.buffer[index + 1] * (1.f - colorAlpha);
						_getRenderer()->frameBuffer.buffer[index + 2] = colPick.r * colorAlpha + _getRenderer()->frameBuffer.buffer[index + 2] * (1.f - colorAlpha);

					}
					else
					{
						_getRenderer()->frameBuffer.buffer[index + 0] = _getRenderer()->frameBuffer.buffer[index + 0] + _getRenderer()->frameBuffer.backgroudColor.b;
						_getRenderer()->frameBuffer.buffer[index + 1] = _getRenderer()->frameBuffer.buffer[index + 1] + _getRenderer()->frameBuffer.backgroudColor.g;
						_getRenderer()->frameBuffer.buffer[index + 2] = _getRenderer()->frameBuffer.buffer[index + 2] + _getRenderer()->frameBuffer.backgroudColor.r;
					}
				}
			}
		}
	}
}

void Renderer_DrawBg() {
	for (size_t y = 0; y < _getRenderer()->frameBuffer.height; y++)
	{
		for (size_t x = 0; x < _getRenderer()->frameBuffer.width; x++)
		{
			size_t index = y * _getRenderer()->frameBuffer.width * _getRenderer()->frameBuffer.bytepp + x * _getRenderer()->frameBuffer.bytepp;

			_getRenderer()->frameBuffer.buffer[index + 0] = _getRenderer()->frameBuffer.backgroudColor.b;
			_getRenderer()->frameBuffer.buffer[index + 1] = _getRenderer()->frameBuffer.backgroudColor.g;
			_getRenderer()->frameBuffer.buffer[index + 2] = _getRenderer()->frameBuffer.backgroudColor.r;
			_getRenderer()->frameBuffer.buffer[index + 3] = _getRenderer()->frameBuffer.backgroudColor.a;
		}
	}
}

Color4 Renderer_UVTextureSample(float u, float v, uint32_t tID) {
	Color4 out = MakeColor4(0, 0, 0, 0);
	Tex* texture = (Tex*)GetArrayElementByIndex(&_getRenderer()->textures, tID - 1);
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
	uint8_t r = texture->pixels[index + 2];
	uint8_t g = texture->pixels[index + 1];
	uint8_t b = texture->pixels[index + 0];

	uint8_t a = 255;
	a = bytepp > 3 ? texture->pixels[index + 3] : 255;

	out = MakeColor4(r, g, b, a);
	return out;
}