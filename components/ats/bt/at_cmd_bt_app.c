#include "ats_config.h"
#include "stdio.h"
#include "cfw.h"
#include "at_cfg.h"
#include "sockets.h"
#include "unistd.h"
#include "osi_api.h"
#include "osi_log.h"
#include "at_response.h"
#include "at_command.h"
#include "at_engine.h"
#include "test/bt_at.h"

extern bt_eut_cmd_t bt_app_cmds[];

struct atCmdEngine *btapp_engine = NULL;
char *argv[BT_CMD_MAX_PARAM_NUM];
int argc;

/**
 * @name    bt_app_print_data
 * @describe it used to print data with blueu verify cmd
 * @param   buff: print buffer, it's must be ascii code
 * @param   len: length of buffer
 *
 * @return none
 */
void bt_app_print_data(char *buff, int len)
{
    OSI_LOGI(0, "[at_cmd_bt_app]buff:");
    OSI_LOGI(0, buff);

    if (NULL != btapp_engine)
    {
        atCmdRespInfoNText(btapp_engine, buff, strlen(buff));
    }
}

void atCmdHandle_BTAPP(atCommand_t *pParam)
{
    char at_rsp[200] = {0};
    int rsp_len = 0;
    int index = 0;
    int ret = 0;

    //get parameter data
    btapp_engine = pParam->engine;
    argc = pParam->param_count;
    for (index = 0; index < argc; index++)
    {
        argv[index] = pParam->params[index]->value;
    }

    //check sub command
    for (index = 0; bt_app_cmds[index].name != NULL; index++)
    {
        if (strcmp(bt_app_cmds[index].name, argv[0]) == 0)
        {
            ret = bt_app_cmds[index].handler(argv, argc, at_rsp, &rsp_len);
            if (ret)
            {
                goto err;
            }
            goto succ;
        }
    }

    if (bt_app_cmds[index].name == NULL)
    {
        OSI_LOGI(0, "please check you input string!\n");
        sprintf(at_rsp, "+SPBTTEST:ERR=command is invalide");
        goto err;
    }

succ:
    OSI_LOGI(0, "out %s() success!\n", __func__);
    atCmdRespOKText(pParam->engine, at_rsp);
    return;
err:
    OSI_LOGI(0, "out %s() error!\n", __func__);
    atCmdRespErrorText(pParam->engine, at_rsp);
    return;
}
