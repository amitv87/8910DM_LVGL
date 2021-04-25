/**
 * @file    bt_types.h
 * @brief    -
 * @details  
 * @mainpage 
 * @version  V1.0.0
 * @date     2019-10-11
 * @license  Copyright (C) 2019 Unisoc Communications Inc
 */

#ifndef __BT_HAL_HCI_TYPES_H__
#define __BT_HAL_HCI_TYPES_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INT8 127
#define MIN_INT8 (-MAX_INT8 - 1)
#define MAX_UINT8 255
#define MIN_UINT8 0
#define MAX_INT16 32767
#define MIN_INT16 (-MAX_INT16 - 1)
#define MAX_UINT16 65535
#define MIN_UINT16 0
#define MAX_INT32 2147483647L
#define MIN_INT32 (-MAX_INT32 - 1)
#define MAX_UINT32 4294967295U
#define MIN_UINT32 0U

#define pENOERROR 0
#define pENOMEMORY 1
#define pENORESOURCE 2
#define pEINVALIDPARM 3
#define pEWOULDBLOCK 4
#define pELOCKERROR 5
#define pEIOERROR 6
#define pENOTSUPPORTED 7
#define pEUNKNOWNERROR 8

#define SIZE_OF_BDADDR 6
#define SIZE_OF_LINKKEY 16
#define SIZE_OF_DHKEY 24
#define LINKKEY_SIZE 16

typedef unsigned long long UINT64;
typedef unsigned int UINT32;
typedef unsigned short UINT16;
typedef long long INT64;
typedef int INT32;
typedef short INT16;
typedef unsigned char UINT8;
typedef char INT8;
typedef unsigned char BOOL;
typedef unsigned int HANDLE;

typedef unsigned short channel_id;

typedef struct
{
    UINT8 bytes[SIZE_OF_BDADDR];
} bdaddr_t;

typedef struct
{
    UINT8 *buf;
    UINT8 transport;
    UINT16 dataLen;
    UINT16 bufLen;
} data_buf_t;

typedef struct
{
    UINT8 *data;
    UINT8 transport;
    UINT16 len;
    UINT16 size;
    UINT16 flags;
    UINT16 hci_handle_flags;
    UINT8 hci_type;
    UINT8 *tail;
} hci_data_t;

typedef hci_data_t hci_cmd_t;

#ifndef false
#define false 0
#endif
#ifndef true
#define true 1
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef BD_ADDR_CMP
#define BD_ADDR_CMP(x, y) !memcmp(((x).bytes), ((y).bytes), SIZE_OF_BDADDR)
#endif

#define RDABT_HOST_BUF_HCI_HEADER_PRESENT 0x0001
#define RDABT_HOST_BUF_USE_RESV_ACL_MEM 0x0002
/* is this packet a packet allocated on the way out or way in? */
/* 1 is a packet from the queues */
#define HCI_BUF_OUTGOING_PACKET_MASK 0x0004
#define HCI_BUF_INCOMING_PACKET_MASK 0x0008

#define RDABT_HOST_BUF_URGENCY_PACKET 0x0010

#define RDABT_HANDLE_FLAG_PICONET_BROADCAST 0x8000
#define RDABT_HANDLE_FLAG_ACTIVE_BROADCAST 0x4000
#define RDABT_HANDLE_FLAG_START_FRAGMENT 0x2000
#define RDABT_HANDLE_FLAG_CONTINUE_FRAGMENT 0x1000

#define RDABT_USER_ENDED_CONNECTION 0x13
#define RDABT_LOW_RESOURCES 0x14
#define RDABT_ABOUT_TO_POWER_OFF 0x15
#define RDABT_TERMINATED_BY_LOCAL_HOST 0x16

#define MAX_PIN_LEN 16
#define DEFAULT_FLUSH_TIMEOUT 0x0000
#define MAX_FLUSH_TIMEOUT 0x07FF
#define DEFAULT_LINK_SUP_TIMEOUT 0x7D00
#define MAX_DEFAULT_LINK_SUP_TIMEOUT 0xFFFF

#define MGR_MAX_REMOTE_NAME_LEN 32
#define MGR_MAX_LOCAL_NAME_LEN 32

typedef enum
{
    BT_SUCCESS,
    BT_FAIL,
    BT_DONE,
    BT_PENDING,
    BT_INVALIDPARAM,
    BT_SECFAIL,
    BT_FLOWCTRL,
    BT_NORESOURCES,
    BT_UNSUPPORTED,
    BT_HWERROR,
    BT_HOSTERROR,
    BT_UNKNOWNERROR,
    BT_NOT_READY,
    BT_RETRY,
    BT_AUTHORISATIONFAIL,
    BT_AUTHENTICATIONFAIL,
    BT_ENCRYPTFAIL,
    BT_TIMEOUT,
    BT_PROTOERROR,
    BT_DISALLOWED,
    BT_BUFTOOSMALL,
    BT_DISCONNECTED,
    BT_INVALIDERRORCODE,
} bt_status_t;

#define HCI_TYPE_CMD 0x01
#define HCI_TYPE_ACL 0x02
#define HCI_TYPE_SCO 0x03
#define HCI_TYPE_EVT 0x04

#define BT_GIAC_LAP 0x9E8B33
#define BT_LIAC_LAP 0x9E8B00
#ifndef GIAC_LAP
#define GIAC_LAP BT_GIAC_LAP
#endif

#define BT_SET_BIT(val, n) ((val) |= (1 << n))
#define BT_CLR_BIT(val, n) ((val) &= ~(1 << n))

#endif
