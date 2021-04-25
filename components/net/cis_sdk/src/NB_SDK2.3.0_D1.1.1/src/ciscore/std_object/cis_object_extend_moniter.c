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

#if CIS_ENABLE_MONITER


typedef struct _moniter_data_
{
	struct _moniter_data_ * next;        // matches st_list_t::next
	uint16_t                instanceId;  // matches st_list_t::id
    uint8_t                 networkBearer;
    uint8_t                 linkUtilization;
    uint8_t                 csq;
    uint8_t                 ecl;
    uint8_t                 snr;
    uint16_t                pci;
    uint32_t                ecgi;
    uint16_t                earfcn;
    uint8_t                 pciFrist;
    uint8_t                 pciSecond;
    int16_t                 rsrpFrist;
    int16_t                 rsrpSecond;
} prv_moniter_data_t;



bool std_moniter_create(st_context_t * contextP,
					   int instanceId,
                       st_object_t * moniterObj)
{
    prv_moniter_data_t * instMoniter=NULL;
    prv_moniter_data_t * targetP = NULL;
    uint8_t instBytes = 0;
    uint8_t instCount = 0;
    cis_iid_t instIndex;
    if (NULL == moniterObj)
    {
        return false;   
    }

    // Manually create a hard-code instance
    targetP = (prv_moniter_data_t *)cis_malloc(sizeof(prv_moniter_data_t));
    if (NULL == targetP)
    {
        return false;
    }

    cis_memset(targetP, 0, sizeof(prv_moniter_data_t));
    targetP->instanceId = (uint16_t)instanceId;
    
    targetP->networkBearer = 0;
    targetP->linkUtilization = 0;
    targetP->csq = 0;
    targetP->ecl = 0;
    targetP->snr = 0;
    targetP->pci = 0;
    targetP->ecgi = 0;
    targetP->earfcn = 0;
    targetP->pciFrist = 0;
    targetP->pciSecond = 0;
    targetP->rsrpFrist = 0;
    targetP->rsrpSecond = 0;


    instMoniter = (prv_moniter_data_t * )std_object_put_moniter(contextP,(cis_list_t*)targetP);
    
    instCount = CIS_LIST_COUNT(instMoniter);
    if(instCount == 0)
    {
        cis_free(targetP);
        return false;
    }

    if(instCount == 1)
    {
        return true;
    }


    moniterObj->instBitmapCount = instCount;
    instBytes = (instCount - 1) / 8 + 1;
    if(moniterObj->instBitmapBytes < instBytes){
        if(moniterObj->instBitmapBytes != 0 && moniterObj->instBitmapPtr != NULL)
        {
            cis_free(moniterObj->instBitmapPtr);
        }
        moniterObj->instBitmapPtr = (uint8_t*)cis_malloc(instBytes);
        moniterObj->instBitmapBytes = instBytes;
    }
    cissys_memset(moniterObj->instBitmapPtr,0,instBytes);
    targetP = instMoniter;
    for (instIndex = 0;instIndex < instCount;instIndex++)
    {
        uint8_t instBytePos = targetP->instanceId / 8;
        uint8_t instByteOffset = 7 - (targetP->instanceId % 8);
        moniterObj->instBitmapPtr[instBytePos] += 0x01 << instByteOffset;

        targetP = targetP->next;
    }
    return true;
}



void std_moniter_clean(st_context_t * contextP)
{
    prv_moniter_data_t * deleteInst;
    prv_moniter_data_t * connInstance = NULL;
    connInstance = (prv_moniter_data_t *)(contextP->moniter_inst);
    while (connInstance != NULL)
    {
        deleteInst = connInstance;
        connInstance = connInstance->next;
        std_object_remove_moniter(contextP,(cis_list_t*)(deleteInst));
        cis_free(deleteInst);
    }
}



static uint8_t prv_moniter_get_value(st_context_t * contextP,
									st_data_t * dataArrayP,
									int number,
									prv_moniter_data_t * devDataP)
{
	uint8_t result = COAP_205_CONTENT;
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

		switch (resId)
		{
		case RES_MONITER_NETWORK_BEARER:
			{
                int8_t bearer = cissys_getBearer();
                if(bearer != CIS_RET_UNSUPPORTIVE)
                {
                    data_encode_int(bearer, dataP);
                }
				break;
			}

		case RES_MONITER_LINK_UTILIZATION:
            {
                int8_t utilization = cissys_getUtilization();
                if(utilization != CIS_RET_UNSUPPORTIVE)
                {
                    data_encode_int(utilization , dataP);
                }
                
                break;
			}
    
        case RES_MONITER_CSQ:
            {
                int8_t csq = cissys_getCSQ();
                if(csq != CIS_RET_UNSUPPORTIVE)
                {
                    data_encode_int(csq, dataP);
                }
                break;
            }
        case RES_MONITER_ECL:
            {
                int8_t ecl = cissys_getECL();
                if(ecl != CIS_RET_UNSUPPORTIVE)
                {
                    data_encode_int(ecl, dataP);
                }
                break;
            }
        case RES_MONITER_SNR:
            {
                int8_t snr = cissys_getSNR();
                if(snr != CIS_RET_UNSUPPORTIVE)
                {
                    data_encode_int(snr, dataP);
                }
                break;
            }
        case RES_MONITER_PCI:
            {
                int8_t pci = cissys_getPCI();
                if(pci != CIS_RET_UNSUPPORTIVE)
                {
                    data_encode_int(pci, dataP);
                }
                break;
            }
        case RES_MONITER_ECGI:
            {
                int8_t ecgi = cissys_getECGI();
                if(ecgi != CIS_RET_UNSUPPORTIVE)
                {
                    data_encode_int(ecgi, dataP);
                }
                break;
            }
        case RES_MONITER_EARFCN:
            {
                int8_t earfcn = cissys_getEARFCN();
                if(earfcn != CIS_RET_UNSUPPORTIVE)
                {
                    data_encode_int(cissys_getEARFCN(), dataP);
                }
                break;
            }
        case RES_MONITER_PCI_FIRST:
            {
                int8_t pciFrist = cissys_getPCIFrist();
                if(pciFrist != CIS_RET_UNSUPPORTIVE)
                {
                    data_encode_int(pciFrist, dataP);
                }
                break;
            }
        case RES_MONITER_PCI_SECOND:
            {
                int8_t pciSecond = cissys_getPCISecond();
                if(pciSecond != CIS_RET_UNSUPPORTIVE)
                {
                    data_encode_int(pciSecond, dataP);
                }
                break;
            }
        case RES_MONITER_RSRP_FIRST:
            {
                int8_t rsrpFrist = cissys_getRSRPFrist();
                if(rsrpFrist != CIS_RET_UNSUPPORTIVE)
                {
                    data_encode_int(rsrpFrist, dataP);
                }
                break;
            }
        case RES_MONITER_RSRP_SECOND:
            {
                int8_t rsrpSecond = cissys_getRSRPSecond();
                if(rsrpSecond != CIS_RET_UNSUPPORTIVE)
                {
                    data_encode_int(rsrpSecond, dataP);
                }
                break;
            }
		case RES_MONITER_PLMN:
		{
			int8_t rsrpSecond = cissys_getPLMN();
			if (rsrpSecond != CIS_RET_UNSUPPORTIVE)
			{
				data_encode_int(rsrpSecond, dataP);
			}
			break;
		}
		default:
			return COAP_404_NOT_FOUND;
		}
	} 
    return result;
}
static prv_moniter_data_t * prv_moniter_find(st_context_t * contextP,cis_iid_t instanceId)
{
	prv_moniter_data_t * targetP;
	targetP = (prv_moniter_data_t *)(std_object_get_moniter(contextP, instanceId));

	if (NULL != targetP)
	{
		return targetP;
	}

	return NULL;
}

uint8_t std_moniter_read(st_context_t * contextP,
						uint16_t instanceId,
                        int * numDataP,
                        st_data_t ** dataArrayP,
                        st_object_t * objectP)
{
    uint8_t result = COAP_205_CONTENT;
    int i;
	prv_moniter_data_t * targetP;
	targetP = prv_moniter_find(contextP,instanceId);
    // this is a single instance object
    if (instanceId != 0||targetP==NULL)//
    {
        return COAP_404_NOT_FOUND;
    }


    // is the server asking for the full object ?
    if (*numDataP == 0)
    {
        uint16_t resList[] = {
            RES_MONITER_NETWORK_BEARER,
            RES_MONITER_LINK_UTILIZATION,
            RES_MONITER_CSQ,
            RES_MONITER_ECL,
            RES_MONITER_SNR,
            RES_MONITER_PCI,
            RES_MONITER_ECGI,
            RES_MONITER_EARFCN,
            RES_MONITER_PCI_FIRST,
            RES_MONITER_PCI_SECOND,
            RES_MONITER_RSRP_FIRST,
            RES_MONITER_RSRP_SECOND,
			RES_MONITER_PLMN
        };
        int nbRes = sizeof(resList)/sizeof(uint16_t);
		(*dataArrayP)->id = 0;
		(*dataArrayP)->type = DATA_TYPE_OBJECT_INSTANCE;
		(*dataArrayP)->value.asChildren.count = nbRes;
		(*dataArrayP)->value.asChildren.array =  data_new(nbRes);
		memset((*dataArrayP)->value.asChildren.array,0,(nbRes)*sizeof(cis_data_t));
		if (*dataArrayP == NULL) return COAP_500_INTERNAL_SERVER_ERROR;
		*numDataP = nbRes;

        if (*dataArrayP == NULL) return COAP_500_INTERNAL_SERVER_ERROR;
        *numDataP = nbRes;
        for (i = 0 ; i < nbRes ; i++)
        {
			(*dataArrayP)->value.asChildren.array[i].id = resList[i];
        }

		if (prv_moniter_get_value(contextP,(*dataArrayP),nbRes,targetP)!=COAP_205_CONTENT)
		{
			return COAP_500_INTERNAL_SERVER_ERROR;
		}
    }
	else
	{
		result = prv_moniter_get_value(contextP,(*dataArrayP),1,targetP);
	}

    return result;
}

cis_coapret_t std_moniter_discover(st_context_t * contextP,
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
            RES_MONITER_NETWORK_BEARER,
            RES_MONITER_LINK_UTILIZATION,
            RES_MONITER_CSQ,
            RES_MONITER_ECL,
            RES_MONITER_SNR,
            RES_MONITER_PCI,
            RES_MONITER_ECGI,
            RES_MONITER_EARFCN,
            RES_MONITER_PCI_FIRST,
            RES_MONITER_PCI_SECOND,
            RES_MONITER_RSRP_FIRST,
            RES_MONITER_RSRP_SECOND,
			RES_MONITER_PLMN
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
            (*dataArrayP)->value.asChildren.array[i].value.asObjLink.objectId = CIS_MONITOR_OBJECT_ID;
            (*dataArrayP)->value.asChildren.array[i].value.asObjLink.instId = 0;
        }
    }
    else
    {
        for (i = 0; i < *numDataP && result == COAP_205_CONTENT; i++)
        {
            switch ((*dataArrayP)[i].id)
            {                  
                case RES_MONITER_NETWORK_BEARER:
                case RES_MONITER_LINK_UTILIZATION:
                case RES_MONITER_CSQ:
                case RES_MONITER_ECL:
                case RES_MONITER_SNR:
                case RES_MONITER_PCI:
                case RES_MONITER_ECGI:
                case RES_MONITER_EARFCN:
                case RES_MONITER_PCI_FIRST:
                case RES_MONITER_PCI_SECOND:
                case RES_MONITER_RSRP_FIRST:
                case RES_MONITER_RSRP_SECOND:
				case RES_MONITER_PLMN:
                        break;
                default:
                    result = COAP_404_NOT_FOUND;
            }
        }
    }

    return result;
}

cis_coapret_t std_moniter_write(st_context_t * contextP,
							   uint16_t instanceId,
                               int numData,
                               st_data_t * dataArray,
                               st_object_t * objectP)
{
    return COAP_501_NOT_IMPLEMENTED;
}

uint8_t std_moniter_execute(st_context_t * contextP,
						   uint16_t instanceId,
                           uint16_t resourceId,
                           uint8_t * buffer,
						   int length,
                           st_object_t * objectP)
{
    return COAP_400_BAD_REQUEST;
}
#endif
