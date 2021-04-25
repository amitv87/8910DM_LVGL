#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <cis_def.h>
//#include <cis_api.h>
#include <cis_if_sys.h>
#include <cis_log.h>
#include <cis_list.h>
#if CIS_ENABLE_CMIOT_OTA
#include "std_object/std_object.h"
#define CIS_CMIOT_OTA_START_TIME_OUT_MSEC  (180*1000)
#endif


#include "cis_sample_defs.h"
#include <string.h>
#include <windows.h>

DWORD WINAPI taskThread(LPVOID lpParam);
DWORD WINAPI updateThread(LPVOID lpParam);
DWORD WINAPI keyThread(LPVOID lpParam);
#if CIS_ENABLE_CMIOT_OTA
extern uint8_t    g_cmiot_otastart_flag; //otastart -flag should be  set when OTASTART command is called by user
extern cis_ota_history_state    g_cmiot_otafinishstate_flag; //Ota finish state flag should be  set when SDK received otafinish:0 from OneNET. It means this terminal has already finished the OTA procedure successfully
extern int32_t    g_cmiot_timeout_duration;


uint8_t    cissys_recover_psm(void);
#endif
static void     prv_observeNotify(void* context, cis_uri_t* uri, cis_mid_t mid);

static void     prv_readResponse(void* context, cis_uri_t* uri, cis_mid_t mid);
static void     prv_discoverResponse(void* context, cis_uri_t* uri, cis_mid_t mid);
static void     prv_writeResponse(void* context, cis_uri_t* uri, const cis_data_t* value, cis_attrcount_t count, cis_mid_t mid);
static void     prv_execResponse(void* context, cis_uri_t* uri, const uint8_t* value, uint32_t length, cis_mid_t mid);
static void     prv_paramsResponse(void* context, cis_uri_t* uri, cis_observe_attr_t parameters, cis_mid_t mid);
static cis_data_t* prv_dataDup(const cis_data_t* value, cis_attrcount_t attrcount);


static cis_coapret_t cis_onRead(void* context, cis_uri_t* uri, cis_mid_t mid);
static cis_coapret_t cis_onWrite(void* context, cis_uri_t* uri, const cis_data_t* value, cis_attrcount_t attrcount, cis_mid_t mid);
static cis_coapret_t cis_onExec(void* context, cis_uri_t* uri, const uint8_t* value, uint32_t length, cis_mid_t mid);
static cis_coapret_t cis_onObserve(void* context, cis_uri_t* uri, bool flag, cis_mid_t mid);
static cis_coapret_t cis_onParams(void* context, cis_uri_t* uri, cis_observe_attr_t parameters, cis_mid_t mid);
static cis_coapret_t cis_onDiscover(void* context, cis_uri_t* uri, cis_mid_t mid);
static void          cis_onEvent(void* context, cis_evt_t eid, void* param);



static struct st_callback_info* g_callbackList = NULL;
static struct st_observe_info* g_observeList = NULL;

void*      g_context = NULL;
static bool       g_shutdown = false;
static bool       g_doUnregister = false;
static bool       g_doRegister = false;


static cis_time_t g_lifetimeLast = 0;
static cis_time_t g_notifyLast = 0;

static st_sample_object     g_objectList[SAMPLE_OBJECT_MAX];
static st_instance_a        g_instList_a[SAMPLE_A_INSTANCE_COUNT];
static st_instance_b        g_instList_b[SAMPLE_B_INSTANCE_COUNT];


static void prv_make_sample_data()
{
	int i = 0;
	cis_instcount_t instIndex;
	cis_instcount_t instCount;
	for (i = 0; i < SAMPLE_OBJECT_MAX; i++)
	{
		st_sample_object* obj = &g_objectList[i];
		switch (i) {
		case 0:
		{
			obj->oid = SAMPLE_OID_A;
			obj->instBitmap = SAMPLE_A_INSTANCE_BITMAP;
			instCount = SAMPLE_A_INSTANCE_COUNT;
			for (instIndex = 0; instIndex < instCount; instIndex++)
			{
				if (obj->instBitmap[instIndex] != '1')
				{
					g_instList_a[instIndex].instId = instIndex;
					g_instList_a[instIndex].enabled = false;
				}
				else
				{
					g_instList_a[instIndex].instId = instIndex;
					g_instList_a[instIndex].enabled = true;

	                g_instList_a[instIndex].instance.floatValue = cissys_rand() * 10.0 / 0xFFFFFFFF;
					g_instList_a[instIndex].instance.intValue = 666;
					strcpy(g_instList_a[instIndex].instance.strValue, "hello onenet");
				}
			}
			obj->attrCount = sizeof(const_AttrIds_a) / sizeof(cis_rid_t);
			obj->attrListPtr = const_AttrIds_a;

			obj->actCount = sizeof(const_ActIds_a) / sizeof(cis_rid_t);
			obj->actListPtr = const_ActIds_a;
		}
		break;
		case 1:
		{
			obj->oid = SAMPLE_OID_B;
			obj->instBitmap = SAMPLE_B_INSTANCE_BITMAP;
			instCount = SAMPLE_B_INSTANCE_COUNT;
			for (instIndex = 0; instIndex < instCount; instIndex++)
			{
				if (obj->instBitmap[instIndex] != '1') {
					g_instList_b[instIndex].instId = instIndex;
					g_instList_b[instIndex].enabled = false;
				}
				else
				{
					g_instList_b[instIndex].instId = instIndex;
					g_instList_b[instIndex].enabled = true;

					g_instList_b[instIndex].instance.boolValue = true;
					g_instList_b[instIndex].instance.floatValue = cissys_rand() * 10.0 / 0xFFFFFFFF;
					g_instList_b[instIndex].instance.intValue = 555;
					strcpy(g_instList_b[instIndex].instance.strValue, "test");
				}
			}

			obj->attrCount = sizeof(const_AttrIds_b) / sizeof(cis_rid_t);
			obj->attrListPtr = const_AttrIds_b;

			obj->actCount = sizeof(const_ActIds_b) / sizeof(cis_rid_t);
			obj->actListPtr = const_ActIds_b;
		}
		break;
		}
	}
}


DWORD WINAPI taskThread(LPVOID lpParam)
{
	while (!g_shutdown)
	{
		struct st_callback_info* node;
		if (g_callbackList == NULL) {
			cissys_sleepms(100);
			continue;
		}
		node = g_callbackList;
		g_callbackList = g_callbackList->next;

		switch (node->flag)
		{
		case 0:
			break;
		case SAMPLE_CALLBACK_READ:
		{
			cis_uri_t uriLocal;
			uriLocal = node->uri;
			prv_readResponse(g_context, &uriLocal, node->mid);
		}
		break;
		case SAMPLE_CALLBACK_DISCOVER:
		{
			cis_uri_t uriLocal;
			uriLocal = node->uri;
			prv_discoverResponse(g_context, &uriLocal, node->mid);
		}
		break;
		case SAMPLE_CALLBACK_WRITE:
		{
			//write
			prv_writeResponse(g_context, &node->uri, node->param.asWrite.value, node->param.asWrite.count, node->mid);
			cis_data_t* data = node->param.asWrite.value;
			cis_attrcount_t count = node->param.asWrite.count;

			for (int i = 0; i < count; i++)
			{
				if (data[i].type == cis_data_type_string || data[i].type == cis_data_type_opaque)
				{
					if (data[i].asBuffer.buffer != NULL)
						cis_free(data[i].asBuffer.buffer);
				}
			}
			cis_free(data);
		}
		break;
		case SAMPLE_CALLBACK_EXECUTE:
		{
			//exec and notify
			prv_execResponse(g_context, &node->uri, node->param.asExec.buffer, node->param.asExec.length, node->mid);
			cis_free(node->param.asExec.buffer);
		}
		break;
		case SAMPLE_CALLBACK_SETPARAMS:
		{
			//set parameters and notify
			prv_paramsResponse(g_context, &node->uri, node->param.asObserveParam.params, node->mid);
		}
		break;
		case SAMPLE_CALLBACK_OBSERVE:
		{
			if (node->param.asObserve.flag) {
				uint16_t count = 0;
				struct st_observe_info* observe_new = (struct st_observe_info*)cis_malloc(sizeof(struct st_observe_info));
				observe_new->mid = node->mid;
				observe_new->uri = node->uri;
				observe_new->next = NULL;

				g_observeList = (struct st_observe_info*)cis_list_add((cis_list_t*)g_observeList, (cis_list_t*)observe_new);

				LOGD("cis_on_observe set(%d): %d/%d/%d",
					count,
					observe_new->uri.objectId,
					CIS_URI_IS_SET_INSTANCE(&observe_new->uri) ? observe_new->uri.instanceId : -1,
					CIS_URI_IS_SET_RESOURCE(&observe_new->uri) ? observe_new->uri.resourceId : -1);

				cis_response(g_context, NULL, NULL, node->mid, CIS_RESPONSE_OBSERVE);
			}
			else {
				st_observe_info* delnode = g_observeList;

				while (delnode) {
					if (node->uri.flag == delnode->uri.flag && node->uri.objectId == delnode->uri.objectId) {
						if (node->uri.instanceId == delnode->uri.instanceId) {
							if (node->uri.resourceId == delnode->uri.resourceId) {
								break;
							}
						}
					}
					delnode = delnode->next;
				}
				if (delnode != NULL) {
					g_observeList = (struct st_observe_info *)cis_list_remove((cis_list_t *)g_observeList, delnode->mid, (cis_list_t **)&delnode);

					LOGD("cis_on_observe cancel: %d/%d/%d\n",
						delnode->uri.objectId,
						CIS_URI_IS_SET_INSTANCE(&delnode->uri) ? delnode->uri.instanceId : -1,
						CIS_URI_IS_SET_RESOURCE(&delnode->uri) ? delnode->uri.resourceId : -1);

					cis_free(delnode);
					cis_response(g_context, NULL, NULL, node->mid, CIS_RESPONSE_OBSERVE);
				}
				else {
					return CIS_RESPONSE_NOT_FOUND;
				}
			}
		}
		default:
			break;
		}

		cis_free(node);
	}

	return 1;
}


int cis_sample_entry(const uint8_t * config_bin, uint32_t config_size)

{
	cis_version_t ver;
	int index = 0;
	cis_callback_t callback;
	callback.onRead = cis_onRead;
	callback.onWrite = cis_onWrite;
	callback.onExec = cis_onExec;
	callback.onObserve = cis_onObserve;
	callback.onSetParams = cis_onParams;
	callback.onEvent = cis_onEvent;
	callback.onDiscover = cis_onDiscover;

	cis_time_t g_lifetime = 720;
	/*init sample data*/
	prv_make_sample_data();
	if (cis_init(&g_context, (void *)config_bin, config_size, NULL) != CIS_RET_OK) {
		if (g_context != NULL)
			cis_deinit(&g_context);
		printf("cis entry init failed.\n");
		return -1;
	}
    cis_set_sota_info( "2.2.0",500);
	cis_version(&ver);
	printf("CIS SDK Version:%u.%u\n", ver.major, ver.minor);

	for (index = 0; index < SAMPLE_OBJECT_MAX; index++) 
	{
		cis_inst_bitmap_t bitmap;
		cis_res_count_t  rescount;
		cis_instcount_t instCount, instBytes;
		const char* instAsciiPtr;
		uint8_t * instPtr;
		cis_oid_t oid;
		int16_t i;
		st_sample_object* obj = &g_objectList[index];

		oid = obj->oid;
		instCount = utils_strlen(obj->instBitmap);//实例的个数包括不使能的实例
		instBytes = (instCount - 1) / 8 + 1;//要用多少个字节来表示实例，每个实例占用一位
		instAsciiPtr = obj->instBitmap;
		instPtr = (uint8_t*)cis_malloc(instBytes);
		cissys_assert(instPtr != NULL);
		cis_memset(instPtr, 0, instBytes);

		for (i = 0; i < instCount; i++)//这一段代码的意思是把类似"1101"的二进制字符串转换为二进制数据1101
		{
			cis_instcount_t instBytePos = i / 8;
			cis_instcount_t instByteOffset = 7 - (i % 8);
			if (instAsciiPtr[i] == '1') {
				instPtr[instBytePos] += 0x01 << instByteOffset;
			}
		}

		bitmap.instanceCount = instCount;
		bitmap.instanceBitmap = instPtr;
		bitmap.instanceBytes = instBytes;

		rescount.attrCount = obj->attrCount;
		rescount.actCount = obj->actCount;

		cis_addobject(g_context, oid, &bitmap, &rescount);
		cis_free(instPtr);
	}

	g_shutdown = false;
	g_doUnregister = false;

	//register enabled
	g_doRegister = true;

	HANDLE hThread;
	DWORD dwThread;
	hThread = CreateThread(NULL, 0, taskThread, (LPVOID)NULL, 0, &dwThread);
	CloseHandle(hThread);

	while (!g_shutdown)
	{
		uint32_t pumpRet;
		{
			if (g_doRegister)
			{
				g_doRegister = false;
				cis_register(g_context, g_lifetime, &callback);//将回调函数注册进SDK内
			}

			if (g_doUnregister) {
				g_doUnregister = false;
				cis_unregister(g_context);
				struct st_observe_info* delnode;
				while (g_observeList != NULL) {
					g_observeList = (struct st_observe_info *)CIS_LIST_RM((cis_list_t *)g_observeList, g_observeList->mid, (cis_list_t **)&delnode);
					cis_free(delnode);
				}
				cissys_sleepms(1000);
				g_doRegister = 1;
			}

		}

		/*pump function*/
		pumpRet = cis_pump(g_context);
		if (pumpRet == PUMP_RET_CUSTOM)
		{
			//printf("pump sleep(1000)\n");
		   // cissys_sleepms(1000);
		}
		uint32_t nowtime;
		/*data observe data report*/
		nowtime = cissys_gettime();
		struct st_observe_info* node = g_observeList;
		if (nowtime - g_notifyLast > 60 * 1000) {
			g_notifyLast = nowtime;
			while (node != NULL) {
				if (node->mid == 0)continue;
				if (node->uri.flag == 0)continue;
				cis_uri_t uriLocal;
				uriLocal = node->uri;
				prv_observeNotify(g_context, &uriLocal, node->mid);//订阅资源发送接口
				node = node->next;
			}
		}

	}


	cis_deinit(&g_context);

	struct st_observe_info* delnode;
	while (g_observeList != NULL) {
		g_observeList = (struct st_observe_info *)CIS_LIST_RM((cis_list_t *)g_observeList, g_observeList->mid, (cis_list_t **)&delnode);
		cis_free(delnode);
	}

	cissys_sleepms(2000);


	return 0;
}


#if CIS_ENABLE_CMIOT_OTA
int ota_sample_entry(const uint8_t * config_bin, uint32_t config_size)
{
	cis_version_t ver;
	int index = 0;
	st_context_t *context_ptr;
	cis_callback_t callback;
	callback.onRead = cis_onRead;
	callback.onWrite = cis_onWrite;
	callback.onExec = cis_onExec;
	callback.onObserve = cis_onObserve;
	callback.onSetParams = cis_onParams;
	callback.onEvent = cis_onEvent;
	callback.onDiscover = cis_onDiscover;

	cis_time_t g_lifetime = 100;
	/*init sample data*/
	prv_make_sample_data();

#define SAMPLE_DEFAULT_CONFIG 0
#if SAMPLE_DEFAULT_CONFIG
	if (cis_init_ota(&g_context, NULL, 0, g_cmiot_timeout_duration) != CIS_RET_OK) {
		printf("cis ota entry init failed.\n");
		return -1;
	}
#else
	if (cis_init_ota(&g_context, (void *)config_bin, config_size, g_cmiot_timeout_duration) != CIS_RET_OK) {
		printf("cis ota entry init failed.\n");
		return -1;
	}
#endif
	context_ptr = (st_context_t*)g_context;
	cis_version(&ver);
	printf("CIS OTA SDK Version:%u.%u\n", ver.major, ver.minor);

	HANDLE hThread;
	DWORD dwThread;
	hThread = CreateThread(NULL, 0, taskThread, (LPVOID)NULL, 0, &dwThread);
	CloseHandle(hThread);
	cis_register(g_context, g_lifetime, &callback);
	while (1)
	{
		uint32_t pumpRet;

		/*pump function*/
		pumpRet = cis_pump(g_context);
		uint32_t nowtime;
		/*data observe data report*/
		nowtime = cissys_gettime();
		struct st_observe_info* node = g_observeList;
		if (nowtime - g_notifyLast > 30 * 1000)
		{
			g_notifyLast = nowtime;
			while (node != NULL)
			{
				if (node->mid == 0)continue;
				if (node->uri.flag == 0)continue;
				cis_uri_t uriLocal;
				uriLocal = node->uri;
				prv_observeNotify(g_context, &uriLocal, node->mid);
				node = node->next;
			}
		}

		if (g_cmiot_otafinishstate_flag == OTA_HISTORY_STATE_FINISHED)
		{
			return 0;
		}
		/*ota timeout detection*/

		if (nowtime - context_ptr->ota_timeout_base > context_ptr->ota_timeout_duration)
		{
			//OTA timeout : Terminal has not received otastart command in specific duration
			cis_unregister(g_context);
			printf("OTA Timeout detected. Unresgister from OneNET \r\n");
			cis_deinit(&g_context);
			struct st_observe_info* delnode;
			while (g_observeList != NULL)
			{
				g_observeList = (struct st_observe_info *)CIS_LIST_RM((cis_list_t *)g_observeList, g_observeList->mid, (cis_list_t **)&delnode);
				cis_free(delnode);
			}

			cissys_sleepms(2000);
			return 0;
		}

	}
}
#endif
//////////////////////////////////////////////////////////////////////////
//private funcation;
static void prv_observeNotify(void* context, cis_uri_t* uri, cis_mid_t mid)
{
	uint8_t index;
	st_sample_object* object = NULL;
	cis_data_t value;
	for (index = 0; index < SAMPLE_OBJECT_MAX; index++)
	{
		if (g_objectList[index].oid == uri->objectId) {
			object = &g_objectList[index];
		}
	}

	if (object == NULL) {
		return;
	}


	if (!CIS_URI_IS_SET_INSTANCE(uri) && !CIS_URI_IS_SET_RESOURCE(uri)) // one object
	{
		switch (uri->objectId)
		{
		case SAMPLE_OID_A:
		{
			for (index = 0; index < SAMPLE_A_INSTANCE_COUNT; index++)
			{
				st_instance_a *inst = &g_instList_a[index];
				if (inst != NULL && inst->enabled == true)
				{
					cis_data_t tmpdata[3];

					tmpdata[0].type = cis_data_type_integer;
					tmpdata[0].value.asInteger = inst->instance.intValue;
					uri->instanceId = index;
					uri->resourceId = attributeA_intValue;
					cis_uri_update(uri);
					cis_notify(context, uri, &tmpdata[0], mid, CIS_NOTIFY_CONTINUE, false);

					tmpdata[1].type = cis_data_type_float;
					tmpdata[1].value.asFloat = inst->instance.floatValue;
					uri->instanceId = index;
					uri->resourceId = attributeA_floatValue;
					cis_uri_update(uri);
					cis_notify(context, uri, &tmpdata[1], mid, CIS_NOTIFY_CONTINUE, false);

					tmpdata[2].type = cis_data_type_string;
					tmpdata[2].asBuffer.length = strlen(inst->instance.strValue);
					tmpdata[2].asBuffer.buffer = (uint8_t*)(inst->instance.strValue);
					uri->instanceId = index;
					uri->resourceId = attributeA_stringValue;
					cis_uri_update(uri);
					cis_notify(context, uri, &tmpdata[2], mid, CIS_NOTIFY_CONTENT, false);
				}
			}
		}	
			break;
		case SAMPLE_OID_B:
		{
			for (index = 0; index < SAMPLE_B_INSTANCE_COUNT; index++)
			{
				st_instance_b *inst = &g_instList_b[index];
				if (inst != NULL && inst->enabled == true)
				{
					cis_data_t tmpdata[4];

					tmpdata[0].type = cis_data_type_integer;
					tmpdata[0].value.asInteger = inst->instance.intValue;
					uri->instanceId = index;
					uri->resourceId = attributeB_intValue;
					cis_uri_update(uri);
					cis_notify(context, uri, &tmpdata[0], mid, CIS_NOTIFY_CONTINUE, false);

					tmpdata[1].type = cis_data_type_float;
					tmpdata[1].value.asFloat = inst->instance.floatValue;
					uri->instanceId = index;
					uri->resourceId = attributeB_floatValue;
					cis_uri_update(uri);
					cis_notify(context, uri, &tmpdata[1], mid, CIS_NOTIFY_CONTINUE, false);

					tmpdata[2].type = cis_data_type_string;
					tmpdata[2].asBuffer.length = strlen(inst->instance.strValue);
					tmpdata[2].asBuffer.buffer = (uint8_t*)(inst->instance.strValue);

					uri->instanceId = index;
					uri->resourceId = attributeB_stringValue;
					cis_uri_update(uri);
					cis_notify(context, uri, &tmpdata[2], mid, CIS_NOTIFY_CONTENT, false);
				}
			}
		}
		break;
	  }
	}
	else if (CIS_URI_IS_SET_INSTANCE(uri))
	{
		switch (object->oid)
		{
		case SAMPLE_OID_A:
		{
			if (uri->instanceId > SAMPLE_A_INSTANCE_COUNT) {
				return;
			}
			st_instance_a *inst = &g_instList_a[uri->instanceId];
			if (inst == NULL || inst->enabled == false) {
				return;
			}

			if (CIS_URI_IS_SET_RESOURCE(uri)) {
				if (uri->resourceId == attributeA_intValue)
				{
					value.type = cis_data_type_integer;
					value.value.asInteger = inst->instance.intValue;
				}
				else if (uri->resourceId == attributeA_floatValue)
				{
					value.type = cis_data_type_float;
					value.value.asFloat = inst->instance.floatValue;
				}
				else if (uri->resourceId == attributeA_stringValue)
				{
					value.type = cis_data_type_string;
					value.asBuffer.length = strlen(inst->instance.strValue);
					value.asBuffer.buffer = (uint8_t*)(inst->instance.strValue);
				}
				else {
					return;
				}

				cis_notify(context, uri, &value, mid, CIS_NOTIFY_CONTENT, false);

			}
			else {
				cis_data_t tmpdata[3];

				tmpdata[0].type = cis_data_type_integer;
				tmpdata[0].value.asInteger = inst->instance.intValue;
				uri->resourceId = attributeA_intValue;
				cis_uri_update(uri);
				cis_notify(context, uri, &tmpdata[0], mid, CIS_NOTIFY_CONTINUE, false);


				tmpdata[1].type = cis_data_type_float;
				tmpdata[1].value.asFloat = inst->instance.floatValue;
				uri->resourceId = attributeA_floatValue;
				cis_uri_update(uri);
				cis_notify(context, uri, &tmpdata[1], mid, CIS_NOTIFY_CONTINUE, false);

				tmpdata[2].type = cis_data_type_string;
				tmpdata[2].asBuffer.length = strlen(inst->instance.strValue);
				tmpdata[2].asBuffer.buffer = (uint8_t*)(inst->instance.strValue);
				uri->resourceId = attributeA_stringValue;
				cis_uri_update(uri);
				cis_notify(context, uri, &tmpdata[2], mid, CIS_NOTIFY_CONTENT, false);
			}
		}	
			break;
		case SAMPLE_OID_B:
		{
			if (uri->instanceId > SAMPLE_B_INSTANCE_COUNT) {
				return;
			}
			st_instance_b *inst = &g_instList_b[uri->instanceId];
			if (inst == NULL || inst->enabled == false) {
				return;
			}

			if (CIS_URI_IS_SET_RESOURCE(uri)) {
				if (uri->resourceId == attributeB_intValue)
				{
					value.type = cis_data_type_integer;
					value.value.asInteger = inst->instance.intValue;
				}
				else if (uri->resourceId == attributeB_floatValue)
				{
					value.type = cis_data_type_float;
					value.value.asFloat = inst->instance.floatValue;
				}
				else if (uri->resourceId == attributeB_stringValue)
				{
					value.type = cis_data_type_string;
					value.asBuffer.length = strlen(inst->instance.strValue);
					value.asBuffer.buffer = (uint8_t*)(inst->instance.strValue);
				}
				else {
					return;
				}

				cis_notify(context, uri, &value, mid, CIS_NOTIFY_CONTENT, false);

			}
			else {
				cis_data_t tmpdata[3];

				tmpdata[0].type = cis_data_type_integer;
				tmpdata[0].value.asInteger = inst->instance.intValue;
				uri->resourceId = attributeB_intValue;
				cis_uri_update(uri);
				cis_notify(context, uri, &tmpdata[0], mid, CIS_NOTIFY_CONTINUE, false);


				tmpdata[1].type = cis_data_type_float;
				tmpdata[1].value.asFloat = inst->instance.floatValue;
				uri->resourceId = attributeB_floatValue;
				cis_uri_update(uri);
				cis_notify(context, uri, &tmpdata[1], mid, CIS_NOTIFY_CONTINUE, false);

				tmpdata[2].type = cis_data_type_string;
				tmpdata[2].asBuffer.length = strlen(inst->instance.strValue);
				tmpdata[2].asBuffer.buffer = (uint8_t*)(inst->instance.strValue);
				uri->resourceId = attributeB_stringValue;
				cis_uri_update(uri);
				cis_notify(context, uri, &tmpdata[2], mid, CIS_NOTIFY_CONTENT, false);
			}
		}
		break;
		}
	}
}



static void prv_readResponse(void* context, cis_uri_t* uri, cis_mid_t mid)
{
	uint8_t index;
	st_sample_object* object = NULL;
	cis_data_t value;
	for (index = 0; index < SAMPLE_OBJECT_MAX; index++)
	{
		if (g_objectList[index].oid == uri->objectId) {
			object = &g_objectList[index];
		}
	}

	if (object == NULL) {
		return;
	}


	if (!CIS_URI_IS_SET_INSTANCE(uri) && !CIS_URI_IS_SET_RESOURCE(uri)) // one object
	{
		switch (uri->objectId)
		{
		case SAMPLE_OID_A:
		{
			for (index = 0; index < SAMPLE_A_INSTANCE_COUNT; index++)
			{
				st_instance_a *inst = &g_instList_a[index];
				if (inst != NULL && inst->enabled == true)
				{
					cis_data_t tmpdata[3];

					tmpdata[0].type = cis_data_type_integer;
					tmpdata[0].value.asInteger = inst->instance.intValue;
					uri->instanceId = inst->instId;
					uri->resourceId = attributeA_intValue;
					cis_uri_update(uri);
					cis_response(context, uri, &tmpdata[0], mid, CIS_RESPONSE_CONTINUE);

					tmpdata[1].type = cis_data_type_float;
					tmpdata[1].value.asFloat = inst->instance.floatValue;
					uri->resourceId = attributeA_floatValue;
					uri->instanceId = inst->instId;
					cis_uri_update(uri);
					cis_response(context, uri, &tmpdata[1], mid, CIS_RESPONSE_CONTINUE);

					tmpdata[2].type = cis_data_type_string;
					tmpdata[2].asBuffer.length = strlen(inst->instance.strValue);
					tmpdata[2].asBuffer.buffer = (uint8_t*)(inst->instance.strValue);
					uri->resourceId = attributeA_stringValue;
					uri->instanceId = inst->instId;
					cis_uri_update(uri);
					cis_response(context, uri, &tmpdata[2], mid, CIS_RESPONSE_CONTINUE);
				}
			}
		}	
		  break;
		case SAMPLE_OID_B:
		{
			for (index = 0; index < SAMPLE_B_INSTANCE_COUNT; index++)
			{
				st_instance_b *inst = &g_instList_b[index];
				if (inst != NULL && inst->enabled == true)
				{
					cis_data_t tmpdata[3];

					tmpdata[0].type = cis_data_type_integer;
					tmpdata[0].value.asInteger = inst->instance.intValue;
					uri->instanceId = inst->instId;
					uri->resourceId = attributeB_intValue;
					cis_uri_update(uri);
					cis_response(context, uri, &tmpdata[0], mid, CIS_RESPONSE_CONTINUE);

					tmpdata[1].type = cis_data_type_float;
					tmpdata[1].value.asFloat = inst->instance.floatValue;
					uri->resourceId = attributeB_floatValue;
					uri->instanceId = inst->instId;
					cis_uri_update(uri);
					cis_response(context, uri, &tmpdata[1], mid, CIS_RESPONSE_CONTINUE);

					tmpdata[2].type = cis_data_type_string;
					tmpdata[2].asBuffer.length = strlen(inst->instance.strValue);
					tmpdata[2].asBuffer.buffer = (uint8_t*)(inst->instance.strValue);
					uri->resourceId = attributeB_stringValue;
					uri->instanceId = inst->instId;
					cis_uri_update(uri);
					cis_response(context, uri, &tmpdata[2], mid, CIS_RESPONSE_CONTINUE);
				}
			}
		}
		break;
		}
		cis_response(context, NULL, NULL, mid, CIS_RESPONSE_READ);

	}
	else
	{
		switch (object->oid)
		{
		case SAMPLE_OID_A:
		{
			if (uri->instanceId > SAMPLE_A_INSTANCE_COUNT) {
				return;
			}
			st_instance_a *inst = &g_instList_a[uri->instanceId];
			if (inst == NULL || inst->enabled == false) {
				return;
			}

			if (CIS_URI_IS_SET_RESOURCE(uri)) {
				if (uri->resourceId == attributeA_intValue)
				{
					value.type = cis_data_type_integer;
					value.value.asInteger = inst->instance.intValue;
				}
				else if (uri->resourceId == attributeA_floatValue)
				{
					value.type = cis_data_type_float;
					value.value.asFloat = inst->instance.floatValue;
				}
				else if (uri->resourceId == attributeA_stringValue)
				{
					value.type = cis_data_type_string;
					value.asBuffer.length = strlen(inst->instance.strValue);
					value.asBuffer.buffer = (uint8_t*)(inst->instance.strValue);
				}
				else {
					return;
				}

				cis_response(context, uri, &value, mid, CIS_RESPONSE_READ);

			}
			else {
				cis_data_t tmpdata[3];

				tmpdata[0].type = cis_data_type_integer;
				tmpdata[0].value.asInteger = inst->instance.intValue;
				uri->resourceId = attributeA_intValue;
				cis_uri_update(uri);
				cis_response(context, uri, &tmpdata[0], mid, CIS_RESPONSE_CONTINUE);


				tmpdata[1].type = cis_data_type_float;
				tmpdata[1].value.asFloat = inst->instance.floatValue;
				uri->resourceId = attributeA_floatValue;
				cis_uri_update(uri);
				cis_response(context, uri, &tmpdata[1], mid, CIS_RESPONSE_CONTINUE);

				tmpdata[2].type = cis_data_type_string;
				tmpdata[2].asBuffer.length = strlen(inst->instance.strValue);
				tmpdata[2].asBuffer.buffer = (uint8_t*)(inst->instance.strValue);
				uri->resourceId = attributeA_stringValue;
				cis_uri_update(uri);
				cis_response(context, uri, &tmpdata[2], mid, CIS_RESPONSE_READ);
			}
		}	
		  break;
		case SAMPLE_OID_B:
		{
			if (uri->instanceId > SAMPLE_B_INSTANCE_COUNT) {
				return;
			}
			st_instance_b *inst = &g_instList_b[uri->instanceId];
			if (inst == NULL || inst->enabled == false) {
				return;
			}

			if (CIS_URI_IS_SET_RESOURCE(uri)) {
				if (uri->resourceId == attributeB_intValue)
				{
					value.type = cis_data_type_integer;
					value.value.asInteger = inst->instance.intValue;
				}
				else if (uri->resourceId == attributeB_floatValue)
				{
					value.type = cis_data_type_float;
					value.value.asFloat = inst->instance.floatValue;
				}
				else if (uri->resourceId == attributeB_stringValue)
				{
					value.type = cis_data_type_string;
					value.asBuffer.length = strlen(inst->instance.strValue);
					value.asBuffer.buffer = (uint8_t*)(inst->instance.strValue);
				}
				else {
					return;
				}

				cis_response(context, uri, &value, mid, CIS_RESPONSE_READ);

			}
			else {
				cis_data_t tmpdata[3];

				tmpdata[0].type = cis_data_type_integer;
				tmpdata[0].value.asInteger = inst->instance.intValue;
				uri->resourceId = attributeB_intValue;
				cis_uri_update(uri);
				cis_response(context, uri, &tmpdata[0], mid, CIS_RESPONSE_CONTINUE);


				tmpdata[1].type = cis_data_type_float;
				tmpdata[1].value.asFloat = inst->instance.floatValue;
				uri->resourceId = attributeB_floatValue;
				cis_uri_update(uri);
				cis_response(context, uri, &tmpdata[1], mid, CIS_RESPONSE_CONTINUE);

				tmpdata[2].type = cis_data_type_string;
				tmpdata[2].asBuffer.length = strlen(inst->instance.strValue);
				tmpdata[2].asBuffer.buffer = (uint8_t*)(inst->instance.strValue);
				uri->resourceId = attributeB_stringValue;
				cis_uri_update(uri);
				cis_response(context, uri, &tmpdata[2], mid, CIS_RESPONSE_READ);
			}
		}
		break;
		}
	}
}


static void prv_discoverResponse(void* context, cis_uri_t* uri, cis_mid_t mid)
{
	uint8_t index;
	st_sample_object* object = NULL;

	for (index = 0; index < SAMPLE_OBJECT_MAX; index++)
	{
		if (g_objectList[index].oid == uri->objectId) {
			object = &g_objectList[index];
		}
	}

	if (object == NULL) {
		return;
	}


	switch (uri->objectId)
	{
	case SAMPLE_OID_A:
	{
		uri->objectId = URI_INVALID;
		uri->instanceId = URI_INVALID;
		uri->resourceId = attributeA_intValue;
		cis_uri_update(uri);
		cis_response(context, uri, NULL, mid, CIS_RESPONSE_CONTINUE);

		uri->objectId = URI_INVALID;
		uri->instanceId = URI_INVALID;
		uri->resourceId = attributeA_floatValue;
		cis_uri_update(uri);
		cis_response(context, uri, NULL, mid, CIS_RESPONSE_CONTINUE);

		uri->objectId = URI_INVALID;
		uri->instanceId = URI_INVALID;
		uri->resourceId = attributeA_stringValue;
		cis_uri_update(uri);
		cis_response(context, uri, NULL, mid, CIS_RESPONSE_CONTINUE);

		uri->objectId = URI_INVALID;
		uri->instanceId = URI_INVALID;
		uri->resourceId = actionA_1;
		cis_uri_update(uri);
		cis_response(context, uri, NULL, mid, CIS_RESPONSE_CONTINUE);
	}	
		break;
	case SAMPLE_OID_B:
	{
		uri->objectId = URI_INVALID;
		uri->instanceId = URI_INVALID;
		uri->resourceId = attributeB_intValue;
		cis_uri_update(uri);
		cis_response(context, uri, NULL, mid, CIS_RESPONSE_CONTINUE);

		uri->objectId = URI_INVALID;
		uri->instanceId = URI_INVALID;
		uri->resourceId = attributeB_floatValue;
		cis_uri_update(uri);
		cis_response(context, uri, NULL, mid, CIS_RESPONSE_CONTINUE);

		uri->objectId = URI_INVALID;
		uri->instanceId = URI_INVALID;
		uri->resourceId = attributeB_stringValue;
		cis_uri_update(uri);
		cis_response(context, uri, NULL, mid, CIS_RESPONSE_CONTINUE);

		uri->objectId = URI_INVALID;
		uri->instanceId = URI_INVALID;
		uri->resourceId = actionB_1;
		cis_uri_update(uri);
		cis_response(context, uri, NULL, mid, CIS_RESPONSE_CONTINUE);
	}
	break;
	}
	cis_response(context, NULL, NULL, mid, CIS_RESPONSE_DISCOVER);
}


static void prv_writeResponse(void* context, cis_uri_t* uri, const cis_data_t* value, cis_attrcount_t count, cis_mid_t mid)
{

	uint8_t index;
	st_sample_object* object = NULL;


	if (!CIS_URI_IS_SET_INSTANCE(uri))
	{
		return;
	}

	for (index = 0; index < SAMPLE_OBJECT_MAX; index++)
	{
		if (g_objectList[index].oid == uri->objectId) {
			object = &g_objectList[index];
		}
	}

	if (object == NULL) {
		return;
	}


	switch (object->oid)
	{
	case SAMPLE_OID_A:
	{
		if (uri->instanceId > SAMPLE_B_INSTANCE_COUNT) {
			return;
		}
		st_instance_a *inst = &g_instList_a[uri->instanceId];
		if (inst == NULL || inst->enabled == false) {
			return;
		}


		for (int i = 0; i < count; i++)
		{
			printf("write %d/%d/%d\n", uri->objectId, uri->instanceId, value[i].id);
			switch (value[i].id)
			{
			case attributeA_intValue:
			{
				inst->instance.intValue = value[i].value.asInteger;
			}
			break;
			case attributeA_floatValue:
			{
				inst->instance.floatValue = value[i].value.asFloat;
			}
			break;
			case  attributeA_stringValue:
			{
				memset(inst->instance.strValue, 0, sizeof(inst->instance.strValue));
				strncpy(inst->instance.strValue, (char*)value[i].asBuffer.buffer, value[i].asBuffer.length);
			}
			break;
			}
		}


	}	
		break;
	case SAMPLE_OID_B:
	{
		if (uri->instanceId > SAMPLE_B_INSTANCE_COUNT) {
			return;
		}
		st_instance_b *inst = &g_instList_b[uri->instanceId];
		if (inst == NULL || inst->enabled == false) {
			return;
		}


		for (int i = 0; i < count; i++)
		{
			printf("write %d/%d/%d\n", uri->objectId, uri->instanceId, value[i].id);
			switch (value[i].id)
			{
			case attributeB_intValue:
			{
				inst->instance.intValue = value[i].value.asInteger;
			}
			break;
			case attributeB_floatValue:
			{
				inst->instance.floatValue = value[i].value.asFloat;
			}
			break;
			case  attributeB_stringValue:
			{
				memset(inst->instance.strValue, 0, sizeof(inst->instance.strValue));
				strncpy(inst->instance.strValue, (char*)value[i].asBuffer.buffer, value[i].asBuffer.length);
			}
			break;
			}
		}


	}
	break;
	}

	cis_response(context, NULL, NULL, mid, CIS_RESPONSE_WRITE);
}


static void prv_execResponse(void* context, cis_uri_t* uri, const uint8_t* value, uint32_t length, cis_mid_t mid)
{

	uint8_t index;
	st_sample_object* object = NULL;


	for (index = 0; index < SAMPLE_OBJECT_MAX; index++)
	{
		if (g_objectList[index].oid == uri->objectId) {
			object = &g_objectList[index];
		}
	}

	if (object == NULL) {
		return;
	}


	switch (object->oid)
	{
	case SAMPLE_OID_A:
	{
		if (uri->instanceId > SAMPLE_B_INSTANCE_COUNT) {
			return;
		}
		st_instance_a *inst = &g_instList_a[uri->instanceId];
		if (inst == NULL || inst->enabled == false) {
			return;
		}

		if (uri->resourceId == actionA_1)
		{
			/*
			*\call action;
			*/
			printf("exec actionA_1\n");
			cis_response(context, NULL, NULL, mid, CIS_RESPONSE_EXECUTE);
		}
		else {
			return;
		}
	}	
		break;
	case SAMPLE_OID_B:
	{
		if (uri->instanceId > SAMPLE_B_INSTANCE_COUNT) {
			return;
		}
		st_instance_b *inst = &g_instList_b[uri->instanceId];
		if (inst == NULL || inst->enabled == false) {
			return;
		}

		if (uri->resourceId == actionB_1)
		{
			/*
			*\call action;
			*/
			printf("exec actionB_1\n");
			cis_response(context, NULL, NULL, mid, CIS_RESPONSE_EXECUTE);
		}
		else {
			return;
		}
	}
	break;
	}
}






static void prv_paramsResponse(void* context, cis_uri_t* uri, cis_observe_attr_t parameters, cis_mid_t mid)
{
	uint8_t index;
	st_sample_object* object = NULL;


	if (CIS_URI_IS_SET_RESOURCE(uri)) {
		printf("prv_params:(%d/%d/%d)\n", uri->objectId, uri->instanceId, uri->resourceId);
	}

	if (!CIS_URI_IS_SET_INSTANCE(uri))
	{
		return;
	}

	for (index = 0; index < SAMPLE_OBJECT_MAX; index++)
	{
		if (g_objectList[index].oid == uri->objectId) {
			object = &g_objectList[index];
		}
	}

	if (object == NULL) {
		return;
	}

	/*set parameter to observe resource*/
	/*do*/

	printf("set:%x,clr:%x\n", parameters.toSet, parameters.toClear);
	printf("min:%d,max:%d,gt:%f,lt:%f,st:%f\n", parameters.minPeriod, parameters.maxPeriod, parameters.greaterThan, parameters.lessThan, parameters.step);

	cis_response(context, NULL, NULL, mid, CIS_RESPONSE_OBSERVE_PARAMS);

}



static cis_data_t* sample_dataDup(const cis_data_t* value, cis_attrcount_t attrcount)
{
	cis_data_t* newData;
	newData = (cis_data_t*)cis_malloc(attrcount * sizeof(cis_data_t));
	if (newData == NULL)
	{
		return NULL;
	}
	cis_attrcount_t index;
	for (index = 0; index < attrcount; index++)
	{
		newData[index].id = value[index].id;
		newData[index].type = value[index].type;
		newData[index].asBuffer.length = value[index].asBuffer.length;
		newData[index].asBuffer.buffer = (uint8_t*)cis_malloc(value[index].asBuffer.length);
		cis_memcpy(newData[index].asBuffer.buffer, value[index].asBuffer.buffer, value[index].asBuffer.length);


		cis_memcpy(&newData[index].value.asInteger, &value[index].value.asInteger, sizeof(newData[index].value));
	}
	return newData;
}


//////////////////////////////////////////////////////////////////////////
static cis_coapret_t cis_onRead(void* context, cis_uri_t* uri, cis_mid_t mid)
{
	struct st_callback_info* newNode = (struct st_callback_info*)cis_malloc(sizeof(struct st_callback_info));
	newNode->next = NULL;
	newNode->flag = SAMPLE_CALLBACK_READ;
	newNode->mid = mid;
	newNode->uri = *uri;
	g_callbackList = (struct st_callback_info*)CIS_LIST_ADD(g_callbackList, newNode);

	printf("cis_onRead:(%d/%d/%d)\n", uri->objectId, uri->instanceId, uri->resourceId);


	return CIS_CALLBACK_CONFORM;
}

static cis_coapret_t cis_onDiscover(void* context, cis_uri_t* uri, cis_mid_t mid)
{

	struct st_callback_info* newNode = (struct st_callback_info*)cis_malloc(sizeof(struct st_callback_info));
	newNode->next = NULL;
	newNode->flag = SAMPLE_CALLBACK_DISCOVER;
	newNode->mid = mid;
	newNode->uri = *uri;
	g_callbackList = (struct st_callback_info*)CIS_LIST_ADD(g_callbackList, newNode);

	printf("cis_onDiscover:(%d/%d/%d)\n", uri->objectId, uri->instanceId, uri->resourceId);

	return CIS_CALLBACK_CONFORM;
}

static cis_coapret_t cis_onWrite(void* context, cis_uri_t* uri, const cis_data_t* value, cis_attrcount_t attrcount, cis_mid_t mid)
{

	st_sample_object* object = NULL;

	if (CIS_URI_IS_SET_RESOURCE(uri)) {
		printf("cis_onWrite:(%d/%d/%d)\n", uri->objectId, uri->instanceId, uri->resourceId);
	}
	else {
		printf("cis_onWrite:(%d/%d)\n", uri->objectId, uri->instanceId);
	}
#if  CIS_ENABLE_UPDATE_MCU
	st_context_t *contextP = (st_context_t*)context;
	if (5 == uri->objectId)
	{
		cissys_sleepms(2000);
		cis_notify_sota_result(contextP,sota_write_success);//模拟MCU写成功或者失败
		return CIS_CALLBACK_CONFORM;
	}
#endif


	struct st_callback_info* newNode = (struct st_callback_info*)cis_malloc(sizeof(struct st_callback_info));
	newNode->next = NULL;
	newNode->flag = SAMPLE_CALLBACK_WRITE;
	newNode->mid = mid;
	newNode->uri = *uri;
	newNode->param.asWrite.count = attrcount;
	newNode->param.asWrite.value = sample_dataDup(value, attrcount);
	g_callbackList = (struct st_callback_info*)CIS_LIST_ADD(g_callbackList, newNode);


	return CIS_CALLBACK_CONFORM;

}


static cis_coapret_t cis_onExec(void* context, cis_uri_t* uri, const uint8_t* value, uint32_t length, cis_mid_t mid)
{
	if (CIS_URI_IS_SET_RESOURCE(uri)) {
		printf("cis_onExec:(%d/%d/%d)\n", uri->objectId, uri->instanceId, uri->resourceId);
	}
	else {
		return CIS_CALLBACK_METHOD_NOT_ALLOWED;
	}

	if (!CIS_URI_IS_SET_INSTANCE(uri))
	{
		return CIS_CALLBACK_BAD_REQUEST;
	}

	struct st_callback_info* newNode = (struct st_callback_info*)cis_malloc(sizeof(struct st_callback_info));
	newNode->next = NULL;
	newNode->flag = SAMPLE_CALLBACK_EXECUTE;
	newNode->mid = mid;
	newNode->uri = *uri;
	newNode->param.asExec.buffer = (uint8_t*)cis_malloc(length);
	newNode->param.asExec.length = length;
	cis_memcpy(newNode->param.asExec.buffer, value, length);
	g_callbackList = (struct st_callback_info*)CIS_LIST_ADD(g_callbackList, newNode);


	return CIS_CALLBACK_CONFORM;
}


static cis_coapret_t cis_onObserve(void* context, cis_uri_t* uri, bool flag, cis_mid_t mid)
{
	st_sample_object* object = NULL;
	printf("cis_onObserve mid:%d uri:(%d/%d/%d)\n",
		mid,
		uri->objectId,
		CIS_URI_IS_SET_INSTANCE(uri) ? uri->instanceId : -1,
		CIS_URI_IS_SET_RESOURCE(uri) ? uri->resourceId : -1);

	struct st_callback_info* newNode = (struct st_callback_info*)cis_malloc(sizeof(struct st_callback_info));
	newNode->next = NULL;
	newNode->flag = SAMPLE_CALLBACK_OBSERVE;
	newNode->mid = mid;
	newNode->uri = *uri;
	newNode->param.asObserve.flag = flag;

	g_callbackList = (struct st_callback_info*)CIS_LIST_ADD(g_callbackList, newNode);

	return CIS_CALLBACK_CONFORM;
}


static cis_coapret_t cis_onParams(void* context, cis_uri_t* uri, cis_observe_attr_t parameters, cis_mid_t mid)
{
	if (CIS_URI_IS_SET_RESOURCE(uri)) {
		printf("cis_on_params:(%d/%d/%d)\n", uri->objectId, uri->instanceId, uri->resourceId);
	}

	if (!CIS_URI_IS_SET_INSTANCE(uri))
	{
		return CIS_CALLBACK_BAD_REQUEST;
	}

	struct st_callback_info* newNode = (struct st_callback_info*)cis_malloc(sizeof(struct st_callback_info));
	newNode->next = NULL;
	newNode->flag = SAMPLE_CALLBACK_SETPARAMS;
	newNode->mid = mid;
	newNode->uri = *uri;
	newNode->param.asObserveParam.params = parameters;
	g_callbackList = (struct st_callback_info*)CIS_LIST_ADD(g_callbackList, newNode);

	return CIS_CALLBACK_CONFORM;
}



static void cis_onEvent(void* context, cis_evt_t eid, void* param)
{
	st_context_t* ctx = (st_context_t*)context;
#if CIS_ENABLE_CMIOT_OTA
	cissys_assert(context != NULL);

#endif
	printf("cis_on_event(%d):%s\n", eid, STR_EVENT_CODE(eid));
	switch (eid)
	{
	case CIS_EVENT_RESPONSE_FAILED:
		printf("cis_on_event response failed mid:%d\n", (int32_t)param);
		break;
	case CIS_EVENT_NOTIFY_FAILED:
		printf("cis_on_event notify failed mid:%d\n", (int32_t)param);
		break;
	case CIS_EVENT_UPDATE_NEED:
		printf("cis_on_event need to update,reserve time:%ds\n", (int32_t)param);
		cis_update_reg(g_context, LIFETIME_INVALID, false);
		break;
	case CIS_EVENT_REG_SUCCESS:
	{
		struct st_observe_info* delnode;
		while (g_observeList != NULL) {
			g_observeList = (st_observe_info *)CIS_LIST_RM((cis_list_t *)g_observeList, g_observeList->mid, (cis_list_t **)&delnode);
			cis_free(delnode);
		}
	}
	break;
#if CIS_ENABLE_UPDATE
	case CIS_EVENT_FIRMWARE_DOWNLOADING:
		printf("FIRMWARE_DOWNLOADING\r\n");
		break;
	case CIS_EVENT_FIRMWARE_DOWNLOAD_FAILED:
		printf("FIRMWARE_DOWNFAILED\r\n");
		break;
	case CIS_EVENT_FIRMWARE_DOWNLOADED:
		printf("FIRMWARE_DOWNLOADED:\r\n");
		break;
	case CIS_EVENT_FIRMWARE_UPDATING:
		printf("FIRMWARE_UPDATING:\r\n");
		break;
	case CIS_EVENT_FIRMWARE_TRIGGER:
		printf("please update the firmware\r\n");
		break;
#if CIS_ENABLE_UPDATE_MCU
	case CIS_EVENT_SOTA_DOWNLOADING:
		printf("SOTA_DOWNLOADING:\r\n");
		break;
	case CIS_EVENT_SOTA_DOWNLOAED:
		printf("SOTA_DOWNLOAED:\r\n");
		break;
	case CIS_EVENT_SOTA_FLASHERASE:
		printf("SOTA_FLASHERASE:\r\n");
		cissys_sleepms(2000);
		cis_notify_sota_result(ctx,sota_erase_success);
		break;
	case CIS_EVENT_SOTA_UPDATING:
		//cis_memcpy(VERSION, "1801107150", strlen("1801107150") + 1);//模拟MCU 升级成功
		cis_set_sota_info( "1801102",500);
		printf("SOTA_UPDATING:\r\n");
		break;
#endif			

#endif
#if CIS_ENABLE_CMIOT_OTA
	case CIS_EVENT_CMIOT_OTA_START:
		printf("cis_on_event CMIOT OTA start:%d\n", (int32_t)param);
		if (ctx->cmiotOtaState == CMIOT_OTA_STATE_IDIL)
		{

			ctx->cmiotOtaState = CMIOT_OTA_STATE_START;
			ctx->ota_timeout_base = cissys_gettime();
			ctx->ota_timeout_duration = CIS_CMIOT_OTA_START_TIME_OUT_MSEC;
			g_cmiot_ota_at_return_code = OTA_FINISH_AT_RETURN_TIMEOUT_SECOND;
		}
		break;
	case CIS_EVENT_CMIOT_OTA_SUCCESS:
		if (ctx->cmiotOtaState == CMIOT_OTA_STATE_START)
		{
			printf("cis_on_event CMIOT OTA success %d\n", (int32_t)param);
			ctx->cmiotOtaState = CMIOT_OTA_STATE_IDIL;
			cissys_recover_psm();
		}
		break;
	case CIS_EVENT_CMIOT_OTA_FAIL:
		if (ctx->cmiotOtaState == CMIOT_OTA_STATE_START)
		{
			ctx->cmiotOtaState = CMIOT_OTA_STATE_IDIL;
			printf("cis_on_event CMIOT OTA fail %d\n", (int32_t)param);
			cissys_recover_psm();
		}
		break;
	case CIS_EVENT_CMIOT_OTA_FINISH:
		if (ctx->cmiotOtaState == CMIOT_OTA_STATE_START || ctx->cmiotOtaState == CMIOT_OTA_STATE_IDIL)
		{
			ctx->cmiotOtaState = CMIOT_OTA_STATE_FINISH;
			printf("cis_on_event CMIOT OTA finish %d\n", *((int8_t *)param));

			cissys_recover_psm();
			switch (*((int8_t *)param))
			{
			case OTA_FINISH_COMMAND_SUCCESS_CODE:
				g_cmiot_otafinishstate_flag = OTA_HISTORY_STATE_FINISHED; //indicate ota procedure has finished
				g_cmiot_ota_at_return_code = OTA_FINISH_AT_RETURN_SUCCESS;
				printf("OTA finish Success!\n");
				break;
			case OTA_FINISH_COMMAND_UNREGISTER_CODE:
				g_cmiot_ota_at_return_code = OTA_FINISH_AT_RETURN_NO_REQUEST;
				printf("OTA finish fail: no OTA register on platform!\n");//indicates there is no ota request on the CMIOT platform
				break;
			case OTA_FINISH_COMMAND_FAIL_CODE:
				g_cmiot_ota_at_return_code = OTA_FINISH_AT_RETURN_FAIL;
				printf("OTA finish fail: target NUM error!\n");//indicates the IMSI is not changed or the new IMSI is illegal
				break;
			default://unkown error from the platform
				printf("OTA finish fail: unknow error!\n");
			}
		}
		break;

#endif
	default:
		break;
	}

}
