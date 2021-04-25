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


def main(argv):
    parser = argparse.ArgumentParser(usage='change or add copyright header')
    parser.add_argument('--force', dest='force_mode', choices=['c', 'sharp'],
                        help='force file mode')
    parser.add_argument('fnames', nargs='+', help='file names')

    args = parser.parse_args(argv)

    copy_c = '''/* Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
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
    copy_s = '''# Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
# All rights reserved.
#
# This software is supplied "AS IS" without any warranties.
# RDA assumes no responsibility or liability for the use of the software,
# conveys no license or title under any patent, copyright, or mask work
# right to the product. RDA reserves the right to make changes in the
# software without notification.  RDA also make no representation or
# warranty that such application will be suitable for the specified use
# without further testing or modification.

'''

    for fname in args.fnames:
        with open(fname, 'r') as fh:
            lines = fh.readlines()

        hashbang = None
        detect_c = False
        detect_s = False

        if lines and lines[0].startswith('#!'):
            hashbang = lines[0]
            lines = lines[1:]

        while lines:
            if lines[0].strip() == '':
                lines = lines[1:]
            else:
                break

        if lines:
            if lines[0].startswith('/* Copyright') and 'RDA' in lines[0]:
                detect_c = True
                while lines:
                    line = lines[0]
                    lines = lines[1:]
                    if line.strip() == '*/':
                        break
            elif lines[0].startswith('# Copyright') and 'RDA' in lines[0]:
                detect_s = True
                while lines:
                    line = lines[0]
                    if not line.startswith('#'):
                        break
                    lines = lines[1:]

        while lines:
            if lines[0].strip() == '':
                lines = lines[1:]
            else:
                break

        mode = None
        if args.force_mode:
            mode = args.force_mode
        elif detect_c:
            mode = 'c'
        elif detect_s:
            mode = 'sharp'
        else:
            print('{}: unknown file type, ignored'.format(fname))
            continue

        if mode == 'c' and detect_s:
            print('{}: unlike to be c, ignored'.format(fname))
            continue
        if mode == 'sharp' and detect_c:
            print('{}: unlike to use #, ignored'.format(fname))
            continue

        if mode == 'c':
            with open(fname, 'w') as fh:
                fh.write(copy_c)
                fh.write(''.join(lines))
        else:
            with open(fname, 'w') as fh:
                if hashbang:
                    fh.write(hashbang)
                fh.write(copy_s)
                fh.write(''.join(lines))

    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
