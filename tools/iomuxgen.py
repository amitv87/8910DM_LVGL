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
import csv
import re
import json
import os

"""
JSON configuration file format:

csv: the chip pinmux csv, related to json file directory
init_fun: a list of functions to be initialized
fun_prop: key is function, values:
    pull: default (can be omit), pull_none, pull_down,
          pull_up (8955/8909), pull_up_3, pull_up_2, pull_up_1
    inout: default (can be omit), input, output_hi, output_lo
    driving: string, 0~3 or 0~15, default is 2
fun_def_pad: key is function, value is pad
"""

DESCRIPTION = """
Generate iomux source files from csv and json
"""

COPYRIGHT = '''/* Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
 * All rights reserved.
 *
 * This software is supplied "AS IS" without any warranties.
 * RDA assumes no responsibility or liability for the use of the software,
 * conveys no license or title under any patent, copyright, or mask work
 * right to the product. RDA reserves the right to make changes in the
 * software without notification.  RDA also make no representation or
 * warranty that such application will be suitable for the specified use
 * without further testing or modification.
 */

'''

INOUT_VAL = {'default': 'HAL_IOMUX_INOUT_DEFAULT',
             'input': 'HAL_IOMUX_FORCE_INPUT',
             'output': 'HAL_IOMUX_FORCE_OUTPUT',
             'output_hi': 'HAL_IOMUX_FORCE_OUTPUT_HI',
             'output_lo': 'HAL_IOMUX_FORCE_OUTPUT_LO'}
PULL_VAL = {'default': 'HAL_IOMUX_PULL_DEFAULT',
            'pull_none': 'HAL_IOMUX_FORCE_PULL_NONE',
            'pull_down': 'HAL_IOMUX_FORCE_PULL_DOWN',
            'pull_up_3': 'HAL_IOMUX_FORCE_PULL_UP_3',
            'pull_up_2': 'HAL_IOMUX_FORCE_PULL_UP_2',
            'pull_up_1': 'HAL_IOMUX_FORCE_PULL_UP_1'}


def sort_nice(l):
    def convert(text):
        return int(text) if text.isdigit() else text

    def alphanum_key(key):
        return [convert(c) for c in re.split('([0-9]+)', key)]

    l.sort(key=alphanum_key)


def output_defpad(fun_def_pad, fname):
    cfg = {}
    cfg['fun_def_pad'] = {}
    for fun, pad in fun_def_pad.items():
        lfun = fun.lower()
        lpad = pad.lower()
        cfg['fun_def_pad'][lfun] = lpad

    with open(fname, 'w') as fh:
        json.dump(cfg, fh, indent=4, sort_keys=True)


def main(argv):
    parser = argparse.ArgumentParser(description=DESCRIPTION)

    parser.add_argument('--chip', dest='chip', required=True,
                        help='chip, list for all')
    parser.add_argument('--def-pad', dest='defpad',
                        help='output non-unique default pad')
    parser.add_argument('json', help='project fun mux configuration')
    parser.add_argument("pincfgin", help="template of hal_iomux_pincfg.c")
    parser.add_argument("pincfg", help="output hal_iomux_pincfg.c")
    parser.add_argument("pindef", nargs='?',
                        help="output hal_iomux_pindef.h")

    args = parser.parse_args()

    if args.chip == 'list':
        print('supported chip: ', ['8910', '8811', '8955', '8909'])
        return 0

    with open(args.pincfgin, 'r') as fh:
        pincfgin = fh.read()

    pads = []  # all pads
    funs = []  # all functions
    pad_idx = {}  # pad -> index
    fun_idx = {}  # fun -> index
    pad_reg = {}  # pad -> reg
    fun_pads = {}  # fun -> [pads]
    pad_funs = {}  # pad -> [funs]
    fun_pad_sel = {}  # fun,pad -> reg,sel
    fun_def_pad = {}  # fun -> default pad
    init_fun = []  # init funs
    fun_prop = {}  # fun -> {props}

    with open(args.json, 'r') as fh:
        pcfg = json.load(fh)

    for fun in pcfg.get('init_fun', []):
        init_fun.append(fun.upper())

    for fun in pcfg.get('fun_prop', {}):
        fun_prop[fun.upper()] = dict(pcfg['fun_prop'][fun])

    # cvs is related to directory of json file
    with open(os.path.join(os.path.dirname(args.json), pcfg['csv']), 'r') as fh:
        pinmux = csv.reader(fh)
        for row in pinmux:
            if not row[0]:  # title row
                continue

            reg = row[0]
            pad = row[1].upper()

            pad_reg[pad] = reg
            if pad not in pads:
                pads.append(pad)
            if pad not in pad_funs:
                pad_funs[pad] = []

            n = -1
            for fun in row[2:]:
                n += 1
                if not fun:
                    continue
                if fun.startswith('x--'):
                    continue

                fun = fun.upper()

                if fun not in funs:
                    funs.append(fun)
                if fun not in fun_pads:
                    fun_pads[fun] = []
                if fun not in fun_prop:
                    fun_prop[fun] = {}

                if fun in pad_funs[pad]:
                    continue

                fun_pads[fun].append(pad)
                pad_funs[pad].append(fun)
                fun_pad_sel[(fun, pad)] = (reg, n)

    sort_nice(funs)
    sort_nice(pads)

    n = 1
    for fun in funs:
        fun_idx[fun] = n
        n += 1
    n = 1
    for pad in pads:
        pad_idx[pad] = n
        n += 1

    # sort
    for fun in funs:
        fun_pads[fun].sort(key=lambda el: pad_idx[el])
    for pad in pads:
        pad_funs[pad].sort(key=lambda el: fun_idx[el])

    # default pad from json
    for lfun in pcfg.get('fun_def_pad', {}):
        lpad = pcfg['fun_def_pad'][lfun]
        fun_def_pad[lfun.upper()] = lpad.upper()

    # when there is only one pad for fun, it is the default pad
    for fun in funs:
        if len(fun_pads[fun]) == 1:
            fun_def_pad[fun] = fun_pads[fun][0]

    # output default pads if needed
    if args.defpad:
        out_def_pad = dict(filter(lambda el: fun_pads[el[0]] != 1,
                                  fun_def_pad.items()))
        output_defpad(out_def_pad, args.defpad)
        return 0

    # output pindef if needed
    if args.pindef:
        with open(args.pindef, 'w') as fh:
            fh.write(COPYRIGHT)
            fh.write('#ifndef _HAL_IOMUX_PIN_H_\n')
            fh.write('#define _HAL_IOMUX_PIN_H_\n')
            fh.write('\n// Auto generated. Don\'t edit it manually!\n')

            fh.write('\n// all functions\n')
            for fun in funs:
                fh.write('#define HAL_IOMUX_FUN_{fun} {n} // 0x{n:x}\n'.
                         format(fun=fun, n=fun_idx[fun]))
            fh.write('#define HAL_IOMUX_FUN_COUNT {}\n'.format(len(funs)))

            fh.write('\n// all pads\n')
            for pad in pads:
                fh.write('#define HAL_IOMUX_PAD_{pad} ({n} << 12) // 0x{n:x}\n'.
                         format(pad=pad, n=pad_idx[pad]))
            fh.write('#define HAL_IOMUX_PAD_COUNT {}\n'.format(len(pads)))

            fh.write('\n// function with specified pad\n')
            for fun in funs:
                for pad in fun_pads[fun]:
                    _, sel = fun_pad_sel[(fun, pad)]
                    val = (sel << 24) | pad_idx[pad] << 12 | fun_idx[fun]
                    fh.write('#define HAL_IOMUX_FUN_{fun}_PAD_{pad} 0x{n:x}\n'.
                             format(fun=fun, pad=pad, n=val))

            fh.write('\n#endif\n')

    fun_prop_configs = []
    pad_prop_configs = []
    init_pad_configs = []
    fun_pad_sel_configs = []
    fun_name_configs = []
    pad_name_configs = []

    def gen_fun_prop(prop):
        inout = fun_prop[fun].get('inout', 'default')
        pull = fun_prop[fun].get('pull', 'default')
        driving = fun_prop[fun].get('driving', '2')
        return INOUT_VAL[inout], PULL_VAL[pull], driving

    def gen_pad_prop_8910(pad, reg):
        if reg == 'NULL':
            return '{{/*{pad}*/ NULL, NULL, 0, 0}}'.format(pad=pad)

        iomux, ana, offset, pull, _ = reg.split(':')
        iomux_reg = 'hwp_iomux->{}'.format(iomux)
        ana_reg = 'hwp_analogReg->{}'.format(ana)
        pad_type = 'PAD_TYPE_SPU' if pull == 'spu' else 'PAD_TYPE_WPUS'
        tmpl = '{{/*{pad}*/ &{iomux_reg}, &{ana_reg}, {offset}, {pad_type}}}'
        return tmpl.format(pad=pad, iomux_reg=iomux_reg, ana_reg=ana_reg,
                           offset=offset, pad_type=pad_type)

    def gen_pad_prop_reg(pad, reg):
        if reg == 'NULL':
            return '{{/*{pad}*/ NULL}}'.format(pad=pad)

        tmpl = '{{/*{pad}*/ &{reg}}}'
        return tmpl.format(pad=pad, reg=reg)

    def gen_pad_prop(pad, reg):
        if args.chip == '8910':
            return gen_pad_prop_8910(pad, reg)
        return gen_pad_prop_reg(pad, reg)

    # pad propperties, must be complete list
    for pad in pads:
        reg = pad_reg[pad]
        pad_prop_configs.append(gen_pad_prop(pad, reg))

    # fun properties, must be complete list
    for fun in funs:
        inout, pull, driving = gen_fun_prop(fun_prop[fun])
        if fun in fun_def_pad:
            def_pad = fun_def_pad[fun]
            _, sel = fun_pad_sel[(fun, def_pad)]
            fun_prop_configs.append('{{/*{fun}*/ PAD_INDEX(HAL_IOMUX_PAD_{def_pad}), {sel}, {inout}, {pull}, {driving}}}'
                                    .format(fun=fun, def_pad=def_pad, sel=sel, inout=inout,
                                            pull=pull, driving=driving))
        else:
            fun_prop_configs.append('{{/*{fun}*/ 0, 0, {inout}, {pull}, {driving}}}'
                                    .format(fun=fun, inout=inout,
                                            pull=pull, driving=driving))

    # init fun for generation
    for fun in init_fun:
        if fun not in fun_def_pad:
            raise Exception('{} without default pad'.format(fun))
        pad = fun_def_pad[fun]
        reg, sel = fun_pad_sel[(fun, pad)]
        inout, pull, driving = gen_fun_prop(fun_prop[fun])
        init_pad_configs.append('{{PAD_INDEX(HAL_IOMUX_PAD_{pad}), {sel} /*{fun}*/, {inout}, {pull}, {driving}}}'
                                .format(fun=fun, pad=pad, sel=sel, inout=inout,
                                        pull=pull, driving=driving))

    # all fun/pad combination
    for fun in funs:
        for pad in fun_pads[fun]:
            fun_pad_sel_configs.append('HAL_IOMUX_FUN_{fun}_PAD_{pad}'
                                       .format(fun=fun, pad=pad))

    # fun string name
    for fun in funs:
        fun_name_configs.append('"{fun}"'.format(fun=fun))

    # pad string name
    for pad in pads:
        pad_name_configs.append('"{pad}"'.format(pad=pad))

    with open(args.pincfg, 'w') as fh:
        fh.write(pincfgin.format(
            fun_pad_sel_config=',\n    '.join(fun_pad_sel_configs),
            fun_name_config=',\n    '.join(fun_name_configs),
            pad_name_config=',\n    '.join(pad_name_configs),
            fun_prop_config=',\n    '.join(fun_prop_configs),
            pad_prop_config=',\n    '.join(pad_prop_configs),
            init_pad_config=',\n    '.join(init_pad_configs)))
        return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
