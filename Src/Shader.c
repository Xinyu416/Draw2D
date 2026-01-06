#include "Shader.h"
#include "GameEngine.h"
#include "GameInstance.h"
#include "Renderer.h"
#include "GameInstance.h"

uint8_t GetNumberOfProcessors() {
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	uint8_t proNum = (uint8_t)sysInfo.dwNumberOfProcessors;
	return proNum;
}

/*顶点变换阶段*/
void VertexTranfrom(uint32_t inTaskIndex) {
	/*顶点变换阶段*/
	Mesh* pmesh = Renderer_GetCurrentMesh();
	//模型的顶点信息
	float* vertex = pmesh->geo.vertices;
	Matrix cameraTM = pmesh->cameraTMForRender;

	//buffer中心
	//Vect2 half = MakeVect2((float)Renderer_GetFrameWidth() / 2.f, (float)Renderer_GetFrameHeight() / 2.f);
	//将mesh的顶点转换到相机空间 *相机的逆矩阵
	Vect2 p0 = Vect2MultMatrix(MakeVect2(vertex[inTaskIndex + 0], vertex[inTaskIndex + 1]), cameraTM.m);
	//顶点转世界计算裁切空间位置 （需要考虑偏移值）
	Vect2 clipP = MakeVect2((p0.x / _getGameIns()->pCam->width), (p0.y / _getGameIns()->pCam->height));
	//裁切空间点信息存入缓存空间
	pmesh->geo.verticesInClipForRender[inTaskIndex + 0] = clipP.x;
	pmesh->geo.verticesInClipForRender[inTaskIndex + 1] = clipP.y;
}

/*片元裁切阶段*/
void FragmentClip(uint32_t inTaskIndex) {
	//以三角面为单位（三个点一组）
	//taskIndex为三角面index
	uint32_t ti = inTaskIndex * 6;

	Mesh* pmesh = Renderer_GetCurrentMesh();
	//屏幕空间点信息
	Vect2 A = MakeVect2(pmesh->geo.verticesInClipForRender[ti + 0], pmesh->geo.verticesInClipForRender[ti + 1]);
	Vect2 B = MakeVect2(pmesh->geo.verticesInClipForRender[ti + 2], pmesh->geo.verticesInClipForRender[ti + 3]);
	Vect2 C = MakeVect2(pmesh->geo.verticesInClipForRender[ti + 4], pmesh->geo.verticesInClipForRender[ti + 5]);
	//计算boundingBox大小 
	float x_min = fminf(fminf(A.x, B.x), C.x);
	float y_min = fminf(fminf(A.y, B.y), C.y);
	float x_max = fmaxf(fmaxf(A.x, B.x), C.x);
	float y_max = fmaxf(fmaxf(A.y, B.y), C.y);

	//printf("FragmentClip::inTaskIndex:%d,xmin:%f,xmax:%f,ymin:%f,ymax:%f\n", inTaskIndex, x_min, x_max, y_min, y_max);
	//写入BBox值
	EnterCriticalSection(_getRenderer()->taskTriangleIndexLock);
	*(pmesh->geo.triangleBBox + inTaskIndex * 4 + 0) = x_min;
	*(pmesh->geo.triangleBBox + inTaskIndex * 4 + 1) = y_min;
	*(pmesh->geo.triangleBBox + inTaskIndex * 4 + 2) = x_max;
	*(pmesh->geo.triangleBBox + inTaskIndex * 4 + 3) = y_max;
	LeaveCriticalSection(_getRenderer()->taskTriangleIndexLock);
}

/*片元着色阶段*/
void FragmentShading(uint32_t inTaskIndex) {
	//以三角面为单位（三个点一组）
	Mesh* pmesh = Renderer_GetCurrentMesh();
	Geometry geo = pmesh->geo;
	//taskIndex为三角面index
	uint32_t ti = inTaskIndex * 6;
	Vect2 half = MakeVect2((float)Renderer_GetFrameWidth() / 2.f, (float)Renderer_GetFrameHeight() / 2.f);
	//三角面内片元（像素数）为单位
	//三个顶点值取出BBox
	//屏幕空间点信息
	Vect2 A = MakeVect2(pmesh->geo.verticesInClipForRender[ti + 0], pmesh->geo.verticesInClipForRender[ti + 1]);
	Vect2 B = MakeVect2(pmesh->geo.verticesInClipForRender[ti + 2], pmesh->geo.verticesInClipForRender[ti + 3]);
	Vect2 C = MakeVect2(pmesh->geo.verticesInClipForRender[ti + 4], pmesh->geo.verticesInClipForRender[ti + 5]);

	uint32_t x_min = (uint32_t)(pmesh->geo.triangleBBox[inTaskIndex * 4 + 0] * Renderer_GetFrameWidth() + half.x);
	uint32_t y_min = (uint32_t)(pmesh->geo.triangleBBox[inTaskIndex * 4 + 1] * Renderer_GetFrameHeight() + half.y);
	uint32_t x_max = (uint32_t)(pmesh->geo.triangleBBox[inTaskIndex * 4 + 2] * Renderer_GetFrameWidth() + half.x);
	uint32_t y_max = (uint32_t)(pmesh->geo.triangleBBox[inTaskIndex * 4 + 3] * Renderer_GetFrameHeight() + half.y);


	Vect2 uv[3] = { 0 };
	uv[0] = MakeVect2(geo.uvs[ti + 0], geo.uvs[ti + 1]);
	uv[1] = MakeVect2(geo.uvs[ti + 2], geo.uvs[ti + 3]);
	uv[2] = MakeVect2(geo.uvs[ti + 4], geo.uvs[ti + 5]);

	uint32_t width = x_max - x_min;
	uint32_t height = y_max - y_min;
	//printf("FragmentShading()::inTaskIndex:%d,xmin:%d,xmax:%d,ymin:%d,ymax:%d,width:%d,height:%d\n",
	//	inTaskIndex, x_min, x_max, y_min, y_max, width, height);

	//return;
	for (size_t y = y_min; y < y_max; y++)
	{
		for (size_t x = x_min; x < x_max; x++)
		{
			//像素在boundingBox内才计算 否则跳过
			size_t index = y * Renderer_GetFrameWidth() * Renderer_GetFrameBytepp() + x * Renderer_GetFrameBytepp();
			//printf("index:%d\n", index);
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
				//_getRenderer()->frameBuffer.buffer[index + 0] = _getRenderer()->frameBuffer.buffer[index + 0] + _getRenderer()->frameBuffer.backgroudColor.b;
				//_getRenderer()->frameBuffer.buffer[index + 1] = _getRenderer()->frameBuffer.buffer[index + 1] + _getRenderer()->frameBuffer.backgroudColor.g;
				//_getRenderer()->frameBuffer.buffer[index + 2] = _getRenderer()->frameBuffer.buffer[index + 2] + _getRenderer()->frameBuffer.backgroudColor.r;

				_getRenderer()->frameBuffer.buffer[index + 0] = 255;
				_getRenderer()->frameBuffer.buffer[index + 1] = 255;
				_getRenderer()->frameBuffer.buffer[index + 2] = 255;
			}
		}
	}
}

DWORD CALLBACK Shader_ThreadMain(ShaderThread* thread) {
	DWORD threadId = thread->id;
	printf("[Shader_ThreadMain]::id = %d\n", thread->id);
	Renderer* r = _getRenderer();

	while (true) {
		//获取任务index
		uint32_t getTaskIndex = Renderer_GetTaskIndex();

		//取不到任务 空跑 什么也不做
		if (getTaskIndex == -1)continue;
		//任务领完了，防止主线程监听不到
		if (getTaskIndex >= r->taskTotalCount)continue;
		switch (r->renderStage) {
		case RENDERSTAGE_VERTEXTRAS:
			//做任务
			VertexTranfrom(getTaskIndex);
			//Sleep(1);
			break;
		case RENDERSTAGE_FRAGMENTCLIP:
			///*片元裁切阶段*/
			FragmentClip(getTaskIndex);
			//做任务
			Sleep(1);
			break;
		case RENDERSTAGE_FRAGMENTSHADING:
			/*片元着色阶段*/

			FragmentShading(getTaskIndex);
			//做任务
			Sleep(1);

			break;
		case RENDERSTAGE_SHADECOMPLETE:

			break;
		}
		Sleep(10);
	}
}



























DWORD CALLBACK __Shader_ThreadMain(ShaderThread* thread) {
	DWORD threadId = thread->id;
	printf("[Shader_ThreadMain]::id = %d\n", thread->id);
	Renderer* r = _getRenderer();
	/*需要生命周期同步*/
	_getRenderer()->taskIndex = 0;
	while (true)
	{
		switch (r->renderStage)
		{
		case 1:

			if (_getGameIns()->cMesh == NULL)continue;
			//顶点变换
			//Mesh 顶点数
			uint32_t numOfVertices = _getGameIns()->cMesh->geo.numOfVertex;

			//模型空间-世界空间-相机空间-裁切空间-屏幕空间
			EnterCriticalSection(_getRenderer()->vertexIndexLock);
			uint32_t currentVertexIndex = _getRenderer()->taskIndex;
			LeaveCriticalSection(_getRenderer()->vertexIndexLock);

			if (currentVertexIndex > numOfVertices - 1)
			{
				//切换渲染阶段
				r->renderStage = 2;
				//重置任务数量
				EnterCriticalSection(_getRenderer()->vertexIndexLock);
				_getRenderer()->taskIndex = 0;
				LeaveCriticalSection(_getRenderer()->vertexIndexLock);
				printf("顶点变换阶段结束\n");
				continue;
			}

			uint32_t vi = currentVertexIndex * 2;


			//printf("[%d]:completeCount:%d,currentVertexIndex:%d\n", thread->id, _getRenderer()->completeCount, currentVertexIndex);
			//模型的顶点信息
			float* vertex = _getGameIns()->cMesh->geo.vertices;
			Mesh* mesh = _getGameIns()->cMesh;
			Matrix m = _getGameIns()->cMesh->modelTMForRender;
			//buffer中心
			Vect2 half = MakeVect2((float)Renderer_GetFrameWidth() / 2.f, (float)Renderer_GetFrameHeight() / 2.f);
			//将mesh的顶点转换到相机空间 *相机的逆矩阵
			Vect2 p0 = Vect2MultMatrix(MakeVect2(vertex[vi + 0], vertex[vi + 1]), mesh->cameraTMForRender.m);
			//顶点转世界计算裁切空间位置 （需要考虑偏移值）
			Vect2 clipP = MakeVect2((p0.x / _getGameIns()->pCam->width), (p0.y / _getGameIns()->pCam->height));
			//Vect2 SA = AddVect2(MakeVect2((p0.x / _getGameIns()->pCam->width) * (float)Renderer_GetFrameWidth(), (p0.y / _getGameIns()->pCam->height) * (float)Renderer_GetFrameHeight()), half);
			//裁切空间点信息存入缓存空间
			_getGameIns()->cMesh->geo.verticesInClipForRender[vi + 0] = clipP.x;
			_getGameIns()->cMesh->geo.verticesInClipForRender[vi + 1] = clipP.y;

			//完成当前任务 任务数递增
			EnterCriticalSection(_getRenderer()->vertexIndexLock);
			_getRenderer()->taskIndex = currentVertexIndex + 1;
			LeaveCriticalSection(_getRenderer()->vertexIndexLock);

			break;
		case 2:
			//片元裁切
			//printf("片元裁切阶段\n");

			EnterCriticalSection(_getRenderer()->vertexIndexLock);
			uint32_t currentTriangleIndex = _getRenderer()->taskIndex;
			LeaveCriticalSection(_getRenderer()->vertexIndexLock);
			if (currentTriangleIndex >= _getGameIns()->cMesh->geo.numOfVertex / 3)
			{
				//切换渲染阶段
				r->renderStage = 3;
				//重置任务数
				EnterCriticalSection(_getRenderer()->vertexIndexLock);
				_getRenderer()->taskIndex = 0;
				LeaveCriticalSection(_getRenderer()->vertexIndexLock);
				printf("片元裁切阶段结束\n");
				continue;
			}

			//以三角面为单位（三个点一组）
			uint32_t ti = currentTriangleIndex * 6;
			Matrix srtm = _getGameIns()->cMesh->cameraTMForRender;

			//屏幕空间点信息
			Vect2 A = MakeVect2(_getGameIns()->cMesh->geo.verticesInClipForRender[ti + 0], _getGameIns()->cMesh->geo.verticesInClipForRender[ti + 1]);
			Vect2 B = MakeVect2(_getGameIns()->cMesh->geo.verticesInClipForRender[ti + 2], _getGameIns()->cMesh->geo.verticesInClipForRender[ti + 3]);
			Vect2 C = MakeVect2(_getGameIns()->cMesh->geo.verticesInClipForRender[ti + 4], _getGameIns()->cMesh->geo.verticesInClipForRender[ti + 5]);

			//计算boundingBox大小 
			float x_min = fminf(fminf(A.x, B.x), C.x);
			float y_min = fminf(fminf(A.y, B.y), C.y);
			float x_max = fmaxf(fmaxf(A.x, B.x), C.x);
			float y_max = fmaxf(fmaxf(A.y, B.y), C.y);

			//写入BBox值
			EnterCriticalSection(_getRenderer()->taskTriangleIndexLock);
			*(_getGameIns()->cMesh->geo.triangleBBox + currentTriangleIndex + 0) = x_min;
			*(_getGameIns()->cMesh->geo.triangleBBox + currentTriangleIndex + 1) = y_min;
			*(_getGameIns()->cMesh->geo.triangleBBox + currentTriangleIndex + 2) = x_max;
			*(_getGameIns()->cMesh->geo.triangleBBox + currentTriangleIndex + 3) = y_max;
			LeaveCriticalSection(_getRenderer()->taskTriangleIndexLock);

			//任务数递增
			EnterCriticalSection(_getRenderer()->vertexIndexLock);
			_getRenderer()->taskIndex = currentTriangleIndex + 1;
			LeaveCriticalSection(_getRenderer()->vertexIndexLock);

			break;
		case 3:
			//片元着色
			//printf("片元着色阶段\n");
			Geometry geo = _getGameIns()->cMesh->geo;

			//遍历屏幕空间像素
			for (size_t y = 0; y < Renderer_GetFrameHeight(); y++)
			{
				for (size_t x = 0; x < Renderer_GetFrameWidth(); x++)
				{
					Vect2 uv[3] = { 0 };

					uv[0] = MakeVect2(geo.uvs[vi + 0], geo.uvs[vi + 1]);
					uv[1] = MakeVect2(geo.uvs[vi + 2], geo.uvs[vi + 3]);
					uv[2] = MakeVect2(geo.uvs[vi + 4], geo.uvs[vi + 5]);

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


			break;
		default:
			break;
		}
	}
}
