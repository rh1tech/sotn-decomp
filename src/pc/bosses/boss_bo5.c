// SPDX-License-Identifier: AGPL-3.0-or-later
#include <psyz/module.h>
#include <game.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../stages/overlay.h"
#include "../pc.h"
#include <cutscene.h>
#include "../boss/bo5/bo5.h"

extern AbbreviatedOverlay2 g_BossOverlay;
extern PfnEntityUpdate EntityUpdates[];
extern LayoutEntity* entityLayoutHorizontal[];
extern LayoutEntity* entityLayoutVertical[];
extern PfnEntityUpdate* PfnEntityUpdates;
extern LayoutEntity** g_pStObjLayoutHorizontal;
extern LayoutEntity** g_pStObjLayoutVertical;
extern u8 cutscene_script[];
extern u8 D_us_8018A928[]; // portraits
extern u8 D_us_8018B6A8[];

// the cutscene script holds PSX addresses (src/pc/cutscene_pc.c)
static void InitCutscenePc(void) {
    static const CutsceneSymbolRange symbols[] = {
        {cutscene_script, 0x80181D5C, 0x80181FA0 - 0x80181D5C},
        {D_us_8018A928, 0x8018A928, 0xD80},
        {D_us_8018B6A8, 0x8018B6A8, 0xD80},
    };
    CutscenePcAlloc(symbols, LEN(symbols));
}

void Psyz_ModuleStart(void* param) {
    Overlay* o = param;

    InitCutscenePc();
    memcpy(o, &g_BossOverlay, sizeof(AbbreviatedOverlay2));
    PfnEntityUpdates = EntityUpdates;
    g_pStObjLayoutHorizontal = entityLayoutHorizontal;
    g_pStObjLayoutVertical = entityLayoutVertical;
}

void Psyz_ModuleStop(void) {}
