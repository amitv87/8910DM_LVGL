/*================================================================
  Copyright (c) 2020 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
=================================================================*/
/*=================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN              WHO         WHAT, WHERE, WHY
------------     -------     -------------------------------------------------------------------------------

=================================================================*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ql_api_osi.h"
#include "ql_api_nw.h"

#include "ql_log.h"
#include "ql_api_datacall.h"
#include "ql_ftp_client.h"
#include "ql_fs.h"

#define QL_FTP_LOG_LEVEL                QL_LOG_LEVEL_INFO
#define QL_FTP_LOG(msg, ...)			QL_LOG(QL_FTP_LOG_LEVEL, "ql_FTP", msg, ##__VA_ARGS__)
#define QL_FTP_LOG_PUSH(msg, ...)	    QL_LOG_PUSH("ql_FTP", msg, ##__VA_ARGS__)

static ql_task_t ftp_task = NULL;
#define PRINT_BUF_SIZE 65
#define MAX_PATH_SIZE		256

int ftp_app_net_datacall_fxn(uint8_t nSim,int profile_idx)
{
	int ret = 0;
	int i = 0;
    ql_data_call_info_s info;
	char ip4_addr_str[16] = {0};
    memset(&info, 0x00, sizeof(ql_data_call_info_s));
	while((ret = ql_network_register_wait(nSim, 120)) != 0 && i < 10)
    {
    	i++;
        QL_FTP_LOG("wait network register count: %d",i);
		ql_rtos_task_sleep_s(1);
	}
	if(ret != 0)
    {
		QL_FTP_LOG("network register failure!");
		return -1;
	}
    
    QL_FTP_LOG("network register sucess nSim = %d,i = %d",nSim,i);
    
    ret = ql_set_data_call_asyn_mode(nSim, profile_idx, 0);
    QL_FTP_LOG("network datacall asyn mode ret =%d",ret);
    ret = ql_start_data_call(nSim, profile_idx, QL_PDP_TYPE_IP, "uninet", NULL, NULL, 0);
    QL_FTP_LOG("network datacall sucess ret =%d",ret);
	if(ret != 0){
		QL_FTP_LOG("data call failure!");
	}


    
	if(ql_get_data_call_info(nSim, profile_idx, &info) != 0){
		QL_FTP_LOG("get datacall info ret: %d", ret);
		ql_stop_data_call(nSim, profile_idx);
		return -1;
	}
    QL_FTP_LOG("info->profile_idx: %d", info.profile_idx);
	QL_FTP_LOG("info->ip_version: %d", info.ip_version);
            
	QL_FTP_LOG("info->v4.state: %d", info.v4.state); 
	inet_ntop(AF_INET, &info.v4.addr.ip, ip4_addr_str, sizeof(ip4_addr_str));
	QL_FTP_LOG("info.v4.addr.ip: %s\r\n", ip4_addr_str);

	inet_ntop(AF_INET, &info.v4.addr.pri_dns, ip4_addr_str, sizeof(ip4_addr_str));
	QL_FTP_LOG("info.v4.addr.pri_dns: %s\r\n", ip4_addr_str);

	inet_ntop(AF_INET, &info.v4.addr.sec_dns, ip4_addr_str, sizeof(ip4_addr_str));
	QL_FTP_LOG("info.v4.addr.sec_dns: %s\r\n", ip4_addr_str);
    return 0;
    
}

char hostname[64] = "220.180.239.212:8309";
char username[] = "test";
char password[] = "test";
char get_filename[] = "FTPtest_get.txt";
char put_filename[] = "FTPtest_put.txt";
char dload_filepath[] = "/quec_ftp_test_path";

static void ftp_app_thread(void * arg)
{
	int ret = 0;
    QFILE fd = 0;    
    void * client = NULL;
    char buf[MAX_PATH_SIZE]={0};
    //int i = 0;
    //QL_FTP_CLIENT_FLIE_INFO_T ql_list_file[5]={0};

	ql_rtos_task_sleep_s(10);
	QL_FTP_LOG("=========================== ftp demo2 start ==============================");
   
    if(ftp_app_net_datacall_fxn(0,1) != 0)
    {
        QL_FTP_LOG("net datacall failed!");
        goto exit;
    }

	client = ql_ftp_client_new();

	while (client) 
	{  
		QL_FTP_LOG("=====================ftp_client_test[]======================");
        QL_FTP_LOG("====================[open test]=======>>>>>>>>>");

		ql_ftp_client_setopt(client, QL_FTP_CLIENT_OPT_PDP_CID,1);
		ql_ftp_client_setopt(client, QL_FTP_CLIENT_OPT_START_POS,0);
		ret=ql_ftp_client_open(client,hostname,username,password);
		QL_FTP_LOG("ftp open[%s] ret = %d, error_code = %d",hostname,ret,ql_ftp_client_get_error_code(client));
        if(ret == -1)
            break;
        
        ret=ql_ftp_client_mkdir(client,dload_filepath);
        QL_FTP_LOG("mkdir[%s]  ret=%d\n",dload_filepath,ret);
            
        ret=ql_ftp_client_cwd(client,dload_filepath);
        QL_FTP_LOG("cwd[%s] ret=%d\n",dload_filepath,ret);

        ret=ql_ftp_client_pwd(client, buf, sizeof(buf));
        QL_FTP_LOG("pwd remotedir:%s ret=%d\n",buf,ret);
           
        QL_FTP_LOG("=====================[get test]============>>>>>>>>>");
        ret= ql_ftp_client_get_ex(client,get_filename,NULL,NULL,NULL);
        QL_FTP_LOG("get[%s] ret %d",get_filename,ret);
        fd = ql_fopen(get_filename,"a+");
        if(fd < 0)
        {
            QL_FTP_LOG("open file failed: %d!",fd);
            return;
        }
        else 
        {
            QL_FTP_LOG("open file [%s] size = %d  fd =  %d!",get_filename,ql_fsize(fd),fd);
            ql_fclose(fd);
        }
        
        QL_FTP_LOG("========================[put test]=============>>>>>>>>>");
        ql_rename(get_filename,put_filename);
        
        fd = ql_fopen(put_filename,"a+");
        if(fd < 0)
        {
            QL_FTP_LOG("open file failed: %d!",fd);
            return;
        }
        else
        {
            QL_FTP_LOG("file fd =%d size =%d\n",fd,ql_fsize(fd));
            ql_fclose(fd);
        }
        ret= ql_ftp_client_put_ex(client,put_filename,NULL,NULL,NULL);
        QL_FTP_LOG("ftp put[%s] ret=%d\n",put_filename,ret);
       
        QL_FTP_LOG("========================[list test]=============>>>>>>>>>");
        ql_rtos_task_sleep_s(10); 

        ret=ql_ftp_client_list(client,dload_filepath,NULL,ql_list_file, 5);
        QL_FTP_LOG("list[%s] ret=%d\n",dload_filepath,ret);
        for(i=0;i<ret;i++)
        {
        	QL_FTP_LOG("file%d:%s\n   type:%d\n   size:%d\n   group:%s\n   permissions:%d\n   time:%04d-%02d-%02d %02d:%02d:%02d\n",i,
        		ql_list_file[i].filename,
        		ql_list_file[i].type, 
        		ql_list_file[i].size,
        		ql_list_file[i].group,
        		ql_list_file[i].permissions,
        		ql_list_file[i].timestamp.tm_year+1900,
        		ql_list_file[i].timestamp.tm_mon+1,
        		ql_list_file[i].timestamp.tm_mday,
        		ql_list_file[i].timestamp.tm_hour,
        		ql_list_file[i].timestamp.tm_min,
        		ql_list_file[i].timestamp.tm_sec
        		);
        }

        QL_FTP_LOG("=========================[delete test]=======>>>>>>>>>");
        ret=ql_ftp_client_delete(client,put_filename);
        QL_FTP_LOG("ftp delete[%s] ret=%d\n",put_filename,ret);

        QL_FTP_LOG("==========================[close test]=======>>>>>>>>>");
        ret=ql_ftp_client_close(client);
        QL_FTP_LOG("ftp close ret=%d\n",ret);
        
        QL_FTP_LOG("=================================ftp_client_test[] end============================\n");

        ql_rtos_task_sleep_s(10); 
	}

	ql_ftp_client_release(client);

exit:		
    
    ql_rtos_task_delete(ftp_task);	
    return;
}

int ql_ftp_app_demo2_init(void)
{
	QlOSStatus err = QL_OSI_SUCCESS;
	
    err = ql_rtos_task_create(&ftp_task, 16*1024, 23, "ftp_app_demo2", ftp_app_thread, NULL, 5);
	if(err != QL_OSI_SUCCESS)
    {
		QL_FTP_LOG("ftp_app_demo2 init failed");
	}
	
    return err;
}


