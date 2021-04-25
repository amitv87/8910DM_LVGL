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
//#include "vfs.h"

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
char data_tmp[]="hello\n";
int read_data_count=0;
#define PRINT_BUF_SIZE 65

#define MAX_PATH_SIZE		256
int dl_size;
int upl_size;

size_t read_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
    /*
    memcpy(ptr,data_tmp,sizeof(data_tmp));
    QL_FTP_LOG("ptr = %s", (char*)ptr);
    QL_FTP_LOG("size = %d,ret = %d",size*nmemb,sizeof(data_tmp));
	if(read_data_count++<30)
		return sizeof(data_tmp);
	else
		return 0;
    */
    int ret = 0;
    QFILE* fd;
    fd = (QFILE*)stream;
    if(upl_size >= ql_fsize(*fd))
    {
        return 0;
    }
    ret = ql_fread(ptr, size, nmemb, *fd);
    QL_FTP_LOG("ret=%d,size = %d", ret,size*nmemb);
    QL_FTP_LOG("data = %s", (char*)ptr);
    upl_size += ret;
    return ret;
}

size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{

	int ret = 0;
	QL_FTP_LOG("data = %s", (char*)ptr);
    QL_FTP_LOG("data_len = %d",size*nmemb);
    QFILE* fd;
    
    fd = (QFILE*)stream;
    ret = ql_fwrite(ptr,size,nmemb,*fd);
    if(ret != size*nmemb)
    {
        return 0;
    }
    dl_size += size*nmemb;
	return size*nmemb;
}
/*
size_t file_list(void *ptr, size_t size, size_t nmemb, void *stream)
{
        //第一次的时候收到数据后直接透传出去，
        QL_FTP_LOG("======> %s",ptr);
        QL_FTP_LOG("ret=%d", size*nmemb);
       
    return size*nmemb;
}
*/
static void ftp_app_thread(void * arg)
{
	int ret = 0;
	int i = 0;
	//ql_nw_reg_status_info_s  nw_info;
	int profile_idx = 1;
    int start_pos = 0;
    ql_data_call_info_s info;
	char ip4_addr_str[16] = {0};
	void * client = NULL;
	char buf[MAX_PATH_SIZE]={0};
    int run_num=1;
	char hostname[64]="220.180.239.212:8309";
	char username[]="test";
	char password[]="test";
	QL_FTP_CLIENT_FLIE_INFO_T ql_list_file[5]={0};
	uint8_t nSim = 0;
	//int fp = -1;

	ql_rtos_task_sleep_s(10);
	QL_FTP_LOG("========== ftp demo start ==========");
	QL_FTP_LOG("wait for network register done");
		
	while((ret = ql_network_register_wait(nSim, 120)) != 0 && i < 10){
    	i++;
		ql_rtos_task_sleep_s(1);
	}
	if(ret == 0){
		i = 0;
		QL_FTP_LOG("====network registered!!!!====");
	}else{
		QL_FTP_LOG("====network register failure!!!!!====");
		goto exit;
	}

	ql_set_data_call_asyn_mode(nSim, profile_idx, 0);

	QL_FTP_LOG("===start data call====");
	ret=ql_start_data_call(nSim, profile_idx, QL_PDP_TYPE_IP, "uninet", NULL, NULL, 0); 
	QL_FTP_LOG("===data call result:%d", ret);
	if(ret != 0){
		QL_FTP_LOG("====data call failure!!!!=====");
	}
	memset(&info, 0x00, sizeof(ql_data_call_info_s));
	
	ret = ql_get_data_call_info(nSim, profile_idx, &info);
	if(ret != 0){
		QL_FTP_LOG("ql_get_data_call_info ret: %d", ret);
		ql_stop_data_call(nSim, profile_idx);
		goto exit;
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

	client = ql_ftp_client_new();
    QFILE fd = 0;
     run_num = 2;
      
	while (client && run_num<=100) 
	{  
        dl_size = 0;
        upl_size = 0;
		read_data_count=0;
		QL_FTP_LOG("==============ftp_client_test[%d]================\n",run_num);
        if(run_num == 2)
            fd = ql_fopen("UFS:20201210test.txt","w+");
        else 
            fd = ql_fopen("UFS:20201210test.txt","r+");
        if(fd < 0)
        {
            QL_FTP_LOG("open file failed !");
            return;
        }
        else 
            QL_FTP_LOG("open file success %d !",fd);
		ret=ql_ftp_client_setopt(client, QL_FTP_CLIENT_OPT_PDP_CID, profile_idx);              /*set PDP cid,if not set,using default PDP*/
		ret=ql_ftp_client_setopt(client, QL_FTP_CLIENT_OPT_START_POS, start_pos);
		ret=ql_ftp_client_open(client,hostname,username,password);
		QL_FTP_LOG("\nql_ftp_client_open[%s] ret=%d;\n",hostname,ret);
		if(ret==-1)
		{
			QL_FTP_LOG("ql_ftp_client_open error_code=%d\n",ql_ftp_client_get_error_code(client));
			break;
		}
		
#if 0

		ret=ql_ftp_client_lcwd(client,"B:/");
		QL_FTP_LOG("\nql_ftp_client_lcwd[B:/] ret=%d\n",ret);

		ret=ql_ftp_client_lpwd(client, buf, sizeof(buf));
		QL_FTP_LOG("\nql_ftp_client_lpwd ret=%d; localdir:%s\n",ret,buf);

		ret=ql_ftp_client_lcwd(client,"U:/");
		QL_FTP_LOG("\nql_ftp_client_lcwd[U:/] ret=%d\n",ret);

		ret=ql_ftp_client_lcwd(client,"test");
		QL_FTP_LOG("\nql_ftp_client_lcwd[test] ret=%d\n",ret);
		if(ret==-1)
		{
			QL_FTP_LOG("error_code=%d\n",ql_ftp_client_get_error_code(client));
		}
		ret=ql_ftp_client_lpwd(client, buf, sizeof(buf));
		QL_FTP_LOG("\nql_ftp_client_lpwd ret=%d; localdir:%s\n",ret,buf);


#endif
		ret=ql_ftp_client_pwd(client, buf, sizeof(buf));
		QL_FTP_LOG("\nql_ftp_client_pwd ret=%d; remotedir:%s\n",ret,buf);
	
		if(run_num % 17== 0){
			ret=ql_ftp_client_mkdir(client, "ftptest");
			QL_FTP_LOG("\nql_ftp_client_mkdir[ftptest] ret=%d\n",ret);

			ret=ql_ftp_client_cwd(client, "ftptest");
			QL_FTP_LOG("\nql_ftp_client_cwd[ftptest] ret=%d\n",ret);

			ret=ql_ftp_client_pwd(client, buf, sizeof(buf));
			QL_FTP_LOG("\nql_ftp_client_pwd ret=%d; remotedir:%s\n",ret,buf);


			ret=ql_ftp_client_cwd(client, "/");
			QL_FTP_LOG("\nql_ftp_client_cwd[/] ret=%d\n",ret);

			ret=ql_ftp_client_rmdir(client, "ftptest");
			QL_FTP_LOG("\nql_ftp_client_rmdir[ftptest] ret=%d\n",ret);

		}else if(run_num % 15== 0){
            ret=ql_ftp_client_cwd(client, "/ftp");
            QL_FTP_LOG("\nql_ftp_client_cwd[ftptest] ret=%d\n",ret);

            ret=ql_ftp_client_pwd(client, buf, sizeof(buf));
            QL_FTP_LOG("\nql_ftp_client_pwd ret=%d; remotedir:%s\n",ret,buf);
            
			ret=ql_ftp_client_list(client, "/ftp",NULL,ql_list_file, 5);
			QL_FTP_LOG("\nql_ftp_client_list[/] ret=%d\n",ret);
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
            run_num = 9;
		}else if(run_num % 13 == 0){

		/*	ret=ql_ftp_client_list(client, "/","1.txt",file_list, 1);
			QL_FTP_LOG("\nql_ftp_client_list[/1.txt] ret=%d\n",ret);
			for(i=0;i<ret;i++)
			{
				QL_FTP_LOG("file%d:%s\n   type:%d\n   size:%d\n   group:%s\n   permissions:%d\n   time:%04d-%02d-%02d %02d:%02d:%02d\n",i,
					file_list[i].filename,
					file_list[i].type, 
					file_list[i].size,
					file_list[i].group,
					file_list[i].permissions,
					file_list[i].timestamp.tm_year+1900,
					file_list[i].timestamp.tm_mon+1,
					file_list[i].timestamp.tm_mday,
					file_list[i].timestamp.tm_hour,
					file_list[i].timestamp.tm_min,
					file_list[i].timestamp.tm_sec
					);
			}
            */
		}
		else if(run_num % 2 == 0){
			ret= ql_ftp_client_get_ex(client,"/ftp/10K.txt",NULL,write_data,(void*)&fd);
			QL_FTP_LOG("\nql_ftp_client_get[1M.txt->test.txt] ret=%d,dl_size = %d\n",ret,dl_size);
            run_num = 15;
		}
		else if(run_num % 9 ==0){
			ret=ql_ftp_client_delete(client,"/ftp/10K.txt");
			QL_FTP_LOG("\nql_ftp_client_delete[test_device.txt] ret=%d\n",ret);
		    ql_rtos_task_sleep_s(25); 
			ret= ql_ftp_client_put_ex(client,"UFS:20201210test.txt","/ftp/10K.txt",read_data,&fd);
			QL_FTP_LOG("\nql_ftp_client_put[test_device.txt->test_device.txt] ret=%d upl_size = %d\n",ret,upl_size);
            run_num = 2;
	    }

		else if(run_num % 7 ==0){
			ret= ql_ftp_client_get_ex(client,"1","/user/test_device.txt",NULL,NULL);
			QL_FTP_LOG("\nql_ftp_client_get[1M.txt->test_device.txt] ret=%d\n",ret);
#if 0
			fp = vfs_open("/usr/test_device.txt", O_RDONLY);
			if(fp >= 0)
			{
				QL_FTP_LOG("ql_fread start\n");
				while(1)
				{
					int n=vfs_read(fp,buf,65);
					
					if(n>0)
					{
						buf[n]=0;
						QL_FTP_LOG("%s",buf);
					}
					else
					{
						QL_FTP_LOG("\nql_fread end\n");
						break;
					}
				}
				vfs_close(fp);
			}
			else
			{
				QL_FTP_LOG("ql_fopen failed\n");
			}
#endif
		}else if(run_num % 3 ==0){
			ret= ql_ftp_client_put_ex(client,"UFS:20201210test.txt","20201210test.txt",read_data,&fd);
			QL_FTP_LOG("\nql_ftp_client_put[20201210test.txt->20201210test.txt] ret=%d,   upl_size = %d\n",ret,upl_size);
            run_num = 2;
		}else if(run_num % 5 == 0){
			ret=ql_ftp_client_delete(client,"test_device_create.txt");
			QL_FTP_LOG("\nql_ftp_client_delete[test_device_create.txt] ret=%d\n",ret);

		}

		ret=ql_ftp_client_close(client);
		QL_FTP_LOG("ql_ftp_client_close ret=%d\n",ret);
		
		QL_FTP_LOG("==============ftp_client_test[%d] end================\n",run_num);

		//run_num++;
		QL_FTP_LOG("file fd =%d size =%d\n",fd,ql_fsize(fd));
        ql_fclose(fd);
		ql_rtos_task_sleep_s(5); 
        
		
	}

	ql_ftp_client_release(client); /*release http resources*/
exit:		
   ql_rtos_task_delete(ftp_task);	
 
	return;
}

int ql_ftp_app_init(void)
{
	QlOSStatus err = QL_OSI_SUCCESS;
	
    err = ql_rtos_task_create(&ftp_task, 16*1024, 23, "ftp_app", ftp_app_thread, NULL, 5);
	if(err != QL_OSI_SUCCESS)
    {
		QL_FTP_LOG("ftp_app init failed");
	}
	
    return err;
}

