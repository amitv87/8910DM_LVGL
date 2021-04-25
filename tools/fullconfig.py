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

import kconfiglib

DESCRIPTION = """
Convert a configuration file to "full" configuration file.
"""

def main():
    parser = argparse.ArgumentParser(description=DESCRIPTION)

    parser.add_argument(
        "--kconfig",
        metavar="KCONFIG_FILENAME",
        dest="kconfig",
        default="Kconfig",
        help="top-level Kconfig file (default: Kconfig)")

    parser.add_argument(
        "config",
        metavar="CONFIG_FILENAME",
        nargs='+',
        help="configuration file name")

    args = parser.parse_args()
    kconf = kconfiglib.Kconfig(args.kconfig)
    for config in args.config:
        kconf.load_config(config)
        kconf.write_config(config)

if __name__ == "__main__":
    main()
