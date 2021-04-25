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

#include <stdint.h>
#include "hal_config.h"
#include "boot_config.h"
#include "boot_platform.h"
#include "boot_uimage.h"
#include "hal_adi_bus.h"
#include "osi_api.h"
#include "hal_chip.h"
#include "hal_lzma.h"
#include "hwregs.h"

extern const unsigned gBoot2CompData[];

// override irq and blue screen to empty functions
void bootIrqHandler(void) { OSI_DEAD_LOOP; }
OSI_NO_RETURN void bootBlueScreen(void *ctx) { OSI_DEAD_LOOP; }

void bootStart(uint32_t param)
{
    // Here: I-cache is enabled, D-cache is disabled
    OSI_CLEAR_SECTION(bss);

    // The jobs here should be as minimal as possible. Trace functions are
    // not linked by design. So called functions shouldn't have traces.
    halClockInit();
    halAdiBusInit();
    bootResetPinEnable();

    void *boot2_addr = (void *)CONFIG_BOOT2_IMAGE_START;
    if (halLzmaDecompressFile(gBoot2CompData, boot2_addr) <= 0)
        goto failed;

    // Boot2 is compressed and embedded in boot, it is not needed to
    // check boot2 uimage. It can save several ms.
    bootJumpFunc_t entry = (bootJumpFunc_t)bootUimageEntry(boot2_addr);
    bootJumpImageEntry(param, entry);

failed:
    OSI_DEAD_LOOP;
}
