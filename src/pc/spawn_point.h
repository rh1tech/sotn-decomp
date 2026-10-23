// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef PC_SPAWN_POINT_H
#define PC_SPAWN_POINT_H

#include <game.h>
#include "pc.h"

int SpawnPoint_StageForName(const char* name);
void SpawnPoint_Init(const struct InitGameParams* params);
void SpawnPoint_OnFrame(void);
void SpawnPoint_HookOverlay(Overlay* o);
// reload stageId and put the player in room roomIndex (rooms[] row) at
// x, y; negative x or y: look for floor
void SpawnPoint_Warp(s32 stageId, s32 roomIndex, s32 x, s32 y);
s32 SpawnPoint_NextStage(s32 stageId);

#endif
