#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ql_type.h"
#include "ql_api_osi.h"
#include "ql_log.h"
#include "ql_api_datacall.h"
#include "ql_fs.h"
#include "ql_power.h"
#include "ql_api_dev.h"
#define QL_FOTA_HTTP_LOG_LEVEL QL_LOG_LEVEL_INFO
#define QL_FOTA_HTTP_LOG(msg, ...) QL_LOG(QL_FOTA_HTTP_LOG_LEVEL, "ql_FOTA_http", msg, ##__VA_ARGS__)
#define QL_FOTA_HTTP_LOG_PUSH(msg, ...) QL_LOG_PUSH("ql_FOTA_http", msg, ##__VA_ARGS__)

#ifdef QL_APP_FEATURE_HTTP_FOTA
#ifdef QL_APP_FEATURE_HTTP

#include "ql_api_fota.h"
#include "ql_http_client.h"
#include "ql_fs.h"
#define QL_VERSION_MAX 256
#define HTTP_HEAD_RANGE_LENGTH_MAX  50
#define QL_HTTP_FOTA_DEMO_NVM_CFG           "http_fota_nv.cfg"    
#define HTTP_DLOAD_URL                      "http://49.235.127.120/output.pack"
ql_task_t fota_http_task = NULL;
ql_task_t fota_http_show_task = NULL;
ql_sem_t fota_http_semp = NULL;
ql_sem_t http_show_semp = NULL;
typedef struct 
{
    bool is_support;
    bool is_show;
    uint total_size;
    uint dload_size;
}fota_http_progress_t;

typedef struct 
{
    http_client_t http_cli;
    bool poweroutage_resume;
    int profile_idx;
    uint8_t sim_id;
    QFILE fd;
    char fota_packname[QL_FOTA_PACK_NAME_MAX_LEN];
    fota_http_progress_t http_progress;
}fota_http_client_t;
fota_http_client_t  fota_http_cli = 
{
    0,
    false,
    0,
    0,
    -1,
    {0},
    {
        false,
        false,
        0,
        0,
    }
};

void http_show_task_destroy(void)
{
    ql_rtos_semaphore_delete(http_show_semp);
    ql_rtos_task_delete(fota_http_show_task);
}

static void http_show_task(void *argv)
{
    float temp = 0;
    ql_rtos_semaphore_create(&http_show_semp, 0);
    while(1)
    {
        ql_rtos_semaphore_wait(http_show_semp, QL_WAIT_FOREVER);
        if(fota_http_cli.http_progress.is_show == false)
        {
            break;
        }
        temp = (float)fota_http_cli.http_progress.dload_size/(float)fota_http_cli.http_progress.total_size;
        QL_FOTA_HTTP_LOG("dload progress:===[%d]===>>",(int)(temp*100));
    }
    http_show_task_destroy();
}

void fota_http_poweroutage_resume_set(bool is_set)
{
    if(is_set == true)
    {
        fota_http_cli.poweroutage_resume = true;
        ql_nvm_fwrite(QL_HTTP_FOTA_DEMO_NVM_CFG, &fota_http_cli, sizeof(fota_http_client_t), 1);
    }
}

void fota_http_poweroutage_resume_clean(void)
{
    fota_http_client_t  temp;
    memset(&temp,0, sizeof(fota_http_client_t));
    ql_nvm_fwrite(QL_HTTP_FOTA_DEMO_NVM_CFG, &temp, sizeof(fota_http_client_t), 1);
    fota_http_cli.poweroutage_resume = false;
}

int fota_http_poweroutage_resume_get(fota_http_client_t* fota_http_cli_p)
{
    fota_http_client_t  temp;
    memset(&temp,0,sizeof(fota_http_client_t));
    if(ql_nvm_fread(QL_HTTP_FOTA_DEMO_NVM_CFG, &temp, sizeof(fota_http_client_t), 1) < 0)
    {
        return -1;
    }
    else 
    {
        if(temp.poweroutage_resume == true)
            memcpy(fota_http_cli_p,&temp,sizeof(fota_http_client_t));
        else 
            return -1;
    }
    return 0;  
}

void fota_http_event_cb(http_client_t *client, int event, int event_code, void *argv)
{
    if (*client != fota_http_cli.http_cli)
        return;
    switch (event)
    {
    case HTTP_EVENT_SESSION_ESTABLISH: {
        if (event_code != HTTP_SUCCESS)
        {
            QL_FOTA_HTTP_LOG("HTTP session create failed!!!!!");
            ql_rtos_semaphore_release(fota_http_semp);
        }
    }
    break;
    case HTTP_EVENT_RESPONE_STATE_LINE: {
        if (event_code == HTTP_SUCCESS)
        {
            int resp_code = 0;
            int content_length = 0;
            int chunk_encode = 0;
            char *location = NULL;
            ql_httpc_getinfo(client, HTTP_INFO_RESPONSE_CODE, &resp_code);
            QL_FOTA_HTTP_LOG("response code:%d", resp_code);
            
            ql_httpc_getinfo(client, HTTP_INFO_CHUNK_ENCODE, &chunk_encode);
            if (chunk_encode == 0)
            {
                ql_httpc_getinfo(client, HTTP_INFO_CONTENT_LEN, &content_length);
                fota_http_cli.http_progress.total_size = content_length;
                QL_FOTA_HTTP_LOG("content_length:%d", fota_http_cli.http_progress.total_size);
                fota_http_cli.http_progress.is_support = true;
            }
            else
            {
                 fota_http_cli.http_progress.is_support = false;
                 fota_http_cli.http_progress.is_show = false;
            }

            if (resp_code >= 300 && resp_code < 400)
            {
                ql_httpc_getinfo(client, HTTP_INFO_LOCATION, &location);
                QL_FOTA_HTTP_LOG("redirect location:%s", location);
                free(location);
            }
        }
    }
    break;
    case HTTP_EVENT_SESSION_DISCONNECT: {
        if (event_code == HTTP_SUCCESS)
        {
            QL_FOTA_HTTP_LOG("===http transfer end!!!!");
        }
        else
        {
            QL_FOTA_HTTP_LOG("===http transfer occur exception!!!!!");
        }
        ql_rtos_semaphore_release(fota_http_semp);
    }
    break;
    }
}

int fota_http_write_response_data(http_client_t *client, void *argv, char *data, int size, unsigned char end)
{
    fota_http_client_t* fota_cli_p = (fota_http_client_t*)argv;
    int ret = 0;
    
    if (*client != fota_cli_p->http_cli || fota_cli_p->fd < 0)
    {
        QL_FOTA_HTTP_LOG("*client:%d, http_cli:%d, fd:%d", *client, fota_cli_p->http_cli, fota_cli_p->fd);
        return 0;
    }
    ret = ql_fwrite(data, size, 1, fota_cli_p->fd);
    if (ret > 0)
    {
        fota_cli_p->http_progress.dload_size += ret;
        if(fota_http_cli.http_progress.is_show)
            ql_rtos_semaphore_release(http_show_semp);
        return ret;
    }
    else
    {
        QL_FOTA_HTTP_LOG("error: ret:%d", ret);
    }
    return 0;
}


int fota_http_network_data_call(  uint8_t sim_id   ,int profile_idx)
{
    int ret = 0;
    int i = 0;
    while ((ret = ql_network_register_wait(sim_id, 120)) != 0 && i < 10)
    {
        i++;
        ql_rtos_task_sleep_s(1);
    }
    if (ret != 0)
    {
        QL_FOTA_HTTP_LOG("network registered fail");
        return -1;
    }
    //pdp data call 激活
    ql_set_data_call_asyn_mode(sim_id, profile_idx, 0);
    ret = ql_start_data_call(sim_id, profile_idx, QL_PDP_TYPE_IP, "uninet", NULL, NULL, 0);
    if (ret != 0)
    {
        QL_FOTA_HTTP_LOG("data call failure!");
        return -1;
    }
    return 0;
}

int fota_http_download_file(void)
{
    int ret = 0;
    int http_method = HTTP_METHOD_GET;
    char dload_range[HTTP_HEAD_RANGE_LENGTH_MAX] = {0};
    if (ql_httpc_new(&fota_http_cli.http_cli, fota_http_event_cb, NULL) != HTTP_SUCCESS)
    {
        QL_FOTA_HTTP_LOG("http create failed");
        ret = -1;
        goto exit;
    }
    //fopen操作获得文件描述符，设置http下载保存在模块文件系统的位置，
    ql_httpc_setopt(&fota_http_cli.http_cli, HTTP_CLIENT_OPT_METHOD,http_method);
    sprintf(dload_range, "Range: bytes=%d-",fota_http_cli.http_progress.dload_size);
    ql_httpc_setopt(&fota_http_cli.http_cli, HTTP_CLIENT_OPT_REQUEST_HEADER,dload_range);   
    ql_httpc_setopt(&fota_http_cli.http_cli, HTTP_CLIENT_OPT_URL, HTTP_DLOAD_URL);
    ql_httpc_setopt(&fota_http_cli.http_cli, HTTP_CLIENT_OPT_PDPCID, fota_http_cli.profile_idx);
    ql_httpc_setopt(&fota_http_cli.http_cli, HTTP_CLIENT_OPT_WRITE_FUNC, fota_http_write_response_data);
    ql_httpc_setopt(&fota_http_cli.http_cli, HTTP_CLIENT_OPT_WRITE_DATA, (void *)&fota_http_cli);
    fota_http_poweroutage_resume_set(true);
    if (ql_httpc_perform(&fota_http_cli.http_cli) == HTTP_SUCCESS)
    {
        ql_rtos_semaphore_wait(fota_http_semp, QL_WAIT_FOREVER);
        QL_FOTA_HTTP_LOG("fota http dload size %d=====End,\n", ql_fsize(fota_http_cli.fd));
    }
    else
    {
        QL_FOTA_HTTP_LOG("http dload failed\n");
        ret = -1;
        goto exit;
    }
exit:
    ql_httpc_release(&fota_http_cli.http_cli);
    return ret;
}

QFILE fota_http_set_FotaPackName(char*FotaPackName,bool poweroutage_resume)
{
    QFILE dload_fd = -1;
    //set fota packname path
    if(poweroutage_resume == true)
        dload_fd = ql_fopen(FotaPackName, "ab+");
    else 
        dload_fd = ql_fopen(FotaPackName, "wb+");
    if (dload_fd < 0)
    {
        if (QL_FILE_ALREADY_OPERATION == dload_fd)
        {
            QL_FOTA_HTTP_LOG("file has been opend [%s]!\n", FotaPackName);
        }
        else
        {
            QL_FOTA_HTTP_LOG("open file failed [%s]!\n", FotaPackName);
        }
        return -1;
    }
    if(poweroutage_resume == true)
    {
        fota_http_cli.http_progress.dload_size = ql_fsize(dload_fd);
        QL_FOTA_HTTP_LOG("poweroutage resume begin site %d\n",fota_http_cli.http_progress.dload_size);
    }
    else 
        fota_http_cli.http_progress.dload_size = 0;
    return dload_fd;
}

int fota_http_download_updata_file(void)
{
    int ret = 0;
    //fopen操作获得文件描述符，设置http下载保存在模块文件系统的位置，
    fota_http_cli.fd = fota_http_set_FotaPackName(fota_http_cli.fota_packname,fota_http_cli.poweroutage_resume);
    if (fota_http_cli.fd < 0)
    {
        ret = -1;
        goto exit;
    }

    if(fota_http_download_file() != 0)
    {
        ret = -1;
        goto exit;
    }
    ret = ql_fota_image_verify(fota_http_cli.fota_packname);
    fota_http_poweroutage_resume_clean();
    if (ret != QL_FOTA_SUCCESS)
    {
        QL_FOTA_HTTP_LOG("fota verify failed,err_code:%d\n", ret);
        ret = -1;
        goto exit;
    }
    else
    {
        QL_FOTA_HTTP_LOG("system reset power!");
        ql_rtos_task_sleep_s(5);
        ql_power_reset(RESET_NORMAL);
    }
exit:
    if (fota_http_cli.fd >= 0)
    {
        ql_fclose(fota_http_cli.fd);
        fota_http_cli.fd = -1;
    }
    if(0 != ql_remove(fota_http_cli.fota_packname))
        QL_FOTA_HTTP_LOG("delete dload file failed!");
    return ret;
}

int fota_http_update_process(void)
{
    ql_errcode_fota_e err_code;
    ql_fota_result_e p_fota_result;
    int ret = 0;
    err_code = ql_fota_get_result(&p_fota_result);
    QL_FOTA_HTTP_LOG("ql fota get result %d!",p_fota_result);
    if (err_code != QL_FOTA_SUCCESS)
    {
        return -1;
    }

    if (p_fota_result == QL_FOTA_FINISHED)
    {
        QL_FOTA_HTTP_LOG("update sucessful!");
        ql_fota_file_reset(TRUE);
    }
    else if (p_fota_result == QL_FOTA_NOT_EXIST)
    {
        QL_FOTA_HTTP_LOG("no fota file,need download file!");
        fota_http_download_updata_file();
        //该函数能够正常退出，那么表明fota下载函数没有完成。
        return -1;
    }
    else if (p_fota_result == QL_FOTA_READY)
    {
        err_code = ql_fota_image_verify(fota_http_cli.fota_packname);
        if (err_code != QL_FOTA_SUCCESS)
        {
            QL_FOTA_HTTP_LOG("fota verify failed, err_code:%d\n", ret);
            return -1;
        }
        else
        {
            QL_FOTA_HTTP_LOG("fota update,system reset power!");
            ql_rtos_task_sleep_s(5);
            ql_power_reset(RESET_NORMAL);
        }
    }
    else if (p_fota_result == QL_FOTA_STATUS_INVALID)
    {
        QL_FOTA_HTTP_LOG("fota status is invalid!!!!");
        return -1;
    }
    return 0;
}

static void fota_http_app_thread(void *argv)
{

    int ret = 0;
    QlOSStatus err = QL_OSI_SUCCESS;
    char version_buf[QL_VERSION_MAX]={0};
    fota_http_client_t  temp_cli;
    memset(&temp_cli,0,sizeof(temp_cli));
    fota_http_poweroutage_resume_get(&temp_cli);
    fota_http_semp = NULL;
    if(temp_cli.poweroutage_resume != true)
    {
        fota_http_cli.sim_id = 0;
        fota_http_cli.profile_idx = 1;
        fota_http_cli.http_progress.is_show = true;
        memcpy(fota_http_cli.fota_packname,"UFS:fota.pack",strlen("UFS:fota.pack") + 1);
    }
    else 
    {
        memcpy(&fota_http_cli,&temp_cli,sizeof(fota_http_client_t));
    } 
    ql_dev_get_firmware_version(version_buf, sizeof(version_buf));
    QL_FOTA_HTTP_LOG("current version:  %s", version_buf);
    ql_rtos_semaphore_create(&fota_http_semp, 0);
    if(fota_http_network_data_call(fota_http_cli.sim_id,fota_http_cli.profile_idx) != 0)
    {
        QL_FOTA_HTTP_LOG("fota http network datacall failed!");
        return ;
    }
    if(fota_http_cli.http_progress.is_show)
    {
        err = ql_rtos_task_create(&fota_http_show_task, 4096, APP_PRIORITY_REALTIME, "fota_http_show", http_show_task, NULL, 5);
        if (err != QL_OSI_SUCCESS)
        {
            QL_FOTA_HTTP_LOG("created http show progress task failed");
        }
    }
    while(1)
    {
        ret = fota_http_update_process();
        if (ret == 0)
        {
            QL_FOTA_HTTP_LOG("fota http update process end !");
            break;
        }
        QL_FOTA_HTTP_LOG("=========wait 20 s ===============>>>>>>");
        ql_rtos_task_sleep_s(20);
    }
    ql_rtos_semaphore_delete(fota_http_semp);
    ql_rtos_task_delete(fota_http_task);
    http_show_task_destroy();
}

#endif
#endif

void ql_fota_http_app_init(void)
{
#ifdef QL_APP_FEATURE_HTTP 
    QL_FOTA_HTTP_LOG("http fota demo support!");
    QlOSStatus err = QL_OSI_SUCCESS;
    err = ql_rtos_task_create(&fota_http_task, 4096*32, APP_PRIORITY_REALTIME, "fota_http_app", fota_http_app_thread, NULL, 5);
    if (err != QL_OSI_SUCCESS)
    {
        QL_FOTA_HTTP_LOG("created task failed");
    }   
#else
    QL_FOTA_HTTP_LOG("http fota demo not support!");
#endif
}
