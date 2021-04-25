#!/usr/bin/env python3
# Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
# All rights reserved.
#
# This software is supplied "AS IS" without any warranties.
# RDA assumes no responsibility or liability for the use of the software,
# conveys no license or title under any patent, copyright, or mask work
# right to the product. RDA reserves the right to make changes in the
# software without notification.  RDA also make no representation or
# warranty that such application will be suitable for the specified use
# without further testing or modification.

import os
import sys
import re
import optparse
import fnmatch

# section types
section_text = [".text", ".text.*", ".ram", "RESET", "VECTORS",
                ".sramboottext", '.sramtext', '.ramtext', ".boot_sector_start",
                ".boottext", ".irqtext", ".romtext", ".bootsramtext",
                ".ram", ".start_entry", ".exception", ".tlb_exception",
                ".tlbtext", ".syssram_L1_text", ".nbsram_patch_text",
                ".init", ".sramTEXT",".noinit"]
section_ro = [".rodata", ".rodata.*",
              ".bootrodata", ".roresdata", ".robsdata", ".extra"]
section_rw = [".data", ".data.*",
              ".rwkeep", ".bootsramdata", ".sramdata", ".sramucdata",
              ".srroucdata", ".ucdata"]
section_zi = [".bss", ".bss.*", "COMMON", ".scommon", ".sdata", ".sdata.*",
              ".sbss", ".sbss.*", ".nbsram_globals",
              ".sramuninit", ".sramucuninit", ".dsp_iq_data", ".ramucbss",
              ".backup", ".bootsrambss", ".ucbss", ".srambss", ".sramucbss",
              ".ucbackup", ".xcv_reg_value", ".abb_reg_value", ".pmu_reg_value",
              ".hal_boot_sector_reload_struct", ".boot_sector_reload_struct_ptr",
              ".boot_sector_struct", ".boot_sector_struct_ptr",
              ".fixptr", ".dbgfunc", ".sram_overlay", ".TTBL1", ".TTBL2"]
section_ignore = [".ARM.exidx", ".ARM.attributes", ".comment", ".debug_*",
                  ".iplt", ".rel.iplt", ".igot.plt", '.reginfo', ".mdebug.*",
                  ".pdr", '.rel.dyn','.noinit']


# check a name matches a list of patterns
def name_match(name, patterns):
    return any(fnmatch.fnmatch(name, pattern) for pattern in patterns)


# section type by section name
def section_type(section_name):
    if name_match(section_name, section_ignore):
        stype = "ignore"
    elif name_match(section_name, section_text):
        stype = "text"
    elif name_match(section_name, section_ro):
        stype = "rodata"
    elif name_match(section_name, section_rw):
        stype = "data"
    elif name_match(section_name, section_zi):
        stype = "bss"
    else:
        stype = "ignore"
#        print("unknown section:", section_name)
#        sys.exit(1)
    return stype


# make an entry, a section in one object file
def make_entry(section_name, size, obj):
    stype = section_type(section_name)
    r = re.compile(r'(\S+)\((\S+)\)')
    m = r.fullmatch(obj)
    fobj = obj
    if m:
        lib = m.group(1)
        obj = m.group(2)
    else:
        lib = obj

    lib = lib.split("/")[-1]
    obj = obj.split("/")[-1]
    return {"section": section_name, "stype": stype, "size": size,
            "lib": lib, "obj": obj, 'fobj': fobj}


# parse map file
def parse_map(fname):
    fh = open(fname, 'r')

    # "SECTION ADDRESS SIZE OBJ"
    r2 = re.compile(
        r"\s+(\S+)\s+0x([0-9a-fA-F]{8,16})\s+0x([0-9a-fA-F]+)\s+(\S+)")
    # "SECTION"
    r3 = re.compile(r"\s+(\S+)")
    # "ADDRESS SIZE OBJ"
    r4 = re.compile(r"\s+0x([0-9a-fA-F]{8,16})\s+0x([0-9a-fA-F]+)\s+(\S+)")

    ents = []
    map_found = False
    section_name = None
    for line in fh.readlines():
        if line.startswith("Linker script and memory map"):
            map_found = True
            continue
        if not map_found:
            continue
        if line.startswith('OUTPUT('):
            break

        line = line.rstrip()

        if section_name:
            m = r4.fullmatch(line)
            if m:
                size = int(m.group(2), 16)
                obj = m.group(3)
                ents.append(make_entry(section_name, size, obj))
            section_name = None
            continue

        m = r2.fullmatch(line)
        if m:
            name = m.group(1)
            size = int(m.group(3), 16)
            obj = m.group(4)
            ents.append(make_entry(name, size, obj))
            continue

        m = r3.fullmatch(line)
        if m:
            section_name = m.group(1)
            continue
    return ents


def sizelist(f):
    sl = [f["text"],
          f["rodata"],
          f["data"],
          f["bss"],
          f["text"] + f["rodata"],
          f["text"] + f["rodata"] + f["data"],
          f["data"] + f["bss"]]
    ss = [str(s) for s in sl]
    return ",".join(ss)


def main(argv):
    opt = optparse.OptionParser(usage="""usage %prog [options]

This utility will analyze code size from map file. Code size are expressed
in concept of: text, rodata, data, bss. There is a map inside this script
to map each section name to section type. When there is an unknown section
name, it is needed to add the section name to this script.
""")

    opt.add_option("--map", action="store", dest="map",
                   help="map file created at linking.")
    opt.add_option("--outobj", action="store", dest="outobj", default="outobj.csv",
                   help="detailed information by object file (default: outobj.csv)")
    opt.add_option("--outlib", action="store", dest="outlib", default="outlib.csv",
                   help="detailed information by library (default: outlib.csv)")
    opt.add_option("--outsect", action="store", dest="outsect", default="outsect.csv",
                   help="detailed information by section (default: outsect.csv)")

    opt, argv = opt.parse_args(argv)

    if not opt.map:
        print("No map file specified!")
        return 1

    ents = parse_map(opt.map)

    stype_total = {"text": 0, "rodata": 0,
                   "data": 0, "bss": 0, 'ignore': 0}
    for ent in ents:
        stype = ent['stype']
        size = ent['size']
        stype_total[stype] += size

    print("total: text,rodata,data,bss,code,flash,ram")
    print("      ", sizelist(stype_total))

    lib_total = {}
    for ent in ents:
        lib = ent['lib']
        stype = ent['stype']
        size = ent['size']
        if lib not in lib_total:
            lib_total[lib] = {"text": 0, "rodata": 0,
                              "data": 0, "bss": 0, 'ignore': 0}
        lib_total[lib][stype] += size

    print("size by library to %s ..." % (opt.outlib))
    fh = open(opt.outlib, "w")
    fh.write("library name,text,rodata,data,bss,code,flash,ram\n")
    for n, l in lib_total.items():
        fh.write("%s,%s\n" % (n, sizelist(l)))
    fh.close()

    obj_total = {}
    for ent in ents:
        fobj = ent['fobj']
        lib = ent['lib']
        obj = ent['obj']
        stype = ent['stype']
        size = ent['size']
        if fobj not in obj_total:
            obj_total[fobj] = {"lib": lib, "obj": obj, "text": 0, "rodata": 0,
                               "data": 0, "bss": 0, 'ignore': 0}
        obj_total[fobj][stype] += size

    print("size by object to %s ..." % (opt.outobj))
    fh = open(opt.outobj, "w")
    fh.write("object name,library name,text,rodata,data,bss,code,flash,ram\n")
    for n, l in obj_total.items():
        fh.write("%s,%s,%s\n" % (l['obj'], l["lib"], sizelist(l)))
    fh.close()

    section_total = {}
    for ent in ents:
        section = ent['section']
        stype = ent['stype']
        size = ent['size']
        if section not in section_total:
            section_total[section] = {"type": stype, "size": 0}
        section_total[section]['size'] += size

    print("size by section to %s ..." % (opt.outsect))
    fh = open(opt.outsect, "w")
    fh.write("section name,section type,size\n")
    for n, l in section_total.items():
        fh.write("%s,%s,%s\n" % (n, l['type'], l['size']))
    fh.close()

    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
