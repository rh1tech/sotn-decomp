// SPDX-License-Identifier: AGPL-3.0-or-later
#include "spawn_point.h"

#include <string.h>

extern u32 CheckAndDoLevelUp(void);

struct SpawnPoint {
    s16 stageId;
    u8 roomIndex;
    s16 x;
    s16 y;
    const char* name;
};

static const struct SpawnPoint s_SpawnPoints[] = {
    {STAGE_NP3, 1, 192, 870, "np3-stage15-door"},
    {STAGE_RNO3, 1, 96, 104, "reverse-trapdoor-underground"},
    {STAGE_NO0, 0, 1440, 688, "marble-soul"},
    {STAGE_NO4, 5, 176, 396, "caverns-soul"},
    {STAGE_RNO0, 0, 64, 132, "reverse-marble-soul"},
    {STAGE_RNO4, 5, 208, 132, "reverse-caverns-soul"},
    {STAGE_RNO4, 26, 128, 128, "reverse-caverns-waterfall"},
    {STAGE_RNO4, 15, 128, 128, "reverse-caverns-bomb-knight"},
    {STAGE_RNO4, 25, 128, 128, "reverse-caverns-cave-troll"},
    {STAGE_RNO4, 8, 128, 128, "reverse-caverns-wall"},
    {STAGE_DAI, 9, 96, 120, "chapel-stained-glass"},
    {STAGE_RDAI, 3, 64, 144, "anti-chapel-panthers"},
    {STAGE_RDAI, 9, 96, 368, "anti-chapel-spectral-sword"},
    {STAGE_RDAI, 11, 416, 1024, "anti-chapel-archers"},
};

enum SpawnPointStep {
    SpawnPoint_Off,
    SpawnPoint_WaitLoad, // SpawnPoint_Warp: the stage is (re)loading
    SpawnPoint_PickRoom,
    SpawnPoint_PlacePlayer,
};

static enum SpawnPointStep s_Step = SpawnPoint_Off;
static const struct SpawnPoint* s_Spawn = NULL;
static struct SpawnPoint s_Warp;
static s32 s_PrefX = 128, s_PrefY = 120; // where FindFloor looks first
static s32 s_WarpStage = -1;
static void (*s_InitRoomEntities)(s32 layoutId) = NULL;

static const struct SpawnPoint* FindSpawnPoint(const char* name) {
    s32 i;

    if (name == NULL) {
        return NULL;
    }
    for (i = 0; i < LEN(s_SpawnPoints); i++) {
        if (!strcmp(name, s_SpawnPoints[i].name)) {
            return &s_SpawnPoints[i];
        }
    }
    return NULL;
}

int SpawnPoint_StageForName(const char* name) {
    const struct SpawnPoint* spawn = FindSpawnPoint(name);
    return spawn == NULL ? -1 : spawn->stageId;
}

void SpawnPoint_Init(const struct InitGameParams* params) {
    s_Step = SpawnPoint_Off;
    s_Spawn = FindSpawnPoint(params->spawnPoint);
    if (s_Spawn != NULL) {
        s_Step = SpawnPoint_PickRoom;
    }
}

static bool SpawnPoint_FindFloor(s32* outX, s32* outY);

void SpawnPoint_OnFrame(void) {
    const RoomHeader* room;

    if (s_Step == SpawnPoint_WaitLoad) {
        // the new overlay is in once the load sequence reaches Play_Init
        if (g_GameState != Game_Play || g_GameStep != Play_Init ||
            g_StageId != s_Spawn->stageId) {
            return;
        }
        s_Step = SpawnPoint_PickRoom;
    }
    if (s_Step != SpawnPoint_PickRoom) {
        return;
    }
    if (g_GameState != Game_Play || g_GameStep < Play_Init) {
        return;
    }
    if (g_StageId != s_Spawn->stageId || g_api.o.rooms == NULL) {
        ERRORF("spawn point does not match the loaded stage %02X", g_StageId);
        s_Step = SpawnPoint_Off;
        return;
    }
    if (g_StageId & STAGE_INVERTEDCASTLE_FLAG) {
        CheckAndDoLevelUp();
    }
    room = &g_api.o.rooms[s_Spawn->roomIndex];
    // The game loads the first room in rooms[] containing this screen, and
    // small rooms can sit inside bigger ones listed later: use a screen
    // only the wanted room has (its top-left corner when it is free).
    {
        s32 x, y, i, cx = room->left, cy = room->top;
        bool found = false;

        for (y = room->top; y <= room->bottom && !found; y++) {
            for (x = room->left; x <= room->right && !found; x++) {
                for (i = 0; i < s_Spawn->roomIndex; i++) {
                    const RoomHeader* r = &g_api.o.rooms[i];
                    if (x >= r->left && x <= r->right && y >= r->top &&
                        y <= r->bottom) {
                        break;
                    }
                }
                if (i == s_Spawn->roomIndex) {
                    cx = x;
                    cy = y;
                    found = true;
                }
            }
        }
        g_Tilemap.left = cx;
        g_Tilemap.top = cy;
        // and stand there: on a shared screen the game would move the
        // player into the other room
        s_PrefX = (cx - room->left) * 256 + 128;
        s_PrefY = (cy - room->top) * 256 + 120;
    }
    D_8003C730 = 1;
    s_Step = SpawnPoint_PlacePlayer;
}

static void SpawnPoint_PlacePlayerNow(void) {
    s32 x, y;

    if (s_Step != SpawnPoint_PlacePlayer) {
        return;
    }
    s_Step = SpawnPoint_Off;

    x = s_Spawn->x;
    y = s_Spawn->y;
    if ((x < 0 || y < 0) && !SpawnPoint_FindFloor(&x, &y)) {
        x = 128;
        y = 128;
    }

    g_PlayerX = x;
    g_PlayerY = y;
    PLAYER.posX.i.hi = (u8)x;
    PLAYER.posY.i.hi = (u8)y;
    PLAYER.facingLeft = PLAYER.posX.i.hi >= 0x80;

    D_8009791C.x = g_Tilemap.left + (g_PlayerX >> 8);
    D_8009791C.y = g_Tilemap.top + (g_PlayerY >> 8);
    g_Tilemap.scrollX.i.hi = (D_8009791C.x - g_Tilemap.left) << 8;
    g_Tilemap.scrollY.i.hi = (D_8009791C.y - g_Tilemap.top) << 8;
    g_PrevScrollX = g_Tilemap.scrollX.i.hi;
    g_PrevScrollY = g_Tilemap.scrollY.i.hi;
}

static void SpawnPoint_InitRoomEntities(s32 layoutId) {
    SpawnPoint_PlacePlayerNow();
    s_InitRoomEntities(layoutId);
}

// Somewhere to stand, in room pixels: floor under the feet (posY + 0x19,
// as the player's own checks) and nothing solid where the body goes. The
// room's layers are in by the time InitRoomEntities runs; the closest spot
// to the middle of the screen PickRoom chose wins.
static bool SpawnPoint_FindFloor(s32* outX, s32* outY) {
    Collider col;
    s32 x, y, dy, best = -1, d;
    s32 w = g_Tilemap.hSize << 8, h = g_Tilemap.vSize << 8;
    bool clear;

    g_Tilemap.scrollX.i.hi = 0; // CheckCollision is relative to the camera
    g_Tilemap.scrollY.i.hi = 0;
    for (y = 32; y < h - 32; y += 4) {
        for (x = 16; x < w - 16; x += 8) {
            g_api.CheckCollision(x, y + 0x19, &col, 0);
            if (!(col.effects & EFFECT_SOLID)) {
                continue;
            }
            clear = true;
            for (dy = -0x18; dy <= 0x14 && clear; dy += 8) {
                g_api.CheckCollision(x, y + dy, &col, 0);
                clear = col.effects == EFFECT_NONE;
            }
            if (!clear) {
                continue;
            }
            d = abs(x - s_PrefX) + abs(y - s_PrefY);
            if (best < 0 || d < best) {
                best = d;
                *outX = x;
                *outY = y;
            }
        }
    }
    if (best < 0) {
        INFOF("spawn: no floor in the room");
    }
    return best >= 0;
}

void SpawnPoint_Warp(s32 stageId, s32 roomIndex, s32 x, s32 y) {
    s_Warp.stageId = stageId;
    s_Warp.roomIndex = roomIndex;
    s_Warp.x = x;
    s_Warp.y = y;
    s_Warp.name = "warp";
    s_Spawn = &s_Warp;
    s_Step = SpawnPoint_WaitLoad;
    s_WarpStage = stageId;
    // what a stage exit does: reload through Play_PrepareNextStage, which
    // takes the stage from SpawnPoint_NextStage
    D_80097C98 = 0;
    g_GameStep = Play_PrepareNextStage;
}

s32 SpawnPoint_NextStage(s32 stageId) {
    if (s_WarpStage >= 0) {
        stageId = s_WarpStage;
        s_WarpStage = -1;
    }
    return stageId;
}

void SpawnPoint_HookOverlay(Overlay* o) {
    if (s_Step == SpawnPoint_Off || o->InitRoomEntities == NULL) {
        return;
    }
    s_InitRoomEntities = o->InitRoomEntities;
    o->InitRoomEntities = SpawnPoint_InitRoomEntities;
}
