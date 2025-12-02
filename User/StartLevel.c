#include "StartLevel.h"
#include "GameInstance.h"

void StartLevel_BeginPlay() {
	printf("StartLevel_BeginPlay(),ID:%d\n", _getGameIns()->pLevel->levelID);
	GameIns_OpenLevel(2);
}

void StartLevel_EndPlay() {
	printf("StartLevel_EndPlay(),ID:%d\n", _getGameIns()->pLevel->levelID);
}

void StartLevel_Tick(float delta) {
	printf("StartLevel_Tick()\n,ID:%d,delta:%f\n", _getGameIns()->pLevel->levelID, delta);
}

void StartLevel_MouseKeyEvent(VMEVENT eventType, void* key) {
	//接收处理键盘事件
	printf("StartLevel_KeyDown: type:%d,key:%c\n", eventType, (char)key);
}