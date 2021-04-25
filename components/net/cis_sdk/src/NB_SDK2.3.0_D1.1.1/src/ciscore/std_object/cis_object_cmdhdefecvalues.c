#include "../cis_api.h"
#include "../cis_internals.h"
#include "std_object.h"


#if CIS_ENABLE_CMIOT_OTA

#define		PRV_CMDHDEFECVALUES_DEFECVALUE_MAX_LEN         					4	
#define		PRV_CMDHDEFECVALUES_REQUESTORIGIN_MAX_LEN	   					32
#define		PRV_CMDHDEFECVALUES_REQUESTCONTEXT_MAX_LEN	   					32
#define		PRV_CMDHDEFECVALUES_REQUESTCHARACTERISTICS_MAX_LEN				4		

#define 	PRV_CMDHDEFECVALUES_REQUESTORIGIN_OTAREQ_LEN			7
#define 	PRV_CMDHDEFECVALUES_REQUESTORIGIN_OTAREADY_LEN			9
#define 	PRV_CMDHDEFECVALUES_REQUESTORIGIN_OTAFINISH_LEN			10
#define 	PRV_CMDHDEFECVALUES_REQUESTORIGIN_OTASTART_LEN			9


#define 	PRV_CMDHDEFECVALUES_REQUESTORIGIN_OTAREQ_STR_LOWERCASE			"otareq:"
#define 	PRV_CMDHDEFECVALUES_REQUESTORIGIN_OTAREQ_STR_CAPITALS			"OTAREQ:"
#define 	PRV_CMDHDEFECVALUES_REQUESTORIGIN_OTAFINISH_STR_LOWERCASE		"otafinish:"
#define 	PRV_CMDHDEFECVALUES_REQUESTORIGIN_OTAFINISH_STR_CAPITALS		"OTAFINISH:"

#define 	PRV_CMDHDEFECVALUES_REQUESTORIGIN_OTASTART_STR_LOWERCASE		"otastart:"
#define 	PRV_CMDHDEFECVALUES_REQUESTORIGIN_OTASTART_STR_CAPITALS		"OTASTART:"


#define 	PRV_CMDHDEFECVALUES_REQUESTORIGIN_OTAREADY_STR					"OTAREADY:"
extern uint16_t cmiot_ota_observe_mid;

typedef struct _cmdhdefecvalues_data_
{
	struct _cmdhdefecvalues_data_ * next;       
	cis_listid_t                 instanceId;  
	bool supported;
	uint32_t				order ;   
	char *				DefEcValue;
	char *				RequestOrigin;	
	char *				RequestContext;	
	bool				RequestContextNotification;	
	char *				RequestCharacteristics;
	
} cmdhdefecvalues_data_t;


bool std_cmdhdefecvalues_create(st_context_t * contextP,
						 int instanceId,
                         st_object_t * cmdhdefecvaluesObj)
{
    cmdhdefecvalues_data_t * instCmdhdefecvalues=NULL;
    cmdhdefecvalues_data_t * targetP = NULL;
    uint8_t instBytes = 0;
    uint8_t instCount = 0;
    cis_iid_t instIndex;
    if (NULL == cmdhdefecvaluesObj)
    {
        return false;   
    }

    // Manually create a hard-code instance
    targetP = (cmdhdefecvalues_data_t *)cis_malloc(sizeof(cmdhdefecvalues_data_t));
    if (NULL == targetP)
    {
        return false;
    }
    cis_memset(targetP, 0, sizeof(cmdhdefecvalues_data_t));

	
	targetP->supported= TRUE;
	targetP->order= 0;	
	
	targetP->DefEcValue = (char*)cis_malloc(PRV_CMDHDEFECVALUES_DEFECVALUE_MAX_LEN);
	if(targetP->DefEcValue == NULL)
	{
		cis_free(targetP);
		return false;
	}	
    cis_memset(targetP->DefEcValue, 0, PRV_CMDHDEFECVALUES_DEFECVALUE_MAX_LEN);

	targetP->RequestOrigin= (char*)cis_malloc(PRV_CMDHDEFECVALUES_REQUESTORIGIN_MAX_LEN);
	if(targetP->RequestOrigin == NULL)
	{
		cis_free(targetP);
		return false;
	}	
    cis_memset(targetP->RequestOrigin, 0, PRV_CMDHDEFECVALUES_REQUESTORIGIN_MAX_LEN);

	targetP->RequestContext= (char*)cis_malloc(PRV_CMDHDEFECVALUES_REQUESTCONTEXT_MAX_LEN);
	if(targetP->RequestContext == NULL)
	{
		cis_free(targetP);
		return false;
	}	
    cis_memset(targetP->RequestContext, 0, PRV_CMDHDEFECVALUES_REQUESTCONTEXT_MAX_LEN);

	targetP->RequestContextNotification = FALSE ;
 
	targetP->RequestCharacteristics= (char*)cis_malloc(PRV_CMDHDEFECVALUES_REQUESTCHARACTERISTICS_MAX_LEN);
	if(targetP->RequestCharacteristics == NULL)
	{
		cis_free(targetP);
		return false;
	}	
    cis_memset(targetP->RequestCharacteristics, 0, PRV_CMDHDEFECVALUES_REQUESTCHARACTERISTICS_MAX_LEN);
	
	
    targetP->instanceId = (uint16_t)instanceId;


    instCmdhdefecvalues = (cmdhdefecvalues_data_t * )std_object_put_cmdhdefecvalues(contextP,(cis_list_t*)targetP);
    
    instCount = CIS_LIST_COUNT(instCmdhdefecvalues);
    if(instCount == 0)
    {
        cissys_free(targetP);
        return false;
    }

	LOGE("cmdhdefecvalues object create finished!!!.\n");

    /*first security object instance
     *don't malloc instance bitmap ptr*/
    if(instCount == 1)
    {
        return true;
    }

    cmdhdefecvaluesObj->instBitmapCount = instCount;
    instBytes = (instCount - 1) / 8 + 1;
    if(cmdhdefecvaluesObj->instBitmapBytes < instBytes){
        if(cmdhdefecvaluesObj->instBitmapBytes != 0 && cmdhdefecvaluesObj->instBitmapPtr != NULL)
        {
            cissys_free(cmdhdefecvaluesObj->instBitmapPtr);
        }
        cmdhdefecvaluesObj->instBitmapPtr = (uint8_t*)cissys_malloc(instBytes);
        cmdhdefecvaluesObj->instBitmapBytes = instBytes;
    }
    cissys_memset(cmdhdefecvaluesObj->instBitmapPtr,0,instBytes);

    for (instIndex = 0;instIndex < instCount;instIndex++)
    {
        uint8_t instBytePos = targetP->instanceId / 8;
        uint8_t instByteOffset = 7 - (targetP->instanceId % 8);
        cmdhdefecvaluesObj->instBitmapPtr[instBytePos] += 0x01 << instByteOffset;

        targetP = targetP->next;
    }
    return true;
}

static uint8_t prv_cmdhdefecvalues_get_value(st_data_t* dataArrayP,int number, cmdhdefecvalues_data_t * cmdhdefecvaluesP)
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
		switch (resId)			
		{
		case RES_M_ORDER:
			{
				data_encode_int(cmdhdefecvaluesP->order,dataP);
				result = COAP_205_CONTENT;
				break;
			}

		case RES_M_DEFECVALUE:
			{
				data_encode_string(cmdhdefecvaluesP->DefEcValue,dataP);
				result = COAP_205_CONTENT;
				break;
			}

		case RES_M_REQUESTORIGIN:
			{
				data_encode_string(cmdhdefecvaluesP->RequestOrigin,dataP);
				result = COAP_205_CONTENT;
				break;

			}

		case RES_M_REQUESTCONTEXT:
			{
				data_encode_string(cmdhdefecvaluesP->RequestContext,dataP);
				result = COAP_205_CONTENT;
				break;
			}

		case RES_M_REQUESTCONTEXTNOTIFICATION:
			{
				data_encode_bool(cmdhdefecvaluesP->RequestContextNotification,dataP);
				result = COAP_205_CONTENT;
				break;
			}

		case RES_M_REQUESTCHARACTERISTICS:
			{
				data_encode_string(cmdhdefecvaluesP->RequestCharacteristics,dataP);
				result = COAP_205_CONTENT;
				break;
			}		

		default:
			return COAP_404_NOT_FOUND;
		}
	}   
	return result;
}

static cmdhdefecvalues_data_t * prv_cmdhdefecvalues_find(st_context_t * contextP,cis_iid_t instanceId)
{
	cmdhdefecvalues_data_t *targetP;
	targetP = (cmdhdefecvalues_data_t *)(std_object_get_cmdhdefecvalues(contextP, instanceId));

	if (NULL != targetP)
	{
		return targetP;
	}

	return NULL;
}

static uint8_t prv_mdhdefecvalues_otaready_notify(st_context_t * contextP,uint16_t mid,cmdhdefecvalues_data_t *dataP,int len)
{
	uint16_t resList[] = 
	{
		RES_M_REQUESTCONTEXT,
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
	uri.objectId = CIS_CMDHDEFECVALUES_OBJECT_ID;
	uri.instanceId = 0;
	tmpdata->type = cis_data_type_string;
	for(loop = 0; loop < nbRes; loop++)
	{
		uri.resourceId = resList[loop];
		cis_uri_update(&uri);
		switch (resList[loop])				//need do error handle
		{
		case RES_M_REQUESTCONTEXT:
			{		
				tmpdata->asBuffer.buffer= (uint8_t*)cis_malloc(len);
				if(tmpdata->asBuffer.buffer == NULL)
				{
					cis_free(tmpdata->asBuffer.buffer);
					cis_free(tmpdata);
					return false;
				}	
				cis_memset(tmpdata->asBuffer.buffer,0, len);
				tmpdata->asBuffer.length = len;
				cis_memcpy(tmpdata->asBuffer.buffer,dataP->RequestContext,len);
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

uint8_t std_cmdhdefecvalues_read(st_context_t * contextP,
						uint16_t instanceId,
                        int * numDataP,
                        st_data_t ** dataArrayP,
                        st_object_t * objectP)
{
    uint8_t result;
    int i;
	cmdhdefecvalues_data_t * targetP;
	targetP = prv_cmdhdefecvalues_find(contextP,instanceId);
    // this is a single instance object
    if (instanceId != 0 || targetP==NULL)
    {
        return COAP_404_NOT_FOUND;
    }

    // is the server asking for the full object ?
    if (*numDataP == 0)
    {

        uint16_t resList[] = {
			RES_M_ORDER,
			RES_M_DEFECVALUE,
			RES_M_REQUESTORIGIN,
			RES_M_REQUESTCONTEXT,
			RES_M_REQUESTCONTEXTNOTIFICATION,
			RES_M_REQUESTCHARACTERISTICS,
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

		if (prv_cmdhdefecvalues_get_value((*dataArrayP),nbRes,targetP)!=COAP_205_CONTENT)
		{
			return COAP_500_INTERNAL_SERVER_ERROR;
		}
    }
	else
	{
		result = prv_cmdhdefecvalues_get_value((*dataArrayP),1,targetP);
	}

    return result;
}

uint8_t std_cmdhdefecvalues_discover(st_context_t * contextP,
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
			RES_M_ORDER,
			RES_M_DEFECVALUE,
			RES_M_REQUESTORIGIN,
			RES_M_REQUESTCONTEXT,
			RES_M_REQUESTCONTEXTNOTIFICATION,
			RES_M_REQUESTCHARACTERISTICS,

        };
        int nbRes = sizeof(resList) / sizeof(uint16_t);

		(*dataArrayP)->id = 0;
		(*dataArrayP)->type = DATA_TYPE_OBJECT_INSTANCE;
		(*dataArrayP)->value.asChildren.count = nbRes;
		(*dataArrayP)->value.asChildren.array = data_new(nbRes);

        if (*dataArrayP == NULL) return COAP_500_INTERNAL_SERVER_ERROR;
        *numDataP = nbRes;	
		
		memset((*dataArrayP)->value.asChildren.array,0,(nbRes)*sizeof(cis_data_t));

        for (i = 0; i < nbRes; i++)
        {
             (*dataArrayP)->value.asChildren.array[i].id = resList[i];
			 (*dataArrayP)->value.asChildren.array[i].type = DATA_TYPE_LINK;
			 (*dataArrayP)->value.asChildren.array[i].value.asObjLink.objectId = CIS_CMDHDEFECVALUES_OBJECT_ID;
			 (*dataArrayP)->value.asChildren.array[i].value.asObjLink.instId = 0;
        }
    }
    else
    {
        for (i = 0; i < *numDataP && result == COAP_205_CONTENT; i++)
        {
            switch ((*dataArrayP)[i].id)
            {
			case RES_M_ORDER:
			case RES_M_DEFECVALUE:
			case RES_M_REQUESTORIGIN:
			case RES_M_REQUESTCONTEXT:
			case RES_M_REQUESTCONTEXTNOTIFICATION:
			case RES_M_REQUESTCHARACTERISTICS:				
                break;
            default:
                result = COAP_404_NOT_FOUND;
            }
        }
    }

    return result;
}

//extern cis_coapret_t std_cmdhdefecvalues_write(st_context_t * contextP,cis_iid_t instanceId,int numDataP,st_data_t * dataArrayP,st_object_t * objectP);

cis_coapret_t std_cmdhdefecvalues_write(st_context_t * contextP,
						   cis_iid_t instanceId,
                           int numData,
                           st_data_t * dataArray,
                           st_object_t * objectP)
{
    int i;
    uint8_t result=COAP_405_METHOD_NOT_ALLOWED;
	cmdhdefecvalues_data_t * targetP;
	targetP = prv_cmdhdefecvalues_find(contextP,instanceId);
    // this is a single instance object
    if (instanceId != 0||targetP==NULL)
    {
        return COAP_404_NOT_FOUND;
    }
	
    i = 0;
    do
    {
		st_data_t * data = &dataArray->value.asChildren.array[i];
        switch (data->id)
        {
        case RES_M_REQUESTORIGIN:
			{
				
				if((data->asBuffer.length > PRV_CMDHDEFECVALUES_REQUESTORIGIN_OTAREQ_LEN)&&(data->asBuffer.length < PRV_CMDHDEFECVALUES_REQUESTORIGIN_MAX_LEN))
				{
					LOGD("write cmdhdefecvalues object: %d, resid: %d,bufferlen: %d \r\n",objectP->objID,data->id,data->asBuffer.length);		
					
					LOGD("Received Command Detail : %s \r\n", data->asBuffer.buffer);

	    			cis_memset(targetP->RequestOrigin, 0, PRV_CMDHDEFECVALUES_REQUESTORIGIN_MAX_LEN);
					cis_memcpy(targetP->RequestOrigin,data->asBuffer.buffer,data->asBuffer.length);
					targetP->RequestOrigin[data->asBuffer.length] = '\0';
					if (strncmp((const char *)targetP->RequestOrigin,PRV_CMDHDEFECVALUES_REQUESTORIGIN_OTAREQ_STR_LOWERCASE,PRV_CMDHDEFECVALUES_REQUESTORIGIN_OTAREQ_LEN) == 0||
						strncmp((const char *)targetP->RequestOrigin,PRV_CMDHDEFECVALUES_REQUESTORIGIN_OTAREQ_STR_CAPITALS,PRV_CMDHDEFECVALUES_REQUESTORIGIN_OTAREQ_LEN) == 0)
					{	
						LOGD("CMIOT:OTAREQ received. Series Num : %s \r\n",data->asBuffer.buffer+PRV_CMDHDEFECVALUES_REQUESTORIGIN_OTAREQ_LEN);
						cis_memset(targetP->RequestContext, 0, PRV_CMDHDEFECVALUES_REQUESTCONTEXT_MAX_LEN);
						cis_memcpy(targetP->RequestContext,PRV_CMDHDEFECVALUES_REQUESTORIGIN_OTAREADY_STR,PRV_CMDHDEFECVALUES_REQUESTORIGIN_OTAREADY_LEN);					
						cis_memcpy(targetP->RequestContext+PRV_CMDHDEFECVALUES_REQUESTORIGIN_OTAREADY_LEN,data->asBuffer.buffer+PRV_CMDHDEFECVALUES_REQUESTORIGIN_OTAREQ_LEN,data->asBuffer.length-PRV_CMDHDEFECVALUES_REQUESTORIGIN_OTAREQ_LEN);							
						targetP->RequestContext[data->asBuffer.length+2]='\0';

						prv_mdhdefecvalues_otaready_notify(contextP,cmiot_ota_observe_mid,targetP,data->asBuffer.length+2);

						result = COAP_204_CHANGED;
					}
					else if(strncmp((const char *)targetP->RequestOrigin,PRV_CMDHDEFECVALUES_REQUESTORIGIN_OTAFINISH_STR_LOWERCASE,PRV_CMDHDEFECVALUES_REQUESTORIGIN_OTAFINISH_LEN) == 0||
						strncmp((const char *)targetP->RequestOrigin,PRV_CMDHDEFECVALUES_REQUESTORIGIN_OTAFINISH_STR_CAPITALS,PRV_CMDHDEFECVALUES_REQUESTORIGIN_OTAFINISH_LEN) == 0)
					{
						LOGD("CMIOT:OTAFINISH received. Result Num : %s \r\n",data->asBuffer.buffer+PRV_CMDHDEFECVALUES_REQUESTORIGIN_OTAFINISH_LEN);
						result = COAP_204_CHANGED;
						core_callbackEvent(contextP,CIS_EVENT_CMIOT_OTA_FINISH,(void *)(data->asBuffer.buffer+PRV_CMDHDEFECVALUES_REQUESTORIGIN_OTAFINISH_LEN));
					}
					else if(strncmp((const char *)targetP->RequestOrigin,PRV_CMDHDEFECVALUES_REQUESTORIGIN_OTASTART_STR_LOWERCASE,PRV_CMDHDEFECVALUES_REQUESTORIGIN_OTASTART_LEN) == 0||
						strncmp((const char *)targetP->RequestOrigin,PRV_CMDHDEFECVALUES_REQUESTORIGIN_OTASTART_STR_CAPITALS,PRV_CMDHDEFECVALUES_REQUESTORIGIN_OTASTART_LEN) == 0)
					{
						LOGD("CMIOT:OTASTART received. Series Num : %s \r\n",data->asBuffer.buffer+PRV_CMDHDEFECVALUES_REQUESTORIGIN_OTASTART_LEN);
						result = COAP_204_CHANGED;
						core_callbackEvent(contextP,CIS_EVENT_CMIOT_OTA_START,NULL);	
					}

					else
					{
						result = COAP_400_BAD_REQUEST;
					}

				}
				else
					result = COAP_400_BAD_REQUEST;
			}
			break;
        default:
			{
				result = COAP_400_BAD_REQUEST;
			}
		}

        i++;
    } while (i < dataArray->value.asChildren.count);

    return result;
}

uint8_t std_cmdhdefecvalues_execute(st_context_t * contextP,
						   uint16_t instanceId,
                           uint16_t resourceId,
                           uint8_t * buffer,
						   int length,
                           st_object_t * objectP)
{
    return COAP_400_BAD_REQUEST;
}

void std_cmdhdefecvalues_clean(st_context_t * contextP)
{
	cmdhdefecvalues_data_t * deleteInst;
	cmdhdefecvalues_data_t * cmdhdefecvaluesInstance = NULL;
	cmdhdefecvaluesInstance = (cmdhdefecvalues_data_t *)(contextP->cmdhdefecvalues_inst);
	while (cmdhdefecvaluesInstance != NULL)
	{
		deleteInst = cmdhdefecvaluesInstance;
		cmdhdefecvaluesInstance = cmdhdefecvaluesInstance->next;
		std_object_remove_cmdhdefecvalues(contextP,(cis_list_t*)(deleteInst));
		cis_free(deleteInst);
	}
}




#endif

