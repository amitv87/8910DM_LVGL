/*
* Copyright (c) 2020, Unisoc Communications Inc. All Rights Reserved.
** Permission is hereby granted, free of charge, to any person obtaining a copy of
* this software and associated documentation files (the 'Software'), to deal in
* the Software without restriction, including without limitation the rights to
* use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
* the Software, and to permit persons to whom the Software is furnished to do so,
* subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
* FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
* COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
* IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
* CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef __BBAT_BT_H__
#define __BBAT_BT_H__

#define MAX_INQUIRY_NUM 16

#define ID_BT_MSG_BASE 0x10000000UL
#define ID_CM_MSG_BASE 0x10010000UL
#define ID_STATUS_BT_ON_RES (ID_BT_MSG_BASE | 0x0001)
#define ID_STATUS_BT_OFF_RES (ID_BT_MSG_BASE | 0x0002)
#define ID_STATUS_CM_INQUIRY_RES (ID_CM_MSG_BASE | 0x0002)
#define ID_STATUS_CM_INQUIRY_FINISH_RES (ID_CM_MSG_BASE | 0x0003)

typedef void (*BT_CALLBACK_STACK)(unsigned int msg_id, char status, void *data_ptr);

enum{
    BBAT_BT_OPEN = 0x01,
    BBAT_BT_SEARCHER,
    BBAT_BT_READ,
    BBAT_BT_CLOSE,
    BBAT_BT_RSSI,
};

typedef struct
{
    unsigned char addr[6];
    unsigned char rssi;
}INQUIRY_DEVICE;

void bt_register_at_callback_func(BT_CALLBACK_STACK callback);
int bbat_bt_test(unsigned char bt_cmd, char *rsp);

#endif

