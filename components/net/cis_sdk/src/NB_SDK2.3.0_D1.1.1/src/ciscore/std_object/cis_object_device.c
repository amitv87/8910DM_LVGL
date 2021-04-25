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
 * This object is single instance only, and is mandatory to all LWM2M device as it describe the object such as its
 * manufacturer, model, etc...
 */

#include "../cis_api.h"
#include "../cis_internals.h"
#include "std_object.h"

#if CIS_ENABLE_UPDATE
/*
 * resources:
 * 3 battery                   read
 * 5 freeMomery                read
 * 8 firmversion               read
 * 26260 voltage               read
 */

#define PRV_MANUFACTURER      "Open Mobile Alliance"
#define PRV_MODEL_NUMBER      "Lightweight M2M Client"
#define PRV_SERIAL_NUMBER     "345000123"
#define PRV_FIRMWARE_VERSION  "1.0"
#define PRV_POWER_SOURCE_1    1
#define PRV_POWER_SOURCE_2    5
#define PRV_POWER_VOLTAGE_1   3800
#define PRV_POWER_VOLTAGE_2   5000
#define PRV_POWER_CURRENT_1   125
#define PRV_POWER_CURRENT_2   900
#define PRV_BATTERY_LEVEL     100
#define PRV_MEMORY_FREE       15
#define PRV_ERROR_CODE        0
#define PRV_TIME_ZONE         "Europe/Berlin"
#define PRV_BINDING_MODE      "U"

#define PRV_OFFSET_MAXLEN   7 //+HH:MM\0 at max
#define PRV_TLV_BUFFER_SIZE 128

#define PRV_VERSION_MAXLEN   16 //+HH:MM\0 at max


typedef struct _device_data_
{
	struct _device_data_ * next;        // matches st_list_t::next
	uint16_t             instanceId;  // matches st_list_t::id
    uint32_t free_memory;
    uint32_t battery_level;
	uint32_t  battery_voltage;
    char * version;
} device_data_t;

bool std_device_create(st_context_t * contextP,
					   int instanceId,
                       st_object_t * deviceObj)
{
    device_data_t * instDevice=NULL;
    device_data_t * targetP = NULL;
    uint8_t instBytes = 0;
    uint8_t instCount = 0;
    cis_iid_t instIndex;
    if (NULL == deviceObj)
    {
        return false;   
    }

    // Manually create a hard-code instance
    targetP = (device_data_t *)cis_malloc(sizeof(device_data_t));
    if (NULL == targetP)
    {
        return false;
    }

    cis_memset(targetP, 0, sizeof(device_data_t));
    targetP->instanceId = (uint16_t)instanceId;
    targetP->version = (char *)cis_malloc(PRV_VERSION_MAXLEN); 
    cis_memset(targetP->version,0,PRV_VERSION_MAXLEN);
	//targetP->version = PRV_FIRMWARE_VERSION;

    targetP->free_memory = 0;
    targetP->battery_level = 0;
	targetP->battery_voltage = 0;

    instDevice = (device_data_t * )std_object_put_device(contextP,(cis_list_t*)targetP);
    
    instCount = CIS_LIST_COUNT(instDevice);
    if(instCount == 0)
    {
        cis_free(targetP->version);
        cis_free(targetP);
        return false;
    }

    /*first security object instance
     *don't malloc instance bitmap ptr*/
    if(instCount == 1)
    {
        return true;
    }


    deviceObj->instBitmapCount = instCount;
    instBytes = (instCount - 1) / 8 + 1;
    if(deviceObj->instBitmapBytes < instBytes){
        if(deviceObj->instBitmapBytes != 0 && deviceObj->instBitmapPtr != NULL)
        {
            cis_free(deviceObj->instBitmapPtr);
        }
        deviceObj->instBitmapPtr = (uint8_t*)cis_malloc(instBytes);
        deviceObj->instBitmapBytes = instBytes;
    }
    cissys_memset(deviceObj->instBitmapPtr,0,instBytes);
    targetP = instDevice;
    for (instIndex = 0;instIndex < instCount;instIndex++)
    {
        uint8_t instBytePos = targetP->instanceId / 8;
        uint8_t instByteOffset = 7 - (targetP->instanceId % 8);
        deviceObj->instBitmapPtr[instBytePos] += 0x01 << instByteOffset;

        targetP = targetP->next;
    }
    return true;
}

static uint8_t prv_device_get_value(st_context_t * contextP,
									st_data_t * dataArrayP,
									int number,
									device_data_t * devDataP)
{
	uint8_t result = COAP_404_NOT_FOUND;
	uint16_t resId;
	st_data_t *dataP;

	for (int i = 0; i<number;i++)
	{
		if(number == 1)
		{
			resId = dataArrayP->id;
			dataP = dataArrayP;
		}
		else
		{
			resId = dataArrayP->value.asChildren.array[i].id;
			dataP = dataArrayP->value.asChildren.array+i;
		}
		switch (resId)//need do error handle
		{
		case RES_O_FIRMWARE_VERSION:
        {
				//char* version = NULL;
				//version = (char*)cis_malloc(PRV_VERSION_MAXLEN);
				if(devDataP->version == NULL) return 0;
				//cis_memset(version,0,PRV_VERSION_MAXLEN);
				if (cissys_getFwVersion((uint8_t **)&(devDataP->version))>0)
				{
					//devDataP->version = version;
					data_encode_string(devDataP->version,dataP);
				}
				result = COAP_205_CONTENT;
				break;
		}

		case RES_O_BATTERY_LEVEL:
			{
				uint32_t batteryLevel = cissys_getFwBatteryLevel();
				if (batteryLevel>0)
				{
					data_encode_int(batteryLevel, dataP);
				}
				result = COAP_205_CONTENT;
				break;
			}
		case RES_O_BATTERY_VOLTAGE:
			{
				uint32_t batteryVoltage = cissys_getFwBatteryVoltage();
				if (batteryVoltage>0)
				{
					data_encode_int(batteryVoltage, dataP);
				}
				result = COAP_205_CONTENT;
				break;
			}
		case RES_O_MEMORY_FREE:
			{
				uint32_t freeMemory = cissys_getFwAvailableMemory();
				if (freeMemory>0)
				{
					data_encode_int(freeMemory, dataP);
				}
				result = COAP_205_CONTENT;
				break;
			}
#if CIS_ENABLE_UPDATE_MCU
       	case RES_O_MEMORY_FREE_MCU:
		  {
				uint32_t McufreeMemory = cissys_getSotaMemory();
				if (McufreeMemory>0)
				{
					data_encode_int(McufreeMemory, dataP);
				}
				result = COAP_205_CONTENT;
				break;
		 }
		case RES_O_SOFTWARE_VERSION:
        {

				if(devDataP->version == NULL) return 0;
				if (cissys_getSotaVersion((uint8_t **)&(devDataP->version))>0)
				{
					data_encode_string(devDataP->version,dataP);
				}
				result = COAP_205_CONTENT;
				break;
		}
#endif
		default:
			return COAP_404_NOT_FOUND;
		}
	}   
    return result;
}
static device_data_t * prv_device_find(st_context_t * contextP,cis_iid_t instanceId)
{
	device_data_t * targetP;
	targetP = (device_data_t *)(std_object_get_device(contextP, instanceId));

	if (NULL != targetP)
	{
		return targetP;
	}

	return NULL;
}

uint8_t std_device_read(st_context_t * contextP,
						uint16_t instanceId,
                        int * numDataP,
                        st_data_t ** dataArrayP,
                        st_object_t * objectP)
{
    uint8_t result = COAP_205_CONTENT;
    int i;
	device_data_t * targetP;
	targetP = prv_device_find(contextP,instanceId);
    // this is a single instance object
    if (instanceId != 0||targetP==NULL)
    {
        return COAP_404_NOT_FOUND;
    }

    // is the server asking for the full object ?
    if (*numDataP == 0)
    {
        uint16_t resList[] = {
                RES_O_FIRMWARE_VERSION,
                RES_O_BATTERY_LEVEL,
				RES_O_BATTERY_VOLTAGE,
			#if CIS_ENABLE_UPDATE_MCU
				RES_O_SOFTWARE_VERSION,
                RES_O_MEMORY_FREE_MCU,
            #endif    
                RES_O_MEMORY_FREE
              
        };
        int nbRes = sizeof(resList)/sizeof(uint16_t);
		(*dataArrayP)->id = 0;
		(*dataArrayP)->type = DATA_TYPE_OBJECT_INSTANCE;
		(*dataArrayP)->value.asChildren.count = nbRes;
		(*dataArrayP)->value.asChildren.array =  data_new(nbRes);
		cis_memset((*dataArrayP)->value.asChildren.array,0,(nbRes)*sizeof(cis_data_t));
		if (*dataArrayP == NULL) return COAP_500_INTERNAL_SERVER_ERROR;
		*numDataP = nbRes;

        if (*dataArrayP == NULL) return COAP_500_INTERNAL_SERVER_ERROR;
        *numDataP = nbRes;
        for (i = 0 ; i < nbRes ; i++)
        {
			(*dataArrayP)->value.asChildren.array[i].id = resList[i];
        }

		if (prv_device_get_value(contextP,(*dataArrayP),nbRes,targetP)!=COAP_205_CONTENT)
		{
			return COAP_500_INTERNAL_SERVER_ERROR;
		}
    }
	else
	{
		result = prv_device_get_value(contextP,(*dataArrayP),1,targetP);
	}

    return result;
}

uint8_t std_device_discover(st_context_t * contextP,
							uint16_t instanceId,
                            int * numDataP,
                            st_data_t ** dataArrayP,
                            st_object_t * objectP)
{
    uint8_t result;
    int i;

    // this is a single instance object
    if (instanceId != 0)
    {
        return COAP_404_NOT_FOUND;
    }

    result = COAP_205_CONTENT;

    // is the server asking for the full object ?
    if (*numDataP == 0)
    {
        uint16_t resList[] = {
            RES_O_FIRMWARE_VERSION,
            RES_O_BATTERY_LEVEL,
			RES_O_BATTERY_VOLTAGE,
		#if CIS_ENABLE_UPDATE_MCU
            RES_O_SOFTWARE_VERSION,   
            RES_O_MEMORY_FREE_MCU,	
         #endif	
            RES_O_MEMORY_FREE
        };
        int nbRes = sizeof(resList) / sizeof(uint16_t);

		(*dataArrayP)->id = 0;
		(*dataArrayP)->type = DATA_TYPE_OBJECT_INSTANCE;
		(*dataArrayP)->value.asChildren.count = nbRes;
		(*dataArrayP)->value.asChildren.array =  data_new(nbRes);
		cis_memset((*dataArrayP)->value.asChildren.array,0,(nbRes)*sizeof(cis_data_t));
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
            case RES_O_FIRMWARE_VERSION:
            case RES_O_BATTERY_LEVEL:
			case RES_O_BATTERY_VOLTAGE:
            case RES_O_MEMORY_FREE:
		#if CIS_ENABLE_UPDATE_MCU
            case RES_O_SOFTWARE_VERSION:   
            case RES_O_MEMORY_FREE_MCU:	
         #endif				
                break;
            default:
                result = COAP_404_NOT_FOUND;
            }
        }
    }

    return result;
}

cis_coapret_t std_device_write(st_context_t * contextP,
							   uint16_t instanceId,
                               int numData,
                               st_data_t * dataArray,
                               st_object_t * objectP)
{
    return COAP_400_BAD_REQUEST;
}

uint8_t std_device_execute(st_context_t * contextP,
						   uint16_t instanceId,
                           uint16_t resourceId,
                           uint8_t * buffer,
						   int length,
                           st_object_t * objectP)
{
    return COAP_400_BAD_REQUEST;
}

void std_device_clean(st_context_t * contextP)
{
	device_data_t * deleteInst;
	device_data_t * deviceInstance = NULL;
	deviceInstance = (device_data_t *)(contextP->device_inst);
	while (deviceInstance != NULL)
	{
		deleteInst = deviceInstance;
		deviceInstance = deviceInstance->next;
		std_object_remove_device(contextP,(cis_list_t*)(deleteInst));
		if(deleteInst->version!=NULL) 
		cis_free(deleteInst->version);
		cis_free(deleteInst);
	}
}
#endif
