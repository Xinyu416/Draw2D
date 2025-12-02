#pragma once
#include "PlayLevel.h"
#include "Define.h"

#define OPENPLAYLEVEL(ins,lvID){\
PlayLevel* play = (PlayLevel*)malloc(sizeof(PlayLevel));\
play->base.MouseKeyEvent = PlayLevel_MouseKeyEvent ; \
play->base.BeginPlay = PlayLevel_BeginPlay ; \
play->base.EndPlay = PlayLevel_EndPlay ; \
play->base.Tick = PlayLevel_Tick ; \
play->base.levelID = lvID;\
ins->pLevel = (Level*)play;\
}

typedef struct {
	Level base;
}PlayLevel;

void PlayLevelInit();

void PlayLevel_BeginPlay();

void PlayLevel_EndPlay();

void PlayLevel_Tick(float delta);

void PlayLevel_MouseKeyEvent(VMEVENT eventType, void* key);