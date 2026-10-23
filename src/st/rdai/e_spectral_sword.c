// SPDX-License-Identifier: AGPL-3.0-or-later
#include "rdai.h"

#include "../step_towards.h"

#ifdef VERSION_PC
// Non-matching C for the PC port, transcribed from the US binary.
// RDAI's Spectral Sword keeps six small swords (E_UNK_24) and four large
// ones (E_UNK_25) in the entity slots right after it; it aims them at the
// player and orders them to strike through RdaiSwordExt.command.
extern EInit g_EInitSpectralSwordRDAI;
extern EInit g_EInitSpear;
extern EInit g_EInitShield;

typedef struct {
    /* 0x7C */ Primitive* prim;      // sword: next destination marker
    /* 0x80 */ s16 timer;            // sword: travel time; children: wait
    /* 0x82 */ s16 sweepTimer;       // sword: frames spent near the player
    /* 0x84 */ u8 side;              // sword: side of the player it flies on
    /* 0x85 */ u8 : 8;
    /* 0x86 */ u8 smallCount;        // sword: small swords still alive
    /* 0x87 */ u8 largeCount;        // sword: large swords still alive
    /* 0x88 */ Pos weaponPos;        // children: formation position
    /* 0x90 */ s16 angle;            // children: strike direction
    /* 0x92 */ s16 : 16;
    /* 0x94 */ u8 command;           // children: strike order, see below
    /* 0x95 */ u8 : 8;
    /* 0x96 */ s16 attackTimer;      // sword: next order; small: delay
} RdaiSwordExt;
#define SWORD(e) ((RdaiSwordExt*)&(e)->ext)

#define SMALL_SWORDS 6
#define LARGE_SWORDS 4

static u8 small_sword_kind[] = {0, 1, 0, 0, 1, 0};
static u8 anim_large_strike[] = {4, 8, 4, 7, 16, 6, 0xFF, 0};
static u8 anim_large_strike_slow[] = {12, 8, 12, 7, 32, 6, 0xFF, 0};
static u8 anim_large_ready[] = {32, 6, 16, 7, 16, 8, 0xFF, 0};

void func_us_801C0C44(Entity* self) {
    Entity* entity;
    Primitive* prim;
    s32 primIndex;
    s32 i, n;
    s32 dx, dy;
    s32 dist;
    s32 posX, posY;
    s32 offset;
    s16 angle;

    if ((self->flags & FLAG_DEAD) && self->step < 8) {
        SetStep(8);
    }
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitSpectralSwordRDAI);
        self->animCurFrame = 1;
        self->blendMode = BLEND_TRANSP | BLEND_ADD;
        self->hitboxOffX = 0;
        self->hitboxOffY = 4;
        primIndex = g_api.AllocPrimitives(PRIM_TILE, 1);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->flags |= FLAG_HAS_PRIMS;
        self->primIndex = primIndex;
        prim = &g_PrimBuf[primIndex];
        SWORD(self)->prim = prim;
        prim->u0 = prim->v0 = 2;
        PGREY(prim, 0) = 192;
        prim->priority = 192;
        prim->drawMode = DRAW_HIDE | DRAW_UNK02;
        entity = self + 1;
        for (i = 0; i < SMALL_SWORDS; i++, entity++) {
            CreateEntityFromCurrentEntity(E_UNK_24, entity);
            entity->params = i | (small_sword_kind[i] << 8);
        }
        for (i = 0; i < LARGE_SWORDS; i++, entity++) {
            CreateEntityFromCurrentEntity(E_UNK_25, entity);
            entity->params = i;
        }
        SetStep(2);
        SWORD(self)->sweepTimer = 0x40;
        SWORD(self)->attackTimer = 0x40;
        SWORD(self)->smallCount = SMALL_SWORDS;
        SWORD(self)->largeCount = LARGE_SWORDS;
        break;
    case 2:
        if (GetDistanceToPlayerX() < 0x60) {
            SetStep(3);
        }
        break;
    case 3:
        SetStep(6);
        SWORD(self)->side = GetSideToPlayer() & 1;
        break;
    case 6: // fly around the player
        if (!self->step_s) {
            angle = Random() * 2;
            angle += SWORD(self)->side ? 0x100 : 0x500;
            posX = (rcos(angle) * 96) >> 12;
            posY = -(rsin(angle) * 96) >> 12;
            prim = SWORD(self)->prim;
            prim->x0 = PLAYER.posX.i.hi + posX;
            prim->y0 = PLAYER.posY.i.hi + posY;
            posX = prim->x0;
            posY = prim->y0;
            angle = ratan2(posY - self->posY.i.hi, posX - self->posX.i.hi);
            self->velocityX = rcos(angle) * 12;
            self->velocityY = rsin(angle) * 12;
            SWORD(self)->timer = 0x80;
            self->step_s++;
        }
        MoveEntity();
        prim = SWORD(self)->prim;
        dx = abs(prim->x0 - self->posX.i.hi);
        dy = abs(prim->y0 - self->posY.i.hi);
        if ((dx < 8 && dy < 8) || !--SWORD(self)->timer) {
            self->step_s = 0;
        }
        dx = PLAYER.posX.i.hi - self->posX.i.hi;
        if (SWORD(self)->side) {
            dx = -dx;
        }
        if (dx > 0x80) {
            self->velocityY = 0;
            self->velocityX = SWORD(self)->side ? FIX(-0.75) : FIX(0.75);
        }
        if (dx < 0x40) {
            self->velocityY = 0;
            self->velocityX = SWORD(self)->side ? FIX(0.75) : FIX(-0.75);
            if (!--SWORD(self)->sweepTimer) {
                SetStep(5);
            }
        } else {
            SWORD(self)->sweepTimer = 0x40;
        }
        if (!SWORD(self)->smallCount || --SWORD(self)->attackTimer) {
            break;
        }
        switch (Random() & 3) {
        case 0:
        case 1: // one random small sword strikes
            SWORD(self)->attackTimer = 0x100;
            n = Random() % SWORD(self)->smallCount + 1;
            entity = self + 1;
            for (i = 0; i < SMALL_SWORDS; i++, entity++) {
                if (!(entity->flags & FLAG_DEAD)) {
                    n--;
                }
                if (!n) {
                    break;
                }
            }
            SWORD(entity)->command = 1;
            break;
        case 2: // all small swords strike together
            SWORD(self)->attackTimer = 0x180;
            angle = GetAngleBetweenEntities(self, &PLAYER);
            entity = self + 1;
            for (i = 0; i < SMALL_SWORDS; i++, entity++) {
                SWORD(entity)->angle = angle;
                SWORD(entity)->command = 3;
            }
            break;
        case 3: // small swords strike one after another
            SWORD(self)->attackTimer = 0x280;
            offset = 0x40;
            entity = self + 1;
            for (i = 0; i < SMALL_SWORDS; i++, entity++) {
                if (!(entity->flags & FLAG_DEAD)) {
                    SWORD(entity)->attackTimer = offset;
                    offset += 0x40;
                    SWORD(entity)->command = 5;
                }
            }
            break;
        }
        break;
    case 5: // cross over to the other side of the player
        switch (self->step_s) {
        case 0:
            angle = ratan2(0x40 - self->posY.i.hi, 0x80 - self->posX.i.hi);
            self->velocityX = rcos(angle) * 48;
            self->velocityY = rsin(angle) * 48;
            SWORD(self)->side ^= 1;
            self->step_s++;
            break;
        case 1:
            MoveEntity();
            posX = self->posX.i.hi;
            if (SWORD(self)->side) {
                if (posX > 0x80) {
                    self->velocityX = FIX(3);
                    self->step_s++;
                }
            } else if (posX < 0x80) {
                self->velocityX = FIX(-3);
                self->step_s++;
            }
            break;
        case 2:
            MoveEntity();
            self->velocityY += FIX(0.125);
            dx = PLAYER.posX.i.hi - self->posX.i.hi;
            if (SWORD(self)->side) {
                dx = -dx;
            }
            if (dx > 0x70) {
                SetStep(6);
            }
            break;
        }
        break;
    case 8:
        switch (self->step_s) {
        case 0:
            self->hitboxState = 0;
            entity = self + 1;
            for (i = 0; i < SMALL_SWORDS + LARGE_SWORDS; i++, entity++) {
                entity->flags |= FLAG_DEAD;
            }
            self->drawFlags |= ENTITY_ROTATE;
            self->rotate = 0;
            PlaySfxPositional(0x697);
            self->step_s++;
            break;
        case 1:
            MoveEntity();
            self->velocityY += FIX(0.09375);
            self->rotate -= 4;
            if (!(g_Timer & 7)) {
                entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
                if (entity != NULL) {
                    CreateEntityFromEntity(E_EXPLOSION, self, entity);
                    entity->params = 3;
                }
            }
            if (self->posY.i.hi > 0x160) {
                self->animCurFrame = 0;
                self->step_s++;
            }
            break;
        case 2:
            n = 0;
            entity = self + 1;
            for (i = 0; i < SMALL_SWORDS + LARGE_SWORDS; i++, entity++) {
                if (!entity->animCurFrame) {
                    n++;
                }
            }
            if (n == SMALL_SWORDS + LARGE_SWORDS) {
                entity = self;
                for (i = 0; i < 1 + SMALL_SWORDS + LARGE_SWORDS;
                     i++, entity++) {
                    DestroyEntity(entity);
                }
                return;
            }
            break;
        }
        break;
    }

    if (self->flags & FLAG_DEAD) {
        return;
    }
    // keep the formation: small swords in a row across the line to the
    // player, large swords in a row further along it
    dx = PLAYER.posX.i.hi - self->posX.i.hi;
    dy = PLAYER.posY.i.hi - self->posY.i.hi;
    angle = ratan2(dy, dx);
    dist = SquareRoot0(dx * dx + dy * dy);
    posX = self->posX.val;
    posY = self->posY.val;
    offset = -0x30;
    entity = self + 1;
    for (i = 0; i < SMALL_SWORDS; i++, entity++) {
        SWORD(entity)->weaponPos.x.val =
            posX + ((offset * rcos(angle + 0x400)) << 4);
        SWORD(entity)->weaponPos.y.val =
            posY + ((offset * rsin(angle + 0x400)) << 4);
        offset += 0x10;
        if (!offset) {
            offset = 0x10;
        }
    }
    dist /= 2;
    if (dist < 0x20) {
        dist = 0x20;
    }
    if (dist > 0x48) {
        dist = 0x48;
    }
    posX += (dist * rcos(angle)) << 4;
    posY += (dist * rsin(angle)) << 4;
    offset = -(SWORD(self)->largeCount >> 1) * 24;
    for (i = 0; i < LARGE_SWORDS; i++, entity++) {
        if (!(entity->flags & FLAG_DEAD)) {
            SWORD(entity)->angle = angle;
            SWORD(entity)->weaponPos.x.val =
                posX + ((offset * rcos(angle + 0x400)) << 4);
            SWORD(entity)->weaponPos.y.val =
                posY + ((offset * rsin(angle + 0x400)) << 4);
            offset += 0x18;
        }
    }
    if (!(self->flags & 0xF)) {
        self->palette = (g_Timer & 1) ? 0x215 : 0x216;
    }
    if (!(g_Timer & 0xF)) {
        entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
        if (entity != NULL) {
            CreateEntityFromEntity(E_UNK_23, self, entity);
            entity->ext.et_801CEB08.unk80 = self;
            entity->zPriority = self->zPriority + 1;
        }
    }
}
#else
INCLUDE_ASM("st/rdai/nonmatchings/e_spectral_sword", func_us_801C0C44);
#endif

#include "../e_spectral_sword_aura.h"

#ifdef VERSION_PC
// Small sword. command bits: 1 strike, 2 strike in formation (the matching
// large sword follows), 4 wait attackTimer frames first.
void func_us_801C17E8(Entity* self) {
    Entity* entity;
    s32 dx, dy;
    s32 dist;
    s32 side;
    s32 radius;
    s16 angle;

    if ((self->flags & FLAG_DEAD) && self->step < 8) {
        SetStep(8);
    }
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitSpear);
        self->drawFlags = ENTITY_ROTATE;
        self->animCurFrame = (self->params & 0x100) ? 4 : 5;
        SetStep(2);
        break;
    case 2: // follow the formation
        MoveEntity();
        dx = SWORD(self)->weaponPos.x.i.hi - self->posX.i.hi;
        dy = SWORD(self)->weaponPos.y.i.hi - self->posY.i.hi;
        dist = SquareRoot0(dx * dx + dy * dy);
        if (dist > 0x38) {
            dist = 0x38;
        }
        angle = ratan2(dy, dx);
        self->velocityX = dist * rcos(angle);
        self->velocityY = dist * rsin(angle);
        StepTowards(&self->rotate,
                    (s16)(GetAngleBetweenEntities(self, &PLAYER) - 0x400),
                    0x10);
        if (SWORD(self)->command) {
            if (!(SWORD(self)->command & 4) || !--SWORD(self)->attackTimer) {
                SetStep(4);
            }
        }
        break;
    case 4: // strike
        switch (self->step_s) {
        case 0:
            if (!(SWORD(self)->command & 2)) {
                SWORD(self)->angle = GetAngleBetweenEntities(self, &PLAYER);
            }
            // wind up away from the target
            self->velocityX = rcos(SWORD(self)->angle - 0x800) << 5;
            self->velocityY = rsin(SWORD(self)->angle - 0x800) << 5;
            // the large sword next to this one in the formation follows
            entity = self - (self->params & 0xFF) + SMALL_SWORDS +
                     ((self->params & 0xFF) * 4) / 6;
            SWORD(entity)->command = (SWORD(self)->command & 2) ? 2 : 1;
            self->step_s++;
            break;
        case 1:
            MoveEntity();
            self->velocityX -= self->velocityX / 16;
            self->velocityY -= self->velocityY / 16;
            if (abs(self->velocityX) < FIX(0.125) &&
                abs(self->velocityY) < FIX(0.125)) {
                self->step_s++;
            }
            StepTowards(
                &self->rotate,
                (s16)(ratan2(self->velocityY, self->velocityX) - 0x400), 8);
            break;
        case 2:
            angle = SWORD(self)->angle;
            if (StepTowards(&self->rotate, (s16)(angle - 0x400), 0x20)) {
                self->velocityX = rcos(angle) << 7;
                self->velocityY = rsin(angle) << 7;
                SWORD(self)->timer = 0x30;
                PlaySfxPositional(0x60A);
                self->step_s++;
            }
            break;
        case 3:
            MoveEntity();
            side = GetSideToPlayer();
            if (abs(self->velocityY) < abs(self->velocityX)) {
                side &= 1;
                if (self->velocityX < 0) {
                    side ^= 1;
                }
            } else {
                side &= 2;
                if (self->velocityY < 0) {
                    side ^= 2;
                }
            }
            if (side || !--SWORD(self)->timer) {
                SWORD(self)->command = 0;
                SetStep(2);
            }
            break;
        }
        break;
    case 8:
        switch (self->step_s) {
        case 0:
            entity = self - (self->params & 0xFF) - 1;
            SWORD(entity)->smallCount--;
            self->hitboxState = 0;
            PlaySfxPositional(0x683);
            self->step_s++;
            // fallthrough
        case 1:
            MoveEntity();
            self->velocityY += FIX(0.125);
            if (!(g_Timer & 7)) {
                entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
                if (entity != NULL) {
                    CreateEntityFromEntity(E_EXPLOSION, self, entity);
                    entity->params = 1;
                }
            }
            if (self->posY.i.hi > 0x140) {
                self->animCurFrame = 0;
                self->step_s++;
            }
            break;
        case 2:
            self->animCurFrame = 0;
            break;
        }
        break;
    }
    radius = (self->params & 0x100) ? 0x18 : 0x16;
    angle = self->rotate + 0x400;
    self->hitboxOffX = (radius * rcos(angle)) >> 12;
    self->hitboxOffY = (radius * rsin(angle)) >> 12;
}

// Large sword: strikes when the small sword next to it does.
void func_us_801C1DE8(Entity* self) {
    Entity* entity;
    s32 dx, dy;
    s32 dist;
    s16 angle;

    if ((self->flags & FLAG_DEAD) && self->step < 8) {
        SetStep(8);
    }
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitShield);
        self->animCurFrame = 7;
        self->zPriority += 2;
        SetStep(3);
        break;
    case 2:
        self->facingLeft = (GetSideToPlayer() & 1) ^ 1;
        dist = GetDistanceToPlayerX();
        self->animCurFrame = 8;
        if (dist < 0x40) {
            self->animCurFrame = 7;
        }
        if (dist < 0x20) {
            self->animCurFrame = 6;
        }
        break;
    case 3: // follow the formation
        MoveEntity();
        if (self->animCurFrame != 8) {
            AnimateEntity(anim_large_ready, self);
        }
        dx = SWORD(self)->weaponPos.x.i.hi - self->posX.i.hi;
        dy = SWORD(self)->weaponPos.y.i.hi - self->posY.i.hi;
        dist = SquareRoot0(dx * dx + dy * dy);
        if (dist > 0x38) {
            dist = 0x38;
        }
        angle = ratan2(dy, dx);
        self->velocityX = dist * rcos(angle);
        self->velocityY = dist * rsin(angle);
        if (self->animCurFrame != 6) {
            self->facingLeft = (GetSideToPlayer() & 1) ^ 1;
        }
        if (SWORD(self)->command) {
            SetStep(4);
        }
        break;
    case 4:
        switch (self->step_s) {
        case 0:
            self->velocityX = rcos(SWORD(self)->angle - 0x800) << 5;
            self->velocityY = rsin(SWORD(self)->angle - 0x800) << 5;
            self->step_s++;
            // fallthrough
        case 1:
            MoveEntity();
            if (!AnimateEntity((SWORD(self)->command & 2)
                                   ? anim_large_strike_slow
                                   : anim_large_strike,
                               self)) {
                SWORD(self)->timer = 0x20;
                self->step_s++;
            }
            break;
        case 2:
            if (!--SWORD(self)->timer) {
                SWORD(self)->command = 0;
                SetStep(3);
            }
            break;
        }
        break;
    case 8:
        switch (self->step_s) {
        case 0:
            entity = self - self->params - 1 - SMALL_SWORDS;
            SWORD(entity)->largeCount--;
            self->hitboxState = 0;
            PlaySfxPositional(0x60F);
            PlaySfxPositional(0x683);
            self->step_s++;
            // fallthrough
        case 1:
            MoveEntity();
            self->velocityY += FIX(0.125);
            if (!(g_Timer & 7)) {
                entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
                if (entity != NULL) {
                    CreateEntityFromEntity(E_EXPLOSION, self, entity);
                    entity->params = 1;
                }
            }
            if (self->posY.i.hi > 0x140) {
                self->animCurFrame = 0;
                self->step_s++;
            }
            break;
        case 2:
            self->animCurFrame = 0;
            break;
        }
        break;
    }
}
#else
INCLUDE_ASM("st/rdai/nonmatchings/e_spectral_sword", func_us_801C17E8);

INCLUDE_ASM("st/rdai/nonmatchings/e_spectral_sword", func_us_801C1DE8);
#endif
