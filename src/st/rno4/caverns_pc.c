// SPDX-License-Identifier: AGPL-3.0-or-later
// PC build: the RNO4 functions upstream still has as assembly that come
// from NO4 (cavern background effects, breakables, the ferryman's boat
// chains) and BO3 (the water level). RNO4's versions are NO4's for the
// upside-down castle: the code here is NO4's C changed where RNO4's US
// binary differs, and the tables are RNO4's, copied from the binary.
#include "rno4.h"

extern EInit g_EInitInteractable;
extern EInit g_EInitCommon;
extern EInit g_EInitParticle;
extern EInit g_EInitBreakable;

// ---- tables (RNO4.BIN) ----
static u8 breakable_anims_0[] = {
    0x4, 0x1, 0x4, 0x2, 0x0, 0x0, 0x0, 0x0,
};
static u8 breakable_anims_1[] = {
    0x4, 0x0, 0x4, 0x0, 0x0, 0x0, 0x0, 0x0,
};
static u8 breakable_anims_2[] = {
    0x5, 0x1, 0x5, 0x2, 0x5, 0x3, 0x5, 0x4, 0x0, 0x0, 0x0, 0x0,
};
static u8 breakable_anims_3[] = {
    0x5, 0x5, 0x5, 0x6, 0x5, 0x7, 0x5, 0x8, 0x0, 0x0, 0x0, 0x0,
};
static u8 breakable_anims_4[] = {
    0x0, 0x0, 0x0, 0x0,
};
static u8 breakable_anims_5[] = {
    0x5, 0xD, 0x5, 0xE, 0x5, 0xF, 0x5, 0x10, 0x0, 0x0, 0x0, 0x0,
};
static u8 breakable_anims_6[] = {
    0x5, 0x11, 0x5, 0x12, 0x5, 0x13, 0x0, 0x0,
};
static u8 breakable_anims_7[] = {
    0x5, 0x17, 0x0, 0x0,
};
static u8 breakable_anims_8[] = {
    0x5, 0x16, 0x0, 0x0,
};
static u8 breakable_anims_9[] = {
    0x5, 0x14, 0xFF, 0xFF, 0x5, 0x15, 0x5, 0x15, 0xFF, 0x0, 0x0, 0x0,
};
static u8* breakable_anims[] = {breakable_anims_0, breakable_anims_1, breakable_anims_2, breakable_anims_3, breakable_anims_4, breakable_anims_5, breakable_anims_6, breakable_anims_7, breakable_anims_8, breakable_anims_9};

static u8 breakable_hitbox_heights[] = {
    0x8, 0x8, 0x28, 0x18, 0x10, 0x10, 0x8, 0x8, 0x8, 0x8, 0x8, 0x0,
};

static u8 breakable_explosion_params[] = {
    0x0, 0x0, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x0, 0x0,
};

static u16 breakable_palettes[] = {
    0x0, 0x0, 0x25D, 0x25D, 0x25D, 0x25D, 0x25D, 0x25D, 0x25D, 0x25D,
};

static u16 breakable_anim_sets[] = {
    0x3, 0x3, 0x8009, 0x8009, 0x8009, 0x8009, 0x8009, 0x8009, 0x8009, 0x8009,
};

static u16 breakable_unk5a[] = {
    0x0, 0x7C, 0x5B, 0x5B, 0x5B, 0x5B, 0x5B, 0x5B, 0x5B, 0x5B,
};

static u8 breakable_blend_modes[] = {
    0x70, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x0, 0x0,
};

static s16 breakable_hitbox_y_offsets[] = {
    0, 0, -24, -16, 0, 0, 0, 0, 0, 0,
};

static u8 surface_anim_uv[] = {
    0x81, 0x81, 0x7E, 0x6, 0x81, 0x89, 0x7E, 0x6, 0x81, 0x91, 0x7E, 0x6,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x81, 0x7E, 0x46,
};

static u8 surface_anim[] = {
    0xA, 0x0, 0xA, 0x1, 0xA, 0x2, 0xA, 0x1, 0x0, 0x0, 0x0, 0x0,
};

s16 water_surface_lines[] = {
    208, 240, 176, 240, -2, 224, 192, 79, 16, 0, 288, 96,
    335, 272, 0, 544, 160, 335, 288, 0, 1296, 688, 335, 208,
    0, 1984, 112, 335, 272, 1, 2160, 560, 223, 128, 0, 2788,
    76, 335, 272, -1, 2864, 384, 335, 96, 0, 928, 80, 335,
    272, 0, 48, 144, 255, 48, 0, 1280, 352, 335, 144, 0,
    1808, 336, 335, 208, 0, 560, 176, 335, 320, 0, 128, 3088,
    335, 160, 0, 208, 208, 79, 32, 0, 64, 112, 71, 32,
    0, 56, 120, 79, 16, 2, 64, 112, 79, 32, 0, 84,
    124, 71, 32, 0, 608, 328, 79, 24, 0, 272, 208, 79,
    24, 0, 48, 1232, 39, 24, 0, 1000, 176, 15, 16, 0,
    332, 116, 39, 24, 0, 0,
};

s16 water_tile_rects[] = {
    208, 240, 177, 240, 224, 192, 80, 16, 288, 96, 336, 272,
    544, 160, 336, 288, 1296, 688, 336, 208, 1984, 104, 336, 272,
    2160, 560, 224, 128, 2792, 72, 336, 272, 2864, 384, 336, 96,
    928, 80, 336, 272, 48, 144, 256, 48, 1166, 466, 336, 144,
    1808, 336, 336, 208, 560, 176, 336, 320, 496, 432, 320, 272,
    494, 482, 272, 46, 192, 302, 224, 94, 2144, 144, 256, 32,
    1902, 242, 208, 32, 128, 3184, 336, 160, 208, 208, 80, 32,
    56, 120, 72, 32, 64, 112, 80, 16, 64, 112, 80, 32,
    84, 124, 72, 32, 608, 328, 80, 24, 272, 208, 80, 24,
    48, 1232, 40, 24, 1000, 176, 16, 16, 332, 116, 40, 24,
};

static s16 bubble_sources[] = {
    64, 104, 32, 48, 2, 512, 96, 280, 32, -1,
};

static s32 waterfall_playing = 0;

static s16 waterfall_sound_params[] = {
    256, 2048, 77, 8, 256, 2048, 127, 8, 0, -2048, 77, -8,
    0, -2048, 127, -8,
};

static u16 chain_uv_indexes[] = {
    0x0, 0x0, 0x0, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,
    0x1, 0x1, 0x1, 0x1,
};

static u8 chain_uv[] = {
    0xC1, 0xDF, 0xE1, 0xFF, 0xF4, 0xFC, 0x9C, 0xFC,
};

static s16 chain_wheel_pos[] = {
    1040, 48, 1040, 416, 1280, 48, 736, 48, 736, 416, 496, 48,
};

static s16 chain_links[] = {
    0, 1058, -16, 0, 1058, 80, 0, 1058, 176, 0, 1058, 272,
    1, 1022, 0, 1, 1022, 96, 1, 1022, 192, 1, 1022, 288,
    2, 1080, 66, 2, 1176, 66, 0, 718, -16, 0, 718, 80,
    0, 718, 176, 0, 718, 272, 1, 754, 0, 1, 754, 96,
    1, 754, 192, 1, 754, 288, 3, 512, 64, 3, 608, 64,
};

static u16 ferryman_gate_tiles[] = {
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x374, 0x374,
};

static s16 water_level = 0;

static u16 water_level_flags = 0x0;

static s16 water_level_rects[] = {
    0, 1232, 216, 240, 104, 176, 216, 240, 832, 116, 216, 240,
};

static s16 water_slow_zones[] = {
    608, 32, 208, 128, 1136, 32, 208, 128, 192, 44, 224, 1280,
};

static s16 bg_columns_parallax_props[] = {
    50, 36, 1, 144, 33, 2, 184, 34, 1, 222, 35, 3,
    306, 36, 1, 400, 33, 2, 440, 34, 1, 478, 35, 3,
    32767, 0, 0, 0,
};

static s16 bg_waterfall_columns[] = {
    16, 50, 112, 158, 223, 272, 306, 368, 414, 479, 528, 0,
};

static u16 plume_order[] = {
    0x0, 0xA, 0x4, 0x8, 0x1, 0xC, 0x5, 0x9, 0x2, 0x6, 0xB, 0x3,
    0x7,
};

static u16 small_plume_order[] = {
    0x0, 0x4, 0x8, 0x1, 0x5, 0x2, 0x6, 0x3, 0x7, 0x0,
};

static s16 fg_waterfalls[] = {
    224, 0, 256, 20, 288, 8, 448, 0, 768, 18, 32767, 0,
};

static u16 breakable_wall_tile_layout[] = {
    0x30E, 0x310, 0x766, 0x312, 0x76B, 0x319, 0x323, 0x310, 0x76D, 0x310, 0x76E, 0x312,
    0x76F, 0x319, 0x770, 0x310, 0x771, 0x310, 0x772, 0x312, 0x773, 0x319, 0x774, 0x310,
    0x30F, 0x310, 0x334, 0x314, 0x327, 0x31B, 0x351, 0x352,
};

// ---- breakables ----

void EntityBreakable(Entity* self) {
    Entity* entity;
    u16 breakableType;

    breakableType = self->params >> 12;
    if (!self->step) {
        InitializeEntity(g_EInitBreakable);
        self->zPriority = g_unkGraphicsStruct.g_zEntityCenter - 0x14;
        self->blendMode = breakable_blend_modes[breakableType];
        self->hitboxHeight = breakable_hitbox_heights[breakableType];
        self->animSet = breakable_anim_sets[breakableType];
        self->unk5A = breakable_unk5a[breakableType];
        self->palette = breakable_palettes[breakableType];
        self->hitboxOffY = breakable_hitbox_y_offsets[breakableType];
        if (!breakableType) {
            // the candle hangs upside down
            self->drawFlags = ENTITY_ROTATE;
            self->rotate = ROT(180);
        }
    }

    AnimateEntity(breakable_anims[breakableType], self);

    if (self->hitParams) {
        entity = AllocEntity(&g_Entities[224], &g_Entities[256]);
        if (entity != NULL) {
            CreateEntityFromCurrentEntity(E_EXPLOSION, entity);
            entity->params = breakable_explosion_params[breakableType];
        }
        switch (breakableType) {
        case 0:
            self->drawFlags = ENTITY_DEFAULT;
            self->rotate = 0;
            g_api.PlaySfx(0x634); // SFX_CANDLE_HIT
            break;
        case 2:
        case 3:
            break;
        case 9:
            entity = AllocEntity(&g_Entities[160], &g_Entities[192]);
            if (entity != NULL) {
                CreateEntityFromCurrentEntity(E_UNK_3C, entity);
                entity->params = 0x100;
            }
            g_api.PlaySfx(0x61D); // SFX_GLASS_BREAK_E
            break;
        case 7:
        case 8:
            g_api.PlaySfx(0x61D); // SFX_GLASS_BREAK_E
            entity = AllocEntity(&g_Entities[160], &g_Entities[192]);
            if (entity != NULL) {
                CreateEntityFromEntity(E_PERSISTENT_ITEM_DROP, self, entity);
                entity->params = self->params & 0x1FF;
            }
            PreventEntityFromRespawning(self);
            DestroyEntity(self);
            return;
        default:
            g_api.PlaySfx(0x634); // SFX_CANDLE_HIT
            break;
        }
        ReplaceBreakableWithItemDrop(self);
    }
}

// NO4's EntityBreakableStatueBroken
void func_us_801C123C_from_no4(Entity* self) {
    if (!self->step) {
        InitializeEntity(g_EInitInteractable);
        self->animSet = ANIMSET_OVL(9);
        self->unk5A = 0x5B;
        self->palette = 0x25D;
        self->animCurFrame = 0x15;
        self->zPriority = 0x6A;
        self->step = 0x100;
    }
}

// ---- water surface ----

void func_us_801C12B0_from_no4(Entity* self) {
    u32 var_s8;
    s16* ptr;
    s32 var_s6;
    u8* var_s5;
    s32 var_s4;
    s32 var_s3;
    s32 var_s2;
    s32 var_s1;
    Primitive* prim;
    s32 primIndex;
    s32 scrollX;
    s32 scrollY;
    s32 i;
    s32 xOffset;
    s32 sp38;

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitInteractable);
        self->animSet = 0;
        self->ext.et_801C12B0.unk80 = 4;
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 16);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->flags |= FLAG_HAS_PRIMS;
        self->primIndex = primIndex;
        prim = &g_PrimBuf[primIndex];
        self->ext.et_801C12B0.prim = prim;
        self->ext.et_801C12B0.unk82 = 0;
        while (prim != NULL) {
            prim->tpage = 0xF;
            prim->clut = 0x5E;
            prim->priority = 0x9E;
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
        break;
    }

    prim = self->ext.et_801C12B0.prim;
    AnimateEntity(surface_anim, self);
    i = (self->params >> 8) & 0xFF;
    ptr = &water_surface_lines[(self->params & 0xFF) * 5];
    scrollX = g_Tilemap.scrollX.i.hi - 0x10;
    scrollY = g_Tilemap.scrollY.i.hi;
    xOffset = scrollX + 0x120;
    var_s5 = &surface_anim_uv[self->animCurFrame * 4];
    var_s8 = var_s5[1];

    for (; i > 0; i--) {
        var_s3 = *ptr++;
        var_s2 = var_s3 + *ptr++;
        if (scrollX >= var_s2 || xOffset < var_s3) {
            ptr += 3;
            continue;
        }
        var_s6 = *ptr++;
        if (scrollY - 4 > var_s6 || var_s6 > scrollY + 0xE0) {
            ptr += 2;
            continue;
        }
        ptr++;
        sp38 = *ptr++;
        var_s6 -= scrollY;
        if (var_s3 < scrollX) {
            var_s3 = scrollX;
        }
        if (xOffset < var_s2) {
            var_s2 = xOffset;
        }
        if (sp38 != 0) {
            self->ext.et_801C12B0.unk82 += sp38;
            var_s4 = (u32)(var_s3 - self->ext.et_801C12B0.unk82) % 126;
        } else {
            var_s4 = (u32)var_s3 % 126;
        }
        var_s2 -= var_s3;
        var_s3 -= scrollX + 0x10;
        var_s4 += var_s5[0];
        do {
            prim->u0 = prim->u2 = var_s4;
            var_s1 = 0x7E - (var_s4 - var_s5[0]);
            if (var_s2 < var_s1) {
                var_s1 = var_s2;
            }
            prim->u1 = prim->u3 = var_s4 + var_s1;
            prim->v0 = prim->v1 = var_s8;
            prim->v2 = prim->v3 = var_s8 + 1;
            prim->x0 = prim->x2 = var_s3;
            var_s3 += var_s1;
            prim->x1 = prim->x3 = var_s3;
            var_s2 -= var_s1;
            var_s4 = var_s5[0];
            prim->y0 = prim->y1 = var_s6;
            prim->y2 = prim->y3 = var_s6 + 1;
            prim->drawMode = DRAW_TPAGE | DRAW_UNK02 | DRAW_TRANSP;
            prim = prim->next;
        } while (var_s2 != 0);
    }

    while (prim != NULL) {
        prim->drawMode = DRAW_HIDE;
        prim = prim->next;
    }
}

void func_us_801C15F8_from_no4(Entity* self) {
    s32 scrollX, scrollY;
    s16* ptr;
    s32 var_s5, var_s4, var_s3, var_s2, var_s1;
    Primitive* prim;
    s32 primIndex;
    s32 xOffset, yOffset;
    s32 i;

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitInteractable);
        self->animSet = 0;
        self->ext.et_801C12B0.unk80 = 4;
        primIndex = g_api.AllocPrimitives(PRIM_TILE, 16);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->flags |= FLAG_HAS_PRIMS;
        self->primIndex = primIndex;
        prim = &g_PrimBuf[primIndex];
        self->ext.et_801C12B0.prim = prim;
        while (prim != NULL) {
            prim->r0 = 0x10;
            prim->g0 = 0x08;
            prim->b0 = 0x18;
            prim->priority = 0x9D;
            prim = prim->next;
        }
        break;
    }

    prim = self->ext.et_801C12B0.prim;
    ptr = &water_tile_rects[(self->params & 0xFF) * 4];
    i = (self->params >> 8) & 0xFF;
    scrollX = g_Tilemap.scrollX.i.hi - 0x10;
    scrollY = g_Tilemap.scrollY.i.hi - 0x10;
    xOffset = scrollX + 0x120;
    yOffset = scrollY + 0x100;

    for (; i > 0; i--) {
        var_s3 = *ptr++;
        var_s2 = var_s3 + *ptr++;
        if (scrollX >= var_s2 || xOffset < var_s3) {
            ptr += 2;
            continue;
        }
        // RNO4 stores the bottom row first
        var_s4 = *ptr++;
        var_s5 = *ptr++;
        if (var_s4 > scrollY && yOffset >= var_s5) {
            if (var_s3 < scrollX) {
                var_s3 = scrollX;
            }
            if (xOffset < var_s2) {
                var_s2 = xOffset;
            }
            var_s2 -= var_s3;
            var_s3 -= scrollX + 0x10;
            if (var_s5 < scrollY) {
                var_s5 = scrollY;
            }
            if (yOffset < var_s4) {
                var_s4 = yOffset;
            }
            var_s4 -= var_s5;
            var_s5 -= scrollY + 0x10;
            if (var_s4 >= 0x100) {
                var_s4 = 0xFF;
            }
            do {
                var_s1 = var_s2;
                if (var_s1 >= 0x100) {
                    var_s1 = 0xFF;
                }
                prim->u0 = var_s1;
                prim->v0 = var_s4;
                prim->x0 = var_s3;
                prim->y0 = var_s5;
                var_s3 += var_s1;
                var_s2 -= var_s1;
                prim->drawMode = DRAW_TPAGE | DRAW_UNK02 | DRAW_TRANSP;
                prim = prim->next;
            } while (var_s2 != 0);
        }
    }

    while (prim != NULL) {
        prim->drawMode = DRAW_HIDE;
        prim = prim->next;
    }
}

// ---- bubbles, waterfall sound, slow water ----

void func_us_801C34EC_from_no4(Entity* self) {
    s32 scrollX;
    s16 xOffset, randX, randY, tpage;
    s32 yOffset;
    s32 scrollY;
    s32 primIndex;
    s16* ptr;
    Primitive* prim;

    if (!self->step) {
        InitializeEntity(g_EInitInteractable);
        self->animSet = 0;
        primIndex = g_api.AllocPrimitives(PRIM_TILE, 0x40);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->flags |= FLAG_HAS_PRIMS;
        self->primIndex = primIndex;
        prim = &g_PrimBuf[primIndex];
        self->ext.et_801C12B0.prim = prim;
        while (prim != NULL) {
            prim->priority = 0x9B;
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
    }

    ptr = &bubble_sources[self->params * 5];
    xOffset = *ptr++;
    randX = *ptr++;
    yOffset = *ptr++;
    randY = *ptr++;
    tpage = *ptr;
    prim = self->ext.et_801C12B0.prim;
    scrollX = g_Tilemap.scrollX.i.hi;
    scrollY = g_Tilemap.scrollY.i.hi;
    self->ext.et_801C12B0.unk80++;
    if (scrollY < 0xF0) {
        yOffset -= scrollY;
        while (prim != NULL) {
            if (prim->drawMode == DRAW_HIDE) {
                prim->r0 = prim->b0 = prim->g0 = 0x80;
                prim->y0 = yOffset + (rand() % randY);
                prim->x0 = (xOffset - scrollX) + (rand() % randX);
                prim->x1 = 0;
                LOH(prim->r1) = 0;
                prim->u0 = prim->v0 = 2;
                LOW(prim->u1) = 0;
                prim->tpage = tpage;
                prim->drawMode = DRAW_UNK02;
                break;
            }
            prim = prim->next;
        }
    }

    prim = self->ext.et_801C12B0.prim;
    while (prim != NULL) {
        if (prim->drawMode != DRAW_HIDE) {
            LOH(prim->b1) = prim->x0;
            LOW(prim->r1) += LOW(prim->u1);
            prim->x0 = LOH(prim->b1);
            prim->r0 -= 8;
            prim->b0 = prim->g0 = prim->r0;
            if (prim->r0 < 8) {
                prim->drawMode = DRAW_HIDE;
            }
        }
        prim = prim->next;
    }
}

void func_us_801C4BD8_from_no4(Entity* self) {
    Entity* player;
    s16* dataPtr;
    s32 volume;

    if (!self->step) {
        InitializeEntity(g_EInitInteractable);
        self->animSet = 0;
    }
    player = &PLAYER;
    dataPtr = &waterfall_sound_params[self->params * 4];
    volume = player->posX.i.hi + g_Tilemap.scrollX.i.hi - *dataPtr++;
    volume = (volume * *dataPtr++) / 0x1000;
    volume += *dataPtr++;
    if (volume < 0) {
        volume = 0;
    } else if (volume > 0x7F) {
        volume = 0x7F;
    }
    if (!volume) {
        if (waterfall_playing) {
            waterfall_playing = false;
            g_api.PlaySfx(SET_UNK_A6);
            return;
        }
    }
    if (waterfall_playing) {
        g_api.SetVolumeCommand22_23(volume, *dataPtr++);
        return;
    }
    g_api.PlaySfxVolPan(SFX_WATERFALL_LOOP, volume, *dataPtr++);
    waterfall_playing = true;
}

void func_us_801C5518_from_no4(Entity* self) {
    Entity* player;
    u16 diff;
    s16* dataPtr;

    player = &PLAYER;
    if (!self->step) {
        InitializeEntity(g_EInitInteractable);
        self->animSet = 0;
    }
    dataPtr = &water_slow_zones[self->params * 4];
    diff = player->posX.i.hi + g_Tilemap.scrollX.i.hi - *dataPtr++;
    if (diff > *dataPtr++) {
        return;
    }
    diff = player->posY.i.hi + g_Tilemap.scrollY.i.hi - *dataPtr++;
    if (diff > *dataPtr++) {
        return;
    }
    if (player->velocityY < 0) {
        player->velocityY *= 7;
        player->velocityY /= 8;
    } else if (player->velocityY > 0) {
        player->nFramesInvincibility = 1;
    }
}

// ---- the ferryman's boat elevator ----

void EntityBoatElevatorChains(Entity* self) {
    u32 primIndex;
    u32 scrollX;

    u32 scrollY;
    s16 cos;
    u8* ptr;
    s32 i;
    s16* ptrTwo;
    s16 sin;
    s16 xOffset;
    s16 yOffset;
    Primitive* prim;

    scrollX = g_Tilemap.scrollX.i.hi;
    scrollY = g_Tilemap.scrollY.i.hi;
    if (!self->step) {
        InitializeEntity(g_EInitInteractable);
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 13);
        if (primIndex != -1) {
            self->flags |= FLAG_HAS_PRIMS;
            self->primIndex = primIndex;
            prim = &g_PrimBuf[primIndex];

            i = 0;
            while (prim != NULL) {
                prim->tpage = 0xF;
                prim->clut = 0x5F;
                ptr = *chain_uv;
                ptr += chain_uv_indexes[i] * 4;
                prim->u0 = prim->u2 = *ptr++;
                prim->u1 = prim->u3 = *ptr++;
                prim->v0 = prim->v1 = *ptr++;
                prim->v2 = prim->v3 = *ptr;
                prim->priority = 0x80;
                prim->drawMode = DRAW_HIDE;
                prim = prim->next;
                i++;
            }
            self->rotate = ROT(45);
        } else {
            self->step = 0;
            return;
        }
    }

    if (self->ext.boatElevator_child.unk7C) {
        if (self->ext.boatElevator_child.unk7C < 0) {
            self->ext.boatElevator_child.unk7E++;
            self->rotate += ROT(1.40625);
        } else {
            self->ext.boatElevator_child.unk7E--;
            self->rotate -= ROT(1.40625);
        }
    }
    self->ext.boatElevator_child.unk7E &= 0xF;
    prim = &g_PrimBuf[self->primIndex];
    i = 0;
    while (prim != NULL) {
        if (i < 3) {
            ptrTwo = &chain_wheel_pos[(self->params * 6) + (i * 2)];
            xOffset = *ptrTwo++ - scrollX;
            yOffset = *ptrTwo - scrollY;
            if (self->params) {
                sin = (rsin(-self->rotate) * 0x1A) >> 0xC;
                cos = (rcos(-self->rotate) * 0x1A) >> 0xC;
            } else {
                sin = (rsin(self->rotate) * 0x1A) >> 0xC;
                cos = (rcos(self->rotate) * 0x1A) >> 0xC;
            }

            prim->x0 = xOffset - cos;
            prim->x1 = xOffset + sin;
            prim->x2 = xOffset - sin;
            prim->x3 = xOffset + cos;
            prim->y0 = yOffset - sin;
            prim->y1 = yOffset - cos;
            prim->y2 = yOffset + cos;
            prim->y3 = yOffset + sin;
            prim->drawMode = DRAW_UNK02;
            prim = prim->next;
        } else {
            ptrTwo = &chain_links[(self->params * 3) * 10 + ((i - 3) * 3)];
            sin = *ptrTwo++;
            xOffset = *ptrTwo++ - scrollX;
            yOffset = *ptrTwo - scrollY;
            switch (sin) {
            case 0:
                prim->x0 = prim->x2 = xOffset - 4;
                prim->x1 = prim->x3 = xOffset + 4;
                yOffset += self->ext.boatElevator_child.unk7E;
                prim->y0 = prim->y1 = yOffset;
                prim->y2 = prim->y3 = yOffset + 0x60;
                break;
            case 1:
                prim->x0 = prim->x2 = xOffset - 4;
                prim->x1 = prim->x3 = xOffset + 4;
                yOffset -= self->ext.boatElevator_child.unk7E;
                prim->y0 = prim->y1 = yOffset;
                prim->y2 = prim->y3 = yOffset + 0x60;
                break;
            case 2:
                xOffset -= self->ext.boatElevator_child.unk7E;
                prim->x0 = prim->x1 = xOffset;
                prim->x2 = prim->x3 = xOffset + 0x60;
                prim->y1 = prim->y3 = yOffset - 4;
                prim->y0 = prim->y2 = yOffset + 4;
                break;
            case 3:
                xOffset += self->ext.boatElevator_child.unk7E;
                prim->x0 = prim->x1 = xOffset;
                prim->x2 = prim->x3 = xOffset + 0x60;
                prim->y1 = prim->y3 = yOffset - 4;
                prim->y0 = prim->y2 = yOffset + 4;
                break;
            }
            prim->drawMode = DRAW_UNK02;
            prim = prim->next;
        }
        i++;
    }
}

void LoadFerrymanGateTiles(void) {
    u16* tileLayoutPtr;
    Tilemap* tileMap;
    s32 i;
    s16 offset;

    tileMap = &g_Tilemap;
    offset = 0xF89;
    tileLayoutPtr = ferryman_gate_tiles;
    for (i = 0; i < 7; i++) {
        tileMap->fg[offset] = *tileLayoutPtr++;
        offset++;
        tileMap->fg[offset] = *tileLayoutPtr++;
        offset += 0xCF;
    }
}

// ---- BO3's water level ----

// raise the water's edge in the tables to the current level
void func_us_801A071C_from_bo3(void) {
    s16* data;
    s16* tileRects;
    s16* surfaceLines;
    s16 value;

    data = water_level_rects;
    tileRects = water_tile_rects;
    surfaceLines = water_surface_lines;
    data += 2;
    tileRects += 0x6E;
    surfaceLines += 0x70;

    value = 0xD8 - water_level;
    if (value < 0x18) {
        value = 0;
    }
    *data = value;
    *tileRects = value;
    *surfaceLines = value;
    data += 4;
    tileRects += 4;
    surfaceLines += 5;

    if (water_level < 0xC0) {
        value = 0xF0;
    } else {
        value = 0x1A8 - water_level;
    }
    *data = value;
    data += 4;
    *data = value;
    if (value > 0xE8) {
        value = 0xE8;
    }
    *surfaceLines = value;
    surfaceLines += 5;
    *surfaceLines = value;
    value++;
    *tileRects = value;
    tileRects += 4;
    *tileRects = value;
}

// ---- cavern background ----

// background columns scrolling in parallax, hanging from the floor of the
// upside-down cavern
void EntityBgColumnsParallax_from_no4(Entity* self) {
    s32 primIndex;
    s32 scrollY, scrollX;
    s32 flipXY;
    s16* ptr;
    s32 posYBottom, posY, posX;
    Primitive* prim;

    if (!self->step) {
        InitializeEntity(g_EInitInteractable);
        self->animSet = 0;
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 10);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->flags |= FLAG_HAS_PRIMS;
        self->primIndex = primIndex;
        prim = &g_PrimBuf[primIndex];
        self->ext.et_801C12B0.prim = prim;
        while (prim != NULL) {
            prim->tpage = 0xE;
            prim->clut = 0x84;
            prim->u0 = prim->u2 = 0xE9;
            prim->u1 = prim->u3 = 0xFD;
            prim->v0 = prim->v1 = 0x7F;
            prim->v2 = prim->v3 = 1;
            prim->priority = 0x1C;
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
    }

    prim = self->ext.et_801C12B0.prim;
    ptr = bg_columns_parallax_props;
    scrollX = g_Tilemap.scrollX.i.hi;
    scrollX /= 2;
    scrollX &= 0xFF;
    scrollY = g_Tilemap.scrollY.i.hi;
    scrollY /= 2;
    for (;;) {
        posX = *ptr++;
        posY = *ptr++;
        flipXY = *ptr++;
        posX -= scrollX;
        posY = (g_Tilemap.height - 0x100) / 2 + 0x7A - posY - scrollY;
        posYBottom = posY + 0x7E;
        if (posX < -20) {
            continue;
        }
        if (posX >= 0x100) {
            break;
        }
        if (posYBottom < 0 || posY >= 0xE0) {
            continue;
        }
        if (flipXY & 1) {
            prim->x0 = prim->x2 = posX + 0x14;
            prim->x1 = prim->x3 = posX;
        } else {
            prim->x0 = prim->x2 = posX;
            prim->x1 = prim->x3 = posX + 0x14;
        }
        if (flipXY & 2) {
            prim->y0 = prim->y1 = posY;
            prim->y2 = prim->y3 = posYBottom;
        } else {
            prim->y0 = prim->y1 = posYBottom;
            prim->y2 = prim->y3 = posY;
        }
        prim->drawMode = DRAW_DEFAULT;
        prim = prim->next;
    }
    while (prim != NULL) {
        prim->drawMode = DRAW_HIDE;
        prim = prim->next;
    }
}

// background waterfall, falling up from the floor of the upside-down
// cavern
void func_us_801C1EE4_from_no4(Entity* self) {
    s32 scrollX, scrollY;
    s32 clut;
    s32 bottom, top;
    s32 texV, len, part, rest, partEnd;
    s32 x;
    Primitive* prim;
    s32 primIndex;
    s16* ptr;

    if (!self->step) {
        InitializeEntity(g_EInitInteractable);
        self->animSet = 0;
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 16);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->ext.et_801C12B0.clut = 0;
        self->flags |= FLAG_HAS_PRIMS;
        self->primIndex = primIndex;
        prim = &g_PrimBuf[primIndex];
        self->ext.et_801C12B0.prim = prim;
        while (prim != NULL) {
            prim->tpage = 0xE;
            prim->u0 = prim->u2 = 0xFC;
            prim->u1 = prim->u3 = 0xFE;
            prim->priority = 0x1A;
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
    }

    prim = self->ext.et_801C12B0.prim;
    ptr = bg_waterfall_columns;
    self->ext.et_801C12B0.clut++;
    if (self->ext.et_801C12B0.clut >= 0xE) {
        self->ext.et_801C12B0.clut = 0;
    }
    scrollX = g_Tilemap.scrollX.i.hi;
    scrollX *= 3;
    scrollX /= 8;
    scrollX &= 0xFF;
    scrollY = g_Tilemap.scrollY.i.hi;
    bottom = g_Tilemap.height - (scrollY + 0x50);
    top = g_Tilemap.height - (scrollY + 0xB0);
    if (bottom > 0xE0) {
        bottom = 0xE0;
    }
    if (bottom >= 0) {
        texV = (scrollY * 3 / 8 + top) % 0x20;
        len = bottom - top;
        if (texV + len > 0x60) {
            part = 0x60 - texV;
        } else {
            part = len;
        }
        rest = len - part;
        partEnd = part + top;
        clut = self->ext.et_801C12B0.clut + 0x90;

        while (true) {
            x = *ptr++;
            x -= scrollX;
            if (x < -2) {
                continue;
            }
            if (x >= 0x100) {
                break;
            }
            prim->clut = clut;
            prim->v2 = prim->v3 = texV + 0x84;
            // RNO4 reads v0 here, not v2 (so v0 keeps last frame's value
            // plus part): kept as in the game
            prim->v0 = prim->v1 = prim->v0 + part;
            prim->x0 = prim->x2 = x;
            prim->x1 = prim->x3 = x + 2;
            prim->y0 = prim->y1 = top;
            prim->y2 = prim->y3 = partEnd;
            prim->drawMode =
                DRAW_UNK_40 | DRAW_TPAGE2 | DRAW_TPAGE | DRAW_TRANSP;
            prim = prim->next;
            if (rest != 0) {
                prim->clut = clut;
                prim->v0 = prim->v1 = 0x84;
                prim->v2 = prim->v3 = rest + 0x84;
                prim->x0 = prim->x2 = x;
                prim->x1 = prim->x3 = x + 2;
                prim->y0 = prim->y1 = partEnd;
                prim->y2 = prim->y3 = partEnd + rest;
                prim->drawMode =
                    DRAW_UNK_40 | DRAW_TPAGE2 | DRAW_TPAGE | DRAW_TRANSP;
                prim = prim->next;
            }
        }
    }

    while (prim != NULL) {
        prim->drawMode = DRAW_HIDE;
        prim = prim->next;
    }
}

// glow over the water (three gouraud quads), mirrored for RNO4
void func_us_801C2850_from_no4(Entity* self) {
    Primitive* prim;
    s32 primIndex;
    s32 scrollX, scrollY;
    s32 top, bottom;
    s32 colTop, colBottom;
    s32 fine;

    if (!self->step) {
        InitializeEntity(g_EInitInteractable);
        self->animSet = 0;
        primIndex = g_api.AllocPrimitives(PRIM_G4, 3);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->flags |= FLAG_HAS_PRIMS;
        self->primIndex = primIndex;
        prim = &g_PrimBuf[primIndex];
        self->ext.et_801C12B0.prim = prim;
        while (prim != NULL) {
            prim->priority = 0x9E;
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
    }

    prim = self->ext.et_801C12B0.prim;
    scrollX = g_Tilemap.scrollX.i.hi;
    scrollY = g_Tilemap.scrollY.i.hi;
    self->ext.et_801C12B0.unk80++;
    if (scrollY < 0x410) {
        top = 0x20 - scrollY;
        bottom = 0x220 - scrollY;
        fine = scrollY & 7;
        if (top < -0x10) {
            top = (-fine & 7) - 0x10;
        }
        if (bottom > 0xF0) {
            bottom = (-fine & 7) + 0xF0;
        }
        colTop = 0x220 - (top + scrollY);
        colBottom = 0x220 - (bottom + scrollY);
        if (self->ext.et_801C12B0.unk80 & 1) {
            colTop = colTop * 0x13 / 40;
            colBottom = colBottom * 0x13 / 40;
        } else {
            colTop = colTop * 0x12 / 40;
            colBottom = colBottom * 0x12 / 40;
        }

        prim->r0 = prim->r1 = prim->b0 = prim->b1 = prim->g0 = prim->g1 =
            colTop;
        prim->r2 = prim->r3 = prim->b2 = prim->b3 = prim->g2 = prim->g3 =
            colBottom;
        prim->x0 = prim->x2 = 0x180 - scrollX;
        prim->x1 = prim->x3 = 0xE0 - scrollX;
        prim->y0 = prim->y1 = top;
        prim->y2 = prim->y3 = bottom;
        prim->drawMode = DRAW_DITHERING | DRAW_TPAGE2 | DRAW_TPAGE |
                         DRAW_COLORS | DRAW_UNK02 | DRAW_TRANSP;
        prim = prim->next;

        prim->r0 = prim->r2 = prim->b0 = prim->b2 = prim->g0 = prim->g2 = 0;
        prim->r1 = prim->b1 = prim->g1 = colTop;
        prim->r3 = prim->b3 = prim->g3 = colBottom;
        prim->x0 = prim->x2 = 0x1E0 - scrollX;
        prim->x1 = prim->x3 = 0x180 - scrollX;
        prim->y0 = prim->y1 = top;
        prim->y2 = prim->y3 = bottom;
        prim->drawMode = DRAW_DITHERING | DRAW_TPAGE2 | DRAW_TPAGE |
                         DRAW_COLORS | DRAW_UNK02 | DRAW_TRANSP;
        prim = prim->next;

        prim->r0 = prim->b0 = prim->g0 = colTop;
        prim->r2 = prim->b2 = prim->g2 = colBottom;
        prim->r1 = prim->b1 = prim->g1 = prim->r3 = prim->b3 = prim->g3 = 0;
        prim->x0 = prim->x2 = 0xE0 - scrollX;
        prim->x1 = prim->x3 = 0x80 - scrollX;
        prim->y0 = prim->y1 = top;
        prim->y2 = prim->y3 = bottom;
        prim->drawMode = DRAW_DITHERING | DRAW_TPAGE2 | DRAW_TPAGE |
                         DRAW_COLORS | DRAW_UNK02 | DRAW_TRANSP;
        prim = prim->next;
    }

    while (prim != NULL) {
        prim->drawMode = DRAW_HIDE;
        prim = prim->next;
    }
}

// spray at the foot of a waterfall
void func_us_801C2B78_from_no4(Entity* self) {
    Primitive* prim;
    s32 i;
    s32 base;
    s32 scrollX, scrollY;
    s32 primIndex;

    if (!self->step) {
        InitializeEntity(g_EInitInteractable);
        self->animSet = 0;
        primIndex = g_api.AllocPrimitives(PRIM_TILE, 128);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->flags |= FLAG_HAS_PRIMS;
        self->primIndex = primIndex;
        prim = &g_PrimBuf[primIndex];
        self->ext.et_801C12B0.prim = prim;
        while (prim != NULL) {
            prim->priority = 0x6C;
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
    }

    prim = self->ext.et_801C12B0.prim;
    scrollX = g_Tilemap.scrollX.i.hi;
    scrollY = g_Tilemap.scrollY.i.hi;
    self->ext.et_801C12B0.unk80++;
    if (scrollY < 0x130) {
        base = 0x50 - scrollY;
        for (i = 0; i < 4; i++) {
            while (prim != NULL) {
                if (prim->drawMode == DRAW_HIDE) {
                    prim->r0 = prim->b0 = prim->g0 = 0x80;
                    prim->x0 = ((rand() % 11) * 4 + 0x115 - scrollX) + i;
                    prim->y0 = base - 4 + (rand() & 7);
                    prim->x1 = 0;
                    LOH(prim->r1) = 0;
                    prim->u0 = prim->v0 = 2;
                    LOW(prim->r2) = FIX(4);
                    LOW(prim->u1) = 0;
                    LOW(prim->x3) = 0x4000 - (rand() & 0xFF0);
                    LOW(prim->r3) = (rand() & 0x1F00) - 0xF80;
                    prim->clut = 0x20;
                    prim->drawMode = DRAW_UNK02;
                    break;
                }
                prim = prim->next;
            }
        }
    }

    prim = self->ext.et_801C12B0.prim;
    while (prim != NULL) {
        if (prim->drawMode != DRAW_HIDE) {
            LOH(prim->b1) = prim->x0;
            prim->y1 = prim->y0;
            LOW(prim->x1) -= LOW(prim->r2); // falls, where NO4's rises
            LOW(prim->r1) += LOW(prim->u1);
            prim->x0 = LOH(prim->b1);
            prim->y0 = prim->y1;
            LOW(prim->r2) -= LOW(prim->x3);
            LOW(prim->u1) -= LOW(prim->r3);
            if (!--prim->clut) {
                prim->drawMode = DRAW_HIDE;
            }
        }
        prim = prim->next;
    }
}

// splash plumes at the foot of a waterfall, growing downwards in RNO4
void func_us_801C2E60_from_no4(Entity* self) {
    s32 jitter;
    s32 scrollX, scrollY;
    s32 base;
    s32 primIndex;
    Primitive* prim;

    if (!self->step) {
        InitializeEntity(g_EInitInteractable);
        self->animSet = 0;
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 32);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->flags |= FLAG_HAS_PRIMS;
        self->primIndex = primIndex;
        prim = &g_PrimBuf[primIndex];
        self->ext.et_801C12B0.prim = prim;
        while (prim != NULL) {
            prim->clut = PAL_CC_STONE_EFFECT;
            prim->tpage = 0x1A;
            prim->u0 = prim->u2 = 0;
            prim->u1 = prim->u3 = 0x1E;
            prim->v0 = prim->v1 = 0x7C;
            prim->v2 = prim->v3 = 0x60;
            prim->priority = 0x9D;
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
    }

    prim = self->ext.et_801C12B0.prim;
    scrollX = g_Tilemap.scrollX.i.hi;
    scrollY = g_Tilemap.scrollY.i.hi;
    self->ext.et_801C12B0.unk80++;
    if (self->ext.et_801C12B0.unk80 >= LEN(plume_order)) {
        self->ext.et_801C12B0.unk80 = 0;
    }
    if (scrollY < 0x180) {
        base = 0x50 - scrollY;
        while (prim != NULL) {
            if (prim->drawMode == DRAW_HIDE) {
                prim->y0 = prim->y1 = prim->y2 = prim->y3 = base;
                jitter = (rand() & 3) - 9;
                prim->x2 = prim->x0 =
                    plume_order[self->ext.et_801C12B0.unk80] * 4 + 0x111 -
                    scrollX + jitter;
                prim->x3 = prim->x1 = prim->x0 + 0x12;
                PGREY(prim, 0) = PGREY(prim, 1) = 0xFF;
                PGREY(prim, 2) = PGREY(prim, 3) = 0x80;
                prim->p1 = 0;
                prim->drawMode = DRAW_UNK_40 | DRAW_TPAGE2 | DRAW_TPAGE |
                                 DRAW_COLORS | DRAW_UNK02 | DRAW_TRANSP;
                break;
            }
            prim = prim->next;
        }
    }
    prim = self->ext.et_801C12B0.prim;
    while (prim != NULL) {
        if (prim->drawMode != DRAW_HIDE) {
            prim->y2++;
            if (rand() & 7) {
                prim->y2++;
            }
            prim->y3 = prim->y2;
            prim->p1++;
            prim->b1 -= 0x10;
            PGREY(prim, 0) = PGREY(prim, 1);
            prim->b3 -= 8;
            PGREY(prim, 2) = PGREY(prim, 3);
            if (prim->r0 < 0x11) {
                prim->drawMode = DRAW_HIDE;
            }
        }
        prim = prim->next;
    }
}

// spray where the water pours (params 0) and drips (params 1)
void func_us_801C3160_from_no4(Entity* self) {
    s32 scrollY, scrollX;
    s32 primIndex;
    u16 iterations;
    u16 params;
    s32 i;
    s32 yOffset;
    s32 randVal;
    Primitive* prim;

    params = self->params;
    if (!self->step) {
        InitializeEntity(g_EInitInteractable);
        self->animSet = 0;
        if (!params) {
            primIndex = g_api.AllocPrimitives(PRIM_TILE, 0x80);
        } else {
            primIndex = g_api.AllocPrimitives(PRIM_TILE, 8);
        }
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->flags |= FLAG_HAS_PRIMS;
        self->primIndex = primIndex;
        prim = &g_PrimBuf[primIndex];
        self->ext.et_801C12B0.prim = prim;
        while (prim != NULL) {
            prim->priority = 0x6C;
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
    }

    prim = self->ext.et_801C12B0.prim;
    scrollX = g_Tilemap.scrollX.i.hi;
    scrollY = g_Tilemap.scrollY.i.hi;
    self->ext.et_801C12B0.unk80++;
    if (scrollY < g_Tilemap.height - 0xF0) {
        if (!params) {
            iterations = 4;
            yOffset = 0x550 - scrollY;
        } else {
            iterations = 1;
            yOffset = 0x12A - scrollY;
        }
        for (i = 0; i < iterations; i++) {
            while (prim != NULL) {
                if (prim->drawMode == DRAW_HIDE) {
                    if (!params) {
                        randVal = rand() & 0x1F;
                        prim->y0 = yOffset - randVal;
                        prim->x0 =
                            (rand() % 40) - (scrollX - 0x100) + randVal;
                        LOW(prim->u1) = FIX(2);
                        prim->clut = 0x10;
                        prim->u0 = prim->v0 = 2;
                        LOW(prim->r2) = 0;
                    } else {
                        prim->y0 = yOffset - (rand() & 3);
                        prim->x0 = 0x1F2 - (rand() & 3) - scrollX;
                        LOW(prim->u1) = 0xFFFEC000;
                        LOW(prim->r2) = (rand() & 0x1F) << 0xB;
                        prim->u0 = prim->v0 = 1;
                        prim->clut = 8;
                    }
                    prim->r0 = prim->b0 = prim->g0 = 0x80;
                    prim->x1 = 0;
                    LOH(prim->r1) = 0;
                    LOW(prim->x3) = 0x4000;
                    LOW(prim->r3) = 0;
                    prim->drawMode = DRAW_UNK02;
                    break;
                }
                prim = prim->next;
            }
        }
    }

    prim = self->ext.et_801C12B0.prim;
    while (prim != NULL) {
        if (prim->drawMode != DRAW_HIDE) {
            LOH(prim->b1) = prim->x0;
            prim->y1 = prim->y0;
            LOW(prim->x1) = LOW(prim->x1) - LOW(prim->r2);
            LOW(prim->r1) = LOW(prim->r1) + LOW(prim->u1);
            prim->x0 = LOH(prim->b1);
            prim->y0 = prim->y1;
            LOW(prim->r2) = LOW(prim->r2) + LOW(prim->x3);
            prim->r0 = prim->b0 = prim->g0 -= 8;
            if (!--prim->clut) {
                prim->drawMode = DRAW_HIDE;
            }
        }
        prim = prim->next;
    }
}

// ripples: jitters the edges of two water surfaces and draws a shimmer
void func_us_801C37C8_from_no4(Entity* self) {
    Primitive* prim;
    s32 jitter;
    s32 scrollX, scrollY;
    s32 xOffset;
    s32 primIndex;
    s32 clut;

    if (!self->step) {
        InitializeEntity(g_EInitInteractable);
        self->animSet = 0;
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 1);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->ext.et_801C12B0.clut = 0;
        self->flags |= FLAG_HAS_PRIMS;
        self->primIndex = primIndex;
        prim = &g_PrimBuf[primIndex];
        self->ext.et_801C12B0.prim = prim;
        prim->tpage = 0xF;
        prim->v0 = prim->v1 = 0x7F;
        prim->v2 = prim->v3 = 1;
        prim->priority = 0x9C;
        prim->drawMode = DRAW_HIDE;
    }

    jitter = (rand() & 0x1F) - 0x10;
    water_surface_lines[40] = jitter + 0xB30;
    water_surface_lines[41] = 0x180 - jitter;
    water_surface_lines[36] = jitter + 0x4C;
    water_surface_lines[25] = jitter + 0x7C0;
    water_surface_lines[26] = 0x70 - jitter;
    water_surface_lines[21] = jitter + 0x2B0;
    self->ext.et_801C12B0.clut++;
    if (self->ext.et_801C12B0.clut >= 0xE) {
        self->ext.et_801C12B0.clut = 0;
    }

    clut = self->ext.et_801C12B0.clut + 0xA0;
    prim = self->ext.et_801C12B0.prim;
    scrollX = g_Tilemap.scrollX.i.hi;
    scrollY = g_Tilemap.scrollY.i.hi;

    if (scrollX < 0x898) {
        xOffset = 0x828 - scrollX;
        if (xOffset >= 0x100) {
            prim->drawMode = DRAW_HIDE;
            return;
        }
        prim->u0 = prim->u2 = 0x11;
        prim->u1 = prim->u3 = 0x80;
    } else if (scrollX < 0xAE8) {
        xOffset = 0xA78 - scrollX;
        if (xOffset >= 0x100) {
            prim->drawMode = DRAW_HIDE;
            return;
        }
        prim->u0 = prim->u2 = 0x80;
        prim->u1 = prim->u3 = 0x11;
    } else {
        prim->drawMode = DRAW_HIDE;
        return;
    }

    prim->clut = clut;
    prim->x0 = prim->x2 = xOffset;
    prim->x1 = prim->x3 = xOffset + 0x6F;
    prim->y0 = prim->y1 = 0xD1 - scrollY;
    prim->y2 = prim->y3 = prim->y0 + 0x7F;
    prim->drawMode = DRAW_TPAGE | DRAW_UNK02 | DRAW_TRANSP;
}

// spray from a falling-water entity
void func_us_801C3A04_from_no4(Entity* self) {
    Primitive* prim;
    s32 i;
    s32 yOffset;
    s32 posX;
    s32 primIndex;
    s32 scrollY;

    if (!self->step) {
        InitializeEntity(g_EInitCommon);
        self->animSet = 0;
        primIndex = g_api.AllocPrimitives(PRIM_TILE, 0x40);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->flags |= FLAG_HAS_PRIMS;
        self->primIndex = primIndex;
        prim = &g_PrimBuf[primIndex];
        self->ext.et_801C12B0.prim = prim;
        while (prim != NULL) {
            prim->priority = 0x6C;
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
    }

    prim = self->ext.et_801C12B0.prim;
    posX = self->posX.i.hi;
    scrollY = g_Tilemap.scrollY.i.hi;
    self->ext.et_801C12B0.unk80++;
    yOffset = 0xE0 - scrollY;
    for (i = 0; i < 2; i++) {
        while (prim != NULL) {
            if (prim->drawMode == DRAW_HIDE) {
                prim->r0 = prim->b0 = prim->g0 = 0x80;
                prim->x0 = ((rand() % 0x10) * 2) + i + posX;
                prim->y0 = yOffset - 4 + (rand() & 7);
                prim->x1 = 0;
                LOH(prim->r1) = 0;
                prim->u0 = prim->v0 = 2;
                LOW(prim->r2) = 0x40000;
                LOW(prim->u1) = 0;
                LOW(prim->x3) = 0x4000 - (rand() & 0xFF0);
                LOW(prim->r3) = (rand() & 0x1F00) - 0xF80;
                prim->clut = 0x20;
                prim->drawMode = DRAW_UNK02;
                break;
            }
            prim = prim->next;
        }
    }

    prim = self->ext.et_801C12B0.prim;
    while (prim != NULL) {
        if (prim->drawMode != DRAW_HIDE) {
            LOH(prim->b1) = prim->x0;
            prim->y1 = prim->y0;
            LOW(prim->x1) -= LOW(prim->r2);
            LOW(prim->r1) += LOW(prim->u1);
            prim->x0 = LOH(prim->b1);
            prim->y0 = prim->y1;
            LOW(prim->r2) -= LOW(prim->x3);
            LOW(prim->u1) -= LOW(prim->r3);
            if (!--prim->clut) {
                prim->drawMode = DRAW_HIDE;
            }
        }
        prim = prim->next;
    }
}

// small splash plumes on a falling-water entity
void func_us_801C3CC4_from_no4(Entity* self) {
    s32 jitter;
    s32 posX;
    s32 scrollY;
    s32 base;
    s32 primIndex;
    Primitive* prim;

    if (!self->step) {
        InitializeEntity(g_EInitParticle);
        self->animSet = 0;
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 0x20);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->flags |= FLAG_HAS_PRIMS;
        self->primIndex = primIndex;
        prim = &g_PrimBuf[primIndex];
        self->ext.et_801C12B0.prim = prim;
        while (prim != NULL) {
            prim->clut = PAL_CC_STONE_EFFECT;
            prim->tpage = 0x1A;
            prim->u0 = prim->u2 = 0;
            prim->u1 = prim->u3 = 0x1E;
            prim->v0 = prim->v1 = 0x7C;
            prim->v2 = prim->v3 = 0x60;
            prim->priority = 0x9D;
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
    }

    prim = self->ext.et_801C12B0.prim;
    posX = self->posX.i.hi;
    scrollY = g_Tilemap.scrollY.i.hi;
    self->ext.et_801C12B0.unk80++;
    if (self->ext.et_801C12B0.unk80 >= 9) {
        self->ext.et_801C12B0.unk80 = 0;
    }
    base = 0xE0 - scrollY;
    while (prim != NULL) {
        if (prim->drawMode == DRAW_HIDE) {
            prim->y0 = prim->y1 = prim->y2 = prim->y3 = base;
            jitter = rand() & 3;
            prim->x2 = prim->x0 =
                small_plume_order[self->ext.et_801C12B0.unk80] * 4 + jitter -
                0x19 + posX;
            prim->x3 = prim->x1 = prim->x0 + 0x12;
            PGREY(prim, 0) = PGREY(prim, 1) = 0xFF;
            PGREY(prim, 2) = PGREY(prim, 3) = 0x80;
            prim->p1 = 0;
            prim->drawMode = DRAW_UNK_40 | DRAW_TPAGE2 | DRAW_TPAGE |
                             DRAW_COLORS | DRAW_UNK02 | DRAW_TRANSP;
            break;
        }
        prim = prim->next;
    }

    prim = self->ext.et_801C12B0.prim;
    while (prim != NULL) {
        if (prim->drawMode != DRAW_HIDE) {
            prim->y2++;
            if (rand() & 3) {
                prim->y2++;
            }
            prim->y3 = prim->y2;
            prim->p1++;
            prim->b1 -= 0x10;
            PGREY(prim, 0) = PGREY(prim, 1);
            prim->b3 -= 8;
            PGREY(prim, 2) = PGREY(prim, 3);
            if (prim->r0 < 0x11) {
                prim->drawMode = DRAW_HIDE;
            }
        }
        prim = prim->next;
    }
}

// foreground waterfalls, the texture upside down
void func_us_801C3FB0_from_no4(Entity* self) {
    s32 primIndex;
    s32 scrollX, scrollY;
    s16* ptr;
    s32 clut;
    s32 texV, x, height, y;
    Primitive* prim;

    if (!self->step) {
        InitializeEntity(g_EInitInteractable);
        self->animSet = 0;
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 0x10);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->ext.et_801C12B0.clut = 0;
        self->flags |= FLAG_HAS_PRIMS;
        self->primIndex = primIndex;
        prim = &g_PrimBuf[primIndex];
        self->ext.et_801C12B0.prim = prim;
        while (prim != NULL) {
            prim->tpage = 0xF;
            prim->u0 = prim->u2 = 0x82;
            prim->u1 = prim->u3 = 0x9D;
            prim->priority = 0xC0;
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
    }

    prim = self->ext.et_801C12B0.prim;
    ptr = fg_waterfalls;
    self->ext.et_801C12B0.clut++;
    if (self->ext.et_801C12B0.clut >= 0xE) {
        self->ext.et_801C12B0.clut = 0;
    }
    scrollX = g_Tilemap.scrollX.i.hi;
    scrollX *= 5;
    scrollX /= 4;
    scrollY = g_Tilemap.scrollY.i.hi;
    scrollY *= 5;
    scrollY /= 4;
    scrollY %= 0x20;
    clut = self->ext.et_801C12B0.clut + 0x90;

    while (prim != NULL) {
        x = *ptr++;
        x -= scrollX;
        if (x < -0x1B) {
            continue;
        }
        if (x >= 0x100) {
            break;
        }
        texV = scrollY + *ptr++;
        height = 0x60 - texV;
        y = 0;
        while (prim != NULL) {
            prim->clut = clut;
            prim->v0 = prim->v1 = 0x63 - texV;
            prim->v2 = prim->v3 = (0x63 - texV) - height;
            prim->x0 = prim->x2 = x;
            prim->x1 = prim->x3 = x + 0x1B;
            prim->y0 = prim->y1 = y;
            y += height;
            prim->y2 = prim->y3 = y;
            prim->drawMode =
                DRAW_UNK_40 | DRAW_TPAGE2 | DRAW_TPAGE | DRAW_TRANSP;
            prim = prim->next;
            if (y >= 0xE8) {
                break;
            }
            texV = 0;
            if (y > 0x88) {
                height = 0xE8 - y;
            } else {
                height = 0x60;
            }
        }
    }

    while (prim != NULL) {
        prim->drawMode = DRAW_HIDE;
        prim = prim->next;
    }
}

// light shaft at the cavern wall, mirrored for RNO4 (extends leftwards)
void func_us_801C4228_from_no4(Entity* self) {
    s32 scrollY, scrollX;
    s32 primIndex;
    s32 yOffset, yMax, xOffset;
    Primitive* prim;

    if (!self->step) {
        InitializeEntity(g_EInitInteractable);
        self->animSet = 0;
        primIndex = g_api.AllocPrimitives(PRIM_G4, 4);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->flags |= FLAG_HAS_PRIMS;
        self->primIndex = primIndex;
        prim = &g_PrimBuf[primIndex];
        self->ext.et_801C12B0.prim = prim;
        while (prim != NULL) {
            prim->priority = 0xC0;
            prim->r0 = prim->r2 = prim->g0 = prim->g2 = prim->r1 = prim->r3 =
                prim->g1 = prim->g3 = 0x10;
            prim->b0 = prim->b2 = prim->b1 = prim->b3 = 0;
            prim = prim->next;
        }
    }

    prim = self->ext.et_801C12B0.prim;
    scrollX = g_Tilemap.scrollX.i.hi;
    scrollY = g_Tilemap.scrollY.i.hi;
    if (scrollX < 0x70) {
        xOffset = PLAYER.posX.i.hi - 0x10;
    } else {
        xOffset = 0xE0 - scrollX;
    }
    yOffset = 0x30 - scrollY;
    yMax = yOffset + 0x80;
    if (yMax > 0 && xOffset >= -0xF) {
        prim->x0 = prim->x2 = xOffset;
        prim->x1 = prim->x3 = xOffset - 0x30;
        prim->y0 = prim->y1 = yOffset;
        prim->y2 = prim->y3 = yMax;
        prim->r1 = prim->r3 = prim->g1 = prim->g3 = prim->b1 = prim->b3 = 0xFF;
        prim->r0 = prim->r2 = prim->g0 = prim->g2 = prim->b0 = prim->b2 = 0;
        prim->drawMode =
            DRAW_UNK_40 | DRAW_TPAGE | DRAW_COLORS | DRAW_UNK02 | DRAW_TRANSP;
        prim = prim->next;

        prim->x0 = prim->x2 = xOffset - 1;
        xOffset -= 3;
        prim->x1 = prim->x3 = xOffset;
        prim->y0 = prim->y1 = yOffset;
        prim->y2 = prim->y3 = yMax;
        prim->r0 = prim->r2 = prim->g0 = prim->g2 = 7;
        prim->b0 = prim->b2 = prim->b1 = prim->b3 = 0;
        prim->r1 = prim->r3 = prim->g1 = prim->g3 = 8;
        prim->drawMode =
            DRAW_TPAGE2 | DRAW_TPAGE | DRAW_COLORS | DRAW_UNK02 | DRAW_TRANSP;
        prim->priority = 0xC1;
        prim = prim->next;

        prim->x0 = prim->x2 = xOffset;
        xOffset -= 0x2D;
        prim->x1 = prim->x3 = xOffset;
        prim->y0 = prim->y1 = yOffset;
        prim->y2 = prim->y3 = yMax;
        prim->r0 = prim->r2 = prim->g0 = prim->g2 = prim->r1 = prim->r3 =
            prim->g1 = prim->g3 = 0x10;
        prim->b0 = prim->b2 = prim->b1 = prim->b3 = 0;
        prim->drawMode =
            DRAW_TPAGE2 | DRAW_TPAGE | DRAW_COLORS | DRAW_UNK02 | DRAW_TRANSP;
        prim->priority = 0xC1;
        prim = prim->next;

        if (xOffset >= -0xF) {
            prim->x0 = prim->x2 = -0x10;
            prim->x1 = prim->x3 = xOffset;
            prim->y0 = prim->y1 = yOffset;
            prim->y2 = prim->y3 = yMax;
            prim->drawMode = DRAW_UNK02;
            prim = prim->next;
        }
    }

    while (prim != NULL) {
        prim->drawMode = DRAW_HIDE;
        prim = prim->next;
    }
}

static s16 water_direction; // -1 rising, 1 falling (bss in RNO4)

// the water level in the room before the inverted Scylla: rises once,
// then after a pause sets SCYLLA_DEFEATED itself and drains (BO3 instead
// waits for the flag)
void func_us_801A07CC_from_bo3(Entity* self) {
    water_direction = 0;
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitInteractable);
        if (water_level) {
            if (g_CastleFlags[SCYLLA_DEFEATED]) {
                self->step = 5;
            } else {
                self->step = 3;
            }
        } else if (g_CastleFlags[SCYLLA_DEFEATED]) {
            self->step = 6;
        }
        break;
    case 1:
        if (PLAYER.posX.i.hi >= 0x38) {
            self->ext.timer.t = 0x10;
            self->step++;
        }
        break;
    case 2:
        if (!--self->ext.timer.t) {
            self->step++;
        }
        break;
    case 3:
        if (self->ext.timer.t) {
            --self->ext.timer.t;
        } else {
            self->ext.timer.t = 3;
            if (water_level < 0xD0) {
                water_level++;
                water_direction = -1;
            } else {
                self->ext.timer.t = 0x80;
                self->step++;
            }
        }
        func_us_801A071C_from_bo3();
        break;
    case 4:
        if (!--self->ext.timer.t) {
            g_CastleFlags[SCYLLA_DEFEATED] = 1;
            self->step++;
        }
        break;
    case 5:
        if (self->ext.timer.t) {
            --self->ext.timer.t;
        } else {
            self->ext.timer.t = 1;
            if (water_level > 0) {
                water_level--;
                water_direction = 1;
            } else {
                self->step++;
            }
        }
        func_us_801A071C_from_bo3();
        break;
    }
}

// ---- the breakable wall ----

static void BreakableWallSetTiles(u16* tileLayoutPtr) {
    s32 tilePos = 0x9F;
    s32 i;

    // one tile per row, bottom to top
    for (i = 0; i < 4; i++, tilePos -= 0x10, tileLayoutPtr += 2) {
        g_Tilemap.fg[tilePos] = tileLayoutPtr[0];
    }
}

void EntityBreakableWall(Entity* self) {
    Entity* newEntity;
    s32 i;

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitInteractable);
        self->hitboxWidth = 8;
        self->hitboxHeight = 0x20;
        self->hitboxState = 2;
        BreakableWallSetTiles(
            &breakable_wall_tile_layout
                [g_CastleFlags[RNO4_SECRET_WALL_OPEN] ? 0x18 : 0]);
        if (g_CastleFlags[RNO4_SECRET_WALL_OPEN]) {
            DestroyEntity(self);
            break;
        }
        // fallthrough
    case 1:
        if (self->hitFlags) {
            PlaySfxPositional(0x644);
            self->step++;
        }
        break;
    case 2:
        self->ext.breakable.breakCount++;
        BreakableWallSetTiles(
            &breakable_wall_tile_layout[self->ext.breakable.breakCount * 8]);
        newEntity = AllocEntity(&g_Entities[224], &g_Entities[256]);
        if (newEntity != NULL) {
            CreateEntityFromEntity(E_EXPLOSION, self, newEntity);
            newEntity->params = 0x11;
        }
        self->ext.breakable.resetTimer = 0x14;
        self->step++;
        if (self->ext.breakable.breakCount == 3) {
            for (i = 0; i < 0x10; i++) {
                newEntity = AllocEntity(&g_Entities[224], &g_Entities[256]);
                if (newEntity != NULL) {
                    CreateEntityFromEntity(
                        E_BREAKABLE_WALL_DEBRIS, self, newEntity);
                    newEntity->posX.i.hi =
                        newEntity->posX.i.hi + 4 - (Random() & 0xF);
                    newEntity->posY.i.hi =
                        newEntity->posY.i.hi + 0x1F - i * 4;
                    newEntity->velocityX = FIX(0.5) - (Random() << 8);
                    newEntity->params = 1;
                }
            }
            g_api.PlaySfx(0x644);
            g_CastleFlags[RNO4_SECRET_WALL_OPEN] = 1;
            g_api.RevealSecretPassageAtPlayerPositionOnMap(
                RNO4_SECRET_WALL_OPEN);
            DestroyEntity(self);
        }
        break;
    case 3:
        if (!--self->ext.breakable.resetTimer) {
            self->step = 1;
        }
        break;
    }
}
