// SPDX-License-Identifier: AGPL-3.0-or-later
#include "rno4.h"

#ifdef VERSION_PC
// RNO4's Blue Venus Weed is RNO3's with its own palettes and sprites
// (same code, checked instruction by instruction against the US binary).
#define BLUE
#define SPIKE_SPRITES sprites_rno4_4
#define TENDRIL_COUNT 8
#define DART_CLUT_START 0x234
#define DEATH_CLUT 0x23A
#define PLANT_CLUT 0x244
#define SPIKE_CLUT_START 0x24E
#define SPIKE_CLUT_END 0x25C
#define GROWTH_SPEED 2
// RNO4 names these after the blue variant
#define g_EInitVenusWeedRoot g_EInitBlueVenusWeed1
#define g_EInitVenusWeedFlower g_EInitBlueVenusWeed2
#define g_EInitVenusWeedTendril D_us_80180B9C
#define g_EInitVenusWeedDart D_us_80180BA8

#include "../e_venus_weed.h"
#else
INCLUDE_ASM("st/rno4/nonmatchings/e_blue_venus_weed", SetupPrimsForEntitySpriteParts);

INCLUDE_ASM("st/rno4/nonmatchings/e_blue_venus_weed", EntityVenusWeed);

INCLUDE_ASM("st/rno4/nonmatchings/e_blue_venus_weed", EntityVenusWeedFlower);

INCLUDE_ASM("st/rno4/nonmatchings/e_blue_venus_weed", EntityVenusWeedTendril);

INCLUDE_ASM("st/rno4/nonmatchings/e_blue_venus_weed", EntityVenusWeedDart);

INCLUDE_ASM("st/rno4/nonmatchings/e_blue_venus_weed", EntityVenusWeedSpike);
#endif
