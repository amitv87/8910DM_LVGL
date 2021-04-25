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
 *    Fabien Fleutot - Please refer to git log
 *    Toby Jaffey - Please refer to git log
 *    Bosch Software Innovations GmbH - Please refer to git log
 *    Pascal Rieux - Please refer to git log
 *    Baijie & Longrong, China Mobile - Please refer to git log
 *    
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
#include "cis_internals.h"
#include "std_object/std_object.h"
#include "cis_log.h"

static bool prv_getInstId(uint8_t* inst_bitmap,cis_instcount_t index,cis_iid_t* iid);
static int  prv_getObjectTemplate(uint8_t * buffer, size_t length, uint32_t id);
static coap_status_t prv_createFotaObjects(st_context_t * context);
static coap_status_t prv_createMoniterObjects(st_context_t * context);

coap_status_t object_asynAckReadData(st_context_t * context,st_request_t* request,cis_coapret_t result)
{
    et_media_type_t formatP;
    uint8_t* bufferP = NULL;
    int32_t length = 0;
    coap_packet_t packet[1];

    st_data_t* value = request->respData;
    uint16_t size = request->respDataCount;

    if (result == COAP_205_CONTENT && value != NULL)
    {
        if (request->type == CALLBACK_TYPE_READ)
        {
			formatP = request->format;
			length = data_serialize(&request->uri, size, value, &formatP, &bufferP);
			if (length <= -1) 
			{
				LOGD("object_asyn_ack_readdata data serialize failed.");
				return COAP_500_INTERNAL_SERVER_ERROR;
			}
        }else if(request->type == CALLBACK_TYPE_DISCOVER)
		{
			formatP = LWM2M_CONTENT_LINK;
			length = discover_serialize(context, &request->uri, size, value, &bufferP);
			if ( length <= 0 ) 
			{
			        if(bufferP != NULL)
				    cis_free(bufferP);
				return COAP_500_INTERNAL_SERVER_ERROR;
			}
		}
    }

    coap_init_message(packet, COAP_TYPE_ACK, result, (uint16_t)request->mid);
    coap_set_header_token(packet, request->token, request->tokenLen);
    if(length > 0)
    {
        coap_set_header_content_type(packet, formatP);
        coap_set_payload(packet, bufferP, length);
    }
    packet_send(context, packet);
    cis_free(bufferP);

    return COAP_NO_ERROR;
}

coap_status_t object_asynAckNodata(st_context_t * context,st_request_t* request,cis_coapret_t result)
{
    coap_packet_t response[1];
    coap_init_message(response,COAP_TYPE_ACK,result, (uint16_t)request->mid);
    coap_set_header_token(response, request->token, request->tokenLen);
    packet_send(context, response);

    return COAP_NO_ERROR;
}
#if CIS_ENABLE_UPDATE
coap_status_t object_asynAckBlockWrite(st_context_t * context,st_request_t* request,cis_coapret_t result,uint8_t code)
{
	coap_packet_t response[1];
	coap_init_message(response,(coap_message_type_t)code,result, (uint16_t)request->mid);
	coap_set_header_token(response, request->token, request->tokenLen);
	coap_set_header_block1(response,context->fw_request.block1_num, context->fw_request.block1_more,context->fw_request.block1_size);
	//coap_set_header_block1()
	packet_send(context, response);

	return COAP_NO_ERROR;
}
#endif
void object_removeAll(st_context_t* context)
{
    while (NULL != context->objectList)
    {
        st_object_t * targetP;
        targetP = context->objectList;
        context->objectList = context->objectList->next;
        if(targetP->instBitmapPtr)
        {
            cis_free(targetP->instBitmapPtr);
        }
        if(targetP->userData)
        {
            cis_free(targetP->userData);
        }
        cis_free(targetP);
    }
    context->objectList = NULL;
}


coap_status_t object_read(st_context_t * contextP, st_uri_t * uriP, coap_packet_t* message)
{
	coap_status_t   result;
	st_object_t *   targetP;
	uint8_t*        bitmapPtr;
	st_data_t *dataP = NULL;
	int32_t size = 0;

	LOG_URI("object read", uriP);
	LOGD("read objectid: %d,flag: %d, msgid: %d", uriP->objectId, uriP->flag, message->mid);

	if (std_object_isStdObject(uriP->objectId))
	{

#if CIS_ENABLE_UPDATE
		if (uriP->objectId == CIS_DEVICE_OBJECT_ID || uriP->objectId == CIS_FIRMWARE_OBJECT_ID || uriP->objectId == CIS_CONNECTIVITY_OBJECT_ID || uriP->objectId == CIS_DM_OBJECT_ID)
		{
		    LOGD("read objectid: %d,flag: %d, msgid: %d",uriP->objectId,uriP->flag,message->mid);
			if(!contextP->fota_created && !contextP->isDM)
			{
    			if (contextP->fw_flag)
                {
                    if (cissys_resetFwContext())
                    {
                        LOGD("object_read cis_fw_context success");
                    }
                    else
                    {
                        LOGD("object_read cis_fw_context fail");
                    }
                }
				prv_createFotaObjects(contextP);
			}
			if (!contextP->fota_created)
			{
				return COAP_404_NOT_FOUND;
			}
		}
#endif //CIS_ENABLE_UPDATE


#if CIS_ENABLE_MONITER
		if (uriP->objectId == CIS_MONITOR_OBJECT_ID)
		{
			if (!contextP->moniter_created)
			{
				prv_createMoniterObjects(contextP);
			}
			if (!contextP->moniter_created)
			{
				return COAP_503_SERVICE_UNAVAILABLE;
			}
		}
#endif //CIS_ENABLE_MONITER

		targetP = (st_object_t *)CIS_LIST_FIND(contextP->objectList, uriP->objectId);
		if (NULL == targetP)
		{
			return COAP_404_NOT_FOUND;
		}
/*		
#if CIS_ENABLE_UPDATE_MCU
		if (uriP->objectId == CIS_DEVICE_OBJECT_ID && contextP->isupdate_mcu == true )
		{
			if (CIS_URI_IS_SET_INSTANCE(uriP))
			{
					bitmapPtr = targetP->instBitmapPtr;
					if( !object_checkInstExisted( bitmapPtr, uriP->instanceId)) 
					{
						return COAP_404_NOT_FOUND;
					}
			}
				result = contextP->callback.onRead(contextP,uriP,message->mid);
				
				if(result == COAP_206_CONFORM){
					packet_asynPushRequest(contextP,message,CALLBACK_TYPE_READ,message->mid);
					result = COAP_IGNORE;
					}
					return result;
				}
			else if (uriP->objectId == CIS_DEVICE_OBJECT_ID && (uriP->resourceId ==RES_O_SOFTWARE_VERSION||uriP->resourceId ==RES_O_MEMORY_FREE_MCU))
			{
				if (CIS_URI_IS_SET_INSTANCE(uriP))
				{
					bitmapPtr = targetP->instBitmapPtr;
					if( !object_checkInstExisted( bitmapPtr, uriP->instanceId)) 
					{
							return COAP_404_NOT_FOUND;
					}
				}
					result = contextP->callback.onRead(contextP,uriP,message->mid);
				
					if(result == COAP_206_CONFORM){
						packet_asynPushRequest(contextP,message,CALLBACK_TYPE_READ,message->mid);
						result = COAP_IGNORE;
					 } 
						return result;
		}
#endif		
*/

        if (!contextP->isDM)
        {
            dataP = data_new(1);
            cissys_assert(dataP != NULL);
            memset(dataP, 0, sizeof(st_data_t));
            if (CIS_URI_IS_SET_RESOURCE(uriP))
            {
                size = 1;
                dataP->id = uriP->resourceId;
            }
            else
            {
                dataP->type = DATA_TYPE_OBJECT_INSTANCE;
            }
        }
		std_object_read_handler(contextP, 0, (int*)&size, &dataP, targetP);

		st_request_t* request;
		request = (st_request_t *)cis_malloc(sizeof(st_request_t));
		request->format = utils_convertMediaType(message->content_type);
		request->mid = message->mid;

		cis_memcpy(request->token, message->token, message->token_len);
		request->tokenLen = message->token_len;
		request->type = CALLBACK_TYPE_READ;

		request->uri = *uriP;
		request->respData = dataP;

        if (contextP->isDM)
        {
            request->respDataCount = size;
        }
        else
        {
            request->respDataCount = 1; //TODO
        }

		result = object_asynAckReadData(contextP, request, COAP_205_CONTENT);

		cis_free(request);

        if (contextP->isDM)
        {
            data_free(size,dataP);
        }
        else
        {
            data_free(1,dataP);
        }

		result = COAP_IGNORE;
		return result;
	}


	targetP = (st_object_t *)CIS_LIST_FIND(contextP->objectList, uriP->objectId);
	if (NULL == targetP)
	{
		return COAP_404_NOT_FOUND;
	}

	if (CIS_URI_IS_SET_INSTANCE(uriP))
	{
		bitmapPtr = targetP->instBitmapPtr;
		if (!object_checkInstExisted(bitmapPtr, uriP->instanceId))
		{
			return COAP_404_NOT_FOUND;
		}
	}


	result = contextP->callback.onRead(contextP, uriP, message->mid);

	if (result == COAP_206_CONFORM) {
		packet_asynPushRequest(contextP, message, CALLBACK_TYPE_READ, message->mid);
		result = COAP_IGNORE;
	}

	return result;
}



coap_status_t object_write(st_context_t * contextP,
                           st_uri_t * uriP,
                           et_media_type_t format,
                           uint8_t * buffer,
                           size_t length,
						   coap_packet_t* message)
{
    coap_status_t result = COAP_NO_ERROR;
    st_object_t * targetP;
    cis_rescount_t resCount = 0;
    cis_rescount_t resIndex;
	st_data_t * dataP = NULL;
	st_data_t *instData = NULL;
	int size = 0;

    targetP = (st_object_t *)CIS_LIST_FIND(contextP->objectList, uriP->objectId);
    if (NULL == targetP)
    {
        result = COAP_404_NOT_FOUND;
    }

    if (result == COAP_NO_ERROR && CIS_URI_IS_SET_INSTANCE(uriP))
    {
        if( !object_checkInstExisted( targetP->instBitmapPtr, uriP->instanceId)) 
        {
            result = COAP_404_NOT_FOUND;
        }
    }
#if CIS_ENABLE_UPDATE
	if(uriP->objectId == CIS_DEVICE_OBJECT_ID || uriP->objectId == CIS_FIRMWARE_OBJECT_ID || uriP->objectId == CIS_CONNECTIVITY_OBJECT_ID)
	{
		if (!contextP->fota_created)
		{
			return COAP_400_BAD_REQUEST;
		}
		if(message->mid<contextP->fw_request.single_mid)
			return COAP_IGNORE;
		if (contextP->fw_request.single_mid== message->mid) //repeat message   
		{		
		  LOGD("single packet repeat,msgid:%d",message->mid);		
		  switch(contextP->fw_request.write_state)		
		  {		
		     case PACKAGE_WRITE_FAIL:		
			 case PACKAGE_WRITE_SUCCESS:	
			 case PACKAGE_WRITE_IDIL:             
			 object_asynAckNodata(contextP,contextP->fw_request.response_ack,contextP->fw_request.code);		
			 case PACKAGE_WRITING:			
			 	break;		
			default:			
				break;	
			}		
		   return COAP_IGNORE;      
		 }	    
		contextP->fw_request.single_mid= message->mid;
		contextP->fw_request.response_ack->mid = message->mid;
		cis_memcpy(contextP->fw_request.response_ack->token,message->token,message->token_len);
		contextP->fw_request.response_ack->tokenLen = message->token_len;
		contextP->fw_request.response_ack->type = CALLBACK_TYPE_WRITE;

		contextP->fw_request.response_ack->uri = *uriP;

		if (!CIS_URI_IS_SET_INSTANCE(uriP))
		{
			return COAP_404_NOT_FOUND;
		}
		if(contextP->fw_request.write_state==PACKAGE_WRITING)	
			return COAP_IGNORE;
		LOGD("write objectid: %d,flag: %d, msgid: %d",uriP->objectId,uriP->flag,message->mid);
		coap_status_t  writeResult = COAP_404_NOT_FOUND;

		size = data_parse(uriP, buffer, length, format, &dataP);
		if (size <0)
		{
			return COAP_406_NOT_ACCEPTABLE;
		}


		if(dataP->type == (et_data_type_t)cis_data_type_string ||
			dataP->type == (et_data_type_t)cis_data_type_opaque ||
			dataP->type == (et_data_type_t)cis_data_type_integer ||
			dataP->type == (et_data_type_t)cis_data_type_float ||
			dataP->type == (et_data_type_t)cis_data_type_bool)
		{
			instData = data_new(1);
			instData->id = uriP->instanceId;
			instData->type = DATA_TYPE_OBJECT_INSTANCE;
			instData->value.asChildren.array = dataP;
			instData->value.asChildren.count = size;
			writeResult = std_object_write_handler(contextP,uriP->instanceId,1,instData,targetP);
			data_free(1, instData);
		}
		else if(dataP->type == DATA_TYPE_OBJECT_INSTANCE ||
			dataP->type == DATA_TYPE_OBJECT)
		{
			instData = dataP;
			writeResult = std_object_write_handler(contextP,uriP->instanceId,1,instData,targetP);
			data_free(size, instData);
		}		

		LOGD("write result %d",writeResult);
		if (!contextP->fw_request.need_async_ack)   //need async ack
		{
			contextP->fw_request.need_async_ack = false;
			result = object_asynAckNodata(contextP,contextP->fw_request.response_ack,COAP_204_CHANGED);
		}
		result = COAP_IGNORE;
		return result;
	}
#endif

#if CIS_ENABLE_CMIOT_OTA


	if(uriP->objectId == CIS_POWERUPLOG_OBJECT_ID|| uriP->objectId == CIS_CMDHDEFECVALUES_OBJECT_ID)
	{
		size = data_parse(uriP, buffer, length, format, &dataP);
	
		LOGD("write objectid: %d,flag: %d, msgid: %d",uriP->objectId,uriP->flag,message->mid);

		if(uriP->objectId == CIS_CMDHDEFECVALUES_OBJECT_ID)
		{
			coap_status_t  writeResult = COAP_404_NOT_FOUND;
			if(dataP->type == cis_data_type_string ||
				dataP->type == cis_data_type_opaque ||
				dataP->type == cis_data_type_integer ||
				dataP->type == cis_data_type_float ||
				dataP->type == cis_data_type_bool)

			{
				instData = data_new(1);
				instData->id = uriP->instanceId;
				instData->type = DATA_TYPE_OBJECT_INSTANCE;
				instData->value.asChildren.array = dataP;		
				instData->value.asChildren.count = size;


				
				writeResult = std_object_write_handler(contextP,uriP->instanceId,1,instData,targetP);
				LOGD("write result %d",writeResult);

				st_request_t* request;
				request = (st_request_t *)cis_malloc(sizeof(st_request_t));
				request->mid = message->mid;
				
				cis_memcpy(request->token,message->token,message->token_len);
				request->tokenLen = message->token_len;
				request->type = CALLBACK_TYPE_WRITE;
				
				result = object_asynAckNodata(contextP,request,COAP_204_CHANGED);
				
				cis_free(request);

				data_free(1, instData);
			}
		}		
		result = COAP_IGNORE;
		return result;			
	}
#endif

    if (result == COAP_NO_ERROR)
    {
        //NOTE: before object_write called, caller guarantees uriP shall carry instanceID,
        //  So we don't do check the intance ID here.
        // And uriP might NOT carry resource ID while dataP is TLV type and have multiple dataset
        resCount = (cis_rescount_t)data_parse(uriP, buffer, length, format, &dataP);
        if (resCount <= 0)
        {
            return COAP_406_NOT_ACCEPTABLE;
        }
            

        if(dataP->type == DATA_TYPE_STRING ||
            dataP->type == DATA_TYPE_OPAQUE ||
            dataP->type == DATA_TYPE_INTEGER ||
            dataP->type == DATA_TYPE_FLOAT ||
            dataP->type == DATA_TYPE_BOOL)
        {
            cis_data_t* data;

            data = (cis_data_t*)cis_malloc(sizeof(cis_data_t) * resCount);
            if(data == NULL)
            {
                return COAP_500_INTERNAL_SERVER_ERROR;
            }

            result = COAP_206_CONFORM;
            for (resIndex = 0;resIndex< resCount;resIndex++)
            {
                data[resIndex].id = dataP[resIndex].id;
                data[resIndex].type = (cis_datatype_t )dataP[resIndex].type;
                data[resIndex].asBuffer.length = dataP[resIndex].asBuffer.length;
                data[resIndex].asBuffer.buffer = utils_bufferdup(dataP[resIndex].asBuffer.buffer,dataP[resIndex].asBuffer.length);
                cis_memcpy(&data[resIndex].value.asFloat,&dataP[resIndex].value.asFloat,sizeof(data[resIndex].value));
            }

            data_free(resCount, dataP);
            uri_make(uriP->objectId,uriP->instanceId,URI_INVALID,uriP);
            result = contextP->callback.onWrite(contextP,uriP,data,resCount,message->mid);

            for (resIndex = 0;resIndex< resCount;resIndex++)
            {
                if(data[resIndex].type == cis_data_type_opaque || data[resIndex].type == cis_data_type_string)
                {
                    cis_free(data[resIndex].asBuffer.buffer);
                }
            }

            cis_free(data);

        }
        
    }

    if(result == COAP_206_CONFORM){
        packet_asynPushRequest(contextP,message,CALLBACK_TYPE_WRITE,message->mid);
        result = COAP_IGNORE;
    }

    return result;
}

coap_status_t object_execute(st_context_t * contextP,
                             st_uri_t * uriP,
                             uint8_t * buffer,
                             size_t length,
							 coap_packet_t* message)
{
    st_object_t * targetP;
	coap_status_t result = COAP_NO_ERROR;
    targetP = (st_object_t *)CIS_LIST_FIND(contextP->objectList, uriP->objectId);
    if (NULL == targetP) 
    {
        return COAP_404_NOT_FOUND;
    }
    
    if (CIS_URI_IS_SET_INSTANCE(uriP))
    {
        if( !object_checkInstExisted( targetP->instBitmapPtr, uriP->instanceId)) 
        {
            result = COAP_404_NOT_FOUND;
        }
    }
#if CIS_ENABLE_UPDATE 
	if(uriP->objectId == CIS_DEVICE_OBJECT_ID || uriP->objectId == CIS_FIRMWARE_OBJECT_ID || uriP->objectId == CIS_CONNECTIVITY_OBJECT_ID)
	{
		if (!CIS_URI_IS_SET_INSTANCE(uriP))
		{
			return COAP_404_NOT_FOUND;
		}
	    if (contextP->fw_request.single_mid== message->mid) //repeat message  
		{		
		  LOGD("single packet repeat,msgid:%d",message->mid);			
		  return COAP_IGNORE;      
		 }
		contextP->fw_request.single_mid= message->mid;//2018/12/25
		LOGD("execute objectid: %d,flag: %d, msgid: %d",uriP->objectId,uriP->flag,message->mid);
	#if CIS_ENABLE_UPDATE_MCU	
		if (uriP->objectId == CIS_FIRMWARE_OBJECT_ID && (uriP->resourceId == RES_M_FOTA_TRIGGER || uriP->resourceId == RES_M_SWITCH_MCU_UPDATE))
	#else
		if (uriP->objectId == CIS_FIRMWARE_OBJECT_ID && uriP->resourceId == RES_M_FOTA_TRIGGER)
	#endif
        {
             result = std_object_exec_handler(contextP,uriP->instanceId,uriP->resourceId,buffer,length,targetP);
			 return result;
		}

		if (!contextP->fota_created)
		{
			return COAP_404_NOT_FOUND;
		}
		

		contextP->fw_request.response_ack->mid = message->mid;

		cis_memcpy(contextP->fw_request.response_ack->token,message->token,message->token_len);
		contextP->fw_request.response_ack->tokenLen = message->token_len;
		contextP->fw_request.response_ack->type = CALLBACK_TYPE_EXECUTE;
		contextP->fw_request.response_ack->uri = *uriP;
		//contextP->fw_request.response_ack = request;
		result = std_object_exec_handler(contextP,uriP->instanceId,uriP->resourceId,buffer,length,targetP);
        /*
		if (uriP->resourceId != RES_M_UPDATE)
		{
	      result = COAP_IGNORE;
	    }*/ //2018/12/25
		return result;
	}
#endif
    if(result == COAP_NO_ERROR){
        //NOTE: refer 5.4.5, caller makes sure uriP has carried instanceID, resourceID
        result = contextP->callback.onExec(contextP,uriP,buffer,length,message->mid);
    }
    

    if(result == COAP_206_CONFORM){
        packet_asynPushRequest(contextP,message,CALLBACK_TYPE_EXECUTE,message->mid);
        result = COAP_IGNORE;
    }

    return result;
}


coap_status_t object_discover(st_context_t * contextP,st_uri_t * uriP,coap_packet_t* message)
{
	coap_status_t result;
	st_object_t * targetP;
	uint8_t*        bitmapPtr;

	st_data_t * dataP = NULL;
	int size = 0;

	LOGD("flag: %d, msgid: %d",uriP->flag,message->mid);
	targetP = (st_object_t *)CIS_LIST_FIND(contextP->objectList, uriP->objectId);
	if (NULL == targetP)
	{
		return COAP_404_NOT_FOUND;
	}
#if CIS_ENABLE_UPDATE
	if(uriP->objectId == CIS_DEVICE_OBJECT_ID || uriP->objectId == CIS_FIRMWARE_OBJECT_ID || uriP->objectId == CIS_CONNECTIVITY_OBJECT_ID)
	{
		LOGD("discover objectid: %d,flag: %d, msgid: %d",uriP->objectId,uriP->flag,message->mid);

		dataP = data_new(1);
		cissys_assert(dataP != NULL);
		cis_memset(dataP, 0, sizeof(cis_data_t));
		dataP->type = DATA_TYPE_OBJECT_INSTANCE;

		std_object_discover_handler(contextP,0,&size,&dataP,targetP);

		st_request_t* request;
		request = (st_request_t *)cis_malloc(sizeof(st_request_t));
		request->format = LWM2M_CONTENT_LINK;
		request->mid = message->mid;
			
		cis_memcpy(request->token,message->token,message->token_len);
		request->tokenLen = message->token_len;
		request->type = CALLBACK_TYPE_DISCOVER;

		request->uri = *uriP;
		request->respData = dataP;
		request->respDataCount = 1; //TODO
		result = object_asynAckReadData(contextP,request,COAP_205_CONTENT);
		cis_free(request);
		data_free(1,dataP);
		return result;
		
	}
#endif
#if CIS_ENABLE_MONITER
	if(uriP->objectId == CIS_MONITOR_OBJECT_ID || uriP->objectId == CIS_CONNECTIVITY_OBJECT_ID)
	{
		LOGD("discover objectid: %d,flag: %d, msgid: %d",uriP->objectId,uriP->flag,message->mid);

		dataP = data_new(1);
		cissys_assert(dataP != NULL);
		cis_memset(dataP, 0, sizeof(cis_data_t));
		dataP->type = DATA_TYPE_OBJECT_INSTANCE;

		std_object_discover_handler(contextP,0,&size,&dataP,targetP);

		st_request_t* request;
		request = (st_request_t *)cis_malloc(sizeof(st_request_t));
		request->format = LWM2M_CONTENT_LINK;
		request->mid = message->mid;
			
		cis_memcpy(request->token,message->token,message->token_len);
		request->tokenLen = message->token_len;
		request->type = CALLBACK_TYPE_DISCOVER;

		request->uri = *uriP;
		request->respData = dataP;
		request->respDataCount = 1; //TODO
		result = object_asynAckReadData(contextP,request,COAP_205_CONTENT);
		cis_free(request);
		data_free(1,dataP);
		return result;
		
	}
#endif

#if CIS_ENABLE_CMIOT_OTA
		if((uriP->objectId == CIS_POWERUPLOG_OBJECT_ID || uriP->objectId == CIS_CMDHDEFECVALUES_OBJECT_ID)&&(uriP->instanceId == 0))
		{
			LOGD("discover objectid: %d,flag: %d, msgid: %d",uriP->objectId,uriP->flag,message->mid);
	
			dataP = data_new(1);
			cissys_assert(dataP != NULL);
			memset(dataP, 0, sizeof(cis_data_t));
			dataP->type = DATA_TYPE_OBJECT_INSTANCE;
	
			std_object_discover_handler(contextP,0,&size,&dataP,targetP);
	
			st_request_t* request;
			request = (st_request_t *)cis_malloc(sizeof(st_request_t));
			request->format = LWM2M_CONTENT_LINK;
			request->mid = message->mid;
				
			cis_memcpy(request->token,message->token,message->token_len);
			request->tokenLen = message->token_len;
			request->type = CALLBACK_TYPE_DISCOVER;
	
			request->uri = *uriP;
			request->respData = dataP;
			request->respDataCount = 1; //TODO
			result = object_asynAckReadData(contextP,request,COAP_205_CONTENT);
			cis_free(request);
			data_free(1,dataP);
			return result;
			
		}
#endif
	if (CIS_URI_IS_SET_INSTANCE(uriP))//single instance
	{
		bitmapPtr = targetP->instBitmapPtr;
		if( !object_checkInstExisted( bitmapPtr, uriP->instanceId)) 
		{
			return COAP_404_NOT_FOUND;
		}
    }

	result = contextP->callback.onDiscover(contextP,uriP,message->mid);

	if(result == COAP_206_CONFORM){
		packet_asynPushRequest(contextP,message,CALLBACK_TYPE_DISCOVER,message->mid);
        result = COAP_IGNORE;
	}

	return result;
}

bool object_isInstanceNew(st_context_t * contextP,
                          cis_oid_t  objectId,
                          cis_iid_t  instanceId)
{
    st_object_t * targetP;

    targetP = (st_object_t *)CIS_LIST_FIND(contextP->objectList, objectId);
    if (targetP != NULL)
    {
        
        uint8_t* bitmapPtr = targetP->instBitmapPtr;
        cis_instcount_t bitmapBytes = targetP->instBitmapBytes;
        if( bitmapBytes == 0) 
        {
            return false;
        }

        if( !object_checkInstExisted( bitmapPtr, instanceId))
        {
            return false;
        }
    }
    return true;
}


int object_getRegisterPayload(st_context_t * contextP,
                           uint8_t * buffer,
                           size_t bufferLen)
{
    size_t index;
    int result;
    st_object_t * objectP;
    cis_iid_t bitmapIndex;


    // index can not be greater than bufferLen
    index = 0;

    result = utils_stringCopy((char *)buffer, bufferLen, REG_START);
    if (result < 0) 
    {
        return 0;
    }
    index += result;

    if ((contextP->altPath != NULL)
     && (contextP->altPath[0] != 0))
    {
        result = utils_stringCopy((char *)buffer + index, bufferLen - index, contextP->altPath);
    }
    else
    {
        result = utils_stringCopy((char *)buffer + index, bufferLen - index, REG_DEFAULT_PATH);
    }
    if (result < 0) 
    {
        return 0;
    }
    index += result;

    result = utils_stringCopy((char *)buffer + index, bufferLen - index, REG_LWM2M_RESOURCE_TYPE);
    if (result < 0) 
    {
        return 0;
    }
    index += result;

    for (objectP = contextP->objectList; objectP != NULL; objectP = objectP->next)
    {
        size_t start;
        size_t length;

        start = index;
        result = prv_getObjectTemplate(buffer + index, bufferLen - index, objectP->objID);
        if (result < 0) 
        {
            return 0;
        }
        length = result;
        index += length;

        uint8_t* bitmapPtr = objectP->instBitmapPtr;
        cis_instcount_t bitmapBytes = objectP->instBitmapBytes;
        cis_instcount_t bitmapCount = objectP->instBitmapCount;

        if (bitmapBytes == 0)
        {
            index--;  //NOTE: remove the prepended '/'
            result = utils_stringCopy((char *)buffer + index, bufferLen - index, REG_PATH_END);
            if (result < 0) 
            {
                return 0;
            }
            index += result;
        }
        else
        {
            for(bitmapIndex=0; bitmapIndex<bitmapCount; bitmapIndex++)
            {
                cis_iid_t instanceId;;
                if(!prv_getInstId( bitmapPtr, bitmapIndex , &instanceId)) 
                {
                    continue;
                }

                if (bufferLen - index <= length) 
                {
                    return 0;
                }

                if (index != start + length)
                {
                    cis_memcpy(buffer + index, buffer + start, length);
                    index += length;
                }

                result = utils_intCopy((char *)buffer + index, bufferLen - index, instanceId);
                if (result < 0) 
                {
                    return 0;
                }
                index += result;

                result = utils_stringCopy((char *)buffer + index, bufferLen - index, REG_PATH_END);
                if (result < 0) 
                {
                    return 0;
                }
                index += result;
            }
        }
    }

    if (index > 0)
    {
        index = index - 1;  // remove trailing ','
    }

    buffer[index] = 0;

    return index;
}



bool object_checkInstExisted( uint8_t* inst_bitmap, cis_iid_t iid)
{
    cis_instcount_t bytes = iid / 8;
    cis_instcount_t bits = iid % 8;
    if(inst_bitmap == NULL)return true;
    return (((*(inst_bitmap + bytes) >> (7 - bits)) & 0x01) > 0);
}


//////////////////////////////////////////////////////////////////////////


static bool prv_getInstId(uint8_t* inst_bitmap,cis_instcount_t index,cis_iid_t* iid)
{
    if (inst_bitmap == NULL)
    {
        return 0;
    }

    cis_instcount_t bytes = index / 8;
    cis_instcount_t bits = index % 8;

    if ((*(inst_bitmap + bytes) >> (7 - bits)) & 0x01) {
        if(iid != NULL)*iid = index;
        return true;
    }
    return false;
}






static int prv_getObjectTemplate(uint8_t * buffer, size_t length, uint32_t id)
{
    int index;
    int result;

    if (length < REG_OBJECT_MIN_LEN) 
    {
        return -1;
    }

    buffer[0] = '<';
    buffer[1] = '/';
    index = 2;

    result = utils_intCopy((char *)buffer + index, length - index, id);
    if (result < 0) 
    {
        return -1;
    }
    index += result;

    if (length - index < REG_OBJECT_MIN_LEN - 3) 
    {
        return -1;
    }
    buffer[index] = '/';
    index++;

    return index;
}

#if CIS_ENABLE_UPDATE
coap_status_t prv_createFotaObjects(st_context_t * contextP)
{
	cis_ret_t  ret = COAP_IGNORE;

	contextP->fw_sys_state = FOTA_STATE_IDIL;
	contextP->fw_request.block1_mid = 0;
	contextP->fw_request.single_mid = 0;
	contextP->fw_request.block1_more = 0;
	contextP->fw_request.block1_num = 0;
	contextP->fw_request.block1_size = REST_MAX_CHUNK_SIZE;
	st_request_t* request = (st_request_t *)cis_malloc(sizeof(st_request_t));
	if (request == NULL)
	{
		reset_fotaIDIL(contextP, CIS_EVENT_FIRMWARE_UPDATE_FAILED);
		return CIS_RET_ERROR;
	}
	cis_memset(request, 0, sizeof(struct st_request));

	contextP->fw_request.tv_t = 0;
	contextP->fw_request.write_state = PACKAGE_WRITE_IDIL;
	//contextP->fw_request.ack_send_count = 0;
	contextP->fw_request.single_packet_write = false;
	contextP->fw_request.need_async_ack = false;
	contextP->fw_request.response_ack = request;

	//create new object for update
#if CIS_ENABLE_UPDATE_MCU	
    cis_res_count_t  rescount;
    rescount.actCount=0;
	rescount.attrCount=5;
	cis_addobject(contextP,std_object_device,NULL,&rescount);
#else
    cis_addobject(contextP,std_object_device,NULL,NULL);
#endif
	st_object_t* deviceObj = prv_findObject(contextP, std_object_device);
	if (deviceObj == NULL)
	{
		LOGE("ERROR:Failed to init device object");
		return COAP_404_NOT_FOUND;
	}
	std_device_create(contextP, 0, deviceObj);
	
	ret=cis_addobject(contextP, std_object_conn, NULL, NULL);
	st_object_t* connObj = prv_findObject(contextP, std_object_conn);
	if (connObj == NULL)
	{
		LOGE("ERROR:Failed to init conn object");
		return COAP_404_NOT_FOUND;
	}
	if(ret!=CIS_RET_EXIST)
	 std_conn_moniter_create(contextP, 0, connObj);

	cis_addobject(contextP, std_object_firmware, NULL, NULL);
	st_object_t* firmwareObj = prv_findObject(contextP, std_object_firmware);
	if (firmwareObj == NULL)
	{
		LOGE("ERROR:Failed to init firmware object");
		return COAP_404_NOT_FOUND;
	}
	 std_firmware_create(contextP, 0, firmwareObj);
#if CIS_ENABLE_UPDATE_MCU
	contextP->isupdate_mcu=cissys_getSwState();
#endif

	ret = cis_update_reg(contextP, 0, true);
    contextP->fota_created = true;
	return ret;
}
#endif

#if CIS_ENABLE_MONITER
coap_status_t prv_createMoniterObjects(st_context_t * context)
{
	cis_ret_t  ret = COAP_IGNORE;
	
	ret=cis_addobject(context, std_object_conn, NULL, NULL);
	st_object_t* connObj = prv_findObject(context, std_object_conn);
	if (connObj == NULL)
	{
		LOGE("ERROR:Failed to init conn object");
		return COAP_404_NOT_FOUND;
	}
	if(ret!=CIS_RET_EXIST)
	  std_conn_moniter_create(context, 0, connObj);

	cis_addobject(context, std_object_monitor, NULL, NULL);
	st_object_t* moniterObj = prv_findObject(context, std_object_monitor);
	if (moniterObj == NULL)
	{
		LOGE("ERROR:Failed to init extend moniter object");
		return CIS_COAP_404_NOT_FOUND;
	}
	std_moniter_create(context, 0, moniterObj);
	ret = cis_update_reg(context, 0, true);
	context->moniter_created = true;
	return ret;
}

#endif//CIS_ENABLE_MONITER
