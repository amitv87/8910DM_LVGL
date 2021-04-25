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
import csv

DESCRIPTION = '''
Generate keypad map source file from csv
'''


def main():
    parser = argparse.ArgumentParser(description=DESCRIPTION)
    parser.add_argument("csv", help="csv file for keypad map")
    parser.add_argument("keypaddef", nargs="?", default='drv_keypad_def.h')
    args = parser.parse_args()

    pins = []
    with open(args.csv, "r") as fh:
        pinmap = csv.reader(fh)
        key_row = -1
        for col in pinmap:
            if not col[0]:
                continue

            key_row += 1
            key_col = -1
            for pin in col[1:]:
                key_col += 1
                if not pin:
                    continue

                pins.append('{SCAN_ROW_COL(%d, %d), %s}' %
                            (key_row, key_col, pin))

    with open(args.keypaddef, "w") as fh:
        fh.write('//  Auto generated. Don\'t edit it manually!\n\n')
        fh.write('static const drvKeypadScanMap_t gKeyMatrix[] =\n{\n    ')
        fh.write(',\n    '.join(pins))
        fh.write(",\n};\n")


if __name__ == "__main__":
    main()
