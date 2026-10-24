// SPDX-License-Identifier: AGPL-3.0-or-later
//
// The PS1 BIOS kanji ROM for the PC build: Krom2RawAdd() returns the 16x15
// glyph for a Shift-JIS code, which func_80106A28 turns into the menu and
// dialogue font. The glyphs are the OpenBIOS fonts that come with psyz
// (tools/psyz/nugget/openbios/charset, public domain Shinonome 14 dot font);
// the code-to-glyph lookup follows OpenBIOS' sjis.c (MIT, PCSX-Redux
// authors), for the layout of those fonts.
//
// The game's C strings are Shift-JIS on the PS1 but UTF-8 in the sources:
// Pc_NextSjis() reads one character of either.
#include <game.h>

#ifndef KROM_FONT_DIR
#define KROM_FONT_DIR "tools/psyz/nugget/openbios/charset"
#endif

__asm__(".pushsection .rodata\n"
        ".balign 4\n"
        "s_KromFont1:\n"
        ".incbin \"" KROM_FONT_DIR "/font1.raw\"\n"
        ".balign 4\n"
        "s_KromFont2:\n"
        ".incbin \"" KROM_FONT_DIR "/font2.raw\"\n"
        ".popsection\n");
extern const u8 s_KromFont1[] __asm__("s_KromFont1");
extern const u8 s_KromFont2[] __asm__("s_KromFont2");

struct KromRange {
    u16 codepoint;
    u16 offset;
};

// where each run of codes starts in font1.raw / font2.raw
static const struct KromRange s_KromTable[] = {
    {0x8140, 0x0000}, {0x8180, 0x003f}, {0x81ad, 0x006d}, {0x81b8, 0x006c},
    {0x81c0, 0x0080}, {0x81c8, 0x0074}, {0x81cf, 0x008f}, {0x81da, 0x007b},
    {0x81e9, 0x00a9}, {0x81f0, 0x008a}, {0x81f8, 0x00b8}, {0x81fc, 0x0092},
    {0x81fd, 0x00bd}, {0x824f, 0x0093}, {0x8259, 0x0119}, {0x8260, 0x009d},
    {0x827a, 0x013a}, {0x8281, 0x00b7}, {0x829b, 0x015b}, {0x829f, 0x00d1},
    {0x82f2, 0x01b2}, {0x8340, 0x0124}, {0x837f, 0x023f}, {0x8380, 0x0163},
    {0x8397, 0x0257}, {0x839f, 0x017a}, {0x83b7, 0x0277}, {0x83bf, 0x0192},
    {0x83d7, 0x0297}, {0x8440, 0x01aa}, {0x8461, 0x0321}, {0x8470, 0x01cb},
    {0x847f, 0x033f}, {0x8480, 0x01da}, {0x8492, 0x0352}, {0x849f, 0x01ec},
    {0x889f, 0x0000}, {0x8900, 0x001e}, {0x897f, 0x009c}, {0x8a00, 0x00da},
    {0x8a7f, 0x0158}, {0x8b00, 0x0196}, {0x8b7f, 0x0214}, {0x8c00, 0x0252},
    {0x8c7f, 0x02d0}, {0x8d00, 0x030e}, {0x8d7f, 0x038c}, {0x8e00, 0x03ca},
    {0x8e7f, 0x0448}, {0x8f00, 0x0486}, {0x8f7f, 0x0504}, {0x9000, 0x0542},
    {0x907f, 0x05c0}, {0x9100, 0x05fe}, {0x917f, 0x067c}, {0x9200, 0x06ba},
    {0x927f, 0x0738}, {0x9300, 0x0776}, {0x937f, 0x07f4}, {0x9400, 0x0832},
    {0x947f, 0x08b0}, {0x9500, 0x08ee}, {0x957f, 0x096c}, {0x9600, 0x09aa},
    {0x967f, 0x0a28}, {0x9700, 0x0a66}, {0x977f, 0x0ae4}, {0x9800, 0x0b22},
    {0xffff, 0x0000},
};

static u16 Krom2Offset(u16 c) {
    s32 i = 1;

    while (s_KromTable[i].codepoint <= c) {
        i++;
    }
    i--;
    return c - s_KromTable[i].codepoint + s_KromTable[i].offset;
}

// 30 bytes, 15 rows of 16 pixels; (unsigned long)-1 for codes it lacks
unsigned long Krom2RawAdd(unsigned short c) {
    if (c >= 0x8140 && c <= 0x84be) {
        return (unsigned long)(s_KromFont1 + Krom2Offset(c) * 0x1e);
    }
    if (c >= 0x889f && c <= 0x9872) {
        return (unsigned long)(s_KromFont2 + Krom2Offset(c) * 0x1e);
    }
    return (unsigned long)-1;
}

#include "sjis_table.h"

static u16 UnicodeToSjis(u32 uni) {
    s32 lo = 0, hi = LEN(s_Utf8Sjis) - 1, mid;

    while (lo <= hi) {
        mid = (lo + hi) / 2;
        if (s_Utf8Sjis[mid].uni == uni) {
            return s_Utf8Sjis[mid].sjis;
        }
        if (s_Utf8Sjis[mid].uni < uni) {
            lo = mid + 1;
        } else {
            hi = mid - 1;
        }
    }
    return 0x8148; // ？
}

// One two-byte character from str: a Shift-JIS pair as the PS1 strings have
// it, or the UTF-8 sequence of the PC sources converted to one
u16 Pc_NextSjis(const char** str) {
    const u8* s = (const u8*)*str;
    u32 uni;

    if (s[0] >= 0xE0 && s[0] < 0xF0 && (s[1] & 0xC0) == 0x80 &&
        (s[2] & 0xC0) == 0x80) {
        uni = ((s[0] & 0x0F) << 12) | ((s[1] & 0x3F) << 6) | (s[2] & 0x3F);
        *str += 3;
        return UnicodeToSjis(uni);
    }
    if (s[0] >= 0xC2 && s[0] < 0xE0 && (s[1] & 0xC0) == 0x80) {
        uni = ((s[0] & 0x1F) << 6) | (s[1] & 0x3F);
        *str += 2;
        return UnicodeToSjis(uni);
    }
    *str += 2;
    return (s[0] << 8) | s[1];
}
