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

import argparse
import sys
import struct
import shutil
import os
import glob
import subprocess
import json

DESCRIPTION = """
Create modem image use fbdevgen
"""

FLAG_LZMA = 0x100
FLAG_LZMA2 = 0x200
FLAG_LZMA3 = 0x400
FLAG_LZMA_MASK = 0x700
CPIO_FILE_FORMAT = '=2s12H{}s{}s'
CPIO_OLDLE_MAGIC = b'\xc7\x71'


# Create directory if not exists
def ensure_dir(dname):
    if dname and not os.path.exists(dname):
        os.makedirs(dname)


# Collect nvm files from prj. It is required that nvm are located in
# the same directory of prj
def nvm_from_prj(fname):
    nvms = []
    pdir = os.path.dirname(fname)
    with open(fname, 'r') as fh:
        for line in fh.readlines():
            line = line.strip()
            if line.startswith('#') or not line:
                continue
            if line.startswith('MODULE'):
                fields = line.split()
                if len(fields) == 3:
                    nvms.append(os.path.join(pdir, fields[2]))
    return nvms


# Collect modules from prj.
def modules_from_prj(fname):
    modules = []
    with open(fname, 'r') as fh:
        for line in fh.readlines():
            line = line.strip()
            if line.startswith('#') or not line:
                continue
            if line.startswith('MODULE'):
                fields = line.split()
                if len(fields) == 3:
                    modules.append(os.path.basename(fields[2]))
    return modules


# Get generated bin file name from prj
def nvbin_from_prj(prj):
    with open(prj, 'r') as fh:
        for line in fh.readlines():
            line = line.strip()
            if line.startswith('#') or not line:
                continue
            if line.startswith('TARGET'):
                fields = line.split()
                if len(fields) != 3:
                    return None
                return os.path.join(os.path.dirname(prj), fields[2])
    return None


# Merge prj. The header is fixed, and MODULE are merged.
def nvm_prj_merge(dst, srcs):
    dstdir = os.path.dirname(dst)
    nvms = []
    for src in srcs:
        nvms.extend(nvm_from_prj(src))

    nvms = copy_files(dstdir, nvms)

    with open(dst, 'w') as fh:
        fh.write('# SpreadTrum NVEditor Project File, Format Version 1.0\n')
        fh.write('PROJECT = NVitem\n')
        fh.write('ALIGNMENT = 4\n')
        fh.write('TARGET = nvitem.bin\n')
        fh.write('DESCRIPTION =\n')
        for nvm in nvms:
            fh.write('MODULE = {}\n'.format(os.path.basename(nvm)))
    return


# Hack memlist. The flags will be replace by modem configuration
def hack_mem_list(outfname, infname, cfg):
    with open(infname, 'rb') as fh:
        indata = fh.read()

    pos = 0
    insize = len(indata)
    outdata = bytearray()
    while pos < insize:
        name, address, size, flags = struct.unpack(
            '20sIII', indata[pos:pos+32])
        pos += 32

        flags &= ~FLAG_LZMA_MASK
        rname = name.rstrip(b'\x00').decode('utf-8')
        for mbin in cfg:
            if mbin['file'] == rname:
                if 'lzma' in mbin:
                    flags |= FLAG_LZMA
                elif 'lzma2' in mbin:
                    flags |= FLAG_LZMA2
                elif 'lzma3' in mbin:
                    flags |= FLAG_LZMA3

        outdata += struct.pack(
            '20sIII', name, address, size, flags)

    with open(outfname, 'wb') as fh:
        fh.write(outdata)
    return


# Find FBD2 description by block device name
def parti_find_fbdev(parti, name):
    for desc in parti['descriptions']:
        if desc['type'] == 'FBD2' and desc['name'] == name:
            return desc
    return None


# Copy files, and return files in new directory
def copy_files(dst, fnames):
    nnames = []
    for f in fnames:
        nname = os.path.join(dst, os.path.basename(f))
        nnames.append(nname)
        shutil.copy(f, nname)
    return nnames


# Extract nvid from nvdata, and store to fname
def nvbin_extract(nvdata, eid, fname):
    pos = 4
    size = len(nvdata)
    while pos < size:
        nvid, nvsize = struct.unpack('HH', nvdata[pos:pos+4])
        if nvid == 0xffff:
            break

        if nvid == eid:
            with open(fname, 'wb') as fh:
                fh.write(nvdata[pos+4:pos+4+nvsize])
            return True

        nvsize = (nvsize + 3) & 0xfffffffc
        pos += 4 + nvsize
    return False


# Modify piece of data for specified nvid, in nvdata
def nvdata_modify(nvdata, nvid, offset, dlen, ddata):
    pos = 4
    size = len(nvdata)
    while pos < size:
        bin_nvid, nvsize = struct.unpack('HH', nvdata[pos:pos+4])
        if bin_nvid == 0xffff:
            break

        if bin_nvid == nvid:
            nvdata[pos+4+offset:pos+4+offset+dlen] = ddata
            return

        pos += 4 + nvsize
    raise Exception('nvid {} not found'.format(nvid))


# Apply deltanv.bin to nvdata.
# The format of deltanv.bin is multiple bulks. Each nulk:
# * file name: 20B
# * version: 2B
# * bulk size: 4B (total bulk)
# * multiple bulk data
#   * nvid: 2B
#   * offset: 2B
#   * size: 2B (data size plus 4)
#   * data
# * 0xffff: 2B
def nvbin_apply_delta(nvdata, fname):
    with open(fname, 'rb') as fh:
        delta = fh.read()

    size = len(delta)
    pos = 0
    while pos + 26 < size:
        _, dsize = struct.unpack('<HI', delta[pos+20:pos+26])
        dpos = pos + 26
        while dpos < pos + dsize - 2:
            nvid, offset, dlen = struct.unpack('HHH', delta[dpos:dpos+6])
            ddata = delta[dpos+6:dpos+6+dlen]
            # print("delta nvid {} offset {} len {}, at {}".format(nvid, offset, dlen, dpos))
            nvdata_modify(nvdata, nvid, offset, dlen, ddata)
            dpos += 6 + dlen
        pos = dpos + 2


# Add plain_file to partition
def pgen_add_file(pgen, local_name, fs_name):
    if 'plain_file' not in pgen:
        pgen['plain_file'] = []
    pgen['plain_file'].append({'file': fs_name, 'local_file': local_name})


# Add lzma3_file to partition
def pgen_add_lzma3(pgen, local_name, fs_name):
    if 'lzma3_file' not in pgen:
        pgen['lzma3_file'] = []
    pgen['lzma3_file'].append({'file': fs_name, 'local_file': local_name})


# Write file if changes, data is bytes
def write_file_if_change(fname, data):
    if os.path.exists(fname):
        with open(fname, 'rb') as fh:
            old_data = fh.read()
        if old_data == data:
            return

    ensure_dir(os.path.dirname(fname))
    with open(fname, 'wb') as fh:
        fh.write(data)


# Load prepack configuration
def prepack_cfg_from_json(cfg, srctop, bintop):
    with open(cfg, 'r') as fh:
        cfgtxt = fh.read()

    cfgdir = os.path.dirname(cfg)
    cfgtxt = cfgtxt.replace('@SOURCE_TOP_DIR@', srctop).replace(
        '@BINARY_TOP_DIR@', bintop)

    pc = json.loads(cfgtxt)
    prepackcfg = []
    for fm in pc.get('files', []):
        rname = fm['file'].replace('\\', '/').replace('//', '/').lstrip('/')
        fname = fm['local_file']
        if not os.path.isabs(fname):
            fname = os.path.join(cfgdir, fname)
        prepackcfg.append({'file': rname, 'local_file': fname})
    return prepackcfg


# Prepack local files
def file_of_prepack(pc):
    return [x['local_file'] for x in pc]


# Create prepack cpio
def create_prepack_cpio(pc, outfname):
    with open(outfname, 'wb') as fh:
        for fm in pc:
            name = fm['file'].encode('utf-8')
            name_size = len(name) + 1  # include NUL
            name_size_aligned = (name_size + 1) & ~1  # pad to even

            file_data = bytes()
            with open(fm['local_file'], 'rb') as ffh:
                file_data = ffh.read()
            file_size = len(file_data)
            file_size_aligned = (file_size + 1) & ~1  # pad to even

            fstat = os.stat(fm['local_file'])
            fh.write(struct.pack(
                CPIO_FILE_FORMAT.format(name_size_aligned, file_size_aligned),
                CPIO_OLDLE_MAGIC,
                fstat.st_dev & 0xffff,
                fstat.st_ino & 0xffff,
                fstat.st_mode & 0xffff,
                fstat.st_uid & 0xffff,
                fstat.st_gid & 0xffff,
                fstat.st_nlink & 0xffff,
                0,  # rdevice_num
                int(fstat.st_mtime) >> 16,
                int(fstat.st_mtime) & 0xffff,
                name_size & 0xffff,
                file_size >> 16,
                file_size & 0xffff,
                name,
                file_data))

        # When there are no files, write an empty file, and pcgen will
        # ignore PREPACK
        if len(pc) > 0:
            name = b'TRAILER!!!'
            name_size = len(name) + 1
            name_size_aligned = (name_size + 1) & ~1

            fh.write(struct.pack(
                CPIO_FILE_FORMAT.format(name_size_aligned, 0),
                CPIO_OLDLE_MAGIC,
                0, 0, 0, 0, 0,
                1, 0, 0, 0,
                name_size & 0xffff,
                0, 0,
                name,
                bytes()))


def mimggen8910_args(sub_parser):
    parser = sub_parser.add_parser(
        'imggen8910', help='generate dependency rule')
    parser.set_defaults(func=mimggen8910)
    parser.add_argument('--config', dest='config', required=True,
                        help='the json file for modem and nvm')
    parser.add_argument('--partinfo', dest='partinfo', required=True,
                        help='the json file for partition')
    parser.add_argument('--source-top', dest='srctop', required=True,
                        help='SOURCE_TOP_DIR, may be used')
    parser.add_argument('--binary-top', dest='bintop', required=True,
                        help='BINARY_TOP_DIR, may be used')
    parser.add_argument('--cproot', dest='cproot', required=True,
                        help='root directory of CP bin and nvm')
    parser.add_argument('--aproot', dest='aproot', required=True,
                        help='root directory of AP nvm')
    parser.add_argument('--fix-size', dest='fixsize', default=None,
                        help='fixnv size')
    parser.add_argument('--deltainc', dest='deltainc', default=None,
                        help='inlcude directory for deltanv')
    parser.add_argument('--dep', dest='dep', default=None,
                        help='output dependency files')
    parser.add_argument('--deprel', dest='deprel', default=None,
                        help='dependency target relative directory')
    parser.add_argument('--workdir', dest='workdir', required=True,
                        help='root directory of AP nvm')
    parser.add_argument('--prepackfile', dest='prepackfile', required=True,
                        help='the json file for prepack')
    parser.add_argument('variant', help='variant name')
    parser.add_argument('prepack', help='output prepack cpio file')
    parser.add_argument('nvbin', help='output nvitem bin')
    parser.add_argument('image', help='output modem image')


def mimggen8910(args):
    # Variant description json file name
    varjson = os.path.join(args.aproot, 'config', args.variant + '.json')

    with open(args.config, 'r') as fh:
        cpcfg = json.load(fh)
    with open(varjson, 'r') as fh:
        vari = json.load(fh)
    with open(args.partinfo, 'r') as fh:
        parti = json.load(fh)

    workdir = args.workdir
    cpdir = os.path.join(args.cproot, vari['cpdir'])
    apdir = os.path.join(args.aproot, vari['apdir'])
    cpdnvdir = os.path.join(cpdir, vari['cpdeltanv'])
    # Fixed file name by convention
    cpprj = os.path.join(cpdir, 'nvitem/nvitem_modem.prj')
    # Fixed file name by convention
    apprj = os.path.join(apdir, 'nvitem/nvitem.prj')
    dnvin = os.path.join(apdir, 'deltanv/deltanv_all.nv')
    cpfilecfg = cpcfg['cpfilelist'][vari['cpfilelist']]
    imsdir = os.path.join(cpdir, 'ims_delta_nv')
    cpdnvin = os.path.join(cpdnvdir, 'modem_config.nv')

    # Collect cp/ap nvm by parsing prj file
    cpnvms = nvm_from_prj(cpprj)
    apnvms = nvm_from_prj(apprj)
    imsnvms = glob.glob(os.path.join(imsdir, '*.nv'))
    imsxml = os.path.join(imsdir, 'Index.xml')

    # Collect prepack files
    prepack_json = args.prepackfile
    prepackcfg = prepack_cfg_from_json(prepack_json, args.srctop, args.bintop)
    prepackfiles = file_of_prepack(prepackcfg)
    create_prepack_cpio(prepackcfg, args.prepack)

    # Detect conflict MODULE in ap/cp
    apnvmmodules = [os.path.basename(nvm) for nvm in apnvms]
    cpnvmmodules = [os.path.basename(nvm) for nvm in cpnvms]
    conflict = [x for x in apnvmmodules if x in cpnvmmodules]
    if conflict:
        raise Exception('conflict nv modules: ' + conflict)

    # Collect cp files
    cpfiles = []
    for fm in cpfilecfg:
        fname = fm.get('local_file', fm['file'])
        fname = os.path.join(cpdir, fname)
        if fm.get('copy_only', False) and not os.path.exists(fname):
            continue
        cpfiles.append(fname)

    # Output dependency, if changed.
    # The dependency should use relative path. Otherwise, ninja will
    # always rebuild the target.
    if args.dep:
        deps = cpfiles + [cpprj] + cpnvms + [apprj] + \
            apnvms + [prepack_json] + prepackfiles + \
            imsnvms + [imsxml]
        deps.append(args.config)
        deps.append(args.partinfo)
        deps.extend(glob.glob(os.path.join(os.path.dirname(dnvin), '*.nv')))
        deps.extend(glob.glob(os.path.join(os.path.dirname(cpdnvin),'*.nv')))
        deps.append(__file__)

        # HACK: Some special characters can't be handled well by ninja.
        #       So, ignore them in dep. It will make dependency incomplete,
        #       and should ignore only if ninja can't handle it.
        def dep_ignore(fname):
            return '&' in fname

        deptarget = args.image
        if args.deprel:
            deptarget = os.path.relpath(deptarget, args.deprel)
        dep = '{}: {}\n'.format(deptarget, ' '.join(
            [os.path.relpath(x, args.deprel) for x in deps if not dep_ignore(x)]))
        write_file_if_change(args.dep, dep.encode('utf-8'))

    # copy cpbins, keep in output directory for easier debug
    # memory_index_list.bin will be hacked for flags
    cpbindir = os.path.join(workdir, 'cpbin')
    ensure_dir(cpbindir)
    for f in cpfiles:
        cpfile = os.path.basename(f)
        new_name = os.path.join(cpbindir, cpfile)
        if cpfile == 'memory_index_list.bin':
            org_name = new_name + '.org'
            shutil.copy(f, org_name)
            hack_mem_list(new_name, org_name, cpfilecfg)
        else:
            shutil.copy(f, new_name)

    # merge ap nvm and cp nvm
    nvmdir = os.path.join(workdir, 'nvm')
    ensure_dir(nvmdir)
    nvmprj = os.path.join(nvmdir, 'nvitem.prj')
    nvm_prj_merge(nvmprj, [apprj, cpprj])

    nvm_modules = modules_from_prj(nvmprj)
    ims_nv_exists = 'ims_nv.nvm' in nvm_modules

    # Generate nvitem.bin.
    subprocess.run(['NVGen', nvmprj, '-L'])
    nvmbin = nvbin_from_prj(nvmprj)

    # Read nvitem.bin
    with open(nvmbin, 'rb') as fh:
        nvmdata = fh.read()

    # Padding nvitem.bin
    if args.fixsize:
        fixsize = int(args.fixsize, 0)
        if len(nvmdata) > fixsize:
            raise Exception('nvbin exceed fixed size')

        nvmdata += b'\xff' * (fixsize - len(nvmdata))

    # Generate and apply deltanv. The output file name is fixed by NVGen
    dnvdir = os.path.join(workdir, 'deltanv')
    dnvfile = os.path.join(dnvdir, 'delta.nv')
    dnvbin = os.path.join(dnvdir, 'delta_nv.bin')
    ensure_dir(dnvdir)

    subprocess.run(['arm-none-eabi-gcc', '-E', '-P', '-x', 'c',
                    '-I', args.deltainc, dnvin, '-o', dnvfile])

    cpdnvin_exists = os.path.exists(cpdnvin)
    if cpdnvin_exists:
        cpdnvfile = os.path.join(dnvdir, 'cpdelta.nv')

        subprocess.run(['arm-none-eabi-gcc', '-E', '-P', '-x', 'c',
                        '-I', args.deltainc, cpdnvin, '-o', cpdnvfile])

    # Write empty Index.xml, to avoid annoying message from NVGen
    with open(os.path.join(dnvdir, 'Index.xml'), 'w') as fh:
        fh.write('<Operators version="1"/>\n')

    # Generate delta_nv.bin use all *.nv under dnvdir, it will merged according to the order of *.nv file name
    subprocess.run(['NVGen', nvmprj, '-L', '-c', dnvdir])

    nvmdata = bytearray(nvmdata)
    nvbin_apply_delta(nvmdata, dnvbin)

    # Write nvitem.bin to output
    ensure_dir(os.path.dirname(args.nvbin))
    with open(args.nvbin, 'wb') as fh:
        fh.write(nvmdata)

    # Split nvitem.bin for each nvid
    nvbindir = os.path.join(workdir, 'nvbin')
    ensure_dir(nvbindir)

    nvbins = []
    for fm in cpcfg['nvidfilelist']:
        nvid = int(fm['nvid'], 0)
        fname = os.path.join(nvbindir, fm['file'])
        if nvbin_extract(nvmdata, nvid, fname):
            nvbins.append(fname)

    # modem partition description
    pgen = {}
    for fm in cpfilecfg:
        if fm.get('copy_only', False):
            continue

        fname = fm['file']
        local_fname = fm.get('local_file', fname)
        if fm.get('lzma3', False):
            pgen_add_lzma3(pgen, os.path.join(cpbindir, local_fname), fname)
        else:
            pgen_add_file(pgen, os.path.join(cpbindir, local_fname), fname)

    for nv in nvbins:
        nvbase = os.path.basename(nv)
        pgen_add_file(pgen, nv, os.path.join(cpcfg['modemnv_dir'], nvbase))

    # Generate ims_delta_nv.bin
    if ims_nv_exists:
        imsgendir = os.path.join(workdir, 'ims_delta_nv')
        ensure_dir(imsgendir)
        for f in imsnvms:
            shutil.copy(f, os.path.join(imsgendir, os.path.basename(f)))
        shutil.copy(imsxml, os.path.join(imsgendir, os.path.basename(imsxml)))
        subprocess.run(['NVGen', nvmprj, '-L', '-c', imsgendir],
                       stdout=subprocess.DEVNULL)

        pgen_add_file(pgen, os.path.join(imsgendir, 'delta_nv.bin'),
                      'nvm/ims_delta_nv.bin')

    # generate modem image json
    fbdevdesc = parti_find_fbdev(parti, cpcfg['fbdev_name'])
    mgen = {}
    mgen['type'] = fbdevdesc['type']
    mgen['offset'] = fbdevdesc['offset']
    mgen['size'] = fbdevdesc['size']
    mgen['erase_block'] = fbdevdesc['erase_block']
    mgen['logic_block'] = fbdevdesc['logic_block']
    mgen['partiton'] = []
    mgen['partiton'].append(pgen)

    mgenfile = os.path.join(workdir, 'modem.json')
    with open(mgenfile, 'w') as fh:
        json.dump(mgen, fh, indent=4)

    # Run flash block device generator
    subprocess.run(['dtools', 'fbdevgen', mgenfile, args.image])
    return 0


def mnvgen8811_args(sub_parser):
    parser = sub_parser.add_parser(
        'nvgen8811', help='generate dependency rule')
    parser.set_defaults(func=mnvgen8811)
    parser.add_argument('--source-top', dest='srctop', required=True,
                        help='SOURCE_TOP_DIR, may be used')
    parser.add_argument('--binary-top', dest='bintop', required=True,
                        help='BINARY_TOP_DIR, may be used')
    parser.add_argument('--aproot', dest='aproot', required=True,
                        help='root directory of AP nvm')
    parser.add_argument('--fix-size', dest='fixsize', default=None,
                        help='fixnv size')
    parser.add_argument('--deltainc', dest='deltainc', default=None,
                        help='inlcude directory for deltanv')
    parser.add_argument('--dep', dest='dep', default=None,
                        help='output dependency files')
    parser.add_argument('--deprel', dest='deprel', default=None,
                        help='dependency target relative directory')
    parser.add_argument('--workdir', dest='workdir', required=True,
                        help='root directory of AP nvm')
    parser.add_argument('--prepackfile', dest='prepackfile', required=True,
                        help='the json file for prepack')
    parser.add_argument('variant', help='variant name')
    parser.add_argument('prepack', help='output prepack cpio file')
    parser.add_argument('nvbin', help='output nvitem bin')


def mnvgen8811(args):
    workdir = args.workdir
    # Fixed file name by convention
    apdir = os.path.join(args.aproot, args.variant)
    apprj = os.path.join(apdir, 'nvitem/nvitem.prj')
    dnvin = os.path.join(apdir, 'deltanv/deltanv_all.nv')

    # Collect cp/ap nvm by parsing prj file
    apnvms = nvm_from_prj(apprj)

    # Collect prepack files
    prepack_json = args.prepackfile
    prepackcfg = prepack_cfg_from_json(prepack_json, args.srctop, args.bintop)
    prepackfiles = file_of_prepack(prepackcfg)
    create_prepack_cpio(prepackcfg, args.prepack)

    # Output dependency, if changed.
    # The dependency should use relative path. Otherwise, ninja will
    # always rebuild the target.
    if args.dep:
        deps = [apprj] + apnvms + [prepack_json] + prepackfiles
        deps.extend(glob.glob(os.path.join(os.path.dirname(dnvin), '*.nv')))
        deps.append(__file__)

        deptarget = args.nvbin
        if args.deprel:
            deptarget = os.path.relpath(deptarget, args.deprel)
        dep = '{}: {}\n'.format(deptarget, ' '.join(
            [os.path.relpath(x, args.deprel) for x in deps]))
        write_file_if_change(args.dep, dep.encode('utf-8'))

    # Merge ap nvm and cp nvm
    nvmdir = os.path.join(workdir, 'nvm')
    ensure_dir(nvmdir)
    nvmprj = os.path.join(nvmdir, 'nvitem.prj')
    shutil.copyfile(apprj, nvmprj)
    copy_files(nvmdir, apnvms)

    # Generate nvitem.bin.
    subprocess.run(['NVGen', nvmprj, '-L'])
    nvmbin = nvbin_from_prj(nvmprj)

    # Read nvitem.bin
    with open(nvmbin, 'rb') as fh:
        nvmdata = fh.read()

    # Padding nvitem.bin
    if args.fixsize:
        fixsize = int(args.fixsize, 0)
        if len(nvmdata) > fixsize:
            raise Exception('nvbin exceed fixed size')

        nvmdata += b'\xff' * (fixsize - len(nvmdata))

    # Generate and apply deltanv. The output file name is fixed by NVGen
    dnvdir = os.path.join(workdir, 'deltanv')
    dnvfile = os.path.join(dnvdir, 'delta.nv')
    dnvbin = os.path.join(dnvdir, 'delta_nv.bin')
    ensure_dir(dnvdir)

    subprocess.run(['arm-none-eabi-gcc', '-E', '-P', '-x', 'c',
                    '-I', args.deltainc, dnvin, '-o', dnvfile])

    # Write empty Index.xml, to avoid annoying message from NVGen
    with open(os.path.join(dnvdir, 'Index.xml'), 'w') as fh:
        fh.write('<Operators version="1"/>\n')

    subprocess.run(['NVGen', nvmprj, '-L', '-c', dnvdir])

    nvbin_apply_delta(nvmdata, dnvbin)

    # Write nvitem.bin to output
    ensure_dir(os.path.dirname(args.nvbin))
    with open(args.nvbin, 'wb') as fh:
        fh.write(nvmdata)

    return 0

def mprepack_args(sub_parser):
    parser = sub_parser.add_parser(
        'prepackgen', help='generate prepack cpio')
    parser.set_defaults(func=mprepackgen)
    parser.add_argument('--source-top', dest='srctop', required=True,
                        help='SOURCE_TOP_DIR, may be used')
    parser.add_argument('--binary-top', dest='bintop', required=True,
                        help='BINARY_TOP_DIR, may be used')
    parser.add_argument('--prepackfile', dest='prepackfile', required=True,
                        help='the json file for prepack')
    parser.add_argument('prepack', help='output prepack cpio file')

def mprepackgen(args):
    prepack_json = args.prepackfile
    prepackcfg = prepack_cfg_from_json(prepack_json, args.srctop, args.bintop)
    prepackfiles = file_of_prepack(prepackcfg)
    create_prepack_cpio(prepackcfg, args.prepack)

def main(argv):
    parser = argparse.ArgumentParser(description=DESCRIPTION,
                                     formatter_class=argparse.RawTextHelpFormatter)
    sub_parser = parser.add_subparsers(help='sub-commnads')

    mimggen8910_args(sub_parser)
    mnvgen8811_args(sub_parser)
    mprepack_args(sub_parser)

    args = parser.parse_args(argv)
    if args.__contains__('func'):
        return args.func(args)

    parser.parse_args(['-h'])
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
