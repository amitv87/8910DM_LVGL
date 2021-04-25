#include <cis_def.h>
#if CIS_ENABLE_DM
#include <stdio.h>
#include <stdlib.h>
#include <cis_api.h>
#include <cis_if_sys.h>
#include <cis_log.h>
#include <cis_list.h>
#include <cis_api.h>
#include "cis_sample_defs.h"
#include <string.h>
#include <windows.h>

static void     prv_dm_observeNotify(void* context, cis_uri_t* uri, cis_mid_t mid);

static void     prv_dm_readResponse(void* context, cis_uri_t* uri, cis_mid_t mid);
static void     prv_dm_discoverResponse(void* context, cis_uri_t* uri, cis_mid_t mid);
static void     prv_dm_writeResponse(void* context, cis_uri_t* uri, const cis_data_t* value, cis_attrcount_t count, cis_mid_t mid);
static void     prv_dm_execResponse(void* context, cis_uri_t* uri, const uint8_t* value, uint32_t length, cis_mid_t mid);
static void     prv_dm_paramsResponse(void* context, cis_uri_t* uri, cis_observe_attr_t parameters, cis_mid_t mid);

static cis_coapret_t dm_onRead(void* context, cis_uri_t* uri, cis_mid_t mid);
static cis_coapret_t dm_onWrite(void* context, cis_uri_t* uri, const cis_data_t* value, cis_attrcount_t attrcount, cis_mid_t mid);
static cis_coapret_t dm_onExec(void* context, cis_uri_t* uri, const uint8_t* value, uint32_t length, cis_mid_t mid);
static cis_coapret_t dm_onObserve(void* context, cis_uri_t* uri, bool flag, cis_mid_t mid);
static cis_coapret_t dm_onParams(void* context, cis_uri_t* uri, cis_observe_attr_t parameters, cis_mid_t mid);
static cis_coapret_t dm_onDiscover(void* context, cis_uri_t* uri, cis_mid_t mid);
static void        dm_onEvent(void* context, cis_evt_t eid, void* param);

static struct st_callback_info* g_dmcallbackList = NULL;
static struct st_observe_info*  g_dmobserveList = NULL;

static void*      g_dmcontext = NULL;
static bool       g_dmshutdown = false;
static bool       g_doUnregisterdm = false;
static bool       g_doRegisterdm = false;

static st_sample_object     g_objectListdm[SAMPLE_OBJECT_MAX];
static st_instance_a        g_dminstList_a[SAMPLE_A_INSTANCE_COUNT];
static st_instance_b        g_dminstList_b[SAMPLE_B_INSTANCE_COUNT];



DWORD WINAPI dmTaskThread(LPVOID lpParam)
{
	while (!g_dmshutdown)
	{
		struct st_callback_info* node;
		if (g_dmcallbackList == NULL) {
			cissys_sleepms(100);
			continue;
		}
		node = g_dmcallbackList;
		g_dmcallbackList = g_dmcallbackList->next;

		switch (node->flag)
		{
		case 0:
			break;
		case SAMPLE_CALLBACK_READ:
		{
			cis_uri_t uriLocal;
			uriLocal = node->uri;
			prv_dm_readResponse(g_dmcontext, &uriLocal, node->mid);
		}
		break;
		case SAMPLE_CALLBACK_DISCOVER:
		{
			cis_uri_t uriLocal;
			uriLocal = node->uri;
			prv_dm_discoverResponse(g_dmcontext, &uriLocal, node->mid);
		}
		break;
		case SAMPLE_CALLBACK_WRITE:
		{
			//write
			prv_dm_writeResponse(g_dmcontext, &node->uri, node->param.asWrite.value, node->param.asWrite.count, node->mid);
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
			prv_dm_execResponse(g_dmcontext, &node->uri, node->param.asExec.buffer, node->param.asExec.length, node->mid);
			cis_free(node->param.asExec.buffer);
		}
		break;
		case SAMPLE_CALLBACK_SETPARAMS:
		{
			//set parameters and notify
			prv_dm_paramsResponse(g_dmcontext, &node->uri, node->param.asObserveParam.params, node->mid);
		}
		break;
		case SAMPLE_CALLBACK_OBSERVE:
		{
			if (node->param.asObserve.flag) 
			{
				uint16_t count = 0;
				struct st_observe_info* observe_new = (struct st_observe_info*)cis_malloc(sizeof(struct st_observe_info));
				cissys_assert(observe_new != NULL);
				observe_new->mid = node->mid;
				observe_new->uri = node->uri;
				observe_new->next = NULL;

				g_dmobserveList = (struct st_observe_info*)cis_list_add((cis_list_t*)g_dmobserveList, (cis_list_t*)observe_new);
				/*
				LOGD("cis_on_observe set(%d): %d/%d/%d",
				count,
				uri->objectId,
				CIS_URI_IS_SET_INSTANCE(uri)?uri->instanceId:-1,
				CIS_URI_IS_SET_RESOURCE(uri)?uri->resourceId:-1);
				*/

				cis_response(g_dmcontext, NULL, NULL, node->mid, CIS_RESPONSE_OBSERVE);
			}
			else 
			{
				struct st_observe_info* delnode = NULL;

				g_dmobserveList = (struct st_observe_info *)cis_list_remove((cis_list_t *)g_dmobserveList, node->mid, (cis_list_t **)&delnode);
				if (delnode == NULL)
				{
					cis_free(node);
					continue;
				}

				cis_free(delnode);
				/*
				LOGD("cis_on_observe cancel: %d/%d/%d\n",
				uri->objectId,
				CIS_URI_IS_SET_INSTANCE(uri)?uri->instanceId:-1,
				CIS_URI_IS_SET_RESOURCE(uri)?uri->resourceId:-1);
				*/
				cis_response(g_dmcontext, NULL, NULL, node->mid, CIS_RESPONSE_OBSERVE);
			}
		}
		default:
			break;
		}

		cis_free(node);
	}

	return 1;
}



Options dm_config = { " "," ", " ","appkey","secret",4,"" };

int dm_sample_dm_entry(const uint8_t * config_bin, uint32_t config_size)
{
	char ver[15];
	int index = 0;
	cis_time_t g_lifetime = 0;
	cis_callback_t callback;
	callback.onRead = dm_onRead;
	callback.onWrite = dm_onWrite;
	callback.onExec = dm_onExec;
	callback.onObserve = dm_onObserve;
	callback.onSetParams = dm_onParams;
	callback.onEvent = dm_onEvent;
	callback.onDiscover = dm_onDiscover;

	g_lifetime = 120;
	/*init sample data*/
	if (g_lifetime < 120)
	{
		g_lifetime = 120;
	}
	// prv_make_sample_data();
	printf("lifetime:%d\n", g_lifetime);
	printf("version %s\r\n", cis_get_version(ver));
	cis_memcpy(dm_config.szDMv, ver, strlen(ver) + 1);


	if (cis_init(&g_dmcontext, (void *)config_bin, config_size, &dm_config) != CIS_RET_OK) {
		if (g_dmcontext != NULL)
			cis_deinit(&g_dmcontext);
		printf("cis entry init failed.\n");
		return -1;
	}

	log_config(true, 1, 10, 1024);//For debug 


	g_doUnregisterdm = false;

	//register enabled
	g_doRegisterdm = true;

	HANDLE hThread;
	DWORD dwThread;
	hThread = CreateThread(NULL, 0, dmTaskThread, (LPVOID)NULL, 0, &dwThread);
	CloseHandle(hThread);

	while (1)
	{
		uint32_t pumpRet;

		/*
		*wait press keyboard for register test;
		*do register press 'o'
		*do unregister press 'r'
		**/
		{
			if (g_doRegisterdm)
			{
				g_doRegisterdm = false;
				cis_register(g_dmcontext, g_lifetime, &callback);//将回调函数注册进SDK内
			}
			if (g_doUnregisterdm) {
				g_doUnregisterdm = false;
				cis_unregister(g_dmcontext);
				struct st_observe_info* delnode;
				while (g_dmobserveList != NULL) {
					g_dmobserveList = (struct st_observe_info *)CIS_LIST_RM((cis_list_t *)g_dmobserveList, g_dmobserveList->mid, (cis_list_t **)&delnode);
					cis_free(delnode);
				}
				cissys_sleepms(1000);
				g_doRegisterdm = 1;
			}
		}

		/*pump function*/
		pumpRet = cis_pump(g_dmcontext);
		if (pumpRet == PUMP_RET_CUSTOM)
		{
			//printf("pump sleep(1000)\n");
			cissys_sleepms(1000);
		}
	}

	cis_deinit(&g_dmcontext);

	struct st_observe_info* delnode;
	while (g_dmobserveList != NULL) {
		g_dmobserveList = (struct st_observe_info *)CIS_LIST_RM((cis_list_t *)g_dmobserveList, g_dmobserveList->mid, (cis_list_t **)&delnode);
		cis_free(delnode);
	}
	cissys_sleepms(2000);
	return 0;
}



//private funcation;
static void prv_dm_observeNotify(void* context, cis_uri_t* uri, cis_mid_t mid)
{

}



static void prv_dm_readResponse(void* context, cis_uri_t* uri, cis_mid_t mid)
{

}


static void prv_dm_discoverResponse(void* context, cis_uri_t* uri, cis_mid_t mid)
{

}


static void prv_dm_writeResponse(void* context, cis_uri_t* uri, const cis_data_t* value, cis_attrcount_t count, cis_mid_t mid)
{

}


static void prv_dm_execResponse(void* context, cis_uri_t* uri, const uint8_t* value, uint32_t length, cis_mid_t mid)
{

}

static void prv_dm_paramsResponse(void* context, cis_uri_t* uri, cis_observe_attr_t parameters, cis_mid_t mid)
{

}


static cis_coapret_t dm_onRead(void* context, cis_uri_t* uri, cis_mid_t mid)
{
	return CIS_CALLBACK_CONFORM;
}

static cis_coapret_t dm_onDiscover(void* context, cis_uri_t* uri, cis_mid_t mid)
{
	return CIS_CALLBACK_CONFORM;
}

static cis_coapret_t dm_onWrite(void* context, cis_uri_t* uri, const cis_data_t* value, cis_attrcount_t attrcount, cis_mid_t mid)
{
	return CIS_CALLBACK_CONFORM;
}


static cis_coapret_t dm_onExec(void* context, cis_uri_t* uri, const uint8_t* value, uint32_t length, cis_mid_t mid)
{
	return CIS_CALLBACK_CONFORM;
}


static cis_coapret_t dm_onObserve(void* context, cis_uri_t* uri, bool flag, cis_mid_t mid)
{
	return CIS_CALLBACK_CONFORM;
}

static cis_coapret_t dm_onParams(void* context, cis_uri_t* uri, cis_observe_attr_t parameters, cis_mid_t mid)
{
	return CIS_CALLBACK_CONFORM;
}

#include "cis_api.h"
void dm_onEvent(void* context, cis_evt_t eid, void* param)
{
	printf("\r\ndm_onEvent(%d):%s\n", eid, STR_EVENT_CODE(eid));
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
		cis_update_reg(g_dmcontext, LIFETIME_INVALID, false);
		break;
	case CIS_EVENT_REG_SUCCESS:
		printf("cis_on_event register success\r\n");
		break;
	case CIS_EVENT_UPDATE_SUCCESS:
		printf("cis_on_event update success\r\n");
		break;
	default:
		break;
	}
}
#endif