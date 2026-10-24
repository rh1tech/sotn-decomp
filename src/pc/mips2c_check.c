// SPDX-License-Identifier: AGPL-3.0-or-later
// The layout tools/mips2c_pc.py's output (include/mips2c_rt.h) assumes.
#include <game.h>

STATIC_ASSERT(sizeof(void*) == 4, "translated asm needs 32-bit pointers");
STATIC_ASSERT(sizeof(Primitive) == 0x38, "PRIM_SIZE_PC");
STATIC_ASSERT(OFF(Primitive, r0) == 8, "Primitive: next, then the length");
STATIC_ASSERT(MAX_PRIM_COUNT == 0x500, "PRIM_COUNT");
STATIC_ASSERT(sizeof(Entity) == 0xBC, "Entity has the PSX layout");
STATIC_ASSERT(OFF(PlayerState, unk24) == 0x378, "g_Player remap in mips2c_pc.py");
