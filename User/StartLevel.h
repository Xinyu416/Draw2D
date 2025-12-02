#pragma once
#include "Define.h"

#define OPENSTARTLEVEL(ins,lvID){\
StartLevel* start = (StartLevel*)malloc(sizeof(StartLevel));\
start->base.MouseKeyEvent = StartLevel_MouseKeyEvent ; \
start->base.BeginPlay = StartLevel_BeginPlay ; \
start->base.EndPlay = StartLevel_EndPlay ; \
start->base.Tick = StartLevel_Tick ; \
start->base.levelID = lvID; \
ins->pLevel = (Level*)start; \
}

typedef struct {
	Level base;
}StartLevel;

void StartLevel_BeginPlay();

void StartLevel_EndPlay();

void StartLevel_Tick(float delta);

void StartLevel_MouseKeyEvent(VMEVENT eventType, void* key);