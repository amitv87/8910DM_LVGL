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

#include "osi_log.h"
#include "osi_api.h"
#include "osi_trace.h"
#include "osi_sysnv.h"
#include "unity_fixture.h"
#include "drv_uart.h"
#include "hal_config.h"
#include <string.h>

#define UNITY_OUTPUT_LINE_SIZE (512)

typedef struct
{
    osiSemaphore_t *lock;
    drvUart_t *uart;
    uint16_t output_size;
    char output[UNITY_OUTPUT_LINE_SIZE];
} unityContext_t;

static unityContext_t gUnityCtx;

bool unityInit(void)
{
    unityContext_t *d = &gUnityCtx;
    if (d->lock != NULL)
        return true;

    d->lock = osiSemaphoreCreate(1, 1);
    d->output_size = 0;
    d->uart = NULL;
    return (d->lock != NULL);
}

int unityStart(void)
{
    unityContext_t *d = &gUnityCtx;
    if (d->lock == NULL || !osiSemaphoreTryAcquire(d->lock, 0))
        return -1;

    const char *argv[] = {"", "-v"};
    memset(&Unity, 0, sizeof(Unity));
    memset(&UnityFixture, 0, sizeof(UnityFixture));
    int ret = UnityMain(2, argv, UnityRunAllTests);
    osiSemaphoreRelease(d->lock);
    return ret;
}

int unityStartCases(void (*run_cases)(void))
{
    unityContext_t *d = &gUnityCtx;
    if (d->lock == NULL || !osiSemaphoreTryAcquire(d->lock, 0))
        return -1;

    const char *argv[] = {"", "-v"};
    memset(&Unity, 0, sizeof(Unity));
    memset(&UnityFixture, 0, sizeof(UnityFixture));
    int ret = UnityMain(2, argv, run_cases);
    osiSemaphoreRelease(d->lock);
    return ret;
}

bool unityOpenUart(uint32_t name, unsigned baud)
{
    unityContext_t *d = &gUnityCtx;
    if (d->lock == NULL || !osiSemaphoreTryAcquire(d->lock, 0))
        return false;

    if (d->uart != NULL)
        goto fail_unlock;

    drvUartCfg_t uart_cfg = {
        .baud = baud,
        .data_bits = DRV_UART_DATA_BITS_8,
        .stop_bits = DRV_UART_STOP_BITS_1,
        .parity = DRV_UART_NO_PARITY,
        .rx_buf_size = 256,
        .tx_buf_size = 256,
        .event_mask = 0,
        .event_cb = 0,
        .event_cb_ctx = NULL,
    };
    d->uart = drvUartCreate(name, &uart_cfg);
    if (d->uart == NULL || !drvUartOpen(d->uart))
    {
        drvUartDestroy(d->uart);
        d->uart = NULL;
        goto fail_unlock;
    }

    osiSemaphoreRelease(d->lock);
    return true;

fail_unlock:
    osiSemaphoreRelease(d->lock);
    return false;
}

bool unityCloseUart(void)
{
    unityContext_t *d = &gUnityCtx;
    if (d->lock == NULL || !osiSemaphoreTryAcquire(d->lock, 0))
        return false;

    if (d->uart != NULL)
    {
        drvUartDestroy(d->uart);
        d->uart = NULL;
    }

    osiSemaphoreRelease(d->lock);
    return true;
}

void UnityOutputChar(int c)
{
    unityContext_t *d = &gUnityCtx;
    if (d == NULL)
        return;

    if (d->uart == NULL)
    {
        if (d->output_size < UNITY_OUTPUT_LINE_SIZE - 1)
            d->output[d->output_size++] = (char)c;

        if (c == 0 || c == '\n')
        {
            d->output[d->output_size++] = '\0';
            d->output_size = 0;
            OSI_LOGXI(OSI_LOGPAR_S, 0, "%s", d->output);
        }
    }
    else
    {
        uint8_t ch = c;
        drvUartSendAll(d->uart, &ch, 1, -1U);
    }
}

void UnityOutputFlush(void)
{
}

void UnityOutputStart(void)
{
}

void UnityOutputComplete(void)
{
}
