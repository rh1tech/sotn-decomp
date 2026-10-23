// SPDX-License-Identifier: AGPL-3.0-or-later
// PC build: func_us_801D6B8C, still assembly upstream, transcribed from
// RNO4's US binary. An effect the Cave Troll (E_UNK_40, its parent) spawns:
// it grows, holds, then shrinks away, its hitbox reaching towards the
// player.
#include "rno4.h"

extern EInit D_us_80180B78;

static u8 anim_effect_start[] = {0x01, 0x15, 0x01, 0x16, 0x01, 0x17, 0x01, 0x18, 0x01, 0x19, 0x01, 0x1A, 0x01, 0x1B, 0x01, 0x1C, 0x01, 0x00, 0xFF, 0x00};
static u8 anim_effect_loop[] = {0x02, 0x1D, 0x02, 0x1E, 0x02, 0x1F, 0x02, 0x20, 0x02, 0x21, 0x02, 0x22, 0x00, 0x00, 0x00, 0x00};

#define PARENT(e) (*(Entity**)((u8*)(e) + 0x9C))
#define TIMER(e) (*(s16*)((u8*)(e) + 0x80))
#define PARENT_ACTIVE(e) (*((u8*)(e) + 0x90))

void func_us_801D6B8C(Entity* self) {
    Entity* parent = PARENT(self);
    s32 radius, dist, dx;
    s16 angle;

    switch (self->step) {
    case 0:
        InitializeEntity(D_us_80180B78);
        self->animCurFrame = 0;
        break;
    case 1:
        if (parent->pose == 4) {
            self->step++;
        }
        break;
    case 2:
        AnimateEntity(anim_effect_start, self);
        if (PARENT_ACTIVE(parent)) {
            self->drawFlags = ENTITY_SCALEX | ENTITY_SCALEY;
            self->scaleX = self->scaleY = 0x180;
            self->facingLeft = 0;
            TIMER(self) = 0x60;
            self->zPriority = parent->zPriority - 1;
            SetStep(3);
        }
        break;
    case 3:
        AnimateEntity(anim_effect_loop, self);
        if (self->scaleX < 0x200) {
            self->scaleX = self->scaleY = self->scaleY + 0x20;
        } else if (!--TIMER(self)) {
            SetStep(4);
        }
        // fallthrough
    case 4:
        AnimateEntity(anim_effect_loop, self);
        if (!self->scaleX) {
            PARENT_ACTIVE(parent) = 0;
            DestroyEntity(self);
            return;
        }
        self->scaleX = self->scaleY = self->scaleY - 4;
        angle = GetAngleBetweenEntities(self, &PLAYER);
        radius = (self->scaleX * 15) >> 7;
        // the game measures the X distance twice
        dx = GetDistanceToPlayerX();
        dist = GetDistanceToPlayerX();
        dist = SquareRoot0(dx * dx + dist * dist) - 4;
        if (dist < radius) {
            radius = dist;
        }
        self->hitboxOffX = (radius * rcos(angle)) >> 12;
        self->hitboxOffY = (radius * rsin(angle)) >> 12;
        break;
    }
    self->palette = (g_Timer & 1) ? 0x8163 : 0x819C;
    if (parent->entityId != E_UNK_40) {
        DestroyEntity(self);
    }
}
