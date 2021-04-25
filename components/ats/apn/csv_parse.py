#!/usr/bin/env python3

import argparse
import csv
import sys

def clearRepeatedPlmn(originalList, index):
    for i in range(index, len(originalList) - 1):
        if originalList[i][0] == originalList[i+1][0]:
            originalList.remove(originalList[i+1])
            return True
    return False

def takeFirst(elem):
    return elem[0]

def readApn(argv):
    parser = argparse.ArgumentParser(
        usage='convert apn information to h')

    parser.add_argument('input', help='input csv file')
    parser.add_argument('output', help='output header file')

    args = parser.parse_args(argv)

    with open(args.input,"r",encoding="mac_roman") as fr:
        reader = csv.reader(fr)
        list1 = []
        for line in reader:
            if not line[0]:
                continue
            if line[6] != '0':
                continue
            if len(line[5]) == 1:
                list1.append([int(line[4] + '0' + line[5]), line[7], line[12], line[13]])
            else:
                list1.append([int(line[4] + line[5]), line[7], line[12], line[13]])
        list1.sort(key=takeFirst)
        i = 0
        while i < len(list1):
            returnValue = clearRepeatedPlmn(list1, i)
            if returnValue == True:
                continue
            else:
                i = i + 1

        with open(args.output, 'w') as fw:
            fw.write('#include "ats_config.h"\n')
            fw.write('#include "at_engine.h"\n')
            fw.write('#include "at_command.h"\n')
            fw.write('\n')
            fw.write('typedef struct _MOBILE_NETWORK_APNS\n')
            fw.write('{\n')
            fw.write('    const uint32_t plmn;\n')
            fw.write('    const char *apn;\n')
            fw.write('    const char *username;\n')
            fw.write('    const char *password;\n')
            fw.write('} MOBILE_NETWORK_APNS;\n')
            fw.write('\n')
            fw.write('const MOBILE_NETWORK_APNS mobileNetworkApns[] =\n')
            fw.write('{\n')
            for i in range(len(list1)):
                fw.write('    {' + str(list1[i][0]) + ', "' + list1[i][1] + '", "' + list1[i][2] + '", "' + list1[i][3] + '"},\n')
            fw.write('};\n')
            fw.close()

if __name__ == "__main__":
    sys.exit(readApn(sys.argv[1:]))