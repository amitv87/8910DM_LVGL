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
import os


def main(argv):
    parser = argparse.ArgumentParser(
        description='generate group linker script')
    parser.add_argument('--base', dest='base', default=None,
                        help='base directory of libraries')
    parser.add_argument('output', help='output file name')
    parser.add_argument('libs', nargs='+',
                        help='library file names')

    args = parser.parse_args(argv)

    libs = args.libs
    if args.base:
        libs = [os.path.relpath(x, args.base) for x in libs]

    with open(args.output, 'w') as fh:
        fh.write('GROUP(\n' + '\n'.join(libs) + ')\n')
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
