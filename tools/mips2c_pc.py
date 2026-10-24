#!/usr/bin/env python3
# mips2c_pc.py OVL OUT.c FUNC... : the asm-only functions of an overlay as
# portable C, instruction by instruction, for the 32-bit PC build.
#
# Each MIPS register is a local u32 and memory is native memory: the data
# structures have the PSX layout on 32-bit targets, so a pointer loaded
# from an entity or passed by C code is used as is. Globals stay symbolic:
# splat writes %hi/%lo(symbol) and every %lo is resolved against the
# symbol its matching %hi loaded (a forward dataflow over the function),
# so globals that are separate variables on PC still resolve. Jump tables
# become switches, delay slots run before their branch, calls pass a0-a3
# and four stack words. The output is generated from the disc's code:
# like the other gen/ files, it is never committed.
#
#   OVL: the splat name (bobo5, stno3, ...); FUNC: functions to translate,
#   default: every INCLUDE_ASM of the overlay's C that is active on US.
#
# SPDX-License-Identifier: AGPL-3.0-or-later
import glob, os, re, subprocess, sys

ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..")
REGS = ["zero", "at", "v0", "v1", "a0", "a1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
        "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra"]
BRANCH = {"beq", "bne", "beqz", "bnez", "bgez", "bgtz", "blez", "bltz", "b", "bal", "bgezal", "bltzal"}
JUMP = {"j", "jal", "jr", "jalr"}


class Insn:
    def __init__(self, addr, op, args, line):
        self.addr, self.op, self.args, self.line = addr, op, args, line
        self.label = None


def parse(path):
    """(name, insns, labels, jtables) of a splat function .s"""
    insns, labels, jtbls, cur = [], {}, {}, None
    name = None
    section = ".text"
    pending = []
    for l in open(path):
        s = l.strip()
        if s.startswith(".section"):
            section = s.split()[1]
            continue
        m = re.match(r"glabel (\w+)", s)
        if m:
            if section == ".text" and name is None:
                name = m.group(1)
            elif section != ".text":
                cur = m.group(1)
                jtbls[cur] = []
            continue
        m = re.match(r"(\.L\w+):", s)
        if m:
            pending.append(m.group(1))
            continue
        m = re.match(r"/\* \w+ (\w+) \w+ \*/\s+\.word\s+(\S+)", s)
        if m and section != ".text":
            if cur is not None:
                jtbls[cur].append(m.group(2))
            continue
        m = re.match(r"/\* \w+ (\w+) (\w+) \*/\s+(\S+)\s*(.*)", s)
        if m and section == ".text":
            args = [a.strip() for a in re.split(r",(?![^(]*\))", m.group(4))] if m.group(4) else []
            ins = Insn(int(m.group(1), 16), m.group(3), args, s)
            ins.raw = int.from_bytes(bytes.fromhex(m.group(2)), "little")
            for p in pending:
                labels[p] = len(insns)
            if pending:
                ins.label = pending[0]
            pending = []
            insns.append(ins)
    return name, insns, labels, jtbls


def reg(a):
    a = a.strip()
    assert a.startswith("$"), a
    r = a[1:]
    return "fp" if r == "s8" else r


def imm(a):
    a = a.replace(" ", "")
    if re.fullmatch(r"[-+()0-9A-Fa-fx<>&|~]+", a):
        return eval(a)  # splat writes (0xC8202000 >> 16), (0x... & 0xFFFF)
    return int(a, 0)


class Ctx:
    """symbol resolution: splat name (+offset) -> native C expression"""

    def __init__(self, ovl):
        self.ovl = ovl
        self.syms = {}  # name -> ps1 address
        self.size = {}
        for f in ("symbols.us.txt", "symbols.us.%s.txt" % ovl):
            p = os.path.join(ROOT, "config", f)
            if not os.path.exists(p):
                continue
            for l in open(p):
                m = re.match(r"\s*(\w+)\s*=\s*0x([0-9A-Fa-f]+);(?:.*size:\s*0x([0-9A-Fa-f]+))?", l)
                if m:
                    self.syms.setdefault(m.group(1), int(m.group(2), 16))
                    if m.group(3):
                        self.size[m.group(1)] = int(m.group(3), 16)
        self.size["g_Entities"] = 0xBC * 256
        self.size.update({"g_api": 0x144, "g_Settings": 0x110, "g_Status": 0x334, "g_Player": 0x3D0,
                          "g_Tilemap": 0x54, "g_unkGraphicsStruct": 0x98, "g_PlayerDraw": 0x280})
        self.native = set()  # names that exist as C symbols (set by the caller)
        self.used = set()

    def addr(self, name):
        if name in self.syms:
            return self.syms[name]
        m = re.match(r"(?:D|func|jtbl|g_\w+?)_(?:us_)?([0-9A-F]{8})$", name)
        return int(m.group(1), 16) if m else None

    def load_stage(self, kind, stage):
        """raw data regions (from gen/data_pc.S) and the stage's C-defined
        identifiers, to tell which overlay names exist on PC"""
        self.regions = []
        p = os.path.join(ROOT, "src", kind, stage, "gen", "data_pc.S")
        if os.path.exists(p):
            for m in re.finditer(r"/\* 0x([0-9A-F]{8})\.\.0x([0-9A-F]{8})", open(p).read()):
                self.regions.append((int(m.group(1), 16), int(m.group(2), 16)))
        self.cdefs, static = set(), set()
        files = [f for f in glob.glob(os.path.join(ROOT, "src", kind, stage, "*.c"))
                 if "psp" not in os.path.basename(f)]
        unifdef = lambda f: subprocess.run(["unifdef", "-DVERSION_US", "-D_internal_version_us", "-DVERSION_PC",
                                            "-UVERSION_PSP", "-UVERSION_HD", "-U_internal_version_hd", f],
                                           capture_output=True, text=True).stdout
        for f in files:  # grows while it runs: recursive
            for inc in re.findall(r'#include\s+"([^"]+)"', unifdef(f)):
                g = os.path.normpath(os.path.join(os.path.dirname(f), inc))
                if os.path.exists(g) and g not in files:
                    files.append(g)
        for f in files:
            txt = subprocess.run(["unifdef", "-DVERSION_US", "-D_internal_version_us", "-DVERSION_PC",
                                  "-UVERSION_PSP", "-UVERSION_HD", "-U_internal_version_hd", f],
                                 capture_output=True, text=True).stdout
            for m in re.finditer(r"^(?!extern\b)(static\s+)?(?:const\s+)?[A-Za-z_][\w\s\*]*?\b(\w+)\s*"
                                 r"(?:\[[^\]]*\])*\s*=", txt, re.M):
                (static if m.group(1) else self.cdefs).add(m.group(2))
        self.cdefs -= static
        self.dra = sorted((a, n) for n, a in self.syms.items() if n in self.native and a < 0x80180000)
        self.ovl_c = sorted((a, n) for n, a in self.syms.items() if n in self.cdefs and a >= 0x80180000)

    def resolve(self, expr):
        """'sym + 0x10' -> (cname, offset)"""
        import bisect
        m = re.match(r"(\w+)\s*(?:([+-])\s*(0x[0-9A-Fa-f]+|\d+))?$", expr.strip())
        name, off = m.group(1), 0
        if m.group(2):
            off = int(m.group(3), 0) * (1 if m.group(2) == "+" else -1)
        if name in self.native or name in getattr(self, "cdefs", ()):
            return name, off
        a = self.addr(name)
        if a is None:
            return name, off
        a += off
        if 0x1F800000 <= a < 0x1F800400:  # the scratchpad
            return "g_Scratchpad", a - 0x1F800000
        # inside a sized global that exists natively
        for b, sz in self.size.items():
            if b in self.native and b in self.syms and self.syms[b] <= a < self.syms[b] + sz:
                return b, a - self.syms[b]
        if a < 0x80180000 and getattr(self, "dra", None):
            # a field alias (g_pads_1_pressed): the native global before it
            i = bisect.bisect_right([x for x, _ in self.dra], a) - 1
            if i >= 0:
                b = self.dra[i][1]
                if b == name or self.size.get(b, 1 << 30) > a - self.dra[i][0]:
                    return b, a - self.dra[i][0]
        if a >= 0x80180000:
            if any(r0 <= a < r1 for r0, r1 in getattr(self, "regions", ())):
                return name, off  # raw data: tools/pc_raw_data.py places it
            i = bisect.bisect_right([x for x, _ in self.ovl_c], a) - 1
            if i >= 0 and a - self.ovl_c[i][0] < 0x100:
                # inside a global C array (a large gap: not its data)
                return self.ovl_c[i][1], a - self.ovl_c[i][0]
            # else tools/pc_raw_data.py gives the asm a copy of the bytes
        return name, off


class Fn:
    def __init__(self, ctx, path):
        self.ctx = ctx
        self.name, self.ins, self.labels, self.jtbls = parse(path)
        self.out = []
        self.calls = set()
        self.syms = set()

    # hi tracking: state[reg] = (symbol expr, offset adjust) for registers
    # that hold %hi(sym) (+ an index); %lo(other) then resolves against it
    def hi_states(self):
        n = len(self.ins)
        succ = [[] for _ in range(n)]
        for i, ins in enumerate(self.ins):
            nxt = i + 1
            op = ins.op
            if op in BRANCH or op in JUMP:
                # the delay slot follows; control continues after it
                if i + 1 < n:
                    succ[i].append(i + 1)
                continue
            if i > 0 and (self.ins[i - 1].op in BRANCH or self.ins[i - 1].op in JUMP):
                p = self.ins[i - 1]
                if p.op in ("j", "b"):
                    succ[i].append(self.labels[p.args[-1]])
                elif p.op == "jr":
                    if reg(p.args[0]) != "ra":
                        succ[i] += [self.labels[t] for tb in self.jtbls.values() for t in tb if t in self.labels]
                elif p.op in BRANCH:
                    succ[i].append(self.labels[p.args[-1]])
                    succ[i].append(nxt)
                else:  # jal, jalr
                    succ[i].append(nxt)
                continue
            succ[i].append(nxt)
        state = [None] * (n + 1)
        state[0] = {}
        work = [0]
        while work:
            i = work.pop()
            if i >= n:
                continue
            st = dict(state[i])
            ins = self.ins[i]
            a = ins.args
            if ins.op == "lui" and "%hi(" in a[1]:
                st[reg(a[0])] = re.search(r"%hi\((.*)\)", a[1]).group(1)
            elif ins.op == "addiu" and "%lo(" in a[2]:
                st[reg(a[0])] = re.search(r"%lo\((.*)\)", a[2]).group(1)
            elif ins.op in ("addu",) and len(a) == 3 and (reg(a[1]) in st or reg(a[2]) in st):
                s = st.get(reg(a[1])) or st.get(reg(a[2]))
                st[reg(a[0])] = s
            else:
                d = self.dest(ins)
                if d:
                    st.pop(d, None)
                if ins.op in ("jal", "jalr"):
                    for r in ("v0", "v1", "a0", "a1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
                              "t8", "t9", "at", "ra"):
                        st.pop(r, None)
            for j in succ[i]:
                if j > n:
                    continue
                if state[j] is None:
                    state[j] = st
                    work.append(j)
                else:
                    merged = {k: v for k, v in state[j].items() if st.get(k) == v}
                    conflict = {k for k in state[j] if k in st and st[k] != state[j][k]}
                    for k in conflict:
                        merged[k] = "?"
                    if merged != state[j]:
                        state[j] = merged
                        work.append(j)
        return state

    def dest(self, ins):
        if ins.op in ("sw", "sh", "sb", "swl", "swr", "swc2", "j", "jr", "b", "mult", "multu", "div", "divu",
                      "mtc2", "ctc2", "nop", "break", "mthi", "mtlo") or ins.op in BRANCH:
            return None
        if ins.op in ("jal", "jalr"):
            return "ra"
        if not ins.args:
            return None
        a = ins.args[0]
        return reg(a) if a.startswith("$") else None

    def symaddr(self, expr):
        """native address expression of a splat symbol (+offset)"""
        name, off = self.ctx.resolve(expr)
        if name == "g_Player" and off >= 0x374:
            off += 4  # PC: unk20 at 0x370 is 64-bit
        self.syms.add(name)
        return "((u32)%s + 0x%X)" % (name, off & 0xFFFFFFFF) if off else "((u32)%s)" % name

    def lo(self, a, st, base):
        """the value of an immediate operand that may be %lo(sym)"""
        m = re.match(r"%lo\((.*)\)", a)
        if not m:
            return "0x%X" % (imm(a) & 0xFFFFFFFF), None
        target = m.group(1)
        hi = st.get(base) if base else None
        if hi and hi != "?" and hi != target:
            # base holds &hi_sym (+index): rebase onto the target symbol
            return "(%s - %s)" % (self.symaddr(target), self.symaddr(hi)), target
        return "0", target

    def r(self, a):
        x = reg(a)
        return "0u" if x == "zero" else x

    def emit(self, s):
        self.out.append("    " + s)

    def mem(self, ins, st):
        a = ins.args
        rt = reg(a[0])
        m = re.match(r"(.*)\((\$\w+)\)$", a[1])
        off, base = m.group(1).strip(), reg(m.group(2))
        o, _ = self.lo(off, st, base) if off else ("0", None)
        if off.startswith("%lo(") and base != "zero" and not st.get(base):
            # %lo without a known %hi in the base: take the symbol's address
            o = "(%s - %s)" % (self.symaddr(re.match(r"%lo\((.*)\)", off).group(1)), base)
        b = "0u" if base == "zero" else base
        ea = "(%s + %s)" % (b, o)
        return rt, ea

    def jtbl_of(self, st, r):
        s = st.get(r)
        if s and s in self.ctx_jtbls():
            return s
        return None

    def ctx_jtbls(self):
        return self.jtbls

    def translate(self):
        states = self.hi_states()
        ins = self.ins
        n = len(ins)
        frame = 0
        for x in ins:
            if x.op == "addiu" and reg(x.args[0]) == "sp" and reg(x.args[1]) == "sp" and imm(x.args[2]) < 0:
                frame = max(frame, -imm(x.args[2]))
        used_labels = set()
        for x in ins:
            if x.op in BRANCH or x.op in ("j",):
                used_labels.add(x.args[-1])
        for tb in self.jtbls.values():
            used_labels.update(tb)
        i = 0
        body = []
        self.out = body
        jt_index = {}  # register -> jtbl name, after lw from a jtbl
        while i < n:
            x = ins[i]
            for lab, idx in self.labels.items():
                if idx == i and lab in used_labels:
                    body.append("%s:;" % lab.replace(".", "_"))
            st = states[i] or {}
            if x.op in BRANCH or x.op in JUMP:
                d = ins[i + 1] if i + 1 < n else None
                self.branch(x, d, st, states[i + 1] if i + 1 < n else {})
                i += 2
                continue
            self.one(x, st)
            i += 1
        regs = [r for r in REGS if r not in ("zero", "sp", "a0", "a1", "a2", "a3")]
        head = ["u32 %s(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7) {" % self.name,
                "    u32 %s;" % ", ".join("%s = 0" % r for r in regs + ["hi", "lo"]),
                "    u32 stk[(0x%X + 0x20) / 4];" % frame,
                "    u32 sp = (u32)stk + 0x%X;" % frame,
                "    u64 r64;",
                "    (void)r64;",
                "    MEM32(sp + 0x10) = a4; MEM32(sp + 0x14) = a5; MEM32(sp + 0x18) = a6; MEM32(sp + 0x1C) = a7;"]
        return "\n".join(head + body + ["    return v0;", "}"])

    def call(self, target_expr, direct=None):
        if direct:
            self.calls.add(direct)
        self.emit("v0 = CALL(%s, a0, a1, a2, a3, MEM32(sp + 0x10), MEM32(sp + 0x14), MEM32(sp + 0x18), "
                  "MEM32(sp + 0x1C)); v1 = 0;" % target_expr)

    def cond(self, x):
        a = x.args
        op = x.op
        if op == "beq":
            return "%s == %s" % (self.r(a[0]), self.r(a[1]))
        if op == "bne":
            return "%s != %s" % (self.r(a[0]), self.r(a[1]))
        if op == "beqz":
            return "%s == 0" % self.r(a[0])
        if op == "bnez":
            return "%s != 0" % self.r(a[0])
        if op == "bgez":
            return "(s32)%s >= 0" % self.r(a[0])
        if op == "bgtz":
            return "(s32)%s > 0" % self.r(a[0])
        if op == "blez":
            return "(s32)%s <= 0" % self.r(a[0])
        if op == "bltz":
            return "(s32)%s < 0" % self.r(a[0])
        if op == "b":
            return "1"
        raise Exception("branch " + x.line)

    def branch(self, x, d, st, st_d):
        lab = lambda t: t.replace(".", "_")
        if x.op in BRANCH:
            if x.op == "b" or (x.op == "beq" and self.r(x.args[0]) == self.r(x.args[1])):
                if d:
                    self.one(d, st_d)
                self.emit("goto %s;" % lab(x.args[-1]))
                return
            self.emit("{ int c = %s;" % self.cond(x))
            if d:
                self.one(d, st_d)
            self.emit("  if (c) goto %s; }" % lab(x.args[-1]))
            return
        if x.op == "j":
            if d:
                self.one(d, st_d)
            self.emit("goto %s;" % lab(x.args[0]))
            return
        if x.op == "jal":
            if d:
                self.one(d, st_d)
            self.call(x.args[0], direct=x.args[0])
            return
        if x.op == "jalr":
            r = reg(x.args[-1])
            self.emit("{ u32 fn = %s;" % r)
            if d:
                self.one(d, st_d)
            self.call("fn")
            self.emit("}")
            return
        if x.op == "jr":
            r = reg(x.args[0])
            if r == "ra":
                if d:
                    self.one(d, st_d)
                self.emit("return v0;")
                return
            # a jump table: r holds the table index (see the lw from it)
            tb = st.get("jt:" + r) or self.jt_last
            self.emit("{ u32 t = %s;" % r)
            if d:
                self.one(d, st_d)
            self.emit("  switch (t) {")
            for k, t in enumerate(self.jtbls[tb]):
                if t.startswith(".L"):  # not the alignment padding
                    self.emit("  case %d: goto %s;" % (k, lab(t)))
            self.emit("  } }")
            self.emit("return v0; /* bad jump table index */")
            return
        raise Exception(x.line)

    jt_last = None

    def one(self, x, st):
        a = x.args
        op = x.op
        r = self.r
        e = self.emit
        if op == "nop":
            return
        d = reg(a[0]) if a and a[0].startswith("$") else None
        if d == "zero" and op not in ("sw", "sh", "sb", "mult", "multu", "div", "divu"):
            return
        if op == "lui":
            m = re.match(r"%hi\((.*)\)", a[1])
            if m:
                if m.group(1) in self.jtbls:
                    e("%s = 0;" % d)
                else:
                    e("%s = %s;" % (d, self.symaddr(m.group(1))))
            else:
                e("%s = 0x%X;" % (d, (imm(a[1]) << 16) & 0xFFFFFFFF))
            return
        if op in ("addiu", "addi"):
            v, sym = self.lo(a[2], st, reg(a[1]))
            if sym and not st.get(reg(a[1])):
                e("%s = %s;" % (d, self.symaddr(sym)))
            else:
                e("%s = %s + %s;" % (d, r(a[1]), v))
            return
        if op in ("ori", "andi", "xori", "slti", "sltiu"):
            v = imm(a[2]) & 0xFFFF if op != "slti" and op != "sltiu" else imm(a[2])
            if op == "ori":
                e("%s = %s | 0x%X;" % (d, r(a[1]), v))
            elif op == "andi":
                e("%s = %s & 0x%X;" % (d, r(a[1]), v))
            elif op == "xori":
                e("%s = %s ^ 0x%X;" % (d, r(a[1]), v))
            elif op == "slti":
                e("%s = (s32)%s < %d;" % (d, r(a[1]), imm(a[2])))
            else:
                e("%s = %s < 0x%Xu;" % (d, r(a[1]), imm(a[2]) & 0xFFFFFFFF))
            return
        if op in ("addu", "add"):
            # the PSX byte offset of a primitive: PC ones are bigger
            if st.get(reg(a[1])) == "g_PrimBuf" and st.get(reg(a[2])) != "g_PrimBuf":
                e("%s = PRIM_ADD(%s, %s);" % (d, r(a[1]), r(a[2])))
            elif st.get(reg(a[2])) == "g_PrimBuf" and st.get(reg(a[1])) != "g_PrimBuf":
                e("%s = PRIM_ADD(%s, %s);" % (d, r(a[2]), r(a[1])))
            else:
                e("%s = %s + %s;" % (d, r(a[1]), r(a[2])))
            return
        if op in ("subu", "sub"):
            e("%s = %s - %s;" % (d, r(a[1]), r(a[2])))
            return
        if op == "negu":
            e("%s = -%s;" % (d, r(a[1])))
            return
        if op in ("and", "or", "xor"):
            e("%s = %s %s %s;" % (d, r(a[1]), {"and": "&", "or": "|", "xor": "^"}[op], r(a[2])))
            return
        if op == "nor":
            e("%s = ~(%s | %s);" % (d, r(a[1]), r(a[2])))
            return
        if op == "not":
            e("%s = ~%s;" % (d, r(a[1])))
            return
        if op == "move":
            e("%s = %s;" % (d, r(a[1])))
            return
        if op == "slt":
            e("%s = (s32)%s < (s32)%s;" % (d, r(a[1]), r(a[2])))
            return
        if op == "sltu":
            e("%s = %s < %s;" % (d, r(a[1]), r(a[2])))
            return
        if op == "sll":
            e("%s = %s << %d;" % (d, r(a[1]), imm(a[2])))
            return
        if op == "srl":
            e("%s = %s >> %d;" % (d, r(a[1]), imm(a[2])))
            return
        if op == "sra":
            e("%s = (u32)((s32)%s >> %d);" % (d, r(a[1]), imm(a[2])))
            return
        if op == "sllv":
            e("%s = %s << (%s & 31);" % (d, r(a[1]), r(a[2])))
            return
        if op == "srlv":
            e("%s = %s >> (%s & 31);" % (d, r(a[1]), r(a[2])))
            return
        if op == "srav":
            e("%s = (u32)((s32)%s >> (%s & 31));" % (d, r(a[1]), r(a[2])))
            return
        if op == "mult":
            e("r64 = (u64)((s64)(s32)%s * (s64)(s32)%s); lo = (u32)r64; hi = (u32)(r64 >> 32);" % (r(a[0]), r(a[1])))
            return
        if op == "multu":
            e("r64 = (u64)%s * (u64)%s; lo = (u32)r64; hi = (u32)(r64 >> 32);" % (r(a[0]), r(a[1])))
            return
        if op == "div":
            x0, x1 = r(a[-2]), r(a[-1])
            e("if (%s) { lo = (u32)((s32)%s / (s32)%s); hi = (u32)((s32)%s %% (s32)%s); }" % (x1, x0, x1, x0, x1))
            return
        if op == "divu":
            x0, x1 = r(a[-2]), r(a[-1])
            e("if (%s) { lo = %s / %s; hi = %s %% %s; }" % (x1, x0, x1, x0, x1))
            return
        if op == "mflo":
            e("%s = lo;" % d)
            return
        if op == "mfhi":
            e("%s = hi;" % d)
            return
        if op == "mtlo":
            e("lo = %s;" % r(a[0]))
            return
        if op == "mthi":
            e("hi = %s;" % r(a[0]))
            return
        if op in ("lw", "lh", "lhu", "lb", "lbu"):
            rt, ea = self.mem(x, st)
            m = re.match(r"%lo\(([^)]*)\)", a[1])
            if op == "lw" and m and m.group(1) in self.jtbls:
                # jump table entry: keep the index (base is the byte offset)
                b = reg(re.search(r"\((\$\w+)\)", a[1]).group(1))
                e("%s = %s >> 2;" % (rt, b))
                Fn.jt_last = m.group(1)
                self.jt_last = m.group(1)
                return
            t = {"lw": "MEM32", "lh": "(u32)(s32)MEMS16", "lhu": "MEM16", "lb": "(u32)(s32)MEMS8", "lbu": "MEM8"}[op]
            e("%s = %s(%s);" % (rt, t, ea))
            return
        if op in ("sw", "sh", "sb"):
            rt, ea = self.mem(x, st)
            v = "0u" if rt == "zero" else rt
            t = {"sw": "MEM32", "sh": "MEM16", "sb": "MEM8"}[op]
            e("%s(%s) = %s;" % (t, ea, v if op == "sw" else "(%s)%s" % ({"sh": "u16", "sb": "u8"}[op], v)))
            return
        if op in ("lwl", "lwr", "swl", "swr"):
            rt, ea = self.mem(x, st)
            # little endian unaligned pair: lwl at off+3, lwr at off
            if op == "lwr":
                e("%s = LWR(%s, %s);" % (rt, rt, ea))
            elif op == "lwl":
                e("%s = LWL(%s, %s);" % (rt, rt, ea))
            elif op == "swr":
                e("SWR(%s, %s);" % ("0u" if rt == "zero" else rt, ea))
            else:
                e("SWL(%s, %s);" % ("0u" if rt == "zero" else rt, ea))
            return
        if op == "break":
            return
        if op in ("lwc2", "swc2"):
            m = re.match(r"\$?(\d+)", a[0])
            rt, ea = self.mem(Insn(x.addr, op, ["$zero", a[1]], x.line), st)
            if op == "lwc2":
                e("Psyz_GteDataWrite(%s, MEM32(%s));" % (m.group(1), ea))
            else:
                e("MEM32(%s) = Psyz_GteDataRead(%s);" % (ea, m.group(1)))
            return
        if op in ("mtc2", "ctc2"):
            n = re.match(r"\$?(\d+)", a[1]).group(1)
            e("Psyz_Gte%sWrite(%s, %s);" % ("Data" if op == "mtc2" else "Ctrl", n, r(a[0])))
            return
        if op in ("mfc2", "cfc2"):
            n = re.match(r"\$?(\d+)", a[1]).group(1)
            e("%s = Psyz_Gte%sRead(%s);" % (d, "Data" if op == "mfc2" else "Ctrl", n))
            return
        if op in ("rtps", "rtpt", "nclip", "avsz3", "avsz4", "mvmva", "dpcs", "op", "sqr", "ncs", "nct",
                  "ncds", "ncdt", "nccs", "ncct", "cdp", "cc", "dpct", "intpl", "gpf", "gpl", "rtpt"):
            e("Psyz_GteCommand(0x%X); /* %s */" % (x.raw & 0x1FFFFFF, op))
            return
        raise Exception("unhandled: " + x.line)


HEADER = """/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* Generated by tools/mips2c_pc.py from the disc's code: never committed. */
#include "mips2c_rt.h"
"""


def main():
    ovl, out = sys.argv[1], sys.argv[2]
    names = sys.argv[3:]
    stage = re.sub(r"^(bo|st)", "", ovl)
    kind = "boss" if ovl.startswith("bo") else "st"
    asmdir = os.path.join(ROOT, "asm/us", kind, stage, "nonmatchings")
    ctx = Ctx(ovl)
    nat = os.environ.get("MIPS2C_NATIVE")
    if nat and os.path.exists(nat):
        ctx.native = set(open(nat).read().split())
    ctx.load_stage(kind, stage)
    fns = []
    for n in names:
        p = glob.glob(os.path.join(asmdir, "**", n + ".s"), recursive=True)
        if not p:
            sys.exit("no asm for " + n)
        fns.append(Fn(ctx, p[0]))
    bodies = [f.translate() for f in fns]
    defined = {f.name for f in fns}
    ext = set()
    for f in fns:
        ext |= f.syms | f.calls
    ext -= defined
    lines = [HEADER]
    for n in sorted(ext):
        lines.append("extern char %s[];" % n)
    for f in fns:
        lines.append("u32 %s(u32, u32, u32, u32, u32, u32, u32, u32);" % f.name)
    lines.append("")
    lines += bodies
    open(out, "w").write("\n\n".join(lines) + "\n")


if __name__ == "__main__":
    main()
