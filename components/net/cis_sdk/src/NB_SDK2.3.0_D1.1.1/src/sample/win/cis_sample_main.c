#include <stdio.h>
#include <string.h>
#include <cis_def.h>
#include <cis_api.h>
#include <windows.h>

extern int cis_sample_entry(const uint8_t * config_bin,uint32_t config_size);
extern int dm_sample_dm_entry(const uint8_t * config_bin,uint32_t config_size);
DWORD WINAPI cis_sample_thread(LPVOID lpParam);
DWORD WINAPI dm_sample_thread(LPVOID lpParam);
#if CIS_ENABLE_CMIOT_OTA
extern int ota_sample_entry(const uint8_t * config_bin,uint32_t config_size);
#endif

#if 1  
//authcode和psk需要重新生成
//183.230.40.39  
static const uint8_t config_hex[] = {
0x13, 0x00, 0x59,
0xf1, 0x00, 0x03,
0xf2, 0x00, 0x4b,
    0x04, 0x00 /*mtu*/, 0x11 /*Link & bind type*/, 0x80 /*BS DTLS ENABLED*/,
    0x00, 0x05 /*apn length*/, 0x43, 0x4d, 0x49, 0x4f, 0x54 /*apn: CMIOT*/,
    0x00, 0x00 /*username length*/, /*username*/
    0x00, 0x00 /*password length*/, /*password*/
    0x00, 0x12 /*host length*/, 0x31, 0x38, 0x33, 0x2e, 0x32, 0x33, 0x30, 0x2e,
0x34, 0x30, 0x2e, 0x33, 0x39, 0x3a, 0x35, 0x36, 0x38, 0x33 /*host: 183.230.40.39
:5683*/,
    0x00, 0x23 /*userdata length*/, 0x41, 0x75, 0x74, 0x68, 0x43, 0x6f, 0x64, 0x65, 
	0x3a, 0x79, 0x61, 0x6e, 0x67, 0x79, 0x61, 0x6e, 0x6c, 0x69, 0x75, 0x3b, 0x50, 
	0x53, 0x4b, 0x3a, 0x79, 0x61, 0x6e, 0x67, 0x79, 0x61, 0x6e, 0x6c, 0x69, 0x75,
0x3b /*userdata: AuthCode:yangyanliu;PSK:yangyanliu;*/,
0xf3, 0x00, 0x08,
    0xe4 /*log config*/, 0x00, 0xc8 /*LogBufferSize: 200*/,
    0x00, 0x00 /*userdata length*//*userdata*/
	
};
#endif



#if		CIS_ENABLE_CMIOT_OTA
uint8_t    g_cmiot_otastart_flag=0; //otastart -flag should be  set when OTASTART command is called by user
cis_ota_history_state    g_cmiot_otafinishstate_flag = OTA_HISTORY_STATE_RAW; //Ota finish state flag should be  set when SDK received otafinish:0 from OneNET. It means this terminal has already finished the OTA procedure successfully
int32_t    g_cmiot_timeout_duration=0;
uint8_t    g_cmiot_ota_at_return_code=99;//the return result defination

DWORD WINAPI ota_sample_thread(LPVOID lpParam)
{
    
   // while(1)
   {
	  ota_sample_entry(config_hex,sizeof(config_hex));
    }
    return 1;
}


#endif

DWORD WINAPI cis_sample_thread(LPVOID lpParam)
{
    
    while(1){
	  cis_sample_entry(config_hex,sizeof(config_hex));
   
    }
    return 1;
}
#if CIS_ENABLE_DM
static const uint8_t dm_config_hex[] = {
0x13,0x00,0x5f,0xf1,0x00,0x03,0xf2,0x00,0x48,0x04,0x00,0x11,0x00,0x00,0x05,0x43,
0x4d,0x49,0x4f,0x54,0x00,0x04,0x73,0x64,0x6b,0x55,0x00,0x04,0x73,0x64,0x6b,0x50,
0x00,0x10,0x31,0x31,0x37,0x2e,0x31,0x36,0x31,0x2e,0x32,0x2e,0x37,0x3a,0x35,0x36,
0x38,0x33,0x00,0x1a,0x41,0x75,0x74,0x68,0x43,0x6f,0x64,0x65,0x3a,0x3b,0x55,0x73,
0x65,0x72,0x64,0x61,0x74,0x61,0x3a,0x61,0x62,0x63,0x31,0x32,0x33,0x3b,0xf3,0x00,
0x11,0xe4,0x00,0xc8,0x00,0x09,0x73,0x79,0x73,0x55,0x44,0x3a,0x31,0x32,0x33
};

DWORD WINAPI dm_sample_thread(LPVOID lpParam)
{
    
    while(1){
	 dm_sample_dm_entry(dm_config_hex,sizeof(dm_config_hex));
       
    }
    return 1;
}
#endif
int main(int argc, char *argv[])
{
	HANDLE hThread;
	DWORD dwThread;
#if CIS_ENABLE_CMIOT_OTA
   g_cmiot_otastart_flag = 1; //otastart -flag should be	set when OTASTART command is called by user
   g_cmiot_otafinishstate_flag = OTA_HISTORY_STATE_RAW; //Ota finish state flag should be  set when SDK received otafinish:0 from OneNET. It means this terminal has already finished the OTA procedure successfully  									//Ota finish state flag should be stored in the  nonvolatile memory(flash) and  can be querird and changed by the AT COMMAND.
    g_cmiot_timeout_duration=120; // After OTASTART command being called, OTA precedure will be terminated in a finite time -t while the terminal has not receice the OTASTART command from OneNET.

	if(g_cmiot_otastart_flag==1)
	{
		if(g_cmiot_otafinishstate_flag==OTA_HISTORY_STATE_FINISHED)
		{
		//Ota finish state flag should be  set when SDK received otafinish:0 from OneNET. It means this terminal has already finished the OTA procedure successfully
   		//Ota finish state flag should be stored in the  nonvolatile memory(flash) and  can be querird and changed by the AT COMMAND.
   		//SDK should immediately  return finish:0 indication to the user who call OTASTART when g_cmiot_otafinishstate_flag is high.
			g_cmiot_ota_at_return_code = OTA_FINISH_AT_RETURN_REPEATED;
			return 0;
		}
		g_cmiot_ota_at_return_code = OTA_FINISH_AT_RETURN_TIMEOUT_FIRST;
		hThread = CreateThread(NULL,0,ota_sample_thread,(LPVOID)NULL,0,&dwThread);
		CloseHandle(hThread);				
	}
	else
	{
#endif
	hThread = CreateThread(NULL, 0, cis_sample_thread, (LPVOID)NULL, 0, &dwThread);
	CloseHandle(hThread);
#if CIS_ENABLE_DM
	//Sleep(3000);
	//hThread = CreateThread(NULL, 0, dm_sample_thread, (LPVOID)NULL, 0, &dwThread);
	//CloseHandle(hThread);
#endif
#if CIS_ENABLE_CMIOT_OTA
	}
#endif
	while (1)
		Sleep(1000);
	return 0;
}



