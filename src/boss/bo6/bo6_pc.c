// SPDX-License-Identifier: AGPL-3.0-or-later
// PC versions of the functions BO6 still has only as assembly: these follow
// the player Richter code in src/ric, retargeted to the boss entity (RIC,
// g_Ric) and to the constants the bo6 binary uses.
#include "bo6.h"

#ifdef VERSION_PC
#include "../../rebound_stone.h"

#define FAKEPRIM ((FakePrim*)prim)

// from src/ric/ric.h and the src/ric sources
enum BlueprintKind {
    B_DECORATION,
    B_WEAPON,
    B_WEAPON_UNIQUE,
    B_EFFECTS,
    B_WHIP,
    B_CUTSCENE_MARIA,
    B_WEAPON_CHILDREN,
    B_KIND_7,
    NUM_BLUEPRINT_KIND,
};
enum BlueprintOrigin {
    B_ORIGIN_DEFAULT,
    B_ORIGIN_FOLLOW_CAMERA,
    B_ORIGIN_FOLLOW_PLAYER,
    B_ORIGIN_FOLLOW_PLAYER_WHILE_PLAYER_IS_HIT,
    B_ORIGIN_FOLLOW_PLAYER_WHILE_PLAYER_IS_RUNNING,
    B_ORIGIN_5,
    B_ORIGIN_6,
    B_ORIGIN_FOLLOW_PLAYER_WHILE_PLAYER_IS_NOT_HIT,
    B_ORIGIN_FOLLOW_PARENT_ENTITY,
    B_ORIGIN_SUBWEAPON_CRASH_PARTICLE,
};
enum DeathKind {
    DEATH_GENERIC,
    DEATH_BY_FIRE,
    DEATH_BY_THUNDER,
    DEATH_BY_ICE,
};
typedef enum { AXE_INIT, AXE_FLYING, AXE_BOUNCE } AxeSteps;
typedef enum {
    DAGGER_INIT,
    DAGGER_FLYING,
    DAGGER_BOUNCE,
    DAGGER_HIT_ENEMY
} DaggerSteps;
#define BIBLE_PAGE_COUNT 6
#define LIGHTNING_COUNT 8


void RicStepStand(void);
void RicStepJump(void);
void RicStepCrouch(void);
void RicStepHit(s32 damageEffect, u32 damageKind, s16 prevStep, s32 prevStepS);
void RicStepDead(
    s32 damageEffects, s32 damageKind, s32 prevStep, s32 prevStepS);
void RicStepEnableFlameWhip(void);
void RicStepThrowDaggers(void);
void RicStepSlide(void);
void RicStepSlideKick(void);
void RicStepBladeDash(void);
void func_us_801B8E80(u16 arg0);
void RicStepHighJump(void);
void func_us_801B94CC(void);
s32 RicCheckFacing(void);
void RicSetInvincibilityFrames(s32 kind, s16 invincibilityFrames);
void RicSetCrouch(s32 kind, s32 velocityX);
void RicSetFall(void);
bool RicDoAttack(void);
bool RicDoCrash(void);
void RicSetSlide(void);
void RicSetSlideKick(void);
void func_us_801BA9D0(void);
bool RicCheckInput(s32 checks);
Entity* RicGetFreeEntityReverse(s16 start, s16 end);
void func_us_801BB314(s32 arg0);
void func_us_801BB370(Entity* entity);
s32 func_us_801BB5BC(Primitive* prim, s16 posX, s16 posY);
void RicEntityHitByHoly(Entity* entity);
void RicEntityHitByDark(Entity* entity);
void RicEntityFactory(Entity* self);
void func_us_801BC2F0(Entity* entity);
void func_us_801BC4F8(Entity* self);
void func_us_801BC5C8(Entity* self);
void func_us_801BC678(Entity* self);
void RicEntityHitByCutBlood(Entity* self);
void func_us_801BD384(Entity* self);
bool func_us_801BD47C(Entity* entity);
void func_us_801BE79C(Entity* self);
void RicEntityShrinkingPowerUpRing(Entity* self);
void RicEntityHitByIce(Entity* self);
void RicEntityHitByLightning(Entity* self);
void RicEntityWhip(Entity* self);
void RicEntityArmBrandishWhip(Entity* entity);
void RicEntitySubwpnHolyWaterBreakGlass(Entity* self);
void RicEntityCrashHydroStorm(Entity* self);
void DebugShowWaitInfo(const char* msg);
void DebugInputWait(const char* msg);
void RicEntitySubwpnHolyWaterFlame(Entity* self);
void RicEntitySubwpnCrashCross(Entity* self);
void func_us_801C488C(Entity* self);
void RicEntitySubwpnCrossTrail(Entity* self);
void RicEntitySubwpnCrashCrossParticles(Entity* self);
void RicEntitySubwpnThrownAxe(Entity* self);
void RicEntitySubwpnKnife(Entity* self);
void RicEntitySubwpnReboundStone(Entity* self);
void RicEntitySubwpnThrownVibhuti(Entity* self);
u8 BO6_PrimDecreaseBrightness(Primitive* prim, u8 amount);
void RicEntitySubwpnAgunea(Entity* self);
void RicEntityAguneaHitEnemy(Entity* self);
void RicEntityCrashVibhuti(Entity* self);
void func_us_801C8590(Entity* entity);
void RicEntityCrashReboundStoneExplosion(Entity* self);
void RicEntityCrashReboundStone(Entity* entity);
void RicEntityCrashBibleBeam(Entity* self);
void RicEntityCrashBible(Entity* self);
void func_us_801C9DE8(Entity* self);
s16 GetAguneaLightningAngle(s16* arg0, s16 arg1, s16 arg2, s16* arg3);
void AguneaShuffleParams(s32 bufSize, s32* buf);
void RicEntityAguneaLightning(Entity* self);
void RicEntityAguneaCircle(Entity* self);
void RicEntitySubwpnStopwatchCircle(Entity* self);
void EntityStopWatch(Entity* self);
void RicEntitySubwpnBible(Entity* self);
void RicEntityCrashCrossBeam(Entity* self);

extern AnimationFrame D_us_80182130[]; // D_801555A8
extern AnimationFrame D_us_801822B8[]; // ric_anim_brandish_whip
extern AnimationFrame D_us_80181F1C[]; // ric_anim_press_up
extern AnimationFrame D_us_801820E4[]; // D_8015555C
extern AnimationFrame D_us_801822C8[]; // D_80155740
extern AnimationFrame D_us_801820B0[]; // D_80155528
extern AnimationFrame D_us_80182068[]; // D_801554E0
extern AnimationFrame D_us_80182110[]; // D_80155588
extern AnimationFrame D_us_801822C0[]; // D_80155738
extern AnimationFrame D_us_80182048[]; // ric_anim_crouch
extern AnimationFrame D_us_8018224C[]; // D_801556C4
extern AnimationFrame D_us_80182224[]; // D_8015569C
extern AnimationFrame D_us_8018228C[]; // D_80155704
extern AnimationFrame D_us_8018221C[]; // ric_anim_stun
// a reduced RicStepHit: no stun timer, no cut/holy effects, no knock-down
extern AnimationFrame D_us_801820CC[]; // D_80155544
extern RECT D_us_8018127C; // D_801545A0
extern s16 D_us_801D07EC; // D_80174F6C
extern s16 D_us_801D07E8; // D_80174F68
extern u8 D_us_801CF3E8[0x1400]; // dead_dissolve_bmp
extern s32 D_us_801D07F0; // death_kind
extern s32 D_us_801D07F4; // the damage effects, kept for step_s 1
extern s32 D_us_80181278;
extern s32 D_us_801D07F8; // throw_dagger_timer
extern AnimationFrame D_us_80182310[]; // D_80155788
extern AnimationFrame D_us_801820BC[]; // D_80155534
extern u8 D_us_80181284[]; // D_801545B0
extern AnimationFrame D_us_80182058[]; // ric_anim_crouch_from_stand
extern AnimationFrame D_us_80182038[]; // ric_anim_crouch_from_stand2
extern AnimationFrame D_us_80182050[]; // ric_anim_land_from_air_run
extern AnimationFrame D_us_80182150[]; // D_801555C8
extern AnimationFrame D_us_8018246C[]; // D_801558E4
extern AnimationFrame D_us_801823C8[]; // ric_anim_flame_whip
extern AnimationFrame D_us_80182334[]; // ric_anim_point_finger_up
extern AnimationFrame D_us_801821E0[]; // ric_anim_stand_in_air
extern AnimationFrame D_us_80182190[]; // ric_anim_throw_daggers
extern AnimationFrame D_us_801822D8[]; // D_80155750
extern AnimationFrame D_us_80182304[]; // D_8015577C
extern AnimationFrame D_us_80182360[]; // ric_anim_blade_dash
extern AnimationFrame D_us_80182170[]; // D_801555E8
extern AnimationFrame D_us_801821C0[]; // D_80155638
extern u8 D_us_801812B8[][4]; // D_80154674
extern u8 D_us_801D07FC; // D_80174FAC
extern u8 D_us_801D0800; // D_80174FB0
extern u8 D_us_801D0804; // D_80174FB4
extern u8 D_us_801D0808; // D_80174FB8
extern SubweaponDef subweapons_def[];
// RicSetSubweaponParams without the attack scaling and the hit-tracking reset
extern u8 D_us_80181524[6][8]; // uv_anim_801548F4
extern Point16 D_us_801D080C[16]; // D_80174FBC
extern s32 D_us_801D084C; // D_80174FFC
extern AnimationFrame D_us_80181554[]; // anim_smoke_dark
extern FactoryBlueprint D_us_801816A0[]; // blueprints
extern u8 D_us_80181868[NUM_BLUEPRINT_KIND][2]; // entity_ranges
extern EInit D_us_8018043C;
// RicEntitySlideKick
extern EInit D_us_80180424;
// RicEntityBladeDash
extern EInit D_us_80180430;
// the high jump attack
extern s16 D_us_80181878[]; // pos_x_80154C50
extern s32 D_us_80181884[]; // velocity_x_80154C5C
extern s16 D_us_8018189C[]; // rot_x_80154C74
extern AnimationFrame D_us_801818A8[]; // anim_smoke_puff
// RicEntitySmokePuff without the wall-sensor variants
extern unkStr_8011E4BC* D_us_801819AC[]; // D_80154DA0
extern s32 D_us_801D11C8[];
extern s32 D_us_801D1248[];
extern AnimationFrame D_us_80181A40[]; // anim_80154E38
extern s16 D_us_80181CAC[][6]; // D_8015519C
extern Point16* D_us_80181D54[72]; // D_80155244
extern u16 D_us_80181E74[]; // lightning_clut
extern EInit D_us_8018040C;
extern EInit D_us_80180418;
extern s16 D_us_80182580[]; // D_80155A40
extern Point16 D_us_8018255C[]; // D_80155A1C
extern s16 D_us_80182548[]; // D_80155A08
extern Point16 D_us_80182524[]; // D_801559E4
extern s16 D_us_801825B8[]; // D_80155A78
extern Point16 D_us_80182594[]; // D_80155A54
extern Point16 D_us_80182508[]; // D_801559C8
extern Point16 D_us_801824EC[]; // D_801559AC
extern Point32 D_us_80182624[]; // D_80155AE4
extern u16 D_us_801827B0[3]; // D_80155C70
extern Point32 D_us_801825EC[]; // D_80155AAC
extern s16 D_us_801827B8[]; // D_80155C78
extern u32 D_us_801D0874; // D_80175080
extern s32 D_us_801D0878; // D_80175084
extern s16 D_us_801827D8[]; // D_80155C98
extern s32 D_us_8018266C[]; // D_80155B2C_0
extern s32 D_us_80182690[]; // D_80155B2C_1
extern s32 D_us_801826B4[]; // D_80155B2C_2
extern s32 D_us_801826D8[]; // D_80155B2C_3
extern s32 D_us_801826FC[]; // D_80155B2C_4
extern s32 D_us_80182720[]; // D_80155B2C_5
extern s32 D_us_80182744[]; // D_80155B2C_6
extern s32 D_us_80182768[]; // D_80155B2C_7
extern s32 D_us_8018278C[]; // D_80155B2C_8
extern u16 D_us_8018280C[]; // D_80155CCC
extern u16 D_us_801827F8[]; // D_80155CB8
extern u16 D_us_80182834[]; // D_80155CF4
extern u16 D_us_80182820[]; // D_80155CE0
extern u16 D_us_8018285C[]; // D_80155D1C
extern u16 D_us_80182848[]; // D_80155D08
extern s16 D_us_80182890[4][6]; // D_80155D64
extern EInit D_us_80180484;
extern s32 D_us_801D087C; // debug_wait_info_timer
extern s32 D_us_801828C8[]; // D_80155D9C
extern EInit D_us_8018046C;
extern EInit D_us_8018049C;
extern u16 D_us_80182908[]; // crash_cross_img_data
extern RECT D_us_80182968; // crash_cross_img_vram
extern s16 D_us_80182994[]; // D_80155E68
extern u8 D_us_8018299C[]; // D_80155E70
extern EInit D_us_80180478;
extern s32 D_us_801D10CC; // D_8017588C
extern s16 D_us_801829C4[]; // D_80155E98
// WARN drawMode 0x133 not in asm
extern Point16 D_us_801D10D0[BIBLE_PAGE_COUNT]; // bible_pages_pos
extern EInit D_us_801804CC;
extern u16 D_us_801D10E8; // the hitbox state held back until the beam fires
extern s16 D_us_80182A1C[BIBLE_PAGE_COUNT]; // bible_page_angles
extern s32 D_us_801D10EC; // bible_pages_volume
extern EInit D_us_801804C0;
extern s32 D_us_801D10F0[LIGHTNING_COUNT]; // g_AguneaParams
// bo6 keeps two flags where Richter's stopwatch has its parent pointer
// RicEntitySubwpnStopwatch without the crash's chain of stopwatches
// This variable is uninitialized. It's an error for PSP compiler.
// Maybe they tossed this "= 0" line as a quick workaround.
extern EInit D_us_801804A8;
extern u8 D_us_80182E88[46][5]; // D_8015635C, uv per prim
extern SVECTOR* D_us_80182BA8[][4]; // D_8015607C

void RicStepStand(void) {
    if (!RicCheckInput(CHECK_FALL | CHECK_FACING | CHECK_JUMP | CHECK_CRASH |
                       CHECK_ATTACK | CHECK_CROUCH | CHECK_SLIDE)) {
        DecelerateX(FIX(0.125));
        switch (RIC.step_s) {
        case 0:
            if (RicCheckFacing()) {
                func_us_801B9DE4();
            } else if (g_Ric.padPressed & PAD_UP) {
                RicSetAnimation(D_us_80181F1C);
                RIC.step_s = 1;
                break;
            }
            break;
        case 1:
            if (RicCheckFacing()) {
                func_us_801B9DE4();
            } else if (g_Ric.padPressed & PAD_UP) {
            } else {
                RicSetStand(0);
            }
            break;
        case 64:
            DisableAfterImage(1, 1);
            if (RIC.pose < 3) {
                RicCheckFacing();
                if (g_Ric.padPressed & PAD_DOWN) {
                    RIC.step = PL_S_CROUCH;
                    RIC.anim = D_us_80182130;
                    break;
                }
            }

            if (RIC.poseTimer < 0) {
                if (g_Ric.padPressed & PAD_SQUARE) {
                    RIC.step_s++;
                    g_Ric.unk46 = 2;
                    RicSetAnimation(D_us_801822B8);
                    RicCreateEntFactoryFromEntity(
                        g_CurrentEntity, BP_ARM_BRANDISH_WHIP, 0);
                    break;
                }
                g_Ric.unk46 = 0;
                RicSetStand(0);
            }
            break;
        case 65:
            DisableAfterImage(1, 1);
            if (g_Ric.padPressed & PAD_SQUARE) {
                break;
            }
            g_Ric.unk46 = 0;
            RicSetStand(0);
            break;
        case 66:
            DisableAfterImage(1, 1);
            if (RIC.pose < 3) {
                RicCheckFacing();
            }
            if (RIC.poseTimer < 0) {
                g_Ric.unk46 = 0;
                RicSetStand(0);
            }
            break;
        }
    }
}

void RicStepJump(void) {
    s32 facing;


    if ((g_Ric.vram_flag & TOUCHING_CEILING) &&
        (RIC.velocityY < FIX(-1))) {
        RIC.velocityY = FIX(-0.25);
        g_Ric.unk44 |= 0x20;
    }
    if (RicCheckInput(
            CHECK_GROUND | CHECK_FACING | CHECK_ATTACK | CHECK_GRAVITY_JUMP)) {
        return;
    }
    switch (RIC.step_s) {
    case 0:
        DecelerateX(FIX(0.0625));
        facing = RicCheckFacing();
        if (facing) {
            if (g_Ric.unk44 & 0x10) {
                RicSetSpeedX(FIX(2.25));
            } else {
                RicSetSpeedX(FIX(1.25));
            }
            g_Ric.unk44 &= ~4;
        } else {
            g_Ric.unk44 &= ~0x10;
            if ((RIC.pose < 2) && !(g_Ric.unk44 & 8) &&
                (g_Ric.unk44 & 4) && (g_Ric.padTapped & PAD_CROSS)) {
                RicSetAnimation(D_us_801820E4);
                RicSetSpeedX(FIX(-1.5));
                RIC.velocityY = FIX(-2.625);
                g_Ric.unk44 |= 0xA;
                g_Ric.unk44 &= ~4;
                RIC.step_s = 2;
            }
        }
        break;
    case 2:
        break;
    case 0x40:
        DisableAfterImage(1, 1);
        if (RIC.pose < 3) {
            facing = RicCheckFacing();
            if (facing) {
                if (g_Ric.unk44 & 0x10) {
                    RicSetSpeedX(FIX(2.25));
                } else {
                    RicSetSpeedX(FIX(1.25));
                }
                g_Ric.unk44 &= ~4;
            } else {
                g_Ric.unk44 &= ~0x10;
            }
        } else {
            if (((g_Ric.padPressed & PAD_RIGHT) && RIC.facingLeft == 0) ||
                ((g_Ric.padPressed & PAD_LEFT) && RIC.facingLeft)) {
                if (g_Ric.unk44 & 0x10) {
                    RicSetSpeedX(FIX(2.25));
                } else {
                    RicSetSpeedX(FIX(1.25));
                }
                RicSetSpeedX(FIX(1.25));
                g_Ric.unk44 &= ~4;
            } else {
                g_Ric.unk44 &= ~0x10;
            }
        }
        if (RIC.poseTimer < 0) {
            if (g_Ric.padPressed & PAD_SQUARE) {
                RIC.step_s += 1;
                g_Ric.unk46 = 2;
                RicSetAnimation(D_us_801822C8);
                RicCreateEntFactoryFromEntity(
                    g_CurrentEntity, BP_ARM_BRANDISH_WHIP, 0);
            } else {
                g_Ric.unk46 = 0;
                RIC.step_s = 0;
                RicSetAnimation(D_us_801820B0);
            }
        }
        break;
    case 0x41:
        DisableAfterImage(1, 1);
        if (!(g_Ric.padPressed & PAD_SQUARE)) {
            g_Ric.unk46 = 0;
            RIC.step_s = 0;
            RicSetAnimation(D_us_801820B0);
        }
        break;
    case 0x42:
        DisableAfterImage(1, 1);
        if (RIC.pose < 3) {
            facing = RicCheckFacing();
            if (facing) {
                if (g_Ric.unk44 & 0x10) {
                    RicSetSpeedX(FIX(2.25));
                } else {
                    RicSetSpeedX(FIX(1.25));
                }
                g_Ric.unk44 &= ~4;
            } else {
                g_Ric.unk44 &= ~0x10;
            }
        } else {
            if (((g_Ric.padPressed & PAD_RIGHT) && RIC.facingLeft == 0) ||
                ((g_Ric.padPressed & PAD_LEFT) && RIC.facingLeft)) {
                if (g_Ric.unk44 & 0x10) {
                    RicSetSpeedX(FIX(2.25));
                } else {
                    RicSetSpeedX(FIX(1.25));
                }
                RicSetSpeedX(FIX(1.25));
                g_Ric.unk44 &= ~4;
            } else {
                g_Ric.unk44 &= ~0x10;
            }
        }
        if (RIC.poseTimer < 0) {
            g_Ric.unk46 = 0;
            RIC.step_s = 0;
            RicSetAnimation(D_us_801820B0);
        }
        break;
    }
}

void RicStepCrouch(void) {
    if (RicCheckInput(CHECK_FALL | CHECK_FACING | CHECK_JUMP | CHECK_CRASH |
                      CHECK_ATTACK | CHECK_SLIDE)) {
        return;
    }
    DecelerateX(FIX(0.125));
    switch (RIC.step_s) {
    case 0x0:
        if (!(g_Ric.padPressed & PAD_DOWN)) {
            RicSetAnimation(D_us_80182068);
            RIC.step_s = 2;
            return;
        }
        break;
    case 0x1:
        if (!(g_Ric.padPressed & PAD_DOWN)) {
            if (RicCheckFacing()) {
                func_us_801B9DE4();
                return;
            }
            RIC.anim = D_us_80182068;
            RIC.step_s = 2;
            RIC.poseTimer = 1;
            RIC.pose = 2 - RIC.pose;
            return;
        }
    case 0x4:
        if (RIC.poseTimer != -1) {
            return;
        }
        RicSetAnimation(D_us_80182048);
        RIC.step_s = 0;
        return;
    case 0x2:
        if (RicCheckFacing()) {
            func_us_801B9DE4();
            return;
        }
        if (RIC.poseTimer == -1) {
            RicSetStand(0);
            return;
        }
        break;
    case 0x3:
        if (RIC.poseTimer < 0) {
            RicSetAnimation(D_us_80182048);
            RIC.step_s = 0;
            return;
        }
        break;
    case 0x40:
        DisableAfterImage(1, 1);
        if (RIC.pose < 3) {
            RicCheckFacing();
            if (!(g_Ric.padPressed & PAD_DOWN)) {
                RIC.step = PL_S_STAND;
                RIC.anim = D_us_80182110;
                return;
            }
        }
        if (RIC.poseTimer < 0) {
            if (g_Ric.padPressed & PAD_SQUARE) {
                g_Ric.unk46 = 2;
                RIC.step_s++;
                RicSetAnimation(D_us_801822C0);
                RicCreateEntFactoryFromEntity(
                    g_CurrentEntity, BP_ARM_BRANDISH_WHIP, 0);
                return;
            }
            g_Ric.unk46 = 0;
            RIC.step_s = 0;
            RicSetAnimation(D_us_80182048);
        }
        break;
    case 0x41:
        DisableAfterImage(1, 1);
        if (!(g_Ric.padPressed & PAD_SQUARE)) {
            g_Ric.unk46 = 0;
            RIC.step_s = 0;
            RicSetAnimation(D_us_80182048);
        }
        break;
    }
}

void RicStepHit(s32 damageEffect, u32 damageKind, s16 prevStep, s32 prevStepS) {
    switch (RIC.step_s) {
    case 0:
        RicResetPose();
        if (damageKind < DAMAGEKIND_16) {
            func_us_801B77D8();
        } else {
            RIC.entityRoomIndex = RIC.facingLeft;
        }
        if (damageEffect & ELEMENT_THUNDER) {
            RicCreateEntFactoryFromEntity(g_CurrentEntity, 0x30, 0);
            RIC.velocityY = FIX(-4);
            func_us_801B9ACC(FIX(-1.25));
            RIC.step_s = 1;
            RIC.anim = D_us_8018224C;
            g_Ric.damagePalette = PAL_FLAG(0x220);
            RicCreateEntFactoryFromEntity(g_CurrentEntity, 0x30, 0);
            RicCreateEntFactoryFromEntity(g_CurrentEntity, 0x460021, 0);
            g_Ric.timers[PL_T_2] = 6;
            g_api.PlaySfx(0x83B);
            return;
        }
        if (damageEffect & ELEMENT_ICE) {
            damageKind = 3;
        }
        if (damageKind == 2) {
            switch (prevStep) {
            case PL_S_STAND:
            case PL_S_WALK:
                RIC.velocityY = 0;
                func_us_801B9ACC(FIX(-1.25));
                RIC.step_s = 6;
                RIC.anim = D_us_80182224;
                g_api.PlaySfx(0x837);
                RicCreateEntFactoryFromEntity(g_CurrentEntity, 0, 0);
                break;
            case PL_S_CROUCH:
                RIC.velocityY = 0;
                func_us_801B9ACC(FIX(-1.25));
                RIC.step_s = 7;
                RIC.anim = D_us_8018228C;
                RicCreateEntFactoryFromEntity(g_CurrentEntity, 0, 0);
                g_api.PlaySfx(0x836);
                break;
            case PL_S_FALL:
            case PL_S_JUMP:
                RIC.velocityY = FIX(-3);
                func_us_801B9ACC(FIX(-1.25));
                RIC.step_s = 1;
                RIC.anim = D_us_8018221C;
                g_api.PlaySfx(0x838);
                break;
            }
        } else {
            switch (prevStep) {
            default:
            case PL_S_STAND:
            case PL_S_WALK:
                RIC.velocityY = FIX(-4);
                func_us_801B9ACC(FIX(-1.25));
                RIC.step_s = 1;
                RIC.anim = D_us_8018221C;
                g_api.PlaySfx(0x839);
                break;
            case PL_S_CROUCH:
                RIC.velocityY = 0;
                func_us_801B9ACC(FIX(-1.25));
                RIC.step_s = 7;
                RIC.anim = D_us_8018228C;
                RicCreateEntFactoryFromEntity(g_CurrentEntity, 0, 0);
                g_api.PlaySfx(0x83A);
                break;
            case PL_S_FALL:
            case PL_S_JUMP:
                RIC.velocityY = FIX(-3);
                func_us_801B9ACC(FIX(-1.25));
                RIC.step_s = 1;
                RIC.anim = D_us_8018221C;
                g_api.PlaySfx(0x702);
                g_api.PlaySfx(0x83A);
                break;
            }
        }
        g_Ric.damagePalette = PAL_FLAG(PAL_CC_RED_EFFECT_A);
        g_Ric.timers[PL_T_2] = 6;
        if (damageEffect & ELEMENT_FIRE) {
            RicCreateEntFactoryFromEntity(g_CurrentEntity, 0x10008, 0);
            RicCreateEntFactoryFromEntity(g_CurrentEntity, 9, 0);
            RicCreateEntFactoryFromEntity(g_CurrentEntity, 0x430021, 0);
            g_Ric.damagePalette = PAL_FLAG(PAL_CC_FIRE_EFFECT);
            g_Ric.timers[PL_T_2] = 0x10;
        } else if (damageEffect & ELEMENT_ICE) {
            RicCreateEntFactoryFromEntity(g_CurrentEntity, 0x2F, 0);
            g_Ric.timers[PL_T_2] = 0xC;
            g_Ric.damagePalette = PAL_FLAG(PAL_CC_BLUE_EFFECT_A);
        } else if (damageEffect & 0x800) {
            RicCreateEntFactoryFromEntity(g_CurrentEntity, 0x46, 0);
            RicCreateEntFactoryFromEntity(g_CurrentEntity, 0x560021, 0);
            g_Ric.timers[PL_T_2] = 0x10;
            g_Ric.damagePalette = PAL_FLAG(0x164);
        } else if (damageEffect & 0x1000) {
            RicCreateEntFactoryFromEntity(g_CurrentEntity, 0x47, 0);
            RicCreateEntFactoryFromEntity(g_CurrentEntity, 0x570021, 0);
            g_Ric.timers[PL_T_2] = 8;
            g_Ric.damagePalette = PAL_FLAG(0x168);
        } else if (!(damageEffect & 0xF840)) {
            RicCreateEntFactoryFromEntity(g_CurrentEntity, 0x530021, 0);
        }
        break;
    case 1:
        if ((g_Ric.vram_flag & TOUCHING_CEILING) && (RIC.velocityY < FIX(-1))) {
            RIC.velocityY = FIX(-1);
        }
        RicCheckInput(CHECK_80 | CHECK_GRAVITY_HIT | CHECK_GROUND_AFTER_HIT);
        break;
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
        DecelerateX(FIX(0.125));
        if (!(g_Ric.vram_flag & TOUCHING_GROUND)) {
            RicSetFall();
        }
        if (RIC.poseTimer < 0) {
            if (g_Ric.unk5C && g_Status.hp < 20) {
                RicSetDeadPrologue();
                return;
            }
            RicSetStand(RIC.velocityX);
        }
        break;
    case 7:
        DecelerateX(FIX(0.125));
        if (!(g_Ric.vram_flag & TOUCHING_GROUND)) {
            RicSetFall();
        }
        if (RIC.poseTimer < 0) {
            if (g_Ric.unk5C && g_Status.hp < 20) {
                RicSetDeadPrologue();
                return;
            }
            RicSetCrouch(0, RIC.velocityX);
        }
        break;
    }
}

void RicStepDead(
    s32 damageEffects, s32 damageKind, s32 prevStep, s32 prevStepS) {
    s32 j;
    s32 i;
    u8* s2;
    PlayerDraw* playerDraw = &g_PlayerDraw[8];

    switch (RIC.step_s) {
    case 0:
        RicResetPose();
        func_us_801B77D8();
        func_us_801B9ACC(FIX(-0.75));
        RicSetAnimation(D_us_801820CC);
        RIC.velocityY = FIX(-1.625);
        g_api.PlaySfx(0x83E);
        D_us_801D07F4 = damageEffects;
        RIC.step_s++;
        if (!g_CastleFlags[0x95] && g_DemoMode == Demo_None) {
            CreateEntityFromCurrentEntity(E_ID_17, &g_Entities[0xC8]);
            g_Entities[0xC8].params = 2;
            g_Entities[0xC8].flags = FLAG_UNK_10000;
        }
        D_us_80181278 = 10;
        RIC.palette = PAL_FLAG(0x220);
        return;
    case 1:
        if (D_us_801D07F4 & ELEMENT_FIRE) {
            func_us_801BB314(0);
            RicCreateEntFactoryFromEntity(g_CurrentEntity, 0x480021, 0);
            RicCreateEntFactoryFromEntity(g_CurrentEntity, 0x20035, 0);
            D_us_801D07F0 = DEATH_BY_FIRE;
        } else if (D_us_801D07F4 & ELEMENT_THUNDER) {
            func_us_801BB314(2);
            RicCreateEntFactoryFromEntity(g_CurrentEntity, 0x4C0021, 0);
            RicCreateEntFactoryFromEntity(g_CurrentEntity, 0x10030, 0);
            RicCreateEntFactoryFromEntity(g_CurrentEntity, 0x20030, 0);
            D_us_801D07F0 = DEATH_BY_THUNDER;
        } else if (D_us_801D07F4 & ELEMENT_ICE) {
            func_us_801BB314(3);
            RicCreateEntFactoryFromEntity(g_CurrentEntity, 0x4D0021, 0);
            RicCreateEntFactoryFromEntity(g_CurrentEntity, 0x2F, 0);
            RicCreateEntFactoryFromEntity(g_CurrentEntity, 0x1002F, 0);
            D_us_801D07F0 = DEATH_BY_ICE;
            RIC.blendMode = BLEND_TRANSP | BLEND_ADD;
        } else {
            func_us_801BB314(1);
            RicCreateEntFactoryFromEntity(g_CurrentEntity, 0x4A0021, 0);
            RicCreateEntFactoryFromEntity(g_CurrentEntity, 0x5000B, 0);
            D_us_801D07F0 = DEATH_GENERIC;
        }
        playerDraw->r0 = playerDraw->g0 = playerDraw->b0 = playerDraw->r1 =
            playerDraw->g1 = playerDraw->b1 = playerDraw->r2 = playerDraw->g2 =
                playerDraw->b2 = playerDraw->r3 = playerDraw->g3 =
                    playerDraw->b3 = 0x80;
        playerDraw->enableColorBlend = 1;
        RIC.step_s++;
        break;
    case 2:
        if (RIC.pose == 4) {
            RIC.step_s++;
        }
        break;
    case 3:
        RIC.velocityY += FIX(11.0 / 256);
        if (RIC.velocityY > FIX(1.0 / 16)) {
            RIC.velocityY >>= 2;
            RIC.velocityX >>= 3;
            StoreImage(&D_us_8018127C, (u_long*)D_us_801CF3E8);
            D_us_801D07EC = 0;
            D_us_801D07E8 = 0x40;
            RIC.step_s++;
        }
        break;
    case 4:
        if (g_Timer & 1) {
            break;
        }
        if (D_us_801D07E8 > 16) {
            RIC.velocityY += FIX(0.0086);
        } else {
            RIC.velocityX = 0;
            RIC.velocityY = 0;
        }
        for (i = 0; i < 4; i++) {
            s2 = D_us_801CF3E8;
            s2 += ((D_us_801D07EC >> 1) & 7);
            s2 += ((D_us_801D07EC & 0xFF) >> 4) << 6;
            for (j = 0; j < 0x28; j++) {
                if (D_us_801D07EC & 1) {
                    *(s2 + ((j & 7) * 8) + ((j >> 3) * 0x400)) &= 0xF0;
                } else {
                    *(s2 + ((j & 7) * 8) + ((j >> 3) * 0x400)) &= 0x0F;
                }
            }
            D_us_801D07EC += 0x23;
            D_us_801D07EC &= 0xFF;
        }
        LoadImage(&D_us_8018127C, (u_long*)D_us_801CF3E8);
        if (--D_us_801D07E8 == 0) {
            RIC.velocityY = 0;
            playerDraw->enableColorBlend = 0;
            RIC.step_s = 0x80;
        }
        break;
    case 0x80:
        break;
    }
    if (D_us_801D07F0 == DEATH_GENERIC) {
        if (playerDraw->r0 < 0xF8) {
            playerDraw->r0 += 2;
        }
        if (playerDraw->b0 > 8) {
            playerDraw->b0 -= 2;
        }
        playerDraw->r3 = playerDraw->r2 = playerDraw->r1 = playerDraw->r0;
        playerDraw->g0 = playerDraw->g1 = playerDraw->b1 = playerDraw->g2 =
            playerDraw->b2 = playerDraw->g3 = playerDraw->b3 = playerDraw->b0;
    }
    if (D_us_801D07F0 == DEATH_BY_FIRE || D_us_801D07F0 == DEATH_BY_THUNDER) {
        if (playerDraw->b0 > 8) {
            playerDraw->b0 -= 2;
        }
        playerDraw->r3 = playerDraw->r2 = playerDraw->r1 = playerDraw->r0 =
            playerDraw->g0 = playerDraw->g1 = playerDraw->b1 = playerDraw->g2 =
                playerDraw->b2 = playerDraw->g3 = playerDraw->b3 =
                    playerDraw->b0;
    }
    if (D_us_801D07F0 == DEATH_BY_ICE) {
        if ((playerDraw->r0 > 8) && (g_Timer & 1)) {
            playerDraw->r0--;
        }
        playerDraw->r3 = playerDraw->r2 = playerDraw->r1 = playerDraw->g3 =
            playerDraw->g2 = playerDraw->g1 = playerDraw->g0 = playerDraw->r0;
    }
}

void RicStepEnableFlameWhip(void) {
    if (RIC.animCurFrame == 181 && RIC.poseTimer == 1) {
        RicCreateEntFactoryFromEntity(g_CurrentEntity, BP_35, 0);
        g_api.PlaySfx(SFX_WEAPON_APPEAR);
    }

    if (RIC.poseTimer < 0) {
        RicSetStand(0);
        g_Ric.unk46 = 0;
        RicCreateEntFactoryFromEntity(
            g_CurrentEntity, FACTORY(BP_RIC_BLINK, 0x45), 0);
        g_Ric.timers[PL_T_POISON] = 0x800;
    }
}

void RicStepThrowDaggers(void) {
    if (RIC.step_s == 0) {
        D_us_801D07F8 = 0x200;
        RIC.step_s++;
    } else {
        RicCheckFacing();
        if (!--D_us_801D07F8) {
            g_Ric.unk46 = 0;
            RicSetStand(0);
            g_Ric.unk4E = 1;
        }
    }
    if (g_Ric.padTapped & PAD_CROSS) {
        func_us_801B9E70();
        g_Ric.unk46 = 0;
        g_Ric.unk4E = 1;
        D_us_801D07F8 = 0;
    }
}

void RicStepSlide(void) {
    s32 isTouchingGround = 0;

    if (RIC.facingLeft == 0 && g_Ric.vram_flag & TOUCHING_R_WALL) {
        isTouchingGround = 1;
    }
    if (RIC.facingLeft && g_Ric.vram_flag & TOUCHING_L_WALL) {
        isTouchingGround = 1;
    }
    if (RIC.posX.i.hi >= STAGE_WIDTH - 4 && RIC.facingLeft == 0) {
        isTouchingGround = 1;
    }
    if (RIC.posX.i.hi <= 4 && RIC.facingLeft) {
        isTouchingGround = 1;
    }
    // the boss reads Alucard's floor sensors here, as the original does
    if ((RIC.facingLeft == 0 &&
         g_Player.colFloor[2].effects & EFFECT_UNK_8000) ||
        (RIC.facingLeft && g_Player.colFloor[3].effects & EFFECT_UNK_8000)) {
        isTouchingGround = 1;
    }
    if (isTouchingGround && RIC.pose < 6) {
        RIC.pose = 6;
        if (RIC.velocityX > FIX(1)) {
            RIC.velocityX = FIX(2);
        }
        if (RIC.velocityX < FIX(-1)) {
            RIC.velocityX = FIX(-2);
        }
        RicCreateEntFactoryFromEntity(g_CurrentEntity, BP_SKID_SMOKE, 0);
    }
    if (RIC.pose < 5) {
        if (RicCheckInput(CHECK_FALL | CHECK_CRASH)) {
            return;
        }
        if (g_Ric.padTapped & PAD_CROSS) {
            RIC.posY.i.hi -= 4;
            RicSetSlideKick();
            return;
        }
    } else if (RIC.pose < 7) {
        if (RicCheckInput(CHECK_FALL | CHECK_CRASH | CHECK_SLIDE)) {
            return;
        }
    } else if (
        RicCheckInput(CHECK_FALL | CHECK_FACING | CHECK_CRASH | CHECK_SLIDE)) {
        return;
    }

    DecelerateX(FIX(0.125));

    switch (RIC.step_s) {
    case 0:
        if (!(g_GameTimer & 3) && RIC.pose < 6 && RIC.pose > 2) {
            RicCreateEntFactoryFromEntity(
                g_CurrentEntity, FACTORY(BP_SLIDE, 2), 0);
        }
        if (RIC.pose == 6 && RIC.poseTimer == 1) {
            RicCreateEntFactoryFromEntity(g_CurrentEntity, BP_SKID_SMOKE, 0);
        }
        if (RIC.poseTimer < 0) {
            RicSetCrouch(0, RIC.velocityX);
        }
        break;
    }
}

void RicStepSlideKick(void) {
    // If we are pressing square while in contact with an enemy
    // (as detected in g_Ric.unk44), we will bounce back.
    if (g_Ric.padPressed & PAD_SQUARE && g_Ric.unk44 & 0x80) {
        RIC.step = PL_S_JUMP;
        RicSetAnimation(D_us_801820E4);
        RicSetSpeedX(FIX(-1.5));
        RIC.velocityY = FIX(-3.5);
        g_Ric.unk44 |= (8 + 2);
        g_Ric.unk44 &= ~4;
        RIC.step_s = 2;
        return;
    }
    DecelerateX(FIX(0.0625));
    RIC.velocityY += 0x1000;

    if (g_Ric.vram_flag & TOUCHING_GROUND) {
        g_CurrentEntity->velocityX /= 2;
        RicCreateEntFactoryFromEntity(g_CurrentEntity, BP_SKID_SMOKE, 0);
        RIC.facingLeft++;
        RIC.facingLeft &= 1;
        RicSetCrouch(3, RIC.velocityX);
        g_api.PlaySfx(0x64B);
        return;
    }

    if (RIC.velocityX < 0) {
        if (g_Ric.padPressed & PAD_RIGHT) {
            DecelerateX(FIX(0.125));
        }
        if (RIC.velocityX > FIX(-3) ||
            (g_Ric.vram_flag & TOUCHING_L_WALL)) {
            RIC.facingLeft++;
            RIC.facingLeft &= 1;
            RIC.velocityX /= 2;
            RicSetAnimation(D_us_80182310);
            g_Ric.unk44 = 0xA;
            RIC.step_s = 2;
            RIC.step = PL_S_JUMP;
        }
    }
    if (RIC.velocityX > 0) {
        if (g_Ric.padPressed & PAD_LEFT) {
            DecelerateX(FIX(0.125));
        }
        if (RIC.velocityX < FIX(3) ||
            (g_Ric.vram_flag & TOUCHING_R_WALL)) {
            RIC.facingLeft++;
            RIC.facingLeft &= 1;
            RIC.velocityX /= 2;
            RicSetAnimation(D_us_80182310);
            g_Ric.unk44 = 0xA;
            RIC.step_s = 2;
            RIC.step = PL_S_JUMP;
        }
    }
}

void RicStepBladeDash(void) {
    DecelerateX(0x1C00);

    if (RIC.poseTimer < 0) {
        g_Ric.unk46 = 0;
        RicSetStand(0);
    } else if (RIC.pose >= 0x12 && !(g_Ric.vram_flag & TOUCHING_GROUND)) {
        g_Ric.unk46 = 0;
        RicSetFall();
    } else {
        if (!(g_GameTimer & 3) && RIC.pose < 0x12 &&
            g_Ric.vram_flag & TOUCHING_GROUND) {
            RicCreateEntFactoryFromEntity(
                g_CurrentEntity, FACTORY(BP_SLIDE, 2), 0);
        }

        if (RIC.pose == 18 && RIC.poseTimer == 1 &&
            (g_Ric.vram_flag & TOUCHING_GROUND)) {
            RicCreateEntFactoryFromEntity(g_CurrentEntity, BP_SKID_SMOKE, 0);
        }
    }
}

void func_us_801B8E80(u16 arg0) {
    s16 xMod = 3;
    if (RIC.facingLeft) {
        xMod = -xMod;
    }

    RIC.posY.i.hi -= 16;
    RIC.posX.i.hi += xMod;
    RicCreateEntFactoryFromEntity(g_CurrentEntity, FACTORY(BP_EMBERS, 1), 0);
    RIC.posY.i.hi += 16;
    RIC.posX.i.hi -= xMod;

    if (arg0 & 1) {
        g_api.ShakeCamera(SHAKE_Y_SMALL2);
        g_api.PlaySfx(SFX_WALL_DEBRIS_B);
    }
    if (arg0 & 2) {
        RIC.velocityX = 0;
        RIC.velocityY = 0;
    }
}

void RicStepHighJump(void) {
    bool loadAnim;


    loadAnim = false;
    g_Ric.high_jump_timer++;
    switch (RIC.step_s) {
    case 0:
        if (g_Ric.padPressed & (PAD_LEFT | PAD_RIGHT)) {
            if (RIC.facingLeft) {
                if (!(g_Ric.padPressed & PAD_LEFT)) {
                    DecelerateX(FIX(0.0625));
                }
            } else {
                if (!(g_Ric.padPressed & PAD_RIGHT)) {
                    DecelerateX(FIX(0.0625));
                }
            }
        } else {
            DecelerateX(FIX(0.0625));
        }

        if (g_Ric.vram_flag & TOUCHING_CEILING) {
            func_us_801B8E80(3);
            g_Ric.high_jump_timer = 0;
            RIC.step_s = 2;
        } else if (g_Ric.high_jump_timer > 0x1C) {
            RIC.step_s = 1;
            RIC.velocityY = -0x60000;
        }
        break;
    case 1:
        if (g_Ric.vram_flag & TOUCHING_CEILING) {
            RIC.step_s = 2;
            func_us_801B8E80(3);
            g_Ric.high_jump_timer = 0;
        } else {
            RIC.velocityY += 0x6000;
            if (RIC.velocityY > 0x8000) {
                loadAnim = true;
            }
        }
        break;
    case 2:
        if (g_Ric.high_jump_timer > 4) {
            loadAnim = true;
        }
        break;
    }

    if (loadAnim) {
        RicSetAnimation(D_us_801820BC);
        RicSetStep(PL_S_JUMP);
    }
}

void func_us_801B94CC(void) {
    byte stackpad[40];
    Primitive* prim;
    s32 i;

    if (g_Entities[STAGE_ENTITY_START + E_AFTERIMAGE_1].ext.afterImage.disableFlag) {
        return;
    }
    if ((g_Ric.padTapped & GAMEBUTTONS) ||
        ((g_Ric.padHeld ^ g_Ric.padPressed) & g_Ric.padHeld &
         GAMEBUTTONS) ||
        (RIC.velocityY > FIX(0.5))) {
        g_Entities[STAGE_ENTITY_START + E_AFTERIMAGE_1].ext.afterImage.index = 0;
        g_Entities[STAGE_ENTITY_START + E_AFTERIMAGE_1].ext.afterImage.timer = 0;
    } else {
        if (g_Entities[STAGE_ENTITY_START + E_AFTERIMAGE_1].ext.afterImage.index >=
            MaxAfterImageIndex) {
            return;
        }
        if (g_Entities[STAGE_ENTITY_START + E_AFTERIMAGE_1].ext.afterImage.timer == 0) {
            g_Entities[STAGE_ENTITY_START + E_AFTERIMAGE_1].ext.afterImage.timer =
                D_us_80181284[g_Entities[STAGE_ENTITY_START + E_AFTERIMAGE_1].ext.afterImage.index];
        }
        if (--g_Entities[STAGE_ENTITY_START + E_AFTERIMAGE_1].ext.afterImage.timer == 0) {
            g_Entities[STAGE_ENTITY_START + E_AFTERIMAGE_1].ext.afterImage.index++;
            g_Entities[STAGE_ENTITY_START + E_AFTERIMAGE_1].ext.afterImage.timer =
                D_us_80181284[g_Entities[STAGE_ENTITY_START + E_AFTERIMAGE_1].ext.afterImage.index];
        }
    }
    if (g_Entities[STAGE_ENTITY_START + E_AFTERIMAGE_1].pose) {
        g_Entities[STAGE_ENTITY_START + E_AFTERIMAGE_1].pose--;
        return;
    }
#if !defined(VERSION_PSP)
    prim = &g_PrimBuf[g_Entities[STAGE_ENTITY_START + E_AFTERIMAGE_1].primIndex];
#endif
    for (prim = &g_PrimBuf[g_Entities[STAGE_ENTITY_START + E_AFTERIMAGE_1].primIndex], i = 0;
         prim != NULL; i++, prim = prim->next) {
        if (i == g_Entities[STAGE_ENTITY_START + E_AFTERIMAGE_1].entityId) {
            prim->r0 = prim->g0 = prim->b0 = 0x80;
            prim->x0 = RIC.posX.i.hi;
            prim->y0 = RIC.posY.i.hi;
            prim->x1 = RIC.animCurFrame;
            prim->y1 = 0;
            prim->x2 = RIC.facingLeft;
            prim->y2 = RIC.palette;
        }
    }
    g_Entities[STAGE_ENTITY_START + E_AFTERIMAGE_1].pose = 2;
    g_Entities[STAGE_ENTITY_START + E_AFTERIMAGE_1].entityId++;
    if (g_Entities[STAGE_ENTITY_START + E_AFTERIMAGE_1].entityId >= MaxAfterImages) {
        g_Entities[STAGE_ENTITY_START + E_AFTERIMAGE_1].entityId = 0;
    }
}

s32 RicCheckFacing(void) {
    if (g_Ric.unk44 & 2) {
        return 0;
    }

    if (RIC.facingLeft == 1) {
        if (g_Ric.padPressed & PAD_RIGHT) {
            RIC.facingLeft = 0;
            g_Ric.unk4C = 1;
            return -1;
        } else if (g_Ric.padPressed & PAD_LEFT) {
            return 1;
        }
    } else {
        if (g_Ric.padPressed & PAD_RIGHT) {
            return 1;
        }
        if (g_Ric.padPressed & PAD_LEFT) {
            RIC.facingLeft = 1;
            g_Ric.unk4C = 1;
            return -1;
        }
    }
    return 0;
}

void RicSetInvincibilityFrames(s32 kind, s16 invincibilityFrames) {
    if (!kind) {
        RicCreateEntFactoryFromEntity(
            g_CurrentEntity, FACTORY(BP_CRASH_DAGGER, 0x15), 0);
        if (g_Ric.timers[PL_T_INVINCIBLE_SCENE] <= invincibilityFrames) {
            g_Ric.timers[PL_T_INVINCIBLE_SCENE] = invincibilityFrames;
        }
    } else if (g_Ric.timers[PL_T_INVINCIBLE] <= invincibilityFrames) {
        g_Ric.timers[PL_T_INVINCIBLE] = invincibilityFrames;
    }
}

void RicSetCrouch(s32 kind, s32 velocityX) {
    RicSetStep(PL_S_CROUCH);
    RicSetAnimation(D_us_80182048);
    RIC.velocityX = velocityX;
    RIC.velocityY = 0;
    if (kind == 1) {
        RIC.anim = D_us_80182038;
        RIC.step_s = 4;
    }
    if (kind == 2) {
        RIC.anim = D_us_80182058;
        RIC.step_s = 1;
    }
    if (kind == 3) {
        RIC.anim = D_us_80182050;
        RIC.step_s = 4;
    }
}

void RicSetFall(void) {
    if (g_Ric.prev_step != PL_S_RUN && g_Ric.prev_step != PL_S_SLIDE) {
        RIC.velocityX = 0;
    }
    if (g_Ric.prev_step != PL_S_WALK && g_Ric.prev_step != PL_S_RUN) {
        RicSetAnimation(D_us_801820BC);
    }
    if (g_Ric.prev_step == PL_S_RUN) {
        g_Ric.unk44 = 0x10;
    }
    RicSetStep(PL_S_FALL);
    RIC.velocityY = FIX(2);
    g_Ric.timers[PL_T_5] = 8;
    g_Ric.timers[PL_T_6] = 8;
    g_Ric.timers[PL_T_CURSE] = 0;
    g_Ric.timers[PL_T_8] = 0;
    if (g_Ric.prev_step == PL_S_SLIDE) {
        g_Ric.timers[PL_T_5] = g_Ric.timers[PL_T_6] = 0;
        RIC.pose = 2;
        RIC.poseTimer = 0x10;
        RIC.velocityX /= 2;
    }
}

bool RicDoAttack(void) {
    s32 i;
    s16 poisoned;
    s16 sfxGrunt;

    sfxGrunt = rand() % 6;
    if (RicDoSubweapon() == 0) {
        if (sfxGrunt == 0) {
            g_api.PlaySfx(0x82C);
        }
        if (sfxGrunt == 1) {
            g_api.PlaySfx(0x82D);
        }
        if (sfxGrunt == 2) {
            g_api.PlaySfx(0x82E);
        }
        if (sfxGrunt == 3) {
            g_api.PlaySfx(0x82F);
        }
        return true;
    }
    poisoned = g_Ric.timers[PL_T_POISON] != 0;
    for (i = STAGE_ENTITY_START + 16; i < STAGE_ENTITY_START + 31; i++) {
        DestroyEntity(&g_Entities[i]);
    }
    if (!RicCreateEntFactoryFromEntity(
            g_CurrentEntity, FACTORY(BP_WHIP, poisoned), 0)) {
        return false;
    }
    if (poisoned) {
        g_api.PlaySfx(0x6B5);
    } else {
        g_api.PlaySfx(0x825);
    }
    if (sfxGrunt == 0) {
        g_api.PlaySfx(0x82C);
    }
    if (sfxGrunt == 1) {
        g_api.PlaySfx(0x82D);
    }
    if (sfxGrunt == 2) {
        g_api.PlaySfx(0x82E);
    }
    if (sfxGrunt == 3) {
        g_api.PlaySfx(0x82F);
    }
    switch (RIC.step) {
    case PL_S_STAND:
    case PL_S_WALK:
        RIC.step = PL_S_STAND;
        RicSetAnimation(D_us_80182110);
        g_CurrentEntity->velocityX = 0;
        break;
    case PL_S_RUN:
        RIC.step = PL_S_STAND;
        RicSetAnimation(D_us_80182110);
        RicCreateEntFactoryFromEntity(g_CurrentEntity, BP_SKID_SMOKE, 0);
        break;
    case PL_S_CROUCH:
        RicSetAnimation(D_us_80182130);
        g_CurrentEntity->velocityX = 0;
        break;
    case PL_S_FALL:
    case PL_S_JUMP:
        RIC.step = PL_S_JUMP;
        RicSetAnimation(D_us_80182150);
        break;
    default:
        return false;
    }
    g_Ric.unk46 = 1;
    RIC.step_s = 0x40;
    g_Ric.timers[PL_T_ATTACK] = 4;
    return true;
}

bool RicDoCrash(void) {
    SubweaponDef subWpn;
    Entity* subWpnEnt = NULL;
    s16 subWpnID;

    subWpnID = RicCheckSubweapon(&subWpn, true, false);
    if (subWpnID < 0) {
        return 0;
    }
    if (subWpnID == PL_W_HOLYWATER && g_Ric.timers[PL_T_3]) {
        return 0;
    }
    if (subWpn.blueprintNum) {
        if (subWpnID == 1) {
            subWpnEnt = RicCreateEntFactoryFromEntity(
                g_CurrentEntity, FACTORY(subWpn.blueprintNum, 1), 0);
        } else {
            subWpnEnt = RicCreateEntFactoryFromEntity(
                g_CurrentEntity, FACTORY(subWpn.blueprintNum, 0), 0);
        }
    }
    if (subWpnEnt == NULL) {
        return 0;
    }
    g_Ric.unk46 = 4;
    g_Ric.unk4E = 0;
    RIC.velocityX = RIC.velocityY = 0;
    switch (subWpnID) {
    case SUBWPN_NONE:
        RicSetStep(PL_S_FLAME_WHIP);
        RicSetAnimation(D_us_801823C8);
        RicCreateEntFactoryFromEntity(g_CurrentEntity, FACTORY(BP_36, 1), 0);
        break;
    case SUBWPN_DAGGER:
        RicSetStep(PL_S_THROW_DAGGERS);
        RicSetAnimation(D_us_80182190);
        g_api.PlaySfx(0x827);
        RicCreateEntFactoryFromEntity(
            g_CurrentEntity, FACTORY(BP_RIC_BLINK, 2), 0);
        break;
    case SUBWPN_AXE:
        RicSetStep(PL_S_STAND_IN_AIR);
        RicSetAnimation(D_us_801821E0);
        RIC.velocityY = FIX(-4.6875);
        func_us_801B9C14();
        g_api.PlaySfx(0x827);
        RicCreateEntFactoryFromEntity(
            g_CurrentEntity, FACTORY(BP_RIC_BLINK, 2), 0);
        break;
    case SUBWPN_HOLYWATER:
        RicSetStep(PL_S_HYDROSTORM);
        RicSetAnimation(D_us_80182334);
        RicCreateEntFactoryFromEntity(
            g_CurrentEntity, FACTORY(BP_RIC_BLINK, 0x40), 0);
        RicCreateEntFactoryFromEntity(
            g_CurrentEntity, FACTORY(BP_RIC_BLINK, 0x47), 0);
        g_api.PlaySfx(0x828);
        break;
    case SUBWPN_REBNDSTONE:
    case SUBWPN_VIBHUTI:
    case SUBWPN_AGUNEA:
        RicSetStep(PL_S_SUBWPN_CRASH);
        RicSetAnimation(D_us_80182334);
        RicCreateEntFactoryFromEntity(
            g_CurrentEntity, FACTORY(BP_RIC_BLINK, 0x40), 0);
        RicCreateEntFactoryFromEntity(
            g_CurrentEntity, FACTORY(BP_RIC_BLINK, 0x47), 0);
        g_api.PlaySfx(0x827);
        break;
    case SUBWPN_BIBLE:
    case SUBWPN_STOPWATCH:
        RicSetStep(PL_S_SUBWPN_CRASH);
        RicSetAnimation(D_us_8018246C);
        RicCreateEntFactoryFromEntity(
            g_CurrentEntity, FACTORY(BP_RIC_BLINK, 0x40), 0);
        RicCreateEntFactoryFromEntity(
            g_CurrentEntity, FACTORY(BP_RIC_BLINK, 0x47), 0);
        g_api.PlaySfx(0x827);
        break;
    case SUBWPN_CROSS:
        RicSetStep(PL_S_STAND_IN_AIR);
        RicSetAnimation(D_us_801821E0);
        RIC.velocityY = FIX(-4.6875);
        func_us_801B9C14();
        g_api.PlaySfx(0x829);
        RicCreateEntFactoryFromEntity(
            g_CurrentEntity, FACTORY(BP_RIC_BLINK, 2), 0);
        break;
    }
    g_Ric.timers[PL_T_12] = 4;
    return 1;
}

void RicSetSlide(void) {
    RicCheckFacing();
    RicSetStep(PL_S_SLIDE);
    RicSetAnimation(D_us_801822D8);
    g_CurrentEntity->velocityY = 0;
    RicSetSpeedX(FIX(5.5));
    func_us_801B9C14();
    RicCreateEntFactoryFromEntity(g_CurrentEntity, BP_25, 0);
    g_api.PlaySfx(0x826);
    g_Ric.timers[PL_T_12] = 4;
}

void RicSetSlideKick(void) {
    g_Ric.unk44 = 0;
    RicSetStep(PL_S_SLIDE_KICK);
    RicSetAnimation(D_us_80182304);
    g_CurrentEntity->velocityY = FIX(-2);
    RicSetSpeedX(FIX(5.5));
    func_us_801B9C14();
    RicCreateEntFactoryFromEntity(g_CurrentEntity, BP_25, 0);
    g_api.PlaySfx(0x82C);
    g_Ric.timers[PL_T_12] = 4;
    RicCreateEntFactoryFromEntity(g_CurrentEntity, BP_31, 0);
}

void func_us_801BA9D0(void) {
    RicSetStep(PL_S_BLADEDASH);
    RicSetAnimation(D_us_80182360);
    g_CurrentEntity->velocityY = 0;
    RicSetSpeedX(FIX(5.5));
    g_Ric.unk46 = 5;
    g_Ric.timers[PL_T_12] = 4;
    RicCreateEntFactoryFromEntity(g_CurrentEntity, BP_BLADE_DASH, 0);
    func_us_801B9C14();
    g_api.PlaySfx(0x834);
    g_api.PlaySfx(0x707);
}

bool RicCheckInput(s32 checks) {
    s32 velYChange;
    u32 effects;

    if (checks & CHECK_FACING) {
        if (g_Ric.unk46 == 0) {
            RicCheckFacing();
        }
    }
    velYChange = 0;
    if (checks & CHECK_GRAVITY_FALL) {
        velYChange = FIX(28.0 / 128);
    }
    if (checks & CHECK_GRAVITY_JUMP) {
        if (RIC.velocityY < FIX(3.0 / 8) &&
            RIC.velocityY > FIX(-1.0 / 8) && !(g_Ric.unk44 & 0x20) &&
            (g_Ric.padPressed & PAD_CROSS)) {
            // Note that 5.6 is precisely 1/5 of 28.
            velYChange = FIX(5.6 / 128);
        } else {
            velYChange = FIX(28.0 / 128);
        }
    }
    if (checks & CHECK_GRAVITY_HIT) {
        if (RIC.velocityY < FIX(3.0 / 8) &&
            RIC.velocityY > FIX(-1.0 / 8)) {
            velYChange = FIX(14.0 / 128);
        } else {
            velYChange = FIX(28.0 / 128);
        }
    }

    RIC.velocityY += velYChange;
    if (RIC.velocityY > FIX(7)) {
        RIC.velocityY = FIX(7);
    }
    if ((checks & CHECK_80) && (g_Ric.vram_flag & TOUCHING_CEILING) &&
        (RIC.velocityY < FIX(-1))) {
        RIC.velocityY = FIX(-1);
    }
    if (RIC.velocityY >= 0) {
        if ((checks & CHECK_GROUND) && (g_Ric.vram_flag & TOUCHING_GROUND)) {
            switch (g_Ric.unk46) {
            case 0:
            default:
                if (g_Ric.unk44 & 8) {
                    RicSetCrouch(3, RIC.velocityX);
                    g_api.PlaySfx(SFX_STOMP_SOFT_A);
                    RicCreateEntFactoryFromEntity(
                        g_CurrentEntity, BP_SKID_SMOKE, 0);
                    return true;
                }
                if (RIC.velocityY > FIX(6.875)) {
                    RicSetCrouch(1, 0);
                    g_api.PlaySfx(SFX_STOMP_SOFT_A);
                    RicCreateEntFactoryFromEntity(
                        g_CurrentEntity, BP_SKID_SMOKE, 0);
                } else if (g_Ric.padPressed & (PAD_LEFT | PAD_RIGHT)) {
                    if (g_Ric.unk44 & 0x10) {
                        func_us_801B9D74();
                    } else {
                        func_us_801B9DE4();
                    }
                } else {
                    RicSetStand(0);
                }
                return true;
            case 1:
                if (RIC.velocityY > FIX(6.875)) {
                    RIC.step = PL_S_CROUCH;
                    RIC.anim = D_us_80182130;
                    g_api.PlaySfx(SFX_STOMP_HARD_B);
                    RicCreateEntFactoryFromEntity(
                        g_CurrentEntity, BP_SKID_SMOKE, 0);
                } else {
                    RIC.step = PL_S_STAND;
                    RIC.anim = D_us_80182110;
                    if (!(g_Ric.unk44 & 8)) {
                        RIC.velocityX = 0;
                    } else {
                        RicCreateEntFactoryFromEntity(
                            g_CurrentEntity, BP_SKID_SMOKE, 0);
                        g_api.PlaySfx(SFX_STOMP_HARD_B);
                    }
                }
                RIC.velocityY = 0;
                g_Ric.unk44 = 0;
                return true;
            case 2:
                if (RIC.velocityY > FIX(6.875)) {
                    RIC.step = PL_S_CROUCH;
                    RIC.anim = D_us_801822C0;
                    g_api.PlaySfx(SFX_STOMP_SOFT_A);
                    RicCreateEntFactoryFromEntity(
                        g_CurrentEntity, BP_SKID_SMOKE, 0);
                } else {
                    RIC.step = PL_S_STAND;
                    RIC.anim = D_us_801822B8;
                    if (!(g_Ric.unk44 & 8)) {
                        RIC.velocityX = 0;
                    } else {
                        RicCreateEntFactoryFromEntity(
                            g_CurrentEntity, BP_SKID_SMOKE, 0);
                        g_api.PlaySfx(SFX_STOMP_SOFT_A);
                    }
                }
                RIC.velocityY = 0;
                g_Ric.unk44 = 0;
                return true;
            case 3:
                if ((RIC.velocityY > FIX(6.875)) || (g_Ric.unk44 & 8)) {
                    g_api.PlaySfx(SFX_STOMP_SOFT_A);
                    RicCreateEntFactoryFromEntity(
                        g_CurrentEntity, BP_SKID_SMOKE, 0);
                }
                RIC.velocityY = 0;
                if (!(g_Ric.unk44 & 8)) {
                    RIC.velocityX = 0;
                }
                RIC.step = PL_S_STAND;
                RIC.anim = D_us_80182170;
                g_Ric.unk44 = 0;
                return true;
            }
        } else if (checks & CHECK_GROUND_AFTER_HIT &&
                   (g_Ric.vram_flag & TOUCHING_GROUND)) {
            RicSetCrouch(1, RIC.velocityX);
            g_api.PlaySfx(SFX_STOMP_SOFT_A);
            if (g_Ric.unk5C && (g_Status.hp < 2)) {
                RicSetDeadPrologue();
                return true;
            } else {
                RicCreateEntFactoryFromEntity(
                    g_CurrentEntity, BP_SKID_SMOKE, 0);
            }
            return true;
        }
    }
    if (checks & CHECK_FALL && !(g_Ric.vram_flag & TOUCHING_GROUND)) {
        if (g_Ric.unk46) {
            if (g_Ric.unk46 == 1) {
                RIC.step_s = 0x40;
                RIC.step = PL_S_JUMP;
                RIC.anim = D_us_80182150;
                return true;
            }
            if (g_Ric.unk46 == 2) {
                RIC.step_s = 0x41;
                RIC.step = PL_S_JUMP;
                RIC.anim = D_us_80182150;
                return true;
            }
            if (g_Ric.unk46 == 3) {
                RIC.step_s = 0x42;
                RIC.step = PL_S_JUMP;
                RIC.anim = D_us_801821C0;
                return true;
            }
        } else {
            RicSetFall();
            return true;
        }
    }
    if (g_Ric.unk46) {
        return false;
    }
    if (checks & CHECK_CRASH && (g_Ric.padTapped & PAD_TRIANGLE) &&
        RicDoCrash()) {
        return true;
    }
    if (checks & CHECK_SLIDE &&
        // Alucard's floor sensors, as in the original
        !(RIC.facingLeft == 0 &&
          g_Player.colFloor[2].effects & EFFECT_UNK_8000) &&
        !(RIC.facingLeft &&
          g_Player.colFloor[3].effects & EFFECT_UNK_8000) &&
        (RIC.posX.i.hi < (u8)-4 || RIC.facingLeft != 0) &&
        (RIC.posX.i.hi >= 5 || !RIC.facingLeft) &&
        g_Ric.padPressed & PAD_DOWN && g_Ric.padTapped & PAD_CROSS) {
        RicSetSlide();
        return true;
    }
    if (checks & CHECK_JUMP && (g_Ric.padTapped & PAD_CROSS)) {
        func_us_801B9E70();
        return true;
    }
    if (checks & CHECK_ATTACK && (g_Ric.padTapped & PAD_SQUARE) &&
        RicDoAttack()) {
        return true;
    }
    if (checks & CHECK_CROUCH && (g_Ric.padPressed & PAD_DOWN)) {
        RicSetCrouch(2, 0);
        return true;
    }
    return false;
}

Entity* RicGetFreeEntityReverse(s16 start, s16 end) {
    Entity* entity = &g_Entities[end - 1];
    s16 i;
    for (i = end - 1; i >= start; i--, entity--) {
        if (entity->entityId == E_NONE) {
            return entity;
        }
    }
    return NULL;
}

void func_us_801BB314(s32 arg0) {
    D_us_801D07FC = D_us_801812B8[arg0][0];
    D_us_801D0800 = D_us_801812B8[arg0][1];
    D_us_801D0804 = D_us_801812B8[arg0][2];
    D_us_801D0808 = D_us_801812B8[arg0][3];
}

void func_us_801BB370(Entity* entity) {
    SubweaponDef* subwpn = &subweapons_def[entity->ext.subweapon.subweaponId];
    if (g_Ric.timers[PL_T_INVINCIBLE_SCENE]) {
        entity->attack = subwpn->attack * 2;
    } else {
        entity->attack = subwpn->attack;
    }
    entity->attackElement = subwpn->attackElement;
    entity->hitboxState = subwpn->hitboxState;
    entity->nFramesInvincibility = subwpn->nFramesInvincibility;
    entity->stunFrames = subwpn->stunFrames;
    entity->hitEffect = subwpn->hitEffect;
    entity->entityRoomIndex = subwpn->entityRoomIndex;
}

s32 func_us_801BB5BC(Primitive* prim, s16 posX, s16 posY) {
    s16 offset;
    s32 ret;
    u8* uvAnim;

    ret = 0;
    uvAnim = D_us_80181524[0];
    if (prim->b0 >= 6) {
        prim->b0 = 0;
        ret = -1;
    }

    uvAnim = &uvAnim[prim->b0 * 8];

    if (prim->b0 >= 3) {
        offset = 4;
    } else {
        offset = 6;
    }

    prim->x0 = posX - offset;
    prim->y0 = posY - offset;
    prim->x1 = posX + offset;
    prim->y1 = posY - offset;
    prim->x2 = posX - offset;
    prim->y2 = posY + offset;
    prim->x3 = posX + offset;
    prim->y3 = posY + offset;
    prim->u0 = *uvAnim++;
    prim->v0 = *uvAnim++;
    prim->u1 = *uvAnim++;
    prim->v1 = *uvAnim++;
    prim->u2 = *uvAnim++;
    prim->v2 = *uvAnim++;
    prim->u3 = *uvAnim++;
    prim->v3 = *uvAnim;

    prim->b1++;
    if (!(prim->b1 & 1)) {
        prim->b0++;
    }
    return ret;
}

void RicEntityHitByHoly(Entity* entity) {
    Primitive* prim;
    s32 i;
    s32 temp;
    s16 hitboxX;
    s16 hitboxY;
    s16 temp_xRand;
    s16 temp_yRand;

    switch (entity->step) {
    case 0:
        entity->primIndex =
            (s16)g_api.AllocPrimitives(PRIM_GT4, LEN(D_us_801D080C));
        if (entity->primIndex == -1) {
            DestroyEntity(entity);
            return;
        }
        entity->flags =
            FLAG_HAS_PRIMS | FLAG_POS_CAMERA_LOCKED;
        hitboxX = RIC.posX.i.hi + RIC.hitboxOffX;
        hitboxY = RIC.posY.i.hi + RIC.hitboxOffY;
        prim = &g_PrimBuf[entity->primIndex];
        for (i = 0; i < LEN(D_us_801D080C); i++) {
            temp_xRand = hitboxX + rand() % 24 - 12;
            temp_yRand = hitboxY + rand() % 48 - 24;
            D_us_801D080C[i].x = temp_xRand;
            D_us_801D080C[i].y = temp_yRand;
            prim->clut = PAL_UNK_1B2;
            prim->tpage = 0x1A;
            prim->b0 = 0;
            prim->b1 = 0;
            prim->g0 = 0;
            prim->g1 = (rand() & 7) + 1;
            prim->g2 = 0;
            prim->priority = RIC.zPriority + 4;
            prim->drawMode = DRAW_UNK_100 | DRAW_TPAGE | DRAW_HIDE |
                             DRAW_UNK02 | DRAW_TRANSP;
            if (rand() & 1) {
                prim->drawMode =
                    DRAW_UNK_100 | DRAW_UNK_40 | DRAW_TPAGE2 | DRAW_TPAGE |
                    DRAW_HIDE | DRAW_UNK02 | DRAW_TRANSP;
            }
            prim = prim->next;
        }
        entity->step++;
        break;
    case 1:
        if (!(g_Ric.status & PLAYER_STATUS_UNK10000)) {
            DestroyEntity(entity);
            return;
        }
        break;
    }

    prim = &g_PrimBuf[entity->primIndex];
    for (i = 0; i < 16; i++) {
        switch (prim->g0) {
        case 0:
            if (--prim->g1 == 0) {
                prim->g0++;
            }
            break;
        case 1:
            hitboxX = D_us_801D080C[i].x;
            hitboxY = D_us_801D080C[i].y;
            temp = func_us_801BB5BC(prim, hitboxX, hitboxY);
            D_us_801D080C[i].y--;
            if (temp < 0) {
                prim->drawMode |= DRAW_HIDE;
                prim->g0++;
            } else {
                prim->drawMode &= ~DRAW_HIDE;
            }
            break;
        }
        prim = prim->next;
    }
}

void RicEntityHitByDark(Entity* entity) {
    s16 x, y;

    switch (entity->step) {
    case 0:
        entity->flags =
            FLAG_UNK_20000000 | FLAG_POS_CAMERA_LOCKED;
        entity->unk5A = 0x79;
        entity->animSet = ANIMSET_DRA(14);
        entity->zPriority = RIC.zPriority + 2;
        entity->palette = PAL_FLAG(PAL_UNK_19F);
        if (D_us_801D084C & 1) {
            entity->blendMode = BLEND_TRANSP | BLEND_QUARTER;
        } else {
            entity->blendMode = BLEND_TRANSP;
        }
        D_us_801D084C++;
        entity->opacity = 0xFF;
        entity->drawFlags =
            ENTITY_SCALEX | ENTITY_SCALEY | ENTITY_MASK_R | ENTITY_MASK_G;
        entity->scaleX = entity->scaleY = 0x40;
        entity->anim = D_us_80181554;
        entity->posY.i.hi += (rand() % 35) - 15;
        entity->posX.i.hi += (rand() % 20) - 10;
        entity->velocityY = -0x6000 - (rand() & 0x3FFF);
        entity->step++;
        break;
    case 1:
        if (entity->opacity > 16) {
            entity->opacity -= 8;
        }
        entity->posY.val += entity->velocityY;
        entity->scaleX += 8;
        entity->scaleY += 8;
        if (entity->poseTimer < 0) {
            DestroyEntity(entity);
        }
        break;
    }
}

#define g_Entities_64 (&g_Entities[STAGE_ENTITY_START])
void RicEntityFactory(Entity* self) {
    Entity* newEntity;
    s16 nPerCycle;
    s16 i;
    s16 startIndex;
    s16 endIndex;
    u8* data;

    if (self->step == 0) {
        data = (u8*)&D_us_801816A0[self->params];
        self->ext.factory.newEntityId = *data++;
        self->ext.factory.amount = *data++;
        self->ext.factory.nPerCycle = *data & 0x3F;
        self->ext.factory.isNonCritical = (s16)(*data >> 7) & 1;
        self->ext.factory.incParamsKind = (s16)(*data++ >> 6) & 1;
        self->ext.factory.tCycle = *data++;
        self->ext.factory.kind = *data & 0x7;
        self->ext.factory.origin = (s16)(*data++ >> 3) & 0x1F;
        self->ext.factory.delay = *data;
        self->flags |= FLAG_UNK_10000000;
        self->step++;
        switch (self->ext.factory.origin) {
        case B_ORIGIN_DEFAULT:
            self->flags |= FLAG_POS_CAMERA_LOCKED;
            break;
        case B_ORIGIN_FOLLOW_PLAYER_WHILE_PLAYER_IS_RUNNING:
        case B_ORIGIN_FOLLOW_PLAYER:
        case B_ORIGIN_SUBWEAPON_CRASH_PARTICLE:
            self->flags |= FLAG_POS_CAMERA_LOCKED;
        case B_ORIGIN_FOLLOW_PLAYER_WHILE_PLAYER_IS_HIT:
        case B_ORIGIN_FOLLOW_PLAYER_WHILE_PLAYER_IS_NOT_HIT:
            self->posX.val = RIC.posX.val;
            self->posY.val = RIC.posY.val;
            break;
        case B_ORIGIN_FOLLOW_PARENT_ENTITY:
            if (self->ext.factory.parent) {
                self->posX.val = self->ext.factory.parent->posX.val;
                self->posY.val = self->ext.factory.parent->posY.val;
            }
            self->flags |= FLAG_POS_CAMERA_LOCKED;
            break;
        }
    } else {
        switch (self->ext.factory.origin) {
        case B_ORIGIN_DEFAULT:
            break;
        case B_ORIGIN_SUBWEAPON_CRASH_PARTICLE:
            if (g_Ric.unk4E) {
                DestroyEntity(self);
                return;
            }
        case B_ORIGIN_FOLLOW_PLAYER:
            self->posX.val = RIC.posX.val;
            self->posY.val = RIC.posY.val;
            break;
        case B_ORIGIN_FOLLOW_PLAYER_WHILE_PLAYER_IS_RUNNING:
            self->posX.val = RIC.posX.val;
            self->posY.val = RIC.posY.val;
            if (RIC.step != PL_S_RUN) {
                self->entityId = 0;
                return;
            }
            break;
        case B_ORIGIN_FOLLOW_PLAYER_WHILE_PLAYER_IS_HIT:
            self->posX.val = RIC.posX.val;
            self->posY.val = RIC.posY.val;
            if (RIC.step == PL_S_HIT) {
                self->entityId = 0;
                return;
            }
            break;
        case B_ORIGIN_FOLLOW_PLAYER_WHILE_PLAYER_IS_NOT_HIT:
            self->posX.val = RIC.posX.val;
            self->posY.val = RIC.posY.val;
            if (RIC.step != PL_S_HIT) {
                self->entityId = 0;
                return;
            }
            break;
        case B_ORIGIN_FOLLOW_PARENT_ENTITY:
            if (self->ext.factory.parent) {
                self->posX.val = self->ext.factory.parent->posX.val;
                self->posY.val = self->ext.factory.parent->posY.val;
            }
            break;
        }
    }
    if (self->ext.factory.delay) {
        if (--self->ext.factory.delay) {
            return;
        }
        self->ext.factory.delay = self->ext.factory.tCycle;
    }
    nPerCycle = self->ext.factory.nPerCycle;
    for (i = 0; i < nPerCycle; i++) {
        data = D_us_80181868[0];
        data += self->ext.factory.kind * 2;
        startIndex = *data++;
        endIndex = *data;
        if (self->ext.factory.kind == B_DECORATION) {
            newEntity = RicGetFreeEntityReverse(startIndex, endIndex + 1);
        } else if (self->ext.factory.kind == B_WHIP) {
            newEntity = &g_Entities_64[31];
        } else if (self->ext.factory.kind == B_CUTSCENE_MARIA) {
            newEntity = &g_Entities_64[48];
        } else {
            newEntity = RicGetFreeEntity(startIndex, endIndex + 1);
        }
        if (newEntity == NULL) {
            if (self->ext.factory.isNonCritical == 1) {
                self->entityId = 0;
            } else {
                self->ext.factory.delay = self->ext.factory.tCycle;
            }
            return;
        }
        DestroyEntity(newEntity);
        newEntity->entityId =
            self->ext.factory.newEntityId + self->ext.factory.entityIdMod;
        newEntity->params = self->ext.factory.paramsBase;
        // The child  (newEntity) is not an ent factory, but because the
        // factory creates many entities, we can't pick a particular extension.
        // But we're not allowed to use generic, so i'll just reuse entFactory.
        newEntity->ext.factory.parent = self->ext.factory.parent;
        newEntity->posX.val = self->posX.val;
        newEntity->posY.val = self->posY.val;
        newEntity->facingLeft = self->facingLeft;
        newEntity->zPriority = self->zPriority;

        if (self->ext.factory.incParamsKind) {
            newEntity->params += self->ext.factory.spawnIndex;
        } else {
            newEntity->params += i;
        }
        self->ext.factory.spawnIndex++;
        if (self->ext.factory.spawnIndex == self->ext.factory.amount) {
            self->entityId = 0;
            return;
        }
    }
    self->ext.factory.delay = self->ext.factory.tCycle;
}

void func_us_801BC2F0(Entity* entity) {
    if (RIC.step != PL_S_SLIDE) {
        DestroyEntity(entity);
        return;
    }
    entity->posX.i.hi = RIC.posX.i.hi;
    entity->posY.i.hi = RIC.posY.i.hi;
    entity->facingLeft = RIC.facingLeft;
    if (entity->step == 0) {
        InitializeEntity(D_us_8018043C);
        entity->flags = FLAG_POS_CAMERA_LOCKED | FLAG_UNK_10000000;
        entity->hitboxOffX = 0x14;
        entity->hitboxOffY = 0xC;
        entity->hitboxWidth = entity->hitboxHeight = 9;
        entity->step = 1;
        entity->ext.subweapon.timer = entity->hitboxState;
    }
    entity->hitboxState = entity->ext.subweapon.timer;
    if (RIC.pose < 2) {
        entity->hitboxState = 0;
    }
    if (RIC.pose > 7) {
        DestroyEntity(entity);
    }
}

void func_us_801BC4F8(Entity* self) {
    if (RIC.step != PL_S_BLADEDASH) {
        DestroyEntity(self);
        return;
    }
    self->posX.i.hi = RIC.posX.i.hi;
    self->posY.i.hi = RIC.posY.i.hi;
    self->facingLeft = RIC.facingLeft;
    if (self->step == 0) {
        InitializeEntity(D_us_80180424);
        self->flags = FLAG_POS_CAMERA_LOCKED | FLAG_UNK_10000000;
        self->hitboxWidth = self->hitboxHeight = 12;
        self->hitboxOffX = self->hitboxOffY = 0;
        self->ext.subweapon.subweaponId = 0x11;
        self->step++;
    }
    if (RIC.pose > 18) {
        DestroyEntity(self);
    }
}

void func_us_801BC5C8(Entity* self) {
    if (RIC.step != PL_S_HIGHJUMP) {
        DestroyEntity(self);
        return;
    }
    self->posX.i.hi = RIC.posX.i.hi;
    self->posY.i.hi = RIC.posY.i.hi;
    self->facingLeft = RIC.facingLeft;
    if (self->step == 0) {
        InitializeEntity(D_us_80180430);
        self->flags = FLAG_POS_CAMERA_LOCKED | FLAG_UNK_10000000;
        self->hitboxOffX = 0xC;
        self->hitboxOffY = -0x1A;
        self->hitboxWidth = 12;
        self->hitboxHeight = 12;
        self->step = 1;
    }
}

void func_us_801BC678(Entity* self) {
    s16 posX;
    s16 paramsHi = self->params >> 8;
    s16 paramsLo = self->params & 0xFF;

    switch (self->step) {
    case 0:
        self->animSet = 5;
        self->anim = D_us_801818A8;
        self->flags = FLAG_POS_CAMERA_LOCKED | FLAG_UNK_20000000;
        self->blendMode = BLEND_TRANSP | BLEND_ADD;
        self->drawFlags = ENTITY_SCALEX | ENTITY_SCALEY | ENTITY_OPACITY;
        self->opacity = 0x60;
        self->zPriority = RIC.zPriority + 2;
        posX = D_us_80181878[paramsLo];
        if (paramsHi == 0) {
            posX += 6;
        }
        if (paramsHi == 1) {
            posX -= 8;
        }
        if (paramsHi == 2) {
            posX -= 6;
        }
        if (paramsHi == 5) {
            posX = -6;
        }
        if (paramsHi == 3) {
            self->posY.i.hi -= 8;
        }
        if (paramsHi == 4) {
            DestroyEntity(self);
            return;
        }
        if (paramsHi == 1 && (g_Ric.vram_flag & TOUCHING_ANY_SLOPE)) {
            posX /= 2;
        }
        if (self->facingLeft) {
            posX = -posX;
        }
        self->posY.i.hi += 0x18;
        self->posX.i.hi += posX;
        self->scaleX = D_us_8018189C[paramsLo] + 0x40;
        self->velocityY = D_us_80181884[paramsLo];
        if (paramsHi == 1) {
            self->velocityY = FIX(-0.25);
            RicSetSpeedX(-0x3000);
            self->scaleX = D_us_8018189C[1] + 0x40;
        }
        if (paramsHi == 5) {
            self->velocityY = D_us_80181884[4 - paramsLo * 2];
        }
        if (paramsHi == 2) {
            self->velocityY = FIX(-0.5);
            RicSetSpeedX(-0x3000);
            self->scaleX = D_us_8018189C[1] + 0x40;
        }
        self->scaleY = self->scaleX;
        self->step++;
        break;
    case 1:
        self->opacity -= 2;
        self->posY.val += self->velocityY;
        self->posX.val += self->velocityX;
        if (self->poseTimer < 0) {
            DestroyEntity(self);
            return;
        }
        break;
    }
}

void RicEntityHitByCutBlood(Entity* self) {
    byte stackpad[0x28];
    u8 thickness;
    s16 variant;
    s16 playerX;
    s16 playerY;
    s16 velocityScale;
    s16 velocityRnd;
    s32 timer;
    s16 x;
    s16 y;
    s16 rnd;
    s32 i;
    unkStr_8011E4BC* props;
    FakePrim* tilePrim;

    variant = (self->params) >> 8;
    props = D_us_801819AC[variant];
    x = self->posX.i.hi;
    y = self->posY.i.hi;
    playerX = RIC.posX.i.hi;
    playerY = RIC.posY.i.hi;
    switch (self->step) {
    case 0:
        self->primIndex = g_api.func_800EDB58(PRIM_TILE_ALT, props->count + 1);
        if (self->primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->flags = props->flags;
        switch (props->unkA) {
        case 8:
            self->ext.et_8011E4BC.unk7C = 0x100;
            break;
        case 7:
            self->ext.et_8011E4BC.unk7C = 0x3F;
            break;
        case 4:
            self->ext.et_8011E4BC.unk7C = 0x2F;
            break;
        case 3:
        case 6:
            self->ext.et_8011E4BC.unk7C = 0x1F;
            break;
        }
        tilePrim = (FakePrim*)&g_PrimBuf[self->primIndex];
        i = 0;
        while (1) {
            tilePrim->drawMode = props->drawMode;
            tilePrim->priority = RIC.zPriority + props->priority;
            if (tilePrim->next == NULL) {
                tilePrim->drawMode &= ~DRAW_HIDE;
                tilePrim->y0 = tilePrim->x0 = tilePrim->w = 0;
                break;
            }
            tilePrim->posX.i.hi = x;
            tilePrim->posY.i.hi = y;
            tilePrim->posX.i.lo = tilePrim->posY.i.lo = 0;
            switch (props->unkA) {
            case 0:
                velocityRnd = rand() & PSP_RANDMASK;
                velocityScale = (velocityRnd & 1) + 2;
                tilePrim->velocityX.val = (rcos(velocityRnd) << velocityScale);
                tilePrim->velocityY.val = -(rsin(velocityRnd) << velocityScale);
                break;
            case 1:
                tilePrim->velocityX.val = ((rand() & 0x1FF) - 0x100) << 8;
                tilePrim->velocityY.val = ((rand() & 0x1FF) - 0x100) << 8;
                break;
            case 3:
                tilePrim->posX.i.hi = x + (rand() & 0xF) - 7;
                tilePrim->posY.i.hi = y - (rand() & 0xF);
                tilePrim->velocityY.val = FIX(-0.75) - (rand() & 0x7FFF);
                if (self->ext.et_8011E4BC.parent) {
                    tilePrim->velocityX.val =
                        self->ext.et_8011E4BC.parent->velocityX;
                }
                tilePrim->delay = (rand() & 0xF) + 0x10;
                break;
            case 4:
                rnd = rand() & 0x1F;
                tilePrim->posX.i.hi = x + rnd - 0x10;
                rnd = rand() & 0x1F;
                tilePrim->posY.i.hi = y + rnd - 0x14;
                rnd = rand() & 0x1F;
                tilePrim->velocityX.val = D_us_801D11C8[rnd];
                tilePrim->velocityY.val = D_us_801D1248[rnd];
                break;
            case 6:
                tilePrim->posX.i.hi = self->posX.i.hi + (rand() & 0xF) - 8;
                tilePrim->posY.i.hi = self->posY.i.hi + (rand() & 0xF) - 4;
                tilePrim->velocityY.val = (rand() & PSP_RANDMASK) + FIX(0.5);
                tilePrim->delay = (rand() & 0xF) + 0x10;
                break;
            case 7:
                tilePrim->posX.i.hi = x;
                tilePrim->posY.i.hi = y;
                if (i < 10) {
                    tilePrim->velocityY.val =
                        -((i * i * FIX(0.09375)) + FIX(0.125));
                } else {
                    tilePrim->velocityY.val =
                        (i - 10) * (i - 10) * FIX(0.09375) + FIX(0.125);
                }
                tilePrim->delay = 0x3F;
                break;
            case 8:
                tilePrim->posX.i.hi = x;
                tilePrim->posY.i.hi = y;
#if defined(VERSION_PSP)
                tilePrim->velocityX.val = ((rand() & 0x7FFF) - FIX(0.25)) >> 1;
#else
                tilePrim->velocityX.val = (rand() - FIX(0.25)) >> 1;
#endif
                tilePrim->velocityY.val = -((rand() & 0x1FFF) + FIX(0.375));
                tilePrim->timer = (i * 4);
                break;
            }
            tilePrim->x0 = tilePrim->posX.i.hi;
            tilePrim->y0 = tilePrim->posY.i.hi;
            tilePrim->r0 = props->r;
            tilePrim->g0 = props->g;
            tilePrim->b0 = props->b;
            tilePrim->w = props->w;
            tilePrim->h = props->h;
            i++;
            tilePrim = tilePrim->next;
        }
        self->step++;
        break;
    case 1:
        switch (props->unkA) {
        case 3:
        case 6:
        case 7:
        case 8:
            if (--self->ext.et_8011E4BC.unk7C == 0) {
                DestroyEntity(self);
                return;
            }
        case 4:
            if (--self->ext.et_8011E4BC.unk7C == 0) {
                DestroyEntity(self);
                return;
            }
            thickness = 3;
            timer = self->ext.et_8011E4BC.unk7C;
            if (timer < 9) {
                thickness--;
            }
            if (timer < 4) {
                thickness--;
            }
        }
        tilePrim = (FakePrim*)&g_PrimBuf[self->primIndex];
        i = 0;
        while (1) {
            if (tilePrim->next == NULL) {
                tilePrim->drawMode &= ~DRAW_HIDE;
                tilePrim->y0 = tilePrim->x0 = tilePrim->w = 0;
                return;
            }
            tilePrim->posX.i.hi = tilePrim->x0;
            tilePrim->posY.i.hi = tilePrim->y0;
            switch (props->unkA) {
            case 0:
                tilePrim->posY.val += tilePrim->velocityY.val;
                tilePrim->posX.val += tilePrim->velocityX.val;
                tilePrim->r0 -= 6;
                tilePrim->g0 -= 6;
                tilePrim->b0 -= 6;
                if (tilePrim->r0 < 8) {
                    DestroyEntity(self);
                    return;
                }
                break;
            case 1:
                tilePrim->posY.val += tilePrim->velocityY.val;
                tilePrim->posX.val += tilePrim->velocityX.val;
                tilePrim->velocityY.val =
                    (tilePrim->velocityY.val + FIX(0.15625));
                tilePrim->r0 -= 3;
                tilePrim->g0 -= 3;
                tilePrim->b0 -= 3;
                if (tilePrim->r0 < 8) {
                    DestroyEntity(self);
                    return;
                }
                break;
            case 3:
            case 6:
            case 7:
                tilePrim->posY.val += tilePrim->velocityY.val;
                if (--tilePrim->delay < 0) {
                    tilePrim->drawMode |= DRAW_HIDE;
                }
                break;
            case 4:
                tilePrim->posX.val += tilePrim->velocityX.val;
                tilePrim->posY.val += tilePrim->velocityY.val;
                // There is probably a clever way to write this
                tilePrim->velocityY.val -= tilePrim->velocityY.val >> 5;
                if (!(timer & 7)) {
                    tilePrim->velocityX.val >>= 1;
                    tilePrim->velocityY.val >>= 1;
                    if (timer & 0x20) {
                        tilePrim->velocityY.val >>= 1;
                    }
                    if (timer == 0x18) {
                        tilePrim->drawMode = DRAW_UNK02;
                    }
                }
                tilePrim->w = tilePrim->h = thickness;
                break;
            case 8:
                if (tilePrim->timer == 0) {
                    tilePrim->drawMode &= ~DRAW_HIDE;
                    tilePrim->r0 -= 1;
                    tilePrim->g0 -= 1;
                    tilePrim->b0 -= 1;
                    tilePrim->posY.val += tilePrim->velocityY.val;
                    tilePrim->posX.val += tilePrim->velocityX.val;
                    if (g_unkGraphicsStruct.D_80097448 == 0 ||
                        !(tilePrim->posY.i.hi >
                          (RIC.posY.i.hi - g_unkGraphicsStruct.D_80097448 +
                           0x19))) {
                        tilePrim->drawMode |= DRAW_HIDE;
                    }
                } else {
                    tilePrim->timer--;
                }
                break;
            }
            tilePrim->x0 = tilePrim->posX.i.hi;
            tilePrim->y0 = tilePrim->posY.i.hi;
            i++;
            tilePrim = tilePrim->next;
        }
    }
}

void func_us_801BD384(Entity* self) {
    switch (self->step) {
    case 0:
        self->animSet = ANIMSET_DRA(2);
        self->anim = D_us_80181A40;
        self->flags = FLAG_UNK_20000000 | FLAG_POS_CAMERA_LOCKED;
        self->zPriority = RIC.zPriority + 4;
        self->velocityY = (rand() & 0x3FFF) - 0x10000;
        self->step++;
        break;
    case 1:
        if (self->pose == 6) {
            if (self->poseTimer == 1 && (rand() & 1)) {
                RicCreateEntFactoryFromEntity(self, BP_EMBERS, 0);
            }
        }
        self->posY.val += self->velocityY;
        if (self->poseTimer < 0) {
            DestroyEntity(self);
        }
        break;
    }
}

bool func_us_801BD47C(Entity* entity) {
    Entity* e;
    s32 i;
    s16 objId;
    s16 params;

    objId = entity->entityId;
    params = entity->params;
    for (e = &g_Entities[STAGE_ENTITY_START + E_WEAPON],
        i = STAGE_ENTITY_START + E_WEAPON; i < STAGE_ENTITY_START * 2; e++,
        i++) {
        if (objId == e->entityId && params == e->params && e != entity) {
            return true;
        }
    }
    return false;
}

void func_us_801BE79C(Entity* self) {
    Primitive* prim;
    s16 primIndex;

    self->posX.i.hi = RIC.posX.i.hi - 0;
    self->posY.i.hi = RIC.posY.i.hi - 8;
    switch (self->step) {
    case 0:
        self->primIndex = g_api.AllocPrimitives(PRIM_GT4, 1);
        if (self->primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->ext.circleExpand.width = 16;
        self->ext.circleExpand.height = 12;
        prim = &g_PrimBuf[self->primIndex];
        prim->u0 = prim->u2 = 64;
        prim->v0 = prim->v1 = 192;
        prim->u1 = prim->u3 = 127;
        prim->v2 = prim->v3 = 255;
        PGREY(prim, 0) = PGREY(prim, 1) = PGREY(prim, 2) = PGREY(prim, 3) = 128;
        prim->tpage = 0x1A;
        prim->clut = PAL_CC_FIRE_EFFECT;
        prim->priority = RIC.zPriority + 8;
        prim->drawMode = DRAW_TPAGE2 | DRAW_TPAGE | DRAW_COLORS | DRAW_TRANSP;
        self->flags =
            FLAG_POS_CAMERA_LOCKED | FLAG_UNK_10000000 | FLAG_HAS_PRIMS;
        self->step++;
        break;

    case 1:
        self->ext.circleExpand.width += 2;
        self->ext.circleExpand.height += 2;
        if (self->ext.circleExpand.width > 56) {
            DestroyEntity(self);
            return;
        }
        break;
    }

    prim = &g_PrimBuf[self->primIndex];
    prim->x0 = self->posX.i.hi - self->ext.circleExpand.width;
    prim->y0 = self->posY.i.hi - self->ext.circleExpand.height;
    prim->x1 = self->posX.i.hi + self->ext.circleExpand.width;
    prim->y1 = self->posY.i.hi - self->ext.circleExpand.height;
    prim->x2 = self->posX.i.hi - self->ext.circleExpand.width;
    prim->y2 = self->posY.i.hi + self->ext.circleExpand.height;
    prim->x3 = self->posX.i.hi + self->ext.circleExpand.width;
    prim->y3 = self->posY.i.hi + self->ext.circleExpand.height;
    if (prim->b3 >= 12) {
        prim->b3 -= 12;
    }
    // remember last element of PGREY(prim,3) is prim->b3
    PGREY(prim, 0) = PGREY(prim, 1) = PGREY(prim, 2) = PGREY(prim, 3);
}

void RicEntityShrinkingPowerUpRing(Entity* self) {
    s16 variant;
    s16 rMod;
    s16* loadedParams;
    s16 x;
    s16 gMod;
    s16 bMod;
    s16 gOffset;
    s16 bOffset;
    s16 angleStart;
    s16 y;
    s16 baseAngle;
    s32 i;
    s16 angle;
    Primitive* prim1;
    Primitive* prim2;

    variant = (self->params & 0x7F00) >> 8;
    loadedParams = D_us_80181CAC[variant];
    rMod = loadedParams[2];
    gMod = loadedParams[3];
    bMod = loadedParams[4];
    gOffset = loadedParams[0];
    bOffset = loadedParams[1];
    self->posX.i.hi = RIC.posX.i.hi;
    self->posY.i.hi = RIC.posY.i.hi;
    switch (self->step) {
    case 0:
        self->primIndex = g_api.AllocPrimitives(PRIM_GT4, 32);
        if (self->primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->flags = FLAG_UNK_10000000 | FLAG_HAS_PRIMS | FLAG_POS_CAMERA_LOCKED;
        x = self->posX.i.hi;
        y = self->posY.i.hi;
        prim2 = prim1 = &g_PrimBuf[self->primIndex];
        for (i = 0; i < 16; i++) {
            prim1 = prim1->next;
        }
        for (i = 0; i < 16; i++) {
            angle = i << 8;
            prim2->u0 = (rcos(angle) >> 4 << 5 >> 8) + 0x20;
            prim2->v0 = -(rsin(angle) >> 4 << 5 >> 8) + 0xDF;
            angle = (i + 1) << 8;
            prim2->u1 = (rcos(angle) >> 4 << 5 >> 8) + 0x20;
            prim2->v1 = -(rsin(angle) >> 4 << 5 >> 8) + 0xDF;
            prim1->u2 = prim1->u3 = 0x20;
            prim1->v2 = prim1->v3 = 0xDF;
            prim2->u2 = prim1->u0 = (prim2->u0 + prim1->u2) / 2;
            prim2->v2 = prim1->v0 = (prim2->v0 + prim1->v2) / 2;
            prim2->u3 = prim1->u1 = (prim2->u1 + prim1->u3) / 2;
            prim2->v3 = prim1->v1 = (prim2->v1 + prim1->v3) / 2;
            prim1->tpage = prim2->tpage = 0x1A;
            prim1->clut = prim2->clut = PAL_FILL_WHITE;
            prim1->priority = prim2->priority = RIC.zPriority + 2;
            prim1->drawMode = prim2->drawMode =
                DRAW_UNK02 | DRAW_TPAGE2 | DRAW_TPAGE | DRAW_COLORS |
                DRAW_TRANSP;
            prim2 = prim2->next;
            prim1 = prim1->next;
        }
        self->ext.ricPowerRing.unk80 = self->ext.ricPowerRing.unk82 = 0x280;
        self->ext.ricPowerRing.unk84 = self->ext.ricPowerRing.unk86 = 0x240;
        self->ext.ricPowerRing.unk8A = loadedParams[5];
        self->ext.ricPowerRing.unk88 = 0xC0;
        self->step++;
        break;
    case 1:
        self->ext.ricPowerRing.unk7E += 0x40;
        self->ext.ricPowerRing.unk86 -= 10;
        if (self->ext.ricPowerRing.unk86 < 0) {
            self->ext.ricPowerRing.unk86 = 0;
            self->ext.ricPowerRing.unk7C = 0x20;
            self->step++;
        }
        self->ext.ricPowerRing.unk84 = self->ext.ricPowerRing.unk86;
        self->ext.ricPowerRing.unk82 -= 5;
        self->ext.ricPowerRing.unk80 -= 5;
        break;
    case 2:
        self->ext.ricPowerRing.unk7E += 0x40;
        self->ext.ricPowerRing.unk82 -= 3;
        self->ext.ricPowerRing.unk80 -= 6;
        if (--self->ext.ricPowerRing.unk7C == 0) {
            self->step++;
        }
        break;
    case 3:
        self->ext.ricPowerRing.unk7E += 0x40;
        self->ext.ricPowerRing.unk82 -= 3;
        self->ext.ricPowerRing.unk80 -= 6;
        self->ext.ricPowerRing.unk88 -= 12;
        if (self->ext.ricPowerRing.unk88 < 0) {
            DestroyEntity(self);
            return;
        }
        break;
    }
    angleStart = self->ext.ricPowerRing.unk8A;
    x = self->posX.i.hi;
    y = self->posY.i.hi;
    prim1 = prim2 = &g_PrimBuf[self->primIndex];
    for (i = 0; i < 16; i++) {
        prim1 = prim1->next;
    }
    for (i = 0; i < 16; i++) {
        prim2->x0 =
            x + (prim2->u0 - 0x20) * self->ext.ricPowerRing.unk80 / 0x100;
        prim2->y0 =
            y + (prim2->v0 - 0xE0) * self->ext.ricPowerRing.unk82 / 0x100;
        prim2->x1 =
            x + (prim2->u1 - 0x20) * self->ext.ricPowerRing.unk80 / 0x100;
        prim2->y1 =
            y + (prim2->v1 - 0xE0) * self->ext.ricPowerRing.unk82 / 0x100;
        prim1->x2 = x + (rcos((i + 1) << 8) >> 4 << 5 >> 8) *
                            self->ext.ricPowerRing.unk84 / 0x100;
        prim1->y2 = y - (rsin((i + 1) << 8) >> 4 << 5 >> 8) *
                            self->ext.ricPowerRing.unk86 / 0x100;
        prim1->x3 = x + (rcos((i + 2) << 8) >> 4 << 5 >> 8) *
                            self->ext.ricPowerRing.unk84 / 0x100;
        prim1->y3 = y - (rsin((i + 2) << 8) >> 4 << 5 >> 8) *
                            self->ext.ricPowerRing.unk86 / 0x100;
        prim2->x2 = prim1->x0 = (prim2->x0 + prim1->x2) / 2;
        prim2->y2 = prim1->y0 = (prim2->y0 + prim1->y2) / 2;
        prim2->x3 = prim1->x1 = (prim2->x1 + prim1->x3) / 2;
        prim2->y3 = prim1->y1 = (prim2->y1 + prim1->y3) / 2;
        baseAngle = i * angleStart;
        angle = self->ext.ricPowerRing.unk7E + baseAngle;
        prim1->r0 = prim2->r2 =
            ((rsin(angle) + 0x1000) >> 7) * self->ext.ricPowerRing.unk88 / rMod;
        angle = self->ext.ricPowerRing.unk7E + gOffset + baseAngle;
        prim1->g0 = prim2->g2 =
            ((rsin(angle) + 0x1000) >> 7) * self->ext.ricPowerRing.unk88 / gMod;
        angle = self->ext.ricPowerRing.unk7E + bOffset + baseAngle;
        prim1->b0 = prim2->b2 =
            ((rsin(angle) + 0x1000) >> 7) * self->ext.ricPowerRing.unk88 / bMod;
        angle = self->ext.ricPowerRing.unk7E + angleStart + baseAngle;
        prim1->r1 = prim2->r3 =
            ((rsin(angle) + 0x1000) >> 7) * self->ext.ricPowerRing.unk88 / rMod;
        angle = self->ext.ricPowerRing.unk7E + gOffset + angleStart + baseAngle;
        prim1->g1 = prim2->g3 =
            ((rsin(angle) + 0x1000) >> 7) * self->ext.ricPowerRing.unk88 / gMod;
        angle = self->ext.ricPowerRing.unk7E + bOffset + angleStart + baseAngle;
        prim1->b1 = prim2->b3 =
            ((rsin(angle) + 0x1000) >> 7) * self->ext.ricPowerRing.unk88 / bMod;
        prim2->r0 = prim2->g0 = prim2->b0 = prim2->r1 = prim2->g1 = prim2->b1 =
            0;
        prim1->r2 = prim1->g2 = prim1->b2 = prim1->r3 = prim1->g3 = prim1->b3 =
            0;
        prim2 = prim2->next;
        prim1 = prim1->next;
    }
}

void RicEntityHitByIce(Entity* self) {
    const int PrimCount = LEN(D_us_80181D54) / 3;
    s16 x;
    s16 y;
    s32 i;
    bool terminateFlag;
    s16 deltaX;
    s16 deltaY;
    s16 yMod;
    s32 distance;
    s16 angle;
    Point16* pos;
    Primitive* prim;

    self->posX.i.hi = RIC.posX.i.hi;
    self->posY.i.hi = RIC.posY.i.hi;
    terminateFlag = 0;
    if (!(g_Ric.status & PLAYER_STATUS_UNK10000)) {
        terminateFlag = 1;
    }
    switch (self->step) {
    case 0:
        self->primIndex = g_api.AllocPrimitives(PRIM_GT3, PrimCount);
        if (self->primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->flags = FLAG_HAS_PRIMS | FLAG_POS_CAMERA_LOCKED;
        prim = &g_PrimBuf[self->primIndex];
        while (prim != NULL) {
            prim->r0 = prim->r1 = prim->r2 = prim->r3 = (rand() & 0x0F) + 0x30;
            prim->b0 = prim->b1 = prim->b2 = prim->b3 = (rand() & 0x7F) + 0x80;
            prim->g0 = prim->g1 = prim->g2 = prim->g3 = (rand() & 0x1F) + 0x30;
            if (rand() & 1) {
                prim->drawMode = DRAW_UNK_100 | DRAW_TPAGE2 | DRAW_TPAGE | DRAW_COLORS | DRAW_TRANSP | DRAW_UNK02;
            } else {
                prim->drawMode = DRAW_UNK_100 | DRAW_TPAGE | DRAW_COLORS | DRAW_TRANSP | DRAW_UNK02;
            }
            prim->type = PRIM_G4;
            prim->priority = RIC.zPriority + 2;
            prim = prim->next;
        }
        // Weird repeated conditional
        if (RIC.velocityY != 0) {
            self->ext.hitbyice.unk7E = 1;
        }
        if (RIC.velocityY != 0) {
            if (RIC.facingLeft) {
                self->rotate = ROT(22.5);
            } else {
                self->rotate = ROT(-22.5);
            }
        } else {
            if (RIC.velocityX > 0) {
                self->rotate = ROT(11.25);
            } else {
                self->rotate = ROT(348.75);
            }
        }
        if (RIC.step == PL_S_DEAD) {
            if (RIC.facingLeft) {
                self->rotate = ROT(33.75);
            } else {
                self->rotate = ROT(-33.75);
            }
            self->ext.hitbyice.unk80 = 1;
            self->ext.hitbyice.unk82 = 0x3C;
            if (self->params & 0x7F00) {
                self->ext.hitbyice.unk82 = 0x14;
            }
            self->ext.hitbyice.unk7E = 0;
        }
        self->step++;
        break;
    case 1:
        if (RIC.step == PL_S_DEAD) {
            if ((RIC.animCurFrame & 0x7FFF) == 0x21) {
                if (RIC.facingLeft) {
                    self->rotate = ROT(56.25);
                } else {
                    self->rotate = ROT(-56.25);
                }
            }
            if ((RIC.animCurFrame & 0x7FFF) == 0x22) {
                if (RIC.facingLeft) {
                    self->rotate = ROT(78.75);
                } else {
                    self->rotate = ROT(-78.75);
                }
            }
            if ((RIC.animCurFrame & 0x7FFF) == 0x20) {
                if (RIC.facingLeft) {
                    self->rotate = ROT(33.75);
                } else {
                    self->rotate = ROT(-33.75);
                }
            }
        }
        if (self->ext.hitbyice.unk80 && --self->ext.hitbyice.unk82 == 0) {
            terminateFlag = true;
        }
        if (self->ext.hitbyice.unk7E &&
            g_Ric.vram_flag & (TOUCHING_L_WALL | TOUCHING_R_WALL)) {
            terminateFlag = true;
        }
        if (terminateFlag) {
            self->ext.hitbyice.unk7C = 0x40;
            if (self->ext.hitbyice.unk80) {
                self->ext.hitbyice.unk7C = 0x80;
            }
            self->step++;
        }
        break;
    case 2:
        if (--self->ext.hitbyice.unk7C == 0) {
            DestroyEntity(self);
            return;
        }
        break;
    }

    x = self->posX.i.hi;
    y = self->posY.i.hi;
    prim = &g_PrimBuf[self->primIndex];
    for (i = 0; i < PrimCount; i++) {
        pos = D_us_80181D54[i * 3];
        if (prim->u0 < 2) {
            distance = SquareRoot12((pos->x * pos->x + pos->y * pos->y) << 0xC);
            angle = self->rotate + ratan2(pos->y, pos->x);
            deltaX = (((rcos(angle) >> 4) * distance) + 0x80000) >> 0x14;
            deltaY = (((rsin(angle) >> 4) * distance) + 0x80000) >> 0x14;
            prim->x0 = x + deltaX;
            prim->y0 = y + deltaY;

            pos = D_us_80181D54[i * 3 + 1];
            distance = SquareRoot12((pos->x * pos->x + pos->y * pos->y) << 0xC);
            angle = self->rotate + ratan2(pos->y, pos->x);
            deltaX = (((rcos(angle) >> 4) * distance) + 0x80000) >> 0x14;
            deltaY = (((rsin(angle) >> 4) * distance) + 0x80000) >> 0x14;
            prim->x1 = x + deltaX;
            prim->y1 = y + deltaY;

            pos = D_us_80181D54[i * 3 + 2];
            distance = SquareRoot12((pos->x * pos->x + pos->y * pos->y) << 0xC);
            angle = self->rotate + ratan2(pos->y, pos->x);
            deltaX = (((rcos(angle) >> 4) * distance) + 0x80000) >> 0x14;
            deltaY = (((rsin(angle) >> 4) * distance) + 0x80000) >> 0x14;
            prim->x2 = prim->x3 = x + deltaX;
            prim->y2 = prim->y3 = y + deltaY;
        }
        if (prim->u0 == 0 && terminateFlag) {
            prim->u0++;
            prim->v0 = (rand() & 15) + 1;
            if (self->ext.hitbyice.unk80) {
                prim->v0 = (rand() % 60) + 1;
            }
        }
        if (prim->u0 == 1) {
            if (--prim->v0 == 0) {
                prim->u0++;
                prim->v0 = 0x20;
                prim->u2 = 0xF0;
                if (self->ext.hitbyice.unk80) {
                    prim->v0 = (rand() & 31) + 0x28;
                }
            }
        }
        if (prim->u0 == 2) {
            if (prim->u2 < 0x70 || prim->u2 > 0xD0) {
                prim->u2 += 4;
            }
            yMod = (s8)prim->u2 >> 4;
            if (self->ext.hitbyice.unk80) {
                yMod = yMod >> 1;
            }
            prim->y0 += yMod;
            prim->y1 += yMod;
            prim->y2 += yMod;
            prim->y3 += yMod;
            if (prim->r3 < 4) {
                prim->r3 -= 4;
            }
            if (prim->g3 < 4) {
                prim->g3 -= 4;
            }
            if (prim->b3 < 4) {
                prim->b3 -= 4;
            }
            prim->r0 = prim->r1 = prim->r2 = prim->r3;
            prim->b0 = prim->b1 = prim->b2 = prim->b3;
            prim->g0 = prim->g1 = prim->g2 = prim->g3;
            prim->drawMode &= ~(DRAW_UNK_100 | DRAW_UNK02);
            if (--prim->v0 == 0) {
                prim->drawMode |= DRAW_HIDE;
            }
        }
        prim = prim->next;
    }
}

void RicEntityHitByLightning(Entity* self) {
    s16 x;
    s16 y;
    s16 mul2;
    s32 i;
    s32 xMod;
    s32 yMod;
    bool terminate;
    s32 mul;
    s16 angle;
    Primitive* primPrev;
    Primitive* prim;

    terminate = false;
    if ((self->params & 0xFF00) == 0x100) {
        if ((++self->ext.hitbylightning.unk9C) > 0xA8) {
            terminate = true;
        }
    } else if ((self->params & 0xFF00) == 0x200) {
        if (++self->ext.hitbylightning.unk9C > 0x90) {
            terminate = true;
        }
    } else if (RIC.step != PL_S_HIT) {
        terminate = true;
    }
    switch (self->step) {
    case 0:
        self->primIndex = g_api.AllocPrimitives(PRIM_GT4, 6);
        if (self->primIndex == -1) {
            DestroyEntity(self);
            return;
        }

        self->flags = FLAG_POS_CAMERA_LOCKED | FLAG_HAS_PRIMS;
        self->ext.hitbylightning.unk7C =
            ((self->params & 0xF) << 9) + (rand() & 0x3F);
        self->ext.hitbylightning.unk80 = (rand() & PSP_RANDMASK);
        self->ext.hitbylightning.unk82 = (rand() & 0x1FF) + 0x100;
        prim = &g_PrimBuf[self->primIndex];
        while (prim != NULL) {
            prim->x0 = prim->x1 = prim->x2 = prim->x3 = self->posX.i.hi;
            prim->y0 = prim->y1 = prim->y2 = prim->y3 = self->posY.i.hi;
            prim->tpage = 0x1A;
            prim->clut = D_us_80181E74[rand() & 1];
            prim->priority = RIC.zPriority - 2;
            prim->r0 = prim->g0 = prim->b0 = prim->r1 = prim->g1 = prim->b1 =
                prim->r2 = prim->g2 = prim->b2 = prim->r3 = prim->g3 =
                    prim->b3 = 0x80;
            prim->drawMode = DRAW_UNK_100 | DRAW_TPAGE2 | DRAW_TPAGE |
                             DRAW_UNK02 | DRAW_TRANSP;
            prim = prim->next;
        }
        if ((RIC.velocityY != 0) && (RIC.step != PL_S_DEAD)) {
            self->ext.hitbylightning.unk92 = 1;
        }
        self->ext.hitbylightning.unk94 = 0x10;
        g_api.PlaySfx(0x6E4);
        self->step++;
        break;
    case 1:
        self->ext.hitbylightning.unk7C =
            ((self->params & 0xF) << 9) + (rand() & 0x1FF);
        mul = rsin(self->ext.hitbylightning.unk80);
        self->ext.hitbylightning.unk80 += self->ext.hitbylightning.unk82;
        xMod = ((rcos(self->ext.hitbylightning.unk7C) * mul) >> 7) * 12;
        yMod = (-((rsin(self->ext.hitbylightning.unk7C) * mul) >> 7) * 7) << 1;
        self->posX.val = xMod + RIC.posX.val;
        self->posY.val = yMod + RIC.posY.val;
        if (self->ext.hitbylightning.unk92 &&
            g_Ric.vram_flag &
                (TOUCHING_L_WALL | TOUCHING_R_WALL | TOUCHING_CEILING)) {
            terminate = true;
        }
        if (terminate) {
            self->ext.hitbylightning.unk90 = (rand() & 0xF) + 0x10;
            self->step++;
        }
        break;
    case 2:
        if (--self->ext.hitbylightning.unk90 == 0) {
            DestroyEntity(self);
            return;
        }
        if (self->ext.hitbylightning.unk94 > 0) {
            self->ext.hitbylightning.unk94--;
        }
        self->ext.hitbylightning.unk7C =
            ((self->params & 0xF) << 9) + (rand() & 0xFF);
        mul = rsin(self->ext.hitbylightning.unk80);
        self->ext.hitbylightning.unk80 += self->ext.hitbylightning.unk82;
        xMod = ((rcos(self->ext.hitbylightning.unk7C) * mul) >> 7) *
               ((rand() % 8) + 8);
        yMod = -((rsin(self->ext.hitbylightning.unk7C) * mul) >> 7) *
                   ((rand() % 8) + 10) +
               self->ext.hitbylightning.unk98;
        self->posX.val = RIC.posX.val + xMod;
        self->posY.val = RIC.posY.val + yMod;
        self->ext.hitbylightning.unk98 -= 0x8000;
        prim = &g_PrimBuf[self->primIndex];
        break;
    }

    x = (self->posX.i.hi + (rand() & 7)) - 4;
    y = (self->posY.i.hi + (rand() & 0x1F)) - 0x18;
    mul2 = self->ext.hitbylightning.unk94;
    mul2 = (mul2 * rsin(self->ext.hitbylightning.unk80)) >> 0xC;
    prim = &g_PrimBuf[self->primIndex];
    for (i = 0; i < 5; i++) {
        primPrev = prim;
        prim = prim->next;
        *primPrev = *prim;
        primPrev->next = prim;
        primPrev->u0 = primPrev->u2 = (i * 0x10) + 0x90;
        primPrev->u1 = primPrev->u3 = ((i + 1) * 0x10) + 0x90;
        primPrev->v0 = primPrev->v1 = 0xC0;
        primPrev->v2 = primPrev->v3 = 0xCF;
    }
    prim->x0 = prim->x1;
    prim->y0 = prim->y1;
    prim->x2 = prim->x3;
    prim->y2 = prim->y3;
    angle = self->ext.hitbylightning.unk7C + 0x400;
    prim->x1 = x + (((rcos(angle) >> 4) * mul2) >> 8);
    prim->y1 = y - (((rsin(angle) >> 4) * mul2) >> 8);
    angle = self->ext.hitbylightning.unk7C - 0x400;
    prim->x3 = x + (((rcos(angle) >> 4) * mul2) >> 8);
    prim->y3 = y - (((rsin(angle) >> 4) * mul2) >> 8);
    angle = self->ext.hitbylightning.unk80 & 0xFFF;
    if (angle < 0x400) {
        prim->priority = RIC.zPriority + 2;
    } else if (angle < 0xC00) {
        prim->priority = RIC.zPriority - 2;
    } else {
        prim->priority = RIC.zPriority + 2;
    }
    prim->u0 = prim->u2 = (i << 4) + 0x90;
    prim->u1 = prim->u3 = ((i + 1) << 4) + 0x90;
    prim->v0 = prim->v1 = 0xC0;
    prim->v2 = prim->v3 = 0xCF;
}

void RicEntityWhip(Entity* self) {
    byte stackpad[40];
    s16 sp4E;
    s16 radius;
    s32 sp44;
    s32 sp40;
    s32 prevX;
    s32 prevY;
    s16 sp4A;
    s32 aimPath;
    s32 newY;
    s16 angle;
    s32 diffY;
    s16 psp_s4;
    s32 diffX;
    s32 newX;
    Primitive* prim;
    s32 psp_s0;

    psp_s4 = self->params & 0xFF;
    sp4A = self->params & 0xFF00;
    if (psp_s4 == 0) {
        if (self->ext.whip.unkA6) {
            self->ext.whip.unkA6--;
        }
    }
    if (g_Ric.unk46 == 0) {
        DestroyEntity(self);
        return;
    }
    if (self->step == 2 && g_Ric.unk46 == 2) {
        self->step = 1;
        self->velocityX = 0;
        self->velocityY = 0;
        self->palette = PAL_FLAG(0x248);
        if (RIC.facingLeft == 0) {
            self->ext.whip.unk88 = 2;
        } else {
            self->ext.whip.unk88 = 3;
        }
        self->ext.whip.unk8C = 0x600;
    }
    if (self->step) {
        diffX = self->posX.val - self->ext.whip.curX;
        diffY = self->posY.val - self->ext.whip.curY;
        self->ext.whip.prevX += diffX;
        self->ext.whip.prevY += diffY;
    }
    aimPath = g_Ric.padPressed & (PAD_UP | PAD_RIGHT | PAD_DOWN | PAD_LEFT);
    // @bug: up+down or left+right are not handled, psp_s0 will be undefined.
    // this never happens due to physical limitatitions of the PS controller.
    ASSERT(aimPath != (PAD_UP | PAD_DOWN));
    ASSERT(aimPath != (PAD_LEFT | PAD_RIGHT));
    if (aimPath == PAD_UP) {
        psp_s0 = D_us_8018266C[self->ext.whip.unk88];
    }
    if (aimPath == PAD_DOWN) {
        psp_s0 = D_us_801826D8[self->ext.whip.unk88];
    }
    if (aimPath == PAD_RIGHT) {
        psp_s0 = D_us_80182690[self->ext.whip.unk88];
    }
    if (aimPath == PAD_LEFT) {
        psp_s0 = D_us_801826B4[self->ext.whip.unk88];
    }
    if (aimPath == (PAD_UP + PAD_RIGHT)) {
        psp_s0 = D_us_801826FC[self->ext.whip.unk88];
    }
    if (aimPath == (PAD_UP + PAD_LEFT)) {
        psp_s0 = D_us_80182720[self->ext.whip.unk88];
    }
    if (aimPath == (PAD_DOWN + PAD_RIGHT)) {
        psp_s0 = D_us_80182744[self->ext.whip.unk88];
    }
    if (aimPath == (PAD_DOWN + PAD_LEFT)) {
        psp_s0 = D_us_80182768[self->ext.whip.unk88];
    }
    if (aimPath == 0) {
        psp_s0 = D_us_8018278C[self->ext.whip.unk88];
    }

    self->ext.whip.unk86 = 0;
    if (psp_s4 == 0) {
        if (self->step == 1) {
            if (RIC.step == PL_S_CROUCH) {
                if (RIC.facingLeft == 0) {
                    newX = D_us_80182580[psp_s0];
                } else {
                    newX = D_us_8018255C[psp_s0].x;
                }
                newY = D_us_8018255C[psp_s0].y;
            } else if (RIC.step == PL_S_STAND) {
                if (RIC.facingLeft == 0) {
                    newX = D_us_80182548[psp_s0];
                } else {
                    newX = D_us_80182524[psp_s0].x;
                }
                newY = D_us_80182524[psp_s0].y;
            } else {
                if (RIC.facingLeft == 0) {
                    newX = D_us_801825B8[psp_s0];
                } else {
                    newX = D_us_80182594[psp_s0].x;
                }
                newY = D_us_80182524[psp_s0].y;
            }
        } else {
            if (RIC.step == PL_S_CROUCH) {
                newX = D_us_80182508[RIC.pose].x;
                newY = D_us_80182508[RIC.pose].y;
            } else {
                newX = D_us_801824EC[RIC.pose].x;
                newY = D_us_801824EC[RIC.pose].y;
            }
            if (RIC.facingLeft == 0) {
                newX = -newX;
            }
        }
        newX <<= 16;
        newY <<= 16;
        newX += RIC.posX.val;
        newY += RIC.posY.val;
    } else {
        self--;
        newX = self->posX.val;
        newY = self->posY.val;
        self++;
    }
    switch (self->step) {
    case 0:
        InitializeEntity(sp4A ? D_us_80180418 : D_us_8018040C);
        self->animSet = ANIMSET_OVL(3);
        self->animCurFrame = 0x1E;
        self->palette = PAL_FLAG(0x238);
        self->unk5A = 0x24;
        if (psp_s4 < 8) {
            self->hitboxWidth = 3;
            self->hitboxHeight = 3;
            self->hitboxOffX = 0;
            self->hitboxOffY = 0;
        }
        if (psp_s4 == 0) {
            self->zPriority = RIC.zPriority - 2;
        }
        if (psp_s4 == 7) {
            self->animCurFrame = 0x1F;
            self->hitboxWidth++;
            self->hitboxHeight++;
        }
        self->zPriority = RIC.zPriority + 4;
        self->flags = FLAG_UNK_10000000 | FLAG_POS_CAMERA_LOCKED;
        self->posX.val = newX;
        self->posY.val = newY;
        self->ext.whip.prevX = self->posX.val;
        self->ext.whip.prevY = self->posY.val;
        self->ext.whip.curX = self->ext.whip.prevX;
        self->ext.whip.curY = self->ext.whip.prevY;
        self->ext.whip.unk8C = 0x500;
        self->primIndex = g_api.AllocPrimitives(PRIM_LINE_G2, 1);
        if (self->primIndex != -1) {
            self->flags |= FLAG_HAS_PRIMS;
            prim = &g_PrimBuf[self->primIndex];
            prim->r0 = 0x7F;
            prim->g0 = 0x7F;
            prim->b0 = 0x7F;
            prim->r1 = 0x1F;
            prim->g1 = 0x1F;
            prim->b1 = 0x1F;
            prim->priority = RIC.zPriority + 2;
            prim->drawMode =
                DRAW_TPAGE2 | DRAW_TPAGE | DRAW_COLORS | DRAW_TRANSP;
        }
        self->step = 2;
        RicEntityWhip(self);
        break;
    case 1:
        if (sp4A == 0) {
            if (self->palette < PAL_FLAG(0x248) &&
                self->palette >= PAL_FLAG(0x240)) {
                if (self->ext.whip.unkA4) {
                    self->ext.whip.unkA4--;
                } else {
                    self->palette++;
                }
            }
        } else if (self->ext.whip.unkA4) {
            if (!(psp_s4 & 1)) {
                RicCreateEntFactoryFromEntity(self, BP_20, 0);
                self->ext.whip.unkA4 = 0;
            }
        }
        if (psp_s4 < 8) {
            if (self->ext.whip.unk84) {
                if (--self->ext.whip.unk84 == 0) {
                    if (self->velocityY < 0) {
                        self->velocityY = 0;
                    }
                    self->velocityX = 0;
                }
            } else {
                self->velocityX = self->velocityX * 3 / 4;
            }
            if (self->ext.whip.unk88 == psp_s0) {
                break;
            }
            if (psp_s4 == 0) {
                if (sp4A) {
                    if (self->ext.whip.unkA6 == 0) {
                        g_api.PlaySfx(SFX_RIC_WHIP_RATTLE_A);
                        self->ext.whip.unkA6 = 0x20;
                    }
                } else {
                    if (self->ext.whip.unkA6 == 0) {
                        g_api.PlaySfx(SFX_RIC_WHIP_RATTLE_A);
                        self->ext.whip.unkA6 = 0x20;
                    }
                }
                if (sp4A == 0) {
                    RicCreateEntFactoryFromEntity(self, FACTORY(BP_18, 0), 0);
                } else {
                    RicCreateEntFactoryFromEntity(self, FACTORY(BP_18, 1), 0);
                }
            }

            self->ext.whip.unk84 = 6;
            if (psp_s0 == 0) {
                if (RIC.facingLeft == 0) {
                    diffX = D_us_80182548[0];
                } else {
                    diffX = D_us_80182524[0].x;
                }
                diffY = D_us_80182524[0].y;
                diffX += RIC.posX.i.hi;
                diffY += RIC.posY.i.hi;
                diffX <<= 16;
                diffY <<= 16;

                diffX -= self->posX.val;
                diffY -= self->posY.val;
                angle = ratan2(-diffY, diffX);
                radius = 0x80;
                self->velocityX = (radius * rcos(angle)) >> 4;
                self->velocityY = -(radius * rsin(angle)) >> 4;
            } else {
                self->velocityY = D_us_80182624[psp_s0].x;
                self->velocityX = D_us_80182624[psp_s0].y;
            }
            self->ext.whip.unk88 = psp_s0;
        }
        break;
    case 2:
        if (psp_s4 > 7) {
            if (self->flags & FLAG_HAS_PRIMS) {
                if (RIC.pose == 5) {
                    g_api.FreePrimitives(self->primIndex);
                    self->flags &= ~FLAG_HAS_PRIMS;
                }
            }
            if (RIC.pose == 6) {
                if (RIC.poseTimer > 14) {
                    self->palette = PAL_FLAG(0x238);
                } else if (RIC.poseTimer == 0) {
                    self->palette = PAL_FLAG(0x238);
                } else if ((RIC.poseTimer == 11 || RIC.poseTimer == 12) ||
                           (RIC.poseTimer == 13 || RIC.poseTimer == 14)) {
                    self->palette = PAL_FLAG(0x23C);
                } else {
#ifdef FIX_UB
                    // poseTimer can be -1 apparently.
                    // todo this should read the previous element out of bounds?
                    if (RIC.poseTimer >= 0) {
                        self->palette =
                            D_us_801827B0[RIC.poseTimer % LEN(D_us_801827B0)];
                    }
#else
                    self->palette =
                        D_us_801827B0[RIC.poseTimer % LEN(D_us_801827B0)];
#endif
                }
            }
        } else {
            self->velocityX = D_us_801825EC[RIC.pose].x;
            self->velocityY = D_us_801825EC[RIC.pose].y;
            switch (RIC.pose) {
            case 0:
                radius = 0x800;
                angle = (rand() & 0x3FF) - 0x200;
                self->velocityX = (radius * rcos(angle)) >> 4;
                self->velocityY = -(radius * rsin(angle)) >> 4;
                break;
            case 1:
                radius = 0x800;
                angle = (rand() & 0x3FF) - 0x400;
                self->velocityX = (radius * rcos(angle)) >> 4;
                self->velocityY = -(radius * rsin(angle)) >> 3;
                self->ext.whip.unk8C = 0x600;
                break;
            case 4:
                self->ext.whip.unk8C = 0x3C0;
                if (RIC.poseTimer == 1) {
                    self->velocityX = FIX(-8);
                }
                break;
            case 5:
                self->ext.whip.unk8C += 0xC0;
                if (self->flags & FLAG_HAS_PRIMS) {
                    g_api.FreePrimitives(self->primIndex);
                    self->flags &= ~FLAG_HAS_PRIMS;
                }
                break;
            case 6:
                if (sp4A) {
                    if (RIC.poseTimer == D_us_801827B8[psp_s4]) {
                        RicCreateEntFactoryFromEntity(self, BP_20, 0);
                    }
                }
                if (psp_s4 == (0x10 - RIC.poseTimer)) {
                    self->ext.whip.unk8C = 0x600;
                }
                // This very well might be a switch
                if (RIC.poseTimer > 14) {
                    self->palette = PAL_FLAG(0x238);
                } else if (RIC.poseTimer == 0) {
                    self->palette = PAL_FLAG(0x238);
                } else if (RIC.poseTimer == 11 || RIC.poseTimer == 12 ||
                           RIC.poseTimer == 13 || RIC.poseTimer == 14) {
                    self->palette = PAL_FLAG(0x23C);
                } else {
#ifdef FIX_UB
                    // poseTimer can be -1 apparently.
                    // todo this should read the previous element out of bounds?
                    if (RIC.poseTimer >= 0) {
                        self->palette = D_us_801827B0[RIC.poseTimer % 3];
                    }
#else
                    self->palette = D_us_801827B0[RIC.poseTimer % 3];
#endif
                }
                break;
            }
            if (RIC.facingLeft == 0) {
                self->velocityX = -self->velocityX;
            }
            self->velocityX = self->velocityX / 2;
            self->velocityY = self->velocityY / 2;
        }
        break;
    }
    sp4E = self->ext.whip.unk8C;
    prevX = self->posX.val;
    prevY = self->posY.val;
    if (psp_s4 == 0) {
        self->posX.val = newX;
        self->posY.val = newY;
    } else if (psp_s4 > 7) {
        diffX = (self - 8)->posX.val;
        diffY = (self - 8)->posY.val;
        sp44 = (self - 7)->posX.val;
        sp40 = (self - 7)->posY.val;
        self->posX.val = (diffX + sp44) / 2;
        self->posY.val = (diffY + sp40) / 2;
    } else {
        sp44 = self->posX.val - self->ext.whip.prevX;
        sp40 = self->posY.val - self->ext.whip.prevY;
        if (self->step == 1) {
            if (self->velocityY < FIX(0.5)) {
                self->posX.val += sp44 * 16 / 16;
                self->posY.val += sp40 * 16 / 16;
            }
            if (self->velocityY >= FIX(0.5) && self->velocityY < FIX(1.25)) {
                self->posX.val += sp44 * 15 / 16;
                self->posY.val += sp40 * 15 / 16;
            }
            if (self->velocityY >= FIX(1.25) || self->velocityY < 0) {
                self->posX.val += sp44 * 13 / 16;
                self->posY.val += sp40 * 13 / 16;
            }
        } else {
            self->posX.val += sp44 * 3 / 4;
            self->posY.val += sp40 * 3 / 4;
        }
        if (self->velocityY < FIX(8) || self->velocityY < 0) {
            self->velocityY += 0xA00;
        }
        self->posX.val += self->velocityX;
        self->posY.val += self->velocityY;
        diffY = self->posY.val - newY;
        diffX = self->posX.val - newX;
        if (((sp4E * sp4E) >> 0x10) < ((diffY * (diffY >> 0x10)) >> 0x10) +
                                          ((diffX * (diffX >> 0x10)) >> 0x10)) {
            angle = ratan2(-diffY, diffX);
            diffX = (rcos(angle) * sp4E) >> 4;
            diffY = -(rsin(angle) * sp4E) >> 4;
            self->posX.val = newX + diffX;
            self->posY.val = newY + diffY;
        }
    }
    self->ext.whip.prevX = prevX;
    self->ext.whip.prevY = prevY;
    self->ext.whip.curX = self->posX.val;
    self->ext.whip.curY = self->posY.val;
    self->ext.whip.unkA0 = sp40;
    if (psp_s4 == 0) {
        D_us_801D0874 = psp_s0;
        D_us_801D0878 = self->ext.whip.unk88;
    }
    if (self->primIndex != -1 && (self->flags & FLAG_HAS_PRIMS)) {
        prim = &g_PrimBuf[self->primIndex];
        prim->x0 = self->posX.i.hi;
        prim->y0 = self->posY.i.hi;
        prim->x1 = self->ext.whip.prevX >> 0x10;
        prim->y1 = self->ext.whip.prevY >> 0x10;
        if (RIC.pose == 4) {
            prim->priority = RIC.zPriority + 4;
        } else {
            prim->priority = RIC.zPriority + 2;
        }
    }
    if (sp4A) {
        if (psp_s4 != 7) {
            angle = (((g_GameTimer >> 2) & 7) + D_us_801827D8[psp_s4]) * 512;
            self->opacity = (rsin(angle) >> 6) + 0x80;
            self->drawFlags |= DRAW_HIDE;
            self->blendMode = BLEND_TRANSP | BLEND_ADD;
        }
        self->palette = PAL_FLAG(PAL_CC_FIRE_EFFECT);
    }
}

void RicEntityArmBrandishWhip(Entity* entity) {
    if (g_Ric.unk46 == 0) {
        DestroyEntity(entity);
        return;
    }
    entity->facingLeft = RIC.facingLeft;
    if (entity->step == 0) {
        entity->flags = FLAG_POS_CAMERA_LOCKED | FLAG_UNK_10000000;
        entity->animSet = ANIMSET_OVL(3);
        entity->unk5A = 0x24;
        entity->palette = PAL_FLAG(0x220);
        entity->zPriority = RIC.zPriority + 2;
    }
    if (RIC.step == PL_S_CROUCH) {
        if (RIC.facingLeft) {
            entity->animCurFrame = D_us_8018280C[D_us_801D0874];
        } else {
            entity->animCurFrame = D_us_801827F8[D_us_801D0874];
        }
    } else if (RIC.step == PL_S_STAND) {
        if (RIC.facingLeft) {
            entity->animCurFrame = D_us_80182834[D_us_801D0874];
        } else {
            entity->animCurFrame = D_us_80182820[D_us_801D0874];
        }
    } else if (RIC.facingLeft) {
        entity->animCurFrame = D_us_8018285C[D_us_801D0874];
    } else {
        entity->animCurFrame = D_us_80182848[D_us_801D0874];
    }
    entity->posX.val = RIC.posX.val;
    entity->posY.val = RIC.posY.val;
}

void RicEntitySubwpnHolyWaterBreakGlass(Entity* self) {
    Point16 sp10[8];
    Primitive* prim;
    s16 posX;
    s16 posY;
    s16 arrIndex;
    s32 i;

    switch (self->step) {
    case 0:
        self->primIndex = g_api.AllocPrimitives(PRIM_GT4, 16);
        if (self->primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        posX = self->posX.i.hi;
        posY = self->posY.i.hi;
        prim = &g_PrimBuf[self->primIndex];
        for (i = 0; prim != NULL; i++, prim = prim->next) {
            if (i < 8) {
                sp10[i].x = FAKEPRIM->posX.i.hi = FAKEPRIM->x0 = posX;
                sp10[i].y = FAKEPRIM->posY.i.hi = FAKEPRIM->y0 = posY;
                // Random velocity from 0.25 to 0.5
                FAKEPRIM->velocityX.val = (rand() & 0x3FFF) + FIX(0.25);
                if (i & 1) {
                    FAKEPRIM->velocityX.val = -FAKEPRIM->velocityX.val;
                }
                FAKEPRIM->velocityY.val =
                    -(((rand() & PSP_RANDMASK) * 2) + FIX(2.5));
                FAKEPRIM->drawMode = DRAW_HIDE | DRAW_UNK02;
                FAKEPRIM->type = PRIM_TILE;
            } else {
                prim->r0 = prim->r1 = prim->r2 = prim->r3 =
                    (rand() & 0xF) + 0x30;
                prim->b0 = prim->b1 = prim->b2 = prim->b3 =
                    (rand() & 0x7F) + 0x80;
                prim->g0 = prim->g1 = prim->g2 = prim->g3 =
                    (rand() & 0x1F) + 0x30;
                if (rand() & 1) {
                    prim->drawMode = (DRAW_TPAGE2 | DRAW_TPAGE | DRAW_COLORS |
                                      DRAW_UNK02 | DRAW_TRANSP);
                } else {
                    prim->drawMode = (DRAW_COLORS | DRAW_UNK02);
                }
                posX = sp10[i - 8].x;
                posY = sp10[i - 8].y;
                prim->u0 = arrIndex = i & 3;
                prim->x0 = posX + D_us_80182890[arrIndex][0];
                prim->y0 = posY + D_us_80182890[arrIndex][1];
                prim->x1 = posX + D_us_80182890[arrIndex][2];
                prim->y1 = posY + D_us_80182890[arrIndex][3];
                prim->x3 = prim->x2 = posX + D_us_80182890[arrIndex][4];
                prim->y3 = prim->y2 = posY + D_us_80182890[arrIndex][5];
                prim->type = PRIM_G4;
                prim->priority = RIC.zPriority + 2;
            }
        }
        self->flags = FLAG_POS_CAMERA_LOCKED | FLAG_HAS_PRIMS;
        self->ext.timer.t = 20;
        self->step++;
        break;
    case 1:
        if (--self->ext.timer.t == 0) {
            DestroyEntity(self);
            return;
        }
        prim = &g_PrimBuf[self->primIndex];
        for (i = 0; prim != NULL; i++, prim = prim->next) {
            if (i < 8) {
                FAKEPRIM->posX.i.hi = FAKEPRIM->x0;
                FAKEPRIM->posY.i.hi = FAKEPRIM->y0;
                FAKEPRIM->posX.val += FAKEPRIM->velocityX.val;
                FAKEPRIM->posY.val += FAKEPRIM->velocityY.val;
                FAKEPRIM->velocityY.val += FIX(36.0 / 128);
                sp10[i].x = FAKEPRIM->posX.i.hi;
                sp10[i].y = FAKEPRIM->posY.i.hi;
                FAKEPRIM->x0 = FAKEPRIM->posX.i.hi;
                FAKEPRIM->y0 = FAKEPRIM->posY.i.hi;
            } else {
                posX = sp10[i - 8].x;
                posY = sp10[i - 8].y;
                arrIndex = prim->u0;
                prim->x0 = posX + D_us_80182890[arrIndex][0];
                prim->y0 = posY + D_us_80182890[arrIndex][1];
                prim->x1 = posX + D_us_80182890[arrIndex][2];
                prim->y1 = posY + D_us_80182890[arrIndex][3];
                prim->x3 = prim->x2 = posX + D_us_80182890[arrIndex][4];
                prim->y3 = prim->y2 = posY + D_us_80182890[arrIndex][5];
            }
        }
        break;
    }
}

void RicEntityCrashHydroStorm(Entity* self) {
    PrimLineG2* line;
    s16 primcount;
    s32 i;

    if (self->params < 24) {
        primcount = 32;
    } else {
        primcount = 33 - (self->params - 32) * 2;
    }

    switch (self->step) {
    case 0:
        self->primIndex = g_api.AllocPrimitives(PRIM_LINE_G2, primcount);
        if (self->primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->ext.subweapon.subweaponId = 0x10;
        InitializeEntity(D_us_80180484);
        self->posX.i.hi = 0x80;
        self->posY.i.hi = 0x70;
        self->hitboxWidth = 0x78;
        self->hitboxHeight = 0x78;
        self->facingLeft = 0;
        line = (PrimLineG2*)&g_PrimBuf[self->primIndex];
        self->flags |= FLAG_UNK_10000000 | FLAG_HAS_PRIMS;
        while (line != NULL) {
            line->r0 = 0x1F;
            line->g0 = 0x1F;
            line->b0 = 0x30;
            line->r1 = 0x3F;
            line->g1 = 0x50;
            line->b1 = 0x7F;
            line->x0 = rand() & 0x1FF;
            line->x1 = line->x0;
            line->y0 = -(rand() & 0xF);
            line->y1 = line->y0;
            line->preciseX.i.hi = line->x1;
            line->preciseY.i.hi = line->y1;

            // both rcos and rsin could be pre-calcualted values
            // PSP and PSX are logically equivalent
#if defined(VERSION_PSP)
            line->velocityX.val = (rcos(ROT(258.75)) * 16) * 12;
#else
            line->velocityX.val = -(rcos(ROT(258.75)) * -16) * 12;
#endif
            line->velocityY.val = -(rsin(ROT(258.75)) * 16) * 12;
            line->timer = 0;
            line->delay = (rand() & 0xF) + 12;
            if (rand() & 1) {
                line->priority = RIC.zPriority + 2;
            } else {
                line->priority = RIC.zPriority - 2;
            }
            line->drawMode = DRAW_TPAGE2 | DRAW_TPAGE | DRAW_TRANSP;
            line = line->next;
        }
        if (self->params == 1) {
            g_api.SetFadeMode(FADE_BLUE_TINT);
        }
        self->ext.subweapon.timer = 0x160;
        if ((self->params < 16) && !(self->params & 3)) {
            g_api.PlaySfx(0x830);
        }
        self->step = 1; // InitializeEntity already counted one step
        break;
    case 1:
        line = (PrimLineG2*)&g_PrimBuf[self->primIndex];
        for (i = 0; line != NULL; i++, line = line->next) {
            if (line->timer == 0) {
                line->preciseX.i.hi = line->x1;
                line->preciseY.i.hi = line->y1;
                line->preciseX.val += line->velocityX.val;
                line->preciseY.val += line->velocityY.val;
                line->x1 = line->preciseX.i.hi;
                line->y1 = line->preciseY.i.hi;
                if (line->y1 > line->delay) {
                    line->timer++;
                    line->xLength = line->x0 - line->x1;
                    line->yLength = line->y0 - line->y1;
                }
            } else {
                line->preciseX.i.hi = line->x1;
                line->preciseY.i.hi = line->y1;
                line->preciseX.val += line->velocityX.val;
                line->preciseY.val += line->velocityY.val;
                line->x1 = line->preciseX.i.hi;
                line->y1 = line->preciseY.i.hi;
                line->y0 = line->y1 + line->yLength;
                line->x0 = line->x1 + line->xLength;
                if (line->y0 >= 0xD8) {
                    self->step = 2;
                }
            }
        }
        self->ext.subweapon.timer++;
        break;
    case 2:
        if (self->params == 0x18) {
            g_Ric.unk4E = 1;
        }
        DestroyEntity(self);
        break;
    }
    g_Ric.timers[PL_T_3] = 16;
}

void DebugShowWaitInfo(const char* msg) {
    g_CurrentBuffer = g_CurrentBuffer->next;
    FntPrint(msg);
    if (D_us_801D087C++ & 4) {
        FntPrint("\no\n");
    }
    DrawSync(0);
    VSync(0);
    PutDrawEnv(&g_CurrentBuffer->draw);
    PutDispEnv(&g_CurrentBuffer->disp);
    FntFlush(-1);
}

void DebugInputWait(const char* msg) {
    while (PadRead(0))
        DebugShowWaitInfo(msg);
    while (PadRead(0) == 0)
        DebugShowWaitInfo(msg);
}

void RicEntitySubwpnHolyWaterFlame(Entity* self) {
    s16 sp10[5];
    s16 sp20[5];
    s16 xMod;
    s16 variant;
    s16* primYPtr;
    s32 doubleparams;
    u8 randR;
    u8 randG;
    u8 randB;
    s16 flameHeight;
    s16 angle;
    u8 texX;
    u8 texY;
    s32 i;
    Primitive* prim;

    variant = self->params >> 8;
    texX = D_us_801828C8[(g_GameTimer & 7) * 2 + 0];
    texY = D_us_801828C8[(g_GameTimer & 7) * 2 + 1];
    switch (self->step) {
    case 0:
        self->ext.holywater.subweaponId = 0xB;
        InitializeEntity(D_us_8018046C);
        self->ext.holywater.hitboxState = self->hitboxState;
        self->hitboxState = 0;
        randR = (rand() & 0x3F) + 0x2F;
        randG = (rand() & 0x3F) + 0x6F;
        randB = (rand() & 0x7F) + 0x6F;
        self->primIndex = g_api.AllocPrimitives(PRIM_GT4, 4);
        if (self->primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        prim = &g_PrimBuf[self->primIndex];
        i = 0;
        while (prim) {
            prim->r0 = prim->r1 = prim->r2 = prim->r3 = randR;
            prim->g0 = prim->g1 = prim->g2 = prim->g3 = randG;
            prim->b0 = prim->b1 = prim->b2 = prim->b3 = randB;
            if (i == 0) {
                prim->r0 = prim->r1 = 0;
                prim->g0 = prim->g1 = 0;
                prim->b0 = prim->b1 = 0;
            }
            prim->clut = PAL_UNK_1B0;
            prim->tpage = 0x1A;
            prim->priority = RIC.zPriority + 2;
            prim->drawMode = DRAW_UNK_40 | DRAW_TPAGE2 | DRAW_TPAGE |
                             DRAW_HIDE | DRAW_COLORS | DRAW_UNK02 | DRAW_TRANSP;
            i++;
            prim = prim->next;
        }
        self->flags = FLAG_POS_CAMERA_LOCKED | FLAG_HAS_PRIMS;
        self->ext.holywater.timer = 1;
        self->step = 1; // InitializeEntity already counted one step
        break;
    case 1:
        if (--self->ext.holywater.timer == 0) {
            self->hitboxWidth = 4;
            self->hitboxState = self->ext.holywater.hitboxState;
            self->posY.i.hi -= 10;
            RicCreateEntFactoryFromEntity(self, FACTORY(BP_EMBERS, 3), 0);
            self->posY.i.hi += 10;
            self->ext.holywater.timer = 0x50;
            self->ext.holywater.unk80 = (rand() & 0xF) + 0x12;
            self->ext.holywater.angle = rand() & 0xFFF;
            self->step++;
        }
        break;
    case 2:
        if (self->flags & FLAG_DEAD) {
            DestroyEntity(self);
            return;
        }
        xMod = -1;
        if (self->facingLeft) {
            xMod = -xMod;
        }
        angle = self->ext.holywater.angle;
        self->ext.holywater.angle += 0xC0;
        for (i = 0; i < 4; i++) {
            sp10[i] = self->posX.i.hi + (rsin(angle + (i * 1024)) >> 10);
        }
        sp10[4] = self->posX.i.hi;
        sp10[0] = self->posX.i.hi + xMod;
        angle = (self->ext.holywater.timer - 16) * 64 + 0xC00;
        flameHeight = (rsin(angle) >> 8) + self->ext.holywater.unk80;
        flameHeight = flameHeight * 3 >> 1;
        sp20[0] = self->posY.i.hi - flameHeight;
        sp20[4] = self->posY.i.hi;
        sp20[2] = (sp20[0] + sp20[4]) / 2;
        sp20[1] = (sp20[0] + sp20[2]) / 2;
        sp20[3] = (sp20[2] + sp20[4]) / 2;
        prim = &g_PrimBuf[self->primIndex];
        if (--self->ext.holywater.timer < 0) {
            DestroyEntity(self);
            return;
        }
        if (self->ext.holywater.timer & 3) {
            self->hitboxState = 0;
        } else {
            self->hitboxState = self->ext.holywater.hitboxState;
        }
        if (self->ext.holywater.timer < 0x15) {
            self->hitboxState = 0;
        }
        i = 0;
        while (prim) {
            if (self->ext.holywater.timer <= variant * 2 + 0x18) {
                if (prim->g0 >= 10) {
                    prim->g0 -= 5;
                }
                if (prim->b0 >= 10) {
                    prim->b0 -= 5;
                }
                if (prim->r0 >= 16) {
                    prim->r0 -= 8;
                }
                prim->g1 = prim->g2 = prim->g3 = prim->g0;
                prim->r1 = prim->r2 = prim->r3 = prim->r0;
                prim->b1 = prim->b2 = prim->b3 = prim->b0;
            }
            prim->x0 = sp10[i] - 8;
            prim->x1 = sp10[i] + 8;
            prim->y0 = sp20[i];
            prim->y1 = sp20[i];
            prim->x2 = sp10[i + 1] - 8;
            prim->x3 = sp10[i + 1] + 8;
            prim->y2 = sp20[i + 1];
            prim->y3 = sp20[i + 1];
            prim->drawMode &= ~DRAW_HIDE;
            prim->u0 = prim->u1 = texX + 0x80 - i * 7;
            prim->u2 = prim->u3 = texX + 0x80 - ((i + 1) * 7);
            prim->v0 = prim->v2 = texY + 0x80;
            prim->v1 = prim->v3 = texY + 0x90;
            if ((sp20[4] - sp20[0]) < 7) {
                prim->drawMode |= DRAW_HIDE;
                DestroyEntity(self);
                return;
            }
            i++;
            prim = prim->next;
        }
        self->hitboxHeight = flameHeight >> 1;
        self->hitboxOffY = (-flameHeight >> 1);
        break;
    }
    g_Ric.timers[PL_T_3] = 2;
}

void RicEntitySubwpnCrashCross(Entity* self) {
    s16 psp_s4;
    s16 psp_s3;
    s16 right;
    s16 left;
    Primitive* prim;

    psp_s4 = 3;
    psp_s3 = 1;
    self->posY.i.hi = 0x78;
    self->posX.i.hi = RIC.posX.i.hi;
    switch (self->step) {
    case 0:
        self->ext.crashcross.subweaponId = 0xC;
        InitializeEntity(D_us_8018049C);
        self->primIndex = g_api.AllocPrimitives(PRIM_GT4, 1);
        if (self->primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->flags =
            FLAG_UNK_10000000 | FLAG_HAS_PRIMS;
        self->ext.crashcross.unk80 = 1;
        self->zPriority = 0xC2;
        LoadImage(&D_us_80182968, (u_long*)D_us_80182908);
        g_api.PlaySfx(0x6DF);
        g_api.PlaySfx(SFX_TELEPORT_BANG_B);
        self->step = 1; // InitializeEntity already counted one step
        break;
    case 1:
        self->ext.crashcross.unk7E.val += psp_s4;
        self->ext.crashcross.unk82 += psp_s4 * 2;
        if (self->ext.crashcross.unk7E.i.lo >= 0x70) {
            RicCreateEntFactoryFromEntity(self, BP_CRASH_CROSSES_ONLY, 0);
            RicCreateEntFactoryFromEntity(self, BP_CRASH_CROSS_PARTICLES, 0);
            self->step++;
        }
        break;
    case 2:
        if (g_Timer & 1) {
            self->ext.crashcross.unk7C += psp_s3;
            self->ext.crashcross.unk80 += psp_s3 * 2;
            if (self->ext.crashcross.unk80 >= 0x2C) {
                self->step++;
                self->ext.crashcross.unk84 = 0x80;
            }
        }
        break;
    case 3:
        if (--self->ext.crashcross.unk84 == 0) {
            g_api.SetFadeMode(FADE_NONE);
            left = self->posX.i.hi - self->ext.crashcross.unk7C;
            if (left < 0) {
                left = 0;
            }
            right = self->posX.i.hi + self->ext.crashcross.unk7C;
            if (right > 0xFF) {
                right = 0xFF;
            }
            g_api.PlaySfx(SFX_WEAPON_APPEAR);
            self->step++;
        }
        break;
    case 4:
        psp_s3 *= 3;
        left = abs(self->posX.i.hi - 0x80);
        psp_s3 = psp_s3 * (left + 0x80) / 112;
        self->ext.crashcross.unk7C += psp_s3;

        left = self->posX.i.hi - self->ext.crashcross.unk7C;
        if (left < 0) {
            left = 0;
        }
        right = self->posX.i.hi + self->ext.crashcross.unk7C;
        if (right > 0xFF) {
            right = 0xFF;
        }
        if (right - left > 0xF8) {
            g_Ric.unk4E = 1;
            DestroyEntity(self);
            return;
        }
        break;
    }
    self->hitboxOffY = 0;
    self->hitboxHeight = self->ext.crashcross.unk7E.val;
    if (self->step == 4) {
        self->hitboxWidth = (right - left) >> 1;
        self->hitboxOffX = ((left + right) >> 1) - self->posX.i.hi;
    } else {
        self->hitboxWidth = self->ext.crashcross.unk7C;
        self->hitboxOffX = 0;
    }
    prim = &g_PrimBuf[self->primIndex];
    prim->x0 = prim->x2 = self->posX.i.hi - self->ext.crashcross.unk7C;
    prim->y1 = prim->y0 = self->posY.i.hi - self->ext.crashcross.unk7E.val;
    prim->x1 = prim->x3 = prim->x0 + self->ext.crashcross.unk80;
    prim->y2 = prim->y3 = prim->y0 + self->ext.crashcross.unk82;
    prim->u0 = prim->u2 = 1;
    prim->u1 = prim->u3 = 0x30;
    prim->v0 = prim->v1 = prim->v2 = prim->v3 = 0xF8;
    prim->tpage = 0x11C;
    if (self->step == 4) {
        prim->x0 = prim->x2 = left;
        prim->x1 = prim->x3 = right;
    }
    prim->drawMode = DRAW_TPAGE2 | DRAW_TPAGE | DRAW_TRANSP;
    prim->priority = self->zPriority;
    g_Ric.timers[PL_T_3] = 2;
}

void func_us_801C488C(Entity* self) {
    Primitive* prim;
    s32 posX;
    s32 posY;

    if (self->step == 0) {
        self->primIndex = g_api.AllocPrimitives(PRIM_GT4, 1);
        if (self->primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->flags = FLAG_POS_CAMERA_LOCKED | FLAG_HAS_PRIMS;
        self->velocityY = FIX(0.5);
        self->posX.i.hi += (rand() & 0xF) - 8;
        self->posY.i.hi += (rand() & 0xF) - 4;
#if defined(VERSION_PSP)
        prim = &g_PrimBuf[self->primIndex];
        prim->clut = PAL_UNK_1B0;
#endif
        prim = &g_PrimBuf[self->primIndex];
        prim->clut = PAL_UNK_1B0;
        prim->tpage = 0x1A;
        prim->b0 = 0;
        prim->b1 = 0;
        prim->priority = self->zPriority + 4;
        prim->drawMode = DRAW_TPAGE2 | DRAW_TPAGE | DRAW_TRANSP;
        func_us_801BB5BC(prim, self->posX.i.hi, self->posY.i.hi);
        self->step++;
    } else {
        prim = &g_PrimBuf[self->primIndex];
        self->posY.val += self->velocityY;
        if (func_us_801BB5BC(prim, self->posX.i.hi, self->posY.i.hi)) {
            DestroyEntity(self);
        }
    }
}

void RicEntitySubwpnCrossTrail(Entity* self) {
    s16* temp;

    switch (self->step) {
    case 0:
        self->flags = FLAG_UNK_10000000 | FLAG_POS_CAMERA_LOCKED;
        // the parent pointer is set in RicEntityFactory.
        // the value of unk84 is set in RicEntitySubwpnCross
        self->ext.crossBoomerang.unk84 =
            self->ext.crossBoomerang.parent->ext.crossBoomerang.unk84;
        self->animSet = ANIMSET_OVL(4);
        self->animCurFrame = D_us_80182994[self->params];
        self->unk5A = 0x44;
        self->palette = PAL_FLAG(PAL_UNK_1B0);
        self->blendMode = BLEND_TRANSP;
        self->facingLeft = RIC.facingLeft;
        self->zPriority = RIC.zPriority;
        self->drawFlags = ENTITY_ROTATE;
        self->rotate = ROT(270);
        self->step++;
        break;
    case 1:
        self->rotate -= ROT(11.25);
        if (self->ext.crossBoomerang.parent->step == 6 ||
            self->ext.crossBoomerang.parent->step == 7) {
            self->step++;
            self->ext.crossBoomerang.timer = (self->params + 1) * 4;
        }
        break;
    case 2:
        self->rotate -= ROT(11.25);
        if (--self->ext.crossBoomerang.timer == 0) {
            DestroyEntity(self);
            return;
        }
        break;
    }

    // get the x and y position from the parent (must align)
    temp = (s16*)&self->ext.crossBoomerang.unk84[0];
    temp += self->ext.crossBoomerang.unk80 * 2;
    self->posX.i.hi = *temp - g_Tilemap.scrollX.i.hi;
    temp++;
    self->posY.i.hi = *temp - g_Tilemap.scrollY.i.hi;
    self->ext.crossBoomerang.unk80++;
    self->ext.crossBoomerang.unk80 &= 0x3F;
}

void RicEntitySubwpnCrashCrossParticles(Entity* self) {
    Primitive* prim;
    s16 psp_s4;
    s16 psp_s3;
    s16 psp_s2;
    u8* psp_s1;
    s16 temp_s0;

    if (self->step == 0) {
        self->primIndex = g_api.AllocPrimitives(PRIM_GT4, 0x20);
        if (self->primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->flags =
            FLAG_UNK_10000000 | FLAG_HAS_PRIMS;
        self->ext.timer.t = 0xC0;
        self->step++;
        return;
    }
    if (--self->ext.timer.t == 0) {
        DestroyEntity(self);
        return;
    }
    if (self->ext.timer.t > 8 && !(self->ext.timer.t & 3)) {
        for (prim = &g_PrimBuf[self->primIndex]; prim; prim = prim->next) {
            if (prim->r0 == 0) {
                prim->r0 = 1;
                prim->r1 = 0;
                break;
            }
        }
    }
    for (prim = &g_PrimBuf[self->primIndex]; prim; prim = prim->next) {
        if (!prim->r0) {
            continue;
        }
        if (prim->r1 == 0) {
            temp_s0 = rand() & 0x3F;
            prim->g0 = (rand() % 237) + 9;
            prim->g1 = 0xF0 - (rand() & 0x20);
            prim->clut = PAL_UNK_1B0;
            prim->tpage = 0x1A;
            prim->b0 = 0;
            prim->priority = RIC.zPriority + temp_s0 - 0x20;
            prim->drawMode = DRAW_TPAGE2 | DRAW_TPAGE | DRAW_TRANSP;
            prim->g3 = ((u8)temp_s0 >> 2) + 4;
            prim->r1++;
        } else {
            prim->g1 -= prim->g3;
            if ((u8)prim->b0 >= 6 || (u8)prim->g1 < 0x18) {
                prim->drawMode = DRAW_HIDE;
                prim->r0 = 0;
            }
        }
        if (!prim->r0) {
            continue;
        }
        psp_s4 = 0;
        psp_s3 = 0;
        psp_s4 |= prim->g0;
        psp_s3 |= prim->g1;
        psp_s1 = D_us_80181524[0];
        psp_s1 += prim->b0 * 8; // weird array indexing
        if (prim->b0 >= 3) {
            psp_s2 = 4;
        } else {
            psp_s2 = 8;
        }
        prim->x0 = psp_s4 - psp_s2;
        prim->y0 = psp_s3 - psp_s2;
        prim->x1 = psp_s4 + psp_s2;
        prim->y1 = psp_s3 - psp_s2;
        prim->x2 = psp_s4 - psp_s2;
        prim->y2 = psp_s3 + psp_s2;
        prim->x3 = psp_s4 + psp_s2;
        prim->y3 = psp_s3 + psp_s2;
        prim->u0 = *psp_s1;
        psp_s1++;
        prim->v0 = *psp_s1;
        psp_s1++;
        prim->u1 = *psp_s1;
        psp_s1++;
        prim->v1 = *psp_s1;
        psp_s1++;
        prim->u2 = *psp_s1;
        psp_s1++;
        prim->v2 = *psp_s1;
        psp_s1++;
        prim->u3 = *psp_s1;
        psp_s1++;
        prim->v3 = *psp_s1;
        psp_s1++;
        if (!(g_GameTimer & 1)) {
            prim->b0++;
        }
    }
}

void RicEntitySubwpnThrownAxe(Entity* self) {
    u8 rVal;
    u8 gVal;
    u8 bVal;
    s16 xVar;
    s16 yVar;
    s32 rgbIdx;
    s16 angle0;
    s16 angle1;
    s16 angle2;
    s16 angle3;
    s16 angleOffset;
    s32 twentyone;
    Primitive* prevPrim;
    s32 i;
    Primitive* prim;
    s32 graphicsTemp;

    twentyone = 21;

    switch (self->step) {
    case AXE_INIT:
        self->ext.subwpnAxe.subweaponId = 2;
        InitializeEntity(D_us_80180478);
        self->primIndex = g_api.AllocPrimitives(PRIM_GT4, 5);
        if (self->primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->flags = FLAG_POS_CAMERA_LOCKED | FLAG_UNK_10000000 | FLAG_HAS_PRIMS;
        self->facingLeft = (RIC.facingLeft + 1) & 1;
        RicSetSpeedX(FIX(-2));
        self->velocityY = FIX(-6);
        if (self->facingLeft) {
            self->ext.subwpnAxe.unk7C = 0x400;
        } else {
            self->ext.subwpnAxe.unk7C = 0xC00;
        }

        self->posY.i.hi -= 12;
        for (prim = &g_PrimBuf[self->primIndex], i = 0; prim != NULL; i++,
            prim = prim->next) {
            prim->tpage = 0x1C;
            prim->u0 = 0;
            prim->v0 = 0;
            prim->u1 = 0x18;
            prim->v1 = 0;
            prim->u2 = 0;
            prim->v2 = 0x28;
            prim->u3 = 0x18;
            prim->v3 = 0x28;
            prim->priority = RIC.zPriority - 2;
            if (i != 0) {
                prim->drawMode = DRAW_UNK_100 | DRAW_TPAGE2 | DRAW_TPAGE |
                                 DRAW_HIDE | DRAW_COLORS | DRAW_TRANSP;
                self->ext.subwpnAxe.unk8C[i - 1] = 0;
                self->ext.subwpnAxe.unk90[i - 1] = 0;
                self->ext.subwpnAxe.unk94[i - 1] = 0;
            } else {
                prim->drawMode = DRAW_UNK_100 | DRAW_HIDE;
            }
        }
        self->hitboxWidth = 12;
        self->hitboxHeight = 12;
        g_api.PlaySfx(SFX_WEAPON_SWISH_C);
        self->ext.subwpnAxe.unk98 = 0x7F;
        self->step = 1; // InitializeEntity already counted one step
        break;
    case AXE_FLYING:
        if (self->facingLeft) {
            angleOffset = -0x80;
        } else {
            angleOffset = 0x80;
        }
        self->ext.subwpnAxe.unk7C += angleOffset;
        if (!(self->ext.subwpnAxe.unk7C & 0x3FF)) {
            g_api.PlaySfxVolPan(
                SFX_WEAPON_SWISH_C, self->ext.subwpnAxe.unk98, 0);
            self->ext.subwpnAxe.unk98 -= 8;
            if (self->ext.subwpnAxe.unk98 < 0) {
                self->ext.subwpnAxe.unk98 = 0;
            }
        }
        self->velocityY += FIX(34.0 / 128);
        if (self->velocityY > FIX(8)) {
            self->velocityY = FIX(8);
        }
        self->posX.val += self->velocityX;
        self->posY.val += self->velocityY;
        if (self->posY.i.hi > 0x100) {
            DestroyEntity(self);
            return;
        }
        if (self->hitFlags == 2 || (self->flags & FLAG_DEAD)) {
            self->hitboxState = 0;
            self->velocityX = -(self->velocityX / 2);
            self->velocityY = FIX(-3);
            self->step = AXE_BOUNCE;
        }
        break;
    case AXE_BOUNCE:
        if (self->facingLeft) {
            angleOffset = 0xC0;
        } else {
            angleOffset = -0xC0;
        }
        self->ext.subwpnAxe.unk7C += angleOffset;
        self->velocityY += FIX(18.0 / 128);
        if (self->velocityY > FIX(8)) {
            self->velocityY = FIX(8);
        }
        self->posX.val += self->velocityX;
        self->posY.val += self->velocityY;
        if (self->posY.i.hi > 0x100) {
            DestroyEntity(self);
            return;
        }
        break;
    }

    if (self->poseTimer == 0) {
        graphicsTemp = self->pose;
        self->ext.subwpnAxe.unk8C[graphicsTemp] = 0;
        self->ext.subwpnAxe.unk90[graphicsTemp] = 1;
        self->ext.subwpnAxe.unk94[graphicsTemp] = 1;
        graphicsTemp++;
        graphicsTemp &= 3;
        self->pose = graphicsTemp;
        self->poseTimer = 2;
    } else {
        self->poseTimer--;
    }
    for (prim = &g_PrimBuf[self->primIndex], prevPrim = prim,
        graphicsTemp = ((g_GameTimer >> 1) & 1) + 0x1AB, i = 0;
         prim != NULL; i++, prim = prim->next) {
        prim->clut = graphicsTemp;
        if (i == 0) {
            if (self->facingLeft) {
                angle0 = 0x560;
                angle1 = 0x2A0;
                angle2 = 0xAA0;
                angle3 = 0xD60;
            } else {
                angle1 = 0x560;
                angle0 = 0x2A0;
                angle3 = 0xAA0;
                angle2 = 0xD60;
            }

            xVar = self->posX.i.hi;
            yVar = self->posY.i.hi;
            angleOffset = self->ext.subwpnAxe.unk7C;
            angle0 += angleOffset;
            angle1 += angleOffset;
            angle2 += angleOffset;
            angle3 += angleOffset;
            prim->x0 = xVar + (((rcos(angle0) << 4) * twentyone) >> 0x10);
            prim->y0 = yVar + -(((rsin(angle0) << 4) * twentyone) >> 0x10);
            prim->x1 = xVar + (((rcos(angle1) << 4) * twentyone) >> 0x10);
            prim->y1 = yVar + -(((rsin(angle1) << 4) * twentyone) >> 0x10);
            prim->x2 = xVar + (((rcos(angle2) << 4) * twentyone) >> 0x10);
            prim->y2 = yVar + -(((rsin(angle2) << 4) * twentyone) >> 0x10);
            prim->x3 = xVar + (((rcos(angle3) << 4) * twentyone) >> 0x10);
            prim->y3 = yVar + -(((rsin(angle3) << 4) * twentyone) >> 0x10);
            prim->drawMode &= ~DRAW_HIDE;
        } else if (self->ext.subwpnAxe.unk90[i - 1]) {
            if (self->ext.subwpnAxe.unk94[i - 1]) {
                self->ext.subwpnAxe.unk94[i - 1] = 0;
                prim->x0 = prevPrim->x0;
                prim->y0 = prevPrim->y0;
                prim->x1 = prevPrim->x1;
                prim->y1 = prevPrim->y1;
                prim->x2 = prevPrim->x2;
                prim->y2 = prevPrim->y2;
                prim->x3 = prevPrim->x3;
                prim->y3 = prevPrim->y3;
            }
            rgbIdx = self->ext.subwpnAxe.unk8C[i - 1]++;
            if (rgbIdx < 10) {
                rVal = D_us_8018299C[rgbIdx * 4 + 0];
                gVal = D_us_8018299C[rgbIdx * 4 + 1];
                bVal = D_us_8018299C[rgbIdx * 4 + 2];
                prim->r0 = rVal;
                prim->g0 = gVal;
                prim->b0 = bVal;
                prim->r1 = rVal;
                prim->g1 = gVal;
                prim->b1 = bVal;
                prim->r2 = rVal;
                prim->g2 = gVal;
                prim->b2 = bVal;
                prim->r3 = rVal;
                prim->g3 = gVal;
                prim->b3 = bVal;
                prim->drawMode &= ~DRAW_HIDE;
            } else {
                self->ext.subwpnAxe.unk90[i - 1] = 0;
                prim->drawMode |= DRAW_HIDE;
            }
        }
    }
}

void RicEntitySubwpnKnife(Entity* self) {
    Collider collider;
    Primitive* prim;
    s16 offsetX;
    s16 offsetY;
    s16 angle1;
    s16 angle2;
    s16 angle3;
    s16 angle4;
    s16 x;
    s16 y;
    s16 xCol;
    s32 modX;
    s32 modY;
    s32 i;

    switch (self->step) {
    case DAGGER_INIT:
        self->primIndex = g_api.AllocPrimitives(PRIM_GT4, 2);
        if (self->primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->flags = FLAG_POS_CAMERA_LOCKED | FLAG_HAS_PRIMS;
        self->facingLeft = RIC.facingLeft;
        // bo6: no subweapon params here
        self->hitboxWidth = 4;
        self->hitboxHeight = 2;
        self->hitboxOffX = 4;
        self->hitboxOffY = 0;
        // This conditional block is very different from DRA
        if (self->params & 0xFF00) {
            self->posY.i.hi += D_us_801829C4[D_us_801D10CC & 7];
            D_us_801D10CC++;
        } else {
            self->posY.i.hi -= 9;
        }
        prim = &g_PrimBuf[self->primIndex];
        prim->tpage = 0x1C;
        prim->clut = PAL_UNK_1AB;
        prim->u0 = prim->u1 = 0x18;
        prim->v0 = prim->v2 = 0x18;
        prim->u2 = prim->u3 = 0x20;
        prim->v1 = prim->v3 = 0;
        prim->priority = RIC.zPriority + 2;
        prim->drawMode = DRAW_HIDE | DRAW_UNK02;

        prim = prim->next;
        prim->type = PRIM_LINE_G2;
        prim->priority = RIC.zPriority + 2;
        prim->drawMode =
            DRAW_TPAGE2 | DRAW_TPAGE | DRAW_HIDE | DRAW_UNK02 | DRAW_TRANSP;
        prim->r0 = 0x7F;
        prim->g0 = 0x3F;
        prim->b0 = 0;
        RicSetSpeedX(FIX(8));
        g_api.PlaySfx(SFX_WEAPON_SWISH_C);
        self->step++;
        break;
    case DAGGER_FLYING:
        self->ext.subweapon.timer++;
        if (self->velocityX > 0) {
            xCol = 8;
        }
        if (self->velocityX < 0) {
            xCol = -8;
        }
        if (self->hitFlags == 1) {
            self->ext.subweapon.timer = 4;
            self->step = 3;
            self->hitboxState = 0;
            return;
        }
        for (i = 0; i < 8; i++) {
            if (self->velocityX > 0) {
                self->posX.i.hi++;
            }
            if (self->velocityX < 0) {
                self->posX.i.hi--;
            }
            g_api.CheckCollision(
                self->posX.i.hi + xCol, self->posY.i.hi, &collider, 0);
            if ((self->hitFlags == 2) || (collider.effects & EFFECT_BLOCK)) {
                self->ext.subweapon.timer = 64;
                self->velocityX = -(self->velocityX >> 3);
                self->velocityY = FIX(-2.5);
                self->hitboxState = 0;
                self->posX.i.hi += xCol;
                RicCreateEntFactoryFromEntity(self, FACTORY(BP_42, 2), 0);
                self->posX.i.hi -= xCol;
                g_api.PlaySfx(SFX_UI_SUBWEAPON_TINK);
                self->step++;
                return;
            }
        }
        x = self->posX.i.hi;
        y = self->posY.i.hi;
        offsetX = 12;
        offsetY = 8;
        if (self->facingLeft) {
            offsetX = -offsetX;
            offsetY = -offsetY;
        }
        prim = &g_PrimBuf[self->primIndex];
        prim->x0 = x - offsetX;
        prim->y0 = y - 4;
        prim->x1 = x + offsetX;
        prim->y1 = y - 4;
        prim->x2 = x - offsetX;
        prim->y2 = y + 4;
        prim->x3 = x + offsetX;
        prim->y3 = y + 4;
        // Difference here vs DRA in how g_GameTimer works with clut
        prim->clut = ((g_GameTimer >> 1) & 1) + 0x1AB;
        prim->drawMode &= ~DRAW_HIDE;
        prim = prim->next;
        prim->x0 = x - offsetY;
        prim->y0 = y - 1;
        prim->x1 = x - (offsetX * (self->ext.subweapon.timer / 2));
        prim->y1 = y - 1;
        prim->drawMode &= ~DRAW_HIDE;
        if (self->step != 1) {
            prim->drawMode |= DRAW_HIDE;
        }
        break;
    case DAGGER_BOUNCE:
        prim = &g_PrimBuf[self->primIndex];
        if (--self->ext.subweapon.timer == 0) {
            DestroyEntity(self);
            return;
        }
        if (self->ext.subweapon.timer == 0x20) {
            prim->drawMode |=
                DRAW_TPAGE2 | DRAW_TPAGE | DRAW_COLORS | DRAW_TRANSP;
            PGREY(prim, 0) = PGREY(prim, 1) = PGREY(prim, 2) = PGREY(prim, 3) =
                0x60;
        }
        self->posX.val += self->velocityX;
        self->posY.val += self->velocityY;
        self->velocityY += FIX(0.125);
        x = self->posX.i.hi;
        y = self->posY.i.hi;
        offsetX = 12;
        if (self->facingLeft == 0) {
            angle1 = 0x800 - 0xD2;
            angle2 = 0xD2;
            angle3 = 0x800 + 0xD2;
            angle4 = -0xD2;
            self->rotate -= ROT(11.25);
        } else {
            angle2 = 0x800 - 0xD2;
            angle1 = 0xD2;
            angle4 = 0x800 + 0xD2;
            angle3 = -0xD2;
            self->rotate += ROT(11.25);
        }
        angle1 += self->rotate;
        angle2 += self->rotate;
        angle3 += self->rotate;
        angle4 += self->rotate;
        // offsetX is not used at all down here, but this block is needed.
        // It doesn't show up in the asm at all, but moves everything else into
        // alignment. Yet another lesson from PSP.
        if (self->facingLeft) {
            offsetX = -offsetX;
        }
        prim = &g_PrimBuf[self->primIndex];
        modX = (rcos(angle1) * 0xCA0) >> 0x14;
        modY = -(rsin(angle1) * 0xCA0) >> 0x14;
        prim->x0 = x + (s16)modX;
        prim->y0 = y - (s16)modY;
        modX = (rcos(angle2) * 0xCA0) >> 0x14;
        modY = -(rsin(angle2) * 0xCA0) >> 0x14;
        prim->x1 = x + (s16)modX;
        prim->y1 = y - (s16)modY;
        modX = (rcos(angle3) * 0xCA0) >> 0x14;
        modY = -(rsin(angle3) * 0xCA0) >> 0x14;
        prim->x2 = x + (s16)modX;
        prim->y2 = y - (s16)modY;
        modX = (rcos(angle4) * 0xCA0) >> 0x14;
        modY = -(rsin(angle4) * 0xCA0) >> 0x14;
        prim->x3 = x + (s16)modX;
        prim->y3 = y - (s16)modY;
        // same deal here with g_GameTimer
        prim->clut = ((g_GameTimer >> 1) & 1) + 0x1AB;
        if (self->ext.subweapon.timer < 0x21) {
            prim->r0 -= 2;
            prim->g0 = prim->b0 = PGREY(prim, 1) = PGREY(prim, 2) =
                PGREY(prim, 3) = prim->r0;
        }
        prim->drawMode &= ~DRAW_HIDE;
        prim = prim->next;
        prim->drawMode |= DRAW_HIDE;
        break;
    case DAGGER_HIT_ENEMY:
        if (--self->ext.subweapon.timer == 0) {
            DestroyEntity(self);
            return;
        }
        break;
    }
}

void RicEntitySubwpnReboundStone(Entity* self) {
    s16 playerX;
    s16 playerY;
    Collider collider;
    s32 speed;
    s32 currX;
    s32 currY;
    s32 collX;
    s32 collY;
    s32 deltaX;
    s32 deltaY;
    s32 i;
    s32 colliderFlags;
    PrimLineG2* prim;

    speed = 0x400;
    self->ext.reboundStone.unk82 = 0;
    playerX = self->posX.i.hi;
    playerY = self->posY.i.hi;

    switch (self->step) {
    case 0:
        self->primIndex = g_api.AllocPrimitives(PRIM_LINE_G2, 16);
        if (self->primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->posY.i.hi -= 0x10;
        playerY = self->posY.i.hi;
        for (prim = (PrimLineG2*)&g_PrimBuf[self->primIndex], i = 0;
             prim != NULL; i++, prim = prim->next) {
            PGREY(prim, 0) = PGREY(prim, 1) = 0xFF;
            prim->priority = RIC.zPriority + 2;
            prim->drawMode =
                DRAW_TPAGE2 | DRAW_TPAGE | DRAW_UNK02 | DRAW_TRANSP;
            if (i != 0) {
                prim->drawMode |= DRAW_HIDE;
            }
            prim->x0 = prim->x1 = playerX;
            prim->y0 = prim->y1 = playerY;
            prim->timer = 20;
        }
        self->flags =
            FLAG_POS_CAMERA_LOCKED | FLAG_UNK_10000000 | FLAG_HAS_PRIMS;
        self->zPriority = RIC.zPriority + 2;

        if (RIC.facingLeft) {
            self->ext.reboundStone.stoneAngle = 0x980;
        } else {
            self->ext.reboundStone.stoneAngle = 0xE80;
        }
        self->ext.reboundStone.stoneAngle += (rand() & 0x7F) - 0x40;

        self->ext.reboundStone.lifeTimer = 0x40;
        // bo6: no subweapon params here
        self->hitboxWidth = 4;
        self->hitboxHeight = 4;
        g_api.CheckCollision(self->posX.i.hi, self->posY.i.hi, &collider, 0);
        if (collider.effects & EFFECT_SOLID) {
            self->ext.reboundStone.unk84 = 4;
        }
        self->step++;
        g_api.PlaySfx(SFX_WEAPON_SWISH_C);
        break;
    case 1:
        deltaX = rcos(self->ext.reboundStone.stoneAngle) * 0x10;
        deltaY = -rsin(self->ext.reboundStone.stoneAngle) * 0x10;
        currX = self->posX.val;
        currY = self->posY.val;
        if (!self->ext.reboundStone.unk84) {
            for (i = 0; i < 6; i++) {
                collX = FIX_TO_I(currX);
                collY = FIX_TO_I(currY + deltaY);
                g_api.CheckCollision(collX, collY, &collider, 0);
                colliderFlags =
                    collider.effects &
                    (EFFECT_UNK_8000 | EFFECT_UNK_4000 | EFFECT_UNK_2000 |
                     EFFECT_UNK_1000 | EFFECT_UNK_0800 | EFFECT_BLOCK);
                if (colliderFlags & EFFECT_SOLID) {
                    colliderFlags &= 0xFF00;
                    if (deltaY > 0) {
                        if ((colliderFlags == 0) ||
                            (colliderFlags & EFFECT_UNK_0800)) {
                            ReboundStoneBounce1(0x800);
                        }
                        if (colliderFlags == EFFECT_UNK_8000) {
                            ReboundStoneBounce2(0x200);
                        }
                        if (colliderFlags ==
                            EFFECT_UNK_8000 + EFFECT_UNK_1000) {
                            ReboundStoneBounce2(0x12E);
                        }
                        if (colliderFlags ==
                            EFFECT_UNK_8000 + EFFECT_UNK_2000) {
                            ReboundStoneBounce2(0xA0);
                        }
                        if (colliderFlags ==
                            EFFECT_UNK_8000 + EFFECT_UNK_4000) {
                            ReboundStoneBounce2(0x600);
                        }
                        if (colliderFlags == EFFECT_UNK_8000 + EFFECT_UNK_4000 +
                                                 EFFECT_UNK_1000) {
                            ReboundStoneBounce2(0x6D2);
                        }
                        if (colliderFlags == EFFECT_UNK_8000 + EFFECT_UNK_4000 +
                                                 EFFECT_UNK_2000) {
                            ReboundStoneBounce2(0x760);
                        }
                    }
                    if (deltaY < 0) {
                        if ((colliderFlags == 0) ||
                            (colliderFlags & EFFECT_UNK_8000)) {
                            ReboundStoneBounce1(0x800);
                        }
                        if (colliderFlags == EFFECT_UNK_0800) {
                            ReboundStoneBounce2(0xE00);
                        }
                        if (colliderFlags ==
                            EFFECT_UNK_0800 + EFFECT_UNK_1000) {
                            ReboundStoneBounce2(0xED2);
                        }
                        if (colliderFlags ==
                            EFFECT_UNK_0800 + EFFECT_UNK_2000) {
                            ReboundStoneBounce2(0xF60);
                        }
                        if (colliderFlags ==
                            EFFECT_UNK_0800 + EFFECT_UNK_4000) {
                            ReboundStoneBounce2(0xA00);
                        }
                        if (colliderFlags == EFFECT_UNK_0800 + EFFECT_UNK_4000 +
                                                 EFFECT_UNK_1000) {
                            ReboundStoneBounce2(0x92E);
                        }
                        if (colliderFlags == EFFECT_UNK_0800 + EFFECT_UNK_4000 +
                                                 EFFECT_UNK_2000) {
                            ReboundStoneBounce2(0x8A0);
                        }
                    }
                }
                collY = FIX_TO_I(currY);
                collX = FIX_TO_I(currX + deltaX);
                g_api.CheckCollision(collX, collY, &collider, 0);
                colliderFlags =
                    collider.effects &
                    (EFFECT_UNK_8000 | EFFECT_UNK_4000 | EFFECT_UNK_2000 |
                     EFFECT_UNK_1000 | EFFECT_UNK_0800 | EFFECT_BLOCK);
                if (colliderFlags & EFFECT_SOLID) {
                    colliderFlags &= 0xFF00;
                    // Cases when traveling right
                    if (deltaX > 0) {
                        if ((colliderFlags == 0) ||
                            TEST_BITS(colliderFlags, 0x4800) ||
                            TEST_BITS(colliderFlags, 0xC000)) {
                            ReboundStoneBounce1(0x400);
                        }
                        if (colliderFlags == EFFECT_UNK_0800) {
                            ReboundStoneBounce2(0xE00);
                        }
                        if (colliderFlags ==
                            EFFECT_UNK_0800 + EFFECT_UNK_1000) {
                            ReboundStoneBounce2(0xED2);
                        }
                        if (colliderFlags ==
                            EFFECT_UNK_0800 + EFFECT_UNK_2000) {
                            ReboundStoneBounce2(0xF60);
                        }
                        if (colliderFlags == EFFECT_UNK_8000) {
                            ReboundStoneBounce2(0x200);
                        }
                        if (colliderFlags ==
                            EFFECT_UNK_8000 + EFFECT_UNK_1000) {
                            ReboundStoneBounce2(0x12E);
                        }
                        if (colliderFlags ==
                            EFFECT_UNK_8000 + EFFECT_UNK_2000) {
                            ReboundStoneBounce2(0xA0);
                        }
                    }
                    // Cases when traveling left
                    if (deltaX < 0) {
                        if ((colliderFlags == 0) ||
                            ((colliderFlags & 0x4800) == 0x800) ||
                            ((colliderFlags & 0xC000) == 0x8000)) {
                            ReboundStoneBounce1(0x400);
                        }
                        if (colliderFlags ==
                            EFFECT_UNK_0800 + EFFECT_UNK_4000) {
                            ReboundStoneBounce2(0xA00);
                        }
                        if (colliderFlags == EFFECT_UNK_0800 + EFFECT_UNK_4000 +
                                                 EFFECT_UNK_1000) {
                            ReboundStoneBounce2(0x92E);
                        }
                        if (colliderFlags == EFFECT_UNK_0800 + EFFECT_UNK_4000 +
                                                 EFFECT_UNK_2000) {
                            ReboundStoneBounce2(0x8A0);
                        }
                        if (colliderFlags ==
                            EFFECT_UNK_8000 + EFFECT_UNK_4000) {
                            ReboundStoneBounce2(0x600);
                        }
                        if (colliderFlags == EFFECT_UNK_8000 + EFFECT_UNK_4000 +
                                                 EFFECT_UNK_1000) {
                            ReboundStoneBounce2(0x6D2);
                        }
                        if (colliderFlags == EFFECT_UNK_8000 + EFFECT_UNK_4000 +
                                                 EFFECT_UNK_2000) {
                            ReboundStoneBounce2(0x760);
                        }
                    }
                }
                if (self->ext.reboundStone.unk82) {
                    goto block_93;
                }
                currX += deltaX;
                currY += deltaY;
            }
        } else {
            self->ext.reboundStone.unk84--;
        }
    block_93:
        if (self->ext.reboundStone.unk82) {
            // @bug: should call RicCreateEntFactoryFromEntity instead in case
            // E_FACTORY goes out of order between RIC and DRA
            g_api.CreateEntFactoryFromEntity(self, FACTORY(BP_42, 2), 0);
            g_api.PlaySfx(SFX_UI_SUBWEAPON_TINK);
        }
        if (self->posX.i.hi < -0x40 || self->posX.i.hi > 0x140 ||
            self->posY.i.hi < -0x40 || self->posY.i.hi > 0x140 ||
            self->ext.reboundStone.unk80 == 15) {
            self->step = 2;
        } else {
            deltaX =
                ((rcos(self->ext.reboundStone.stoneAngle) << 4) * speed) >> 8;
            self->posX.val += deltaX;
            deltaY =
                -((rsin(self->ext.reboundStone.stoneAngle) << 4) * speed) >> 8;
            self->posY.val += deltaY;
        }
        break;
    case 2:
        if (--self->ext.reboundStone.lifeTimer == 0) {
            DestroyEntity(self);
            return;
        }
        if (self->ext.reboundStone.lifeTimer == 0x20) {
            self->hitboxState = 0;
        }
        prim = (PrimLineG2*)&g_PrimBuf[self->primIndex];
        while (prim != NULL) {
            prim->timer = 0;
            prim = prim->next;
        }
        break;
    }

    prim = (PrimLineG2*)&g_PrimBuf[self->primIndex];
    i = 0;
    if (self->step == 2) {
        colliderFlags = 4;
    } else {
        colliderFlags = 2;
    }
    // cleaner to use previous 3 lines than to put them in the for's initializer
    for (; prim != NULL; i++, prim = prim->next) {
        if (self->ext.reboundStone.unk82) {
            if (i == self->ext.reboundStone.unk80) {
                prim->x0 = playerX;
                prim->y0 = playerY;
                prim->drawMode &= ~DRAW_HIDE;
            }
        }
        if (i == self->ext.reboundStone.unk80) {
            prim->x1 = self->posX.i.hi;
            prim->y1 = self->posY.i.hi;
        }
        if (!(prim->drawMode & DRAW_HIDE)) {
            if (prim->timer) {
                prim->timer--;
            } else {
                // again not colliderFlags, seems to control stone fading
                if (colliderFlags < prim->b1) {
                    prim->b1 -= colliderFlags;
                }
                PGREY(prim, 0) = PGREY(prim, 1);
            }
        }
    }
}

void RicEntitySubwpnThrownVibhuti(Entity* self) {
    Collider col;
    FakePrim* prim;
    s16 randomAngle;
    s16 x;
    s16 y;
    s16 temp; // used for multiple unrelated things
    s32 i;

    switch (self->step) {
    case 0:
        self->primIndex = g_api.func_800EDB58(PRIM_TILE_ALT, 13);
        if (self->primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->flags = FLAG_POS_CAMERA_LOCKED | FLAG_HAS_PRIMS;
        // bo6: no subweapon params here
        self->hitboxWidth = self->hitboxHeight = 4;
        self->ext.subweapon.timer = 0x80;
        prim = (FakePrim*)&g_PrimBuf[self->primIndex];
        self->posY.i.hi -= 15;
        if (RIC.facingLeft) {
            self->posX.i.hi -= 13;
        } else {
            self->posX.i.hi += 13;
        }
        x = self->posX.i.hi;
        y = self->posY.i.hi;
        i = 0;
        while (true) {
            prim->drawMode = DRAW_UNK02;
            prim->priority = RIC.zPriority - 1;
            if (prim->next == NULL) {
                prim->drawMode &= ~DRAW_HIDE;
                prim->y0 = prim->x0 = prim->w = 0;
                break;
            }
            prim->posX.i.hi = x;
            prim->posY.i.hi = y;
            prim->posX.i.lo = prim->posY.i.lo = 0;
            randomAngle = (rand() & 0xFF) + 0x100;
            temp = (rand() & 0xFF) + 0x80;
            prim->velocityX.val = ((rcos(randomAngle) << 4) * temp >> 9);
            prim->velocityX.val += FIX(0.5);
            prim->velocityY.val = -((rsin(randomAngle) << 4) * temp >> 9);
            prim->velocityX.val = (prim->velocityX.val * 3) >> 1;
            if (self->facingLeft) {
                prim->velocityX.val = -prim->velocityX.val;
            }
            prim->posY.i.hi -= 4;
            prim->delay = 1;
            prim->x0 = prim->posX.i.hi;
            prim->y0 = prim->posY.i.hi;
            prim->r0 = 0xFF;
            prim->g0 = 0xFF;
            prim->b0 = 0xFF;
            prim->w = 2;
            prim->h = 2;
            i++;
            prim = prim->next;
        }
        g_api.PlaySfx(SFX_WEAPON_SWISH_C);
        self->step++;
        break;
    case 1:
        temp = 2;
        if (self->facingLeft) {
            temp = -temp;
        }
        if (--self->ext.subweapon.timer == 0) {
            DestroyEntity(self);
            return;
        }
        prim = (FakePrim*)&g_PrimBuf[self->primIndex];
        i = 0;
        while (true) {
            if (prim->next == NULL) {
                prim->drawMode &= ~DRAW_HIDE;
                prim->y0 = prim->x0 = prim->w = 0;
                break;
            }
            prim->posX.i.hi = prim->x0;
            prim->posY.i.hi = prim->y0;
            if (prim->delay) {
                if (prim->velocityX.val != 0) {
                    prim->posX.val += prim->velocityX.val;
                    g_api.CheckCollision(
                        prim->posX.i.hi + temp, prim->posY.i.hi, &col, 0);
                    if (col.effects & EFFECT_SIDE) {
                        prim->velocityX.val = 0;
                    }
                }
                prim->posY.val += prim->velocityY.val;
                prim->velocityY.val += FIX(12.0 / 128);
                if (prim->velocityY.val > FIX(4)) {
                    prim->velocityY.val = FIX(4);
                }
                if (prim->velocityY.val > 0) {
                    g_api.CheckCollision(
                        prim->posX.i.hi, prim->posY.i.hi, &col, 0);
                    if (col.effects & EFFECT_SOLID) {
                        prim->delay = 0;
                        prim->posY.i.hi += col.unk18;
                        prim->posY.i.hi -= (i % 3 + 1);
                        prim->w = prim->h = 3;
                    }
                }
            }
            if ((self->ext.subweapon.timer & 7) == i) {
                self->posX.i.hi = prim->posX.i.hi;
                self->posY.i.hi = prim->posY.i.hi;
                if (prim->drawMode & DRAW_HIDE) {
                    self->hitboxWidth = self->hitboxHeight = 0;
                } else {
                    self->hitboxWidth = self->hitboxHeight = 4;
                }
                if (prim->delay) {
                    self->hitboxOffY = 0;
                } else {
                    self->hitboxOffY = -6;
                }
            }
            if (self->hitFlags && ((self->ext.subweapon.timer + 1) & 7) == i) {
                prim->drawMode = DRAW_HIDE;
            }
            if (self->ext.subweapon.timer - 1 == i) {
                prim->drawMode = DRAW_HIDE;
            }
            i++;
            prim->x0 = prim->posX.i.hi;
            prim->y0 = prim->posY.i.hi;
            prim = prim->next;
        }
        self->hitFlags = 0;
        break;
    }
}

u8 BO6_PrimDecreaseBrightness(Primitive* prim, u8 amount) {
    s32 i;
    s32 j;
    u8* colorPtr;   // points to an RGB color
    u8* channelPtr; // points to a single channel of that color
    u8 isEnd;

    isEnd = 0;
    colorPtr = &prim->r0;
    for (i = 0; i < 4; colorPtr += OFF(Primitive, r1) - OFF(Primitive, r0),
        i++) {
        for (j = 0; j < 3; j++) {
            channelPtr =
                &colorPtr[j]; // get the red, green, blue, or pad channel
            *channelPtr -= amount;

            if (*channelPtr < 16) {
                *channelPtr = 16;
            } else {
                isEnd |= 1;
            }
        }
    }
    return isEnd;
}

void RicEntitySubwpnAgunea(Entity* self) {
    Primitive* prim;
    s32 heartCost;
    s16 tempX;
    s16 tempY;
    u32 heartBroachesWorn;

    if (g_Ric.status & (PLAYER_STATUS_TRANSFORM | PLAYER_STATUS_UNK10000)) {
        DestroyEntity(self);
        return;
    }
    switch (self->step) {
    case 0:
        self->primIndex = g_api.AllocPrimitives(PRIM_GT4, 1);
        if (self->primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->flags =
            FLAG_POS_CAMERA_LOCKED | FLAG_UNK_10000000 | FLAG_HAS_PRIMS;
        self->facingLeft = RIC.facingLeft;
        // bo6: no subweapon params here
        self->hitboxWidth = self->hitboxHeight = 4;
        self->hitboxOffX = 4;
        self->hitboxOffY = 0;
        self->posY.i.hi = self->ext.agunea.unk82 =
            RIC.posY.i.hi + RIC.hitboxOffY - 8;
        self->posX.i.hi = self->ext.agunea.unk80 = RIC.posX.i.hi;
        prim = &g_PrimBuf[self->primIndex];
        prim->type = PRIM_LINE_G2;
        prim->priority = RIC.zPriority + 2;
        prim->drawMode = DRAW_UNK_100 | DRAW_TPAGE2 | DRAW_TPAGE | DRAW_TRANSP | DRAW_UNK02;
        prim->r1 = 0x60;
        prim->g1 = 0;
        prim->b1 = 0x80;
        RicSetSpeedX(FIX(6));
        g_api.PlaySfx(SFX_WEAPON_SWISH_C);
        self->step++;
        break;
    case 1:
        self->posX.val += self->velocityX;
        if (self->posX.i.hi < -0x40 || self->posX.i.hi > 0x140 ||
            self->posY.i.hi < -0x20 || self->posY.i.hi > 0x120) {
            self->step = 2;
        }
        if (self->hitFlags) {
            self->ext.agunea.parent = self->unkB8;
            self->step = 3;
        }
        break;
    case 4:
        self->posX.i.hi = self->ext.agunea.parent->posX.i.hi;
        self->posY.i.hi = self->ext.agunea.parent->posY.i.hi;
        if (++self->ext.agunea.unk7C < 16) {
            break;
        }
        // Fallthrough
    case 2:
        prim = &g_PrimBuf[self->primIndex];
        if (prim->r1 < 5) {
            DestroyEntity(self);
            return;
        }
        break;
    case 3:
        if ((g_Ric.padPressed & (PAD_UP + PAD_SQUARE)) !=
            (PAD_UP + PAD_SQUARE)) {
            self->step = 4;
        }
        if (!self->ext.agunea.parent->entityId) {
            self->step = 2;
            return;
        }
        if (self->ext.agunea.unk7C &&
            (self->ext.agunea.parent->hitPoints > 0x7000 ||
             !self->ext.agunea.parent->hitPoints ||
             !self->ext.agunea.parent->hitboxState)) {
            self->step = 2;
            return;
        }

        tempX = self->posX.i.hi = self->ext.agunea.parent->posX.i.hi;
        tempY = self->posY.i.hi = self->ext.agunea.parent->posY.i.hi;
        if ((self->ext.agunea.unk7C % 12) == 0) {
            self->posX.i.hi += (rand() & 0xF) - 8;
            self->posY.i.hi += (rand() & 0xF) - 8;
            if (g_Status.hearts >= 5) {
                g_Status.hearts -= 5;
                RicCreateEntFactoryFromEntity(self, BP_AGUNEA_HIT_ENEMY, 0);
                g_api.PlaySfx(SFX_THUNDER_B);
            } else if (self->ext.agunea.unk84 == 0) {
                RicCreateEntFactoryFromEntity(self, BP_AGUNEA_HIT_ENEMY, 0);
                g_api.PlaySfx(SFX_THUNDER_B);
                self->ext.agunea.unk84++;
            } else {
                self->step = 4;
            }
        }
        self->posX.i.hi = tempX;
        self->posY.i.hi = tempY;
        self->ext.agunea.unk7C++;
        break;
    }
    prim = &g_PrimBuf[self->primIndex];
    if (prim->r1 >= 4) {
        prim->r1 -= 4;
    }
    if (prim->g1 >= 4) {
        prim->g1 -= 4;
    }
    if (prim->b1 >= 4) {
        prim->b1 -= 4;
    }
    if (prim->b1 < 5) {
        prim->drawMode |= DRAW_HIDE;
    }
    prim->x0 = self->ext.agunea.unk80;
    prim->y0 = self->ext.agunea.unk82;
    prim->x1 = self->posX.i.hi;
    prim->y1 = self->posY.i.hi;
}

void RicEntityAguneaHitEnemy(Entity* self) {
    Entity* parent;
    Primitive* prim;
    Primitive* temp_s3;
    Primitive* var_a0;
    s16 arctan;
    s16 angle;
    s16 xOffset;
    s16 yOffset;
    s16 temp_s2;
    u8 var_s3;
    s32 i;
    u8 var_s8;

    parent = self->ext.et_801291C4.parent;
    self->posX.i.hi = RIC.posX.i.hi;
    self->posY.i.hi = (RIC.posY.i.hi + RIC.hitboxOffY) - 8;
    if (self->ext.et_801291C4.parent->entityId != 0x2C) {
        switch (self->step) {
        case 0:
            DestroyEntity(self);
            return;
        case 1:
        case 2:
        case 4:
            self->step = 3;
        }
    }
    switch (self->step) {
    case 0:
        self->primIndex = g_api.AllocPrimitives(PRIM_GT4, 0x28);
        if (self->primIndex == -1) {
            DestroyEntity(self);
            break;
        }
        self->flags = FLAG_UNK_10000000 | FLAG_HAS_PRIMS;
        self->facingLeft = RIC.facingLeft;
        self->ext.et_801291C4.unk84 = (rand() & 0x3FF) - 0x200;
        if (self->facingLeft) {
            // @bug: This should be assigned to something. As-is, does nothing.
            self->ext.et_801291C4.unk84 + 0x800;
        }
        self->ext.et_801291C4.unk84 &= 0xFFF;
        self->ext.et_801291C4.unk90 = (self->params >> 8) & 0xFF;
        prim = &g_PrimBuf[self->primIndex];
        self->ext.et_801291C4.prim1 = prim;
        self->ext.et_801291C4.prim2 = prim;
        for (i = 0; prim != NULL;) {
            prim->tpage = 0x1A;
            prim->clut = PAL_UNK_194;
            prim->u0 = prim->u1 = i * 0x10 + 0x90;
            prim->u2 = prim->u3 = prim->u0 + 0x10;
            prim->v0 = prim->v2 = 0xD0;
            prim->v1 = prim->v3 = 0xC0;
            prim->x0 = self->posX.i.hi;
            prim->y0 = self->posY.i.hi;
            prim->x2 = self->posX.i.hi;
            prim->y2 = self->posX.i.hi;
            prim->r0 = prim->g0 = prim->b0 = 0xF0;
            LOW(prim->r1) = LOW(prim->r0);
            LOW(prim->r2) = LOW(prim->r0);
            LOW(prim->r3) = LOW(prim->r0);
            prim->priority = self->zPriority;
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
            i++;
            if (i > 5) {
                i = 0;
            }
        }
        prim = self->ext.et_801291C4.prim1;
        prim->x0 = self->posX.i.hi;
        prim->y0 = self->posY.i.hi;
        prim->x1 = prim->x0;
        prim->y1 = prim->y1 - 0x10;
        prim->x2 = self->posX.i.hi;
        prim->y2 = self->posY.i.hi;
        prim->x3 = prim->x2;
        prim->y3 = prim->y2 - 0x10;
        self->ext.et_801291C4.prim2 = prim;
        while (prim != NULL) {
            prim->clut = PAL_UNK_194;
            prim->r0 = prim->g0 = prim->b0 = 0x80;
            LOW(prim->r1) = LOW(prim->r0);
            LOW(prim->r2) = LOW(prim->r0);
            LOW(prim->r3) = LOW(prim->r0);
            prim->priority = self->zPriority;
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
        self->ext.et_801291C4.unk88 = 0;
        self->step++;
        break;
    case 1:
        for (i = 0; i < 2; i++) {
            prim = self->ext.et_801291C4.prim2;
            temp_s2 = self->ext.et_801291C4.unk84;
            xOffset = parent->posX.i.hi - prim->x2;
            yOffset = parent->posY.i.hi - prim->y2;
            if (abs(xOffset) < 8 && abs(yOffset) < 8) {
                self->step++;
                break;
            }
            if (abs(xOffset) < 0x40 && abs(yOffset) < 0x40) {
                var_s3 = 1;
            } else {
                var_s3 = 0;
            }
            if (!self->ext.et_801291C4.unk88) {
                self->ext.et_801291C4.unk88 = 4;
                if (var_s3) {
                    self->ext.et_801291C4.unk88 = 2;
                }
                arctan = ratan2(-yOffset, xOffset);
                angle = arctan - temp_s2;
                if (angle > 0x800) {
                    angle = 0x1000 - angle;
                }
                if (angle < -0x800) {
                    angle = 0x1000 + angle;
                }
                if (!var_s3) {
                    angle /= 4;
                } else {
                    angle /= 2;
                }
                self->ext.et_801291C4.unk86 = angle;
            }
            temp_s2 += self->ext.et_801291C4.unk86;
            if (!var_s3) {
                temp_s2 += 0x180 - ((rand() & 3) << 8);
            }
            temp_s2 &= 0xFFF;
            temp_s3 = prim->next;
            if (temp_s3 == NULL) {
                self->step++;
                return;
            }
            LOW(temp_s3->x0) = LOW(prim->x2);
            LOW(temp_s3->x1) = LOW(prim->x3);
            self->ext.et_801291C4.unk84 = temp_s2;
            self->ext.et_801291C4.prim2 = temp_s3;
            xOffset = (rcos(temp_s2) * 0xC) >> 0xC;
            yOffset = -((rsin(temp_s2) * 0xC) >> 0xC);
            temp_s3->x2 = temp_s3->x0 + xOffset;
            temp_s3->y2 = temp_s3->y0 + yOffset;
            angle = temp_s2 - 0x400;
            var_s8 = 0x10 - (self->params * 4);
            xOffset = (var_s8 * rcos(angle)) >> 0xC;
            yOffset = -((var_s8 * rsin(angle)) >> 0xC);
            temp_s3->x3 = temp_s3->x2 + xOffset;
            temp_s3->y3 = temp_s3->y2 + yOffset;
            temp_s3->drawMode = DRAW_COLORS | DRAW_UNK02;
            self->ext.et_801291C4.unk88--;
        }
        break;
    case 2:
        if (!self->step_s) {
            prim = self->ext.et_801291C4.prim1;
            while (prim != NULL) {
                prim->clut = PAL_FILL_WHITE;
                prim = prim->next;
            }
            self->step_s++;
            return;
        }
        prim = self->ext.et_801291C4.prim1;
        while (prim != NULL) {
            prim->clut = PAL_UNK_194;
            prim->r0 = prim->g0 = prim->b0 = 0x60;
            LOW(prim->r1) = LOW(prim->r0);
            LOW(prim->r2) = LOW(prim->r0);
            LOW(prim->r3) = LOW(prim->r0);
            prim = prim->next;
        }
        self->step_s = 0;
        self->step++;
        break;
    case 3:
        var_s8 = 1;
        prim = self->ext.et_801291C4.prim1;
        while (prim != NULL) {
            var_s8 &= !BO6_PrimDecreaseBrightness(prim, 4);
            prim = prim->next;
        }
        if (var_s8) {
            prim = self->ext.et_801291C4.prim1;
            while (prim != NULL) {
                prim->drawMode = DRAW_HIDE;
                prim = prim->next;
            }
            DestroyEntity(self);
        }
        break;
    }
}

void RicEntityCrashVibhuti(Entity* self) {
    FakePrim* prim;
    s32 angle;
    s32 magnitude;
    s32 i;
    s32 facing;

    switch (self->step) {
    case 0:
        self->primIndex = g_api.AllocPrimitives(PRIM_TILE, 9);
        if (self->primIndex == -1) {
            DestroyEntity(self);
            g_Ric.unk4E = 1;
            return;
        }
        self->flags =
            FLAG_UNK_10000000 | FLAG_HAS_PRIMS;
        prim = (FakePrim*)&g_PrimBuf[self->primIndex];
        for (i = 0; i < 9; i++) {
            prim->r0 = prim->g0 = prim->b0 = 0xFF;
            prim->w = prim->h = 1;
            prim->priority = RIC.zPriority + 8;
            prim->drawMode = DRAW_HIDE | DRAW_UNK02;
            prim = prim->next;
        }
        self->step++;
        break;
    case 1:
        // Weird fake stuff to load unk7E a second time
        self->ext.vibhutiCrash.unk7E++;
        if (!(self->ext.vibhutiCrash.unk7E % 2) &&
            self->ext.vibhutiCrash.timer < 8) {
            self->ext.vibhutiCrash.timer++;
            self->ext.vibhutiCrash.unk80++;
            if (self->ext.vibhutiCrash.unk80 >= 0x30) {
                self->step++;
            }
            prim = (FakePrim*)&g_PrimBuf[self->primIndex];
            for (i = 0; i < 9; i++) {
                if (prim->drawMode & DRAW_HIDE) {
                    break;
                }
                prim = prim->next;
            }
            prim->posX.val = RIC.posX.val;
            prim->posY.val = RIC.posY.val - FIX(24);
            angle = rand() % 0x200 + 0x300;
            magnitude = (rand() % 24) + 0x20;
            prim->velocityX.val = (rcos(angle) * magnitude);
            prim->velocityY.val = -(rsin(angle) * magnitude);
            prim->drawMode &= ~DRAW_HIDE;
            prim->delay = 0x10;
        }
        // fallthrough
    case 2:

        prim = (FakePrim*)&g_PrimBuf[self->primIndex];
        for (i = 0; i < 9; i++) {
            if (!(prim->drawMode & DRAW_HIDE)) {
                if (!--prim->delay) {
                    prim->drawMode |= DRAW_HIDE;
                    self->ext.vibhutiCrash.timer--;
                    self->ext.vibhutiCrash.x = prim->posX.val;
                    self->ext.vibhutiCrash.y = prim->posY.val;
                    if (prim->velocityX.val > 0) {
                        facing = false;
                    } else {
                        facing = true;
                    }
                    self->ext.vibhutiCrash.facing = facing;
                    RicCreateEntFactoryFromEntity(
                        self, BP_VITHUBI_CRASH_CLOUD, 0);
                } else {
                    prim->posX.val += prim->velocityX.val;
                    prim->posY.val += prim->velocityY.val;
                    prim->velocityY.val += FIX(0.25);
                    prim->x0 = prim->posX.i.hi;
                    prim->y0 = prim->posY.i.hi;
                }
            }
            prim = prim->next;
        }
        if (self->step == 2 && !self->ext.vibhutiCrash.timer) {
            self->step++;
        }
        break;
    case 3:
        g_Ric.unk4E = 1;
        g_Ric.unk4E = 1;
        DestroyEntity(self);
        break;
    }
}

void func_us_801C8590(Entity* entity) {
    switch (entity->step) {
    case 0:
        entity->flags = FLAG_UNK_10000000;

        entity->hitboxWidth = 4;
        entity->hitboxHeight = 4;
        entity->step++;
        break;
    case 1:
        entity->ext.subweapon.timer++;
        if (entity->ext.subweapon.timer > 3) {
            DestroyEntity(entity);
        }
        break;
    }
}

void RicEntityCrashReboundStoneExplosion(Entity* self) {
    Primitive* prim;
    s32 i;

    switch (self->step) {
    case 0:
        self->primIndex = g_api.AllocPrimitives(PRIM_G4, 0x10);
        if (self->primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->flags = FLAG_UNK_10000000 | FLAG_HAS_PRIMS;
        prim = &g_PrimBuf[self->primIndex];
        for (i = 0; i < 0x10; i++) {
            prim->priority = 0xC2;
            prim->drawMode = DRAW_DITHERING | DRAW_TPAGE2 | DRAW_TPAGE |
                             DRAW_COLORS | DRAW_TRANSP;
            prim->x0 = prim->x1 = 0x80;
            prim->y0 = prim->y1 = 0;
            prim = prim->next;
        }
        self->ext.reboundStoneCrashExplosion.unk7C = 0x40;
        self->ext.reboundStoneCrashExplosion.unk7E = 0;
        self->ext.reboundStoneCrashExplosion.unk84 = 0;
        self->ext.reboundStoneCrashExplosion.unk80 = 0x10;
        self->ext.reboundStoneCrashExplosion.unk82 = 8;
        self->step++;
        break;
    case 1:
        self->ext.reboundStoneCrashExplosion.unk84 += 0x20;
        if (self->ext.reboundStoneCrashExplosion.unk84 > 0x120) {
            self->posX.val = FIX(128.0);
            self->posY.val = FIX(128.0);
            self->hitboxWidth = 0x80;
            self->hitboxHeight = 0x80;
            self->step++;
        }
        break;
    case 2:
        self->ext.reboundStoneCrashExplosion.unk86++;
        if (self->ext.reboundStoneCrashExplosion.unk86 == 5) {
            self->ext.reboundStoneCrashExplosion.unk80 = -0x18;
        } else if (self->ext.reboundStoneCrashExplosion.unk86 >= 0xF) {
            self->ext.reboundStoneCrashExplosion.unk82 = -0x18;
            self->step++;
        }
        break;
    case 3:
        break;
    case 4:
        DestroyEntity(self);
        return;
    }
    self->ext.reboundStoneCrashExplosion.unk7C +=
        self->ext.reboundStoneCrashExplosion.unk80;
    if (self->ext.reboundStoneCrashExplosion.unk7C > 0xFF) {
        self->ext.reboundStoneCrashExplosion.unk7C = 0xFF;
        self->ext.reboundStoneCrashExplosion.unk80 = 0;
    } else if (self->ext.reboundStoneCrashExplosion.unk7C < 0) {
        self->ext.reboundStoneCrashExplosion.unk7C =
            self->ext.reboundStoneCrashExplosion.unk80 = 0;
    }
    self->ext.reboundStoneCrashExplosion.unk7E +=
        self->ext.reboundStoneCrashExplosion.unk82;
    if (self->ext.reboundStoneCrashExplosion.unk7E > 0xFF) {
        self->ext.reboundStoneCrashExplosion.unk7E = 0xFF;
        self->ext.reboundStoneCrashExplosion.unk82 = 0;
    } else if (self->ext.reboundStoneCrashExplosion.unk7E < 0) {
        self->ext.reboundStoneCrashExplosion.unk7E =
            self->ext.reboundStoneCrashExplosion.unk82 = 0;
        self->step += 1;
    }
    prim = &g_PrimBuf[self->primIndex];
    for (i = 0; i < 16; i++) {
        prim->b0 = prim->b1 = self->ext.reboundStoneCrashExplosion.unk7C;
        prim->b2 = prim->b3 = self->ext.reboundStoneCrashExplosion.unk7E;
        prim->r0 = prim->r1 = prim->g0 = prim->g1 =
            self->ext.reboundStoneCrashExplosion.unk7C;
        prim->r2 = prim->r3 = prim->g2 = prim->g3 =
            self->ext.reboundStoneCrashExplosion.unk7E;
        if (self->step < 2) {
            prim->x2 =
                ((rcos(i << 7) * self->ext.reboundStoneCrashExplosion.unk84) >>
                 0xC) +
                0x80;
            prim->x3 = ((rcos((i + 1) << 7) *
                         self->ext.reboundStoneCrashExplosion.unk84) >>
                        0xC) +
                       0x80;
            prim->y2 =
                ((rsin(i << 7) * self->ext.reboundStoneCrashExplosion.unk84) >>
                 0xC);
            prim->y3 = ((rsin((i + 1) << 7) *
                         self->ext.reboundStoneCrashExplosion.unk84) >>
                        0xC);
        }
        prim = prim->next;
    }
}

void RicEntityCrashReboundStone(Entity* entity) {
    switch (entity->step) {
    case 0:
        entity->flags = FLAG_UNK_10000000;
        entity->step++;
        entity->ext.timer.t = 0x14;
        // fallthrough
    case 1:
        if (--entity->ext.timer.t) {
            break;
        }
    case 3:
    case 5:
        RicCreateEntFactoryFromEntity(entity, BP_57, 0);
        entity->step++;
    case 2:
    case 4:
    case 6:
        entity->ext.timer.t++;
        if (entity->ext.timer.t > 10) {
            entity->ext.timer.t = 0;
            entity->posX.val = FIX(128.0);
            entity->posY.val = 0;
            RicCreateEntFactoryFromEntity(entity, FACTORY(BP_EMBERS, 1), 0);
            entity->step++;
        }
        break;
    case 7:
        entity->ext.timer.t++;
        if (entity->ext.timer.t > 15) {
            DestroyEntity(entity);
            g_Ric.unk4E = 1;
            RicCreateEntFactoryFromEntity(
                entity, BP_CRASH_REBOUND_STONE_EXPLOSION, 0);
        }
        break;
    }
}

void RicEntityCrashBibleBeam(Entity* self) {
    Primitive* prim;
    s32 i;
    s32 var_s3;
    s32 psp_s3;
    s32 halfwidth;
    s32 hitboxOffX;
    s16 var_s7;

    switch (self->step) {
    case 0:
        InitializeEntity(D_us_801804CC);
        self->ext.bibleBeam.subweaponId = 0x11;
        D_us_801D10E8 = self->hitboxState;
        self->hitboxState = 0;
        self->primIndex = g_api.AllocPrimitives(PRIM_G4, BIBLE_PAGE_COUNT);
        if (self->primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->flags |= FLAG_UNK_10000000 | FLAG_HAS_PRIMS;
        if (self->facingLeft) {
            self->ext.bibleBeam.unk7C = -16;
            self->ext.bibleBeam.unk7E = -2;
        } else {
            self->ext.bibleBeam.unk7C = 16;
            self->ext.bibleBeam.unk7E = 2;
        }
        prim = &g_PrimBuf[self->primIndex];
        for (i = 0; i < BIBLE_PAGE_COUNT; i++) {
            var_s3 = i + 2;
            if (var_s3 >= BIBLE_PAGE_COUNT) {
                var_s3 -= 6;
            }
            prim->x0 = prim->x1 = D_us_801D10D0[i].x;
            prim->y0 = prim->y1 = D_us_801D10D0[i].y;
            prim->x2 = prim->x3 = D_us_801D10D0[var_s3].x;
            prim->y2 = prim->y3 = D_us_801D10D0[var_s3].y;
            prim->priority = 0xC2;
            prim->drawMode = DRAW_DITHERING | DRAW_TPAGE2 | DRAW_TPAGE |
                             DRAW_COLORS | DRAW_TRANSP;
            prim = prim->next;
        }
        self->step = 1; // InitializeEntity already counted one step
        break;
    case 1:
        self->ext.bibleBeam.unk80++;
        if (self->ext.bibleBeam.unk80 >= 0x3C) {
            g_api.PlaySfx(SFX_WEAPON_APPEAR);
            g_api.PlaySfx(SFX_TELEPORT_BANG_A);
            self->hitboxState = D_us_801D10E8;
            self->step++;
        }
        break;
    case 2:
        self->ext.bibleBeam.unk80++;
        self->ext.bibleBeam.unk7E += self->ext.bibleBeam.unk7C;
        var_s3 = D_us_801D10D0[1].x + self->ext.bibleBeam.unk7E;
        if (var_s3 < -0x50 || var_s3 > 0x150) {
            self->step++;
        }
        break;
    case 3:
        self->ext.bibleBeam.unk80++;
        if (self->ext.bibleBeam.unk80 >= 0x78) {
            DestroyEntity(self);
            return;
        }
        break;
    }
    prim = &g_PrimBuf[self->primIndex];
    var_s7 = 0; // @bug: this is never unused
    for (i = 0; i < BIBLE_PAGE_COUNT; i++) {
        var_s3 = i + 2;
        if (var_s3 >= BIBLE_PAGE_COUNT) {
            var_s3 -= 6;
        }
        psp_s3 = i * 256;
        prim->r0 = prim->r1 =
            abs((rsin((self->ext.bibleBeam.unk80 * 20) + psp_s3) * 96) >> 0xc);
        prim->g0 = prim->g1 =
            abs((rsin((self->ext.bibleBeam.unk80 * 15) + psp_s3) * 96) >> 0xc);
        prim->b0 = prim->b1 =
            abs((rsin((self->ext.bibleBeam.unk80 * 10) + psp_s3) * 96) >> 0xc);
        psp_s3 = var_s3 * 256;
        prim->r2 = prim->r3 =
            abs((rsin((self->ext.bibleBeam.unk80 * 15) + psp_s3) * 96) >> 0xc);
        prim->g2 = prim->g3 =
            abs((rsin((self->ext.bibleBeam.unk80 * 10) + psp_s3) * 96) >> 0xc);
        prim->b2 = prim->b3 =
            abs((rsin((self->ext.bibleBeam.unk80 * 20) + psp_s3) * 96) >> 0xc);
        prim->x1 = D_us_801D10D0[i].x;
        prim->y0 = prim->y1 = D_us_801D10D0[i].y;
        prim->x3 = D_us_801D10D0[var_s3].x;
        prim->y2 = prim->y3 = D_us_801D10D0[var_s3].y;
        prim->x0 = D_us_801D10D0[i].x + self->ext.bibleBeam.unk7E;
        prim->x2 = D_us_801D10D0[var_s3].x + self->ext.bibleBeam.unk7E;
        if (var_s7 < abs(D_us_801D10D0[i].y)) {
            var_s7 = abs(D_us_801D10D0[i].y);
        }
        prim = prim->next;
    }
    self->hitboxOffX = self->facingLeft ? -(self->ext.bibleBeam.unk7E / 2)
                                        : (self->ext.bibleBeam.unk7E / 2);
    self->hitboxWidth = abs(self->hitboxOffX);
    self->hitboxHeight = var_s7 - self->posY.i.hi;
}

void RicEntityCrashBible(Entity* self) {
    Primitive* prim;
    s32 i;
    s32 psp_s2;
    s16 psp_s4;
    s16 psp_s3;
    s32 psp_s6;
    s32 psp_s5;
    s32 sp3C;
    s32 sp48;
    s32 sp40;
    long sp44;
    long sp4C;
    s32 psp_s8;
    s32 psp_s7;

    switch (self->step) {
    case 0:
        self->primIndex = g_api.AllocPrimitives(PRIM_GT4, 1 + BIBLE_PAGE_COUNT);
        if (self->primIndex == -1) {
            DestroyEntity(self);
            g_Ric.unk4E = 1;
            return;
        }
        self->flags =
            FLAG_UNK_10000000 | FLAG_HAS_PRIMS;
        self->posX.val = RIC.posX.val;
        self->posY.val = RIC.posY.val;
        self->velocityY = FIX(-4);
        if (self->facingLeft) {
            self->velocityX = FIX(12);
            self->ext.et_8016E9E4.unk88 = FIX(1.5);
            self->ext.et_8016E9E4.unk7E = -0x200;
        } else {
            self->velocityX = FIX(-12);
            self->ext.et_8016E9E4.unk88 = FIX(-1.5);
            self->ext.et_8016E9E4.unk7E = 0x200;
        }
        prim = &g_PrimBuf[self->primIndex];
        prim->tpage = 0x1E;
        prim->clut = 0x17F;
        prim->u0 = prim->u2 = 0x98;
        prim->v0 = prim->v1 = 0xD8;
        prim->u1 = prim->u3 = 0xA8;
        prim->v2 = prim->v3 = 0xF0;
        prim->priority = 0xC2;
        prim->drawMode = DRAW_HIDE;
        prim = prim->next;
        for (i = 0; i < BIBLE_PAGE_COUNT; i++) {
            prim->tpage = 0x1C;
            prim->clut = PAL_UNK_1AE;
            prim->u0 = prim->u2 = 0x20;
            prim->u1 = prim->u3 = 0x30;
            prim->v0 = prim->v1 = 0;
            prim->v2 = prim->v3 = 0x10;
            prim->priority = 0xC1;
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
        self->ext.et_8016E9E4.unk84 = 0x40;
        D_us_801D10EC = 0x60;
        self->step++;
        break;
    case 1:
        prim = &g_PrimBuf[self->primIndex];
        prim->drawMode &= ~DRAW_HIDE;
        self->step++;
    case 2:
        self->posX.val += self->velocityX;
        self->posY.val += self->velocityY;
        self->velocityX -= self->ext.et_8016E9E4.unk88;
        self->velocityY -= FIX(0.5);
        self->ext.et_8016E9E4.unk82++;
        if (self->ext.et_8016E9E4.unk82 >= 8) {
            self->ext.et_8016E9E4.unk82 = 0;
            self->step++;
        }
        break;
    case 3:
        self->ext.et_8016E9E4.unk82++;
        if (self->ext.et_8016E9E4.unk82 > 5) {
            prim = &g_PrimBuf[self->primIndex];
            prim->clut = PAL_UNK_19F;
            prim->drawMode |=
                DRAW_TPAGE2 | DRAW_TPAGE | DRAW_COLORS | DRAW_TRANSP;
            prim = prim->next;
            for (i = 0; i < BIBLE_PAGE_COUNT; i++) {
                prim->drawMode &= ~DRAW_HIDE;
                prim = prim->next;
            }
            self->ext.et_8016E9E4.unk82 = 0;
            self->step++;
        }
        break;
    case 4:
        self->ext.et_8016E9E4.unk84 -= 4;
        if (self->ext.et_8016E9E4.unk84 <= 0) {
            self->step++;
        }
        break;
    case 5:
        prim = &g_PrimBuf[self->primIndex];
        prim->drawMode |= DRAW_HIDE;
        self->step++;
    case 6:
        self->ext.et_8016E9E4.unk7C += 0x80;
        self->ext.et_8016E9E4.unk7C &= 0xFFF;
        self->ext.et_8016E9E4.unk80 += 4;
        if (self->ext.et_8016E9E4.unk80 >= 0x30) {
            self->step++;
        }
        break;
    case 7:
        self->ext.et_8016E9E4.unk7C += 0x80;
        self->ext.et_8016E9E4.unk7C &= 0xFFF;
        self->ext.et_8016E9E4.unk82++;
        if (self->ext.et_8016E9E4.unk82 >= 0x1E) {
            // create bible page beam
            RicCreateEntFactoryFromEntity(self, BP_CRASH_BIBLE_BEAM, 0);
            self->ext.et_8016E9E4.unk82 = 0;
            self->step++;
        }
        break;
    case 8:
        self->ext.et_8016E9E4.unk7C += 0x80;
        self->ext.et_8016E9E4.unk7C &= 0xFFF;
        self->ext.et_8016E9E4.unk82++;
        if (self->ext.et_8016E9E4.unk82 >= 0x60) {
            g_Ric.unk4E = 1;
            self->step++;
        }
        break;
    case 9:
        self->ext.et_8016E9E4.unk7C += 0x80;
        self->ext.et_8016E9E4.unk7C &= 0xFFF;
        self->ext.et_8016E9E4.unk80 -= 2;
        if (self->ext.et_8016E9E4.unk80 <= 0) {
            DestroyEntity(self);
            return;
        }
        break;
    }
    // FAKE, needed for reg match
    if (self->ext.et_8016E9E4.unk7C == 0x100 ||
        self->ext.et_8016E9E4.unk7C == 0x500 ||
        self->ext.et_8016E9E4.unk7C == 0x900 ||
        self->ext.et_8016E9E4.unk7C == 0xD00) {
        if (self->step < 9) {
            g_api.PlaySfxVolPan(SFX_ARROW_SHOT_A, D_us_801D10EC, 0);
            if (self->step >= 5) {
                D_us_801D10EC -= 4;
            }
            if (D_us_801D10EC < 0) {
                D_us_801D10EC = 0;
            }
        }
    }

    prim = &g_PrimBuf[self->primIndex];
    psp_s4 = self->posX.i.hi;
    psp_s3 = self->posY.i.hi;
    prim->x0 = prim->x2 = psp_s4 - 8;
    prim->x1 = prim->x3 = psp_s4 + 8;
    prim->y0 = prim->y1 = psp_s3 - 12;
    prim->y2 = prim->y3 = psp_s3 + 12;
    prim->r0 = prim->g0 = prim->b0 = prim->r1 = prim->g1 = prim->b1 = prim->r2 =
        prim->g2 = prim->b2 = prim->r3 = prim->g3 = prim->b3 =
            self->ext.et_8016E9E4.unk84;
    prim = prim->next;

    sp44 = rsin(self->ext.et_8016E9E4.unk7E);
    sp4C = rcos(self->ext.et_8016E9E4.unk7E);
    for (i = 0; i < BIBLE_PAGE_COUNT; i++) {
        sp3C = 0;
        sp48 = (rsin(self->ext.et_8016E9E4.unk7C + D_us_80182A1C[i]) *
                self->ext.et_8016E9E4.unk80) >>
               0xC;
        sp40 = (rcos(self->ext.et_8016E9E4.unk7C + D_us_80182A1C[i]) *
                self->ext.et_8016E9E4.unk80) >>
               0xC;
        psp_s6 = (sp4C * sp3C + sp44 * sp40) >> 0xC;
        psp_s2 = (sp4C * sp40 - sp44 * sp3C) >> 0xC;
        psp_s5 = sp48;
        psp_s2 += 0x200;
        psp_s6 = (psp_s6 << 9) / psp_s2;
        psp_s5 = (psp_s5 << 9) / psp_s2;
        psp_s4 = self->posX.i.hi + psp_s6;
        psp_s3 = self->posY.i.hi + psp_s5;
        D_us_801D10D0[i].x = psp_s4;
        D_us_801D10D0[i].y = psp_s3;
        psp_s8 = 0x1000 / psp_s2;
        psp_s7 = 0x1000 / psp_s2;
        prim->x0 = prim->x2 = psp_s4 - psp_s8;
        prim->x1 = prim->x3 = psp_s4 + psp_s8;
        prim->y0 = prim->y1 = psp_s3 - psp_s7;
        prim->y2 = prim->y3 = psp_s3 + psp_s7;
        prim = prim->next;
    }
}

void func_us_801C9DE8(Entity* self) {
    const int PrimCount = 16;
#if defined(VERSION_PSP)
    const int MaxHeight = 248;
    const int HalfHeight = 128;
#else
    const int MaxHeight = 240;
    const int HalfHeight = 120;
#endif
    Primitive* prim;
    s32 i;
    s32 tpage;
    s32 angleX;
    s32 angleY;
    s16 startMod;
    s16 endMod;
    s16 sx;
    s16 ex;
    s16 sy;
    s16 ey;

    switch (self->step) {
    case 0:
        self->primIndex = g_api.AllocPrimitives(PRIM_GT4, PrimCount);
        if (self->primIndex == -1) {
            DestroyEntity(self);
#if !defined(VERSION_PSP)
            g_Ric.unk4E = 1;
#endif
            return;
        }
        self->flags = FLAG_UNK_10000000 | FLAG_HAS_PRIMS;
        prim = &g_PrimBuf[self->primIndex];
        for (i = 0; i < 16; i++) {
            prim->priority = 0xC2;
            prim->drawMode = DRAW_HIDE;
            prim = prim->next;
        }
        self->step++;
        break;
    case 1:
        prim = &g_PrimBuf[self->primIndex];
        for (i = 0; i < 16; i++) {
            prim->drawMode &= ~DRAW_HIDE;
            prim = prim->next;
        }
        self->step++;
    case 2:
        self->ext.factory.unk7C++;
        if (self->ext.factory.unk7C >= 0x18) {
            self->step++;
        }
        break;
    case 3:
#if !defined(VERSION_PSP)
        g_Ric.unk4E = 1;
#endif
        DestroyEntity(self);
        return;
    }
    if (!self->ext.factory.unk7C) {
        return;
    }
    if (
#if !defined(VERSION_PSP)
        g_CurrentBuffer == g_GpuBuffers
#else
        0
#endif
    ) {
        tpage = 0x104;
    } else {
        tpage = 0x100;
    }
    prim = &g_PrimBuf[self->primIndex];
    for (i = 0; i < PrimCount; i++) {
        angleX = rsin(i << 8);
        angleY = rcos(i << 8);
        startMod = self->ext.factory.unk7C * 8;
        endMod = 0;
        if (self->ext.factory.unk7C >= 4) {
            endMod = (self->ext.factory.unk7C - 4) * 8;
        }
        sx = ((angleY * startMod) >> 0xC) + 0x80;
        ex = ((angleY * endMod) >> 0xC) + 0x80;
        sy = ((angleX * startMod) >> 0xC) + HalfHeight;
        ey = ((angleX * endMod) >> 0xC) + HalfHeight;
        prim->x0 = sx = CLAMP(sx, 0, 0xFF);
        prim->x2 = ex = CLAMP(ex, 0, 0xFF);
        prim->y0 = sy = CLAMP(sy, 0, MaxHeight);
        prim->y2 = ey = CLAMP(ey, 0, MaxHeight);
        prim->u0 = 0xFF - prim->x0;
        prim->u2 = 0xFF - prim->x2;
        prim->v0 = MaxHeight - prim->y0;
        prim->v2 = MaxHeight - prim->y2;

        angleX = rsin((i + 1) << 8);
        angleY = rcos((i + 1) << 8);
        sx = ((angleY * startMod) >> 0xC) + 0x80;
        ex = ((angleY * endMod) >> 0xC) + 0x80;
        sy = ((angleX * startMod) >> 0xC) + HalfHeight;
        ey = ((angleX * endMod) >> 0xC) + HalfHeight;
        prim->x1 = sx = CLAMP(sx, 0, 0xFF);
        prim->x3 = ex = CLAMP(ex, 0, 0xFF);
        prim->y1 = sy = CLAMP(sy, 0, MaxHeight);
        prim->y3 = ey = CLAMP(ey, 0, MaxHeight);
        prim->u1 = 0xFF - prim->x1;
        prim->u3 = 0xFF - prim->x3;
        prim->v1 = MaxHeight - prim->y1;
        prim->v3 = MaxHeight - prim->y3;
#if defined(VERSION_PSP)
        prim->u0 = prim->u0 = CLAMP(prim->u0, 1, 0xFE);
        prim->u2 = prim->u2 = CLAMP(prim->u2, 1, 0xFE);
        prim->u1 = prim->u1 = CLAMP(prim->u1, 1, 0xFE);
        prim->u3 = prim->u3 = CLAMP(prim->u3, 1, 0xFE);
#endif

        prim->tpage = tpage;
        prim = prim->next;
    }
}

s16 GetAguneaLightningAngle(s16* arg0, s16 arg1, s16 arg2, s16* arg3) {
    arg1 += rand() % 256 - 0x80;
    *arg3 = (rand() % 48) + 0x10;
    arg0[0] = arg0[1];
    arg0[2] = arg0[3];
    if (arg2) {
        arg0[1] += (rcos(arg1) * *arg3) >> 0xC;
        arg0[3] += (rsin(arg1) * *arg3) >> 0xC;
        if (arg2 % 2) {
            return GetAguneaLightningAngle(arg0, arg1 - 0x140, arg2 / 2, arg3);
        } else {
            rand();
            rand();
            return GetAguneaLightningAngle(
                arg0, arg1 + 0x140, (arg2 - 1) / 2, arg3);
        }
    }
    return arg1;
}

void AguneaShuffleParams(s32 bufSize, s32* buf) {
    s32 i, idx, swapTemp;

    for (i = bufSize - 1; i > 0; i--) {
        // Pick random index
        idx = rand() % bufSize;

        // Hold swap value temporarily
        swapTemp = buf[i];
        buf[i] = buf[idx];
        buf[idx] = swapTemp;
    }
}

void RicEntityAguneaLightning(Entity* self) {
    s16 sp20;
    s16 angle;
    s16 sp18;
    s32 randomSeed;
    s16 sp10[4];
    s16 xCoord;
    s16 yCoord;
    s16 psp_s6;
    s16 psp_s5;
    s32 psp_s4;
    s32 psp_s3;
    s32 i;
    Primitive* prim;

    switch (self->step) {
    case 0:
        self->primIndex = g_api.AllocPrimitives(PRIM_GT4, 0xF);
        if (self->primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->flags =
            FLAG_UNK_10000000 | FLAG_HAS_PRIMS | FLAG_POS_CAMERA_LOCKED;
        prim = &g_PrimBuf[self->primIndex];
        for (i = 0; i < 15; i++) {
            prim->tpage = 0x1A;
            prim->clut = PAL_UNK_194;
            xCoord = (rand() % 5) * 0x10;
            prim->u0 = prim->u2 = xCoord + 0x90;
            prim->u1 = prim->u3 = xCoord + 0xB0;
            if (rand() % 2) {
                prim->v0 = prim->v1 = 0xD0;
                prim->v2 = prim->v3 = 0xE0;
            } else {
                prim->v0 = prim->v1 = 0xE0;
                prim->v2 = prim->v3 = 0xD0;
            }
            prim->r0 = prim->g0 = prim->b0 = prim->r1 = prim->g1 = prim->b1 =
                prim->r2 = prim->g2 = prim->b2 = prim->r3 = prim->g3 =
                    prim->b3 = 0x80;
            prim->priority = 0xC1;
            prim->drawMode = DRAW_HIDE | DRAW_COLORS | DRAW_UNK02;
            prim = prim->next;
        }
        prim = &g_PrimBuf[self->primIndex];
        sp20 = ((self->params & 0xFF00) >> 8) * 0x200;
        sp20 += rand() % 0x200 - 0x100;
        randomSeed = rand() & PSP_RANDMASK;
        for (i = 0; i < 15; i++) {
            srand(randomSeed);
            sp10[1] = self->posX.i.hi;
            sp10[3] = self->posY.i.hi;
            angle = GetAguneaLightningAngle(sp10, sp20, i, &sp18);
            xCoord = sp10[0];
            yCoord = sp10[2];
            psp_s6 = !i ? 2 : 8;
            psp_s5 = (i < 7) ? 8 : 2;

            psp_s4 = rcos(angle);
            psp_s3 = rsin(angle);
            prim->x0 = xCoord + (-(psp_s3 * -psp_s6) >> 0xC);
            prim->y0 = yCoord + ((psp_s4 * -psp_s6) >> 0xC);
            prim->x1 = xCoord + ((psp_s4 * sp18 - (psp_s3 * -psp_s5)) >> 0xC);
            prim->y1 = yCoord + ((psp_s3 * sp18 + (psp_s4 * -psp_s5)) >> 0xC);
            prim->x2 = xCoord + (-(psp_s3 * psp_s6) >> 0xC);
            prim->y2 = yCoord + ((psp_s4 * psp_s6) >> 0xC);
            prim->x3 = xCoord + ((psp_s4 * sp18 - (psp_s3 * psp_s5)) >> 0xC);
            prim->y3 = yCoord + ((psp_s3 * sp18 + (psp_s4 * psp_s5)) >> 0xC);
            prim = prim->next;
        }
        self->ext.et_8017091C.unk7E = 1;
        self->step++;
        break;
    case 1:
        prim = &g_PrimBuf[self->primIndex];
        for (i = 0; i < 15; i++) {
            prim->drawMode &= ~DRAW_HIDE;
            prim = prim->next;
        }
        self->step++;
    case 2:
        self->ext.et_8017091C.unk7C++;
        if (self->ext.et_8017091C.unk7C > 4) {
            prim = &g_PrimBuf[self->primIndex];
            for (i = 0; i < 15; i++) {
                prim->v0 = prim->v1 = prim->v0 - 0x10;
                prim->v2 = prim->v3 = prim->v2 - 0x10;
                prim->clut = PAL_FILL_WHITE;
                prim->r0 = prim->g0 = prim->b0 = prim->r1 = prim->g1 =
                    prim->b1 = prim->r2 = prim->g2 = prim->b2 = prim->r3 =
                        prim->g3 = prim->b3 = 0xFF;
                prim = prim->next;
            }
            self->ext.et_8017091C.unk7C = 0;
            self->step++;
        }
        break;
    case 3:
    case 5:
        prim = &g_PrimBuf[self->primIndex];
        for (i = 0; i < 15; i++) {
            prim->clut = PAL_UNK_194;
            prim = prim->next;
        }
        self->step++;
        break;
    case 4:
        prim = &g_PrimBuf[self->primIndex];
        for (i = 0; i < 15; i++) {
            prim->clut = PAL_FILL_WHITE;
            prim = prim->next;
        }
        self->step++;
        break;
    case 6:
        prim = &g_PrimBuf[self->primIndex];
        for (i = 0; i < 15; i++) {
            prim->r0 = prim->g0 = prim->b0 = prim->r1 = prim->g1 = prim->b1 =
                prim->r2 = prim->g2 = prim->b2 = prim->r3 = prim->g3 =
                    prim->b3 = 0x60 - (self->ext.et_8017091C.unk7C * 4);
            prim = prim->next;
        }
        self->ext.et_8017091C.unk7C++;
        if (self->ext.et_8017091C.unk7C > 15) {
            DestroyEntity(self);
            return;
        }
        break;
    }
}

void RicEntityAguneaCircle(Entity* self) {
    Primitive* prim;
    s16 rand_angle;
    s16 xCoord;
    s16 yCoord;
    s32 quarterSelfY;
    s32 i;
    s16 psp_s4;

    switch (self->step) {
    case 0:
        self->primIndex = g_api.AllocPrimitives(PRIM_GT4, 4);
        if (self->primIndex == -1) {
            DestroyEntity(self);
            g_Ric.unk4E = 1;
            return;
        }
        InitializeEntity(D_us_801804C0);
        self->ext.aguneaCrash.subweaponId = 0x1A;
        prim = &g_PrimBuf[self->primIndex];
        self->posY.i.hi -= 0x20;
        self->flags |= FLAG_UNK_10000000 | FLAG_HAS_PRIMS;
        quarterSelfY = self->posY.i.hi / 4;
        xCoord = self->posX.i.hi;
        yCoord = 0;
        for (i = 0; i < 4; i++) {
            prim->tpage = 0x1A;
            prim->clut = PAL_UNK_194;
            prim->x0 = xCoord - 8;
            prim->x2 = xCoord + 8;
            prim->y0 = prim->y2 = yCoord;
            if (i < 3) {
                rand_angle = rand() % 0x100 + 0x380;
                xCoord += ((rcos(rand_angle) * quarterSelfY) >> 0xC);
                yCoord += ((rsin(rand_angle) * quarterSelfY) >> 0xC);
                prim->x1 = xCoord - 8;
                prim->x3 = xCoord + 8;
            } else {
                xCoord = self->posX.i.hi;
                yCoord = self->posY.i.hi;
                prim->x1 = xCoord - 4;
                prim->x3 = xCoord + 4;
            }
            prim->y1 = prim->y3 = yCoord;
            psp_s4 = (rand() % 6) * 0x10;
            prim->u0 = prim->u2 = psp_s4 + 0x90;
            prim->u1 = prim->u3 = psp_s4 + 0xA0;
            if (rand() % 2) {
                prim->v0 = prim->v1 = 0xD0;
                prim->v2 = prim->v3 = 0xE0;
            } else {
                prim->v0 = prim->v1 = 0xE0;
                prim->v2 = prim->v3 = 0xD0;
            }
            prim->priority = 0xC2;
            prim->drawMode = DRAW_HIDE | DRAW_UNK02;
            prim = prim->next;
        }
        self->step = 1; // InitializeEntity already counted one step
        break;
    case 1:
        prim = &g_PrimBuf[self->primIndex];
        for (i = 0; i < self->ext.aguneaCrash.unk7C; i++) {
            prim = prim->next;
        }
        prim->drawMode &= ~DRAW_HIDE;
        self->ext.aguneaCrash.unk7C++;
        if (self->ext.aguneaCrash.unk7C >= 4) {
            self->ext.aguneaCrash.unk7C = 0;
            self->step++;
        }
        break;
    case 2:
        prim = &g_PrimBuf[self->primIndex];
        for (i = 0; i < self->ext.aguneaCrash.unk7C; i++) {
            prim = prim->next;
        }
        if (!self->ext.aguneaCrash.unk7C) {
            prim->drawMode = DRAW_TPAGE2 | DRAW_TPAGE | DRAW_COLORS | DRAW_TRANSP | DRAW_UNK02;
            prim->tpage = 0x1A;
            prim->clut = PAL_UNK_19F;
            prim->u0 = prim->u2 = 0;
            prim->u1 = prim->u3 = 0x3F;
            prim->v0 = prim->v1 = 0xC0;
            prim->v2 = prim->v3 = 0xFF;
            prim->r0 = prim->g0 = prim->r1 = prim->g1 = prim->r2 = prim->g2 =
                prim->r3 = prim->g3 = 0x7F;
            prim->b0 = prim->b1 = prim->b2 = prim->b3 = 0xFF;
            self->ext.aguneaCrash.unk7E = 1;
            self->ext.aguneaCrash.unk80 = 2;
            self->ext.aguneaCrash.unk82 = 0x64;
        } else {
            self->ext.aguneaCrash.unk80 += 2;
            prim->drawMode |= DRAW_HIDE;
        }
        self->ext.aguneaCrash.unk7C++;
        if (self->ext.aguneaCrash.unk7C > 3) {
            // think this loop has to count down since we assign to i
            for (i = 0; i < LIGHTNING_COUNT; i++) {
                D_us_801D10F0[i] = i;
            }
            AguneaShuffleParams(LIGHTNING_COUNT, &D_us_801D10F0[0]);
            self->ext.aguneaCrash.unk7C = 0;
            g_api.PlaySfx(SFX_THUNDER_B);
            self->step++;
        }
        break;
    case 3:
        RicCreateEntFactoryFromEntity(
            self,
            FACTORY(BP_CRASH_AGUNEA_THUNDER,
                    D_us_801D10F0[self->ext.aguneaCrash.unk7C]),
            0);
        self->ext.aguneaCrash.unk7C++;
        if (self->ext.aguneaCrash.unk7C > LIGHTNING_COUNT - 1) {
            self->hitboxWidth = 0x80;
            self->hitboxHeight = 0x80;
            self->ext.aguneaCrash.unk7C = 0;
            self->step++;
        }
        break;
    case 4:
        self->ext.aguneaCrash.unk7C++;
        if (self->ext.aguneaCrash.unk7C > LIGHTNING_COUNT) {
            self->step++;
        }
        break;
    case 5:
        self->ext.aguneaCrash.unk80 += 2;
        self->ext.aguneaCrash.unk82 -= 10;
        if (self->ext.aguneaCrash.unk82 <= 0) {
            self->hitboxWidth = 0;
            self->hitboxHeight = 0;
            self->step++;
        }
        break;
    case 6:
        g_Ric.unk4E = 1;
        DestroyEntity(self);
        return;
    }
    if (self->ext.aguneaCrash.unk7E) {
        prim = &g_PrimBuf[self->primIndex];
        prim->x0 = prim->x2 = self->posX.i.hi - self->ext.aguneaCrash.unk80;
        prim->x1 = prim->x3 = self->posX.i.hi + self->ext.aguneaCrash.unk80;
        prim->y0 = prim->y1 = self->posY.i.hi - self->ext.aguneaCrash.unk80;
        prim->y2 = prim->y3 = self->posY.i.hi + self->ext.aguneaCrash.unk80;
        prim->r0 = prim->g0 = prim->r1 = prim->g1 = prim->r2 = prim->g2 =
            prim->r3 = prim->g3 = (self->ext.aguneaCrash.unk82 * 0x7F) / 100;
        prim->b0 = prim->b1 = prim->b2 = prim->b3 =
            (self->ext.aguneaCrash.unk82 * 0xFF) / 100;
    }
}

void RicEntitySubwpnStopwatchCircle(Entity* self) {
    s16 temp_s0_4;
    s16 psp_s4;
    s32 sine;
    s32 cosine;
    s16 xCoord;
    s16 yCoord;
    Primitive* prim;
    s32 i;

    switch (self->step) {
    case 0:
        self->primIndex = g_api.AllocPrimitives(PRIM_GT4, 0x10);
        if (self->primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->flags = FLAG_POS_CAMERA_LOCKED | FLAG_HAS_PRIMS;
        prim = &g_PrimBuf[self->primIndex];
        for (i = 0; i < 16; i++) {
            prim->tpage = 0x1A;
            prim->clut = PAL_FILL_WHITE;
            prim->priority = self->zPriority = 0xC2;
            prim->drawMode = DRAW_DITHERING | DRAW_TPAGE2 | DRAW_TPAGE |
                             DRAW_COLORS | DRAW_TRANSP;
            psp_s4 = i * 0x100;
            prim->u0 = ((rsin(psp_s4) << 5) >> 0xC) + 0x20;
            prim->v0 = -((rcos(psp_s4) << 5) >> 0xC) + 0xDF;
            psp_s4 = (i + 1) * 0x100;
            prim->u1 = ((rsin(psp_s4) << 5) >> 0xC) + 0x20;
            prim->v1 = -((rcos(psp_s4) << 5) >> 0xC) + 0xDF;
            prim->u2 = prim->u3 = 0x20;
            prim->v2 = prim->v3 = 0xE0;
            prim->r0 = prim->r1 = prim->g0 = prim->g1 = prim->b0 = prim->b1 =
                0x40;
            prim->r2 = prim->r3 = prim->g2 = prim->g3 = 0;
            prim->b2 = prim->b3 = 0x20;
            prim = prim->next;
        }
        self->ext.et_stopwatchCircle.size = 0x20;
        self->step++;
        break;
    case 1:
        self->ext.et_stopwatchCircle.size += 0x18;
        self->ext.et_stopwatchCircle.timer++;
        if (self->ext.et_stopwatchCircle.timer > 0x1E) {
            DestroyEntity(self);
            return;
        }
    }
    xCoord = self->posX.i.hi;
    yCoord = self->posY.i.hi;
    prim = &g_PrimBuf[self->primIndex];
    for (i = 0; i < 16; i++) {
        sine = rsin(i * 0x100);
        cosine = rcos(i * 0x100);
        temp_s0_4 = self->ext.et_stopwatchCircle.size - 0x20;
        prim->x0 = xCoord + ((sine * self->ext.et_stopwatchCircle.size) >> 0xC);
        prim->y0 =
            yCoord - ((cosine * self->ext.et_stopwatchCircle.size) >> 0xC);
        prim->x2 = xCoord + ((sine * (temp_s0_4)) >> 0xC);
        prim->y2 = yCoord - ((cosine * temp_s0_4) >> 0xC);
        sine = rsin((i + 1) * 0x100);
        cosine = rcos((i + 1) * 0x100);
        prim->x1 = xCoord + ((sine * self->ext.et_stopwatchCircle.size) >> 0xC);
        prim->y1 =
            yCoord - ((cosine * self->ext.et_stopwatchCircle.size) >> 0xC);
        prim->x3 = xCoord + ((sine * temp_s0_4) >> 0xC);
        prim->y3 = yCoord - ((cosine * temp_s0_4) >> 0xC);
        prim = prim->next;
    }
}

#define SW_UNK90(e) (((s16*)&(e)->ext.ricStopWatch.parent)[0])
#define SW_UNK92(e) (((s16*)&(e)->ext.ricStopWatch.parent)[1])

void EntityStopWatch(Entity* self) {
    s32 sp4C;
    s32 psp_s6;
    s32 psp_s5;
    s16 psp_s4;
    s16 psp_s3;
    s32 psp_s2;
    s32 psp_s1;
    Primitive* prim;

    if (g_unkGraphicsStruct.pauseEnemies) {
        g_unkGraphicsStruct.D_800973FC = 0;
        if (self->step && self->step < 4) {
            self->step = 4;
        }
    }
    switch (self->step) {
    case 0:
        self->primIndex = g_api.AllocPrimitives(PRIM_GT4, 3);
        if (self->primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->flags = FLAG_POS_CAMERA_LOCKED | FLAG_UNK_10000000 | FLAG_HAS_PRIMS;
        prim = &g_PrimBuf[self->primIndex];
        prim->tpage = 0x1E;
        prim->clut = 0x17F;
        prim->v0 = prim->v1 = 0xC0;
        prim->v2 = prim->v3 = 0xD8;
        if (self->facingLeft) {
            prim->u0 = prim->u2 = 0x98;
            prim->u1 = prim->u3 = 0xA8;
        } else {
            prim->u0 = prim->u2 = 0xA8;
            prim->u1 = prim->u3 = 0x98;
        }
        prim->priority = RIC.zPriority + 1;
        prim->drawMode = DRAW_UNK_200 | DRAW_UNK_100 | DRAW_HIDE | DRAW_UNK02;
        prim = prim->next;

        prim->tpage = 0x1A;
        prim->clut = 0x186;
        prim->priority = RIC.zPriority + 3;
        prim->drawMode = DRAW_UNK_200 | DRAW_UNK_100 | DRAW_HIDE | DRAW_UNK02;

        prim = prim->next;

        prim->tpage = 0x1A;
        prim->clut = 0x186;
        prim->priority = RIC.zPriority + 3;
        prim->drawMode = DRAW_UNK_200 | DRAW_UNK_100 | DRAW_HIDE | DRAW_UNK02;
        if (self->params & 0xFF00) {
            RicCreateEntFactoryFromEntity(self, BP_66, 0);
        } else {
            RicCreateEntFactoryFromEntity(self, BP_STOPWATCH_CIRCLE, 0);
        }
        g_api.PlaySfx(SFX_UI_ALERT_TINK);
        g_unkGraphicsStruct.D_800973FC = 1;
        self->step++;
        break;
    case 1:
        prim = &g_PrimBuf[self->primIndex];
        prim->drawMode &= ~DRAW_HIDE;
        self->ext.ricStopWatch.unk84.val += 0x18000;
        if (self->ext.ricStopWatch.unk84.val >= 0x1A0000) {
            self->step++;
        }
        break;
    case 2:
        self->ext.ricStopWatch.unk84.val += 0xFFFF0000;
        if (self->ext.ricStopWatch.unk84.val <= 0x100000) {
            self->ext.ricStopWatch.t = 12;
            g_api.PlaySfx(SFX_STOPWATCH_TICK);
            self->step++;
        }
        break;
    case 3:
        self->ext.ricStopWatch.unk7E++;
        if (self->ext.ricStopWatch.unk7E > 0x50) {
            g_api.PlaySfx(SFX_STOPWATCH_TICK);
            self->ext.ricStopWatch.unk7E = 0;
            SW_UNK90(self) = 1;
            self->ext.ricStopWatch.t--;
            if (self->ext.ricStopWatch.t < 0) {
                self->step++;
                break;
            }
        }
        if (self->ext.ricStopWatch.t < 12) {
            prim = &g_PrimBuf[self->primIndex];
            prim = prim->next;
            if (self->ext.ricStopWatch.t >= 10) {
                SW_UNK92(self) = 1;
                psp_s1 = 8 * (self->ext.ricStopWatch.t / 10);
                prim->u0 = prim->u2 = psp_s1 + 0x18;
                prim->v0 = prim->v1 = 0x40;
                prim->u1 = prim->u3 = psp_s1 + 0x1E;
                prim->v2 = prim->v3 = 0x49;
                prim->drawMode &= ~DRAW_HIDE;
                prim = prim->next;
            } else {
                SW_UNK92(self) = 0;
            }
            psp_s1 = 8 * (self->ext.ricStopWatch.t % 10);
            if (psp_s1 == 0) {
                psp_s1 = 0x50;
            }
            prim->u0 = prim->u2 = psp_s1 + 0x18;
            prim->v0 = prim->v1 = 0x40;
            prim->u1 = prim->u3 = psp_s1 + 0x1E;
            prim->v2 = prim->v3 = 0x49;
            prim->drawMode &= ~DRAW_HIDE;
        }
        break;
    case 4:
        prim = &g_PrimBuf[self->primIndex];
        prim->priority = 0xC2;
        prim->drawMode &= ~DRAW_UNK_200;
        prim = prim->next;
        prim->drawMode |= DRAW_HIDE;
        prim = prim->next;
        prim->drawMode |= DRAW_HIDE;
        self->posX.i.hi += self->facingLeft ? 6 : -6;
        self->posY.i.hi -= self->ext.ricStopWatch.unk84.val >> 0x10;
        self->ext.ricStopWatch.t = 0;
        self->step++;
        break;
    case 5:
        self->ext.ricStopWatch.t++;
        if (self->ext.ricStopWatch.t >= 4) {
            prim = &g_PrimBuf[self->primIndex];
            prim->clut = PAL_FILL_WHITE;
            prim->drawMode |= DRAW_COLORS;
            prim->r0 = prim->r1 = prim->r2 = prim->r3 = 0x40;
            prim->g0 = prim->g1 = prim->g2 = prim->g3 = 0x40;
            prim->b0 = prim->b1 = prim->b2 = prim->b3 = 0x60;
            g_api.PlaySfx(SFX_UI_SUBWEAPON_TINK);
            self->step++;
        }
        break;
    case 6:
        self->ext.ricStopWatch.t++;
        if (self->ext.ricStopWatch.t > 14) {
            RicCreateEntFactoryFromEntity(self, FACTORY(BP_EMBERS, 7), 0);
            self->step++;
        }
        break;
    case 7:
        g_unkGraphicsStruct.D_800973FC = 0;
        DestroyEntity(self);
        return;
    }
    if (!self->step) {
        return;
    }
    prim = &g_PrimBuf[self->primIndex];
    if (self->step < 5) {
        psp_s1 = RIC.posX.val + (RIC.facingLeft ? FIX(8) : FIX(-8));
        if (RIC.hitboxOffY == 0 && RIC.hitboxHeight == 0) {
            psp_s2 = RIC.posY.val - FIX(16);
        } else {
            psp_s2 = RIC.posY.val - ((RIC.hitboxHeight - RIC.hitboxOffY) << 16);
        }
        self->posX.val += (psp_s1 - self->posX.val) / 12;
        self->posY.val += (psp_s2 - self->posY.val) / 4;
        if (RIC.facingLeft != self->facingLeft) {
            if (abs(psp_s1 - self->posX.val) < FIX(1)) {
                self->facingLeft = RIC.facingLeft;
            } else if (!self->facingLeft && psp_s1 < self->posX.val) {
                self->facingLeft = RIC.facingLeft;
            } else if (self->facingLeft && self->posX.val < psp_s1) {
                self->facingLeft = RIC.facingLeft;
            }
        }
        if (self->facingLeft) {
            prim->u0 = prim->u2 = 0x98;
            prim->u1 = prim->u3 = 0xA8;
        } else {
            prim->u0 = prim->u2 = 0xA8;
            prim->u1 = prim->u3 = 0x98;
        }
    }
    if (self->step < 3) {
        psp_s1 = self->posX.i.hi + (self->facingLeft ? 6 : -6);
        psp_s2 = self->posY.i.hi - (self->ext.ricStopWatch.unk84.val >> 0x10);
        if (self->ext.ricStopWatch.unk82 < 0x64) {
            self->ext.ricStopWatch.unk82 += 4;
        }
        if (self->ext.ricStopWatch.unk80 < 0x1000) {
            self->ext.ricStopWatch.unk80 += 0x80;
        }
        psp_s4 = (self->ext.ricStopWatch.unk82 * 8) / 100;
        psp_s3 = (self->ext.ricStopWatch.unk82 * 12) / 100;
        psp_s6 = rsin(self->ext.ricStopWatch.unk80);
        psp_s5 = rcos(self->ext.ricStopWatch.unk80);
        prim->x0 = psp_s1 + ((psp_s5 * -psp_s4 - psp_s6 * -psp_s3) >> 0xC);
        prim->y0 = psp_s2 + ((psp_s6 * -psp_s4 + psp_s5 * -psp_s3) >> 0xC);
        prim->x1 = psp_s1 + ((psp_s5 * psp_s4 - psp_s6 * -psp_s3) >> 0xC);
        prim->y1 = psp_s2 + ((psp_s6 * psp_s4 + psp_s5 * -psp_s3) >> 0xC);
        prim->x2 = psp_s1 + ((psp_s5 * -psp_s4 - psp_s6 * psp_s3) >> 0xC);
        prim->y2 = psp_s2 + ((psp_s6 * -psp_s4 + psp_s5 * psp_s3) >> 0xC);
        prim->x3 = psp_s1 + ((psp_s5 * psp_s4 - psp_s6 * psp_s3) >> 0xC);
        prim->y3 = psp_s2 + ((psp_s6 * psp_s4 + psp_s5 * psp_s3) >> 0xC);
    } else if (self->step < 5) {
        if (self->ext.ricStopWatch.unk84.val <= 0x100000) {
            if (SW_UNK90(self)) {
                self->ext.ricStopWatch.unk88 = (rand() % 0x40 + 0x200) * 0x100;
                SW_UNK90(self) = 0;
            } else {
                self->ext.ricStopWatch.unk88 = (rand() % 0x80 + 0x100) * 0x100;
            }
            if (self->ext.ricStopWatch.unk80 >= 0) {
                self->ext.ricStopWatch.unk80 = -(rand() % 0x40 + 0x40);
            } else {
                self->ext.ricStopWatch.unk80 = rand() % 0x40 + 0x40;
            }
        }
        self->ext.ricStopWatch.unk84.val += self->ext.ricStopWatch.unk88;
        self->ext.ricStopWatch.unk88 -= 0x4000;
        psp_s1 = self->posX.i.hi + (self->facingLeft ? 6 : -6);
        psp_s2 = self->posY.i.hi - (self->ext.ricStopWatch.unk84.val >> 0x10);
        psp_s6 = rsin(self->ext.ricStopWatch.unk80);
        psp_s5 = rcos(self->ext.ricStopWatch.unk80);
        prim->x0 = psp_s1 + (((psp_s5 * -8) - (psp_s6 * -12)) >> 0xC);
        prim->y0 = psp_s2 + (((psp_s6 * -8) + (psp_s5 * -12)) >> 0xC);
        prim->x1 = psp_s1 + (((psp_s5 * 8) - (psp_s6 * -12)) >> 0xC);
        prim->y1 = psp_s2 + (((psp_s6 * 8) + (psp_s5 * -12)) >> 0xC);
        prim->x2 = psp_s1 + (((psp_s5 * -8) - (psp_s6 * 12)) >> 0xC);
        prim->y2 = psp_s2 + (((psp_s6 * -8) + (psp_s5 * 12)) >> 0xC);
        prim->x3 = psp_s1 + (((psp_s5 * 8) - (psp_s6 * 12)) >> 0xC);
        prim->y3 = psp_s2 + (((psp_s6 * 8) + (psp_s5 * 12)) >> 0xC);
    } else {
        psp_s4 = 8 - self->ext.ricStopWatch.t;
        if (psp_s4 <= 0) {
            psp_s4 = 1;
        }
#if defined(VERSION_PSP)
        psp_s3 = (self->ext.ricStopWatch.t << 5) + 0xC;
#else
        psp_s3 = (self->ext.ricStopWatch.t << 16 >> 11) + 0xC;
#endif
        if (psp_s3 >= 0x80) {
            psp_s3 = 0x80;
        }
        prim->x0 = prim->x2 = self->posX.i.hi - psp_s4;
        prim->x1 = prim->x3 = self->posX.i.hi + psp_s4;
        prim->y0 = prim->y1 = self->posY.i.hi - psp_s3;
        prim->y2 = prim->y3 = self->posY.i.hi + psp_s3;
    }
    if (self->step < 4) {
        psp_s2 = self->posY.i.hi - 0xE;
        if (SW_UNK92(self)) {
            psp_s1 = self->posX.i.hi - (self->facingLeft ? 10 : -4);
            prim = prim->next;
            if (self->ext.ricStopWatch.unk7E < 8) {
                prim->x0 = prim->x2 =
                    psp_s1 - (self->ext.ricStopWatch.unk7E / 2);
                prim->x1 = prim->x3 =
                    psp_s1 + (self->ext.ricStopWatch.unk7E / 2);
                prim->y0 = prim->y1 =
                    (psp_s2 - 8) - (7 - self->ext.ricStopWatch.unk7E);
                prim->y2 = prim->y3 =
                    (psp_s2 + 8) + (7 - self->ext.ricStopWatch.unk7E);
            } else if (self->ext.ricStopWatch.unk7E >= 0x44) {
                psp_s4 = (0x50 - (self->ext.ricStopWatch.unk7E + 4)) / 2;
                if (psp_s4 < 0) {
                    psp_s4 = 0;
                }
                psp_s3 = self->ext.ricStopWatch.unk7E - 0x44;
                if (psp_s3 > 8) {
                    psp_s3 = 8;
                }
                prim->x0 = prim->x2 = psp_s1 - psp_s4;
                prim->x1 = prim->x3 = psp_s1 + psp_s4;
                prim->y0 = prim->y1 = (psp_s2 - 8) - psp_s3;
                prim->y2 = prim->y3 = (psp_s2 + 8) + psp_s3;
            } else {
                prim->x0 = prim->x2 = psp_s1 - 4;
                prim->x1 = prim->x3 = psp_s1 + 4;
                prim->y0 = prim->y1 = psp_s2 - 8;
                prim->y2 = prim->y3 = psp_s2 + 8;
            }
            psp_s1 = self->posX.i.hi - (self->facingLeft ? 4 : -10);
            prim = prim->next;
            if (self->ext.ricStopWatch.unk7E < 0xC) {
                psp_s4 = (self->ext.ricStopWatch.unk7E - 4) / 2;
                if (psp_s4 < 0) {
                    psp_s4 = 0;
                }
                psp_s3 = 7 - (self->ext.ricStopWatch.unk7E - 4);
                if (psp_s3 < 0) {
                    psp_s3 = 0;
                }
                prim->x0 = prim->x2 = psp_s1 - psp_s4;
                prim->x1 = prim->x3 = psp_s1 + psp_s4;
                prim->y0 = prim->y1 = (psp_s2 - 8) - psp_s3;
                prim->y2 = prim->y3 = (psp_s2 + 8) + psp_s3;
            } else if (self->ext.ricStopWatch.unk7E >= 0x48) {
                prim->x0 = prim->x2 =
                    psp_s1 - ((0x50 - self->ext.ricStopWatch.unk7E) / 2);
                prim->x1 = prim->x3 =
                    psp_s1 + ((0x50 - self->ext.ricStopWatch.unk7E) / 2);
                prim->y0 = prim->y1 =
                    (psp_s2 - 8) - (-0x48 + self->ext.ricStopWatch.unk7E);
                prim->y2 = prim->y3 =
                    (psp_s2 + 8) + (-0x48 + self->ext.ricStopWatch.unk7E);
            } else {
                prim->x0 = prim->x2 = psp_s1 - 4;
                prim->x1 = prim->x3 = psp_s1 + 4;
                prim->y0 = prim->y1 = psp_s2 - 8;
                prim->y2 = prim->y3 = psp_s2 + 8;
            }
        } else {
            psp_s1 = self->posX.i.hi - (self->facingLeft ? 4 : -4);
            prim = prim->next;
            if (self->ext.ricStopWatch.unk7E < 8) {
                prim->x0 = prim->x2 =
                    psp_s1 - (self->ext.ricStopWatch.unk7E / 2);
                prim->x1 = prim->x3 =
                    psp_s1 + (self->ext.ricStopWatch.unk7E / 2);
                prim->y0 = prim->y1 =
                    (psp_s2 - 8) - (7 - self->ext.ricStopWatch.unk7E);
                prim->y2 = prim->y3 =
                    (psp_s2 + 8) + (7 - self->ext.ricStopWatch.unk7E);
            } else if (self->ext.ricStopWatch.unk7E >= 0x48) {
                prim->x0 = prim->x2 =
                    psp_s1 - ((0x50 - self->ext.ricStopWatch.unk7E) / 2);
                prim->x1 = prim->x3 =
                    psp_s1 + ((0x50 - self->ext.ricStopWatch.unk7E) / 2);
                prim->y0 = prim->y1 =
                    (psp_s2 - 8) - (-0x48 + self->ext.ricStopWatch.unk7E);
                prim->y2 = prim->y3 =
                    (psp_s2 + 8) + (-0x48 + self->ext.ricStopWatch.unk7E);
            } else {
                prim->x0 = prim->x2 = psp_s1 - 5;
                prim->x1 = prim->x3 = psp_s1 + 5;
                prim->y0 = prim->y1 = psp_s2 - 8;
                prim->y2 = prim->y3 = psp_s2 + 8;
            }
        }
    }
}

void RicEntitySubwpnBible(Entity* self) {
    Primitive* prim;
    s32 sp48;
    s32 sp44;
    s32 sp40;
    s16 selfX;
    s16 selfY;
    s32 sp3C;
    s32 psp_s8;
    s32 psp_s7;
    s32 psp_s6;
    s32 psp_s5;
    s32 psp_s4;
    s32 psp_s3;
    s32 psp_s2;
    s32 psp_s1;

#if defined(VERSION_PSP)
    psp_s4 = 0;
#endif
    switch (self->step) {
    case 0:
        self->primIndex = g_api.AllocPrimitives(PRIM_GT4, 1);
        if (self->primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        self->flags =
            FLAG_UNK_10000000 | FLAG_HAS_PRIMS;
        prim = &g_PrimBuf[self->primIndex];
        prim->tpage = 0x1E;
        prim->clut = 0x17F;
        prim->u0 = prim->u2 = 0x98;
        prim->v0 = prim->v1 = 0xD8;
        prim->u1 = prim->u3 = 0xA8;
        prim->v2 = prim->v3 = 0xF0;
        prim->priority = RIC.zPriority + 1;
        prim->drawMode = DRAW_HIDE;
        if (self->facingLeft) {
            sp44 = 0x20;
        } else {
            sp44 = -0x20;
        }
        self->ext.et_BibleSubwpn.unk84 = sp44;

        self->hitboxWidth = 6;
        self->hitboxHeight = 6;
        self->step++;
        break;
    case 1:
        prim = &g_PrimBuf[self->primIndex];
        prim->drawMode &= ~DRAW_HIDE;
        self->ext.et_BibleSubwpn.unk86++;
        self->step++;
    case 2:
        self->ext.et_BibleSubwpn.unk7C++;
        self->ext.et_BibleSubwpn.unk7E++;
        if (self->ext.et_BibleSubwpn.unk7E >= 0x30) {
            self->step++;
        }
        break;
    case 3:
        self->ext.et_BibleSubwpn.unk7C++;
        if (self->ext.et_BibleSubwpn.unk7C >= 0x12C) {
            self->flags &= ~FLAG_UNK_10000000;
            if (self->facingLeft) {
                sp40 = FIX(-12);
            } else {
                sp40 = FIX(12);
            }
            self->velocityX = sp40;
            self->velocityY = FIX(-12);

            self->ext.et_BibleSubwpn.unk86++;
            self->step++;
        }
        break;
    }
    switch (self->ext.et_BibleSubwpn.unk86) {
    case 0:
        break;
    case 1:
        // All this logic is a mess, could use a cleanup
        psp_s2 = rsin(self->ext.et_BibleSubwpn.unk80);
        psp_s1 = rcos(self->ext.et_BibleSubwpn.unk80);
        psp_s5 = (psp_s2 * self->ext.et_BibleSubwpn.unk7E) >> 0xC;
        psp_s3 = (psp_s1 * self->ext.et_BibleSubwpn.unk7E) >> 0xC;
        psp_s7 = (psp_s1 * psp_s5 + psp_s2 * psp_s3);
        sp48 = (psp_s1 * psp_s3 - psp_s2 * psp_s5);
        psp_s5 = psp_s7 >> 0xC;
        psp_s3 = sp48 >> 0xC;
        psp_s2 = rsin(self->ext.et_BibleSubwpn.unk82);
        psp_s1 = rcos(self->ext.et_BibleSubwpn.unk82);
        psp_s7 = ((psp_s1 * psp_s5) + (psp_s2 * psp_s4)) >> 0xC;
        psp_s6 = ((psp_s1 * psp_s4) - (psp_s2 * psp_s5)) >> 0xC;
        psp_s4 = psp_s6;
        if (self->facingLeft) {
            psp_s6 = ((psp_s1 * psp_s4) + (psp_s2 * psp_s3)) >> 0xC;
        } else {
            psp_s6 = ((psp_s1 * psp_s4) - (psp_s2 * psp_s3)) >> 0xC;
        }
        self->ext.et_BibleSubwpn.unk80 += self->facingLeft ? 0x80 : -0x80;
        self->ext.et_BibleSubwpn.unk80 &= 0xFFF;
        self->ext.et_BibleSubwpn.unk82 += self->ext.et_BibleSubwpn.unk84;
        if (abs(self->ext.et_BibleSubwpn.unk82) >= 0x200) {
            self->ext.et_BibleSubwpn.unk84 *= -1;
        }
        self->posX.i.hi = RIC.posX.i.hi + psp_s7;
        self->posY.i.hi = RIC.posY.i.hi + psp_s6;
        self->zPriority = RIC.zPriority + (psp_s3 < 0 ? 2 : -2);
        break;
    case 2:
        self->posX.val += self->velocityX;
        self->posY.val += self->velocityY;
        self->velocityY -= FIX(2);
        break;
    }
    if (self->ext.et_BibleSubwpn.unk86) {
        selfX = self->posX.i.hi;
        selfY = self->posY.i.hi;
        prim = &g_PrimBuf[self->primIndex];
        prim->x0 = prim->x2 = selfX - 8;
        prim->x1 = prim->x3 = selfX + 8;
        prim->y0 = prim->y1 = selfY - 12;
        prim->y2 = prim->y3 = selfY + 12;
        prim->priority = self->zPriority;
        RicCreateEntFactoryFromEntity(self, BP_BIBLE_TRAIL, 0);
        if (g_GameTimer % 10 == 0) {
            g_api.PlaySfx(SFX_WEAPON_SWISH_C);
        }
    }
}

void RicEntityCrashCrossBeam(Entity* self) {
    MATRIX m;
    SVECTOR rot;
    VECTOR trans1;
    SVECTOR pos = {0, 0, 0};
    SVECTOR sp50 = {-96, 0, 0};
    s32 z;
    s32 nclip;
    Primitive* prim;
    s32 temp_v1;
    s32 i;
    u8* primUVCoords;
    SVECTOR** vectors_ptr;
    SVECTOR* psp_s7;
    SVECTOR* psp_s6;
    SVECTOR* temp_a3;
    SVECTOR* psp_s4;

    if (self->step == 0) {
        self->ext.giantcross.subweaponId = 0xD;
        InitializeEntity(D_us_801804A8);
        self->primIndex = g_api.func_800EDB58(PRIM_GT4, LEN(D_us_80182E88));
        if (self->primIndex == -1) {
            DestroyEntity(self);
            return;
        }
        prim = &g_PrimBuf[self->primIndex];
        self->flags |= FLAG_UNK_10000000 | FLAG_HAS_PRIMS;
        while (prim != NULL) {
            prim->tpage = 0x1C;
            prim->drawMode = DRAW_UNK_100 | DRAW_HIDE;
            prim = prim->next;
        }
        self->hitboxHeight = 0x50;
        self->hitboxWidth = 0xC;
        self->facingLeft = 0;
        self->posY.i.hi = 0x160;
        self->velocityY = FIX(-6.0);
        self->ext.giantcross.unk7C = 0;
        self->ext.giantcross.unk7E = 0x400;
        g_api.PlaySfx(SFX_FIREBALL_SHOT_B);
        prim = &g_PrimBuf[self->primIndex];
        primUVCoords = D_us_80182E88[0];
        for (i = 0; i < LEN(D_us_80182E88); i++, prim = prim->next,
            primUVCoords += 5) {
            prim->clut = (primUVCoords[4] & 0xF) + 0x1A0;
            switch (primUVCoords[4] & 0xF0) {
            case 0x10:
                prim->u0 = primUVCoords[0] + primUVCoords[2];
                prim->v0 = primUVCoords[1];
                prim->u1 = primUVCoords[0];
                prim->v1 = primUVCoords[1];
                prim->u2 = primUVCoords[0] + primUVCoords[2];
                prim->v2 = primUVCoords[1] + primUVCoords[3];
                prim->u3 = primUVCoords[0];
                prim->v3 = primUVCoords[1] + primUVCoords[3];
                break;
            case 0x20:
                prim->u0 = primUVCoords[0];
                prim->v0 = primUVCoords[1] + primUVCoords[3];
                prim->u1 = primUVCoords[0] + primUVCoords[2];
                prim->v1 = primUVCoords[1] + primUVCoords[3];
                prim->u2 = primUVCoords[0];
                prim->v2 = primUVCoords[1];
                prim->u3 = primUVCoords[0] + primUVCoords[2];
                prim->v3 = primUVCoords[1];
                break;
            case 0x30:
                prim->u0 = primUVCoords[0] + primUVCoords[2];
                prim->v0 = primUVCoords[1] + primUVCoords[3];
                prim->u1 = primUVCoords[0];
                prim->v1 = primUVCoords[1] + primUVCoords[3];
                prim->u2 = primUVCoords[0] + primUVCoords[2];
                prim->v2 = primUVCoords[1];
                prim->u3 = primUVCoords[0];
                prim->v3 = primUVCoords[1];
                break;
            case 0x40:
                prim->u3 = primUVCoords[0];
                prim->v3 = primUVCoords[1];
                prim->u1 = primUVCoords[0] + primUVCoords[2];
                prim->v1 = primUVCoords[1];
                prim->u2 = primUVCoords[0];
                prim->v2 = primUVCoords[1] + primUVCoords[3];
                prim->u0 = primUVCoords[0] + primUVCoords[2];
                prim->v0 = primUVCoords[1] + primUVCoords[3];
                break;
            case 0x50:
                prim->u3 = primUVCoords[0] + primUVCoords[2];
                prim->v3 = primUVCoords[1];
                prim->u1 = primUVCoords[0];
                prim->v1 = primUVCoords[1];
                prim->u2 = primUVCoords[0] + primUVCoords[2];
                prim->v2 = primUVCoords[1] + primUVCoords[3];
                prim->u0 = primUVCoords[0];
                prim->v0 = primUVCoords[1] + primUVCoords[3];
                break;
            case 0x60:
                prim->u3 = primUVCoords[0];
                prim->v3 = primUVCoords[1] + primUVCoords[3];
                prim->u1 = primUVCoords[0] + primUVCoords[2];
                prim->v1 = primUVCoords[1] + primUVCoords[3];
                prim->u2 = primUVCoords[0];
                prim->v2 = primUVCoords[1];
                prim->u0 = primUVCoords[0] + primUVCoords[2];
                prim->v0 = primUVCoords[1];
                break;
            case 0x70:
                prim->u3 = primUVCoords[0] + primUVCoords[2];
                prim->v3 = primUVCoords[1] + primUVCoords[3];
                prim->u1 = primUVCoords[0];
                prim->v1 = primUVCoords[1] + primUVCoords[3];
                prim->u2 = primUVCoords[0] + primUVCoords[2];
                prim->v2 = primUVCoords[1];
                prim->u0 = primUVCoords[0];
                prim->v0 = primUVCoords[1];
                break;
            default:
                prim->u0 = primUVCoords[0];
                prim->v0 = primUVCoords[1];
                prim->u1 = primUVCoords[0] + primUVCoords[2];
                prim->v1 = primUVCoords[1];
                prim->u2 = primUVCoords[0];
                prim->v2 = primUVCoords[1] + primUVCoords[3];
                prim->u3 = primUVCoords[0] + primUVCoords[2];
                prim->v3 = primUVCoords[1] + primUVCoords[3];
                break;
            }
        }
        self->step = 1;
        return;
    }
    if (self->posY.i.hi < -0x40) {
        DestroyEntity(self);
        return;
    }
    if (self->posY.i.hi < 0) {
        self->velocityY -= FIX(0.5);
    }
    self->posY.val += self->velocityY;
    self->ext.giantcross.unk7C += 0x60;
    self->ext.giantcross.unk7E += 0x60;
    trans1.vx = -(rcos(self->ext.giantcross.unk7C) * 0x60) >> 0xC;
    trans1.vy = self->posY.i.hi - 0x80;
    trans1.vz = ((rsin(self->ext.giantcross.unk7C) * 0x60) >> 0xC) + 0x180;
    rot.vy = self->ext.giantcross.unk7E;
    rot.vz = 0x40;
    rot.vx = 0;

    SetGeomOffset(self->posX.i.hi, 120);
    gte_SetGeomScreen(320);
    RotMatrix(&rot, &m);
    TransMatrix(&m, &trans1);
    gte_SetRotMatrix(&m);
    gte_SetTransMatrix(&m);
    gte_ldv0(&pos);
    gte_rtps();
    prim = &g_PrimBuf[self->primIndex];
    vectors_ptr = D_us_80182BA8[0];
    gte_stsxy((long*)&prim->x0);
    gte_stszotz((long*)&z);
    self->hitboxOffX = prim->x0 - self->posX.i.hi;
    self->hitboxOffY = prim->y0 - self->posY.i.hi;
    for (i = 0; i < LEN(D_us_80182E88); i++, prim = prim->next, vectors_ptr += 4) {
        psp_s7 = vectors_ptr[0];
        psp_s6 = vectors_ptr[1];
        psp_s4 = vectors_ptr[3];
        gte_ldv3(psp_s7, psp_s6, psp_s4);
        gte_rtpt();
        temp_a3 = vectors_ptr[2];
        prim->type = PRIM_GT4;
        gte_nclip();
        prim->drawMode = DRAW_HIDE;
        gte_stopz(&nclip);
        if (nclip < 0) {
            continue;
        }
        gte_stsxy3(&prim->x0, &prim->x1, &prim->x2);
        gte_ldv0(temp_a3);
        gte_rtps();
        prim->drawMode = DRAW_DEFAULT;
        if (z < 16) {
            prim->priority = 0x1F6;
        } else if (z > 998) {
            prim->priority = 0x10;
        } else {
            prim->priority = 0xD0 - (z - 0x50);
        }
        gte_stsxy((long*)&prim->x3);
    }
}

#endif
