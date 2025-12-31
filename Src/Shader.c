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

DWORD CALLBACK Shader_ThreadMain(ShaderThread* thread) {
	DWORD threadId = thread->id;
	printf("[Shader_ThreadMain]::id = %d\n", thread->id);
	Renderer* r = _getRenderer();
	/*需要生命周期同步*/
	*(_getRenderer()->vertexIndex) = 0;
	while (true)
	{
		switch (r->renderStage)
		{
		case 1:
			//顶点变换
			//Sleep(10);
			
			//Mesh 顶点数
			uint32_t numOfVertices = _getGameIns()->cMesh->geo.numOfQuad * 6;

			printf("numOfVertices:%d\n", numOfVertices);
			//模型空间-世界空间-相机空间-裁切空间-屏幕空间
			EnterCriticalSection(_getRenderer()->taskTriangleIndexLock);
			uint32_t currentVertexIndex = *(_getRenderer()->vertexIndex);
			*(_getRenderer()->vertexIndex) = currentVertexIndex + 1;
			LeaveCriticalSection(_getRenderer()->taskTriangleIndexLock);

			if (currentVertexIndex >= numOfVertices)
			{
				r->renderStage = 2;
			}

			uint32_t vi = currentVertexIndex * 2;

			if (_getGameIns()->cMesh == NULL)continue;

			//printf("[%d]:completeCount:%d,currentVertexIndex:%d\n", thread->id, _getRenderer()->completeCount, currentVertexIndex);
			//模型的顶点信息
			float* vertex = _getGameIns()->cMesh->geo.vertices;
			Matrix m = _getGameIns()->cMesh->tmRenderCopy;

			//顶点从模型空间转换成世界空间
			Vect2 vp = Vect2MultMatrix(MakeVect2(vertex[vi + 0], vertex[vi + 1]), m.m);

			//buffer中心
			Vect2 half = MakeVect2((float)Renderer_GetFrameWidth() / 2.f, (float)Renderer_GetFrameHeight() / 2.f);

			//将mesh的顶点转换到相机空间 *相机的逆矩阵
			Vect2 p0 = Vect2MultMatrix(vp, _getRenderer()->camera.tm.m);

			//顶点转世界计算画幅空间位置 （需要考虑偏移值）
			Vect2 A = AddVect2(MakeVect2((p0.x / _getGameIns()->pCam->width) * (float)Renderer_GetFrameWidth(), (p0.y / _getGameIns()->pCam->height) * (float)Renderer_GetFrameHeight()), half);

			//屏幕空间点信息存入缓存空间
			_getGameIns()->cMesh->geo.verticesRenderCopy[vi + 0] = A.x;
			_getGameIns()->cMesh->geo.verticesRenderCopy[vi + 1] = A.y;

			//完成当前任务 任务数递增
			EnterCriticalSection(_getRenderer()->vertexIndexLock);
			_getRenderer()->completeCount += 1;
			LeaveCriticalSection(_getRenderer()->vertexIndexLock);

			break;
		case 2:
			//片元裁切
			//printf("片元裁切阶段\n");
			//以三角面为单位（三个点一组）
			for (size_t v = 0; v < _getGameIns()->cMesh->geo.numOfQuad * 2; v++)
			{
				uint32_t vi = v * 6;

				Matrix srtm = _getGameIns()->cMesh->tmRenderCopy;

				//屏幕空间点信息
				Vect2 A = MakeVect2(_getGameIns()->cMesh->geo.verticesRenderCopy[vi + 0], _getGameIns()->cMesh->geo.verticesRenderCopy[vi + 1]);
				Vect2 B = MakeVect2(_getGameIns()->cMesh->geo.verticesRenderCopy[vi + 2], _getGameIns()->cMesh->geo.verticesRenderCopy[vi + 3]);
				Vect2 C = MakeVect2(_getGameIns()->cMesh->geo.verticesRenderCopy[vi + 4], _getGameIns()->cMesh->geo.verticesRenderCopy[vi + 5]);

				//计算boundingBox大小 
				float x_min = fminf(fminf(A.x, B.x), C.x);
				float x_max = fmaxf(fmaxf(A.x, B.x), C.x);
				float y_min = fminf(fminf(A.y, B.y), C.y);
				float y_max = fmaxf(fmaxf(A.y, B.y), C.y);

			}

			break;
		case 3:
			//片元着色
			//printf("片元着色阶段\n");
			break;
		default:
			break;
		}
	}
}

//shader请求任务的小函数
//结果缓冲区边界判断
//顶点任务完成判断
