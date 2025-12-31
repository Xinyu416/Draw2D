#include "Renderer.h"
#include "Shader.h"

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

typedef struct {
	uint32_t id; //4byte
	Vect2 pos; //8byte
	Matrix tm; //36byte
	float rot; //4byte
	Vect2 scale; //8byte
	Material mat;//5byte
	uint8_t block[3];
}RMesh;

static Renderer* _renderer = NULL;

Renderer* _getRenderer() {
	if (_renderer == NULL)
	{
		_renderer = (Renderer*)calloc(1, sizeof(Renderer));
	}
	return _renderer;
}

void Renderer_Stop() {
	_getRenderer()->isRunning = false;
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

void Renderer_Initialize(uint32_t width, uint32_t height, uint8_t bytepp) {

	uint8_t* data = (uint8_t*)malloc(width * height * bytepp);
	FrameBuffer FB = { .buffer = data,.width = width,.height = height,.bytepp = bytepp,.backgroudColor = MakeColor4(0.f,0.f,0.f,0.f) };
	_getRenderer()->frameBuffer = FB;

	_getRenderer()->taskFragmentIndex = (uint32_t*)malloc(sizeof(uint32_t));

	_getRenderer()->taskTriangleIndex = (uint32_t*)malloc(sizeof(uint32_t));

	_getRenderer()->vertexIndex = (uint32_t*)malloc(sizeof(uint32_t));

	_getRenderer()->verticesInClip = (float*)malloc(sizeof(float) * 2 * 3 * 2 * 30 * 30);//三角面*2*宽*高 测试数据（临时用）
	_getRenderer()->triangleBBox = (float*)malloc(sizeof(float) * 2 * 2 * 2 * 30 * 30);

	_getRenderer()->vertexIndexLock = (CRITICAL_SECTION*)malloc(sizeof(CRITICAL_SECTION));
	_getRenderer()->taskTriangleIndexLock = (CRITICAL_SECTION*)malloc(sizeof(CRITICAL_SECTION));
	_getRenderer()->taskFragmentIndexLock = (CRITICAL_SECTION*)malloc(sizeof(CRITICAL_SECTION));


	////创建内存管理器128MB
	//_getRenderer()->memM = Mem_Create(128 * 1024 * 1024);

	////创建texture数组
	//_getRenderer()->textures = ArrayCreate(sizeof(Tex));

	////创建obj数组
	//_getRenderer()->objcects = ArrayCreate(sizeof(Geo));

}

void Renderer_ReleaseMEM(uint8_t type) {
	if (_getRenderer() == NULL)return;
	switch (type)
	{
	case 0:
		//全部释放
		Mem_Release(&(_getRenderer()->memM));
		break;
	case 1:
		//释放静态内存
		Mem_ClearStatic(&(_getRenderer()->memM));
		break;
	case 2:
		//释放动态内存
		Mem_ClearDynamic(&(_getRenderer()->memM));
	default:
		break;
	}
}

void Renderer_Release() {
	if (_getRenderer() == NULL)return;
	Renderer_ReleaseMEM(0);
	free(_getRenderer()->frameBuffer.buffer);
	ArrayRelease(&(_getRenderer()->objcects));
	ArrayRelease(&(_getRenderer()->textures));
	ArrayRelease(&(_getRenderer()->RenderMeshs));
	free(_getRenderer());
}

void Renderer_SubmitTexture(uint8_t* inPixels, uint32_t inWidth, uint32_t inHeight, uint8_t bpp) {
	Tex* texture = (Tex*)Mem_AllocateStatic(&(_getRenderer()->memM), sizeof(Tex));
	if (texture == NULL)return;
	uint8_t* data = (uint8_t*)Mem_AllocateStatic(&(_getRenderer()->memM), sizeof(uint8_t) * inWidth * inHeight * bpp / 8);
	if (data == NULL)return;
	memcpy(data, inPixels, inWidth * inHeight * bpp / 8);

	texture->id = _getRenderer()->textures.length + 1;
	texture->pixels = data;
	texture->bpp = bpp;
	texture->width = inWidth;
	texture->height = inHeight;

	ArrayPush(&_getRenderer()->textures, texture);
}

uint32_t Renderer_SubmitObject(float* inVertices, float* inUvs, uint32_t inNumOfVetices, uint32_t inObjID) {

	uint32_t objID = inObjID;
	//顶点数据
	float* vertices = (float*)Mem_AllocateStatic(&_getRenderer()->memM, sizeof(float) * inNumOfVetices * 2);
	if (vertices == NULL)return NULL;
	memcpy(vertices, inVertices, sizeof(float) * inNumOfVetices * 2);

	//uv 数据
	float* uvs = (float*)Mem_AllocateStatic(&_getRenderer()->memM, sizeof(float) * inNumOfVetices * 2);
	if (uvs == NULL)return NULL;
	memcpy(uvs, inUvs, sizeof(float) * inNumOfVetices * 2);

	//为边界盒数据准备的空间
	float* bboxes = (float*)Mem_AllocateStatic(&_getRenderer()->memM, sizeof(float) * inNumOfVetices / 3 * 2);
	if (bboxes == NULL) return NULL;

	//为裁切空间点数据准备的空间
	float* clipVertices = (float*)Mem_AllocateStatic(&_getRenderer()->memM, sizeof(float) * inNumOfVetices * 2);
	if (clipVertices == NULL) return NULL;

	//几何体结构体数据
	Geo* obj = (Geo*)Mem_AllocateStatic(&_getRenderer()->memM, sizeof(Geo));
	obj->vertices = vertices;
	obj->uvs = uvs;
	obj->bboxes = bboxes;
	obj->clipVertices = clipVertices;
	obj->numOfVetices = inNumOfVetices;
	if (inObjID == 0)
	{
		objID = _getRenderer()->objcects.length + 1;
	}
	obj->id = objID;
	ArrayPush(&_getRenderer()->objcects, obj);
	return objID;
}

void Renderer_SubmitCamera(Camera cam) {
	Camera* pCam = (Camera*)Mem_AllocateDynamic(&_getRenderer()->memM, sizeof(Camera));
	if (pCam == NULL)return;
	memcpy(pCam, &cam, sizeof(Camera));
}

void Renderer_SubmitMesh(Vect2 pos, Matrix tm, float rot, Vect2 scale, Material mat, uint32_t meshID) {
	RMesh* pRmesh = (RMesh*)Mem_AllocateDynamic(&_getRenderer()->memM, sizeof(RMesh));
	if (pRmesh == NULL)return;
	pRmesh->pos = pos;
	pRmesh->tm = tm;
	pRmesh->rot = rot;
	pRmesh->scale = scale;
	pRmesh->mat = mat;
	pRmesh->id = meshID;
	ArrayPush(&_getRenderer()->RenderMeshs, pRmesh);
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

		//以三角面为单位（三个点一组）
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

			//计算boundingBox大小 
			float x_min = fminf(fminf(A.x, B.x), C.x);
			float x_max = fmaxf(fmaxf(A.x, B.x), C.x);
			float y_min = fminf(fminf(A.y, B.y), C.y);
			float y_max = fmaxf(fmaxf(A.y, B.y), C.y);

			//左下到右上 两个点
			//Vect2 leftDown = MakeVect2(x_min, y_min);
			//Vect2 rightTop = MakeVect2(x_max, y_max);


			//遍历屏幕空间像素
			for (size_t y = 0; y < Renderer_GetFrameHeight(); y++)
			{
				for (size_t x = 0; x < Renderer_GetFrameWidth(); x++)
				{
					//像素在boundingBox内才计算 否则跳过
					if (!(x >= x_min && x <= x_max && y >= y_min && y <= y_max))continue;

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
						Color4 colPick = Renderer_UVTextureSample(uv_u, uv_v, 1);

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

void Renderer_Clear() {
	//清空buffer （画背景）
	Renderer_DrawBg();
	//释放动态内存
	Renderer_ReleaseMEM(2);
}

Tex* Renderer_GetTextureByID(uint32_t tID) {
	for (size_t i = 0; i < &(_getRenderer()->textures).length; i++)
	{
		Tex* texture = (Tex*)GetArrayElementByIndex(&(_getRenderer()->textures), i);
		if (texture->id = tID)
		{
			return texture;
		}
	}
	return NULL;
}

Color4 Renderer_UVTextureSample(float u, float v, uint32_t tID) {
	Color4 out = MakeColor4(0, 0, 0, 0);

	Tex* texture = Renderer_GetTextureByID(tID);
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

void Renderer_ThreadMain(RendererThread* thread) {

	DWORD threadId = thread->id;
	Message fromMain = MsgAssistant_GetMsgToThread(_getGameEngine()->msgAssist, true);
	uint32_t width = fromMain.data[1];
	uint32_t height = fromMain.data[2];
	uint32_t bytepp = fromMain.data[3];

	Renderer_Initialize(width, height, bytepp);

	//向主线程发送的消息
	Message sendToMain = { .type = MESSAGE_START,.data[0] = 1 };
	MsgAssistant_SendMsgToMain(_getGameEngine()->msgAssist, sendToMain, false);
	Renderer* r = _getRenderer();

	/*创建Shader线程*/
	//ShaderThread* shaderThread = (ShaderThread*)malloc(sizeof(ShaderThread));
	//shaderThread->handle = CreateThread(NULL, 0, Shader_ThreadMain, shaderThread, 0, &shaderThread->id);

	//最大线程数
	const uint32_t MAX_THREADS = 1;
	ShaderThread* threadArray = (ShaderThread*)malloc(sizeof(ShaderThread) * MAX_THREADS);
	//渲染线程
	ShaderThread* shaderThread = NULL;
	for (int i = 0; i < MAX_THREADS; i++)
	{
		shaderThread = threadArray + i;
		shaderThread->handle = CreateThread(NULL, 0, Shader_ThreadMain, shaderThread, 0, &shaderThread->id);

	}

	InitializeCriticalSection(r->taskFragmentIndexLock);
	InitializeCriticalSection(r->taskTriangleIndexLock);
	InitializeCriticalSection(r->vertexIndexLock);

	r->isRunning = true;
	while (r->isRunning)
	{
		printf("Renderer Tick Start\n");

		//模型空间矩阵拷贝进缓存空间
		if (_getGameIns()->cMesh)
		{
			/*计算模型空间矩阵*/
			Geometry* geo = &(_getGameIns()->cMesh->geo);
			Matrix srm = CreateStandardMatrix();
			Matrix srtm = CreateStandardMatrix();
			Matrix ms = MakeScaMatrix(1, 1);
			Matrix mr = MakeRotMatrix(Deg2Rad(0));
			Matrix mt = MakeTranslataMatrix(0, 0);
			Multi2Matrix(mr.m, ms.m, srm.m);
			Multi2Matrix(mt.m, srm.m, srtm.m);
			_getGameIns()->cMesh->tmRenderCopy = srtm;
			//渲染阶段为顶点变换
			r->renderStage = 1;
		}

		Sleep(10);

		//完成渲染任务
		sendToMain.type = MESSAGE_RENDEROVER;
		MsgAssistant_SendMsgToMain(_getGameEngine()->msgAssist, sendToMain, true);
		printf("Renderer Tick End\n");
	}

	//释放
	Renderer_Release();
	//发送关闭消息到主线程
	Message sendToMainClose = { .type = MESSAGE_CLOSE,.data[0] = 1 };
	printf("Renderer::will Close\n");
	MsgAssistant_SendMsgToMain(_getGameEngine()->msgAssist, sendToMainClose, false);

}

void Renderer_TaskMain() {

	RendererThread* thread = (RendererThread*)malloc(sizeof(RendererThread));
	thread->handle = CreateThread(NULL, 0, Renderer_ThreadMain, thread, 0, &thread->id);

	uint32_t* resultCount = (uint32_t*)malloc(sizeof(uint32_t));
	uint32_t taskCount = 0;
	//Queue RMeshs = CreateQueue(_getRenderer()->RenderMeshs.length, sizeof(RMesh));
	//for (size_t i = 0; i < _getRenderer()->RenderMeshs.length; i++)
	//{
	//	EnQueue(&RMeshs, GetArrayElementByIndex(&_getRenderer()->RenderMeshs, i));
	//}
	//RMesh* rmesh = (RMesh*)DeQueue(&RMeshs);
	//for (size_t i = 0; i < _getRenderer()->objcects.length; i++)
	//{
	//	Geo* geo = (Geo*)GetArrayElementByIndex(&_getRenderer()->objcects, i);
	//	if (geo->id == rmesh->id)
	//	{
	//		//取顶点数据
	//		taskCount = geo->numOfVetices;
	//	}
	//}

	bool isRunning = true;
	while (isRunning)
	{
		uint8_t fromThreadMsg = _getGameEngine()->msgAssist->fromThreadMessage.type;
		if (fromThreadMsg != MESSAGE_NONE)
		{
			if (fromThreadMsg == MESSAGE_START)
			{
				_getGameEngine()->msgAssist->toThreadMessage.type = MESSAGE_START;
			}
			if (fromThreadMsg == MESSAGE_TYPE2)
			{
				_getGameEngine()->msgAssist->toThreadMessage.type = MESSAGE_TYPE2;

			}
			if (fromThreadMsg == MESSAGE_CLOSE)
			{
				_getGameEngine()->msgAssist->toThreadMessage.type = MESSAGE_CLOSE;

			}
			_getGameEngine()->msgAssist->fromThreadMessage.type = MESSAGE_NONE;
		}

		//if (*resultCount == taskCount)
		//{
		//	RMesh* rmesh = (RMesh*)DeQueue(&RMeshs);
		//	for (size_t i = 0; i < _getRenderer()->objcects.length; i++)
		//	{
		//		Geo* geo = (Geo*)GetArrayElementByIndex(&_getRenderer()->objcects, i);
		//		if (geo->id == rmesh->id)
		//		{
		//			//取顶点数据
		//			taskCount = geo->numOfVetices;
		//			*resultCount = 0;
		//			thread->toThreadMessage.type = 11;
		//		}
		//	}
		//}

		//条件触发
		//isRunning = false;
		Sleep(10);
	}

	CloseHandle(thread->handle);

}











/*创建消息助手*/
MessageAssistant* CreateMessageAssistant() {
	MessageAssistant* assistant = (MessageAssistant*)malloc(sizeof(MessageAssistant));
	assistant->pFromLock = (CRITICAL_SECTION*)malloc(sizeof(CRITICAL_SECTION));
	assistant->pToLock = (CRITICAL_SECTION*)malloc(sizeof(CRITICAL_SECTION));
	assistant->fromThreadMessage.type = MESSAGE_NONE;
	assistant->toThreadMessage.type = MESSAGE_NONE;
	/*初始化锁*/
	InitializeCriticalSection(assistant->pToLock);
	InitializeCriticalSection(assistant->pFromLock);

	return assistant;
}

/*释放消息助手*/
void ReleaseMessageAssistant(MessageAssistant* assistant) {
	if (assistant == NULL)return;
	if (assistant->pFromLock != NULL) {
		DeleteCriticalSection(assistant->pFromLock);
		free(assistant->pFromLock);
	}
	if (assistant->pToLock != NULL) {
		DeleteCriticalSection(assistant->pToLock);
		free(assistant->pToLock);
	}
	free(assistant);
}

/*给子线程发消息*/
Message MsgAssistant_SendMsgToThread(MessageAssistant* assistant, Message msg, const bool isblock) {

	EnterCriticalSection(assistant->pToLock);
	assistant->toThreadMessage = msg;
	LeaveCriticalSection(assistant->pToLock);

	Message ret = { .type = MESSAGE_NONE,.data = {0} };

	while (isblock)
	{
		if (assistant->fromThreadMessage.type == msg.type)
		{
			EnterCriticalSection(assistant->pFromLock);
			ret = msg;
			assistant->fromThreadMessage.type = MESSAGE_NONE;
			LeaveCriticalSection(assistant->pFromLock);
			return ret;
		}
		Sleep(1);
	}
	return ret;
}

/*给主线程发消息*/
Message MsgAssistant_SendMsgToMain(MessageAssistant* assistant, Message msg, const bool isblock) {
	EnterCriticalSection(assistant->pFromLock);
	assistant->fromThreadMessage = msg;
	LeaveCriticalSection(assistant->pFromLock);

	Message ret = { .type = MESSAGE_NONE,.data = {0} };

	while (isblock)
	{
		if (assistant->toThreadMessage.type == msg.type)
		{
			EnterCriticalSection(assistant->pToLock);
			ret = msg;
			assistant->toThreadMessage.type = MESSAGE_NONE;
			LeaveCriticalSection(assistant->pToLock);
			return ret;
		}
		Sleep(1);
	}
	return ret;
}

/*从子线程获取消息*/
Message MsgAssistant_GetMsgFromThread(MessageAssistant* assistant, const bool isblock) {
	Message ret = { .type = MESSAGE_NONE,.data = {0} };

	while (true) {
		if (assistant->fromThreadMessage.type != MESSAGE_NONE)
		{
			EnterCriticalSection(assistant->pFromLock);
			ret = assistant->fromThreadMessage;
			assistant->fromThreadMessage.type = MESSAGE_NONE;
			LeaveCriticalSection(assistant->pFromLock);
			return ret;
		}
		if (!isblock)break;
		Sleep(1);
	}
	return ret;
}

/*从主线程获取消息*/
Message MsgAssistant_GetMsgToThread(MessageAssistant* assistant, const bool isblock) {
	Message ret = { .type = MESSAGE_NONE,.data = {0} };
	while (true) {
		if (assistant->toThreadMessage.type != MESSAGE_NONE)
		{
			EnterCriticalSection(assistant->pToLock);
			ret = assistant->toThreadMessage;
			assistant->toThreadMessage.type = MESSAGE_NONE;
			LeaveCriticalSection(assistant->pToLock);
			return ret;
		}
		if (!isblock)break;
		Sleep(1);
	}
	return ret;
}