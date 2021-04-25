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

#include "hal_config.h"
#include "hwregs.h"

#ifdef CONFIG_SOC_8811
/**
 * whether is lp uart
 */
OSI_UNUSED static inline bool prvUartIsLp(unsigned name)
{
    return (name == DRV_NAME_UART1 || name == DRV_NAME_UART2);
}

/**
 * set baud divder by name, sel_clk is contained for lp uart
 */
OSI_UNUSED static void prvUartSetBaudDivider(unsigned name, unsigned divider)
{
    if (name == DRV_NAME_UART1)
        hwp_pwrCtrl->clk_uart1_cfg = divider | PMUC_UART1_DIV_UPDATE;
    else if (name == DRV_NAME_UART2)
        hwp_pwrCtrl->clk_uart2_cfg = divider | PMUC_UART2_DIV_UPDATE;
    else if (name == DRV_NAME_UART3)
        hwp_sysCtrl->cfg_clk_uart3 = divider;
    else if (name == DRV_NAME_UART4)
        hwp_sysCtrl->cfg_clk_uart4 = divider;
    else if (name == DRV_NAME_UART5)
        hwp_sysCtrl->cfg_clk_uart5 = divider;
}

/**
 * get hwp by name, NULL for invalid name
 */
OSI_UNUSED static HWP_UART_T *prvUartGetHwp(unsigned name)
{
    if (name == DRV_NAME_UART1)
        return hwp_uart1;
    if (name == DRV_NAME_UART2)
        return hwp_uart2;
    if (name == DRV_NAME_UART3)
        return hwp_uart3;
    if (name == DRV_NAME_UART4)
        return hwp_uart4;
    if (name == DRV_NAME_UART5)
        return hwp_uart5;
    return NULL;
}

/**
 * get irqn by name
 */
OSI_UNUSED static unsigned prvUartGetIrqn(unsigned name)
{
    if (name == DRV_NAME_UART1)
        return SYS_IRQ_UART1;
    if (name == DRV_NAME_UART2)
        return SYS_IRQ_UART2;
    if (name == DRV_NAME_UART3)
        return SYS_IRQ_UART3;
    if (name == DRV_NAME_UART4)
        return SYS_IRQ_UART4;
    if (name == DRV_NAME_UART5)
        return SYS_IRQ_UART5;
    return SYS_IRQ_UART5; // name should be already checked
}

/**
 * get irqn priority name
 */
OSI_UNUSED static unsigned prvUartGetIrqPriority(unsigned name)
{
    if (name == DRV_NAME_UART1)
        return SYS_IRQ_PRIO_UART1;
    if (name == DRV_NAME_UART2)
        return SYS_IRQ_PRIO_UART2;
    if (name == DRV_NAME_UART3)
        return SYS_IRQ_PRIO_UART3;
    if (name == DRV_NAME_UART4)
        return SYS_IRQ_PRIO_UART4;
    if (name == DRV_NAME_UART5)
        return SYS_IRQ_PRIO_UART5;
    return SYS_IRQ_PRIO_UART5; // name should be already checked
}

/**
 * fifo read, return read bytes
 */
OSI_UNUSED static int prvUartFifoRead(HWP_UART_T *hwp, void *data, unsigned size)
{
    REG_UART_STATUS_T status = {hwp->status};
    int bytes = status.b.rx_fifo_level;
    if (bytes > size)
        bytes = size;

    uint8_t *data8 = (uint8_t *)data;
    for (int n = 0; n < bytes; n++)
        *data8++ = hwp->rxtx_buffer;
    return bytes;
}

/**
 * fifo write, return written bytes
 */
OSI_UNUSED static int prvUartFifoWrite(HWP_UART_T *hwp, const void *data, unsigned size)
{
    REG_UART_STATUS_T status = {hwp->status};
    int bytes = status.b.tx_fifo_space;
    if (bytes > size)
        bytes = size;

    const uint8_t *data8 = (const uint8_t *)data;
    for (int n = 0; n < bytes; n++)
        hwp->rxtx_buffer = *data8++;
    return bytes;
}

/**
 * fifo write all by loop
 */
OSI_UNUSED static bool prvUartFifoWriteAll(HWP_UART_T *hwp, const void *data, unsigned size)
{
    while (size > 0)
    {
        int bytes = prvUartFifoWrite(hwp, data, size);
        if (bytes > 0)
        {
            data = (const char *)data + bytes;
            size -= bytes;
        }
    }
    return true;
}
#endif
