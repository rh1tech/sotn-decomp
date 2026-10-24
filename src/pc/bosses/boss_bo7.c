// SPDX-License-Identifier: AGPL-3.0-or-later
#include <psyz/module.h>
#include <game.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../stages/overlay.h"
#include "../pc.h"
#include "../boss/bo7/bo7.h"

extern Overlay g_BossOverlay;
extern PfnEntityUpdate EntityUpdates[];
extern LayoutEntity* entityLayoutHorizontal[];
extern LayoutEntity* entityLayoutVertical[];
extern PfnEntityUpdate* PfnEntityUpdates;
extern LayoutEntity** g_pStObjLayoutHorizontal;
extern LayoutEntity** g_pStObjLayoutVertical;
extern CutsceneSymbolRange bo7_pc_regions[];
extern int bo7_pc_region_count;

// code translated from asm builds PSX addresses of the overlay's data
// (include/mips2c_rt.h): the raw data regions resolve them
static void InitPsxRanges(void) {
    CutscenePcAlloc(bo7_pc_regions, bo7_pc_region_count);
}

void Psyz_ModuleStart(void* param) {
    Overlay* o = param;

    InitPsxRanges();
    memcpy(o, &g_BossOverlay, sizeof(Overlay));
    PfnEntityUpdates = EntityUpdates;
    g_pStObjLayoutHorizontal = entityLayoutHorizontal;
    g_pStObjLayoutVertical = entityLayoutVertical;
}

void Psyz_ModuleStop(void) {}
