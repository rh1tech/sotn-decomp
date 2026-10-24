// SPDX-License-Identifier: AGPL-3.0-or-later
// Debug commands for test scripts. The platform hands over one line at a
// time (the RP2350 build reads "~C LINE~" from its debug UART, see
// tools/castle_cmd.py there); replies are single lines starting with "~R ".
//
//   go STAGE ROOM [X Y]  reload STAGE (name or id) and put the player in
//                        rooms[ROOM] at X, Y, or on the nearest floor
//   near STAGE ROOM X Y  as go, on the floor closest to X, Y (room pixels)
//   stat                 stage, room, position, HP, game state
//   rooms                the current stage's rooms (rooms[] rows, and the
//                        fg layer params: 0x20 is a save room)
//   god 0|1              keep HP, MP and hearts full
//   playtest 0|1         god mode, and every enemy (bosses too) at 1 HP:
//                        a whole play-through quickly (also --play-test)
//   hp N                 set HP (god mode off): 1 and a hit, or 0, for Game Over
//   fx 0|1|2             reduced effects: never, when running late, always
//   ehp N                cap the HP of the stage's hittable entities
//                        (bosses too) at N
//   ents                 the stage's hittable entities: index, id, x, y, HP
//   cflag I [V]          read, or set, g_CastleFlags[I]
//   equip SLOT ID        put item ID in equipment SLOT (2: head; 0x22 is
//                        the Holy glasses)
#include <game.h>
#include "pc.h"
#include "spawn_point.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int Platform_DebugCmdLine(char* out, int n) __attribute__((weak));
// reduced effects: 0 full, 1 when running late, 2 always (platform option)
void Platform_SetFxMode(int mode) __attribute__((weak));
int Platform_GetFxMode(void) __attribute__((weak));
int ParseStageName(const char* name);
const char* StageName(int stageId);

RoomLoadDef* PcCurrentRoomLoad(void);

static bool s_God;
static bool s_PlayTest;

void DebugCmd_SetPlayTest(bool on) { s_PlayTest = on; }

static s32 CurrentRoom(void) {
    s32 i;

    if (g_api.o.rooms == NULL) {
        return -1;
    }
    // the room the game loaded (camera locks can narrow g_Tilemap's
    // bounds, and small rooms can share corners with bigger ones)
    for (i = 0; g_api.o.rooms[i].left != 0x40; i++) {
        if (PcCurrentRoomLoad() == &g_api.o.rooms[i].load) {
            return i;
        }
    }
    return -1;
}

static void Run(char* line) {
    char* argv[6];
    s32 argc = 0, i, stage;
    char* tok = strtok(line, " ");

    while (tok && argc < 6) {
        argv[argc++] = tok;
        tok = strtok(NULL, " ");
    }
    if (argc == 0) {
        return;
    }
    if (!strcmp(argv[0], "go") && argc >= 3) {
        stage = ParseStageName(argv[1]);
        if (stage < 0 || g_GameState != Game_Play) {
            printf("~R ERR go %s: %s\n", argv[1],
                   stage < 0 ? "unknown stage" : "not in gameplay");
            return;
        }
        SpawnPoint_Warp(stage, atoi(argv[2]), argc >= 5 ? atoi(argv[3]) : -1,
                        argc >= 5 ? atoi(argv[4]) : -1);
        printf("~R OK go %s %s\n", StageName(stage), argv[2]);
    } else if (!strcmp(argv[0], "near") && argc >= 5) {
        stage = ParseStageName(argv[1]);
        if (stage < 0 || g_GameState != Game_Play) {
            printf("~R ERR near %s\n", argv[1]);
            return;
        }
        SpawnPoint_WarpNear(stage, atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
        printf("~R OK near %s %s\n", StageName(stage), argv[2]);
    } else if (!strcmp(argv[0], "stat")) {
        printf("~R STAT stage=%s room=%d x=%d y=%d hp=%d/%d state=%d step=%d "
               "dead=%d at=%d,%d\n",
               StageName(g_StageId), CurrentRoom(),
               g_Tilemap.scrollX.i.hi + PLAYER.posX.i.hi,
               g_Tilemap.scrollY.i.hi + PLAYER.posY.i.hi, g_Status.hp,
               g_Status.hpMax, g_GameState, g_GameStep,
               (g_Player.status & PLAYER_STATUS_DEAD) != 0, g_Tilemap.left,
               g_Tilemap.top);
    } else if (!strcmp(argv[0], "rooms")) {
        if (g_api.o.rooms == NULL) {
            printf("~R ROOMS 0\n");
            return;
        }
        for (i = 0; g_api.o.rooms[i].left != 0x40; i++) {
            const RoomHeader* r = &g_api.o.rooms[i];
            s32 params = 0;
            // the fg layer's params: 0x20 marks a save room
            if (r->load.tilesetId != 0xFF && g_api.o.tileLayers &&
                g_api.o.tileLayers[r->load.tileLayoutId].fg) {
                params = g_api.o.tileLayers[r->load.tileLayoutId].fg->rect.params;
            }
            // tilesetId 0xFF: a loading room (the way to another stage)
            printf("~R ROOM %d %d %d %d %d %d %d %d\n", i, r->left, r->top,
                   r->right, r->bottom, r->load.tileLayoutId,
                   r->load.tilesetId, params);
        }
        printf("~R ROOMS %d\n", i);
    } else if (!strcmp(argv[0], "fx") && argc >= 2) {
        if (Platform_SetFxMode) {
            Platform_SetFxMode(atoi(argv[1]));
        }
        printf("~R OK fx %d\n", Platform_GetFxMode ? Platform_GetFxMode() : -1);
    } else if (!strcmp(argv[0], "hp") && argc >= 2) {
        s_God = false;
        g_Status.hp = atoi(argv[1]);
        printf("~R OK hp %d\n", g_Status.hp);
    } else if (!strcmp(argv[0], "ehp") && argc >= 2) {
        s32 n = 0, cap = atoi(argv[1]);
        for (i = STAGE_ENTITY_START; i < LEN(g_Entities); i++) {
            Entity* e = &g_Entities[i];
            if (e->entityId && e->hitboxState && e->hitPoints > cap) {
                e->hitPoints = cap;
                n++;
            }
        }
        printf("~R OK ehp %d (%d entities)\n", cap, n);
    } else if (!strcmp(argv[0], "ents")) {
        s32 n = 0;
        for (i = STAGE_ENTITY_START; i < LEN(g_Entities); i++) {
            Entity* e = &g_Entities[i];
            if (e->entityId && e->hitboxState && e->hitPoints > 0) {
                printf("~R ENT %d %d %d %d %d\n", i, e->entityId,
                       g_Tilemap.scrollX.i.hi + e->posX.i.hi,
                       g_Tilemap.scrollY.i.hi + e->posY.i.hi, e->hitPoints);
                n++;
            }
        }
        printf("~R ENTS %d\n", n);
    } else if (!strcmp(argv[0], "cflag") && argc >= 2) {
        i = atoi(argv[1]);
        if (i < 0 || i >= LEN(g_CastleFlags)) {
            printf("~R ERR cflag %d\n", i);
            return;
        }
        if (argc >= 3) {
            g_CastleFlags[i] = atoi(argv[2]);
        }
        printf("~R OK cflag %d %d\n", i, g_CastleFlags[i]);
    } else if (!strcmp(argv[0], "equip") && argc >= 3) {
        i = atoi(argv[1]);
        // equipment[] runs on into wornEquipment[]: seven slots
        if (i < 0 || i >= 7) {
            printf("~R ERR equip slot %d\n", i);
            return;
        }
        g_Status.equipment[i] = strtol(argv[2], NULL, 0);
        printf("~R OK equip %d %d\n", i, g_Status.equipment[i]);
    } else if (!strcmp(argv[0], "playtest") && argc >= 2) {
        s_PlayTest = atoi(argv[1]) != 0;
        printf("~R OK playtest %d\n", s_PlayTest);
    } else if (!strcmp(argv[0], "god") && argc >= 2) {
        s_God = atoi(argv[1]) != 0;
        printf("~R OK god %d\n", s_God);
    } else {
        printf("~R ERR unknown command %s\n", argv[0]);
    }
}

void DebugCmd_OnFrame(void) {
    char line[96];

    if (s_PlayTest && g_GameState == Game_Play) {
        s32 i;
        for (i = STAGE_ENTITY_START; i < LEN(g_Entities); i++) {
            Entity* e = &g_Entities[i];
            if (e->entityId && e->hitboxState && e->hitPoints > 1) {
                e->hitPoints = 1;
            }
        }
    }
    if ((s_God || s_PlayTest) && g_GameState == Game_Play) {
        g_Status.hp = g_Status.hpMax;
        g_Status.mp = g_Status.mpMax;
        g_Status.hearts = g_Status.heartsMax;
    }
    if (Platform_DebugCmdLine && Platform_DebugCmdLine(line, sizeof(line))) {
        Run(line);
    }
}
