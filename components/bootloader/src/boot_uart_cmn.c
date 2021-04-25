/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
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

#include "boot_uart.h"

bool bootUartWriteAll(bootUart_t *d, const void *data, size_t size)
{
    if (size == 0)
        return true;
    if (d == NULL || data == NULL)
        return false;

    while (size > 0)
    {
        int wsize = bootUartWrite(d, data, size);
        if (wsize > 0)
        {
            size -= wsize;
            data = (const char *)data + wsize;
        }
    }
    return true;
}

static bool prvUartWriteByte(bootUart_t *d, char ch)
{
    return bootUartWriteAll(d, &ch, 1);
}

static void prvUartSendHdlc(bootUart_t *d, const void *data, size_t size)
{
    while (size > 0)
    {
        char ch = *(const char *)data;
        if (ch == 0x7e || ch == 0x7d)
        {
            prvUartWriteByte(d, 0x7d);
            prvUartWriteByte(d, ch ^ 0x20);
        }
        else
        {
            prvUartWriteByte(d, ch);
        }
        size--;
        data = (const char *)data + 1;
    }
}

bool bootUartWriteHdlc(bootUart_t *d, const void *data, size_t size)
{
    if (size == 0)
        return true;
    if (d == NULL || data == NULL)
        return false;

    prvUartWriteByte(d, 0x7e);
    prvUartSendHdlc(d, data, size);
    prvUartWriteByte(d, 0x7e);
    return true;
}

bool bootUartWriteMultiHdlc(bootUart_t *d, const osiBuffer_t *bufs, unsigned count)
{
    if (count == 0)
        return true;
    if (d == NULL || bufs == NULL)
        return false;

    prvUartWriteByte(d, 0x7e);
    for (unsigned n = 0; n < count; n++)
        prvUartSendHdlc(d, (const void *)bufs[n].ptr, bufs[n].size);
    prvUartWriteByte(d, 0x7e);
    return true;
}
