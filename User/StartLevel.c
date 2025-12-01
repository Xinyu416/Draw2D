#include "StartLevel.h"
#include "GameInstance.h"

void StartLevel_BeginPlay() {
	
	printf("StartLevel_BeginPlay(),ID:%d\n", _getGameIns()->pLevel->levelID);
}