// SPDX-License-Identifier: AGPL-3.0-or-later
#include <psyz/module.h>
#include <game.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../stages/overlay.h"
#include "../pc.h"
#include <cutscene.h>
#include "../boss/bo6/bo6.h"

extern AbbreviatedOverlay2 g_BossOverlay;
extern PfnEntityUpdate EntityUpdates[];
extern LayoutEntity* entityLayoutHorizontal[];
extern LayoutEntity* entityLayoutVertical[];
extern PfnEntityUpdate* PfnEntityUpdates;
extern LayoutEntity** g_pStObjLayoutHorizontal;
extern LayoutEntity** g_pStObjLayoutVertical;
extern u8 bo6_region_8018303C[];

// the cutscene scripts hold PSX addresses (src/pc/cutscene_pc.c); scripts
// and portraits all live in this raw data region
static void InitCutscenePc(void) {
    static const CutsceneSymbolRange symbols[] = {
        {bo6_region_8018303C, 0x8018303C, 0x801A6320 - 0x8018303C},
    };
    CutscenePcAlloc(symbols, LEN(symbols));
}

void Psyz_ModuleStart(void* param) {
    AbbreviatedOverlay2* o = param;

    InitCutscenePc();
    memcpy(o, &g_BossOverlay, sizeof(AbbreviatedOverlay2));
    PfnEntityUpdates = EntityUpdates;
    g_pStObjLayoutHorizontal = entityLayoutHorizontal;
    g_pStObjLayoutVertical = entityLayoutVertical;
}

void Psyz_ModuleStop(void) {}
