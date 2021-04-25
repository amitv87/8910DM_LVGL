#include "../cis_api.h"
#include "../cis_internals.h"
#include "std_object.h"


typedef struct st_security_instance
{
	struct st_security_instance *	next;        // matches lwm2m_list_t::next
	cis_listid_t					instanceId;  // matches lwm2m_list_t::id
	char *							host;        // ip address;
#if CIS_ENABLE_DTLS
	char *							identify;
	char *							secret;
	char *							public_key;
#endif
	bool                            isBootstrap;
	uint16_t						shortID;
	uint32_t						clientHoldOffTime;
	uint8_t							securityMode;
#if CIS_ENABLE_DTLS
#if CIS_ENABLE_PSK
	uint8_t							id_len;
	uint8_t							psk_len;
#endif
#endif
} security_instance_t;


static uint8_t prv_get_value(st_context_t * contextP,
	                         st_data_t * dataP,
                             security_instance_t * targetP)
{
    switch (dataP->id)
    {
    case RES_SECURITY_URI_ID:
        data_encode_string(targetP->host, dataP);
        return COAP_205_CONTENT;

    case RES_SECURITY_BOOTSTRAP_ID:
        data_encode_bool(targetP->isBootstrap, dataP);
        return COAP_205_CONTENT;

    case RES_SECURITY_SECURITY_ID:
        data_encode_int(targetP->securityMode, dataP);
        return COAP_205_CONTENT;
    case RES_SECURITY_SHORT_SERVER_ID:
        data_encode_int(targetP->shortID, dataP);
        return COAP_205_CONTENT;

    case RES_SECURITY_HOLD_OFF_ID:
        data_encode_int(targetP->clientHoldOffTime, dataP);
        return COAP_205_CONTENT;
    default:
        return COAP_404_NOT_FOUND;
    }
}


static security_instance_t * prv_security_find(st_context_t * contextP,cis_iid_t instanceId)
{
    security_instance_t * targetP;
    targetP = (security_instance_t *)cis_list_find(std_object_get_securitys(contextP), instanceId);
    if (NULL != targetP)
    {
        return targetP;
    }

    return NULL;
}



cis_coapret_t std_security_read(st_context_t * contextP,cis_iid_t instanceId,
                                 int * numDataP,
                                 st_data_t ** dataArrayP,
                                 st_object_t * objectP)
{
    security_instance_t * targetP;
    uint8_t result;
    int i;

    targetP = prv_security_find(contextP,instanceId);
    if (NULL == targetP) return COAP_404_NOT_FOUND;

    // is the server asking for the full instance ?
    if (*numDataP == 0)
    {
		uint16_t resList[] = { RES_SECURITY_URI_ID,
							   RES_SECURITY_BOOTSTRAP_ID,
							   RES_SECURITY_SHORT_SERVER_ID,
							   RES_SECURITY_HOLD_OFF_ID };
        int nbRes = sizeof(resList)/sizeof(uint16_t);

        *dataArrayP = data_new(nbRes);
        if (*dataArrayP == NULL) return COAP_500_INTERNAL_SERVER_ERROR;
        *numDataP = nbRes;
        for (i = 0 ; i < nbRes ; i++)
        {
            (*dataArrayP)[i].id = resList[i];
        }
    }

    i = 0;
    do
    {
        result = prv_get_value(contextP,(*dataArrayP) + i, targetP);
        i++;
    } while (i < *numDataP && result == COAP_205_CONTENT);

    return result;
}

cis_coapret_t std_security_write(st_context_t * contextP,
	cis_iid_t instanceId,
	int numData,
	st_data_t * dataArray,
	st_object_t * objectP)
{
	security_instance_t * targetP;
	int i;
	uint8_t result = COAP_204_CHANGED;

	targetP = prv_security_find(contextP, instanceId);
	if (NULL == targetP)
	{
		return COAP_404_NOT_FOUND;
	}

	i = 0;
	do {
		switch (dataArray[i].id)
		{
		case RES_SECURITY_URI_ID:
			if (targetP->host != NULL) cis_free(targetP->host);
			targetP->host = (char *)cis_malloc(dataArray[i].asBuffer.length + 1);
			if (targetP->host != NULL)
			{
				cis_memset(targetP->host, 0, dataArray[i].asBuffer.length + 1);
#if CIS_ENABLE_DTLS 
			   if(TRUE== contextP->isDTLS)
				 utils_stringCopy(targetP->host, dataArray[i].asBuffer.length - 8 - URI_TAILER_HOST_LEN, (char*)dataArray[i].asBuffer.buffer + 8);
			   else
				 utils_stringCopy(targetP->host, dataArray[i].asBuffer.length - 7 - URI_TAILER_HOST_LEN, (char*)dataArray[i].asBuffer.buffer + 7);
#else
				utils_stringCopy(targetP->host, dataArray[i].asBuffer.length - URI_HEADER_HOST_LEN - URI_TAILER_HOST_LEN, (char*)dataArray[i].asBuffer.buffer + URI_HEADER_HOST_LEN);
#endif
				result = COAP_204_CHANGED;
			}
			else
			{
				result = COAP_500_INTERNAL_SERVER_ERROR;
			}
			break;

		case RES_SECURITY_SECURITY_ID:
		{
			int64_t value;

			if (1 == data_decode_int(dataArray + i, &value))
			{
				if (value >= 0 && value <= 3)
				{
					targetP->securityMode = (uint8_t)value;
					result = COAP_204_CHANGED;
				}
				else
				{
					result = COAP_406_NOT_ACCEPTABLE;
				}
			}
			else
			{
				result = COAP_400_BAD_REQUEST;
			}
		}
		break;
		case RES_SECURITY_BOOTSTRAP_ID:
			if (1 == data_decode_bool(dataArray + i, &(targetP->isBootstrap)))
			{
				result = COAP_204_CHANGED;
			}
			else
			{
				result = COAP_400_BAD_REQUEST;
			}
			break;

		case RES_SECURITY_SHORT_SERVER_ID:
		{
			int64_t value;

			if (1 == data_decode_int(dataArray + i, &value))
			{
				if (value >= 0 && value <= 0xFFFF)
				{
					targetP->shortID = (uint16_t)value;
					result = COAP_204_CHANGED;
				}
				else
				{
					result = COAP_406_NOT_ACCEPTABLE;
				}
			}
			else
			{
				result = COAP_400_BAD_REQUEST;
			}
		}
		break;

		case RES_SECURITY_HOLD_OFF_ID:
		{
			int64_t value;

			if (1 == data_decode_int(dataArray + i, &value))
			{
				if (value >= 0 && value <= 0xFFFF)
				{
					targetP->clientHoldOffTime = (uint32_t)value;
					result = COAP_204_CHANGED;
				}
				else
				{
					result = COAP_406_NOT_ACCEPTABLE;
				}
			}
			else
			{
				result = COAP_400_BAD_REQUEST;
			}
			break;
		}
#if CIS_ENABLE_DTLS
		case RES_SECURITY_IDENTIFY:
			if (targetP->identify != NULL) cis_free(targetP->identify);
			targetP->identify = (char *)cis_malloc(dataArray[i].asBuffer.length + 1);
			if (targetP->identify != NULL)
			{
				cis_memset(targetP->identify, 0, dataArray[i].asBuffer.length + 1);
				cis_memcpy(targetP->identify, (char*)dataArray[i].asBuffer.buffer, dataArray[i].asBuffer.length);
				targetP->id_len = dataArray[i].asBuffer.length;
				result = COAP_204_CHANGED;
			}
			else
			{
				result = COAP_500_INTERNAL_SERVER_ERROR;
			}
			break;
		case RES_SECURITY_SECRET:
			if (targetP->secret != NULL) cis_free(targetP->secret);
			targetP->secret = (char *)cis_malloc(dataArray[i].asBuffer.length + 1);
			if (targetP->secret != NULL)
			{
				cis_memset(targetP->secret, 0, dataArray[i].asBuffer.length + 1);
				cis_memmove(targetP->secret, (char*)dataArray[i].asBuffer.buffer, dataArray[i].asBuffer.length);
				targetP->psk_len = dataArray[i].asBuffer.length;
				result = COAP_204_CHANGED;
			}
			else
			{
				result = COAP_500_INTERNAL_SERVER_ERROR;
			}
			break;
		case RES_SECURITY_PUBLIC_KEY:
			if (targetP->public_key != NULL) cis_free(targetP->public_key);
			targetP->public_key = (char *)cis_malloc(dataArray[i].asBuffer.length + 1);
			if (targetP->public_key != NULL)
			{
				cis_memset(targetP->public_key, 0, dataArray[i].asBuffer.length + 1);
				cis_memcpy(targetP->public_key, (char*)dataArray[i].asBuffer.buffer, dataArray[i].asBuffer.length);
				result = COAP_204_CHANGED;
			}
			else
			{
				result = COAP_500_INTERNAL_SERVER_ERROR;
			}
			break;
#endif
		default:
			return COAP_404_NOT_FOUND;
		}
		i++;
	} while (i < numData && result == COAP_204_CHANGED);

	return result;
}


cis_coapret_t std_security_discover(st_context_t * contextP,
								    uint16_t instanceId,
								    int * numDataP,
							        st_data_t ** dataArrayP,
								    st_object_t * objectP)
{
	return COAP_404_NOT_FOUND;
}

bool std_security_create(st_context_t * contextP,
	int instanceId,
	const char* serverHost,
	bool isBootstrap,
	st_object_t * securityObj)
{
    int res;
	security_instance_t * instSecurity = NULL;
	security_instance_t * targetP = NULL;
	cis_instcount_t instBytes = 0;
	cis_instcount_t instCount = 0;
	cis_iid_t instIndex;
	if (NULL == securityObj)
	{
		return false;
	}

	// Manually create a hard-code instance
	targetP = (security_instance_t *)cis_malloc(sizeof(security_instance_t));
	if (NULL == targetP)
	{
		return false;
	}

	cis_memset(targetP, 0, sizeof(security_instance_t));
	targetP->instanceId = (uint16_t)instanceId;
	targetP->host = (char*)cis_malloc(utils_strlen(serverHost) + 1);
	if (targetP->host == NULL)
	{
		cis_free(targetP);
		return false;
	}
    cis_memset(targetP->host, 0, utils_strlen(serverHost) + 1);
    res = utils_stringCopy(targetP->host, utils_strlen(serverHost) + 1, serverHost);
    if(res < 0)
    {
        cis_free(targetP->host);
        targetP->host = NULL;
        cis_free(targetP);
        targetP = NULL;
        return false;
    }
#if CIS_ENABLE_DTLS
#if CIS_ENABLE_DM
	if (TRUE == isBootstrap && TRUE != contextP->isDM && TRUE == contextP->isDTLS)
#else
	if (TRUE == isBootstrap && TRUE == contextP->isDTLS)
#endif
	{
#if CIS_ENABLE_PSK
		//read psk file and get the encrypt
		targetP->identify = (char*)cis_malloc(NBSYS_IMEI_MAXLENGTH + 2);
		if (targetP->identify == NULL)
		{
			cis_free(targetP->host);
			cis_free(targetP);
			return false;
		}
		cis_memset(targetP->identify, 0, NBSYS_IMEI_MAXLENGTH + 2);
		cissys_getIMEI(targetP->identify, NBSYS_IMEI_MAXLENGTH);
		targetP->id_len = strlen(targetP->identify);
        
        if(NULL==contextP->PSK)
		{
			cis_free(targetP->host);
			cis_free(targetP->identify);
			cis_free(targetP);
			return false;
		}
	
        targetP->psk_len = strlen(contextP->PSK);		
		targetP->secret = (char *)cis_malloc(targetP->psk_len+1);
		if (targetP->secret == NULL)
		{
			cis_free(targetP->host);
			cis_free(targetP->identify);
			cis_free(targetP);
			return false;
		}
		cis_memcpy(targetP->secret,contextP->PSK,targetP->psk_len+1);
		//targetP->psk_len = cissys_getPSK(targetP->secret, NBSYS_PSK_MAXLENGTH);
#endif
	}
#endif

	targetP->isBootstrap = isBootstrap;
	targetP->shortID = 0;
	targetP->clientHoldOffTime = 10;

	instSecurity = (security_instance_t *)std_object_put_securitys(contextP, (cis_list_t*)targetP);

	instCount = (cis_instcount_t)CIS_LIST_COUNT(instSecurity);
	if (instCount == 0)
	{
#if CIS_ENABLE_DTLS
		if (targetP->identify != NULL)
			cis_free(targetP->identify);
		if (targetP->secret != NULL)
			cis_free(targetP->secret);
#endif
		cis_free(targetP->host);
		cis_free(targetP);
		return false;
	}

	/*first security object instance
	 *don't malloc instance bitmap ptr*/
	if (instCount == 1)
	{
		return true;
	}

	securityObj->instBitmapCount = instCount;
	instBytes = (instCount - 1) / 8 + 1;
	if (securityObj->instBitmapBytes < instBytes) {
		if (securityObj->instBitmapBytes != 0 && securityObj->instBitmapPtr != NULL)
		{
			cis_free(securityObj->instBitmapPtr);
		}
		securityObj->instBitmapPtr = (uint8_t*)cis_malloc(instBytes);
		securityObj->instBitmapBytes = instBytes;
	}
	cis_memset(securityObj->instBitmapPtr, 0, instBytes);

	for (instIndex = 0; instIndex < instCount; instIndex++)
	{
		uint16_t instBytePos = (uint16_t)instSecurity->instanceId / 8;
		uint16_t instByteOffset = 7 - (instSecurity->instanceId % 8);
		securityObj->instBitmapPtr[instBytePos] += 0x01 << instByteOffset;
		instSecurity = instSecurity->next;
	}
	return true;
}



void std_security_clean(st_context_t * contextP)
{
	security_instance_t * deleteInst;
	security_instance_t * securityInstance = NULL;
	securityInstance = (security_instance_t *)(contextP->instSecurity);

	while (securityInstance != NULL)
	{
		deleteInst = securityInstance;
		securityInstance = securityInstance->next;

		std_object_remove_securitys(contextP, (cis_list_t*)deleteInst);
		if (NULL != deleteInst->host)
		{
			cis_free(deleteInst->host);
		}
#if CIS_ENABLE_DTLS
		if (NULL != deleteInst->identify)
		{
			cis_free(deleteInst->identify);
		}
		if (NULL != deleteInst->secret)
		{
			cis_free(deleteInst->secret);
		}
		if (NULL != deleteInst->public_key)
		{
			cis_free(deleteInst->public_key);
		}
#endif
		cis_free(deleteInst);
	}
}



char * std_security_get_host(st_context_t * contextP,cis_iid_t InstanceId)
{
    security_instance_t * targetP = prv_security_find(contextP,InstanceId);

    if (NULL != targetP)
    {
        return utils_strdup(targetP->host);
    }

    return NULL;
}

#if CIS_ENABLE_DTLS
#include "dtls/dtls.h"
#include "dtls/debug.h"
#if CIS_ENABLE_PSK
int get_psk_info(dtls_context_t *ctx,
	const session_t *session,
	dtls_credentials_type_t type,
	const unsigned char *id, size_t id_len,
	unsigned char *result, size_t result_length) {

	security_instance_t * targetP = NULL;
	st_context_t * contextP = (st_context_t *)(ctx->app);
	if (contextP->stateStep == PUMP_STATE_BOOTSTRAPPING)
	{
		targetP = prv_security_find(contextP, 1);
		if (NULL == targetP)
		{
			dtls_warn("cannot find psk\n");
			return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
		}

	}
	else
	{
		targetP = prv_security_find(contextP, 0);
		if (NULL == targetP)
		{
			dtls_warn("cannot find psk\n");
			return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
		}
	}
	switch (type) {
	case DTLS_PSK_IDENTITY:
		if (id_len) {
			dtls_debug("got psk_identity_hint: '%.*s'\n", id_len, id);
		}

		if (result_length < targetP->id_len) {
			dtls_warn("cannot set psk_identity -- buffer too small\n");
			return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
		}

		cis_memcpy(result, targetP->identify, targetP->id_len);
		return targetP->id_len;
	case DTLS_PSK_KEY:
		if (id_len != targetP->id_len || cis_memcmp(targetP->identify, id, id_len) != 0) {
			dtls_warn("PSK for unknown id requested, exiting\n");
			return dtls_alert_fatal_create(DTLS_ALERT_ILLEGAL_PARAMETER);
		}
		else if (result_length < targetP->psk_len) {
			dtls_warn("cannot set psk -- buffer too small\n");
			return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
		}

		cis_memcpy(result, targetP->secret, targetP->psk_len);
		return targetP->psk_len;
	default:
		dtls_warn("unsupported request type: %d\n", type);
	}

	return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
}
#endif
#endif /* DTLS_PSK */
