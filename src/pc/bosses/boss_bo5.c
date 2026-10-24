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

extern CutsceneSymbolRange bo5_pc_regions[];
extern int bo5_pc_region_count;

// PSX addresses (src/pc/cutscene_pc.c): the cutscene script and the
// portraits are C data, the rest of the data raw regions that the code
// translated from asm addresses (include/mips2c_rt.h)
static int ByPsxAddr(const void* a, const void* b) {
    u32 x = ((const CutsceneSymbolRange*)a)->psxAddr;
    u32 y = ((const CutsceneSymbolRange*)b)->psxAddr;
    return x < y ? -1 : x > y;
}

static void InitCutscenePc(void) {
    static CutsceneSymbolRange symbols[24];
    int n = 0, i;

    symbols[n++] = (CutsceneSymbolRange){
        cutscene_script, 0x80181D5C, 0x80181FA0 - 0x80181D5C};
    symbols[n++] = (CutsceneSymbolRange){D_us_8018A928, 0x8018A928, 0xD80};
    symbols[n++] = (CutsceneSymbolRange){D_us_8018B6A8, 0x8018B6A8, 0xD80};
    for (i = 0; i < bo5_pc_region_count && n < LEN(symbols); i++) {
        symbols[n++] = bo5_pc_regions[i];
    }
    qsort(symbols, n, sizeof(symbols[0]), ByPsxAddr);
    CutscenePcAlloc(symbols, n);
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
