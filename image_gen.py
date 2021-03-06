#!/usr/bin/python

# _*_ coding: utf-8 _*_
# @Time    :   2020/12/24 11:03:57
# @FileName:   image_gen.py
# @Author  :   Wang Hua(whfyzg@gmail.com)
# @Software:   VSCode
# @Descripton: Script to generate LVGL image file from csv file

# @Time    :   2021/04/12 10:18:33
# @FileName:   image_gen.py
# @Author  :   Hobby(Modify)
# @Software:   PyCharm
# @Descripton: Script to generate LVGL image file from csv file

import hashlib
import json
import csv
import os
import os.path
import random
import sys
from os import path
from optparse import OptionParser

php_path=r".\\tools\\php\\php.exe "
lvglUtilPath=r".\\tools\\lvgl\\lv_utils\\img_conv_core.php"

def Main():
    outDir=None
    csvFile=None
    Iformat=None
    cf=None

    # create OptionParser object
    parser = OptionParser()

    # add options
    parser.add_option('-o', dest = 'outDir',
                      type = 'string',
                      help = 'file out dir')
    parser.add_option('-F', dest = 'csvFile',
                      type = 'string',
                      help = 'images desc file')
    parser.add_option("-c", "--cf",
                      dest = "cf",
                      help = "color format: true_color, true_color_alpha, true_color_chroma, indexed_1, indexed_2, indexed_4, indexed_8, alpha_1, alpha_2, alpha_4, alpha_8, raw, raw_alpha, raw_chroma")

    parser.add_option("-f", "--format",
                      dest = "format",
                      help = "output file format: c_array, bin_332, bin_565, bin_565_swap, bin_888")

    (options, args) = parser.parse_args()
    if (options.outDir == None):
            print (parser.usage)
            exit(0)
    else:
            outDir = options.outDir

    if (options.csvFile == None):
            print (parser.usage)
            exit(0)
    else:
            csvFile = options.csvFile

    if (options.format == None or options.format not in ["c_array", "bin_332", "bin_565", "bin_565_swap", "bin_888"]):
            print (parser.usage)
            exit(0)
    else:
        Iformat = options.format

    if (options.cf == None or options.cf not in ["true_color", "true_color_alpha", "true_color_chroma", "indexed_1", "indexed_2", "indexed_4", "indexed_8", "alpha_1", "alpha_2", "alpha_4", "alpha_8", "raw", "raw_alpha", "raw_chroma"]):
            print (parser.usage)
            exit(0)
    else:
        cf = options.cf;

    #start convet
    list_c_file = None
    list_h_file = None
    declare_h_file=None

    if (options.format == "c_array"):
        list_c_file = open(outDir+"/image_id_list.c", "w")
        list_c_file.write("/*THIS FILE is auto generated by script, Don not modify it*/\n\n")
        list_c_file.write("#include \"image_id_list.h\"\n\n")
        list_c_file.write("const void* g_image_resource_list[IMAGE_ID_NUM] = {\n")


        list_h_file = open(outDir+"/image_id_list.h", "w")

        list_h_file.write("/*THIS FILE is auto generated by script, Don not modify it*/\n\n")
        list_h_file.write("#ifndef __IMAGE_ID_LIST_H__\n")
        list_h_file.write("#define __IMAGE_ID_LIST_H__\n\n")
        list_h_file.write("#include \"image_declare.h\"\n\n")
        list_h_file.write("typedef enum {\n")



        declare_h_file = open(outDir+"/image_declare.h", "w")
        declare_h_file.write("/*THIS FILE is auto generated by script, Don not modify it*/\n\n")

        declare_h_file.write("#ifndef __IMAGE_DECLARE_H__\n")
        declare_h_file.write("#define __IMAGE_DECLARE_H__\n\n")
        declare_h_file.write("#if defined(PLATFORM_EC600)\n")
        declare_h_file.write("#include \"lvgl.h\"\n")
        declare_h_file.write("#else\n")
        declare_h_file.write("#include \"lvgl/lvgl.h\"\n")
        declare_h_file.write("#endif\n\n")


    Cfile = open(csvFile, 'r')
    reader = csv.reader(Cfile)
    for row in reader:
        print(row)

        #convert "name=icon&img=bunny.png&format=c_array&cf=true_color_alpha"
        cmd = php_path + lvglUtilPath+ " \"dith=1&name="+row[0]+"&img="+row[1]+"&format="+Iformat+"&cf="+cf+"&outDir="+outDir+"\""
        print(cmd)
        os.system(cmd)

        if (options.format == "c_array"):
            list_h_file.write("    "+row[0]+"_ID,\n")
            list_c_file.write("    &"+row[0]+",\n")
            #LV_IMG_DECLARE(analog_clock1_bg);
            declare_h_file.write("LV_IMG_DECLARE("+row[0]+");\n")


    if (options.format == "c_array"):
        list_c_file.write("};\n")

        list_h_file.write("    IMAGE_ID_NUM,\n")
        list_h_file.write("}image_id_enum;\n\n")
        list_h_file.write("typedef struct {\n")
        list_h_file.write("    image_id_enum id;\n")
        list_h_file.write("    int img_ptr;\n")
        list_h_file.write("}image_list_item_t;\n\n")


        list_h_file.write("extern const void* g_image_resource_list[IMAGE_ID_NUM]; \n\n")

        list_h_file.write("#endif")

        declare_h_file.write("#endif")


if __name__ == "__main__":
    Main()
