#!/usr/bin/env python3
# pc_raw_data.py KIND STAGE OUT.S [extra symbols...]
#   KIND: st or boss; e.g. pc_raw_data.py boss bo6 src/boss/bo6/gen/data_pc.S
# The overlay's raw data regions (splat "data"/"bss"/"rodata" subsegments,
# assembly on PSX) as an assembly file for 32-bit PC builds, byte for byte
# from the disc's BIN: generated, like the other gen/ files, never
# committed. Every identifier the stage's C uses that lives in a raw region
# is placed at its offset; words pointing into the overlay or at known
# globals become relocations. Big regions mix pointer tables with pixel
# data, so there only runs of pointers (or code pointers) are relocated.
import re, sys, struct, json, os, glob, subprocess
ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..")
kind, stage, out = sys.argv[1:4]
extra = sys.argv[4:]
splat = ("bo" if kind == "boss" else "st") + stage
src = "boss" if kind == "boss" else "st"
binp = ROOT + "/disks/us/%s/%s/%s.BIN" % ("BOSS" if kind == "boss" else "ST", stage.upper(), stage.upper())
data = open(binp, "rb").read()
yaml = open(ROOT + "/config/splat.us.%s.yaml" % splat).read()
BASE = int(re.search(r"^\s*vram:\s*0x([0-9A-Fa-f]+)", yaml, re.M).group(1), 16)
END = BASE + len(data)
segs = []
for m in re.finditer(r"^\s*- \[0x([0-9A-Fa-f]+)(?:,\s*([.\w]+))?(?:,\s*([\w/]+))?", yaml, re.M):
    segs.append((int(m.group(1), 16) + BASE, m.group(2), m.group(3)))
segs.append((END, "end", None))
# the C files that still have assembly on US: their .rodata (jump tables,
# strings of the asm functions) is not made by the C build either
asm_files = set()
for f in glob.glob(ROOT + "/src/%s/%s/*.c" % ("boss" if kind == "boss" else "st", stage)):
    txt = subprocess.run(["unifdef", "-DVERSION_US", "-D_internal_version_us", "-DVERSION_PC",
                          "-UVERSION_PSP", "-UVERSION_HD", "-U_internal_version_hd", f],
                         capture_output=True, text=True).stdout
    if re.search(r"INCLUDE_ASM\(", txt):
        asm_files.add(os.path.basename(f)[:-2])
regions = []  # raw data: no C file
for i, (a, t, n) in enumerate(segs[:-1]):
    if segs[i + 1][0] <= a:
        continue
    if t in ("data", "bss", "rodata") or (t == ".rodata" and n in asm_files):
        regions.append((a, segs[i + 1][0], t, n))
# merge adjacent regions
merged = []
for r in regions:
    if merged and merged[-1][1] == r[0]:
        merged[-1] = (merged[-1][0], r[1], merged[-1][2], merged[-1][3])
    else:
        merged.append(r)
regions = merged
gsyms, ssyms, gsize = {}, {}, {}
for f, d in (("symbols.us.txt", gsyms), ("symbols.us.%s.txt" % splat, ssyms)):
    for l in open(ROOT + "/config/" + f):
        m = re.match(r"\s*(\w+)\s*=\s*0x([0-9A-Fa-f]+);(?:\s*//.*size:\s*0x([0-9A-Fa-f]+))?", l)
        if m:
            d[m.group(1)] = int(m.group(2), 16)
            if m.group(3) and d is gsyms: gsize[m.group(1)] = int(m.group(3), 16)
gsize["g_Entities"] = 0xBC * 256
# functions of the overlay by address: splat's asm files (their names are
# the ones the C uses; needs `splat split` of the stage), then the symbol file
fn = {}
for f in glob.glob(ROOT + "/asm/us/%s/%s/*matchings/*/*.s" % (src, stage)):
    t = open(f).read()
    name = os.path.basename(f)[:-2]
    m = re.search(r"glabel %s\s*\n\s*/\* \w+ ([0-9A-F]{8})" % name, t)
    if m: fn[int(m.group(1), 16)] = name
for n, a in ssyms.items():
    fn.setdefault(a, n)

gby = {}
for n, a in gsyms.items(): gby.setdefault(a, n)
sby = {}
for n, a in ssyms.items(): sby.setdefault(a, n)
# identifiers the stage's sources use (candidates for placement)
undef = set(extra)
files = glob.glob(ROOT + "/src/%s/%s/*.[ch]" % (src, stage)) + glob.glob(ROOT + "/src/%s/%s/gen/*.[ch]" % (src, stage))
i = 0
while i < len(files):  # and the shared headers they include, recursively
    f = files[i]; i += 1
    for inc in re.findall(r'#include\s+"([^"]+)"', open(f, errors="replace").read()):
        g = os.path.normpath(os.path.join(os.path.dirname(f), inc))
        if os.path.exists(g) and g not in files: files.append(g)
for f in files:
    undef |= set(re.findall(r"\b[A-Za-z_]\w*\b", open(f, errors="replace").read()))
def addr(n):
    if n in ssyms: return ssyms[n]
    if n in gsyms: return gsyms[n]
    m = re.match(r"D_(?:us_)?([0-9A-Fa-f]{8})$", n)
    return int(m.group(1), 16) if m else None
def region_of(a):
    for r in regions:
        if r[0] <= a < r[1]: return r
    return None
def rname(r): return "%s_region_%X" % (stage, r[0])
# identifiers the stage's C defines (file scope, not extern) on US/PC,
# in its .c files and the headers they include; static ones can't be
# referenced from another file
def unifdef(f):
    return subprocess.run(["unifdef", "-DVERSION_US", "-D_internal_version_us", "-DVERSION_PC",
                           "-UVERSION_PSP", "-UVERSION_HD", "-U_internal_version_hd", f],
                          capture_output=True, text=True).stdout
cdefs, cstatic = set(), set()
cfiles = [f for f in glob.glob(ROOT + "/src/%s/%s/*.c" % (src, stage)) if "psp" not in os.path.basename(f)]
cfiles += glob.glob(ROOT + "/src/%s/%s/gen/*.c" % (src, stage))
for f in cfiles:  # grows while it runs: recursive
    for inc in re.findall(r'#include\s+"([^"]+)"', unifdef(f)):
        g = os.path.normpath(os.path.join(os.path.dirname(f), inc))
        if os.path.exists(g) and g not in cfiles: cfiles.append(g)
for f in cfiles:
    for m in re.finditer(r"^(?!extern\b)(static\s+)?(?:const\s+)?[A-Za-z_][\w\s\*]*?\b(\w+)\s*(?:\[[^\]]*\])*\s*=",
                         unifdef(f), re.M):
        (cstatic if m.group(1) else cdefs).add(m.group(2))
cdefs -= cstatic
# C symbols with a known address, in order: the one before a word owns it
cplaced = sorted((a, n) for n, a in ssyms.items() if n in cdefs and BASE <= a < END)
bounds = sorted(set([a for n, a in ssyms.items() if BASE <= a < END] + [r[0] for r in regions] +
                    [r[1] for r in regions] + [sg[0] for sg in segs]))
def c_owner(w):
    import bisect
    i = bisect.bisect_right([a for a, n in cplaced], w) - 1
    if i < 0: return None
    a, n = cplaced[i]
    j = bisect.bisect_right(bounds, a)
    nxt = bounds[j] if j < len(bounds) else END
    return (n, w - a) if w < nxt else None
want = {}
copies = {}  # name -> (start, end): bytes the asm reads in C-owned data
missing = []
for n in sorted(undef):
    a = addr(n)
    if a is not None and region_of(a) and n not in cdefs: want[n] = a
    elif (a is not None and BASE <= a < END and n not in cdefs and n not in cstatic
          and n not in fn.values() and re.match(r"D_(us_)?[0-9A-F]{8}$", n)):
        # a label inside C-defined data: give the asm a copy of its bytes
        j = [b for b in bounds if b > a]
        copies[n] = (a, min(j) if j else END)
unresolved = []
lines = ["/* SPDX-License-Identifier: AGPL-3.0-or-later */",
         "/* %s.BIN's raw data regions (asm on PSX) for the PC build, byte for" % stage.upper(),
         " * byte; pointers into the overlay or at globals are relocations.",
         " * Generated by tools/pc_raw_data.py. */",
         "    .data", "    .balign 4"]
def candidate(w):
    """(reference, is_code) if word w looks like a pointer, else None"""
    if not 0x80000000 <= w < 0x80800000: return None
    rr = region_of(w)
    if rr: return ("%s + 0x%X" % (rname(rr), w - rr[0]), False)
    if BASE <= w < END:
        if w in fn: return (fn[w], True)
        if w in sby and sby[w] in cdefs: return (sby[w], False)
        o = c_owner(w)
        if o: return ("%s + 0x%X" % o if o[1] else o[0], False)
        return None
    if w in gby: return (gby[w], False)
    if w < BASE:
        inside = [n for n, sz in gsize.items() if gsyms[n] <= w < gsyms[n] + sz]
        if inside: return ("%s + 0x%X" % (inside[0], w - gsyms[inside[0]]), False)
    return None
for r in regions:
    a0, a1 = r[0], r[1]
    lines.append("    .global %s" % rname(r))  # for cutscene address ranges
    lines.append("%s:  /* 0x%08X..0x%08X %s %s */" % (rname(r), a0, a1, r[2], r[3] or ""))
    names = sorted([(a, n) for n, a in want.items() if a0 <= a < a1])
    for a, n in names:
        lines.append("    .global %s" % n)
        lines.append("    .set %s, %s + 0x%X" % (n, rname(r), a - a0))
    # Big regions mix pointer tables with pixel data whose words can look
    # like addresses: there only runs of 3+ pointers, or code pointers,
    # count. Small regions are tables and structs: every candidate counts.
    big = a1 - a0 >= 0x4000
    words = {}
    a = (a0 + 3) & ~3
    while a + 4 <= min(a1, END):
        words[a] = candidate(struct.unpack_from("<I", data, a - BASE)[0])
        a += 4
    def run(a):
        n = 1; b = a - 4
        while words.get(b): n += 1; b -= 4
        b = a + 4
        while words.get(b): n += 1; b += 4
        return n
    a = a0
    while a < a1:
        if a in words:
            w = struct.unpack_from("<I", data, a - BASE)[0]
            c = words[a]
            if c and (not big or c[1] or run(a) >= 3):
                lines.append("    .word %s /* 0x%08X */" % (c[0], w))
            else:
                if c is None and 0x80000000 <= w < 0x80800000 and not (0x80000000 <= w < 0x80010000):
                    unresolved.append((a, w))
                lines.append("    .word 0x%08X" % w)
            a += 4; continue
        b = data[a - BASE] if a < END else 0
        lines.append("    .byte 0x%02X" % b); a += 1
    lines.append("    .balign 4")
for n, (a0, a1) in sorted(copies.items()):
    lines.append("    .global %s" % n)
    lines.append("%s:  /* copy of 0x%08X..0x%08X, inside C data */" % (n, a0, a1))
    for a in range(a0, min(a1, END)):
        lines.append("    .byte 0x%02X" % data[a - BASE])
    lines.append("    .balign 4")
# the regions as cutscene address ranges (src/pc/cutscene_pc.c): scripts
# hold PSX addresses of their own data and of the portraits
lines += ["    .balign 4", "    .global %s_pc_regions" % stage, "%s_pc_regions:" % stage]
for r in regions:
    lines.append("    .word %s, 0x%08X, 0x%X" % (rname(r), r[0], r[1] - r[0]))
lines += ["    .global %s_pc_region_count" % stage, "%s_pc_region_count:" % stage,
          "    .word %d" % len(regions)]
open(out, "w").write("\n".join(lines) + "\n")
print("regions:", ", ".join("%X..%X" % (r[0], r[1]) for r in regions))
print("placed %d symbols" % len(want))
nrel = sum(1 for l in lines if l.startswith("    .word") and "/*" in l)
print("relocations: %d" % nrel)
print("unresolved pointer-like words: %d" % len(unresolved))
for a, w in unresolved[:40]: print("  at %08X: %08X" % (a, w))
