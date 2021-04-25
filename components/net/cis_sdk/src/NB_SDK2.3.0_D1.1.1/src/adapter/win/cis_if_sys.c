#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "cis_if_sys.h"
#include "cis_def.h"
#include "cis_internals.h"
#include "cis_config.h"
#include "cis_log.h"
#include "cis_list.h"

#include "osi_api.h"
#include "osi_log.h"
#include "netutils.h"
#include "nvm.h"
#include "vfs.h"
#include "fupdate.h"

//#include "win_timers.h"
//#include <process.h>
//#include <windows.h>

#if CIS_ENABLE_CMIOT_OTA
	#include "std_object/std_object.h"
#endif

/*#if CIS_ENABLE_UPDATE
static void testThread(void *lpParam);
#endif*/

#if CIS_ENABLE_CMIOT_OTA
#define CIS_CMIOT_OTA_WAIT_UPLINK_FINISHED    1000
#define CIS_CMIOT_OTA_START_QUERY_INTERVAL    10000
#define CIS_CMIOT_OTA_START_QUERY_LOOP   	  6
#define CIS_CMIOT_NEWIMSI_STR   	"460040983303979"
extern uint8_t    cissys_changeIMSI(char* buffer,uint32_t len);


DWORD WINAPI testCmiotOta(LPVOID lpParam);	
uint8_t std_poweruplog_ota_notify(st_context_t * contextP,cis_oid_t * objectid,uint16_t mid);
uint8_t    cissys_remove_psm(void);
cis_ota_history_state    cissys_quary_ota_finish_state(void);
uint8_t    cissys_change_ota_finish_state(uint8_t flag);
uint8_t    cissys_recover_psm(void);

extern cis_ota_history_state g_cmiot_otafinishstate_flag;
extern void*      g_context;

DWORD WINAPI testCmiotOta(LPVOID lpParam)
{
	cis_oid_t test_obj_id = CIS_POWERUPLOG_OBJECT_ID;
	cis_cmiot_ota_state  test_monitor_ota_state = CMIOT_OTA_STATE_IDIL;
	cissys_assert(g_context!=NULL);
    st_context_t* ctx = (st_context_t*)g_context;	
	poweruplog_data_t * targetP;
	char * NewImsiStr;

	//cissys_callback_t *cb=(cissys_callback_t *)lpParam;
	while(1)
	{	
		//Sleep(5000);
		uint8_t ota_queri_loop;
		bool ota_imsi_change_stat = FALSE;
		test_monitor_ota_state = ctx->cmiotOtaState ;
		uint8_t len_imsi =0;
		switch(test_monitor_ota_state)	
		{
			case CMIOT_OTA_STATE_IDIL:
			{
				Sleep(5000);
			}
			break;

			case CMIOT_OTA_STATE_START:
			{		
				targetP = (poweruplog_data_t *)(std_object_get_poweruplog(ctx, 0)); 				
				if (NULL != targetP)
				{				
					printf("UE is trigged to OTA start state and waiting the OTA procedule finished. \r\n");
						
					printf("OTA started with current IMSI: %s . \r\n",targetP->IMSI);	

					NewImsiStr= (char*)cis_malloc(NBSYS_IMSI_MAXLENGTH);
					if(NewImsiStr == NULL)
					{
						cis_free(NewImsiStr);
					}	
    				cis_memset(NewImsiStr, 0, NBSYS_IMSI_MAXLENGTH);			

					Sleep(CIS_CMIOT_OTA_WAIT_UPLINK_FINISHED);	
					
					cis_unregister(ctx);	

					cissys_remove_psm();
				
					
					Sleep(CIS_CMIOT_OTA_START_QUERY_INTERVAL);	
					
					for(ota_queri_loop=0;ota_queri_loop<CIS_CMIOT_OTA_START_QUERY_LOOP;ota_queri_loop++)
					{
						Sleep(CIS_CMIOT_OTA_START_QUERY_INTERVAL);
						cis_memset(NewImsiStr, 0, NBSYS_IMSI_MAXLENGTH);						
						len_imsi = cissys_getIMSI(NewImsiStr,NBSYS_IMSI_MAXLENGTH);

						if(cis_memcmp(targetP->IMSI,NewImsiStr,len_imsi)!=0)
						{
							ota_imsi_change_stat = TRUE;
							cis_memset(targetP->IMSI, 0, NBSYS_IMSI_MAXLENGTH);
							cis_memcpy(targetP->IMSI,NewImsiStr,len_imsi);					
							break;
						}
						//change the IMSI for test
						if(ota_queri_loop == 3)
						{
							cis_memcpy(NewImsiStr,CIS_CMIOT_NEWIMSI_STR,NBSYS_IMSI_MAXLENGTH);	
							cissys_changeIMSI(NewImsiStr,NBSYS_IMSI_MAXLENGTH);
						}
						
					}
					if(ota_imsi_change_stat == TRUE)
					{
						core_callbackEvent(ctx,CIS_EVENT_CMIOT_OTA_SUCCESS,NULL);
						printf("OTA success detected. \r\n");
						
	
					}
					else
					{
						core_callbackEvent(ctx,CIS_EVENT_CMIOT_OTA_FAIL,NULL);
						printf("OTA fail detected. \r\n");
					}
					printf("OTA finished with current IMSI: %s . \r\n",targetP->IMSI);
					cis_free(NewImsiStr);
				}
				
				ctx->registerEnabled = true;
				printf("Reregister to ONENET to update OTA result. \r\n");	
				

			}
			break;		

			case CMIOT_OTA_STATE_SUCCESS:
			case CMIOT_OTA_STATE_FAIL:
			{
				
			}
			break;

			case CMIOT_OTA_STATE_FINISH:
			{
				cis_unregister(g_context);
				printf("OTA Finish detected.Unresgister from OneNET \r\n");
				cis_deinit(&g_context);
				return 0;
			}
			break;

			default:
				Sleep(5000);				
			
		}

		//std_poweruplog_ota_notify((st_context_t *)g_context,&test_obj_id, cmiot_ota_observe_mid);
	}
}

#endif//CIS_ENABLE_CMIOT_OTA


static uint32_t startTime = 0;
cis_ret_t cissys_init(void *context, const cis_cfg_sys_t* cfg, cissys_callback_t* event_cb)
{
	struct st_cis_context * ctx = (struct st_cis_context *)context;
	cis_memcpy(&(ctx->g_sysconfig), cfg, sizeof(cis_cfg_sys_t));

#if CIS_ENABLE_CMIOT_OTA
	HANDLE hThread;
	HANDLE dwThread;
	//if(TRUE!=ctx->isDM)
	{
		hThread = CreateThread(NULL, 0, testCmiotOta, NULL, 0, &dwThread);
		CloseHandle(hThread);
	}
#endif//CIS_ENABLE_CMIOT_OTA

#if CIS_ENABLE_UPDATE
	if (event_cb != NULL) {
		ctx->g_fotacallback.onEvent = event_cb->onEvent;
		ctx->g_fotacallback.userData = event_cb->userData;
#if CIS_ENABLE_DM
		if (TRUE != ctx->isDM)
#endif
		{
			//hThread = CreateThread(NULL, 0, testThread, (LPVOID)&(ctx->g_fotacallback), 0, &dwThread);
			//CloseHandle(hThread);
			//g_nFotaTestThreadExit = 0;
			//ctx->g_fotacallback.hander = (HANDLE)osiThreadCreate("cissys_recv", testThread, (void *)&(ctx->g_fotacallback), OSI_PRIORITY_NORMAL, 8192 * 4, 32);
		}
	}
#endif
	return 1;
}

uint32_t cissys_gettime()
{
    uint64_t rtime = 0;
    struct timeval t;
    t.tv_sec = t.tv_usec = 0;
    gettimeofday(&t, NULL);
    rtime = (((t.tv_sec) * 1000000LL + t.tv_usec));
    rtime = rtime / 1000;
    rtime -= startTime;
    // OSI_PRINTFI("cissys_gettime:%lu", (uint32_t)rtime);
    return (uint32_t)rtime;

}

void    cissys_sleepms(uint32_t ms)
{
    osiThreadSleep(ms);
}


clock_t cissys_tick(void)
{
	return osiUpTime();
}


void    cissys_logwrite(uint8_t* buffer,uint32_t length)
{
    OSI_PRINTFI("cissys_logwrite:%s", buffer);
}

void*     cissys_malloc(size_t length)
{
    return malloc(length);
}

void    cissys_free(void* buffer)
{
    free(buffer);
}


void* cissys_memset( void* s, int c, size_t n)
{
	return memset( s, c, n);
}

void* cissys_memcpy(void* dst, const void* src, size_t n)
{
	return memcpy( dst, src, n);
}

void* cissys_memmove(void* dst, const void* src, size_t n)
{
	return memmove( dst, src, n);
}

int cissys_memcmp( const void* s1, const void* s2, size_t n)
{
	return memcmp( s1, s2, n);
}

void    cissys_fault(uint16_t id)
{
    //exit(0);
    OSI_PRINTFI("**** cissys_fault ****");
}


void cissys_assert(bool flag)
{
    if(!flag)
        OSI_PRINTFI("**** cissys_assert ****");
}


uint32_t 	cissys_rand()
{
    srand(cissys_gettime());
    union{
        uint8_t varray[4];
        uint32_t vint;
    }val;

    for(int i=0;i<4;i++)
    {
        val.varray[i]=rand()%0xFF;
    }
    
    return val.vint;
}

/*
#if CIS_ENABLE_PSK
uint8_t	cissys_getPSK(char* buffer, uint32_t maxlen)
{
	//const char* str = "imeiwb1";
	char* pBuffer = NULL;
	uint32_t readlen = 0;
	uint8_t len = 0;
	FILE* f = fopen("psk", "r");
	if (f == NULL)
	{
		return 0;
	}

	pBuffer = (char*)cissys_malloc(maxlen + 1);
	readlen = fread(pBuffer, 1, maxlen, f);
	if (readlen <= 0)
	{
		cissys_free(pBuffer);
		fclose(f);
		return 0;
	}
	pBuffer[readlen] = '\0';

	len = strlen(pBuffer);
	if (maxlen < len)return 0;
	cissys_memcpy(buffer, pBuffer, len);
	cissys_free(pBuffer);
	return len;
}
#endif//CIS_ENABLE_PSK
*/
uint8_t	cissys_getIMEI(char* buffer,uint8_t maxlen)
{
    if (maxlen < 16)
        return 0;

    if (netif_default == NULL){
        OSI_LOGI(0, "MIPL netif_default is NULL");
        return 0;
    }
    
    uint8_t sim = (netif_default->sim_cid & 0xf0) >> 4;
    OSI_LOGI(0, "MIPL sim: %d", sim);
    return nvmReadImei(sim, (nvmImei_t *)buffer);
}
uint8_t    cissys_getIMSI(char* buffer,uint8_t maxlen)
{
    if (maxlen < 16)
        return 0;
    if (!getSimImsi(0, (uint8_t *)buffer, &maxlen))
        return 0;
    return maxlen;
}

#if CIS_ENABLE_CMIOT_OTA

uint8_t    cissys_changeIMSI(char* buffer,uint32_t len)
{
    FILE* f = fopen("imsi","wb");
    if(f != NULL)
    {
        fwrite(buffer,1,len,f);
        fclose(f);
        return true;
    }
    return false;
}


uint8_t    cissys_getEID(char* buffer,uint32_t maxlen)
{
    char* pBuffer = NULL;
    uint32_t readlen = 0;
    uint8_t len = 0;
    FILE* f = fopen("eid","r");
    if(f == NULL)
    {
        return 0;
    }

    pBuffer = (char*)malloc(maxlen+1);
    readlen = fread(pBuffer,1,maxlen,f);
    if(readlen <= 0)
    {
        free(pBuffer);
        fclose(f);
        return 0;
    }
    pBuffer[readlen] = '\0';

    len = strlen(pBuffer);
    if(maxlen < len)return 0;
    memcpy(buffer,pBuffer,len);
    free(pBuffer);
    return len;
}	

uint8_t    cissys_remove_psm(void)
{
    return 1;
}	

uint8_t    cissys_recover_psm(void)
{
    return 1;
}	

cis_ota_history_state    cissys_quary_ota_finish_state(void)
{
    return g_cmiot_otafinishstate_flag;
}	
uint8_t    cissys_change_ota_finish_state(cis_ota_history_state flag)
{
	g_cmiot_otafinishstate_flag = flag;
    return 1;
}	
#endif

void        cissys_lockcreate(void** mutex)
{
    (*mutex) = (void *)osiMutexCreate();
}

void        cissys_lockdestory(void* mutex)
{
    osiMutexDelete((osiMutex_t *)(mutex));
}

cis_ret_t   cissys_lock(void* mutex,uint32_t ms)
{
    osiMutexTryLock((osiMutex_t *)mutex, ms);
    return CIS_RET_OK;
}

void        cissys_unlock(void* mutex)
{
    osiMutexUnlock((osiMutex_t *)(mutex));
}

bool        cissys_save(uint8_t* buffer,uint32_t length)
{
#if 0
    FILE* f = NULL;
    f = fopen("d:\\cis_serialize.bin","wb");
    if(f != NULL)
    {
        fwrite(buffer,1,length,f);
        fclose(f);
        return true;
    }
    return false;
#endif
    OSI_PRINTFI("cissys_save length=%ld,buffer=%s",length,buffer);
    return true;
}

bool        cissys_load(uint8_t* buffer,uint32_t length)
{
#if 0
    uint32_t readlen;
    FILE* f = fopen("d:\\cis_serialize.bin","rb");
    if(f != NULL)
    {
        while(length)
        {
            readlen = fread(buffer,1,length,f);
            if(readlen == 0)
            {
                break;
            }
            length -=readlen;
        }
        if(length == 0)
        {
            return true;
        }
    }
    return false;
#endif
    return true;
}
char VERSION[16]="2.2.0";
uint32_t    cissys_getFwVersion(uint8_t** version)
{
	int length = strlen(VERSION)+1;
	//pBuffer = (char*)malloc(sizeof(str)+1);
	//if(pBuffer == NULL)return 0;
	//if(length<)
	cis_memcpy(*version,VERSION,length);
	//free(pBuffer);
	//*version = (uint8_t*) str;
	return length;
}

#define DEFAULT_CELL_ID (95)
#define DEFAULT_RADIO_SIGNAL_STRENGTH (99)

uint32_t    cissys_getCellId()
{
	return DEFAULT_CELL_ID;
}

uint32_t    cissys_getRadioSignalStrength()
{
	return DEFAULT_RADIO_SIGNAL_STRENGTH;
}



#if CIS_ENABLE_UPDATE

#define DEFAULT_BATTERY_LEVEL (99)
#define DEFAULT_BATTERY_VOLTAGE (3800)
#define DEFAULT_FREE_MEMORY   (554990)

#if CIS_ENABLE_UPDATE_MCU
bool isupdatemcu = false;
#endif


int LENGTH = 0;
int RESULT = 0;
int STATE = 0;



int writeCallback = 0;
int validateCallback = 0;
int eraseCallback = 0;
int updateSuccessCallback = 0;
//int  g_nFotaTestThreadExit = 0;

#if 0
static void testThread(void *lpParam)
{
    OSI_PRINTFI("cis_if_sys.c ==> testThread entry!");
	cissys_callback_t *cb=(cissys_callback_t *)lpParam;
	while(g_nFotaTestThreadExit == 0)
	{
		if(writeCallback == 1) //write callback
		{
			cissys_sleepms(2000);
			//FILE* f = NULL;
			//int i = 0;
			//OutputDebugString("cissys_writeFwBytes\n");

			//Sleep(100);
			//f = fopen("cis_serialize_package.bin","a+b");
			//fseek(f, 0, SEEK_END);
			//if(f != NULL)
			//{
			// fwrite(buffer,1,size,f);
			// fclose(f);
			//return true;
			//}
			writeCallback = 0;
			cb->onEvent(cissys_event_write_success,NULL,cb->userData,NULL);
			//cb->onEvent(cissys_event_write_fail,NULL,cb->userData,NULL);
			
		}
		else if(eraseCallback == 1) //erase callback
		{
			//cissys_sleepms(5000);
			eraseCallback = 0;
            // LENGTH=0;
                      
            fupdateInvalidate(true);

            /*if (ret == -1)
            {
                cb->onEvent(cissys_event_fw_erase_fail,0,cb->userData,NULL);
            }
            else
            {*/
                cb->onEvent(cissys_event_fw_erase_success,0,cb->userData,NULL);
           // }
			//cb->onEvent(cissys_event_fw_erase_fail,0,cb->userData,NULL);
		}
		else if(validateCallback == 1)//validate
		{
            //cissys_sleepms(3000);
            validateCallback = 0;
            if (fupdateSetReady(NULL)){
                cb->onEvent(cissys_event_fw_validate_success,0,cb->userData,NULL);
            }
            else{
                printf("executeFotaUpgrade failed");
                cb->onEvent(cissys_event_fw_validate_fail,0,cb->userData,NULL);
            }
            //cb->onEvent(cissys_event_fw_validate_fail,0,cb->userData,NULL);
            //cb->onEvent(cissys_event_fw_validate_success,0,cb->userData,NULL);
		}
        else if (updateSuccessCallback == 1)
        {
			cissys_sleepms(3000);
			updateSuccessCallback = 0;
			//cb->onEvent(cissys_event_fw_validate_fail,0,cb->userData,NULL);
			cb->onEvent(cissys_event_fw_update_success,0,cb->userData,NULL);
		}
        cissys_sleepms(20);
		/*else if(nThreadNo == 3)
		{
			Sleep(3000);
			g_syscallback.onEvent(cissys_event_fw_update_success,0,g_syscallback.userData,NULL);
			nThreadNo = -1;
		}*/
	}
    osiThreadExit();
}

#endif


uint32_t    cissys_getFwBatteryLevel()
{
	return DEFAULT_BATTERY_LEVEL;
}

uint32_t    cissys_getFwBatteryVoltage()
{
	return DEFAULT_BATTERY_VOLTAGE;
}
uint32_t    cissys_getFwAvailableMemory()
{
	return DEFAULT_FREE_MEMORY;
}

int    cissys_getFwSavedBytes()
{
	//FILE * pFile;
	//long size = 0;
	//pFile = fopen ("F:\\cissys_writeFwBytes.bin","rb");
	//if (pFile==NULL)
	//	perror ("Error opening file");
	//else
	//{
	//	fseek (pFile, 0, SEEK_END);
	//	size=ftell (pFile); 
	//	fclose (pFile);
	//}
	//return size;
	return LENGTH;
}

int cissys_setFwSavedBytes(cis_fw_context_t * context, uint32_t length)
{
    LENGTH=length;
    context->savebytes = LENGTH;
    vfs_file_write("/cis_fw_context", context, sizeof(cis_fw_context_t));
    return 1;
}

bool    cissys_checkFwValidation(cissys_callback_t *cb)
{
    //validateCallback = 0;
    if (fupdateSetReady(NULL)){
        cb->onEvent(cissys_event_fw_validate_success,0,cb->userData,NULL);
    }
    else{
        printf("executeFotaUpgrade failed");
        cb->onEvent(cissys_event_fw_validate_fail,0,cb->userData,NULL);
    }
	//validateCallback = 1;
	//cissys_sleepms(1000);
	//cb->onEvent(cissys_event_fw_validate_success,0,cb->userData,NULL);
	return true;
}



bool        cissys_eraseFwFlash (cissys_callback_t *cb)
{
	//eraseCallback = 1;
    printf("cissys_eraseFwFlash");
    fupdateInvalidate(true);
    cb->onEvent(cissys_event_fw_erase_success,0,cb->userData,NULL);
	//cissys_sleepms(8000);
	//LENGTH=0;
    //cb->onEvent(cissys_event_fw_erase_success,0,cb->userData,NULL);
	return true;
}

void cissys_ClearFwBytes(void)
{
	LENGTH = 0;

}

uint32_t   cissys_writeFwBytes(cis_fw_context_t * context, uint32_t size, uint8_t* buffer, cissys_callback_t *cb)
{
    /*if (writeFotaUpgradeData(0, 0, buffer, size) != COAP_NO_ERROR)
    {
        cb->onEvent(cissys_event_write_fail,NULL,cb->userData,NULL);
        return 0;
    }*/
	//writeCallback = 1;
	
    LENGTH+=size;
    context->savebytes = LENGTH;
    vfs_file_write("/cis_fw_context", context, sizeof(cis_fw_context_t));
    cissys_sleepms(100);
	cb->onEvent(cissys_event_write_success, NULL, cb->userData, (int *)&size);
	return 1;
}


void cissys_savewritebypes(uint32_t size)
{
    printf("cissys_savewritebypes");
	//LENGTH += size;
}

bool		cissys_updateFirmware (cissys_callback_t *cb)
{
    cissys_sleepms(3000);
	cb->onEvent(cissys_event_fw_update_success,0,cb->userData,NULL);
    //updateSuccessCallback = 1;
	//nThreadNo = 3;
	//cissys_sleepms(10000);
    //cb->onEvent(cissys_event_fw_update_success,NULL,cb->userData,NULL);
	cis_memcpy(VERSION,"1801102",sizeof("1801102"));
	return true;
}

bool    cissys_readContext(cis_fw_context_t * context)
{
    int len = vfs_file_read("/cis_fw_context", context, sizeof(cis_fw_context_t));
    if (len == -1)
    {
        context->result = RESULT;
        context->savebytes = LENGTH;
        context->state = STATE;
        vfs_file_write("/cis_fw_context", context, sizeof(cis_fw_context_t));
        printf("cissys_readContext create file cis_fw_context success");
        return true;
    }

    printf("cissys_readContext context->result: %d", context->result);
    printf("cissys_readContext context->savebytes: %d", context->savebytes);
    printf("cissys_readContext context->state: %d", context->state);

    RESULT = context->result;
    LENGTH = context->savebytes;
    STATE = context->state;
	//context->result = RESULT;
	//context->savebytes = LENGTH;
	//context->state = STATE;
	/*uint32_t readlen;
	char buffer[10];
    FILE* f = fopen("f:\\cis_setFwUpdateResult.bin","rb");
    if(f != NULL)
    {
        fread(buffer,1,sizeof(buffer),f);
		sscanf(buffer,"%d",&(context->result));
    }
	fclose(f);

	f = fopen("f:\\cis_setFwState.bin","rb");
	if(f != NULL)
	{
		fread(buffer,1,sizeof(buffer),f);
		sscanf(buffer,"%d",&(context->state));
	}
	fclose(f);

	f = fopen("f:\\cis_setFwSavedBytes.bin","rb");
    if(f != NULL)
    {
        fread(buffer,1,sizeof(buffer),f);
		sscanf(buffer,"%d",&(context->savebytes));
    }
	fclose(f);*/
	return 1;
}


bool        cissys_setFwState(cis_fw_context_t * context, uint8_t state)
{
	//FILE* f = NULL;
	//int i = 0;
	//char buffer[10];
	//OutputDebugString("cissys_setFwState\n");
	//sprintf(buffer,"%d",state);
	//f = fopen("f:\\cis_setFwState.bin","wb");
	////fseek(f, 0, SEEK_END);
	//if(f != NULL)
	//{
	//	fwrite(buffer,1,1,f);
	//	fclose(f);
	//}
    STATE=state;
    context->state = STATE;
    vfs_file_write("/cis_fw_context", context, sizeof(cis_fw_context_t));
    return true;
}
bool        cissys_setFwUpdateResult(cis_fw_context_t * context, uint8_t result)
{
	//FILE* f = NULL;
	//int i = 0;
	//char buffer[10];
	//OutputDebugString("cissys_setFwUpdateResult\n");
	//sprintf(buffer,"%d",result);
	//f = fopen("f:\\cis_setFwUpdateResult.bin","wb");
	////fseek(f, 0, SEEK_END);
	//if(f != NULL)
	//{
	//	fwrite(buffer,1,1,f);
	//	fclose(f);
	//}
	RESULT=result;
    context->result = RESULT;
    vfs_file_write("/cis_fw_context", context, sizeof(cis_fw_context_t));
    return true;
}

bool        cissys_resetFwContext()
{
    int ret = vfs_unlink("/cis_fw_context");
    if (ret == -1)
    {
        return false;
    }
    else
    {
        return true;
    }
}

#if CIS_ENABLE_UPDATE_MCU

char SOTA_VERSION[16]="0.0.0";
uint32_t MCU_SotaSize=100;


bool cissys_setSwState(bool ismcu)
{
	isupdatemcu = ismcu;
	return true;
}

bool cissys_getSwState(void)
{

    return isupdatemcu;
}

void  cissys_setSotaMemory(uint32_t size)
{
	MCU_SotaSize=size;
}

uint32_t  cissys_getSotaMemory(void)
{
	return MCU_SotaSize;
}

uint32_t    cissys_getSotaVersion(uint8_t** version)
{
	int length = strlen(SOTA_VERSION)+1;
	cis_memcpy(*version,SOTA_VERSION,length);
	return length;
}

void  cissys_setSotaVersion(char* version)
{
	int length = strlen(version)+1;
	cis_memcpy(SOTA_VERSION,version,length);
}

#endif


#endif // CIS_ENABLE_UPDATE

#if CIS_ENABLE_MONITER
/*
int8_t      cissys_getRSRP()
{
	return 0;
}
*/
int8_t      cissys_getSINR()
{
	return 0;
}
int8_t    cissys_getBearer()
{
    return 0;
}

int8_t    cissys_getUtilization()
{
    return 0;
}

int8_t    cissys_getCSQ()
{
    return 0;
}

int8_t    cissys_getECL()
{
    return 0;
}

int8_t    cissys_getSNR()
{
    return 0;
}

int16_t   cissys_getPCI()
{
    return 0;
}

int32_t   cissys_getECGI()
{
    return 0;
}

int32_t   cissys_getEARFCN()
{
    return 0;
}

int16_t   cissys_getPCIFrist()
{
    return 0;
}

int16_t   cissys_getPCISecond()
{
    return 0;
}

int8_t     cissys_getRSRPFrist()
{
    return 0;
}

int8_t     cissys_getRSRPSecond()
{
    return 0;
}

int8_t     cissys_getPLMN()
{
	return 0;
}

int8_t     cissys_getLatitude()
{
  return 0;

}

int8_t     cissys_getLongitude()
{
  return 0;

}

int8_t     cissys_getAltitude()
{
 return 0;

}
#endif // CIS_ENABLE_MONITER
