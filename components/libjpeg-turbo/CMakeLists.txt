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

set(JPEG_LIB_VERSION 62)
set(VERSION 2.0.0)
set(LIBJPEG_TURBO_VERSION_NUMBER 2000000)
set(C_ARITH_CODING_SUPPORTED off)
set(D_ARITH_CODING_SUPPORTED off)
set(MEM_SRCDST_SUPPORTED on)
set(STDIO_SRCDST_SUPPORTED off)
set(WITH_SIMD on)
set(BITS_IN_JSAMPLE 8)
set(HAVE_LOCALE_H off)
set(HAVE_STDDEF_H on)
set(HAVE_STDLIB_H on)
set(NEED_SYS_TYPES_H on)
set(HAVE_UNSIGNED_CHAR on)
set(HAVE_UNSIGNED_SHORT on)
set(INCOMPLETE_TYPES_BROKEN off)
set(RIGHT_SHIFT_IS_UNSIGNED off)
set(__CHAR_UNSIGNED__ off)
set(BUILD 20181020)
set(INLINE "__inline__ __attribute__((always_inline))")
set(CMAKE_PROJECT_NAME "libjpeg-turbo")
set(SIZE_T 4)
set(HAVE_BUILTIN_CTZL on)
set(HAVE_INTRIN_H off)

# configure_file(src/jconfig.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/jconfig.h)
# configure_file(src/jconfigint.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/jconfigint.h)

set(target libjpeg-turbo)

add_library(${target} STATIC EXCLUDE_FROM_ALL
    src/jcapimin.c
    src/jcapistd.c
    src/jccoefct.c
    src/jccolor.c
    src/jcdctmgr.c
    src/jchuff.c
    src/jcicc.c
    src/jcinit.c
    src/jcmainct.c
    src/jcmarker.c
    src/jcmaster.c
    src/jcomapi.c
    src/jcparam.c
    src/jcphuff.c
    src/jcprepct.c
    src/jcsample.c
    src/jctrans.c
    src/jdapimin.c
    src/jdapistd.c
    src/jdatadst.c
    src/jdatasrc.c
    src/jdcoefct.c
    src/jdcolor.c
    src/jddctmgr.c
    src/jdhuff.c
    src/jdicc.c
    src/jdinput.c
    src/jdmainct.c
    src/jdmarker.c
    src/jdmaster.c
    src/jdmerge.c
    src/jdphuff.c
    src/jdpostct.c
    src/jdsample.c
    src/jdtrans.c
    src/jerror.c
    src/jfdctflt.c
    src/jfdctfst.c
    src/jfdctint.c
    src/jidctflt.c
    src/jidctfst.c
    src/jidctint.c
    src/jidctred.c
    src/jquant1.c
    src/jquant2.c
    src/jutils.c
    src/jmemmgr.c
    src/jmemnobs.c

    src/simd/arm/jsimd.c
    src/simd/arm/jsimd_neon.S
)
set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${out_lib_dir})
target_compile_definitions(${target} PRIVATE OSI_LOG_TAG=LOG_TAG_JPEG_TURBO)
target_include_directories(${target} PUBLIC include)
target_include_targets(${target} PRIVATE kernel)
# target_compile_options(${target} PRIVATE "-O3")
set_target_properties(${target} PROPERTIES OUTPUT_NAME jpeg-turbo)
