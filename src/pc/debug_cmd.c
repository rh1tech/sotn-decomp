// SPDX-License-Identifier: AGPL-3.0-or-later
// Debug commands for test scripts. The platform hands over one line at a
// time (the RP2350 build reads "~C LINE~" from its debug UART, see
// tools/castle_cmd.py there); replies are single lines starting with "~R ".
//
//   go STAGE ROOM [X Y]  reload STAGE (name or id) and put the player in
//                        rooms[ROOM] at X, Y, or on the nearest floor
//   stat                 stage, room, position, HP, game state
//   rooms                the current stage's rooms (rooms[] rows)
//   god 0|1              keep HP, MP and hearts full
//   hp N                 set HP (god mode off): 1 and a hit, or 0, for Game Over
#include <game.h>
#include "pc.h"
#include "spawn_point.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int Platform_DebugCmdLine(char* out, int n) __attribute__((weak));
int ParseStageName(const char* name);
const char* StageName(int stageId);

RoomLoadDef* PcCurrentRoomLoad(void);

static bool s_God;

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
            // tilesetId 0xFF: a loading room (the way to another stage)
            printf("~R ROOM %d %d %d %d %d %d %d\n", i, r->left, r->top,
                   r->right, r->bottom, r->load.tileLayoutId,
                   r->load.tilesetId);
        }
        printf("~R ROOMS %d\n", i);
    } else if (!strcmp(argv[0], "hp") && argc >= 2) {
        s_God = false;
        g_Status.hp = atoi(argv[1]);
        printf("~R OK hp %d\n", g_Status.hp);
    } else if (!strcmp(argv[0], "god") && argc >= 2) {
        s_God = atoi(argv[1]) != 0;
        printf("~R OK god %d\n", s_God);
    } else {
        printf("~R ERR unknown command %s\n", argv[0]);
    }
}

void DebugCmd_OnFrame(void) {
    char line[96];

    if (s_God && g_GameState == Game_Play) {
        g_Status.hp = g_Status.hpMax;
        g_Status.mp = g_Status.mpMax;
        g_Status.hearts = g_Status.heartsMax;
    }
    if (Platform_DebugCmdLine && Platform_DebugCmdLine(line, sizeof(line))) {
        Run(line);
    }
}
