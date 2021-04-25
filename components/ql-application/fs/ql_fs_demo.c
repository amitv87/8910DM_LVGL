#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ql_api_osi.h"
#include "ql_log.h"
#include "ql_fs_demo.h"
#include "ql_fs.h"

#define QL_FS_DEMO_LOG_LEVEL		QL_LOG_LEVEL_INFO
#define QL_FS_DEMO_LOG(msg, ...)	QL_LOG(QL_FS_DEMO_LOG_LEVEL, "ql_fs_demo", msg, ##__VA_ARGS__)

ql_task_t ql_fs_task = NULL;

void ql_fs_callback(void *ctx)
{
	int fd = 0;
	int64 err = QL_FILE_OK;
	char buffer[100];
	char *str = TEST_STR;

	err = ql_mkdir(DIR_PATH, 0);
	if(err < 0)
	{
		if(err == QL_DIR_DIR_ALREADY_EXIST)
		{
			QL_FS_DEMO_LOG("dir exist, not create");
		}
		else
		{
			QL_FS_DEMO_LOG("make dir failed");
			goto exit;
		}
	}

	fd = ql_fopen(FILE_PATH, "wb+");
	if(fd < 0)
	{
		QL_FS_DEMO_LOG("open file failed");
		err = fd;
		goto exit;
	}

	err = ql_fwrite(str, strlen(str) + 1, 1, fd);   //strlen not include '\0'
	if(err < 0)
	{
		QL_FS_DEMO_LOG("write file failed");
		ql_fclose(fd);
		goto exit;		
	}

	err = ql_frewind(fd);
	if(err < 0)
	{
		QL_FS_DEMO_LOG("rewind file failed");
		ql_fclose(fd);
		goto exit;					
	}
	
	err = ql_fread(buffer, ql_fsize(fd), 1, fd);
	if(err < 0)
	{
		QL_FS_DEMO_LOG("read file failed");
		ql_fclose(fd);
		goto exit;					
	}

	QL_FS_DEMO_LOG("file read result is %s", buffer);
	ql_fclose(fd);

	err = ql_nvm_fwrite(TEST_NVM, str, strlen(str) + 1, 1);  //nvm 配置文件不需要输入路径
	if(err < 0)
	{
		QL_FS_DEMO_LOG("write nv failed");
		goto exit;
	}

	memset(buffer, 0, 100);
	err = ql_nvm_fread(TEST_NVM, buffer, strlen(str) + 1, 1);
	if(err < 0)
	{
		QL_FS_DEMO_LOG("read nv failed");
		goto exit;
	}	
    
    QL_FS_DEMO_LOG("nvm read result is %s", buffer);

    err = ql_cust_nvm_fwrite(str, strlen(str) + 1, 1);  //客户nvm 配置文件不需要输入文件名，使用空间不大于1KB 
	if(err < 0)
	{
		QL_FS_DEMO_LOG("write nv failed");
		goto exit;
	}

	memset(buffer, 0, 100);
	err = ql_cust_nvm_fread(buffer, strlen(str) + 1, 1);
	if(err < 0)
	{
		QL_FS_DEMO_LOG("read nv failed");
		goto exit;
	}	

	QL_FS_DEMO_LOG("cust nvm read result is %s", buffer);
	
exit:
	
	if(err < 0)
	{
		QL_FS_DEMO_LOG("errcode is %x", err);
	}
	
	QL_FS_DEMO_LOG("exit file demo");
	ql_rtos_task_delete(NULL);
}


int ql_fs_demo_init(void)
{	
	QlOSStatus err = QL_OSI_SUCCESS;
	
	err = ql_rtos_task_create(&ql_fs_task, QL_FS_TASK_STACK_SIZE, QL_FS_TASK_PRIO, "ql_fs_task", ql_fs_callback, NULL, QL_FS_TASK_EVENT_CNT);
	if(err != QL_OSI_SUCCESS)
	{
		QL_FS_DEMO_LOG("file_task created failed");
	}

	return err;
}


