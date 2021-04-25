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
 *    Baijie & Longrong, China Mobile - Please refer to git log
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
   * 8 supported protocol        read/write
   * 5 update result             read
   * 9 delivery method           read/write
   * 26500 saved bytes           read/write
   * 26501 change state          exec
   */


#include "../cis_api.h"
#include "../cis_internals.h"
#include "std_object.h"


#if CIS_ENABLE_UPDATE

bool std_firmware_create(st_context_t * contextP,
	int instanceId,
	st_object_t * firmwareObj)
{
	firmware_data_t * instFirmware = NULL;
	firmware_data_t * targetP = NULL;
	uint8_t instBytes = 0;
	uint8_t instCount = 0;
	cis_iid_t instIndex;
	if (NULL == firmwareObj)
	{
		return false;
	}

	// Manually create a hard-code instance
	targetP = (firmware_data_t *)cis_malloc(sizeof(firmware_data_t));
	if (NULL == targetP)
	{
		return false;
	}

	cis_memset(targetP, 0, sizeof(firmware_data_t));
	targetP->instanceId = (uint16_t)instanceId;

	targetP->supported = TRUE;
	targetP->delivery = 0;

	targetP->fw_info = (cis_fw_context_t *)cis_malloc(sizeof(cis_fw_context_t));
	if (NULL == targetP->fw_info)
	{
		cis_free(targetP);
		return false;
	}

	cis_memset(targetP->fw_info, 0, sizeof(cis_fw_context_t));
	cissys_readContext(targetP->fw_info);
	contextP->fw_sys_state = (cis_fw_state)targetP->fw_info->state;
	cis_memcpy(targetP->download_uri, PRV_FIRMWARE_URI, sizeof(PRV_FIRMWARE_URI));
	instFirmware = (firmware_data_t *)std_object_put_firmware(contextP, (cis_list_t*)targetP);

	instCount = CIS_LIST_COUNT(instFirmware);
	if (instCount == 0)
	{
		cis_free(targetP);
		return false;
	}

	if (instCount == 1)
	{
		return true;
	}


	firmwareObj->instBitmapCount = instCount;
	instBytes = (instCount - 1) / 8 + 1;
	if (firmwareObj->instBitmapBytes < instBytes) {
		if (firmwareObj->instBitmapBytes != 0 && firmwareObj->instBitmapPtr != NULL)
		{
			cis_free(firmwareObj->instBitmapPtr);
		}
		firmwareObj->instBitmapPtr = (uint8_t*)cis_malloc(instBytes);
		firmwareObj->instBitmapBytes = instBytes;
	}
	cissys_memset(firmwareObj->instBitmapPtr, 0, instBytes);
	targetP = instFirmware;
	for (instIndex = 0; instIndex < instCount; instIndex++)
	{
		uint8_t instBytePos = targetP->instanceId / 8;
		uint8_t instByteOffset = 7 - (targetP->instanceId % 8);
		firmwareObj->instBitmapPtr[instBytePos] += 0x01 << instByteOffset;

		targetP = targetP->next;
	}

	return true;
}


static firmware_data_t * prv_firmware_find(st_context_t * contextP, cis_iid_t instanceId)
{
	firmware_data_t * targetP;
	targetP = (firmware_data_t *)(std_object_get_firmware(contextP, instanceId));
	if (NULL != targetP)
	{
		return targetP;
	}

	return NULL;
}

static uint8_t prv_firmware_get_value(st_data_t* dataArrayP, int number, firmware_data_t * firmwareP)
{
	uint8_t result = COAP_404_NOT_FOUND;
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
			resId = dataArrayP->value.asChildren.array[i].id;
			dataP = dataArrayP->value.asChildren.array + i;
		}
		switch (resId)
		{
		case RES_M_PACKAGE:
		case RES_M_UPDATE:
			result = COAP_405_METHOD_NOT_ALLOWED;
			break;
		case RES_M_PACKAGE_URI:
			data_encode_string(firmwareP->download_uri, dataP);
			result = COAP_205_CONTENT;
			break;
		case RES_M_STATE:
			data_encode_int(firmwareP->fw_info->state, dataP);
			result = COAP_205_CONTENT;
			break;
		case RES_M_UPDATE_RESULT:
			data_encode_int(firmwareP->fw_info->result, dataP);
			result = COAP_205_CONTENT;
			break;
		case RES_M_SUPPORT_PORTOCOL:
			data_encode_int(firmwareP->supported, dataP);
			result = COAP_205_CONTENT;
			break;
		case RES_M_DELIVER_METHOD:
			data_encode_int(firmwareP->delivery, dataP);
			result = COAP_205_CONTENT;
			break;
		case RES_M_TRANSGERRED_BYTES:
			//data_encode_int(firmwareP->fw_info->savebytes, dataP);
			data_encode_int(cissys_getFwSavedBytes(), dataP);
			result = COAP_205_CONTENT;
			break;
		default:
			result = COAP_404_NOT_FOUND;
			break;
		}
	}
	return result;
}

uint8_t std_firmware_read(st_context_t * contextP,
	uint16_t instanceId,
	int * numDataP,
	st_data_t ** dataArrayP,
	st_object_t * objectP)
{
	int i;
	uint8_t result = COAP_205_CONTENT;
	firmware_data_t * targetP;
	targetP = prv_firmware_find(contextP, instanceId);

	// this is a single instance object
	if (instanceId != 0 || targetP == NULL)
	{
		return COAP_404_NOT_FOUND;
	}

	// is the server asking for the full object ?
	if (*numDataP == 0)
	{
		uint16_t resList[] = {
			RES_M_PACKAGE,
			RES_M_PACKAGE_URI,
			RES_M_UPDATE,
			RES_M_STATE,
			RES_M_UPDATE_RESULT,
			RES_M_SUPPORT_PORTOCOL,
			RES_M_DELIVER_METHOD,
			RES_M_TRANSGERRED_BYTES,
			RES_M_SWITCH_TO_DOWNLOAD
		};
		int nbRes = sizeof(resList) / sizeof(uint16_t);

		(*dataArrayP)->id = 0;
		(*dataArrayP)->type = DATA_TYPE_OBJECT_INSTANCE;
		(*dataArrayP)->value.asChildren.count = nbRes;
		(*dataArrayP)->value.asChildren.array = data_new(nbRes);
		cis_memset((*dataArrayP)->value.asChildren.array, 0, (nbRes) * sizeof(cis_data_t));
		if (*dataArrayP == NULL) return COAP_500_INTERNAL_SERVER_ERROR;
		*numDataP = nbRes;

		for (i = 0; i < nbRes; i++)
		{
			(*dataArrayP)->value.asChildren.array[i].id = resList[i];
		}
		if (prv_firmware_get_value((*dataArrayP), nbRes, targetP) != COAP_205_CONTENT)
		{
			return COAP_500_INTERNAL_SERVER_ERROR;
		}

	}
	else
	{
		result = prv_firmware_get_value((*dataArrayP), 1, targetP);
	}

	return result;
}

uint8_t std_firmware_write(st_context_t * contextP,
	uint16_t instanceId,
	int numData,
	st_data_t * dataArray,
	st_object_t * objectP)
{
	int i;
	uint8_t result = COAP_405_METHOD_NOT_ALLOWED;
	firmware_data_t * targetP;
	targetP = prv_firmware_find(contextP, instanceId);
	// this is a single instance object
	if (instanceId != 0 || targetP == NULL)
	{
		return COAP_404_NOT_FOUND;
	}

	i = 0;
	do
	{
		st_data_t * data = &dataArray->value.asChildren.array[i];
		switch (data->id)
		{
		case RES_M_PACKAGE:
		{
#if CIS_ENABLE_UPDATE_MCU
			if (contextP->isupdate_mcu == false) {
				if (data->asBuffer.length == 0)//restart a new update task
				{
					contextP->fw_request.single_packet_write = false;
					contextP->fw_request.need_async_ack = true;
					contextP->fw_request.write_state = PACKAGE_WRITING;
					if (!cissys_eraseFwFlash(&(contextP->g_fotacallback)))
					{
						object_asynAckNodata(contextP, contextP->fw_request.response_ack, COAP_500_INTERNAL_SERVER_ERROR);
						reset_fotaIDIL(contextP, COAP_500_INTERNAL_SERVER_ERROR);
						return COAP_500_INTERNAL_SERVER_ERROR;
					}
				}
				else
				{
					if (targetP->fw_info->state == FOTA_STATE_DOWNLOADING)
					{
						contextP->fw_request.single_packet_write = true;
						contextP->fw_request.need_async_ack = true;
						contextP->fw_request.write_state = PACKAGE_WRITING;
						contextP->fw_request.last_packet_size = data->asBuffer.length;
						if (!cissys_writeFwBytes(targetP->fw_info, data->asBuffer.length, data->asBuffer.buffer, &(contextP->g_fotacallback)))
						{
							object_asynAckNodata(contextP, contextP->fw_request.response_ack, COAP_500_INTERNAL_SERVER_ERROR);
							reset_fotaIDIL(contextP, COAP_500_INTERNAL_SERVER_ERROR);
							return COAP_500_INTERNAL_SERVER_ERROR;
						}
					}
					else
					{
						return COAP_500_INTERNAL_SERVER_ERROR;
					}
				}
				result = COAP_204_CHANGED;
			}
			else {
				if (data->asBuffer.length == 0)//restart a new update task
				{
					contextP->fw_request.single_packet_write = false;
					contextP->fw_request.need_async_ack = true;
					contextP->fw_request.write_state = PACKAGE_WRITING;
					core_callbackEvent(contextP, CIS_EVENT_SOTA_FLASHERASE, NULL);

				}
				else
				{

					if (targetP->fw_info->state == FOTA_STATE_DOWNLOADING)
					{
						cis_uri_t uriP;
						cis_data_t *dataP;

						uriP.objectId = CIS_FIRMWARE_OBJECT_ID;
						uriP.instanceId = 0;
						uriP.resourceId = RES_M_PACKAGE;

						dataP = (cis_data_t*)cis_malloc(sizeof(cis_data_t));

						contextP->fw_request.single_packet_write = true;
						contextP->fw_request.need_async_ack = true;
						contextP->fw_request.write_state = PACKAGE_WRITING;
						contextP->fw_request.last_packet_size = data->asBuffer.length;

						dataP->id = data->id;
						dataP->type = (cis_datatype_t)data->type;
						dataP->asBuffer.length = data->asBuffer.length;
						dataP->asBuffer.buffer = utils_bufferdup(data->asBuffer.buffer, data->asBuffer.length);
						result = contextP->callback.onWrite(contextP, &uriP, dataP, 1, contextP->fw_request.single_mid);
						cis_free(dataP);
					}
					else
					{
						return COAP_500_INTERNAL_SERVER_ERROR;
					}
				}
				result = COAP_204_CHANGED;
			}

#else
			if (data->asBuffer.length == 0)//restart a new update task
			{
				contextP->fw_request.single_packet_write = false;
				contextP->fw_request.need_async_ack = true;
				contextP->fw_request.write_state = PACKAGE_WRITING;
				if (!cissys_eraseFwFlash(&(contextP->g_fotacallback)))
				{
					object_asynAckNodata(contextP, contextP->fw_request.response_ack, COAP_500_INTERNAL_SERVER_ERROR);
					reset_fotaIDIL(contextP, COAP_500_INTERNAL_SERVER_ERROR);
					return COAP_500_INTERNAL_SERVER_ERROR;
				}
				//				contextP->fw_request.single_packet_write = true;
				//				contextP->fw_request.need_async_ack = true;
			}
			else
			{
				if (targetP->fw_info->state == FOTA_STATE_DOWNLOADING)
				{
					contextP->fw_request.single_packet_write = true;
					contextP->fw_request.need_async_ack = true;
					contextP->fw_request.write_state = PACKAGE_WRITING;
					contextP->fw_request.last_packet_size = data->asBuffer.length;
					if (!cissys_writeFwBytes(data->asBuffer.length, data->asBuffer.buffer, &(contextP->g_fotacallback)))
					{
						object_asynAckNodata(contextP, contextP->fw_request.response_ack, COAP_500_INTERNAL_SERVER_ERROR);
						reset_fotaIDIL(contextP, COAP_500_INTERNAL_SERVER_ERROR);
						return COAP_500_INTERNAL_SERVER_ERROR;
					}
					//					contextP->fw_request.single_packet_write = true;
					//					contextP->fw_request.need_async_ack = true;
				}
				else
				{
					return COAP_500_INTERNAL_SERVER_ERROR;
				}
			}
			result = COAP_204_CHANGED;
#endif						
		}
		break;

		case RES_M_PACKAGE_URI:
		{
			// URL for download the firmware
			int length = data->asBuffer.length;
			if (length == 0)
			{
				return COAP_400_BAD_REQUEST;
			}
			else
			{
				if (length > PRV_FIRMWARE_URI_LEN - 1)
				{
					length = PRV_FIRMWARE_URI_LEN - 1;
				}

				cis_memcpy(targetP->download_uri, data->asBuffer.buffer, length);
				targetP->download_uri[length] = '\0';
			}
			result = COAP_204_CHANGED;
		}
		break;

		case RES_M_TRANSGERRED_BYTES:
		{
			result = COAP_400_BAD_REQUEST;
		}
		break;
		default:
		{
			result = COAP_405_METHOD_NOT_ALLOWED;
		}
		}

		i++;
	} while (i < dataArray->value.asChildren.count && result == COAP_204_CHANGED);

	return result;
}

cis_coapret_t std_firmware_execute(st_context_t * contextP,
	uint16_t instanceId,
	uint16_t resourceId,
	uint8_t * buffer,
	int length,
	st_object_t * objectP)
{
	firmware_data_t * targetP;
	targetP = prv_firmware_find(contextP, instanceId);
	// this is a single instance object
	if (instanceId != 0)
	{
		return COAP_404_NOT_FOUND;
	}

	if (resourceId == RES_M_FOTA_TRIGGER)
	{
		core_callbackEvent(contextP, CIS_EVENT_FIRMWARE_TRIGGER, NULL);
		return COAP_204_CHANGED;

	}
#if CIS_ENABLE_UPDATE_MCU	
	else if (resourceId == RES_M_SWITCH_MCU_UPDATE)
	{

		printf("Execute MCU_UPDATE\r\n");
		contextP->isupdate_mcu = true;
		cissys_setSwState(contextP->isupdate_mcu);
		return COAP_204_CHANGED;

	}
#endif   
	if (targetP == NULL)
	{
		return COAP_404_NOT_FOUND;

	}

	// for execute callback, resId is always set.
	switch (resourceId)
	{
	case RES_M_UPDATE:
        LOGD("std_firmware_execute case RES_M_UPDATE targetP->fw_info->state: %d;targetP->fw_info->result:%d", targetP->fw_info->state,targetP->fw_info->result);
		if (targetP->fw_info->state == FOTA_STATE_DOWNLOADED)
		{
			if (targetP->fw_info->result == FOTA_RESULT_INIT
				|| targetP->fw_info->result == FOTA_RESULT_OVERFLOW
				|| targetP->fw_info->result == FOTA_RESULT_DISCONNECT
				|| targetP->fw_info->result == FOTA_RESULT_UPDATEFAIL)//bad request
			{
				LOGD("Execute updating");
				// trigger your firmware download and update logic
				targetP->fw_info->state = FOTA_STATE_UPDATING;
				fw_state_change(contextP, FOTA_STATE_UPDATING);
				cissys_setFwState(targetP->fw_info, FOTA_STATE_UPDATING);
				object_asynAckNodata(contextP, contextP->fw_request.response_ack, COAP_204_CHANGED);

#if CIS_ENABLE_UPDATE_MCU
				if (contextP->isupdate_mcu == false)
				{
					core_callbackEvent(contextP, CIS_EVENT_FIRMWARE_UPDATING, NULL);
					if (!cissys_updateFirmware(&(contextP->g_fotacallback)))
						return COAP_500_INTERNAL_SERVER_ERROR;
				}
				else
				{
					core_callbackEvent(contextP, CIS_EVENT_SOTA_UPDATING, NULL);
				}
#else
				core_callbackEvent(contextP, CIS_EVENT_FIRMWARE_UPDATING, NULL);

				if (!cissys_updateFirmware(&(contextP->g_fotacallback)))
				{
					return COAP_500_INTERNAL_SERVER_ERROR;
				}
#endif				
				//object_asynAckNodata(contextP,contextP->fw_request.response_ack,COAP_204_CHANGED);				
				return COAP_IGNORE;
			}
			else
			{
				return COAP_400_BAD_REQUEST;
			}

		}
		else if (targetP->fw_info->state == FOTA_STATE_UPDATING)
		{

			if (targetP->fw_info->result == FOTA_RESULT_INIT
				|| targetP->fw_info->result == FOTA_RESULT_SUCCESS
				|| targetP->fw_info->result == FOTA_RESULT_NOFREE
				|| targetP->fw_info->result == FOTA_RESULT_OVERFLOW
				|| targetP->fw_info->result == FOTA_RESULT_DISCONNECT
				|| targetP->fw_info->result == FOTA_RESULT_UPDATEFAIL)//bad request
			{
				LOGD("Execute updating again");
				object_asynAckNodata(contextP, contextP->fw_request.response_ack, COAP_204_CHANGED);
#if CIS_ENABLE_UPDATE_MCU
				if (contextP->isupdate_mcu == false)
				{
					core_callbackEvent(contextP, CIS_EVENT_FIRMWARE_UPDATING, NULL);
					if (!cissys_updateFirmware(&(contextP->g_fotacallback)))
						return COAP_500_INTERNAL_SERVER_ERROR;
				}
				else
				{
					core_callbackEvent(contextP, CIS_EVENT_SOTA_UPDATING, NULL);
				}
#else
				core_callbackEvent(contextP, CIS_EVENT_FIRMWARE_UPDATING, NULL);

				if (!cissys_updateFirmware(&(contextP->g_fotacallback)))
				{
					return COAP_500_INTERNAL_SERVER_ERROR;
				}
#endif	
				//object_asynAckNodata(contextP,contextP->fw_request.response_ack,COAP_204_CHANGED);				
				return COAP_IGNORE;
			}
			else
			{
				return COAP_400_BAD_REQUEST;
			}


		}
		else
		{
			// firmware update already running
			return COAP_400_BAD_REQUEST;
		}
	case RES_M_SWITCH_TO_DOWNLOAD://change to downloading
		if (targetP->fw_info->state == FOTA_STATE_IDIL)
		{
			fw_state_change(contextP, FOTA_STATE_DOWNLOADING);
			cissys_setFwState(targetP->fw_info, FOTA_STATE_DOWNLOADING);
			object_asynAckNodata(contextP, contextP->fw_request.response_ack, COAP_204_CHANGED);
#if CIS_ENABLE_UPDATE_MCU
			if (contextP->isupdate_mcu == false)
			{
				core_callbackEvent(contextP, CIS_EVENT_FIRMWARE_DOWNLOADING, NULL);
			}
			else
			{
				core_callbackEvent(contextP, CIS_EVENT_SOTA_DOWNLOADING, NULL);
			}
#else
			core_callbackEvent(contextP, CIS_EVENT_FIRMWARE_DOWNLOADING, NULL);
#endif
			return COAP_IGNORE;//YYL 2018/12/25
		}
		else if (targetP->fw_info->state == FOTA_STATE_DOWNLOADING)
		{
			if (targetP->fw_info->result == FOTA_RESULT_INIT
				|| targetP->fw_info->result == FOTA_RESULT_DISCONNECT
				|| targetP->fw_info->result == FOTA_RESULT_INIT)  //resume downloading
			{
				object_asynAckNodata(contextP, contextP->fw_request.response_ack, COAP_204_CHANGED);
				return COAP_IGNORE;//YYL 2018/12/25
			}
			else
			{
				return COAP_400_BAD_REQUEST;
			}
		}
		else
		{
			//object_asynAckNodata(contextP, contextP->fw_request.response_ack, COAP_400_BAD_REQUEST);//YYL 2018/12/25
			return COAP_400_BAD_REQUEST;
		}
	default:
		return COAP_405_METHOD_NOT_ALLOWED;
	}
}

uint8_t std_firmware_discover(st_context_t * contextP,
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
				RES_M_PACKAGE,
				RES_M_PACKAGE_URI,
				RES_M_UPDATE,
				RES_M_STATE,
				RES_M_UPDATE_RESULT,
				RES_M_SUPPORT_PORTOCOL,
				RES_M_DELIVER_METHOD,
				RES_M_TRANSGERRED_BYTES,
				RES_M_SWITCH_TO_DOWNLOAD,
			#if CIS_ENABLE_UPDATE_MCU
				RES_M_SWITCH_MCU_UPDATE,
			#endif	
				RES_M_FOTA_TRIGGER
		};
		int nbRes = sizeof(resList) / sizeof(uint16_t);

		(*dataArrayP)->id = 0;
		(*dataArrayP)->type = DATA_TYPE_OBJECT_INSTANCE;
		(*dataArrayP)->value.asChildren.count = nbRes;
		(*dataArrayP)->value.asChildren.array = data_new(nbRes);
		cis_memset((*dataArrayP)->value.asChildren.array, 0, (nbRes) * sizeof(cis_data_t));
		if (*dataArrayP == NULL) return COAP_500_INTERNAL_SERVER_ERROR;
		*numDataP = nbRes;
		for (i = 0; i < nbRes; i++)
		{
			(*dataArrayP)->value.asChildren.array[i].id = resList[i];
			(*dataArrayP)->value.asChildren.array[i].type = DATA_TYPE_LINK;
			(*dataArrayP)->value.asChildren.array[i].value.asObjLink.objectId = CIS_FIRMWARE_OBJECT_ID;
			(*dataArrayP)->value.asChildren.array[i].value.asObjLink.instId = 0;
		}
	}
	else
	{
		for (i = 0; i < *numDataP && result == COAP_205_CONTENT; i++)
		{
			switch ((*dataArrayP)[i].id)
			{
			case RES_M_PACKAGE:
			case RES_M_PACKAGE_URI:
			case RES_M_UPDATE:
			case RES_M_STATE:
			case RES_M_UPDATE_RESULT:
				break;
			default:
				result = COAP_404_NOT_FOUND;
			}
		}
	}

	return result;
}

void std_firmware_clean(st_context_t * contextP)
{


	firmware_data_t * deleteInst;
	firmware_data_t * firmwareInstance = (firmware_data_t *)(contextP->firmware_inst);


	//save fota info to flash
	//cissys_setFwState(contextP->fw_sys_state);

	cis_fw_context_t* fw_info = (cis_fw_context_t*)(firmwareInstance->fw_info);
	cissys_setFwState(fw_info, fw_info->state);
	cissys_setFwUpdateResult(fw_info, fw_info->result);

	while (firmwareInstance != NULL)
	{
		if (contextP->fw_request.response_ack != NULL)
		{
			if (contextP->fw_request.response_ack->respData != NULL)
			{
				data_free(contextP->fw_request.response_ack->respDataCount, contextP->fw_request.response_ack->respData);
			}
			cis_free(contextP->fw_request.response_ack);
			contextP->fw_request.response_ack = NULL;
		}
		deleteInst = firmwareInstance;
		firmwareInstance = firmwareInstance->next;
		std_object_remove_firmware(contextP, (cis_list_t*)(deleteInst));
		if (deleteInst->fw_info != NULL)
			cis_free(deleteInst->fw_info);

		cis_free(deleteInst);
	}
}

#endif
