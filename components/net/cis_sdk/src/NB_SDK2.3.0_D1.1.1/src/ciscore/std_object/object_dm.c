/*******************************************************************************
 *
 * Copyright (c) 2013, 2014 Intel Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * The Eclipse Distribution License is available at
 *    http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    David Navarro, Intel Corporation - initial API and implementation
 *    domedambrosio - Please refer to git log
 *    Fabien Fleutot - Please refer to git log
 *    Axel Lorente - Please refer to git log
 *    Bosch Software Innovations GmbH - Please refer to git log
 *    Pascal Rieux - Please refer to git log
 *    Baijie & Longrong, China Mobile - Please refer to git log
 *******************************************************************************/

/*
 Copyright (c) 2013, 2014 Intel Corporation

 Redistribution and use in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:

     * Redistributions of source code must retain the above copyright notice,
       this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
     * Neither the name of Intel Corporation nor the names of its contributors
       may be used to endorse or promote products derived from this software
       without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 THE POSSIBILITY OF SUCH DAMAGE.

 David Navarro <david.navarro@intel.com>

*/

/*
 * This object is multiple instance only, and is mandatory to all LWM2M device as it describe the object such as human readable deviceinfo, etc...
 */

#include "../cis_api.h"
#include "../cis_internals.h"
#include "std_object.h"

#include "../dm_utils/dm_endpoint.h"
#include "../dm_utils/sha256.h"
#include "../dm_utils/j_base64.h"
#include "../dm_utils/aes.h"

#if CIS_ENABLE_DM

// Resource Id's:
#define LWM2M_DM_UE_OBJECT               666

#define RES_DM_DEV_INFO                  6601
#define RES_DM_APP_INFO                  6602
#define RES_DM_MAC                       6603
#define RES_DM_ROM                       6604
#define RES_DM_RAM                       6605
#define RES_DM_CPU                       6606
#define RES_DM_SYS_VERSION               6607
#define RES_DM_FIRM_VERSION              6608
#define RES_DM_FIRM_NAME                 6609
#define RES_DM_VOLTE                     6610

#define RES_DM_NET_TYPE                  6611
#define RES_DM_NET_ACCT                  6612
#define RES_DM_PHONE                     6613
#define RES_DM_LOCATION                  6614


#define RES_VALUE_BUFF_LEN  512

typedef struct _dm_device_data_
{
    struct _dm_device_data_ * next;        // matches st_list_t::next
    uint16_t             instanceId;  // matches st_list_t::id

    uint32_t state;
    uint32_t result;

    char dev_info[RES_VALUE_BUFF_LEN];
    char app_info[RES_VALUE_BUFF_LEN];
    char mac[RES_VALUE_BUFF_LEN];
    char rom[RES_VALUE_BUFF_LEN];
    char ram[RES_VALUE_BUFF_LEN];
    char cpu[RES_VALUE_BUFF_LEN];
    char sys_ver[RES_VALUE_BUFF_LEN];
    char firm_ver[RES_VALUE_BUFF_LEN];
    char firm_name[RES_VALUE_BUFF_LEN];
    char volte[RES_VALUE_BUFF_LEN];

    char net_type[RES_VALUE_BUFF_LEN];
    char net_acct[RES_VALUE_BUFF_LEN];
    char phone[RES_VALUE_BUFF_LEN];
    char location[RES_VALUE_BUFF_LEN];
} dm_device_data_t;

static dm_device_data_t * prv_dm_find(st_context_t * contextP,cis_iid_t instanceId)
{
    dm_device_data_t * targetP;
    targetP = (dm_device_data_t *)(std_object_get_dm(contextP, instanceId));

    if (NULL != targetP)
    {
        return targetP;
    }

    return NULL;
}

bool std_dm_create(st_context_t * contextP,
                       int instanceId,
                       st_object_t * dmObj)
{
    dm_device_data_t * instDm = NULL;
    dm_device_data_t * targetP = NULL;
    uint8_t instBytes = 0;
    uint8_t instCount = 0;
    cis_iid_t instIndex;
    if (NULL == dmObj)
    {
        return false;
    }

    // Manually create a hard-code instance
    targetP = (dm_device_data_t *)cissys_malloc(sizeof(dm_device_data_t));
    if (NULL == targetP)
    {
        return false;
    }

    cis_memset(targetP, 0, sizeof(dm_device_data_t));
    targetP->instanceId = (uint16_t)instanceId;

    targetP->state = 1;
    targetP->result = 0;
    strcpy(targetP->dev_info, "");
    strcpy(targetP->app_info, "2.0");

    instDm = (dm_device_data_t * )std_object_put_dm(contextP,(cis_list_t*)targetP);
    instCount = CIS_LIST_COUNT(instDm);
    if (instCount == 0)
    {
        return false;
    }

    /*first security object instance
     *don't malloc instance bitmap ptr*/
    if (instCount == 1)
    {
        return true;
    }

    dmObj->instBitmapCount = instCount;
    instBytes = (instCount - 1) / 8 + 1;
    if (dmObj->instBitmapBytes < instBytes){
        if (dmObj->instBitmapBytes != 0 && dmObj->instBitmapPtr != NULL)
        {
            cis_free(dmObj->instBitmapPtr);
        }
        dmObj->instBitmapPtr = (uint8_t*)cissys_malloc(instBytes);
        dmObj->instBitmapBytes = instBytes;
    }
    cissys_memset(dmObj->instBitmapPtr,0,instBytes);

    for (instIndex = 0;instIndex < instCount;instIndex++)
    {
        uint8_t instBytePos = targetP->instanceId / 8;
        uint8_t instByteOffset = 7 - (targetP->instanceId % 8);
        dmObj->instBitmapPtr[instBytePos] += 0x01 << instByteOffset;

        targetP = targetP->next;
    }
    return true;
}

uint8_t read_dm_device_info(int resId,char **outbuff)
{
    *outbuff = NULL;

    char buff[256] = { 0 };
    int  buflen = sizeof(buff);

    switch (resId)
    {
        case RES_DM_DEV_INFO://devinfo
            snprintf(buff,buflen,"%s", "");
            break;
        case RES_DM_APP_INFO://appinfo
            snprintf(buff,buflen,"app_name1|pak_name1|1234000|3\r\napp_name2|pak_name2|2234000|5\r\n");
            break;
        case RES_DM_MAC://mac
            snprintf(buff,buflen,"00:50:56:C0:00:01");
            break;
        case RES_DM_ROM://rom
            snprintf(buff,buflen,"4GB");
            break;
        case RES_DM_RAM://ram
            snprintf(buff,buflen,"2GB");
            break;
        case RES_DM_CPU://CPU
            snprintf(buff,buflen,"ARM");
            break;
        case RES_DM_SYS_VERSION://SYS VERSION
            snprintf(buff,buflen,"LINUX 2.6.18");
            break;
        case RES_DM_FIRM_VERSION://FIRMWARE VERSION
            snprintf(buff,buflen,"1.20.1");
            break;
        case RES_DM_FIRM_NAME://FIRMWARE NAME
            snprintf(buff,buflen,"X-A10");
            break;
        case RES_DM_VOLTE://Volte
            snprintf(buff,buflen,"0");
            break;
        case RES_DM_NET_TYPE://NetType
            snprintf(buff,buflen,"4G");
            break;
        case RES_DM_NET_ACCT://Net Account
            snprintf(buff,buflen,"Account");
            break;
        case 6613://PhoneNumber
            snprintf(buff,buflen,"13812345678");
            break;
        case RES_DM_LOCATION://Location
            snprintf(buff,buflen,"25.77701556036132,123.52958679200002,GPS");
            break;
        default:
            return COAP_404_NOT_FOUND;
    }

    *outbuff = (char*)malloc(strlen(buff)+1);
    cis_memset(*outbuff, 0x00, strlen(buff)+1);
    cis_memcpy(*outbuff, buff, strlen(buff));

    return COAP_205_CONTENT;
}

uint8_t get_dm_encode_info(uint16_t resId,char **outbuff, char *pwd)
{
    uint8_t result = COAP_205_CONTENT;
    *outbuff = NULL;

    char *szvalbuf = NULL;
    do
    {
        result = read_dm_device_info(resId, &szvalbuf);
        if (result != COAP_205_CONTENT)
        {
            return COAP_404_NOT_FOUND;
        }

        if (strlen((char *)szvalbuf) == 0)
        {
            result = COAP_IGNORE;
            break;
        }

        //data encode
        int outlen = dm_read_info_encode(szvalbuf, outbuff, pwd);
        if (outlen < 0)
        {
            result = COAP_500_INTERNAL_SERVER_ERROR;
        }
    }while(0);

    if (szvalbuf)
    {
        cissys_free(szvalbuf);
        szvalbuf = NULL;
    }

    LOGD("get_dm_encode_info get device info successfully");
    return result;
}

static uint8_t prv_dm_get_value(st_context_t * contextP,
                                    st_data_t * dataArrayP,
                                    int number,
                                    dm_device_data_t * devDataP)
{
    uint8_t result = 0;
    uint16_t resId;
    st_data_t *dataP;

    for (int i = 0; i < number; i++)
    {
        if (number == 1)
        {
            resId = dataArrayP->id;
            dataP = dataArrayP;
        }
        else
        {
            resId = dataArrayP[i].id;//dataArrayP->value.asChildren.array[i].id;
            dataP = dataArrayP+i;//dataArrayP->value.asChildren.array+i;
        }

        switch (resId)
        {
            case RES_DM_DEV_INFO:
            case RES_DM_APP_INFO:
            case RES_DM_MAC:
            case RES_DM_ROM:
            case RES_DM_RAM:
            case RES_DM_CPU:
            case RES_DM_SYS_VERSION:
            case RES_DM_FIRM_VERSION:
            case RES_DM_FIRM_NAME:
            case RES_DM_VOLTE:
            case RES_DM_NET_TYPE:
            case RES_DM_NET_ACCT:
            case RES_DM_PHONE:
            case RES_DM_LOCATION:
            {
                char *outbuff = NULL;
                result = get_dm_encode_info(resId, &outbuff, contextP->dm_pwd);
                if (COAP_205_CONTENT != result && COAP_IGNORE != result)
                {
                    return result;
                }

                if (result == COAP_205_CONTENT)
                {
                    data_encode_string(outbuff, dataP);
                }

                if (outbuff)
                {
                    cis_free(outbuff);
                }

                result = COAP_205_CONTENT;
                break;
            }

            default:
                return COAP_404_NOT_FOUND;
        }
    }
    return result;
}

uint8_t std_dm_read(st_context_t * contextP,
                        uint16_t instanceId,
                        int * numDataP,
                        st_data_t ** dataArrayP,
                        st_object_t * objectP)
{
    int i;
    uint8_t result;
    dm_device_data_t * targetP;

    targetP = prv_dm_find(contextP, instanceId);

    // this is a single instance object
    if (instanceId != 0 || targetP == NULL)
    {
        return COAP_404_NOT_FOUND;
    }

    // is the server asking for the full object ?
    if (*numDataP == 0)
    {
        uint16_t resList[] = {
                RES_DM_DEV_INFO,
                RES_DM_APP_INFO,
                RES_DM_MAC,
                RES_DM_ROM,
                RES_DM_RAM,
                RES_DM_CPU,
                RES_DM_SYS_VERSION,
                RES_DM_FIRM_VERSION,
                RES_DM_FIRM_NAME,
                RES_DM_VOLTE,
                RES_DM_NET_TYPE,
                RES_DM_NET_ACCT,
                RES_DM_PHONE,
                RES_DM_LOCATION,
        };

        int nbRes = sizeof(resList)/sizeof(uint16_t);

        *dataArrayP = data_new(nbRes);
        if (*dataArrayP == NULL) return COAP_500_INTERNAL_SERVER_ERROR;
        *numDataP = nbRes;

        for(i = 0; i < nbRes; i++)
        {
            (*dataArrayP)[i].id = resList[i];
        }

        if (prv_dm_get_value(contextP, (*dataArrayP), nbRes, targetP) != COAP_205_CONTENT)
        {
            return COAP_500_INTERNAL_SERVER_ERROR;
        }
        result = COAP_205_CONTENT;
    }
    else
    {
        result = prv_dm_get_value(contextP, (*dataArrayP), 1, targetP);
    }

    return result;
}

uint8_t std_dm_discover(st_context_t * contextP,
                            uint16_t instanceId,
                            int * numDataP,
                            st_data_t ** dataArrayP,
                            st_object_t * objectP)
{
    uint8_t result;
    int i;

    result = COAP_205_CONTENT;

    // is the server asking for the full object ?
    if (*numDataP == 0)
    {
        uint16_t resList[] = {
                RES_DM_DEV_INFO,
                RES_DM_APP_INFO,
                RES_DM_MAC,
                RES_DM_ROM,
                RES_DM_RAM,
                RES_DM_CPU,
                RES_DM_SYS_VERSION,
                RES_DM_FIRM_VERSION,
                RES_DM_FIRM_NAME,
                RES_DM_VOLTE,
                RES_DM_NET_TYPE,
                RES_DM_NET_ACCT,
                RES_DM_PHONE,
                RES_DM_LOCATION
        };
        int nbRes = sizeof(resList) / sizeof(uint16_t);

        (*dataArrayP)->id = 0;
        (*dataArrayP)->type = DATA_TYPE_OBJECT_INSTANCE;
        (*dataArrayP)->value.asChildren.count = nbRes;
        (*dataArrayP)->value.asChildren.array =  data_new(nbRes);
        memset((*dataArrayP)->value.asChildren.array,0,(nbRes)*sizeof(cis_data_t));
        if (*dataArrayP == NULL) return COAP_500_INTERNAL_SERVER_ERROR;
        *numDataP = nbRes;
        for (i = 0; i < nbRes; i++)
        {
             (*dataArrayP)->value.asChildren.array[i].id = resList[i];
             (*dataArrayP)->value.asChildren.array[i].type = DATA_TYPE_LINK;
             (*dataArrayP)->value.asChildren.array[i].value.asObjLink.objectId = CIS_DEVICE_OBJECT_ID;
             (*dataArrayP)->value.asChildren.array[i].value.asObjLink.instId = 0;
        }
    }
    else
    {
        for (i = 0; i < *numDataP && result == COAP_205_CONTENT; i++)
        {
            switch ((*dataArrayP)[i].id)
            {
                case RES_DM_DEV_INFO:
                case RES_DM_APP_INFO:
                case RES_DM_MAC:
                case RES_DM_ROM:
                case RES_DM_RAM:
                case RES_DM_CPU:
                case RES_DM_SYS_VERSION:
                case RES_DM_FIRM_VERSION:
                case RES_DM_FIRM_NAME:
                case RES_DM_VOLTE:
                case RES_DM_NET_TYPE:
                case RES_DM_NET_ACCT:
                case RES_DM_PHONE:
                case RES_DM_LOCATION:
                    break;
                default:
                    result = COAP_404_NOT_FOUND;
            }
        }
    }

    return result;
}

cis_coapret_t std_dm_write(st_context_t * contextP,
                               uint16_t instanceId,
                               int numData,
                               st_data_t * dataArray,
                               st_object_t * objectP)
{
    return COAP_400_BAD_REQUEST;
}

uint8_t std_dmd_execute(st_context_t * contextP,
                           uint16_t instanceId,
                           uint16_t resourceId,
                           uint8_t * buffer,
                           int length,
                           st_object_t * objectP)
{
    return COAP_400_BAD_REQUEST;
}

void std_dm_clean(st_context_t * contextP)
{
    dm_device_data_t * deleteInst;
    dm_device_data_t * dmInstance = NULL;
    dmInstance = (dm_device_data_t *)(contextP->dm_inst);
    while (dmInstance != NULL)
    {
        deleteInst = dmInstance;
        dmInstance = dmInstance->next;
        std_object_remove_dm(contextP,(cis_list_t*)(deleteInst));
        cis_free(deleteInst);
    }
}

#endif