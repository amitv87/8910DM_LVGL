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
import binascii

DESCRIPTION = """
Calculate CRC32 of files
"""


def main():
    parser = argparse.ArgumentParser(description=DESCRIPTION)

    parser.add_argument(
        "fnames",
        metavar="FILE_NAME",
        nargs="+",
        help="file names to calculate CRC32")

    args = parser.parse_args()

    for fname in args.fnames:
        with open(fname, 'rb') as fh:
            data = fh.read()

        crc = binascii.crc32(data)
        print('%08x %s' % (crc, fname))


if __name__ == "__main__":
    main()
