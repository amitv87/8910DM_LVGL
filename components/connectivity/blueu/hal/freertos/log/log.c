/**
 * @file     log.c
 * @brief    -
 * @details  
 * @mainpage 
 * @version  V1.0.0
 * @date     2019-10-11
 * @license  Copyright (C) 2019 Unisoc Communications Inc
 */

#include <stdint.h>
#include "bt_types.h"
#include "utrace_dsp_log.h"
#include "log.h"

#define assert(__e) ((void)0)


void logpt_hci_dump(UINT8 direction, UINT8 type, UINT8 *src, UINT32 length)
{
    switch (type)
    {
        case 0x02:
        {
            if (LPT_C2H == direction)
            {
                xlogi_f2(LPT_HCI_ACL_C2H, "", src[0], src[1]);
            }
            else
            {
                xlogi_f2(LPT_HCI_ACL_H2C, "", src[0], src[1]);
            }
            xlogi_f2(LPT_PAYLOAD, "", src[2], src[3]);
            xlogi_f2(LPT_PAYLOAD, "", src[4], src[5]);
        }
        break;

        case 0x04:
        {
            xlogi_f2(LPT_HCI_EVT, "", src[0], src[1]);
            xlogi_f2(LPT_PAYLOAD, "", src[2], src[3]);
            xlogi_f2(LPT_PAYLOAD, "", src[4], src[5]);
        }
        break;

    }
}

void logpt_primary(UINT16 tag, UINT16 points)
{
    xlogi_f1(tag, "", points);
}

void logpt_msg(UINT16 tag, UINT16 index, UINT16 payload)
{
    xlogi_f1(tag, "", index);
    xlogi_f1(LPT_PAYLOAD, "", payload);
}

void bt_assert_mask(unsigned int value)
{
    assert(0);
}

void bt_assert()
{
    assert(0);
}

