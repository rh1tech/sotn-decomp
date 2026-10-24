#!/usr/bin/env python3
# pc_gen_stage.py KIND STAGE [ELF] : the generated sources a stage still
# needs for the 32-bit PC build:
#   gen/mips2c_pc.c  every function the US build only has as assembly
#                    (INCLUDE_ASM active with VERSION_US and VERSION_PC),
#                    translated by tools/mips2c_pc.py
#   gen/data_pc.S    the raw data regions, with every identifier the C
#                    (and the translated code) uses (tools/pc_raw_data.py)
# ELF, a firmware or program built before, names the symbols that exist
# natively, so aliases such as g_Entities_224 become g_Entities + offset.
# KIND: st or boss. The asm comes from splat (asm/us/...).
#
# SPDX-License-Identifier: AGPL-3.0-or-later
import glob, os, re, subprocess, sys, tempfile

ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..")
kind, stage = sys.argv[1], sys.argv[2]
elf = sys.argv[3] if len(sys.argv) > 3 else None
src = os.path.join(ROOT, "src", kind, stage)
gen = os.path.join(src, "gen")
splat = ("bo" if kind == "boss" else "st") + stage

names = []
for f in sorted(glob.glob(os.path.join(src, "*.c"))):
    if "psp" in os.path.basename(f):
        continue
    txt = subprocess.run(["unifdef", "-DVERSION_US", "-D_internal_version_us", "-DVERSION_PC",
                          "-UVERSION_PSP", "-UVERSION_HD", "-U_internal_version_hd", f],
                         capture_output=True, text=True).stdout
    names += re.findall(r'INCLUDE_ASM\(\s*"[^"]*",\s*(\w+)\)', txt)

env = dict(os.environ)
if elf and elf.endswith(".txt"):  # a list of names instead
    env["MIPS2C_NATIVE"] = elf
elif elf:
    nm = subprocess.run(["arm-none-eabi-nm", elf], capture_output=True, text=True).stdout
    tmp = tempfile.NamedTemporaryFile("w", delete=False, suffix=".txt")
    tmp.write("\n".join(sorted({l.split()[-1] for l in nm.splitlines() if l.strip()})))
    tmp.close()
    env["MIPS2C_NATIVE"] = tmp.name
out = os.path.join(gen, "mips2c_pc.c")
os.makedirs(gen, exist_ok=True)
# the raw regions first: the translation reads them from data_pc.S
subprocess.run([sys.executable, os.path.join(ROOT, "tools", "pc_raw_data.py"), kind, stage,
                os.path.join(gen, "data_pc.S")], check=True, stdout=subprocess.DEVNULL)
if names:
    subprocess.run([sys.executable, os.path.join(ROOT, "tools", "mips2c_pc.py"), splat, out] + names,
                   env=env, check=True)
else:
    open(out, "w").write("/* no asm-only functions */\n")
subprocess.run([sys.executable, os.path.join(ROOT, "tools", "pc_raw_data.py"), kind, stage,
                os.path.join(gen, "data_pc.S")], check=True, stdout=subprocess.DEVNULL)
print("%s: %d functions translated" % (stage, len(names)))
