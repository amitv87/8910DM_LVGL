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

DESCRIPTION = """
Generate nor flash properties from csv
"""


def main():
    parser = argparse.ArgumentParser(description=DESCRIPTION)

    parser.add_argument("csv",
                        help="csv file for nor flash properties")
    parser.add_argument("prop",
                        help="output nor flash properties file")

    args = parser.parse_args()

    propnames = []
    props = []
    with open(args.csv, 'r') as fh:
        for row in csv.reader(fh):
            if not row[0] and not row[1]:  # maybe empty line
                continue

            if not row[0]:  # title
                propnames = row[1:]
                continue

            prop = {'name': row[0]}
            for n in range(len(propnames)):
                prop[propnames[n]] = row[n+1]
            props.append(prop)

    with open(args.prop, 'w') as fh:
        for prop in props:
            fh.write('{{ // {}\n'.format(prop['name']))
            for pname in propnames:
                fh.write("    .{} = {},\n".format(pname, prop[pname]))
            fh.write('},\n')

    return 0


if __name__ == "__main__":
    main()
