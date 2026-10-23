// SPDX-License-Identifier: AGPL-3.0-or-later
#include "rdai.h"

#include "../e_stained_glass_blend.h"

#include "../stained_glass_recurse.h"

#ifdef VERSION_PC
// Non-matching C for the PC port, transcribed from the US binary. These
// are DAI's EntityStainedGlass and EntityStainedGlassBackground for the
// inverted castle: every rotation is followed by a half turn around Z.
#include <scratchpad.h>
extern EInit g_EInitInteractable;

static SVECTOR glass_points_0 = {-208, -192, 0};
static SVECTOR glass_points_1 = {208, -192, 0};
static SVECTOR glass_points_2 = {-208, 192, 0};
static SVECTOR glass_points_3 = {208, 192, 0};
static SVECTOR light_points_0 = {-208, -96, 0};
static SVECTOR light_points_1 = {208, -96, 0};
static SVECTOR light_points_2 = {-208, 96, 0};
static SVECTOR light_points_3 = {208, 96, 0};
static VECTOR params[] = {
    {FLT(0.0625), 0, FLT(0.125), 3},   {FLT(0.0625), 0, FLT(0.28125), 2},
    {FLT(0.0625), 0, FLT(0.4375), 2},  {FLT(0.0625), 0, FLT(0.59375), 0},
    {FLT(0.0625), 0, FLT(0.75), 0},    {FLT(0.0625), 0, FLT(0.90625), 0},
    {FLT(-0.0625), 0, FLT(0.125), 3},  {FLT(-0.0625), 0, FLT(0.28125), 2},
    {FLT(-0.0625), 0, FLT(0.4375), 2}, {FLT(-0.0625), 0, FLT(0.59375), 0},
    {FLT(-0.0625), 0, FLT(0.75), 0},   {FLT(-0.0625), 0, FLT(0.90625), 0}};
static s16 y_vals[] = {1024,  1024,  1024,  1024,  1024,  1024,
                       -1024, -1024, -1024, -1024, -1024, -1024};
static CVECTOR colors[] = {
    {128, 128, 128, 32}, {120, 120, 120, 36}, {112, 112, 112, 40},
    {104, 104, 104, 48}, {96, 96, 96, 56},    {88, 88, 88, 64},
    {96, 96, 96, 4},     {88, 88, 88, 4},     {80, 80, 80, 4},
    {72, 72, 72, 4},     {64, 64, 64, 4},     {56, 56, 56, 4}};
static SVECTOR light_rot_vector = {0, 0, 0};
static SVECTOR bg_points_0 = {-192, -64, 0};
static SVECTOR bg_points_1 = {192, -64, 0};
static SVECTOR bg_points_2 = {-192, 64, 0};
static SVECTOR bg_points_3 = {192, 64, 0};
static SVECTOR bg_pos_vector = {0, 0, 0};
static VECTOR trans_vectors[] = {
    {0, 0, FLT(1)},       {0, 240, FLT(0.125)}, {0, 240, FLT(0.25)},
    {0, 240, FLT(0.375)}, {0, 240, FLT(0.5)},   {0, 240, FLT(0.625)},
    {0, 240, FLT(0.75)},  {0, 240, FLT(0.875)}, {0, 240, FLT(1)}};
static SVECTOR bg_rot_vector = {0, 0, 0};

static void SetMatrixAndTrans(MATRIX* mtx, VECTOR* trans) {
    TransMatrix(mtx, trans);
    SetRotMatrix(mtx);
    SetTransMatrix(mtx);
}

static void ProjectQuad(SVECTOR* p0, SVECTOR* p1, SVECTOR* p2, SVECTOR* p3,
                        Primitive* prim) {
    long p, flag;
    RotTransPers4(p0, p1, p2, p3, (long*)&prim->x0, (long*)&prim->x1,
                  (long*)&prim->x2, (long*)&prim->x3, &p, &flag);
}

static void CopyPrimKeepNext(Primitive* dst, Primitive* src) {
    Primitive* next = dst->next;
    *dst = *src;
    dst->next = next;
}

void func_us_801B2C28(Entity* self) {
    s16 midpointX, midpointY;
    s32 primIndex;
    SVECTOR rotVector;
    VECTOR transVector;
    MATRIX mtx;
    u8 transparent;
    s32 iterations;
    s16* yValsPtr;
    VECTOR* paramsPtr;
    CVECTOR* colorPtr;
    s32 count;
    Primitive* glassPrim;
    Primitive* lightPrim;

    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitInteractable);
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 60);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->flags |= FLAG_HAS_PRIMS;
        self->primIndex = primIndex;
        glassPrim = &g_PrimBuf[primIndex];
        self->ext.stainedGlass.glassPrim = glassPrim;
        for (count = 0; count < 12; count++) {
            glassPrim->tpage = 15;
            glassPrim->clut = 0xE;
            glassPrim->u0 = glassPrim->u2 = 4;
            glassPrim->u1 = glassPrim->u3 = 28;
            glassPrim->v0 = glassPrim->v1 = 1;
            glassPrim->v2 = glassPrim->v3 = 174;
            PGREY(glassPrim, 0) = 96;
            LOW(glassPrim->r1) = LOW(glassPrim->r0);
            LOW(glassPrim->r2) = LOW(glassPrim->r0);
            LOW(glassPrim->r3) = LOW(glassPrim->r0);
            glassPrim->priority = 86;
            glassPrim->drawMode = DRAW_TPAGE2 | DRAW_TPAGE | DRAW_TRANSP;
            glassPrim = glassPrim->next;
        }
        for (self->ext.stainedGlass.lightPrim = glassPrim; glassPrim != NULL;
             glassPrim = glassPrim->next) {
            glassPrim->drawMode = DRAW_HIDE;
        }
        break;
    case 1:
        SetGeomScreen(1024);
        SetGeomOffset(128, 160);
        glassPrim = self->ext.stainedGlass.glassPrim;
        lightPrim = self->ext.stainedGlass.lightPrim;
        paramsPtr = params;
        yValsPtr = y_vals;
        colorPtr = colors;
        for (count = 0; count < 12; count++) {
            rotVector.vx = 0;
            rotVector.vy = *yValsPtr;
            rotVector.vz = 0;
            RotMatrix(&rotVector, &mtx);
            RotMatrixZ(0x800, &mtx);
            transVector = *paramsPtr;
            if (transVector.vx > 0) {
                transparent = true;
                glassPrim->drawMode =
                    DRAW_TPAGE2 | DRAW_TPAGE | DRAW_COLORS | DRAW_TRANSP;
            } else {
                transparent = false;
                glassPrim->drawMode = DRAW_COLORS;
            }
            iterations = paramsPtr->pad;
            transVector.vx += self->posX.i.hi - 128;
            transVector.vy += self->posY.i.hi - 160;
            transVector.vz += FLT(0.25);
            SetMatrixAndTrans(&mtx, &transVector);
            CVEC(glassPrim->r0) = *colorPtr;
            glassPrim->type = PRIM_GT4;
            LOW(glassPrim->r1) = LOW(glassPrim->r0);
            LOW(glassPrim->r2) = LOW(glassPrim->r0);
            LOW(glassPrim->r3) = LOW(glassPrim->r0);
            if (iterations) {
                lightPrim = StainedGlassRecurse(
                    &glass_points_0, &glass_points_1, &glass_points_2,
                    &glass_points_3, glassPrim, iterations, lightPrim,
                    (u8*)SP(0));
                glassPrim->drawMode = DRAW_HIDE;
            }
            ProjectQuad(&glass_points_0, &glass_points_1, &glass_points_2,
                        &glass_points_3, glassPrim);
            if (transparent) {
                // shade behind the right side glass
                CopyPrimKeepNext(lightPrim, glassPrim);
                lightPrim->clut = PAL_FILL_WHITE;
                lightPrim->drawMode = DRAW_COLORS;
                lightPrim->priority = glassPrim->priority - 1;
                lightPrim->r0 = lightPrim->g0 = lightPrim->b0 = colorPtr->cd;
                LOW(lightPrim->r1) = LOW(lightPrim->r0);
                lightPrim->r2 = lightPrim->g2 = lightPrim->b2 =
                    colorPtr->cd >> 1;
                LOW(lightPrim->r3) = LOW(lightPrim->r2);
                lightPrim = lightPrim->next;

                // the light ray coming through it
                CopyPrimKeepNext(lightPrim, glassPrim);
                rotVector.vx = 0;
                rotVector.vy = *yValsPtr;
                rotVector.vz = 1024;
                RotMatrix(&light_rot_vector, &mtx);
                RotMatrixY(rotVector.vy, &mtx);
                RotMatrixZ(rotVector.vz, &mtx);
                RotMatrixZ(0x800, &mtx);
                transVector = *paramsPtr;
                transVector.vx = 96;
                transVector.vy = -256;
                transVector.vx += self->posX.i.hi - 128;
                transVector.vy += self->posY.i.hi - 160;
                transVector.vz += 1024;
                SetMatrixAndTrans(&mtx, &transVector);
                CVEC(lightPrim->r0) = *colorPtr;
                lightPrim->type = PRIM_GT4;
                LOW(lightPrim->r1) = LOW(lightPrim->r0);
                LOW(lightPrim->r2) = LOW(lightPrim->r0);
                LOW(lightPrim->r3) = LOW(lightPrim->r0);
                ProjectQuad(&light_points_0, &light_points_1, &light_points_2,
                            &light_points_3, lightPrim);
                lightPrim->clut = 0x200;
                lightPrim->drawMode = DRAW_UNK_40 | DRAW_TPAGE2 | DRAW_TPAGE |
                                      DRAW_COLORS | DRAW_TRANSP;
                midpointX = (lightPrim->x0 + lightPrim->x2) / 2;
                midpointY = (lightPrim->y0 + lightPrim->y1) / 2;
                lightPrim = lightPrim->next;

                CopyPrimKeepNext(lightPrim, glassPrim);
                lightPrim->clut = PAL_FILL_WHITE;
                lightPrim->drawMode =
                    DRAW_TPAGE2 | DRAW_TPAGE | DRAW_COLORS | DRAW_TRANSP;
                lightPrim->r0 = 24;
                lightPrim->g0 = 8;
                lightPrim->b0 = 32;
                LOW(lightPrim->r1) = LOW(lightPrim->r0);
                lightPrim->r2 = 8;
                lightPrim->g2 = 24;
                lightPrim->b2 = 8;
                lightPrim->r3 = lightPrim->g3 = lightPrim->b3 = 0;
                lightPrim->priority = glassPrim->priority + 1;
                LOW(lightPrim->x0) = LOW(glassPrim->x1);
                LOW(lightPrim->x1) = LOW(glassPrim->x0);
                LOW(lightPrim->x2) = LOW(glassPrim->x3);
                lightPrim->x3 = midpointX;
                lightPrim->y3 = midpointY;
                lightPrim = lightPrim->next;
            }
            colorPtr++;
            yValsPtr++;
            paramsPtr++;
            glassPrim = glassPrim->next;
        }
        for (count = 0; lightPrim; lightPrim = lightPrim->next) {
            lightPrim->drawMode = DRAW_HIDE;
            count++;
        }
        FntPrint("rest %x\n", count);
        break;
    }
}
#else
INCLUDE_ASM("st/rdai/nonmatchings/e_stained_glass", func_us_801B2C28);
#endif

#ifdef VERSION_PC
static void BgQuad(Primitive* prim, s16 x0, s16 x1, s16 yTop, s16 yBottom) {
    prim->x0 = prim->x2 = x0;
    prim->x1 = prim->x3 = x1;
    prim->y0 = prim->y1 = yTop;
    prim->y2 = prim->y3 = yBottom;
}

static void BgFill(Primitive* prim) {
    prim->r0 = 16;
    prim->g0 = 0;
    prim->b0 = 56;
    LOW(prim->r1) = LOW(prim->r0);
    LOW(prim->r2) = LOW(prim->r0);
    LOW(prim->r3) = LOW(prim->r0);
    prim->clut = PAL_FILL_WHITE;
    prim->priority = 81;
    prim->drawMode = DRAW_HIDE;
}

// Midpoint of two packed screen coordinates, as the PSX code computes it
static s32 PackedMid(s32 a, s32 b) {
    s32 m = ((a + b) / 2) & 0xFFFF0000;
    return m | (((s16)a + (s16)b) >> 1);
}

void func_us_801B3368(Entity* self) {
    VECTOR transVector;
    MATRIX mtx;
    RECT rect;
    CVECTOR color;
    s32 sxy[4];
    long p, flag;
    s32 primIndex;
    s32 idx;
    s32 mid1, mid2;
    u16 clut;
    s16 tempX, tempY;
    VECTOR* transVectorPtr;
    Primitive* prim;

    g_GpuBuffers[0].draw.r0 = 8;
    g_GpuBuffers[0].draw.g0 = 0;
    g_GpuBuffers[0].draw.b0 = 56;
    g_GpuBuffers[1].draw.r0 = 8;
    g_GpuBuffers[1].draw.g0 = 0;
    g_GpuBuffers[1].draw.b0 = 56;
    switch (self->step) {
    case 0:
        InitializeEntity(g_EInitInteractable);
        primIndex = g_api.AllocPrimitives(PRIM_GT4, 19);
        if (primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->flags |= FLAG_HAS_PRIMS;
        self->primIndex = primIndex;
        prim = &g_PrimBuf[primIndex];
        for (self->ext.stainedGlass.glassPrim = prim; prim != NULL;
             prim = prim->next) {
            PGREY(prim, 0) = 128;
            prim->tpage = 15;
            prim->clut = 15;
            prim->u0 = prim->u2 = 35;
            prim->u1 = prim->u3 = 92;
            prim->v0 = prim->v1 = 1;
            prim->v2 = prim->v3 = 62;
            prim->priority = 84;
            prim->drawMode = DRAW_DEFAULT;
            LOW(prim->r1) = LOW(prim->r0);
            LOW(prim->r2) = LOW(prim->r0);
            LOW(prim->r3) = LOW(prim->r0);
        }
        prim = self->ext.stainedGlass.glassPrim;
        for (idx = 0; idx < 4; idx++, prim = prim->next) {
            prim->clut = 5;
            prim->u0 = prim->u2 = 129;
            prim->u1 = prim->u3 = 174;
            prim->v0 = prim->v1 = 1;
            prim->v2 = prim->v3 = 126;
            prim->priority = 80;
        }
        prim = self->ext.stainedGlass.glassPrim;
        PGREY(prim, 0) = 32;
        PGREY(prim, 1) = 48;
        PGREY(prim, 2) = 64;
        PGREY(prim, 3) = 48;
        prim = prim->next;
        BgFill(prim);
        prim = prim->next;
        PGREY(prim, 0) = 64;
        PGREY(prim, 1) = 48;
        PGREY(prim, 2) = 80;
        PGREY(prim, 3) = 48;
        prim = prim->next;
        BgFill(prim);
        prim = prim->next;
        prim->type = PRIM_G4;
        prim->r0 = 64;
        prim->g0 = 24;
        prim->b0 = 72;
        prim->r2 = 200;
        prim->g2 = 24;
        prim->b2 = 104;
        LOW(prim->r1) = LOW(prim->r0);
        LOW(prim->r2) = LOW(prim->r3); // overwrites the colour set above
        prim->priority = 82;
        prim->drawMode = DRAW_COLORS | DRAW_UNK02;

        rect.x = 0;
        rect.y = 256;
        rect.w = 16;
        rect.h = 1;
        color.r = 8;
        color.g = 0;
        color.b = 56;
        StainedGlassBlendPalette(&rect, 15, 0x201, 16, &color);
        break;
    case 1:
        SetGeomScreen(1024);
        SetGeomOffset(128, 160);
        prim = self->ext.stainedGlass.glassPrim;
        clut = 0x201;
        for (transVectorPtr = trans_vectors, idx = 0; idx < 8;
             idx++, transVectorPtr++, clut += 2) {
            RotMatrix(&bg_rot_vector, &mtx);
            RotMatrixZ(0x800, &mtx);
            transVector = *transVectorPtr;
            transVector.vx += self->posX.i.hi - 128;
            transVector.vy += self->posY.i.hi - 160;
            transVector.vz += 1024;
            SetMatrixAndTrans(&mtx, &transVector);
            if (idx == 0) {
                RotTransPers(&bg_pos_vector, (long*)&sxy[0], &p, &flag);
                prim->drawMode = DRAW_COLORS;
                tempX = sxy[0];
                tempY = sxy[0] >> 16;
                BgQuad(prim, tempX - 45, tempX, tempY + 62, tempY - 62);
                prim = prim->next;
                BgQuad(prim, tempX - 45, tempX, tempY + 62, tempY - 62);
                prim->drawMode = DRAW_TPAGE | DRAW_COLORS | DRAW_TRANSP;
                prim = prim->next;
                BgQuad(prim, tempX + 45, tempX, tempY + 62, tempY - 62);
                prim->drawMode = DRAW_COLORS;
                prim = prim->next;
                BgQuad(prim, tempX + 45, tempX, tempY + 62, tempY - 62);
                prim->drawMode = DRAW_TPAGE | DRAW_COLORS | DRAW_TRANSP;
                prim = prim->next;
                prim->x0 = tempX - 8;
                prim->x1 = tempX + 8;
                prim->x2 = self->posX.i.hi - 24;
                prim->x3 = self->posX.i.hi + 24;
                prim->y0 = prim->y1 = tempY - 56;
                prim->y2 = prim->y3 = self->posY.i.hi - 194;
                prim->drawMode = DRAW_COLORS;
                prim = prim->next;
            } else {
                RotTransPers4(&bg_points_0, &bg_points_1, &bg_points_2,
                              &bg_points_3, (long*)&sxy[0], (long*)&sxy[1],
                              (long*)&sxy[2], (long*)&sxy[3], &p, &flag);
                if ((s16)(sxy[2] >> 16) <= 240) {
                    mid1 = PackedMid(sxy[0], sxy[1]);
                    mid2 = PackedMid(sxy[2], sxy[3]);
                    prim->clut = clut;
                    prim->drawMode = DRAW_DEFAULT;
                    LOW(prim->x0) = sxy[0];
                    LOW(prim->x1) = mid1;
                    LOW(prim->x2) = sxy[2];
                    LOW(prim->x3) = mid2;
                    prim = prim->next;
                    prim->clut = clut;
                    LOW(prim->x0) = sxy[1];
                    LOW(prim->x1) = mid1;
                    LOW(prim->x2) = sxy[3];
                    LOW(prim->x3) = mid2;
                    prim->drawMode = DRAW_DEFAULT;
                    prim = prim->next;
                }
            }
        }
        for (; prim != NULL; prim = prim->next) {
            prim->drawMode = DRAW_HIDE;
        }
        break;
    }
}
#else
INCLUDE_ASM("st/rdai/nonmatchings/e_stained_glass", func_us_801B3368);
#endif
