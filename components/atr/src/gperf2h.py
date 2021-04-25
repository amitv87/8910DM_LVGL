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
import io
import argparse
import subprocess


def main(argv):
    parser = argparse.ArgumentParser(
        usage='convert at command table gperf to h')

    parser.add_argument('input', help='input gperf file')
    parser.add_argument('output', help='output header file')

    args = parser.parse_args(argv)

    funcs = []
    inkeys = False
    with open(args.input, 'r') as fh:
        for line in fh.readlines():
            sline = line.strip()
            if sline.startswith('%%'):
                inkeys = not inkeys
            elif sline.startswith('//'):
                continue
            elif sline.startswith('#'):
                continue
            elif sline == '':
                continue
            elif inkeys:
                f = sline.split(',')
                if len(f) >= 2:
                    funcs.append(f[1].strip())

    proc = subprocess.Popen(
        ['gperf', '-m10', args.input], stdout=subprocess.PIPE)
    out, _ = proc.communicate()

    with open(args.output, 'w') as fh:
        fh.write('DECLARE_CMD_HANDLER(atCmdHandleAT);\n')
        for f in funcs:
            fh.write('DECLARE_CMD_HANDLER({});\n'.format(f))
        fh.write('\n'.join(out.decode('utf-8').splitlines()))
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
