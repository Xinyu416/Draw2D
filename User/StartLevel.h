#pragma once
#include "Define.h"

#define OPENSTARTLEVEL(ins){\
StartLevel* start = (StartLevel*)malloc(sizeof(StartLevel));\
start->base.BeginPlay = StartLevel_BeginPlay ; \
ins->pLevel = (Level*)start;\
\
}

typedef struct {
	Level base;
}StartLevel;

void StartLevel_BeginPlay();