// SPDX-License-Identifier: AGPL-3.0-or-later
#include "rdai.h"

#ifdef VERSION_PC
// Non-matching C for the PC port, transcribed from the US binary.
typedef struct {
    /* 0x7C */ Primitive* strip;     // eight stacked quads
    /* 0x80 */ Primitive* stripLast;
    /* 0x84 */ s16 timer;
    /* 0x86 */ s16 : 16;
    /* 0x88 */ Primitive* quadA;
    /* 0x8C */ Primitive* quadB;
    /* 0x90 */ Primitive* sparkA;    // UnkPolyFunc2 prims
    /* 0x94 */ u8 phaseStrip;
    /* 0x95 */ u8 phaseA;
    /* 0x96 */ u8 phaseB;
    /* 0x97 */ u8 unk97;
    /* 0x98 */ u8 unk98;
    /* 0x99 */ u8 timerA;
    /* 0x9A */ u8 timerB;
    /* 0x9B */ u8 flicker;
    /* 0x9C */ Primitive* sparkB;
} PantherExt;
#define PANTHER(e) ((PantherExt*)&(e)->ext)

// Move (x, y), shared with (nx, ny) of the next quad, 7/8 of the way
// towards the middle of a and b.
static void PantherSmoothCorner(s16* x, s16* y, s16* nx, s16* ny, s16 ax,
                                s16 ay, s16 bx, s16 by) {
    s32 midX = (ax + bx) / 2;
    s32 midY = (ay + by) / 2;
    s16 newX = (*x - midX) / 8 + midX;
    s16 newY = (*y - midY) / 8 + midY;
    *x = *nx = newX;
    *y = *ny = newY;
}

// Relax the strip: each joint between two quads drifts towards the middle
// of the quads' outer edges.
void func_us_801C4B2C(void) {
    Primitive* prim = PANTHER(g_CurrentEntity)->strip;
    Primitive* next;
    s32 i;

    for (i = 0; i < 7; i++) {
        next = prim->next;
        PantherSmoothCorner(&prim->x2, &prim->y2, &next->x0, &next->y0,
                            next->x2, next->y2, prim->x0, prim->y0);
        PantherSmoothCorner(&prim->x3, &prim->y3, &next->x1, &next->y1,
                            next->x3, next->y3, prim->x1, prim->y1);
        prim = next;
    }
}

static void PantherQuad(Primitive* prim, u8 u0, u8 u1, u8 v0, u8 v1) {
    Entity* self = g_CurrentEntity;

    prim->tpage = 0x12;
    prim->clut = 0x164;
    prim->u0 = prim->u2 = u0;
    prim->u1 = prim->u3 = u1;
    prim->v0 = prim->v1 = v0;
    prim->v2 = prim->v3 = v1;
    PGREY(prim, 0) = PGREY(prim, 1) = PGREY(prim, 2) = PGREY(prim, 3) = 0x80;
    prim->x0 = prim->x2 =
        self->facingLeft ? self->posX.i.hi - 0x30 : self->posX.i.hi - 0x10;
    prim->x1 = prim->x3 = prim->x0 + 0x40;
}

static void PantherSpark(Primitive* prim, s16 y, s16 scaleX, s16 scaleY) {
    Entity* self = g_CurrentEntity;

    UnkPolyFunc2(prim);
    prim->tpage = 0x12;
    prim->clut = 0x164;
    prim->next->x1 =
        self->facingLeft ? self->posX.i.hi - 0x12 : self->posX.i.hi + 0x12;
    prim->next->y0 = self->posY.i.hi + y;
    LOW(prim->next->u0) = 0;
    LOW(prim->next->r1) = 0;
    LOH(prim->next->r2) = scaleX;
    LOH(prim->next->b2) = scaleY;
    prim->next->b3 = 0x80;
    prim->drawMode = DRAW_HIDE | DRAW_UNK02;
    prim->priority = self->zPriority + 2;
}

static void PantherSparkGrow(Primitive* prim, s16 stepX) {
    if (LOH(prim->next->b2) < 0xD) {
        LOH(prim->next->b2)++;
    }
    if (LOH(prim->next->r2) < 0x3C) {
        LOH(prim->next->r2) += stepX;
    }
}

// Pounce effect
void func_us_801C4C4C(void) {
    Entity* self = g_CurrentEntity;
    Primitive* prim;
    Primitive* last;
    s32 primIndex;
    s32 i;
    s16 dy;

    switch (self->step_s) {
    case 0:
        self->hitboxState = 0;
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 20);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        prim = &g_PrimBuf[primIndex];
        self->primIndex = primIndex;
        PANTHER(self)->strip = prim;
        self->flags |= FLAG_HAS_PRIMS;
        for (dy = -7, i = 0; dy < 9; dy += 2, i++) {
            PantherQuad(prim, self->facingLeft ? 0x40 : 0,
                        self->facingLeft ? 0 : 0x40, 0xB5 + i * 2,
                        0xB8 + i * 2);
            prim->y0 = prim->y1 = self->posY.i.hi + dy - 3;
            prim->y2 = prim->y3 = prim->y0 + 4;
            prim->drawMode = DRAW_UNK02;
            prim->priority = self->zPriority + 2;
            last = prim;
            prim = prim->next;
        }
        PANTHER(self)->stripLast = last;

        PantherQuad(prim, 0, 0x40, 0xEE, 0xEF);
        prim->y0 = prim->y1 = prim->y2 = prim->y3 = self->posY.i.hi + 8;
        prim->drawMode = DRAW_HIDE | DRAW_UNK02;
        prim->priority = self->zPriority + 3;
        PANTHER(self)->quadA = prim;
        prim = prim->next;

        PantherQuad(prim, 0x40, 0x7F, 0xC8, 0xC9);
        prim->y0 = prim->y1 = prim->y2 = prim->y3 = self->posY.i.hi + 8;
        prim->drawMode = DRAW_HIDE | DRAW_UNK02;
        prim->priority = self->zPriority + 2;
        PANTHER(self)->quadB = prim;
        prim = prim->next;

        PantherSpark(prim, -0x28, 8, 8);
        prim->u0 = prim->u2 = 0x5C;
        prim->u1 = prim->u3 = 0x64;
        prim->v0 = prim->v1 = 0xC0;
        prim->v2 = prim->v3 = 0xC8;
        PANTHER(self)->sparkA = prim;
        prim = prim->next->next;

        PantherSpark(prim, 2, 2, 1);
        prim->u0 = prim->u2 = self->facingLeft ? 0x3C : 0;
        prim->u1 = prim->u3 = self->facingLeft ? 0 : 0x3C;
        prim->v0 = prim->v1 = 0xD0;
        prim->v2 = prim->v3 = 0xDD;
        PANTHER(self)->sparkB = prim;
        PlaySfxPositional(0x672);
        PANTHER(self)->timer = 0x10;
        self->animCurFrame = 0;
        self->step_s++;
        break;
    case 1:
        if (!--PANTHER(self)->timer) {
            PANTHER(self)->phaseStrip = 0;
            PANTHER(self)->phaseA = 0;
            PANTHER(self)->phaseB = 0;
            PANTHER(self)->unk97 = 0;
            PANTHER(self)->unk98 = 0;
            PANTHER(self)->timerA = 0x24;
            PANTHER(self)->timerB = 0x30;
            PANTHER(self)->flicker = 0;
            self->step_s++;
        }
        break;
    case 2:
        PANTHER(self)->timer++;
        switch (PANTHER(self)->phaseStrip) {
        case 0: // gather up
            func_us_801C4B2C();
            prim = PANTHER(self)->strip;
            prim->y0 -= 3;
            prim->y1 -= 3;
            if (PANTHER(self)->timer & 1) {
                prim->x0++;
                prim->x1--;
            }
            if (prim->x1 - prim->x0 < 0x20) {
                PANTHER(self)->timer = 0xC;
                PANTHER(self)->phaseStrip++;
            }
            prim = prim->next;
            for (i = 0; i < 6; i++) {
                prim->x2 += 2;
                prim->x3 -= 2;
                prim = prim->next;
            }
            break;
        case 1: // drop
            func_us_801C4B2C();
            prim = PANTHER(self)->strip;
            prim->y0 += 6;
            prim->y1 += 6;
            prim = prim->next;
            for (i = 0; i < 6; i++) {
                prim->x2++;
                prim->x3--;
                prim = prim->next;
            }
            if (PANTHER(self)->timer & 1) {
                prim = PANTHER(self)->stripLast;
                prim->x2++;
                prim->x3 -= 2;
            }
            prim = PANTHER(self)->strip;
            if (self->posY.i.hi + 8 < prim->y0) {
                for (i = 0; i < 8; i++) {
                    prim->drawMode |= DRAW_HIDE;
                    prim = prim->next;
                }
            }
            break;
        }
        prim = PANTHER(self)->quadA;
        switch (PANTHER(self)->phaseA) {
        case 0:
            if (!--PANTHER(self)->timerA) {
                prim->drawMode &= ~DRAW_HIDE;
                PANTHER(self)->timerA = 0xF;
                PANTHER(self)->phaseA++;
            }
            break;
        case 1:
            prim->v2 = prim->v3 = prim->v2 + 1;
            prim->y0 = prim->y1 = prim->y0 - 2;
            if (!(PANTHER(self)->timer & 3)) {
                prim->x0--;
                prim->x1++;
            }
            if (!--PANTHER(self)->timerA) {
                PANTHER(self)->phaseA++;
            }
            break;
        case 2:
            if (!(PANTHER(self)->timer % 3)) {
                prim->v2 = prim->v3 = prim->v2 - 1;
            }
            prim->y0 = prim->y1 = prim->y0 + 1;
            if (prim->y2 < prim->y0) {
                prim->drawMode |= DRAW_HIDE;
                PANTHER(self)->phaseA = 8;
            }
            break;
        }
        prim = PANTHER(self)->quadB;
        switch (PANTHER(self)->phaseB) {
        case 0:
            if (!--PANTHER(self)->timerB) {
                prim->drawMode &= ~DRAW_HIDE;
                PANTHER(self)->phaseB++;
            }
            break;
        case 1:
            prim->v2 = prim->v3 = prim->v2 + 4;
            prim->y0 = prim->y1 = prim->y0 - 4;
            if (prim->v2 >= 0xF9) {
                prim = PANTHER(self)->sparkA;
                prim->drawMode &= ~DRAW_HIDE;
                LOW(prim->next->r1) = FIX(-1);
                PANTHER(self)->sparkB->drawMode &= ~DRAW_HIDE;
                PANTHER(self)->phaseB++;
            }
            break;
        case 2:
            prim->y0 = prim->y1 = prim->y0 + 1;
            if (prim->y0 == prim->y3) {
                PANTHER(self)->timer = 1;
                PANTHER(self)->flicker = 0;
                prim->drawMode |= DRAW_HIDE;
                self->step_s++;
            }
            prim = PANTHER(self)->sparkA;
            UnkPrimHelper(prim);
            LOW(prim->next->r1) += FIX(1.0 / 16);
            prim = PANTHER(self)->sparkB;
            UnkPrimHelper(prim);
            PantherSparkGrow(prim, 2);
            break;
        }
        break;
    case 3:
        PANTHER(self)->timer++;
        prim = PANTHER(self)->sparkB;
        UnkPrimHelper(prim);
        if (!(PANTHER(self)->timer & 3)) {
            PANTHER(self)->flicker ^= 1;
        }
        PantherSparkGrow(prim, 1);
        prim = PANTHER(self)->sparkA;
        UnkPrimHelper(prim);
        LOW(prim->next->r1) += FIX(1.0 / 16);
        if (self->posY.i.hi + 6 < prim->next->y0) {
            prim->drawMode |= DRAW_HIDE;
            self->step_s++;
        }
        break;
    }
}
#else
INCLUDE_ASM("st/rdai/nonmatchings/e_black_panther", func_us_801C4B2C);

INCLUDE_ASM("st/rdai/nonmatchings/e_black_panther", func_us_801C4C4C);
#endif

#ifdef VERSION_PC
// Reform from the pounce effect back into the panther
void func_us_801C5AA0(void) {
    Entity* self = g_CurrentEntity;
    Primitive* prim;
    u8 wasFacing;
    s32 i;

    switch (self->step_s) {
    case 0:
        wasFacing = self->facingLeft;
        self->facingLeft = (GetSideToPlayer() & 1) ^ 1;
        if (wasFacing != self->facingLeft) {
            self->posX.i.hi += self->facingLeft ? 0x18 : -0x18;
        }
        prim = PANTHER(self)->strip;
        for (i = 0; i < 8; i++) {
            PantherQuad(prim, self->facingLeft ? 0x40 : 0,
                        self->facingLeft ? 0 : 0x40, 0xB5 + i * 2,
                        0xB6 + i * 2);
            prim->y0 = prim->y1 = prim->y2 = prim->y3 = self->posY.i.hi + 8;
            prim->drawMode = DRAW_UNK02;
            prim->priority = self->zPriority + 2;
            prim = prim->next;
        }
        func_us_801C4B2C();
        PlaySfxPositional(0x672);
        PANTHER(self)->timerA = 8;
        self->step_s++;
        break;
    case 1:
        prim = PANTHER(self)->sparkB;
        prim->y0 -= 2;
        prim->y1 -= 2;
        if (!--PANTHER(self)->timerA) {
            PANTHER(self)->timerA = 0x10;
            self->step_s++;
        }
        break;
    case 2:
        prim = PANTHER(self)->sparkB;
        prim->y0--;
        prim->y1--;
        prim = PANTHER(self)->strip;
        prim->y0 -= 4;
        prim->y1 -= 4;
        func_us_801C4B2C();
        if (!--PANTHER(self)->timerA) {
            PANTHER(self)->timerA = 0xF;
            self->step_s++;
        }
        break;
    case 3:
        prim = PANTHER(self)->sparkB;
        prim->y0 += 2;
        prim->y1 += 2;
        if (self->posY.i.hi < prim->y0) {
            prim->drawMode |= DRAW_HIDE;
        }
        prim = PANTHER(self)->strip;
        prim->y0 -= 2;
        prim->y1 -= 2;
        func_us_801C4B2C();
        if (!--PANTHER(self)->timerA) {
            PANTHER(self)->timerA = 0xF;
            self->step_s++;
        }
        break;
    case 4:
        prim = PANTHER(self)->sparkB;
        prim->y0 += 2;
        prim->y1 += 2;
        if (self->posY.i.hi < prim->y0) {
            prim->drawMode |= DRAW_HIDE;
        }
        prim = PANTHER(self)->strip;
        prim->y0 += 2;
        prim->y1 += 2;
        func_us_801C4B2C();
        if (self->posY.i.hi - 6 < prim->y0) {
            self->step_s++;
        }
        break;
    case 5:
        self->animCurFrame = 1;
        PANTHER(self)->unk98 = 0x18;
        self->hitEffect = self->palette;
        self->palette = 0x8164;
        g_api.FreePrimitives(self->primIndex);
        self->flags &= ~FLAG_HAS_PRIMS;
        self->step_s++;
        break;
    case 6:
        if (!--PANTHER(self)->unk98) {
            self->hitboxState = 3;
            self->palette = self->hitEffect;
            self->step_s++;
        }
        break;
    }
}

extern EInit g_EInitBlackPanther;

static s16 panther_wall_sensors[] = {-64, 0, -64, 16, 0xFF, 0};
static s16 panther_floor_sensors[] = {0, 8, 0, 4, 8, -4, -16, 0};
static s16 panther_floor_sensors_run[] = {0, 8, 16, 0};
static u8 anim_panther_run[] = {6, 2, 3, 3, 5, 4, 3, 3, 3, 6,
                                3, 7, 2, 8, 2, 9, 2, 10, 0, 0};
static u8 anim_panther_jump[] = {3, 2, 2, 3, 3, 4, 4, 5, 2, 3, 0xFF, 0};

static bool PantherHitsWall(Entity* self) {
    Collider collider;
    s16 x = self->facingLeft ? self->posX.i.hi + 0x18 : self->posX.i.hi - 0x18;

    g_api.CheckCollision(x, self->posY.i.hi, &collider, 0);
    return collider.effects != 0;
}

void EntityBlackPanther(Entity* self) {
    Entity* entity;
    Primitive* prim;

    if ((self->flags & FLAG_DEAD) && self->step < 0x10) {
        self->hitboxState = 0;
        SetStep(0x10);
    }
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitBlackPanther);
        self->hitboxOffX = 2;
        self->hitboxOffY = -4;
        self->animCurFrame = 1;
        // fallthrough
    case 1: // fall to the floor
        if (UnkCollisionFunc3(panther_floor_sensors) & 1) {
            self->step++;
        }
        break;
    case 2: // wait for the player
        self->animCurFrame = 1;
        self->facingLeft = (GetSideToPlayer() & 1) ^ 1;
        if (GetDistanceToPlayerX() < 0x70) {
            self->step++;
        }
        break;
    case 3: // run
        UnkCollisionFunc2(panther_floor_sensors_run);
        self->velocityX = self->facingLeft ? FIX(2.5) : FIX(-2.5);
        if (!AnimateEntity(anim_panther_run, self)) {
            PlaySfxPositional(0x649);
            if (CheckColliderOffsets(
                    panther_wall_sensors, self->facingLeft ^ 1) != 1) {
                SetStep(4);
            }
        }
        if ((GetSideToPlayer() & 1) != PLAYER.facingLeft &&
            GetDistanceToPlayerX() < 0x50 && (g_Player.status & 0x1C00)) {
            SetStep(7);
        }
        if (PantherHitsWall(self)) {
            SetStep(6);
        }
        break;
    case 4: // leap over a gap
        switch (self->step_s) {
        case 0:
            self->velocityX = self->facingLeft ? FIX(2) : FIX(-2);
            self->velocityY = FIX(-3.5);
            self->step_s++;
            break;
        case 1:
            AnimateEntity(anim_panther_jump, self);
            if (UnkCollisionFunc3(panther_floor_sensors) & 1) {
                self->step_s++;
            }
            if (PantherHitsWall(self)) {
                SetStep(6);
            }
            break;
        case 2:
            self->facingLeft = (GetSideToPlayer() & 1) ^ 1;
            SetStep(3);
            break;
        }
        break;
    case 5: // hop up
        switch (self->step_s) {
        case 0:
            self->animCurFrame = 9;
            self->velocityX = 0;
            self->velocityY = FIX(-4);
            self->step_s++;
            break;
        case 1:
            if (UnkCollisionFunc3(panther_floor_sensors) & 1) {
                self->facingLeft = (GetSideToPlayer() & 1) ^ 1;
                SetStep(3);
            }
            break;
        }
        break;
    case 6: // somersault back off a wall
        if (!self->step_s) {
            self->animCurFrame = 0xB;
            self->velocityX = self->facingLeft ? FIX(2.5) : FIX(-2.5);
            self->velocityY = FIX(-3);
            self->rotate = 0;
            self->drawFlags |= ENTITY_ROTATE;
            self->step_s++;
        }
        self->rotate += 0x100;
        if (UnkCollisionFunc3(panther_floor_sensors) & 1) {
            self->drawFlags &= ~ENTITY_ROTATE;
            SetStep(3);
        } else {
            self->velocityY -= FIX(1.0 / 16);
        }
        break;
    case 7:
        func_us_801C4C4C();
        if (self->step_s == 4) {
            SetStep(8);
        }
        break;
    case 8: // dash as a shadow
        if (!self->step_s) {
            self->velocityX = self->facingLeft ? FIX(4) : FIX(-4);
            PANTHER(self)->timer = 0x30;
            self->step_s++;
            break;
        }
        UnkCollisionFunc2(panther_floor_sensors_run);
        prim = PANTHER(self)->sparkB;
        prim->next->x1 = self->facingLeft ? self->posX.i.hi - 0x12
                                          : self->posX.i.hi + 0x12;
        prim->next->y0 = self->posY.i.hi + 2;
        UnkPrimHelper(prim);
        if (!--PANTHER(self)->timer) {
            SetStep(9);
        }
        break;
    case 9:
        func_us_801C5AA0();
        if (self->step_s == 7) {
            SetStep(3);
        }
        break;
    case 0x10:
        switch (self->step_s) {
        case 0:
            self->animCurFrame = 0xB;
            self->rotate = 0;
            self->drawFlags |= ENTITY_ROTATE;
            self->velocityX = self->facingLeft ? FIX(2) : FIX(-2);
            self->velocityY = FIX(-3.5);
            self->step_s++;
            break;
        case 1:
            self->rotate -= 0x80;
            if (UnkCollisionFunc3(panther_floor_sensors) & 1) {
                self->step_s++;
            }
            break;
        case 2:
            entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
            if (entity != NULL) {
                CreateEntityFromEntity(E_EXPLOSION, self, entity);
                entity->params = 2;
            }
            PlaySfxPositional(0x65E);
            DestroyEntity(self);
            return;
        }
        break;
    case 0x20:
#include "../pad2_anim_debug.h"
    }

    if (self->animCurFrame == 1) {
        self->hitboxOffX = 9;
        self->hitboxOffY = 2;
        self->hitboxWidth = 0x11;
        self->hitboxHeight = 4;
    }
    if (self->animCurFrame == 2) {
        self->hitboxOffX = 5;
        self->hitboxOffY = -5;
        self->hitboxWidth = 0x10;
        self->hitboxHeight = 0xA;
    }
    if (self->animCurFrame >= 3) {
        self->hitboxOffX = 4;
        self->hitboxOffY = -0xC;
        self->hitboxWidth = 0x17;
        self->hitboxHeight = 6;
    }
    if (self->animCurFrame >= 6) {
        self->hitboxOffX = -2;
        self->hitboxOffY = -4;
        self->hitboxWidth = 0xB;
        self->hitboxHeight = 0xB;
    }
}
#else
INCLUDE_ASM("st/rdai/nonmatchings/e_black_panther", func_us_801C5AA0);

INCLUDE_ASM("st/rdai/nonmatchings/e_black_panther", EntityBlackPanther);
#endif
