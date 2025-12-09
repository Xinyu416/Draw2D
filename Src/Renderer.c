#include "Renderer.h"

typedef struct {
	float* vertices;		//2*3  顶点数组			8byte(64位操作系统指针是8字节)
	float* uvs;				//2*3  顶点uv数组		8byte	
	float* bboxes;			//1*2  边界盒			8byte
	float* clipVertices;	//2*3  裁切空间点信息		8byte
	uint32_t numOfVetices;  //*3   顶点数			4byte
	uint32_t id;			//	   顶点id			4byte
}Geometry;

typedef struct {
	uint8_t* pixels;
	uint32_t width;
	uint32_t height;
	uint32_t id;
	uint8_t bpp;
	uint8_t block[3];
}Texture;

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
	Renderer* renderer = _getRenderer();
	//创建texture数组
	renderer->textures = ArrayCreate(sizeof(Texture));
	//创建obj数组
	renderer->objcects = ArrayCreate(sizeof(Geometry));

	//创建内存管理器128MB
	renderer->memM = Mem_Create(128 * 1024 * 1024);
	//背景色
	renderer->frameBuffer.backgroudColor = MakeColor4(255, 255, 0, 255);
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
	Texture* texture = (Texture*)Mem_AllocateStatic(&_getRenderer()->memM, sizeof(Texture));

	uint8_t* data = (uint8_t*)Mem_AllocateStatic(&_getRenderer()->memM, sizeof(uint8_t) * inWidth * inHeight * bytepp);
	PrintMemManager(&_getRenderer()->memM);
	memcpy(data, inPixels, inWidth * inHeight * bytepp);

	texture->id = _getRenderer()->textures.length + 1;
	texture->bpp = bytepp;
	texture->pixels = data;
	texture->width = inWidth;
	texture->height = inHeight;

	ArrayPush(&_getRenderer()->textures, texture);
}

void Renderer_SubmitObject(float* inVertices, float* inUvs, uint32_t inNumOfVetices) {
	//几何体结构体数据
	Geometry* obj = (Geometry*)Mem_AllocateDynamic(&_getRenderer()->memM, sizeof(Geometry));
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
	obj->id = _getRenderer()->objcects.length + 1;

	ArrayPush(&_getRenderer()->objcects, obj);
}

void Renderer_Tick(float delta) {
	Renderer_DrawBg();
}

void Renderer_Render() {

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