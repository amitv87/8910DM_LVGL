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
import io
import os

DESCRIPTION = """
generate C defines for symbols.

To void name conflict, it may be needed to change the symbol names
in some modules. This script will parse a symbol list, and generate
C header file, to define each symbol with a prefix.
"""


def main(argv):
    parser = argparse.ArgumentParser(description=DESCRIPTION)
    parser.add_argument("sym", help="symbol list file")
    parser.add_argument("prefix", help="prefix to be added")
    parser.add_argument("header", help="output header file")
    args = parser.parse_args(argv)

    symbs = []
    with open(args.sym, 'r') as fh:
        for line in fh.readlines():
            if line.startswith('#'):
                continue
            line = line.strip()
            if not line:
                continue
            if line not in symbs:
                symbs.append(line)

    prot = '_{}_'.format(os.path.basename(args.header).upper().replace('.', '_'))
    with open(args.header, 'w') as fh:
        fh.write('#ifndef {}\n'.format(prot))
        fh.write('#define {}\n'.format(prot))
        fh.write('// Auto generated. Don\'t edit it manually!\n')
        for sym in symbs:
            fh.write('#define {} {}{}\n'.format(sym, args.prefix, sym))
        fh.write('#endif\n')

    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
