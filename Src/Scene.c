#include "Scene.h"

void InitGridData() {
	uint32_t wNum = _getGameEngine()->width / GridSize;
	uint32_t hNum = _getGameEngine()->height / GridSize;
	printf("wNum:%d,hNum%d\n",wNum,hNum);
}