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
import struct
import json
import binascii

DESCRIPTION = """
Generate binary partition information file from json.

Configuration json format:
* version: it should be 0x100 now
* descriptions (array)
  * flash block device
    * type: FBD1 or FBD2
    * flash: SFL1 or SFL2 (for external flash)
    * name: device name, will be referred later
    * offset: flash offset
    * size: block device size in byte
    * erase_block: erase block size
    * logic_block: logic block size
    * bootloader_ignore: (optional) flag for ignore by bootloader
    * application_ignore: (optional) flag for ignore by application
  * SFFS file system
    * type: SFFS
    * device: block device or partition name
    * mount: mount point
    * reserve_block: (optional) safe file reserved block count
    * bootloader_ignore: (optional) flag for ignore by bootloader
    * application_ignore: (optional) flag for ignore by application
    * bootloader_ro: (optional) flag for read only by bootloader
    * application_ro: (optional) flag for read only by application
* macros: it will only be used to generate partinfo.cmake for definitions
          all keys start with "CONFIG_" will be written to cmake.
"""

MAGIC = b'PRTI'
VERSION = 0x100
HEADER_SIZE = 16
BOOTLOADER_RO = 0x01
APPLICATION_RO = 0x02
BOOTLOADER_IGNORE = 0x04
APPLICATION_IGNORE = 0x08


class Partinfo(object):

    def __init__(self):
        self._des = []
        pass

    def loadjson(self, fname):
        fh = open(fname, "r")
        self._des = json.load(fh)
        fh.close()

    def storebin(self, fname):
        data = bytearray()
        for p in self._des['descriptions']:
            dtype = p['type']
            if dtype == 'FBD1' or dtype == 'FBD2':
                flags = 0
                if 'bootloader_ignore' in p:
                    flags |= BOOTLOADER_IGNORE
                if 'application_ignore' in p:
                    flags |= APPLICATION_IGNORE

                pdata = struct.pack(
                    '4s4s4siiiii',
                    bytes(dtype, 'utf-8'),
                    bytes(p['flash'], 'utf-8'),
                    bytes(p['name'], 'utf-8'),
                    int(p['offset'], 0),
                    int(p['size'], 0),
                    int(p['erase_block'], 0),
                    int(p['logic_block'], 0),
                    flags)
                data += pdata

            elif dtype == 'BPRT':
                flags = 0
                if 'bootloader_ignore' in p:
                    flags |= BOOTLOADER_IGNORE
                if 'application_ignore' in p:
                    flags |= APPLICATION_IGNORE

                pdata = struct.pack(
                    '4s4s4siii',
                    bytes(dtype, 'utf-8'),
                    bytes(p['device'], 'utf-8'),
                    bytes(p['name'], 'utf-8'),
                    int(p['offset'], 0),
                    int(p['count'], 0),
                    flags)
                data += pdata

            elif dtype == 'SFFS':
                flags = 0
                if 'bootloader_ro' in p:
                    flags |= BOOTLOADER_RO
                if 'application_ro' in p:
                    flags |= APPLICATION_RO
                if 'bootloader_ignore' in p:
                    flags |= BOOTLOADER_IGNORE
                if 'application_ignore' in p:
                    flags |= APPLICATION_IGNORE

                reserve_block = 0
                if 'reserve_block' in p:
                    reserve_block = int(p['reserve_block'], 0)

                pdata = struct.pack(
                    '4s4s64sii',
                    bytes(dtype, 'utf-8'),
                    bytes(p['device'], 'utf-8'),
                    bytes(p['mount'], 'utf-8'),
                    reserve_block,
                    flags)
                data += pdata

        size = HEADER_SIZE + len(data)
        version = struct.pack('ii', size, VERSION)
        crc = binascii.crc32(version + data)

        header = struct.pack('4sIii', MAGIC, crc, size, VERSION)

        if not os.path.exists(os.path.dirname(fname)):
            os.makedirs(os.path.dirname(fname))

        fh = open(fname, 'wb')
        fh.write(header)
        fh.write(data)
        fh.close()

    def storecmake(self, fname, json_fname):
        if not os.path.exists(os.path.dirname(fname)):
            os.makedirs(os.path.dirname(fname))

        fh = open(fname, "w")
        fh.write("# Auto Generated\n")
        kv = self._des.get("macros", {})
        for k, v in kv.items():
            if k.startswith("CONFIG_"):
                fh.write("set(%s %s)\n" % (k, v))
        fh.close()


def main():
    parser = argparse.ArgumentParser(description=DESCRIPTION,
                                     formatter_class=argparse.RawTextHelpFormatter)

    parser.add_argument("json", help="partinfo.json for partition information")
    parser.add_argument("bin", help="partinfo.bin for partition information")
    parser.add_argument("cmake", help="partinfo.cmake for definitions")
    args = parser.parse_args()

    parti = Partinfo()
    parti.loadjson(args.json)
    parti.storebin(args.bin)
    parti.storecmake(args.cmake, args.json)


if __name__ == "__main__":
    main()
