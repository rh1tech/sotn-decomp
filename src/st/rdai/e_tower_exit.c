// SPDX-License-Identifier: AGPL-3.0-or-later
#include "rdai.h"

#ifdef VERSION_PC
// Non-matching C for the PC port, transcribed from the US binary.
// Mirror of DAI's EntityTowerExit: RDAI only moves the camera bounds when
// the player is within 48px vertically, and never shifts g_PlayerX.
extern EInit g_EInitCommon;

void func_us_801B1C34(Entity* self) {
    s16 params = self->params;
    s16 unkX;
    s16 offsetY;

    FntPrint("set:%04x\n", params);
    FntPrint("sx:%04x\n", g_Tilemap.left);
    FntPrint("ex:%04x\n", g_Tilemap.right);
    unkX = g_PlayerX - (g_Player.unk7C - PLAYER.posX.i.hi);

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitCommon);
        self->animCurFrame = 1;
        self->animSet = 0;
        self->zPriority = 0xB0;
        return;
    case 1:
        break;
    default:
        return;
    }

    offsetY = self->posY.i.hi - PLAYER.posY.i.hi;
    if (abs(offsetY) >= 48) {
        return;
    }
    if (g_Player.status & PLAYER_STATUS_DEAD) {
        PLAYER.velocityX = 0;
        return;
    }

#define EXIT_LEFT(n, bump)                                                     \
    if (unkX < (n) && g_PlayerX < (n)) {                                       \
        g_Tilemap.x = (n);                                                     \
        if (bump)                                                              \
            g_Tilemap.left++;                                                  \
        self->step++;                                                          \
    }                                                                          \
    break
#define EXIT_RIGHT(n, bump)                                                    \
    if (unkX > (n) && g_PlayerX > (n)) {                                       \
        g_Tilemap.width = (n);                                                 \
        if (bump)                                                              \
            g_Tilemap.right--;                                                 \
        self->step++;                                                          \
    }                                                                          \
    break

    switch (params) {
    case 0:
        EXIT_LEFT(0x180, true);
    case 1:
        EXIT_RIGHT(0x280, true);
    case 2:
    case 4:
    case 13:
    case 14:
        EXIT_LEFT(0x100, true);
    case 3:
        EXIT_RIGHT(0x300, true);
    case 5:
        EXIT_RIGHT(0x480, false);
    case 6:
    case 7:
    case 9:
        EXIT_LEFT(0x80, false);
    case 8:
        EXIT_RIGHT(0x280, false);
    case 10:
    case 12:
        EXIT_RIGHT(0x280, true);
    case 11:
        EXIT_LEFT(0x180, true);
    }
#undef EXIT_LEFT
#undef EXIT_RIGHT
}
#else
INCLUDE_ASM("st/rdai/nonmatchings/e_tower_exit", func_us_801B1C34);
#endif
