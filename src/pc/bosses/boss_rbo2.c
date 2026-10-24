// SPDX-License-Identifier: AGPL-3.0-or-later
#include <psyz/module.h>
#include <game.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../stages/overlay.h"
#include "../pc.h"
#include <cutscene.h>
#include "../boss/rbo2/rbo2.h"

extern AbbreviatedOverlay g_BossOverlay;
extern PfnEntityUpdate EntityUpdates[];
extern LayoutEntity* entityLayoutHorizontal[];
extern LayoutEntity* entityLayoutVertical[];
extern PfnEntityUpdate* PfnEntityUpdates;
extern LayoutEntity** g_pStObjLayoutHorizontal;
extern LayoutEntity** g_pStObjLayoutVertical;
extern CutsceneSymbolRange rbo2_pc_regions[];
extern int rbo2_pc_region_count;

// the cutscene scripts hold PSX addresses (src/pc/cutscene_pc.c): their
// data and the portraits are in the raw data regions
static void InitCutscenePc(void) {
    CutscenePcAlloc(rbo2_pc_regions, rbo2_pc_region_count);
}

void Psyz_ModuleStart(void* param) {
    Overlay* o = param;

    InitCutscenePc();
    memcpy(o, &g_BossOverlay, sizeof(AbbreviatedOverlay));
    PfnEntityUpdates = EntityUpdates;
    g_pStObjLayoutHorizontal = entityLayoutHorizontal;
    g_pStObjLayoutVertical = entityLayoutVertical;
}

void Psyz_ModuleStop(void) {}
