#include "GameInstance.h"
#include "Mesh.h"

void Game_Init() {

}

void Game_Release() {

}

void Game_Tick(float delta) {

}

void RenderMesh(uint8_t* buffer,uint32_t height,uint32_t width,uint8_t bytepp,Mesh mesh) {
	for (size_t y = 0; y <height; y++)
	{
		for (size_t x = 0; x < width; x++)
		{
			Vect2 p = MakeVect2(width,height);

			//需要判断mesh中的geo中的quad跟buffer的相对坐标 画颜色
			for (size_t i = 0; i < mesh.geo.numOfQuad; i++)
			{
				////Quad quad = mesh.geo.
				//	for (size_t i = 0; i < mesh.geo.numOfQuad * 8; i++)
				//	{
				//		if (i % 2 == 0) {

				//			printf("vertices[%d]:(%.0f,%.0f)\n", (i / 2), mesh.geo.vertices[i], mesh.geo.vertices[i + 1]);
				//		}
				//	}
				//if (IsPointInQuad(p,))
				//{

				//}
			}

			size_t index = y * width * bytepp + x * bytepp;
			//bgr
			if (y % 10 == 0 || (x * bytepp) % 10 == 0)
			{
				buffer[index + 0] = rand() % 255;
				buffer[index + 1] = rand() % 255;
				buffer[index + 2] = rand() % 255;
			}
			else
			{
				buffer[index + 0] = 255;
				buffer[index + 1] = 255;
				buffer[index + 2] = 255;
			}
		}
	}
}