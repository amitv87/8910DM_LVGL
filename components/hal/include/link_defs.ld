/* Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
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

#ifndef _LINK_DEFS_H_
#define _LINK_DEFS_H_

#include "hal_config.h"

#define DEF_RO .text                \
               .text.*              \
               .gnu.linkonce.t.*    \
               .rdata               \
               .rodata              \
               .rodata.*            \
               .rodata1             \
               .gnu.linkonce.r.*

#define DEF_RW .data                \
               .data.*              \
               .sdata               \
               .sdata.*             \
               .data1               \
               .gnu.linkonce.d.*

#define DEF_ZI .bss                 \
               .bss.*               \
               COMMON               \
               .scommon             \
               .sbss                \
               .sbss.*              \
               .sbss2               \
               .sbss2.*             \
               .gnu.linkonce.b.*

#ifdef CONFIG_CPU_MIPS
#define GOTO_CACHABLE . -= 0x20000000
#define GOTO_UNCACHABLE . += 0x20000000
#endif

// It is important to let "__name_end" is the real end, not the aligned
// end. For some sections, especially ".init_array", software will loop
// the contents inside.
//
// All sections are cache line aligned.

// Sections with different "VirtAddr" and "PhysAddr"

#define LOAD_SECT(name) .name : {               \
    __##name##_start = .;                       \
    name##_contents                             \
    __##name##_end = .;                         \
    . = ALIGN(CONFIG_CACHE_LINE_SIZE);          \
    __##name##_load_start = LOADADDR(.name);    \
}

// Sections with same "VirtAddr" and "PhysAddr"

#define NOAT_SECT(name) .name : {               \
    __##name##_start = .;                       \
    name##_contents                             \
    __##name##_end = .;                         \
    . = ALIGN(CONFIG_CACHE_LINE_SIZE);          \
}

// Sections not to be loaded

#define NOLOAD_SECT(name) .name (NOLOAD) : {    \
    __##name##_start = .;                       \
    name##_contents                             \
    __##name##_end = .;                         \
    . = ALIGN(CONFIG_CACHE_LINE_SIZE);          \
}

// Section without contents, and size is specified

#define NOLOAD_SIZE_SECT(name, size) .name (NOLOAD) : { \
    __##name##_start = .;                               \
    . += size;                                          \
    __##name##_end = .;                                 \
    . = ALIGN(CONFIG_CACHE_LINE_SIZE);                  \
}

// Section without contents, and end is specified

#define NOLOAD_END_SECT(name, end) .name (NOLOAD) : {   \
    __##name##_start = .;                               \
    . = end;                                            \
    __##name##_end = .;                                 \
    . = ALIGN(CONFIG_CACHE_LINE_SIZE);                  \
}

// NOAT sections with specified start alignment

#define NOAT_SECT_ALIGNED(name, aligned) .name : ALIGN(aligned) {   \
    __##name##_start = .;                                           \
    name##_contents                                                 \
    __##name##_end = .;                                             \
    . = ALIGN(CONFIG_CACHE_LINE_SIZE);                              \
}

// LOAD sections located at current address (.)

#define LOAD_DOT_SECT(name) .name . : {         \
    __##name##_start = .;                       \
    name##_contents                             \
    __##name##_end = .;                         \
    . = ALIGN(CONFIG_CACHE_LINE_SIZE);          \
    __##name##_load_start = LOADADDR(.name);    \
}

// NOAT sections located at current address (.)

#define NOLOAD_DOT_SECT(name) .name . (NOLOAD) : {  \
    __##name##_start = .;                           \
    name##_contents                                 \
    __##name##_end = .;                             \
    . = ALIGN(CONFIG_CACHE_LINE_SIZE);              \
}

// Skip current address to the end of given range, and check current
// address, which should be less or equal than start of the range.

#define DOT_SKIP_CHECK(start, end, err)         \
    ASSERT(. <= start, err)                     \
    . = end;                                    \
    . = ALIGN(CONFIG_CACHE_LINE_SIZE);          \

// Anchor dummy section, whch just provided ADDR(.name).

#define ANCHOR_SECT(name) .name (NOLOAD) : {    \
    . = ALIGN(CONFIG_CACHE_LINE_SIZE);          \
}

// Anchor dummy section with current address, whch just provided ADDR(.name).

#define ANCHOR_DOT_SECT(name) .name . (NOLOAD) : {  \
    . = ALIGN(CONFIG_CACHE_LINE_SIZE);              \
}

#define IGNORE_SECT(name) name 0 : { *(name) }
#define DISCARD_SECT(name) /DISCARD/ : { *(name) }

#ifdef CONFIG_CPU_ARM
#define IGNORE_DISCARD_SECTS                \
    IGNORE_SECT(.comment)                   \
    IGNORE_SECT(.ARM.attributes)            \
    IGNORE_SECT(.iplt)                      \
    IGNORE_SECT(.igot.plt)                  \
    IGNORE_SECT(.rel.dyn)                   \
    IGNORE_SECT(.glue_7)                    \
    IGNORE_SECT(.glue_7t)                   \
    IGNORE_SECT(.vfp11_veneer)              \
    IGNORE_SECT(.v4_bx)                     \
    DISCARD_SECT(.ARM.exidx*)               \
    DISCARD_SECT(.gnu.linkonce.armexidx*)   \

#endif

#ifdef CONFIG_CPU_MIPS
#define IGNORE_DISCARD_SECTS        \
    IGNORE_SECT(.comment)           \
    IGNORE_SECT(.gnu.attributes)    \
    IGNORE_SECT(.MIPS.abiflags)     \
    IGNORE_SECT(.reginfo)           \
    IGNORE_SECT(.mdebug.abi32)      \
    IGNORE_SECT(.iplt)              \
    IGNORE_SECT(.igot.plt)          \
    IGNORE_SECT(.rel.dyn)           \
    IGNORE_SECT(.pdr)               \

#endif

#endif
