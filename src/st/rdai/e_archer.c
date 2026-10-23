// SPDX-License-Identifier: AGPL-3.0-or-later
#include "rdai.h"

s32 SpearGuardCheckCollision(void) {
    Collider collider;
    s32 x;
    s32 y;

    g_CurrentEntity->posX.val += g_CurrentEntity->velocityX;
    g_CurrentEntity->posY.i.hi -= 3;
    x = g_CurrentEntity->posX.i.hi;
    y = g_CurrentEntity->posY.i.hi - 0x1E;
    g_api.CheckCollision(x, y, &collider, 0);

    if (collider.effects & EFFECT_SOLID) {
        y = (g_CurrentEntity->posY.i.hi += collider.unk20);
    } else {
        return false;
    }

    if (g_CurrentEntity->velocityX == 0) {
        return true;
    }

    if (g_CurrentEntity->velocityX > 0) {
        x += 0x20;
        y -= 0x24;
    } else {
        x -= 0x20;
        y += 0x1C;
    }
    g_api.CheckCollision(x, y, &collider, 0);
    if (!(collider.effects & EFFECT_UNK_8000)) {
        return false;
    }
    return true;
}

#include "../approach_s16.h"

#ifdef VERSION_PC
// Non-matching C for the PC port, transcribed from the US binary.
extern EInit g_EInitRdaiUnk1F;

typedef struct {
    /* 0x7C */ s32 : 32;
    /* 0x80 */ s16 timer;
    /* 0x82 */ s16 : 16;
    /* 0x84 */ u8 reverse;
} RdaiCartExt;
#define CART(e) ((RdaiCartExt*)&(e)->ext)

static u8 anim_cart_roll[] = {8, 0x1E, 8, 0x1F, 8, 0x20, 0, 0};
static u8 anim_cart_turn_left[] = {
    1, 0x1F, 2, 0x21, 16, 0x22, 6, 0x21, 1, 0x1F, 0xFF, 0};
static u8 anim_cart_turn_right[] = {
    1, 0x1F, 2, 0x23, 16, 0x24, 6, 0x23, 1, 0x1F, 0xFF, 0};

static void CartAccelerate(Entity* self) {
    if (!CART(self)->reverse) {
        self->velocityX += FIX(1.0 / 64);
        if (self->velocityX > FIX(0.75)) {
            self->velocityX = FIX(0.75);
        }
    } else {
        self->velocityX -= FIX(1.0 / 64);
        if (self->velocityX < FIX(-0.75)) {
            self->velocityX = FIX(-0.75);
        }
    }
}

// The cart the RDAI archer rides: three wheel/body parts (E_UNK_1F) and
// the archer (E_UNK_1C) sit in the four entity slots after it.
void func_us_801BF830(Entity* self) {
    Entity* entity;
    s32 i;
    s32 velocity;

    if (self->step && (self + 4)->entityId != E_UNK_1C && self->step != 3) {
        SetStep(3);
    }
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitRdaiUnk1F);
        entity = self + 1;
        for (i = 0; i < 3; i++, entity++) {
            CreateEntityFromEntity(E_UNK_1F, self, entity);
            entity->params = i;
        }
        CreateEntityFromEntity(E_UNK_1C, self, self + 4);
        SetStep(1);
        break;
    case 1:
        if (!self->step_s) {
            CART(self)->timer = 0x80;
            self->step_s++;
        }
        CartAccelerate(self);
        AnimateEntity(anim_cart_roll, self);
        SpearGuardCheckCollision();
        if (!(g_Timer & 7)) {
            PlaySfxPositional(0x76A);
        }
        if (!--CART(self)->timer) {
            SetStep(2);
        }
        break;
    case 2:
        switch (self->step_s) {
        case 0: // brake
            SpearGuardCheckCollision();
            velocity = self->velocityX - (self->velocityX >> 8);
            self->velocityX = velocity;
            if (abs(velocity) < FIX(0.5)) {
                self->velocityX = 0;
                CART(self)->reverse ^= 1;
                self->step_s++;
            }
            break;
        case 2:
            CartAccelerate(self);
            // fallthrough
        case 1: // turn around
            SpearGuardCheckCollision();
            i = AnimateEntity(CART(self)->reverse ? anim_cart_turn_left
                                                  : anim_cart_turn_right,
                              self);
            if (self->pose >= 3) {
                self->step_s = 2;
            }
            if (!i) {
                SetStep(1);
            }
            break;
        }
        break;
    case 3:
        PlaySfxPositional(0x6CA);
        for (i = 0; i < 9; i++) {
            if (i == 7) {
                continue;
            }
            entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
            if (entity != NULL) {
                CreateEntityFromEntity(E_UNK_21, self, entity);
                entity->params = i + 1;
            }
        }
        PreventEntityFromRespawning(self);
        DestroyEntity(self);
        break;
    case 0xFF:
#include "../pad2_anim_debug.h"
    }
}
#else
INCLUDE_ASM("st/rdai/nonmatchings/e_archer", func_us_801BF830);
#endif

// Child parts share one implementation; only the fixed EInit address is local.
#include "../e_rdai_unk1f.h"

#include "../e_rdai_archer.h"

#ifdef VERSION_PC
extern EInit D_us_80180884;

typedef struct {
    /* 0x7C */ s32 : 32;
    /* 0x80 */ s16 timer;
    /* 0x82 */ s16 : 16;
    /* 0x84 */ s32 pad84[4];
    /* 0x94 */ s16 stuckX; // offset from the player it is stuck in
    /* 0x96 */ s16 stuckY;
    /* 0x98 */ s32 : 32;
    /* 0x9C */ Entity* owner;
} RdaiArrowExt;
#define ARROW(e) ((RdaiArrowExt*)&(e)->ext)

static u8 anim_part3_draw[] = {22, 0x0B, 8, 0x0C, 16, 0x0D, 0xFF, 0};
static u8 anim_part1_draw[] = {22, 0x19, 8, 0x1A, 16, 0x1B, 0xFF, 0};
static u8 anim_part3_shoot[] = {
    15, 0x0D, 5, 0x0C, 5, 0x0E, 5, 0x0F, 2, 0x0E, 2, 0x0F, 2, 0x0B, 0xFF, 0};
static u8 anim_part1_shoot[] = {
    22, 0x19, 6, 0x10, 6, 0x11, 22, 0x12, 6, 0x11, 6, 0x10, 22, 0x19, 0xFF, 0};

#define POSE_AND_TIMER(e) (*(s32*)&(e)->pose)

static void ArcherPartFollow(Entity* self, Entity* archer, s16 zOffset) {
    self->posX.i.hi = archer->posX.i.hi;
    self->posY.i.hi = archer->posY.i.hi;
    self->rotate = archer->rotate;
    self->zPriority = archer->zPriority + zOffset;
}

static void ArcherPartRest(Entity* self, s16 frame) {
    self->animCurFrame = frame;
    self->pose = 0;
    self->poseTimer = 0;
}

static void ArcherSpawnArrow(Entity* self) {
    Entity* entity = AllocEntity(&g_Entities[160], &g_Entities[192]);
    if (entity != NULL) {
        CreateEntityFromEntity(E_UNK_20, self, entity);
        ARROW(entity)->owner = self;
        entity->zPriority = self->zPriority - 1;
    }
}

// Bow arm (params 1), body (params 2) and bow string (params 3) of the
// archer, which sits params slots before; they mirror its mode.
void func_us_801C0240(Entity* self) {
    Entity* archer;

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitRdaiUnk1F);
        self->drawFlags = ENTITY_ROTATE;
        self->hitboxState = 0;
        self->step = self->params;
        if (self->params == 1) {
            ArcherSpawnArrow(self);
        }
        return;
    case 1:
        archer = self - 1;
        ArcherPartFollow(self, archer, 3);
        switch (self->ext.rdaiArcher.mode) {
        case 1:
            AnimateEntity(anim_part1_draw, self);
            if (POSE_AND_TIMER(self) == 2) {
                PlaySfxPositional(0x6C9);
            }
            break;
        case 2:
            ArcherPartRest(self, 0x1B);
            break;
        case 3:
            AnimateEntity(anim_part1_shoot, self);
            if (POSE_AND_TIMER(self) == 6) {
                ArcherSpawnArrow(self);
            }
            break;
        default:
            ArcherPartRest(self, 0x19);
            break;
        }
        break;
    case 2:
        archer = self - 2;
        self->animCurFrame = 0x1C;
        ArcherPartFollow(self, archer, -1);
        if (archer->entityId != E_UNK_1C) {
            DestroyEntity(self);
        }
        return;
    case 3:
        archer = self - 3;
        ArcherPartFollow(self, archer, 1);
        switch (self->ext.rdaiArcher.mode) {
        case 1:
            AnimateEntity(anim_part3_draw, self);
            if (archer->ext.rdaiArcher.mode == 2) {
                self->pose = 0;
                self->poseTimer = 0;
                self->ext.rdaiArcher.mode = 2;
            }
            break;
        case 2:
            AnimateEntity(anim_part3_shoot, self);
            break;
        default:
            ArcherPartRest(self, 0x0B);
            break;
        }
        break;
    default:
        return;
    }
    self->ext.rdaiArcher.mode = archer->ext.rdaiArcher.mode;
    if (archer->entityId != E_UNK_1C) {
        DestroyEntity(self);
    }
}
#else
INCLUDE_ASM("st/rdai/nonmatchings/e_archer", func_us_801C0240);
#endif

#ifdef VERSION_PC
// Arrow (params 0) and its two halves once broken (params 1 and 2).
void func_us_801C0528(Entity* self) {
    Entity* entity;
    Entity* owner;
    s32 i;
    s32 dist;
    s16 angle;

    switch (self->step) {
    case 0:
        InitializeEntity(D_us_80180884);
        self->animCurFrame = 0x16;
        self->hitboxState = 0;
        self->drawFlags |= ENTITY_ROTATE;
        if (self->params) {
            self->animCurFrame += self->params;
            self->flags |= 0xC0202000;
            self->velocityX = (self->params == 1) ? FIX(0.5) : FIX(-0.5);
            self->step = 4;
            break;
        }
        // fallthrough
    case 1: // nocked on the bow
        owner = ARROW(self)->owner;
        self->posX.i.hi = owner->posX.i.hi;
        self->posY.i.hi = owner->posY.i.hi;
        self->rotate = owner->rotate;
        dist = 0x18;
        if (owner->animCurFrame == 0x1A) {
            dist = 0xE;
        }
        if (owner->animCurFrame == 0x1B) {
            dist = 8;
        }
        angle = self->rotate;
        self->posX.i.hi += (dist * rcos(angle)) >> 12;
        self->posY.i.hi += (dist * rsin(angle)) >> 12;
        if (owner->entityId != E_UNK_1D) {
            DestroyEntity(self);
            break;
        }
        if (owner->ext.rdaiArcher.mode != 2) {
            break;
        }
        self->flags |= 0x80000000;
        self->velocityX = rcos(angle) * 80;
        self->velocityY = rsin(angle) * 80;
        self->hitboxState = 3;
        self->hitboxOffX = rcos(angle) >> 9;
        self->hitboxOffY = rsin(angle) >> 9;
        self->hitboxWidth = self->hitboxHeight = 4;
        self->step = 2;
        break;
    case 2: // flying
        MoveEntity();
        if (self->flags & FLAG_DEAD) {
            for (i = 0; i < 2; i++) {
                entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
                if (entity != NULL) {
                    CreateEntityFromEntity(E_UNK_20, self, entity);
                    entity->params = i + 1;
                    entity->rotate = self->rotate;
                }
            }
            DestroyEntity(self);
            break;
        }
        if (self->hitFlags & 0x80) {
            ARROW(self)->stuckX = PLAYER.posX.i.hi - self->posX.i.hi;
            ARROW(self)->stuckY = PLAYER.posY.i.hi - self->posY.i.hi;
            ARROW(self)->timer = 0x20;
            self->hitboxState = 0;
            self->step++;
        }
        break;
    case 3: // stuck in the player
        self->posX.i.hi = PLAYER.posX.i.hi - ARROW(self)->stuckX;
        self->posY.i.hi = PLAYER.posY.i.hi - ARROW(self)->stuckY;
        if (!--ARROW(self)->timer) {
            DestroyEntity(self);
        }
        break;
    case 4: // broken half falling
        MoveEntity();
        self->velocityY += FIX(0.125);
        if (self->params == 1) {
            self->rotate += 0x20;
        } else {
            self->rotate -= 0x20;
        }
        break;
    }
}
#else
INCLUDE_ASM("st/rdai/nonmatchings/e_archer", func_us_801C0528);
#endif

#ifdef VERSION_PC
extern EInit D_us_80180890;
extern s16* sprites_rdai_2[];

static void DebrisExplode(Entity* self) {
    Entity* entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
    if (entity != NULL) {
        CreateEntityFromEntity(E_EXPLOSION, self, entity);
        entity->posX.i.hi += ARROW(self)->stuckX;
        entity->params = 1;
        entity->posY.i.hi += ARROW(self)->stuckY;
    }
    DestroyEntity(self);
}

// Debris of the cart (params 1-9) and of the archer (params 0x13-0x15);
// params is also the sprite frame, whose centre the piece flies from.
void func_us_801C0898(Entity* self) {
    Entity* entity;
    s16* sprite;
    s16 centerX, centerY;
    s16 angle;

    switch (self->step) {
    case 0:
        InitializeEntity(D_us_80180890);
        self->animCurFrame = self->params;
        sprite = sprites_rdai_2[(s16)self->params];
        centerX = sprite[2] + sprite[4] / 2;
        centerY = sprite[3] + sprite[5] / 2;
        ARROW(self)->stuckX = centerX;
        ARROW(self)->stuckY = centerY;
        ARROW(self)->timer = (Random() & 0xF) + 8;
        angle = ratan2(centerY, centerX);
        self->velocityX = rcos(angle) * 32;
        self->velocityY = rsin(angle) * 32;
        entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
        if (entity != NULL) {
            CreateEntityFromEntity(E_EXPLOSION, self, entity);
            entity->params = 1;
            entity->posX.i.hi += centerX;
            entity->posY.i.hi += centerY;
        }
        if (self->params >= 0x13) {
            self->step = 2;
        }
        break;
    case 1:
        MoveEntity();
        self->velocityY += FIX(0.125);
        if (!--ARROW(self)->timer) {
            PlaySfxPositional(0x657);
            DebrisExplode(self);
        }
        break;
    case 2:
        switch (self->params) {
        case 0x13:
            if (!self->step_s) {
                self->velocityY = FIX(-2.5);
                ARROW(self)->timer = 0x10;
                self->step_s++;
            }
            MoveEntity();
            self->velocityY += FIX(0.125);
            if (!--ARROW(self)->timer) {
                DebrisExplode(self);
            }
            break;
        case 0x14:
            if (!self->step_s) {
                ARROW(self)->timer = 0x18;
                self->velocityX = 0;
                self->velocityY = 0;
                self->step_s++;
            }
            MoveEntity();
            self->velocityY += FIX(1.0 / 32);
            if (!--ARROW(self)->timer) {
                DebrisExplode(self);
            }
            break;
        case 0x15:
            if (!self->step_s) {
                ARROW(self)->timer = 0x40;
                self->velocityY = 0;
            }
            MoveEntity();
            self->velocityY += FIX(0.1875);
            break;
        }
        break;
    }
}
#else
INCLUDE_ASM("st/rdai/nonmatchings/e_archer", func_us_801C0898);
#endif
