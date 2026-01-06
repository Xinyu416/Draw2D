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

Mesh* Renderer_GetCurrentMesh() {
	return _getRenderer()->currentMesh;
}

void Renderer_SetCurrentMesh(Mesh* m) {
	_getRenderer()->currentMesh = m;
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

	_getRenderer()->vertexIndexLock = (CRITICAL_SECTION*)malloc(sizeof(CRITICAL_SECTION));
	_getRenderer()->taskTriangleIndexLock = (CRITICAL_SECTION*)malloc(sizeof(CRITICAL_SECTION));
	_getRenderer()->taskFragmentIndexLock = (CRITICAL_SECTION*)malloc(sizeof(CRITICAL_SECTION));
	_getRenderer()->taskIndexLock = (CRITICAL_SECTION*)malloc(sizeof(CRITICAL_SECTION));
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
	free(_getRenderer()->taskIndexLock);
	ArrayRelease(&(_getRenderer()->objcects));

	free(_getRenderer());
}

void Renderer_SubmitTexture(uint8_t* inPixels, uint32_t inWidth, uint32_t inHeight, uint8_t bpp) {
	Tex* texture = (Tex*)Mem_AllocateStatic(&(_getRenderer()->memM), sizeof(Tex));
	if (texture == NULL)return;
	uint8_t* data = (uint8_t*)Mem_AllocateStatic(&(_getRenderer()->memM), sizeof(uint8_t) * inWidth * inHeight * bpp / 8);
	if (data == NULL)return;
	memcpy(data, inPixels, inWidth * inHeight * bpp / 8);

	//texture->id = _getRenderer()->textures.length + 1;
	texture->pixels = data;
	texture->bpp = bpp;
	texture->width = inWidth;
	texture->height = inHeight;

	//ArrayPush(&_getRenderer()->textures, texture);
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
	//ArrayPush(&_getRenderer()->RenderMeshs, pRmesh);
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
			Vect2 vp[3] = { 0 };
			Vect2 uv[3] = { 0 };
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
	uint8_t randCR = rand() % 255;
	uint8_t randCG = rand() % 255;
	uint8_t randCB = rand() % 255;
	for (size_t y = 0; y < _getRenderer()->frameBuffer.height; y++)
	{
		for (size_t x = 0; x < _getRenderer()->frameBuffer.width; x++)
		{
			size_t index = y * _getRenderer()->frameBuffer.width * _getRenderer()->frameBuffer.bytepp + x * _getRenderer()->frameBuffer.bytepp;

			//_getRenderer()->frameBuffer.buffer[index + 0] = _getRenderer()->frameBuffer.backgroudColor.b;
			//_getRenderer()->frameBuffer.buffer[index + 1] = _getRenderer()->frameBuffer.backgroudColor.g;
			//_getRenderer()->frameBuffer.buffer[index + 2] = _getRenderer()->frameBuffer.backgroudColor.r;
			//_getRenderer()->frameBuffer.buffer[index + 3] = _getRenderer()->frameBuffer.backgroudColor.a;

			//随机色
			_getRenderer()->frameBuffer.buffer[index + 0] = randCB;
			_getRenderer()->frameBuffer.buffer[index + 1] = randCG;
			_getRenderer()->frameBuffer.buffer[index + 2] = randCR;
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
	/*for (size_t i = 0; i < &(_getRenderer()->textures).length; i++)
	{
		Tex* texture = (Tex*)GetArrayElementByIndex(&(_getRenderer()->textures), i);
		if (texture->id = tID)
		{
			return texture;
		}
	}*/
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
	uint32_t nop = GetNumberOfProcessors() - 2;
	printf("Renderer_ThreadMain::GetNumberOfProcessors:%d\n", nop);
	const uint32_t MAX_THREADS = 20;
	ShaderThread* threadArray = (ShaderThread*)malloc(sizeof(ShaderThread) * MAX_THREADS);
	//渲染线程
	ShaderThread* shaderThread = NULL;
	for (int i = 0; i < MAX_THREADS; i++)
	{
		shaderThread = threadArray + i;
		shaderThread->handle = CreateThread(NULL, 0, Shader_ThreadMain, shaderThread, 0, &shaderThread->id);
	}

	/*初始化锁*/
	InitializeCriticalSection(r->taskFragmentIndexLock);
	InitializeCriticalSection(r->taskTriangleIndexLock);
	InitializeCriticalSection(r->vertexIndexLock);
	InitializeCriticalSection(r->taskIndexLock);

	Sleep(10);
	//渲染循环
	r->isRunning = true;
	while (r->isRunning)
	{
		uint32_t numOfMeshes = GetArrayElementCount(&(_getGameIns()->meshs));
		printf("Renderer_ThreadMain::numOfMeshes:%d\n", numOfMeshes);
		for (size_t i = 0; i < numOfMeshes; i++)
		{
			Mesh* pmesh = (Mesh*)GetArrayElementByIndex(&(_getGameIns()->meshs), i);
			Renderer_SetCurrentMesh(pmesh);

			//渲染阶段为顶点变换
			r->renderStage = RENDERSTAGE_VERTEXTRAS;
			r->taskIndex = 0;
			r->taskTotalCount = 100;
			while (true)
			{
				if (r->renderStage == RENDERSTAGE_VERTEXTRAS)
				{
					if (r->taskIndex >= r->taskTotalCount)
					{
						//任务做完 切换状态
						r->renderStage = RENDERSTAGE_NONE;
						break;
					}
					printf("[%d]%d/%d\n", i, r->taskIndex, r->taskTotalCount);
				}
				Sleep(1);
			}



			////当前阶段为片元裁切
			r->renderStage = RENDERSTAGE_FRAGMENTCLIP;
			r->taskIndex = 0;
			r->taskTotalCount = 100;
			printf("Stage 2\n");
			while (true)
			{
				if (r->renderStage == RENDERSTAGE_FRAGMENTCLIP)
				{
					if (r->taskIndex >= r->taskTotalCount)
					{
						//任务做完 切换状态
						r->renderStage = RENDERSTAGE_NONE;
						break;
					}
					printf("[%d]%d/%d\n", i, r->taskIndex, r->taskTotalCount);
				}
				Sleep(3);
			}
			//当前阶段为片元着色
			r->renderStage = RENDERSTAGE_FRAGMENTSHADING;
			r->taskIndex = 0;
			r->taskTotalCount = 100;//三角面数量
			printf("Stage 3\n");
			while (true)
			{
				if (r->renderStage == RENDERSTAGE_FRAGMENTSHADING)
				{
					//逐三顶点处理片元像素
					if (r->taskIndex >= r->taskTotalCount)
					{
						//任务做完 切换状态
						r->renderStage = RENDERSTAGE_NONE;
						break;
					}
					printf("[%d]%d/%d\n", i, r->taskIndex, r->taskTotalCount);
				}
				Sleep(3);
			}
		}
		Sleep(1);
		//完成渲染任务
		sendToMain.type = MESSAGE_RENDEROVER;
		MsgAssistant_SendMsgToMain(_getGameEngine()->msgAssist, sendToMain, true);
		printf("Renderer Tick End\n");
	}


	//关闭线程handle
	for (size_t i = 0; i < MAX_THREADS; i++)
	{
		shaderThread = threadArray + i;
		if (shaderThread != NULL)
		{
			CloseHandle(thread->handle);
		}
	}
	free(threadArray);

	//释放
	Renderer_Release();
	//发送关闭消息到主线程
	Message sendToMainClose = { .type = MESSAGE_CLOSE,.data[0] = 1 };
	printf("Renderer::will Close\n");
	MsgAssistant_SendMsgToMain(_getGameEngine()->msgAssist, sendToMainClose, false);

}


//shader请求任务的小函数
//结果缓冲区边界判断
//顶点任务完成判断
uint32_t Renderer_GetTaskIndex() {
	Renderer* r = _getRenderer();
	uint32_t taskIndex = -1;
	if (r->renderStage == RENDERSTAGE_NONE)
	{
		//无事可做
		taskIndex = -1;
	}
	else if (r->renderStage == RENDERSTAGE_VERTEXTRAS)
	{
		//获取任务index
		EnterCriticalSection(r->taskIndexLock);
		taskIndex = r->taskIndex;
		(r->taskIndex)++;
		LeaveCriticalSection(r->taskIndexLock);
	}
	else if (r->renderStage == RENDERSTAGE_FRAGMENTCLIP) {
		//获取任务index
		EnterCriticalSection(r->taskIndexLock);
		taskIndex = r->taskIndex;
		(r->taskIndex)++;
		LeaveCriticalSection(r->taskIndexLock);
	}
	else if (r->renderStage == RENDERSTAGE_FRAGMENTSHADING) {
		//获取任务index
		EnterCriticalSection(r->taskIndexLock);
		taskIndex = r->taskIndex;
		(r->taskIndex)++;
		LeaveCriticalSection(r->taskIndexLock);
	}
	return taskIndex;
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