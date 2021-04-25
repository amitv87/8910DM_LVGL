#include "../cis_api.h"
#include "../cis_internals.h"
#include "std_object.h"
#if CIS_ENABLE_CMIOT_OTA
bool std_poweruplog_create(st_context_t * contextP,
					   int instanceId,
                       st_object_t * powerupLogObj)
{
    poweruplog_data_t * instPowerupLog=NULL;
    poweruplog_data_t * targetP = NULL;
    uint8_t instBytes = 0;
    uint8_t instCount = 0;
    cis_iid_t instIndex;
	
    if (NULL == powerupLogObj)
    {
        return false;   
    }

    // Manually create a hard-code instance
    targetP = (poweruplog_data_t *)cis_malloc(sizeof(poweruplog_data_t));
    if (NULL == targetP)
    {
        return false;
    }
    cis_memset(targetP, 0, sizeof(poweruplog_data_t));

	targetP->deviceName = (char*)cis_malloc(NBSYS_DEVICENAME_MAXLENGTH);
	if(targetP->deviceName == NULL)
	{
		cis_free(targetP);
		return false;
	}	
    cis_memset(targetP->deviceName, 0, NBSYS_DEVICENAME_MAXLENGTH);
	
	//uint8_t Eid =	cissys_getIMEI(targetP->deviceName,NBSYS_IMEI_MAXLENGTH);

	targetP->IMSI= (char*)cis_malloc(NBSYS_IMSI_MAXLENGTH);
	if(targetP->IMSI == NULL)
	{
		cis_free(targetP);
		return false;
	}	
    cis_memset(targetP->IMSI, 0, NBSYS_IMSI_MAXLENGTH);
	uint8_t Imsi =	cissys_getIMSI(targetP->IMSI,NBSYS_IMSI_MAXLENGTH);

	targetP->IMEI = (char*)cis_malloc(NBSYS_IMEI_MAXLENGTH);
	if(targetP->IMEI == NULL)
	{
		cis_free(targetP);
		return false;
	}	
    cis_memset(targetP->IMEI, 0, NBSYS_IMEI_MAXLENGTH);
	uint8_t Imei =	cissys_getIMEI(targetP->IMEI,NBSYS_IMEI_MAXLENGTH);


	targetP->MSISDN = (char*)cis_malloc(NBSYS_EID_MAXLENGTH);
	if(targetP->MSISDN == NULL)
	{
		cis_free(targetP);
		return false;
	}	
    cis_memset(targetP->MSISDN, 0, NBSYS_EID_MAXLENGTH);
	uint8_t Eid =	cissys_getEID(targetP->MSISDN,NBSYS_EID_MAXLENGTH);

	

	targetP->toolVersion= (char*)cis_malloc(NBSYS_TOOLVERSION_MAXLENGTH);
	if(targetP->toolVersion == NULL)
	{
		cis_free(targetP);
		return false;
	}	
    cis_memset(targetP->toolVersion, 0, NBSYS_TOOLVERSION_MAXLENGTH);
	uint8_t ToolVersion = cis_ota_get_version(targetP->toolVersion);

	if(Eid <= 0 || Imsi <=0 ||Imei <=0 ||ToolVersion <=0)
	{
		LOGE("ERROR:Get EID/IMEI/IMSI ERROR.\n");
		return false;
	}


	
    targetP->instanceId = (uint16_t)instanceId;


    instPowerupLog = (poweruplog_data_t * )std_object_put_poweruplog(contextP,(cis_list_t*)targetP);
    
    instCount = CIS_LIST_COUNT(instPowerupLog);
    if(instCount == 0)
    {
        cissys_free(targetP);
        return false;
    }
	
	LOGE("Power up log create finished!!!.\n");

    /*first security object instance
     *don't malloc instance bitmap ptr*/
    if(instCount == 1)
    {
        return true;
    }

    powerupLogObj->instBitmapCount = instCount;
    instBytes = (instCount - 1) / 8 + 1;
    if(powerupLogObj->instBitmapBytes < instBytes){
        if(powerupLogObj->instBitmapBytes != 0 && powerupLogObj->instBitmapPtr != NULL)
        {
            cissys_free(powerupLogObj->instBitmapPtr);
        }
        powerupLogObj->instBitmapPtr = (uint8_t*)cissys_malloc(instBytes);
        powerupLogObj->instBitmapBytes = instBytes;
    }
    cissys_memset(powerupLogObj->instBitmapPtr,0,instBytes);
    targetP = instPowerupLog;
    for (instIndex = 0;instIndex < instCount;instIndex++)
    {
        uint8_t instBytePos = targetP->instanceId / 8;
        uint8_t instByteOffset = 7 - (targetP->instanceId % 8);
        powerupLogObj->instBitmapPtr[instBytePos] += 0x01 << instByteOffset;

        targetP = targetP->next;
    }
    return true;
}

static uint8_t prv_poweruplog_get_value(st_data_t* dataArrayP,int number, poweruplog_data_t * powerupP)
{
	uint8_t result;
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

		switch (resId)				//need do error handle
		{
		case RES_M_DEVICENAME:
			{
				data_encode_string(powerupP->deviceName,dataP);
				result = COAP_205_CONTENT;
				break;
			}

		case RES_M_TOOLVERSION:
			{
				data_encode_string(powerupP->toolVersion,dataP);
				result = COAP_205_CONTENT;
				break;
			}

		case RES_M_IMEI:
			{
				data_encode_string(powerupP->IMEI,dataP);
				result = COAP_205_CONTENT;
				break;

			}

		case RES_M_IMSI:
			{
				data_encode_string(powerupP->IMSI,dataP);
				result = COAP_205_CONTENT;
				break;
			}

		case RES_M_MSISDN:
			{
				data_encode_string(powerupP->MSISDN,dataP);
				result = COAP_205_CONTENT;
				break;
			}

		default:
			return COAP_404_NOT_FOUND;
		}
	}   
	return result;
}

static poweruplog_data_t * prv_poweruplog_find(st_context_t * contextP,cis_iid_t instanceId)
{
	poweruplog_data_t * targetP;
	targetP = (poweruplog_data_t *)(std_object_get_poweruplog(contextP, instanceId));

	if (NULL != targetP)
	{
		return targetP;
	}

	return NULL;
}

uint8_t std_poweruplog_read(st_context_t * contextP,
						uint16_t instanceId,
                        int * numDataP,
                        st_data_t ** dataArrayP,
                        st_object_t * objectP)
{
    uint8_t result;
    int i;
	poweruplog_data_t * targetP;
	targetP = prv_poweruplog_find(contextP,instanceId);
    // this is a single instance object
    if (instanceId != 0 || targetP==NULL)
    {
        return COAP_404_NOT_FOUND;
    }

    // is the server asking for the full object ?
    if (*numDataP == 0)
    {

        uint16_t resList[] = {
			RES_M_DEVICENAME,
			RES_M_TOOLVERSION,
			RES_M_IMEI,
			RES_M_IMSI,
			RES_M_MSISDN,
        };

        int nbRes = sizeof(resList)/sizeof(uint16_t);
		(*dataArrayP)->id = 0;
		(*dataArrayP)->type = DATA_TYPE_OBJECT_INSTANCE;
		(*dataArrayP)->value.asChildren.count = nbRes;
		(*dataArrayP)->value.asChildren.array = data_new(nbRes);

		if (*dataArrayP == NULL) return COAP_500_INTERNAL_SERVER_ERROR;
		*numDataP = nbRes;	
		
		memset((*dataArrayP)->value.asChildren.array,0,(nbRes)*sizeof(cis_data_t));
		
        for (i = 0 ; i < nbRes ; i++)
        {
			(*dataArrayP)->value.asChildren.array[i].id = resList[i];
        }

		if (prv_poweruplog_get_value((*dataArrayP),nbRes,targetP)!=COAP_205_CONTENT)
		{
			return COAP_500_INTERNAL_SERVER_ERROR;
		}
    }
	else
	{
		result = prv_poweruplog_get_value((*dataArrayP),1,targetP);
	}

    return result;
}

uint8_t std_poweruplog_discover(st_context_t * contextP,
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
			RES_M_DEVICENAME,
			RES_M_TOOLVERSION,
			RES_M_IMEI,
			RES_M_IMSI,
			RES_M_MSISDN,
        };
        int nbRes = sizeof(resList) / sizeof(uint16_t);

		(*dataArrayP)->id = 0;
		(*dataArrayP)->type = DATA_TYPE_OBJECT_INSTANCE;
		(*dataArrayP)->value.asChildren.count = nbRes;
		(*dataArrayP)->value.asChildren.array = data_new(nbRes);
		memset((*dataArrayP)->value.asChildren.array,0,(nbRes)*sizeof(cis_data_t));
        if (*dataArrayP == NULL) return COAP_500_INTERNAL_SERVER_ERROR;
        *numDataP = nbRes;
        for (i = 0; i < nbRes; i++)
        {
             (*dataArrayP)->value.asChildren.array[i].id = resList[i];
			 (*dataArrayP)->value.asChildren.array[i].type = DATA_TYPE_LINK;
			 (*dataArrayP)->value.asChildren.array[i].value.asObjLink.objectId = CIS_POWERUPLOG_OBJECT_ID;
			 (*dataArrayP)->value.asChildren.array[i].value.asObjLink.instId = 0;
        }
    }
    else
    {
        for (i = 0; i < *numDataP && result == COAP_205_CONTENT; i++)
        {
            switch ((*dataArrayP)[i].id)
            {
			case RES_M_DEVICENAME:
			case RES_M_TOOLVERSION:
			case RES_M_IMEI:
			case RES_M_IMSI:
			case RES_M_MSISDN:
                break;
            default:
                result = COAP_404_NOT_FOUND;
            }
        }
    }

    return result;
}

cis_coapret_t std_poweruplog_write(st_context_t * contextP,
							   uint16_t instanceId,
                               int numData,
                               st_data_t * dataArray,
                               st_object_t * objectP)
{
    return COAP_400_BAD_REQUEST;
}

uint8_t std_poweruplog_execute(st_context_t * contextP,
						   uint16_t instanceId,
                           uint16_t resourceId,
                           uint8_t * buffer,
						   int length,
                           st_object_t * objectP)
{
    return COAP_400_BAD_REQUEST;
}

void std_poweruplog_clean(st_context_t * contextP)
{
	poweruplog_data_t * deleteInst;
	poweruplog_data_t * poweruplogInstance = NULL;
	poweruplogInstance = (poweruplog_data_t *)(contextP->poweruplog_inst);
	while (poweruplogInstance != NULL)
	{
		deleteInst = poweruplogInstance;
		poweruplogInstance = poweruplogInstance->next;
		std_object_remove_poweruplog(contextP,(cis_list_t*)(deleteInst));
		cis_free(deleteInst);
	}
}

uint8_t std_poweruplog_ota_notify(st_context_t * contextP,cis_oid_t * objectid,uint16_t mid)
{
	
	if(*objectid == CIS_POWERUPLOG_OBJECT_ID)
	{
		uint16_t resList[] = 
		{
			RES_M_IMEI,
			RES_M_IMSI,
			RES_M_MSISDN,
		};
		cis_data_t *tmpdata = (cis_data_t*)cis_malloc(sizeof(cis_data_t));
		if(tmpdata == NULL)
		{
			cis_free(tmpdata);
			return false;
		}					
		st_uri_t uri;
		uint8_t nbRes = sizeof(resList)/sizeof(uint16_t);
		uint8_t loop;
		uri.flag = 7;//uriP->flag;
		uri.objectId = CIS_POWERUPLOG_OBJECT_ID;
		uri.instanceId = 0;
		tmpdata->type = cis_data_type_string;
		for(loop = 0; loop < nbRes; loop++)
		{
			uri.resourceId = resList[loop];
			cis_uri_update(&uri);
			switch (resList[loop])				//need do error handle
			{
			case RES_M_IMEI:
				{		
					tmpdata->asBuffer.buffer= (uint8_t*)cis_malloc(NBSYS_IMEI_MAXLENGTH);
					if(tmpdata->asBuffer.buffer == NULL)
					{
						cis_free(tmpdata->asBuffer.buffer);
						return false;
					}	
					cis_memset(tmpdata->asBuffer.buffer,0, NBSYS_IMEI_MAXLENGTH);
					tmpdata->asBuffer.length = cissys_getIMEI((char *)tmpdata->asBuffer.buffer,NBSYS_IMEI_MAXLENGTH);
					cis_notify(contextP,&uri,tmpdata,(cis_mid_t)mid,CIS_NOTIFY_CONTINUE,false);
					cis_free(tmpdata->asBuffer.buffer);
					break;
				}
	
			case RES_M_IMSI:
				{
					tmpdata->asBuffer.buffer= (uint8_t*)cis_malloc(NBSYS_IMSI_MAXLENGTH);
					if(tmpdata->asBuffer.buffer == NULL)
					{
						cis_free(tmpdata->asBuffer.buffer);
						return false;
					}	
					cis_memset(tmpdata->asBuffer.buffer,0, NBSYS_IMSI_MAXLENGTH);
					tmpdata->asBuffer.length = cissys_getIMSI((char *)tmpdata->asBuffer.buffer,NBSYS_IMSI_MAXLENGTH);
					cis_notify(contextP,&uri,tmpdata,(cis_mid_t)mid,CIS_NOTIFY_CONTINUE,false);
					cis_free(tmpdata->asBuffer.buffer);
					break;
				}
	
			case RES_M_MSISDN:
				{
					tmpdata->asBuffer.buffer= (uint8_t*)cis_malloc(NBSYS_EID_MAXLENGTH);
					if(tmpdata->asBuffer.buffer == NULL)
					{
						cis_free(tmpdata->asBuffer.buffer);
						return false;
					}	
					cis_memset(tmpdata->asBuffer.buffer,0, NBSYS_EID_MAXLENGTH);
					tmpdata->asBuffer.length = cissys_getEID((char *)tmpdata->asBuffer.buffer,NBSYS_EID_MAXLENGTH);
					cis_notify(contextP,&uri,tmpdata,(cis_mid_t)mid,CIS_NOTIFY_CONTENT,false);
					cis_free(tmpdata->asBuffer.buffer);
					break;

				}
			default:
			break;
			}
		}
		cis_free(tmpdata);
	}
}

#endif
