// SPDX-License-Identifier: AGPL-3.0-or-later
// Runtime for the C that tools/mips2c_pc.py makes from MIPS asm (32-bit PC
// builds: pointers and u32 registers are the same size).
#ifndef MIPS2C_RT_H
#define MIPS2C_RT_H
#include <stdint.h>
#include <string.h>

typedef uint8_t u8;
typedef int8_t s8;
typedef uint16_t u16;
typedef int16_t s16;
typedef uint32_t u32;
typedef int32_t s32;
typedef uint64_t u64;
typedef int64_t s64;

_Static_assert(sizeof(void*) == 4, "mips2c output needs 32-bit pointers");

// Primitive has 4 more bytes on PC (after `next`, the packet length):
// accesses that land in g_PrimBuf move past them, and a byte offset the
// asm adds to g_PrimBuf (index * 0x34) becomes the PC one (PRIM_ADD).
// Checked by src/pc/mips2c_check.c.
#define PRIM_SIZE_PSX 0x34
#define PRIM_SIZE_PC 0x38
#define PRIM_COUNT 0x500
extern char g_PrimBuf[];
// PSX addresses of the overlay (scripts hold them): the stage registers
// its data ranges (src/pc/cutscene_pc.c)
u32 Mips2c_PsxToPc(u32 psxAddr);
// (also as KUSEG 0x0018xxxx: cutscene scripts add 0x100000 to 20 bits)
#define PSX_OVL(a) (((a) & 0x7FFFFFFFu) - 0x00180000u < 0x80000u)
static inline u32 PSXARG(u32 a) { return PSX_OVL(a) ? Mips2c_PsxToPc(a) : a; }

static inline u32 MEMADDR(u32 a) {
    if (PSX_OVL(a)) {
        a = Mips2c_PsxToPc(a);
    }
    u32 rel = a - (u32)(uintptr_t)g_PrimBuf;
    if (rel < PRIM_COUNT * PRIM_SIZE_PC && rel % PRIM_SIZE_PC >= 4) {
        return a + 4;
    }
    return a;
}
static inline u32 PRIM_ADD(u32 base, u32 off) {
    return base + off / PRIM_SIZE_PSX * PRIM_SIZE_PC + off % PRIM_SIZE_PSX;
}

#define MEM32(a) (*(u32*)(uintptr_t)MEMADDR(a))
#define MEM16(a) (*(u16*)(uintptr_t)MEMADDR(a))
#define MEMS16(a) (*(s16*)(uintptr_t)MEMADDR(a))
#define MEM8(a) (*(u8*)(uintptr_t)MEMADDR(a))
#define MEMS8(a) (*(s8*)(uintptr_t)MEMADDR(a))

typedef u32 (*Mips2cFn)(u32, u32, u32, u32, u32, u32, u32, u32);
#define CALL(fn, a0, a1, a2, a3, a4, a5, a6, a7)                               \
    ((Mips2cFn)(uintptr_t)(fn))(PSXARG(a0), PSXARG(a1), PSXARG(a2),           \
                                PSXARG(a3), PSXARG(a4), PSXARG(a5),           \
                                PSXARG(a6), PSXARG(a7))

// unaligned word access, little endian: lwl/swl name the highest byte
// (offset + 3), lwr/swr the lowest; a pair moves the whole word
static inline u32 LWL(u32 rt, u32 a) {
    a = MEMADDR(a);
    u32 sh = (a & 3) * 8, w;
    memcpy(&w, (void*)(uintptr_t)(a & ~3u), 4);
    return (rt & (0x00FFFFFFu >> sh)) | (w << (24 - sh));
}
static inline u32 LWR(u32 rt, u32 a) {
    a = MEMADDR(a);
    u32 sh = (a & 3) * 8, w;
    memcpy(&w, (void*)(uintptr_t)(a & ~3u), 4);
    return (rt & ~(0xFFFFFFFFu >> sh)) | (w >> sh);
}
static inline void SWL(u32 rt, u32 a) {
    a = MEMADDR(a);
    u32 sh = (a & 3) * 8, w;
    memcpy(&w, (void*)(uintptr_t)(a & ~3u), 4);
    w = (w & ~(0xFFFFFFFFu >> (24 - sh))) | (rt >> (24 - sh));
    memcpy((void*)(uintptr_t)(a & ~3u), &w, 4);
}
static inline void SWR(u32 rt, u32 a) {
    a = MEMADDR(a);
    u32 sh = (a & 3) * 8, w;
    memcpy(&w, (void*)(uintptr_t)(a & ~3u), 4);
    w = (w & ~(0xFFFFFFFFu << sh)) | (rt << sh);
    memcpy((void*)(uintptr_t)(a & ~3u), &w, 4);
}

// the GTE (psyz): cop2 registers by number, commands by their encoding
unsigned int Psyz_GteDataRead(unsigned idx);
void Psyz_GteDataWrite(unsigned idx, unsigned int v);
unsigned int Psyz_GteCtrlRead(unsigned idx);
void Psyz_GteCtrlWrite(unsigned idx, unsigned int v);
void Psyz_GteCommand(unsigned int cmd);

#endif
