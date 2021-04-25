/***************************************************************************
 *
 * File:
 *     $Id: ddb.c 1411 2009-04-15 22:10:58Z suneelk $
 *     $Product: iAnywhere Blue SDK v3.x $
 *     $Revision: 1411 $
 *
 * Description:
 *     This is file contains an implementation of the device
 *     database operating in the Windows environment.
 *
 * Created:
 *     January 26, 2000
 *
 * Copyright 2000-2005 Extended Systems, Inc.
 * Portions copyright 2005-2009 iAnywhere Solutions, Inc.
 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of iAnywhere Solutions, Inc.
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of iAnywhere Solutions, Inc.
 *
 * Use of this work is governed by a license granted by iAnywhere Solutions,
 * Inc.  This work contains confidential and proprietary information of
 * iAnywhere Solutions, Inc. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#include "stdlib.h"
#include "stdio.h"
#include "bt_cfg.h"
#include "vfs.h"
#include "bluetooth/bt_config.h"
#include "bluetooth/bt_gatt.h"
#include "ddb.h"
#include "nvm.h"
#include "bt_abs.h"

#ifdef _DB_IN_NV
#define NV_BT_DB        NV_BT_ADDRESS
#endif /* _DB_IN_NV */

/******************************************************************************
 * External dependency
 ******************************************************************************/


/******************************************************************************
 * Debug facility
 ******************************************************************************/
#define DB_DEBUG_ON             1
#define DB_TRACE_ON             1

#if DB_DEBUG_ON
#define DB_ASSERT(_x_)                SCI_ASSERT(_x_)
#else
#define DB_ASSERT(_x_)                ((void)0)
#endif

#if DB_TRACE_ON
#define DB_TRACE(_x_)                SCI_TRACE_LOW(_x_)
void _DB_TraceAddr(const deviceAddr_t *addr);
#else
#define DB_TRACE(_x_)                ((void)0)
#define _DB_TraceRecord(_x_)        ((void)0)
#define _DB_TraceAddr(_x_)          ((void)0)
#endif

#if XA_DEBUG_PRINT == XA_ENABLED
static void _DB_TraceRecord(const scDbDeviceRecord_t *record);
static void _DB_TraceAddr(const BT_ADDRESS *addr);
#else
#define _DB_TraceRecord(_x_)        ((void)0)
#define _DB_TraceAddr(_x_)          ((void)0)
#endif

/******************************************************************************
 * Macro definitions
 ******************************************************************************/
#define DATABASE_FILE_NAME          L"bluetooth.ddb"

#define DATABASE_SIGNATURE          ((uint32)0x53434254) /* 'SCBT' */

#define DB_INVALID_INDEX        0xFFFF
/******************************************************************************
 * Structure definitions
 ******************************************************************************/
typedef struct _DB_DEVICE_RECORD
{
    scDbDeviceRecord_t    record;
    uint32                connectedServices11_00_31;
    uint32                connectedServices11_32_63;
    uint32                connectedServices12_00_31;
    uint32                connectedServices13_00_31;
    BOOLEAN               record_valid;
} DB_DEVICE_RECORD;

typedef struct _DB_RECORD
{
    uint32 magic; //indicate the db record was invalid or not
    uint8 status; //BT is disable or enable for default
    uint8 visibility; //BT visibility
    uint8 volume; //Volume of HFP/A2DP
    uint16 local_name[MAX_BT_DEVICE_NAME]; //BT classic name
    uint16 le_name[MAX_LE_DEVICE_NAME];//name of BLE
    uint8 paired_count; //paired device count
    st_white_list white_list[WHITE_NUM_MAX]; //white list for BLE
    DB_DEVICE_RECORD dev_list[1];//reserve to device service of BT Classic
    bt_dm_device_t dm_device_list[DB_MAX_PAIRED_DEVICE_COUNT];//detail infor of paried device
    sm_key_info_t bt_key_info;//the head of ble smp paired device info
    sm_le_device_info_t le_device_list[LE_SMP_DEVICE_INFO_NUM];//ble smp paired device info
} DB_RECORD;

/******************************************************************************
 * Static data
 ******************************************************************************/
static DB_RECORD    g_db;
static uint8 db_bt_addr[6] = {0};

//static uint32       default_visible = 2;/*0:None 1: inquiry scan enable 2:page scan enable*/
/******************************************************************************
 * Static functions
 ******************************************************************************/
#ifdef _DB_IN_NV
static BOOLEAN _DB_LoadFromNV(void);
static BOOLEAN _DB_SaveToNV(void);
#else
static BOOLEAN _DB_LoadFromFile(void);
static BOOLEAN _DB_SaveToFile(void);
#endif /* _DB_IN_NV */

//extern  int  MGR_SetDeviceInfo(bdaddr_t address,  UINT8 *linkKey,  UINT8 keyType);
extern BOOL rdabt_is_bd_addr_valid(deviceAddr_t *bd_addr);
extern int MGR_AddDeviceRecord(bdaddr_t address,  UINT8 *linkKey,  UINT8 keyType);
extern int MGR_WriteLocalDeviceAddress(bdaddr_t address);
//static BOOL _DB_SearchDevice(const deviceAddr_t *addr, int *index);
void UserFormatToDevAddr(deviceAddr_t* src_addr, const BT_ADDRESS* des_addr);
void DevAddrToUserFormat(BT_ADDRESS* des_addr, const deviceAddr_t* src_addr);


NVITEM_ERR_E EFS_NvitemRead(
    uint16 identifier,
    uint16 count,
    uint8 *buf_ptr)
{
    if (-1 == nvmReadItem(identifier, buf_ptr, count))
    {
        return NVERR_System;
    }
    else
    {
        return NVERR_None;
    }
}
NVITEM_ERR_E EFS_NvitemWrite(

    uint16 identifier,
    uint16 count,
    const uint8 *buf_ptr,
    uint8 basync)
{
    if (-1 == nvmWriteItem(identifier, buf_ptr, count))
    {
        return NVERR_System;
    }
    else
    {
        return NVERR_None;
    }
}
NVITEM_ERR_E NVITEM_UpdateCali(INPUT uint16 ItemID, INPUT uint16 cchSize,
                                 INPUT const void *pBuf)
{
    return EFS_NvitemWrite(ItemID, cchSize, pBuf, 0);
}

BOOLEAN _DB_LoadFromNV(void)
{
    bt_nv_param_t bt_nv_param;
    uint32 status = NVERR_None;

    SCI_MEMSET(&g_db, 0, sizeof(g_db));
    if((NVERR_None != EFS_NvitemRead(NV_BT_DB, sizeof(g_db), (uint8 *)&g_db))
       || (NV_MAIGC_VALUE != g_db.magic))
    {
        /* Initialize fields to default value, if db_record has not init */
        g_db.magic = NV_MAIGC_VALUE;
        g_db.status = FALSE; //BT is disable by default
        g_db.visibility = FALSE; //visibility of BT is disable  by default
        g_db.volume = 5; //volume of BT(HFP/A2dp) is 5 level by default

        //set default device name
        SCI_MEMSET(&bt_nv_param, 0, sizeof(bt_nv_param));
        status = EFS_NvitemRead(NV_BT_CONFIG, sizeof(bt_nv_param), (uint8 *)&bt_nv_param);
        if(NVERR_None != status)
        {
            SCI_TRACE_LOW("[_DB_LoadFromNV] read bt_config nv is failed");
        }
        else
        {
            UTF8toUnicode(bt_nv_param.local_name, 0, g_db.local_name, MAX_BT_DEVICE_NAME);
            UTF8toUnicode(bt_nv_param.local_name, 0,  g_db.le_name, MAX_LE_DEVICE_NAME);
        }

        //save to DE_RECORD
        SCI_TRACE_LOW("[_DB_LoadFromNV] init db record by default value");
        _DB_SaveToNV();
        return FALSE;
    }

    SCI_TRACE_LOW("[_DB_LoadFromNV] g_db.status=%d  name=0x%x 0x%x",g_db.status,g_db.local_name[0],g_db.local_name[1]);

    return TRUE;
}

BOOLEAN _DB_SaveToNV(void)
{
    //////SDI_TRACEID(BT, __DB_SaveToNV__IN, "[_DB_SaveToNV] IN");
    SCI_TRACE_LOW("[_DB_SaveToNV] IN %d",g_db.status);

    if(NVERR_None != EFS_NvitemWrite(NV_BT_DB, sizeof(g_db), (uint8 *)&g_db, 1))
    {
        ////SDI_TRACEID(BT, __DB_SaveToNV__Write_NV_failed, "[_DB_SaveToNV] Write NV failed");
        return FALSE;
    }

    return TRUE;
}

BOOLEAN DB_SaveToNV(void)
{
#ifdef _DB_IN_NV
    return _DB_SaveToNV();
#else
    return _DB_SaveToFile();
#endif /* _DB_IN_NV */
}

//extern BT_CHIP_ID_T bti_BtChipId(void);

BOOLEAN BT_UpdateAddrToNV(BT_ADDRESS *addr)
{
    uint32 status = NVERR_None;
    bt_nv_param_t bt_nv_param;

    //load bt_config of nvitem
    status = EFS_NvitemRead(NV_BT_CONFIG, sizeof(bt_nv_param), (uint8 *)&bt_nv_param);
    SCI_ASSERT(NVERR_None == status); /*assert verified*/

    //update bd_addr
    memcpy((uint8 *)&bt_nv_param.bd_addr, addr, sizeof(BT_ADDRESS));
    if(NVERR_None != EFS_NvitemWrite(NV_BT_CONFIG, sizeof(bt_nv_param), (uint8 *)&bt_nv_param, 1))
    {
        ////SDI_TRACEID(BT, _DB_SaveAddrToNV__Write_NV_failed, "[DB_SaveAddrToNV] Write NV failed");
        return FALSE;
    }

    return TRUE;
}

uint32 GetHostNvCtrl(void)
{
    bt_pskey_nv_info_t bt_sprd;
    uint32 status = NVERR_None;

    status = EFS_NvitemRead(NV_BT_SPRD, sizeof(bt_pskey_nv_info_t), (uint8 *)&bt_sprd);
    SCI_ASSERT(NVERR_None == status); /*assert verified*/

    return bt_sprd.host_reserved[0];
}


/******************************************************************************
 * Implementation
 ******************************************************************************/
BOOLEAN DB_Init(void)
{
    OSI_LOGI(0, "[BT] enter DB_Init");
#ifdef _DB_IN_NV
    return _DB_LoadFromNV();
#else
    return _DB_LoadFromFile();
#endif /* _DB_IN_NV */
}

uint16 DB_SetLocalName(const uint16 *local_name)
{
    uint16 len;

    Assert(local_name);/*assert verified*/

    len = PF_WStrLen(local_name) / 2;

    if(len >= ARRAY_SIZE(g_db.local_name))
    {
        len = ARRAY_SIZE(g_db.local_name) - 1;
    }

    memcpy(g_db.local_name, local_name, len * 2);
    g_db.local_name[len] = 0;

    DB_SaveToNV();
    return len;
}

void DB_SaveLocalNameAndToUTF8(const uint16 *local_name, uint8 *name_utf8)
{
    DB_SetLocalName(local_name);
    UnicodetoUTF8(local_name, 0, name_utf8, MAX_BT_DEVICE_NAME);

    return;
}

uint16 DB_GetLocalName(uint16 *local_name)
{
    Assert(local_name) ;/*assert verified*/

    PF_WStrCpy(local_name, g_db.local_name);
    return PF_WStrLen(g_db.local_name) / 2;
}

BT_STATUS DB_GetLocalNameUTF8(uint8 *name)
{
    uint16  name_in_db[MAX_BT_DEVICE_NAME];

    DB_GetLocalName(name_in_db);
    UnicodetoUTF8(name_in_db, 0, name, MAX_BT_DEVICE_NAME);

    return BT_SUCCESS;
}

uint16 DB_GetLeName(uint16 *le_name)
{
    Assert(le_name) ;/*assert verified*/

    PF_WStrCpy(le_name, g_db.le_name);
    return PF_WStrLen(g_db.le_name) / 2;
}

BT_STATUS DB_GetLeNameUTF8(uint8 *name)
{
    uint16  name_in_db[MAX_LE_DEVICE_NAME];

    DB_GetLeName(name_in_db);
    UnicodetoUTF8(name_in_db, 0, name, MAX_LE_DEVICE_NAME);

    return BT_SUCCESS;
}

BOOLEAN IsSameUserDevAddr( BT_ADDRESS *des,  BT_ADDRESS *src)
{
    Assert(NULL != des);/*assert verified*/
    Assert(NULL != src);/*assert verified*/

    if((des->addr[0] == src->addr[0]) &&
       (des->addr[1] == src->addr[1]) &&
       (des->addr[2] == src->addr[2]) &&
       (des->addr[3] == src->addr[3]) &&
       (des->addr[4] == src->addr[4]) &&
       (des->addr[5] == src->addr[5])
      )
    {
        return TRUE;
    }

    return FALSE;
}

uint32 UTF8toUnicode(const uint8 *utf8, uint16 utf8_length, uint16 *unicode, uint16 unicode_length)
{

    int32 i = 0;
    int32 j = 0;

    /* if uft8_length is 0, utf8 is terminated with 0 */
    while(((0 == utf8_length) || (i < utf8_length)) && (0 != utf8[i]) && (j < unicode_length - 1))
    {
        if(0 == (utf8[i] & 0x80))    /* 0b0xxxxxxx - single-byte character */
        {
            unicode[j] = utf8[i];

            i++;
            j++;
        }
        else if(0xc0 == (utf8[i] & 0xe0))      /* 0b110xxxxx 0b10xxxxxx - 2-bytes utf8 */
        {
            if(((0 != utf8_length) && ((i + 2) > utf8_length))
               || (0x80 != (utf8[i + 1] & 0xc0)))
            {
                break;
            }

            unicode[j] = ((utf8[i] & 0x1f) << 6) | (utf8[i + 1] & 0x3f);

            i += 2;
            j++;
        }
        else if(0xe0 == (utf8[i] & 0xf0))      /* 0b1110xxxx 0b10xxxxxx 0b10xxxxxx - 3-bytes utf8 */
        {
            if(((0 != utf8_length) && ((i + 3) > utf8_length))
               || (0x80 != (utf8[i + 1] & 0xc0))
               || (0x80 != (utf8[i + 2] & 0xc0)))
            {
                break;
            }

            unicode[j] = ((utf8[i] & 0x0f) << 12) | ((utf8[i + 1] & 0x3f) << 6)
                         | (utf8[i + 2] & 0x3f);

            i += 3;
            j++;
        }
        else
        {
            /* unknown character */
            break;
        }
    }

    unicode[j] = 0;
    return j << 1;
}

/* UnicodetoUTF8
 * Return utf8 size in bytes.
 */
uint32 UnicodetoUTF8(const uint16 *unicode, uint16 unicode_length, uint8 *utf8, uint16 utf8_length)
{

    int32 i = 0;
    int32 j = 0;

    /* If unicode_length is 0, unicode is terminated with 0 */
    while(((0 == unicode_length) || (j < unicode_length)) && (0 != unicode[j]))
    {
        if(unicode[j] < 0x0080)    /* single byte */
        {
            if(i + 1 > utf8_length - 1)
            {
                break;
            }

            utf8[i] = (uint8)(unicode[j] & 0x00ff);

            i++;
            j++;
        }
        else if(unicode[j] < 0x0800)      /* 2-bytes */
        {
            if(i + 2 > utf8_length - 1)
            {
                break;
            }

            utf8[i] = (uint8)(0xc0 | (unicode[j] >> 6));
            utf8[i + 1] = (uint8)(0x80 | (unicode[j] & 0x3f));

            i += 2;
            j++;
        }
        else     /* 3-bytes */
        {
            if(i + 3 > utf8_length - 1)
            {
                break;
            }

            utf8[i]     = (uint8)(0xe0 | (unicode[j] >> 12));
            utf8[i + 1] = (uint8)(0x80 | ((unicode[j] >> 6) & 0x3f));
            utf8[i + 2] = (uint8)(0x80 | (unicode[j] & 0x3f));

            i += 3;
            j++;
        }
    }

    utf8[i] = 0;
    return i;
}

uint16 PF_WStrCmp
(
    const uint16 *Str1,
    const uint16 *Str2
)
{

    const uint8 *des_ptr = (const uint8 *)Str1;
    const uint8 *src_ptr = (const uint8 *)Str2;

    while((*des_ptr == *src_ptr) && (*(des_ptr + 1) == *(src_ptr + 1)))
    {
        if((*des_ptr == 0) &&
           (*(des_ptr + 1) == 0))
        {
            return 0;
        }

        des_ptr += 2;
        src_ptr += 2;
    }

    return 1;
}

/*---------------------------------------------------------------------------
 * Compares two strings upto specified no of bytes.
 */

uint16 *PF_WStrCpy
(
    uint16 *dst,
    const uint16 *src
)
{
    uint16 *s = dst;
    uint8 *des_ptr = (uint8 *)dst;
    const uint8 *src_ptr = (uint8 *)src;

    while((0 != *src_ptr) || (0 != *(src_ptr + 1)))
    {
        *des_ptr++ = *src_ptr++;
        *des_ptr++ = *src_ptr++;
    }

    *des_ptr++ = 0;
    *des_ptr = 0;
    return s;
}

/*---------------------------------------------------------------------------
 * Concatanates two strings.
 */

uint16 *PF_WStrCat(uint16 *dst, const uint16 *src)
{
    uint16 *s = dst;

    while(*s != 0)
    {
        s++;
    }

    PF_WStrCpy(s, src);
    return dst;
}


/*---------------------------------------------------------------------------
 * Returns the length of the string.
 */

uint16 PF_WStrLen(const uint16 *Str)
{
    const uint8 *cp = (const uint8 *)Str;
    Assert(Str);/*assert verified*/

    while((0 != *cp) || (0 != *(cp + 1)))
    {
        cp += 2;
    }

    return (cp - (const uint8 *)Str);
}

/*---------------------------------------------------------------------------
 * PF_Tolower()
 *
 * Converts the upper case character to lower case character & returns the
 * lower case character
 *
 * Parameters:
 *    ch - input charater.
 *
 * Returns:
 *    equivalent lower case of the ch character.
 */
char PF_ToLower(char ch)
{
    /* return lowercase, if uppercase */
    if(ch >= 'A' && ch <= 'Z')
    {
        return ch + 32;
    }

    return ch;
}

const uint16 *PF_WStrStr(const uint16 *string, const uint16 *strCharSet)
{
    if(*strCharSet == L'\0')
    {
        return string;
    }

    while(*string)
    {
        const uint16 *tmpPtr = string;
        const uint16 *searchPtr = strCharSet;

        while(*searchPtr && *tmpPtr && (*searchPtr == *tmpPtr))
        {
            searchPtr++;
            tmpPtr++;
        }

        if(*searchPtr == L'\0')
        {
            return string;
        }

        string++;
    }

    return NULL;
}

// 20100804: will.jiang_cr191100: BT supports little endian
/*
   convert wchars from bigendian to host;
*/
void Wchars_BeToHost(uint16 *wstr)
{
#ifdef CHIP_ENDIAN_LITTLE
    uint16 data;

    while(*wstr != 0)
    {
        data = btohs(*wstr);
        *wstr = data;
        wstr++;
    }

#endif
}

/*
   convert wchars from host to bigendian;
*/
void Wchars_HostToBe(uint16 *wstr)
{
#ifdef CHIP_ENDIAN_LITTLE
    uint16 data;

    while(*wstr != 0)
    {
        data = htobs(*wstr);
        *wstr = data;
        wstr++;
    }

#endif
}

uint8 DB_GetAddrTypeFromAddr(st_white_list_info *WhiteInfo)
{
    uint8 i;
    uint8 RetVal = 1;

    for(i = 0; i < WHITE_NUM_MAX; i++)
    {
        if(0 == SCI_MEMCMP(WhiteInfo->addr, g_db.white_list[i].white_list_info.addr, SIZE_OF_BDADDR))
        {
            WhiteInfo->addr_type = g_db.white_list[i].white_list_info.addr_type;
            RetVal = 0;
            break;
        }
    }

    return RetVal;
}

uint8 DB_GetWhiteNun(void)
{
    uint8 i;
    uint8 Num = 0;

    for(i = 0; i < WHITE_NUM_MAX; i++)
    {
        if(g_db.white_list[i].is_used)
        {
            Num++;
        }
    }
    return Num;
}

uint8 DB_GetWhiteInfo(uint8 index, st_white_list_info *white_list_info)
{
    uint8 i;
    uint8 Num = 0;
    uint8 RetVal = 1;

    for(i = 0; i < WHITE_NUM_MAX; i++)
    {
        if(g_db.white_list[i].is_used)
        {
            if(Num == index)
            {
                SCI_MEMCPY(white_list_info, &(g_db.white_list[i].white_list_info), sizeof(st_white_list_info));
                RetVal = 0;
                break;
            }
            Num++;
        }
    }
    return RetVal;

}

uint8 DB_AddWhiteInfo(st_white_list_info *WhiteInfo)
{
    uint8 i;
    uint8 RetVal = 1;

    for(i = 0; i < WHITE_NUM_MAX; i++)
    {
        if(g_db.white_list[i].is_used == 0)
        {
            break;
        }
    }
    if(i < WHITE_NUM_MAX)
    {
        g_db.white_list[i].is_used = 1;
        SCI_MEMCPY(&(g_db.white_list[i].white_list_info), WhiteInfo, sizeof(st_white_list_info));
        RetVal = 0;
    }

    return RetVal;
}

uint8 DB_RemoveWhiteInfo(st_white_list_info *WhiteInfo)
{
    uint8 i;
    uint8 RetVal = 1;

    for(i = 0; i < WHITE_NUM_MAX; i++)
    {
        if(0 == memcmp(WhiteInfo->addr, g_db.white_list[i].white_list_info.addr, SIZE_OF_BDADDR))
        {
            WhiteInfo->addr_type = g_db.white_list[i].white_list_info.addr_type;
            memset(&(g_db.white_list[i]), 0, sizeof(st_white_list));
            RetVal = 0;
            break;
        }
    }

    return RetVal;
}

void DB_ClearWhitelist(void)
{
    memset(g_db.white_list, 0, sizeof(g_db.white_list));

    return;
}

BT_STATUS BT_UpdateNV(void)
{
    if(DB_SaveToNV())
    {
        return BT_SUCCESS;
    }
    else
    {
        return BT_ERROR;
    }
}

void DB_SetVisibility(uint8 visibility)
{
    g_db.visibility= visibility;
}

uint8 DB_GetVisibility(void)
{
    return g_db.visibility;
}

uint8 DB_RemovePairedInfo(bdaddr_t addr)
{
    uint8 i;
    uint8 RetVal = 1;

    for(i = 0; i < DB_MAX_PAIRED_DEVICE_COUNT; i++)
    {
        if(0 == memcmp(addr.bytes, g_db.dm_device_list[i].addr.bytes, SIZE_OF_BDADDR))
        {
            memset(&(g_db.dm_device_list[i]), 0, sizeof(bt_dm_device_t));
            RetVal = 0;
            _DB_SaveToNV();
            break;
        }
    }
    SCI_TRACE_LOW("DB_RemovePairedInfo exit RetVal=%d",RetVal);
    return RetVal;
}

uint8 DB_GetPairedNum(uint32 service_type)
{
    uint8 i;
    uint8 Num = 0;

    for(i = 0; i < DB_MAX_PAIRED_DEVICE_COUNT; i++)
    {
        if(g_db.dm_device_list[i].adp_state)
        {
            Num++;
        }
    }
    SCI_TRACE_LOW("DB_GetPairedNun exit Num=%d",Num);
    return Num;
}

uint8  DB_GetPairedInfoFormIndex(uint8 index, bt_dm_device_t *bt_dm_device)
{
    uint8 i;
    uint8 Num = 0;
    uint8 RetVal = 1;

    for(i = 0; i < DB_MAX_PAIRED_DEVICE_COUNT; i++)
    {
        if(g_db.dm_device_list[i].adp_state)
        {
           if(Num == index)
           {
                memcpy(bt_dm_device, &(g_db.dm_device_list[i]), sizeof(bt_dm_device_t));
                RetVal = 0;
                break;
            }
            Num++;
         }
    }
    //SCI_TRACE_LOW("DB_GetPairedInfoFormIndex exit RetVal=%d, Num=%d device_name[0,1,2]=%d,%d,%d",RetVal, Num, bt_dm_device->device_name[0], bt_dm_device->device_name[1], bt_dm_device->device_name[2]);
    return RetVal;
}

void DB_GetLocalPublicAddr(BT_ADDRESS* bd_addr)
{
    uint8 invalid_addr[2][6] = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, {0,0,0,0,0,0}};
    uint32 status = NVERR_None;
    bt_nv_param_t bt_nv_param;

    //check mac addr is ready or not
    if(0 != memcmp(db_bt_addr, invalid_addr[1], 6))
    {
        SCI_TRACE_LOW("MAC addr is already!");
        memcpy(bd_addr, db_bt_addr, 6);
        SCI_TRACE_LOW("[bd_addr]%02X:%02X:%02X:%02X:%02X:%02X",bd_addr->addr[5],bd_addr->addr[4],bd_addr->addr[3],
                        bd_addr->addr[2],bd_addr->addr[1],bd_addr->addr[0]);
        return;
    }

    //read mac addr from nv
    status = EFS_NvitemRead(NV_BT_CONFIG, sizeof(bt_nv_param), (uint8 *)&bt_nv_param);
    if(NVERR_None != status)
    {
        SCI_TRACE_LOW("[DB_GetLocalPublicAddr] read bt_config nv is failed");
    }
    else
    {
        memcpy((uint8 *)db_bt_addr, (uint8 *)&bt_nv_param.bd_addr, sizeof(BT_ADDRESS));
    }

    //check addr is invalid or not
    for(uint8 i=0; i<2; i++)
    {
        if (0 == memcmp((const char *)invalid_addr[i], (const char *)db_bt_addr, sizeof(BT_ADDRESS)))
        {
            SCI_TRACE_LOW("[DB_GetLocalPublicAddr]MAC addr of NV is invalid, set addr by random");
            //if NV of bt_config was not configure, set default addr
            db_bt_addr[0] = rand() & 0xff;
            db_bt_addr[1] = rand() & 0xff;
            db_bt_addr[2] = rand() & 0xff;
            db_bt_addr[3] = rand() & 0xff;
            db_bt_addr[4] = rand() & 0xff;
            db_bt_addr[5] = rand() & 0xff;
            break;
        }
    }

    //copy out bd addr
    memcpy(bd_addr, db_bt_addr, 6);
    SCI_TRACE_LOW("[bd_addr]%02X:%02X:%02X:%02X:%02X:%02X",bd_addr->addr[5],bd_addr->addr[4],bd_addr->addr[3],
                    bd_addr->addr[2],bd_addr->addr[1],bd_addr->addr[0]);
}

//save the ble smp paired device info to running nv
//we just can save LE_SMP_DEVICE_INFO_NUM device info,
//if paired device num is larger than LE_SMP_DEVICE_INFO_NUM, cover the first one.
uint8 DB_SaveSmpKeyInfo(sm_le_device_info_t *device_info)
{
    uint8 i;
    uint8 RetVal = 0;

    SCI_TRACE_LOW("DB_SaveSmpKeyInfo enter");

    //if device info is already in nv, just return
    for(i = 0; i < LE_SMP_DEVICE_INFO_NUM; i++)
    {
        if(!memcmp(g_db.le_device_list[i].bd_address.bytes, device_info->bd_address.bytes, SIZE_OF_BDADDR))
        {
            SCI_TRACE_LOW("DB_SaveSmpKeyInfo device info already exist");
            return RetVal;
        }
    }

    g_db.bt_key_info.magic = LE_KEY_MAGIC;
    g_db.bt_key_info.le_device_nums++;//paired device num
    SCI_TRACE_LOW("DB_SaveSmpKeyInfo le_device_nums=%d", g_db.bt_key_info.le_device_nums);

    if(g_db.bt_key_info.le_device_nums > LE_SMP_DEVICE_INFO_NUM)
    {
        //if paired device num is larger than LE_SMP_DEVICE_INFO_NUM, cover from the first.
        g_db.bt_key_info.le_device_nums = 1;
    }

    device_info->p_conn_info = NULL;
    g_db.le_device_list[g_db.bt_key_info.le_device_nums - 1] = *device_info;

    _DB_SaveToNV();

    return RetVal;
}

//remove the ble smp paired device info from running nv
uint8 DB_RemoveSmpKeyInfo(bdaddr_t addr)
{
    uint8 i;
    uint8 RetVal = 1;

    for(i = 0; i < LE_SMP_DEVICE_INFO_NUM; i++)
    {
        if(0 == memcmp(addr.bytes, g_db.le_device_list[i].bd_address.bytes, SIZE_OF_BDADDR))
        {
            memset(&(g_db.le_device_list[i]), 0, sizeof(bt_dm_device_t));
            RetVal = 0;
            _DB_SaveToNV();
            break;
        }
    }
    SCI_TRACE_LOW("DB_RemoveSmpKeyInfo exit RetVal=%d",RetVal);
    return RetVal;
}

//get the ble smp paired device info from running nv
void DB_GetSmpKeyInfo(sm_le_device_info_t *device_list)
{
    uint8 i;

    for(i = 0; i < LE_SMP_DEVICE_INFO_NUM; i++)
    {
        SCI_TRACE_LOW("DB_GetSmpKeyInfo device_state=%d",g_db.le_device_list[i].device_state);
        device_list[i] = g_db.le_device_list[i];
    }
}

