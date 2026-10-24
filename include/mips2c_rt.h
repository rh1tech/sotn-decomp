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

#define MEM32(a) (*(u32*)(uintptr_t)(a))
#define MEM16(a) (*(u16*)(uintptr_t)(a))
#define MEMS16(a) (*(s16*)(uintptr_t)(a))
#define MEM8(a) (*(u8*)(uintptr_t)(a))
#define MEMS8(a) (*(s8*)(uintptr_t)(a))

typedef u32 (*Mips2cFn)(u32, u32, u32, u32, u32, u32, u32, u32);
#define CALL(fn, a0, a1, a2, a3, a4, a5, a6, a7)                               \
    ((Mips2cFn)(uintptr_t)(fn))(a0, a1, a2, a3, a4, a5, a6, a7)

// unaligned word access, little endian: lwl/swl name the highest byte
// (offset + 3), lwr/swr the lowest; a pair moves the whole word
static inline u32 LWL(u32 rt, u32 a) {
    u32 sh = (a & 3) * 8, w;
    memcpy(&w, (void*)(uintptr_t)(a & ~3u), 4);
    return (rt & (0x00FFFFFFu >> sh)) | (w << (24 - sh));
}
static inline u32 LWR(u32 rt, u32 a) {
    u32 sh = (a & 3) * 8, w;
    memcpy(&w, (void*)(uintptr_t)(a & ~3u), 4);
    return (rt & ~(0xFFFFFFFFu >> sh)) | (w >> sh);
}
static inline void SWL(u32 rt, u32 a) {
    u32 sh = (a & 3) * 8, w;
    memcpy(&w, (void*)(uintptr_t)(a & ~3u), 4);
    w = (w & ~(0xFFFFFFFFu >> (24 - sh))) | (rt >> (24 - sh));
    memcpy((void*)(uintptr_t)(a & ~3u), &w, 4);
}
static inline void SWR(u32 rt, u32 a) {
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
