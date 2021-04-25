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
 *    Julien Vermillard - initial implementation
 *    Fabien Fleutot - Please refer to git log
 *    David Navarro, Intel Corporation - Please refer to git log
 *    Bosch Software Innovations GmbH - Please refer to git log
 *    Pascal Rieux - Please refer to git log
 *    
 *******************************************************************************/

/*
 * This object is single instance only, and provide firmware upgrade functionality.
 * Object ID is 5.
 */

/*
 * resources:
 * 0 package                   write
 * 1 package url               write
 * 2 update                    exec
 * 3 state                     read
 * 4 update supported objects  read/write
 * 5 update result             read
 */

#include "liblwm2m.h"
#include "lwm2mclient.h"
#include "internals.h"
#include "lwm2m_api.h"
#ifdef CONFIG_LWM2M_FOTA_SUPPORT
#include "fupdate.h"
#include "vfs.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// ---- private object "Firmware" specific defines ----
// Resource Id's:
#define RES_M_PACKAGE                   0
#define RES_M_PACKAGE_URI               1
#define RES_M_UPDATE                    2
#define RES_M_STATE                     3
#define RES_O_UPDATE_SUPPORTED_OBJECTS  4
#define RES_M_UPDATE_RESULT             5
#define RES_O_PKG_NAME                  6
#define RES_O_PKG_VERSION               7

typedef struct
{
    uint8_t state;
    bool supported;
    uint8_t result;
} firmware_data_t;

lwm2m_fota_state_t g_fota_state;
static lwm2m_fota_result_t s_fota_resulte;

void execute_fota_upgrade();
#if 0
#ifdef LTE_NBIOT_SUPPORT
static uint8_t s_psm_state;
extern UINT8 nbiot_nvGetPsmEnable(void);
extern void nbiot_nvSetPsmEnable(UINT8 psmEnable);
extern void CFW_NvParasChangedNotification(UINT8  ChangedValue);
#endif
#endif
static uint8_t * prv_get_uri_path(uint8_t* uri)
{//"coap://192.168.188.200:8763/test/text1.file"
    uint8_t * path = (uint8_t *)strchr((const char *)uri, '/');
    if(path == NULL) return path;
    path = path + 2;
    path = (uint8_t *)strchr((const char *)path, '/');
    return path;
}
static uint8_t prv_firmware_read(uint16_t instanceId,
                                 int * numDataP,
                                 lwm2m_data_t ** dataArrayP,
                                 lwm2m_object_t * objectP)
{
    int i;
    uint8_t result;
    firmware_data_t * data = (firmware_data_t*)(objectP->userData);

    // this is a single instance object
    if (instanceId != 0)
    {
        return COAP_404_NOT_FOUND;
    }

    // is the server asking for the full object ?
    if (*numDataP == 0)
    {
        *dataArrayP = lwm2m_data_new(3);
        if (*dataArrayP == NULL) return COAP_500_INTERNAL_SERVER_ERROR;
        *numDataP = 3;
        (*dataArrayP)[0].id = 3;
        (*dataArrayP)[1].id = 4;
        (*dataArrayP)[2].id = 5;
    }

    i = 0;
    do
    {
        switch ((*dataArrayP)[i].id)
        {
        case RES_M_PACKAGE:
        case RES_M_PACKAGE_URI:
        case RES_M_UPDATE:
            result = COAP_405_METHOD_NOT_ALLOWED;
            break;

        case RES_M_STATE:
            // firmware update state (int)
            lwm2m_data_encode_int(g_fota_state, *dataArrayP + i);
            result = COAP_205_CONTENT;
            break;

        case RES_O_UPDATE_SUPPORTED_OBJECTS:
            lwm2m_data_encode_bool(data->supported, *dataArrayP + i);
            result = COAP_205_CONTENT;
            break;

        case RES_M_UPDATE_RESULT:
            lwm2m_data_encode_int(s_fota_resulte, *dataArrayP + i);
            result = COAP_205_CONTENT;
            break;

        default:
            result = COAP_404_NOT_FOUND;
        }

        i++;
    } while (i < *numDataP && result == COAP_205_CONTENT);

    return result;
}

static uint8_t prv_firmware_write(uint16_t instanceId,
                                  int numData,
                                  lwm2m_data_t * dataArray,
                                  lwm2m_object_t * objectP)
{
    int i;
    uint8_t result;
    firmware_data_t * data = (firmware_data_t*)(objectP->userData);
    lwm2m_context_t * lwm2mH = (lwm2m_context_t*)(objectP->lwm2mH);
    // this is a single instance object
    if (instanceId != 0)
    {
        return COAP_404_NOT_FOUND;
    }

    i = 0;

    do
    {
        switch (dataArray[i].id)
        {
        case RES_M_PACKAGE:
            // inline firmware binary
            result = COAP_204_CHANGED;
            break;

        case RES_M_PACKAGE_URI:
        {
            // URL for download the firmware
            lwm2m_data_t * data = &dataArray[i];
            int length = data->value.asBuffer.length;
            uint8_t * downloadURI = malloc(length+1);
            memcpy(downloadURI, data->value.asBuffer.buffer, length);
            downloadURI[length] = 0;
            LOG_ARG("get RES_M_PACKAGE_URI %d downloadURI %s",length,downloadURI);
            uint8_t * path = prv_get_uri_path(downloadURI);
            if(path != NULL)
            {
#if 0
#ifdef LTE_NBIOT_SUPPORT
                s_psm_state = nbiot_nvGetPsmEnable();
                if(s_psm_state != 0)
                {
                    nbiot_nvSetPsmEnable(0);
                    CFW_NvParasChangedNotification(1);
                }
#endif
#endif
                lwm2mH->fota_context.block2Num = 0;
                lwm2mH->fota_context.block2bufferSize = 0;
                notify_fota_state(LWM2M_FOTA_STATE_DOWNLOADING, LWM2M_FOTA_RESULT_INIT, lwm2mH->ref);
                lwm2m_start_fota_download(path, lwm2mH->ref);
            }else
            {
                notify_fota_state(LWM2M_FOTA_STATE_IDLE, LWM2M_FOTA_RESULT_INVALID_URI, lwm2mH->ref);
            }
            free(downloadURI);
            result = COAP_204_CHANGED;
        }
            break;

        case RES_O_UPDATE_SUPPORTED_OBJECTS:
            if (lwm2m_data_decode_bool(&dataArray[i], &data->supported) == 1)
            {
                result = COAP_204_CHANGED;
            }
            else
            {
                result = COAP_400_BAD_REQUEST;
            }
            break;

        default:
            result = COAP_405_METHOD_NOT_ALLOWED;
        }

        i++;
    } while (i < numData && result == COAP_204_CHANGED);

    return result;
}

static uint8_t prv_firmware_execute(uint16_t instanceId,
                                    uint16_t resourceId,
                                    uint8_t * buffer,
                                    int length,
                                    lwm2m_object_t * objectP)
{
    firmware_data_t * data = (firmware_data_t*)(objectP->userData);
    lwm2m_context_t * lwm2mH = (lwm2m_context_t*)(objectP->lwm2mH);

    // this is a single instance object
    if (instanceId != 0)
    {
        return COAP_404_NOT_FOUND;
    }

    if (length != 0) return COAP_400_BAD_REQUEST;

    // for execute callback, resId is always set.
    switch (resourceId)
    {
    case RES_M_UPDATE:
        if (data->state == 1)
        {
            fprintf(stdout, "\n\t FIRMWARE UPDATE\r\n\n");
            // trigger your firmware download and update logic
            LOG("FIRMWARE UPDATE \r\n");
            notify_fota_state(LWM2M_FOTA_STATE_UPDATING, LWM2M_FOTA_RESULT_SUCCESS, lwm2mH->ref);
            execute_fota_upgrade();
            data->state = 2;
            return COAP_204_CHANGED;
        }
        else
        {
            // firmware update already running
            return COAP_400_BAD_REQUEST;
        }
    default:
        return COAP_405_METHOD_NOT_ALLOWED;
    }
}

void display_firmware_object(lwm2m_object_t * object)
{
#ifdef WITH_LOGS
    firmware_data_t * data = (firmware_data_t *)object->userData;
    fprintf(stdout, "  /%u: Firmware object:\r\n", object->objID);
    if (NULL != data)
    {
        fprintf(stdout, "    state: %u, supported: %s, result: %u\r\n",
                data->state, data->supported?"true":"false", data->result);
    }
#endif
}

static void free_object_firmware(lwm2m_object_t * objectP)
{
    if (NULL != objectP->userData)
    {
        lwm2m_free(objectP->userData);
        objectP->userData = NULL;
    }
    if (NULL != objectP->instanceList)
    {
        lwm2m_free(objectP->instanceList);
        objectP->instanceList = NULL;
    }
    //lwm2m_free(objectP);
}

lwm2m_object_t * get_object_firmware(void * lwm2mH)
{
    /*
     * The get_object_firmware function create the object itself and return a pointer to the structure that represent it.
     */
    lwm2m_object_t * firmwareObj;

    firmwareObj = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));

    if (NULL != firmwareObj)
    {
        memset(firmwareObj, 0, sizeof(lwm2m_object_t));

        /*
         * It assigns its unique ID
         * The 5 is the standard ID for the optional object "Object firmware".
         */
        firmwareObj->objID = LWM2M_FIRMWARE_UPDATE_OBJECT_ID;
        firmwareObj->lwm2mH = lwm2mH;

        /*
         * and its unique instance
         *
         */
        firmwareObj->instanceList = (lwm2m_list_t *)lwm2m_malloc(sizeof(lwm2m_list_t));
        if (NULL != firmwareObj->instanceList)
        {
            memset(firmwareObj->instanceList, 0, sizeof(lwm2m_list_t));
        }
        else
        {
            lwm2m_free(firmwareObj);
            return NULL;
        }

        /*
         * And the private function that will access the object.
         * Those function will be called when a read/write/execute query is made by the server. In fact the library don't need to
         * know the resources of the object, only the server does.
         */
        firmwareObj->readFunc    = prv_firmware_read;
        firmwareObj->writeFunc   = prv_firmware_write;
        firmwareObj->executeFunc = prv_firmware_execute;
        firmwareObj->cleanFunc = free_object_firmware;
        firmwareObj->userData    = lwm2m_malloc(sizeof(firmware_data_t));

        /*
         * Also some user data can be stored in the object with a private structure containing the needed variables
         */
        if (NULL != firmwareObj->userData)
        {
            ((firmware_data_t*)firmwareObj->userData)->state = 1;
            ((firmware_data_t*)firmwareObj->userData)->supported = false;
            ((firmware_data_t*)firmwareObj->userData)->result = 0;
        }
        else
        {
            lwm2m_free(firmwareObj);
            firmwareObj = NULL;
        }
    }

    return firmwareObj;
}

void notify_fota_state(lwm2m_fota_state_t state, lwm2m_fota_result_t resulte, uint8_t ref)
{
    g_fota_state = state;
    s_fota_resulte = resulte;
    LOG_ARG("notify_fota_state state %d resulte %d\r\n", state, resulte);
    if(state == LWM2M_FOTA_STATE_IDLE && (resulte == LWM2M_FOTA_RESULT_SUCCESS || resulte == LWM2M_FOTA_RESULT_INIT))
    {
        LOG_ARG( "get fota err state %d resulte %d\r\n", state, resulte);
#if 0
#ifdef LTE_NBIOT_SUPPORT
        if(s_psm_state != 0)
        {
            nbiot_nvSetPsmEnable(s_psm_state);
            CFW_NvParasChangedNotification(1);
        }
#endif
#endif
    }
    lwm2m_notify_fota_state(state, resulte, ref);

}

uint32_t write_fota_upgrade_data(uint32_t block_num, uint8_t  block_more, uint8_t * data, uint16_t datalen)
{
#ifdef CONFIG_LWM2M_FOTA_SUPPORT
    static int fd = 0;
    if (0==block_num)
    {
        fupdateInvalidate(true);
        vfs_mkdir(CONFIG_FS_FOTA_DATA_DIR, 0);
        fd  = vfs_open(FUPDATE_PACK_FILE_NAME, O_RDWR | O_CREAT | O_TRUNC | O_APPEND, 0);
        if (fd < 0)
            return COAP_IGNORE;
    }

    if (fd <= 0)
        return COAP_IGNORE;

    ssize_t write_len = vfs_write(fd, data, datalen);
    if(write_len != datalen)
    {
      vfs_close(fd);
      fd = 0;
      LOG("write_fota_upgrade_data vfs_write failed\r\n");
      return COAP_IGNORE;
    }
    if(!block_more)
    {
        vfs_close(fd);
        fd = 0;
    }
#endif
    return COAP_NO_ERROR;
}

bool check_fota_file_sanity()
{
    LOG("check_fota_file_sanity \r\n");
 #ifdef CONFIG_LWM2M_FOTA_SUPPORT
    return fupdateIsPackValid(NULL);
 #else
    return true;
 #endif
}

void execute_fota_upgrade()
{
 #ifdef CONFIG_LWM2M_FOTA_SUPPORT
    bool result = fupdateSetReady(NULL);

    if (result)
    {
        LOG("execute_fota_upgrade swithoff \r\n");
        g_reboot = 1;
    }
    else
    {
         fupdateInvalidate(true);
    }
#endif
}

