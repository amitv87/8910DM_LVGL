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

#include "../cis_api.h"
#include "../cis_internals.h"
#include "std_object.h"

#if CIS_ENABLE_UPDATE || CIS_ENABLE_MONITER
/*
 * resources:
 * 2 Radio Signal Strength                   read
 * 8 Cell ID                                 read
 */


#define PRV_OFFSET_MAXLEN   7 //+HH:MM\0 at max
#define PRV_TLV_BUFFER_SIZE 128

#define PRV_VERSION_MAXLEN   16 //+HH:MM\0 at max


typedef struct _conn_moniter_data_
{
	struct _conn_moniter_data_ * next;        // matches st_list_t::next
	uint16_t             instanceId;  // matches st_list_t::id
    uint32_t radio_signal_strength;
	uint32_t cell_id;
} conn_moniter_data_t;

bool std_conn_moniter_create(st_context_t * contextP,
					   int instanceId,
                       st_object_t * connObj)
{
    conn_moniter_data_t * instConn=NULL;
    conn_moniter_data_t * targetP = NULL;
    uint8_t instBytes = 0;
    uint8_t instCount = 0;
    cis_iid_t instIndex;
    if (NULL == connObj)
    {
        return false;   
    }

    // Manually create a hard-code instance
    targetP = (conn_moniter_data_t *)cis_malloc(sizeof(conn_moniter_data_t));
    if (NULL == targetP)
    {
        return false;
    }

    cis_memset(targetP, 0, sizeof(conn_moniter_data_t));
    targetP->instanceId = (uint16_t)instanceId;

    targetP->radio_signal_strength = 0;
	targetP->cell_id = 0;

    instConn = (conn_moniter_data_t * )std_object_put_conn(contextP,(cis_list_t*)targetP);
    
    instCount = CIS_LIST_COUNT(instConn);
    if(instCount == 0)
    {
        cis_free(targetP);
        return false;
    }

    if(instCount == 1)
    {
        return true;
    }


    connObj->instBitmapCount = instCount;
    instBytes = (instCount - 1) / 8 + 1;
    if(connObj->instBitmapBytes < instBytes){
        if(connObj->instBitmapBytes != 0 && connObj->instBitmapPtr != NULL)
        {
            cis_free(connObj->instBitmapPtr);
        }
        connObj->instBitmapPtr = (uint8_t*)cis_malloc(instBytes);
        connObj->instBitmapBytes = instBytes;
    }
    cissys_memset(connObj->instBitmapPtr,0,instBytes);
    targetP = instConn;
    for (instIndex = 0;instIndex < instCount;instIndex++)
    {
        uint8_t instBytePos = targetP->instanceId / 8;
        uint8_t instByteOffset = 7 - (targetP->instanceId % 8);
        connObj->instBitmapPtr[instBytePos] += 0x01 << instByteOffset;

        targetP = targetP->next;
    }
    return true;
}

static uint8_t prv_conn_moniter_get_value(st_context_t * contextP,
									st_data_t * dataArrayP,
									int number,
									conn_moniter_data_t * devDataP)
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
		case RES_CONN_RADIO_SIGNAL_STRENGTH_ID:
			{
				uint32_t radio_signal_strenght = cissys_getRadioSignalStrength();
				data_encode_int(radio_signal_strenght, dataP);
				result = COAP_205_CONTENT;
				break;
			}

		case RES_CONN_CELL_ID:
			{
				uint32_t cell_id = cissys_getCellId();
				data_encode_int(cell_id, dataP);

				result = COAP_205_CONTENT;
				break;
			}
#if CIS_ENABLE_MONITER
/*
		case RES_CONN_RSRP:
		{
			int8_t rsrp = cissys_getRSRP();
			if (rsrp != CIS_RET_UNSUPPORTIVE)
			{
				data_encode_int(rsrp, dataP);
			}
			break;
		}
*/		
		case RES_CONN_SINR:
		{
			int8_t sinr = cissys_getSINR();
			if (sinr != CIS_RET_UNSUPPORTIVE)
			{
				data_encode_int(sinr, dataP);
			}
			break;
		}
#endif
		default:
			return COAP_404_NOT_FOUND;
		}
	}
	return result;
}
                                    
#if 0
static conn_moniter_data_t * prv_conn_moniter_find(st_context_t * contextP,cis_iid_t instanceId)
{
	conn_moniter_data_t * targetP;
	targetP = (conn_moniter_data_t *)(std_object_get_conn(contextP, instanceId));

	if (NULL != targetP)
	{
		return targetP;
	}

	return NULL;
}
#endif

uint8_t std_conn_moniter_read(st_context_t * contextP,
						uint16_t instanceId,
						int * numDataP,
						st_data_t ** dataArrayP,
						st_object_t * objectP)
{
	uint8_t result = COAP_205_CONTENT;
	int i;
	/*
		conn_moniter_data_t * targetP;
		targetP = prv_conn_moniter_find(contextP,instanceId);
		// this is a single instance object
		if (instanceId != 0||targetP==NULL)
		{
			return COAP_404_NOT_FOUND;
		}
	*/
	// is the server asking for the full object ?
	if (*numDataP == 0)
	{
		uint16_t resList[] = {
				RES_CONN_RADIO_SIGNAL_STRENGTH_ID,
#if CIS_ENABLE_MONITER					
				RES_CONN_CELL_ID,
//				RES_CONN_RSRP,
				RES_CONN_SINR
#else
				RES_CONN_CELL_ID
#endif
		};
		int nbRes = sizeof(resList) / sizeof(uint16_t);
		(*dataArrayP)->id = 0;
		(*dataArrayP)->type = DATA_TYPE_OBJECT_INSTANCE;
		(*dataArrayP)->value.asChildren.count = nbRes;
		(*dataArrayP)->value.asChildren.array = data_new(nbRes);
		cis_memset((*dataArrayP)->value.asChildren.array, 0, (nbRes) * sizeof(cis_data_t));
		if (*dataArrayP == NULL) return COAP_500_INTERNAL_SERVER_ERROR;
		*numDataP = nbRes;

		if (*dataArrayP == NULL) return COAP_500_INTERNAL_SERVER_ERROR;
		*numDataP = nbRes;
		for (i = 0; i < nbRes; i++)
		{
			(*dataArrayP)->value.asChildren.array[i].id = resList[i];
		}

		if (prv_conn_moniter_get_value(contextP, (*dataArrayP), nbRes, NULL) != COAP_205_CONTENT)
		{
			return COAP_500_INTERNAL_SERVER_ERROR;
		}
	}
	else
	{
		result = prv_conn_moniter_get_value(contextP, (*dataArrayP), 1, NULL);
	}

	return result;
}

uint8_t std_conn_moniter_discover(st_context_t * contextP,
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
            RES_CONN_RADIO_SIGNAL_STRENGTH_ID,
#if CIS_ENABLE_MONITER					
            RES_CONN_CELL_ID,
//            RES_CONN_RSRP,
            RES_CONN_SINR
#else
            RES_CONN_CELL_ID
#endif
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
			 (*dataArrayP)->value.asChildren.array[i].value.asObjLink.objectId = CIS_CONNECTIVITY_OBJECT_ID;
			 (*dataArrayP)->value.asChildren.array[i].value.asObjLink.instId = 0;
        }
    }
    else
    {
        for (i = 0; i < *numDataP && result == COAP_205_CONTENT; i++)
        {
            switch ((*dataArrayP)[i].id)
            {
            case RES_CONN_RADIO_SIGNAL_STRENGTH_ID:
            case RES_CONN_CELL_ID:
                break;
            default:
                result = COAP_404_NOT_FOUND;
            }
        }
    }

    return result;
}

cis_coapret_t std_conn_moniter_write(st_context_t * contextP,
							   uint16_t instanceId,
                               int numData,
                               st_data_t * dataArray,
                               st_object_t * objectP)
{
    return COAP_400_BAD_REQUEST;
}

uint8_t std_conn_moniter_execute(st_context_t * contextP,
						   uint16_t instanceId,
                           uint16_t resourceId,
                           uint8_t * buffer,
						   int length,
                           st_object_t * objectP)
{
    return COAP_400_BAD_REQUEST;
}

void std_conn_moniter_clean(st_context_t * contextP)
{
	conn_moniter_data_t * deleteInst;
	conn_moniter_data_t * connInstance = NULL;
	connInstance = (conn_moniter_data_t *)(contextP->conn_inst);
	while (connInstance != NULL)
	{
		deleteInst = connInstance;
		connInstance = connInstance->next;
		std_object_remove_conn(contextP,(cis_list_t*)(deleteInst));
		cis_free(deleteInst);
	}
}
#endif
