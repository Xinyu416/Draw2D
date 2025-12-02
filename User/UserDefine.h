#pragma once
#include "Define.h"
#include "StartLevel.h"
#include "PlayLevel.h"
#include "Scene.h"

#define OPENLEVEL(ins,levelID){\
	if (levelID == 1){ OPENSTARTLEVEL(ins,levelID) }\
	if (levelID == 2){ OPENPLAYLEVEL(ins,levelID) }\
	if (levelID == 3){ OPENSCENE(ins,levelID) }\
} 