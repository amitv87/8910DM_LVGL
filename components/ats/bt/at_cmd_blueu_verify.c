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
#include "bluetooth/bt_types.h"

char *argv[BT_CMD_MAX_PARAM_NUM];
int argc;
struct atCmdEngine *at_engine;
static at_platform_cb_t at_cb;

/**
 * @name    bt_at_cmd_print_data
 * @describe it used to print data with blueu verify cmd
 * @param   buff: print buffer, it's must be ascii code
 * @param   len: length of buffer
 *
 * @return none
 */
void bt_verify_print_data(char *buff, int len)
{
#ifdef CONFIG_AT_BLUEU_VERIFY_SUPPORT
    OSI_LOGI(0, "[verify]buff:%s\n", buff);
    atCmdRespInfoNText(at_engine, buff, strlen(buff));
#endif
}

/**
 * @name    atCmdHandle_SPBTTEST
 * @describe it used to register the BT NPI test cmd
 * @param   pParam: parameter of at cmd
 *
 * @return none
 */
void atCmdHandle_SPBTTEST(atCommand_t *pParam)
{
#ifdef BLUEU_VERIFY_BT_NPI_EN
    char at_rsp[200] = {0};
    int rsp_len = 0;
    int index = 0;
    int ret = 0;
    char *query_char = "?";

    OSI_LOGI(0, "%s\n", __func__);

    //get parameter data
    at_engine = pParam->engine;
    memset(argv, 0, sizeof(argv));
    argc = pParam->param_count;
    for (index = 0; index < argc; index++)
    {
        argv[index] = pParam->params[index]->value;
    }

    //query '?' indicate the read command
    if (strstr(argv[0], query_char))
    {
        argv[0] = strtok(argv[0], query_char);
        argv[1] = query_char;
        argc = 2;

        OSI_LOGI(0, "argv[0]=%s,argv[0]=%s,argc=%d\n", argv[0], argv[1], argc);
    }

    //check sub command
    for (index = 0; bt_npi_cmds[index].name != NULL; index++)
    {
        if (strcmp(bt_npi_cmds[index].name, argv[0]) == 0)
        {
            ret = bt_npi_cmds[index].handler(argv, argc, at_rsp, &rsp_len);
            if (ret)
            {
                goto err;
            }
            goto succ;
        }
    }

    if (bt_npi_cmds[index].name == NULL)
    {
        OSI_LOGI(0, "please check you input string!\n");
        sprintf(at_rsp, "+SPBTTEST:ERR=command is invalide");
        goto err;
    }
succ:
    OSI_LOGI(0, "out %s() success!\n", __func__);
    atCmdRespInfoText(pParam->engine, at_rsp);
    AT_CMD_RETURN(atCmdRespOK(pParam->engine));
err:
    OSI_LOGI(0, "out %s() error!\n", __func__);
    atCmdRespInfoText(pParam->engine, at_rsp);
    AT_CMD_RETURN(atCmdRespError(pParam->engine));
#else //undef BLUEU_VERIFY_BT_NPI_EN
    OSI_LOGI(0, "out %s() undef command!\n", __func__);
    atCmdRespInfoText(pParam->engine, "undef command");
    AT_CMD_RETURN(atCmdRespError(pParam->engine));

#endif //BLUEU_VERIFY_BT_NPI_EN
}

/**
 * @name    atCmdHandle_SPBLETEST
 * @describe it used to register the BLE NPI test cmd
 * @param   pParam: parameter of at cmd
 *
 * @return none
 */
void atCmdHandle_SPBLETEST(atCommand_t *pParam)
{
#ifdef BLUEU_VERIFY_BLE_NPI_EN
    char at_rsp[200] = {0};
    int rsp_len = 0;
    int index = 0;
    int ret = 0;
    char *query_char = "?";

    OSI_LOGI(0, "%s\n", __func__);

    //get parameter data
    at_engine = pParam->engine;
    memset(argv, 0, sizeof(argv));
    argc = pParam->param_count;
    for (index = 0; index < argc; index++)
    {
        argv[index] = pParam->params[index]->value;
    }

    //query '?' indicate the read command
    if (strstr(argv[0], query_char))
    {
        argv[0] = strtok(argv[0], query_char);
        argv[1] = query_char;
        argc = 2;

        OSI_LOGI(0, "argv[0]=%s,argv[0]=%s,argc=%d\n", argv[0], argv[1], argc);
    }

    for (index = 0; ble_npi_cmds[index].name != NULL; index++)
    {
        if (strcmp(ble_npi_cmds[index].name, argv[0]) == 0)
        {
            ret = ble_npi_cmds[index].handler(argv, argc, at_rsp, &rsp_len);
            if (ret)
            {
                goto err;
            }
            goto succ;
        }
    }

    if (ble_npi_cmds[index].name == NULL)
    {
        OSI_LOGI(0, "please check you input string!\n");
        sprintf(at_rsp, "+SPBLETEST:ERR=command is invalide");
        goto err;
    }

succ:
    OSI_LOGI(0, "out %s() success!\n", __func__);
    atCmdRespInfoText(pParam->engine, at_rsp);
    AT_CMD_RETURN(atCmdRespOK(pParam->engine));
    return;
err:
    OSI_LOGI(0, "out %s() error!\n", __func__);
    atCmdRespInfoText(pParam->engine, at_rsp);
    AT_CMD_RETURN(atCmdRespError(pParam->engine));
    return;
#else //disable command
    OSI_LOGI(0, "out %s() undef command!\n", __func__);
    atCmdRespInfoText(pParam->engine, "undef command");
    AT_CMD_RETURN(atCmdRespError(pParam->engine));

#endif //BLUEU_VERIFY_BLE_NPI_EN
}

/**
 * @name    atCmdHandle_BTCOMM
 * @describe it used to register the bt common cmd
 * @param   pParam: parameter of at cmd
 *
 * @return none
 */
void atCmdHandle_BTCOMM(atCommand_t *pParam)
{
#ifdef BLUEU_VERIFY_BT_COMM_EN
    char at_rsp[200] = {0};
    int rsp_len = 0;
    int index = 0;
    int ret = 0;
    char *query_char = "?";

    OSI_LOGI(0, "%s\n", __func__);

    //set asynch output data to uart
    at_cb.at_print_data = bt_verify_print_data;
    at_set_platform_callback(&at_cb);
    at_engine = pParam->engine;

    //get parameter data
    memset(argv, 0, sizeof(argv));
    argc = pParam->param_count;
    for (index = 0; index < argc; index++)
    {
        argv[index] = pParam->params[index]->value;
    }

    //query '?' indicate the read command
    if (strstr(argv[0], query_char))
    {
        argv[0] = strtok(argv[0], query_char);
        argv[1] = query_char;
        argc = 2;

        OSI_LOGI(0, "argv[0]=%s,argv[0]=%s,argc=%d\n", argv[0], argv[1], argc);
    }

    for (index = 0; bt_comm_cmds[index].name != NULL; index++)
    {
        if (strcmp(bt_comm_cmds[index].name, argv[0]) == 0)
        {
            ret = bt_comm_cmds[index].handler(argv, argc, at_rsp, &rsp_len);
            if (ret)
            {
                goto err;
            }
            goto succ;
        }
    }

    if (bt_comm_cmds[index].name == NULL)
    {
        OSI_LOGI(0, "please check you input string!\n");
        sprintf(at_rsp, "+BTCOMM:ERR=command is invalide");
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
#else //disable command
    OSI_LOGI(0, "out %s() undef command!\n", __func__);
    atCmdRespInfoText(pParam->engine, "undef command");
    AT_CMD_RETURN(atCmdRespError(pParam->engine));
    return;

#endif //BLUEU_VERIFY_BT_COMM_EN
}

/**
 * @name    atCmdHandle_BTHF
 * @describe it used to register the bt HF of HFP protocal cmd
 * @param   pParam: parameter of at cmd
 *
 * @return none
 */
void atCmdHandle_BTHF(atCommand_t *pParam)
{
#ifdef BLUEU_VERIFY_BT_HFP_HF_EN
    char at_rsp[200] = {0};
    int rsp_len = 0;
    int index = 0;
    int ret = 0;
    char *query_char = "?";

    OSI_LOGI(0, "%s\n", __func__);

    //set asynch output data to uart
    at_cb.at_print_data = bt_verify_print_data;
    at_set_platform_callback(&at_cb);
    at_engine = pParam->engine;

    //get parameter data
    memset(argv, 0, sizeof(argv));
    argc = pParam->param_count;
    for (index = 0; index < argc; index++)
    {
        argv[index] = pParam->params[index]->value;
    }

    //query '?' indicate the read command
    if (strstr(argv[0], query_char))
    {
        argv[0] = strtok(argv[0], query_char);
        argv[1] = query_char;
        argc = 2;

        OSI_LOGI(0, "argv[0]=%s,argv[0]=%s,argc=%d\n", argv[0], argv[1], argc);
    }

    for (index = 0; bt_hfp_hf_cmds[index].name != NULL; index++)
    {
        if (strcmp(bt_hfp_hf_cmds[index].name, argv[0]) == 0)
        {
            ret = bt_hfp_hf_cmds[index].handler(argv, argc, at_rsp, &rsp_len);
            if (ret)
            {
                goto err;
            }
            goto succ;
        }
    }

    if (bt_hfp_hf_cmds[index].name == NULL)
    {
        OSI_LOGI(0, "please check you input string!\n");
        sprintf(at_rsp, "+BTHF:ERR=command is invalide");
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
#else //disable command
    OSI_LOGI(0, "out %s() undef command!\n", __func__);
    atCmdRespInfoText(pParam->engine, "undef command");
    AT_CMD_RETURN(atCmdRespError(pParam->engine));
    return;

#endif //BLUEU_VERIFY_BT_HFP_HF_EN
}

/**
 * @name    atCmdHandle_BTAG
 * @describe it used to register the bt AG mode of HFP protocal cmd
 * @param   pParam: parameter of at cmd
 *
 * @return none
 */
void atCmdHandle_BTAG(atCommand_t *pParam)
{
#ifdef BLUEU_VERIFY_BT_HFP_AG_EN
    char at_rsp[200] = {0};
    int rsp_len = 0;
    int index = 0;
    int ret = 0;
    char *query_char = "?";

    OSI_LOGI(0, "%s\n", __func__);

    //set asynch output data to uart
    at_cb.at_print_data = bt_verify_print_data;
    at_set_platform_callback(&at_cb);
    at_engine = pParam->engine;

    //get parameter data
    memset(argv, 0, sizeof(argv));
    argc = pParam->param_count;
    for (index = 0; index < argc; index++)
    {
        argv[index] = pParam->params[index]->value;
    }

    //query '?' indicate the read command
    if (strstr(argv[0], query_char))
    {
        argv[0] = strtok(argv[0], query_char);
        argv[1] = query_char;
        argc = 2;

        OSI_LOGI(0, "argv[0]=%s,argv[0]=%s,argc=%d\n", argv[0], argv[1], argc);
    }

    for (index = 0; bt_hfp_ag_cmds[index].name != NULL; index++)
    {
        if (strcmp(bt_hfp_ag_cmds[index].name, argv[0]) == 0)
        {
            ret = bt_hfp_ag_cmds[index].handler(argv, argc, at_rsp, &rsp_len);
            if (ret)
            {
                goto err;
            }
            goto succ;
        }
    }

    if (bt_hfp_ag_cmds[index].name == NULL)
    {
        OSI_LOGI(0, "please check you input string!\n");
        sprintf(at_rsp, "+BTAG:ERR=command is invalide");
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
#else //disable command
    OSI_LOGI(0, "out %s() undef command!\n", __func__);
    atCmdRespInfoText(pParam->engine, "undef command");
    AT_CMD_RETURN(atCmdRespError(pParam->engine));
    return;

#endif //BLUEU_VERIFY_BT_HFP_AG_EN
}

/**
 * @name    atCmdHandle_BTSINK
 * @describe it used to register the bt sink mode of a2dp protocal cmd
 * @param   pParam: parameter of at cmd
 *
 * @return none
 */
void atCmdHandle_BTSINK(atCommand_t *pParam)
{
#ifdef BLUEU_VERIFY_BT_A2DP_SINK_EN
    char at_rsp[200] = {0};
    int rsp_len = 0;
    int index = 0;
    int ret = 0;
    char *query_char = "?";

    OSI_LOGI(0, "%s\n", __func__);

    //set asynch output data to uart
    at_cb.at_print_data = bt_verify_print_data;
    at_set_platform_callback(&at_cb);
    at_engine = pParam->engine;

    //get parameter data
    memset(argv, 0, sizeof(argv));
    argc = pParam->param_count;
    for (index = 0; index < argc; index++)
    {
        argv[index] = pParam->params[index]->value;
    }

    //query '?' indicate the read command
    if (strstr(argv[0], query_char))
    {
        argv[0] = strtok(argv[0], query_char);
        argv[1] = query_char;
        argc = 2;

        OSI_LOGI(0, "argv[0]=%s,argv[0]=%s,argc=%d\n", argv[0], argv[1], argc);
    }

    for (index = 0; bt_a2dp_sink_cmds[index].name != NULL; index++)
    {
        if (strcmp(bt_a2dp_sink_cmds[index].name, argv[0]) == 0)
        {
            ret = bt_a2dp_sink_cmds[index].handler(argv, argc, at_rsp, &rsp_len);
            if (ret)
            {
                goto err;
            }
            goto succ;
        }
    }

    if (bt_a2dp_sink_cmds[index].name == NULL)
    {
        OSI_LOGI(0, "please check you input string!\n");
        sprintf(at_rsp, "+BTSINK:ERR=command is invalide");
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
#else //disable command
    OSI_LOGI(0, "out %s() undef command!\n", __func__);
    atCmdRespInfoText(pParam->engine, "undef command");
    AT_CMD_RETURN(atCmdRespError(pParam->engine));
    return;

#endif //BLUEU_VERIFY_BT_A2DP_SINK_EN
}

/**
 * @name    atCmdHandle_BTSRC
 * @describe it used to register the bt src mode of a2dp protocal cmd
 * @param   pParam: parameter of at cmd
 *
 * @return none
 */
void atCmdHandle_BTSRC(atCommand_t *pParam)
{
#ifdef BLUEU_VERIFY_BT_A2DP_SRC_EN
    char at_rsp[200] = {0};
    int rsp_len = 0;
    int index = 0;
    int ret = 0;
    char *query_char = "?";

    OSI_LOGI(0, "%s\n", __func__);

    //set asynch output data to uart
    at_cb.at_print_data = bt_verify_print_data;
    at_set_platform_callback(&at_cb);
    at_engine = pParam->engine;

    //get parameter data
    memset(argv, 0, sizeof(argv));
    argc = pParam->param_count;
    for (index = 0; index < argc; index++)
    {
        argv[index] = pParam->params[index]->value;
    }

    //query '?' indicate the read command
    if (strstr(argv[0], query_char))
    {
        argv[0] = strtok(argv[0], query_char);
        argv[1] = query_char;
        argc = 2;

        OSI_LOGI(0, "argv[0]=%s,argv[0]=%s,argc=%d\n", argv[0], argv[1], argc);
    }

    for (index = 0; bt_a2dp_src_cmds[index].name != NULL; index++)
    {
        if (strcmp(bt_a2dp_src_cmds[index].name, argv[0]) == 0)
        {
            ret = bt_a2dp_src_cmds[index].handler(argv, argc, at_rsp, &rsp_len);
            if (ret)
            {
                goto err;
            }
            goto succ;
        }
    }

    if (bt_a2dp_src_cmds[index].name == NULL)
    {
        OSI_LOGI(0, "please check you input string!\n");
        sprintf(at_rsp, "+BTSRC:ERR=command is invalide");
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
#else //disable command
    OSI_LOGI(0, "out %s() undef command!\n", __func__);
    atCmdRespInfoText(pParam->engine, "undef command");
    AT_CMD_RETURN(atCmdRespError(pParam->engine));
    return;

#endif //BLUEU_VERIFY_BT_A2DP_SRC_EN
}

/**
 * @name    atCmdHandle_BTSPP
 * @describe it used to register the bt SPP protocal cmd
 * @param   pParam: parameter of at cmd
 *
 * @return none
 */
void atCmdHandle_BTSPP(atCommand_t *pParam)
{
#ifdef BLUEU_VERIFY_BT_SPP_EN
    char at_rsp[200] = {0};
    int rsp_len = 0;
    int index = 0;
    int ret = 0;
    char *query_char = "?";

    OSI_LOGI(0, "%s\n", __func__);

    //set asynch output data to uart
    at_cb.at_print_data = bt_verify_print_data;
    at_set_platform_callback(&at_cb);
    at_engine = pParam->engine;

    //get parameter data
    memset(argv, 0, sizeof(argv));
    argc = pParam->param_count;
    for (index = 0; index < argc; index++)
    {
        argv[index] = pParam->params[index]->value;
    }

    //query '?' indicate the read command
    if (strstr(argv[0], query_char))
    {
        argv[0] = strtok(argv[0], query_char);
        argv[1] = query_char;
        argc = 2;

        OSI_LOGI(0, "argv[0]=%s,argv[0]=%s,argc=%d\n", argv[0], argv[1], argc);
    }

    for (index = 0; bt_spp_cmds[index].name != NULL; index++)
    {
        if (strcmp(bt_spp_cmds[index].name, argv[0]) == 0)
        {
            ret = bt_spp_cmds[index].handler(argv, argc, at_rsp, &rsp_len);
            if (ret)
            {
                goto err;
            }
            goto succ;
        }
    }

    if (bt_spp_cmds[index].name == NULL)
    {
        OSI_LOGI(0, "please check you input string!\n");
        sprintf(at_rsp, "+BTSPP:ERR=command is invalide");
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
#else //disable command
    OSI_LOGI(0, "out %s() undef command!\n", __func__);
    atCmdRespInfoText(pParam->engine, "undef command");
    AT_CMD_RETURN(atCmdRespError(pParam->engine));
    return;

#endif //BLUEU_VERIFY_BT_SPP_EN
}

/**
 * @name    atCmdHandle_BLECOMM
 * @describe it used to register the ble commonl cmd
 * @param   pParam: parameter of at cmd
 *
 * @return none
 */
void atCmdHandle_BLECOMM(atCommand_t *pParam)
{
#ifdef BLUEU_VERIFY_BLE_NORMAL_EN
    char at_rsp[200] = {0};
    int rsp_len = 0;
    int index = 0;
    int ret = 0;
    char *query_char = "?";

    OSI_LOGI(0, "%s\n", __func__);

    //set asynch output data to uart
    at_cb.at_print_data = bt_verify_print_data;
    at_set_platform_callback(&at_cb);
    at_engine = pParam->engine;

    //get parameter data
    memset(argv, 0, sizeof(argv));
    argc = pParam->param_count;
    for (index = 0; index < argc; index++)
    {
        argv[index] = pParam->params[index]->value;
    }

    //query '?' indicate the read command
    if (strstr(argv[0], query_char))
    {
        argv[0] = strtok(argv[0], query_char);
        argv[1] = query_char;
        argc = 2;

        OSI_LOGI(0, "argv[0]=%s,argv[0]=%s,argc=%d\n", argv[0], argv[1], argc);
    }

    for (index = 0; ble_comm_cmds[index].name != NULL; index++)
    {
        if (strcmp(ble_comm_cmds[index].name, argv[0]) == 0)
        {
            ret = ble_comm_cmds[index].handler(argv, argc, at_rsp, &rsp_len);
            if (ret)
            {
                goto err;
            }
            goto succ;
        }
    }

    if (ble_comm_cmds[index].name == NULL)
    {
        OSI_LOGI(0, "please check you input string!\n");
        sprintf(at_rsp, "+BLECOMM:ERR=command is invalide");
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
#else  //disable command
    OSI_LOGI(0, "out %s() undef command!\n", __func__);
    atCmdRespInfoText(pParam->engine, "undef command");
    AT_CMD_RETURN(atCmdRespError(pParam->engine));
    return;
#endif //BLUEU_VERIFY_BLE_NORMAL_EN
}

/**
 * @name    atCmdHandle_BLEADV
 * @describe it used to register the ble adv cmd
 * @param   pParam: parameter of at cmd
 *
 * @return none
 */
void atCmdHandle_BLEADV(atCommand_t *pParam)
{
#ifdef BLUEU_VERIFY_BLE_NORMAL_EN
    char at_rsp[200] = {0};
    int rsp_len = 0;
    int index = 0;
    int ret = 0;
    char *query_char = "?";

    OSI_LOGI(0, "%s\n", __func__);

    //set asynch output data to uart
    at_cb.at_print_data = bt_verify_print_data;
    at_set_platform_callback(&at_cb);
    at_engine = pParam->engine;

    //get parameter data
    memset(argv, 0, sizeof(argv));
    argc = pParam->param_count;
    for (index = 0; index < argc; index++)
    {
        argv[index] = pParam->params[index]->value;
    }

    //query '?' indicate the read command
    if (strstr(argv[0], query_char))
    {
        argv[0] = strtok(argv[0], query_char);
        argv[1] = query_char;
        argc = 2;

        OSI_LOGI(0, "argv[0]=%s,argv[0]=%s,argc=%d\n", argv[0], argv[1], argc);
    }

    for (index = 0; ble_adv_cmds[index].name != NULL; index++)
    {
        if (strcmp(ble_adv_cmds[index].name, argv[0]) == 0)
        {
            ret = ble_adv_cmds[index].handler(argv, argc, at_rsp, &rsp_len);
            if (ret)
            {
                goto err;
            }
            goto succ;
        }
    }

    if (ble_adv_cmds[index].name == NULL)
    {
        OSI_LOGI(0, "please check you input string!\n");
        sprintf(at_rsp, "+BLEADV:ERR=command is invalide");
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
#else //disable command
    OSI_LOGI(0, "out %s() undef command!\n", __func__);
    atCmdRespInfoText(pParam->engine, "undef command");
    AT_CMD_RETURN(atCmdRespError(pParam->engine));
    return;

#endif //BLUEU_VERIFY_BLE_NORMAL_EN
}

/**
 * @name    atCmdHandle_BLESCAN
 * @describe it used to register the ble scan cmd
 * @param   pParam: parameter of at cmd
 *
 * @return none
 */
void atCmdHandle_BLESCAN(atCommand_t *pParam)
{
#ifdef BLUEU_VERIFY_BLE_NORMAL_EN
    char at_rsp[200] = {0};
    int rsp_len = 0;
    int index = 0;
    int ret = 0;
    char *query_char = "?";

    OSI_LOGI(0, "%s\n", __func__);

    //set asynch output data to uart
    at_cb.at_print_data = bt_verify_print_data;
    at_set_platform_callback(&at_cb);
    at_engine = pParam->engine;

    //get parameter data
    memset(argv, 0, sizeof(argv));
    argc = pParam->param_count;
    for (index = 0; index < argc; index++)
    {
        argv[index] = pParam->params[index]->value;
    }

    //query '?' indicate the read command
    if (strstr(argv[0], query_char))
    {
        argv[0] = strtok(argv[0], query_char);
        argv[1] = query_char;
        argc = 2;

        OSI_LOGI(0, "argv[0]=%s,argv[0]=%s,argc=%d\n", argv[0], argv[1], argc);
    }

    for (index = 0; ble_scan_cmds[index].name != NULL; index++)
    {
        if (strcmp(ble_scan_cmds[index].name, argv[0]) == 0)
        {
            ret = ble_scan_cmds[index].handler(argv, argc, at_rsp, &rsp_len);
            if (ret)
            {
                goto err;
            }
            goto succ;
        }
    }

    if (ble_scan_cmds[index].name == NULL)
    {
        OSI_LOGI(0, "please check you input string!\n");
        sprintf(at_rsp, "+BLESCAN:ERR=command is invalide");
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
#else //disable command
    OSI_LOGI(0, "out %s() undef command!\n", __func__);
    atCmdRespInfoText(pParam->engine, "undef command");
    AT_CMD_RETURN(atCmdRespError(pParam->engine));
    return;

#endif //BLUEU_VERIFY_BLE_NORMAL_EN
}

/**
 * @name    atCmdHandle_BLESMP
 * @describe it used to register the ble smp cmd
 * @param   pParam: parameter of at cmd
 *
 * @return none
 */
void atCmdHandle_BLESMP(atCommand_t *pParam)
{
#ifdef BLUEU_VERIFY_BLE_NORMAL_EN
    char at_rsp[200] = {0};
    int rsp_len = 0;
    int index = 0;
    int ret = 0;
    char *query_char = "?";

    OSI_LOGI(0, "%s\n", __func__);

    //set asynch output data to uart
    at_cb.at_print_data = bt_verify_print_data;
    at_set_platform_callback(&at_cb);
    at_engine = pParam->engine;

    //get parameter data
    memset(argv, 0, sizeof(argv));
    argc = pParam->param_count;
    for (index = 0; index < argc; index++)
    {
        argv[index] = pParam->params[index]->value;
    }

    //query '?' indicate the read command
    if (strstr(argv[0], query_char))
    {
        argv[0] = strtok(argv[0], query_char);
        argv[1] = query_char;
        argc = 2;

        OSI_LOGI(0, "argv[0]=%s,argv[0]=%s,argc=%d\n", argv[0], argv[1], argc);
    }

    for (index = 0; ble_smp_cmds[index].name != NULL; index++)
    {
        if (strcmp(ble_smp_cmds[index].name, argv[0]) == 0)
        {
            ret = ble_smp_cmds[index].handler(argv, argc, at_rsp, &rsp_len);
            if (ret)
            {
                goto err;
            }
            goto succ;
        }
    }

    if (ble_smp_cmds[index].name == NULL)
    {
        OSI_LOGI(0, "please check you input string!\n");
        sprintf(at_rsp, "+BLESMP:ERR=command is invalide");
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
#else //disable command
    OSI_LOGI(0, "out %s() undef command!\n", __func__);
    atCmdRespInfoText(pParam->engine, "undef command");
    AT_CMD_RETURN(atCmdRespError(pParam->engine));
    return;

#endif //BLUEU_VERIFY_BLE_NORMAL_EN
}

/*---------Controller BQB TEST---------*/

#define CHAR_BACKSPACE 8
#define AT_CMD_LINE_BUFF_LEN 1024 * 5
extern void app_npi_bt_start(void);
extern void app_npi_bt_stop(void);
extern void UART_SetControllerBqbMode(BOOL is_ctrlr_bqb_mode);
extern uint16_t UartDrv_Tx(const uint8_t *buffer, uint16_t length);

typedef struct
{
    uint8_t *buff;     // buffer, shared by all mode
    uint32_t buffLen;  // existed buffer data byte count
    uint32_t buffSize; // buffer size
    osiMutex_t *mutex;
} BYPASS_BUFFER_T;

static BYPASS_BUFFER_T *g_bypass_buf = NULL;
static atCmdEngine_t *g_pstCmdEngine = NULL;

/**
 * @brief   create bypass buffer
 * @describe it will create bypass buffer
 * @param void
 * @return NULL ---null buffer
 *             bypass_buff
 */
BYPASS_BUFFER_T *at_DataBufCreate()
{
    BYPASS_BUFFER_T *bypass_buff = (BYPASS_BUFFER_T *)malloc(sizeof(*bypass_buff));
    if (bypass_buff == NULL)
    {
        return NULL;
    }
    bypass_buff->buff = (uint8_t *)malloc(AT_CMD_LINE_BUFF_LEN);
    if (bypass_buff->buff == NULL)
    {
        free(bypass_buff);
        return NULL;
    }
    bypass_buff->buffLen = 0;
    bypass_buff->buffSize = AT_CMD_LINE_BUFF_LEN;
    bypass_buff->mutex = osiMutexCreate();
    return bypass_buff;
}

/**
 * @brief   destroy bypass buffer
 * @describe it will destroy bypass buffer
 * @param bypass_buff
 * @return void
 */
void at_DataBufDestroy(BYPASS_BUFFER_T *bypass_buff)
{
    if (bypass_buff == NULL)
    {
        return;
    }
    osiMutexDelete(bypass_buff->mutex);
    free(bypass_buff->buff);
    free(bypass_buff);
}

extern void SET_RE_FLAG(int flag);
extern int GET_RE_FLAG();
//Data: UART --> BT
/**
 * @brief   BT receive data from uart callback
 * @describe it will receive data from uart callback
 * @param param---parameter, here is not use
 *             data---input data
 *             length--- length of input data
 *
 * @return void
 */
static int _RecvBDataformUARTCb(void *param, const void *data, size_t length)
{
    uint8_t *data_u8 = (uint8_t *)data;
    size_t length_input = length;
    OSI_LOGI(0x10003f2d, "bypassDataRecv,length=%d ", length);
    if (g_bypass_buf == NULL)
    {
        g_bypass_buf = at_DataBufCreate();
    }
    if (g_bypass_buf == NULL)
    {
        return 0;
    }
    osiMutexLock(g_bypass_buf->mutex);
    while (length > 0)
    {
        uint8_t c = *data_u8++;
        length--;
        // Drop whole buffer at overflow, maybe caused by data error
        // or buffer too small
        if (g_bypass_buf->buffLen >= g_bypass_buf->buffSize)
        {
            OSI_LOGI(0, "bypassDataRecv bypass mode overflow, drop all");
            g_bypass_buf->buffLen = 0;
        }
        g_bypass_buf->buff[g_bypass_buf->buffLen++] = c;
    }
    osiMutexUnlock(g_bypass_buf->mutex);
    SET_RE_FLAG(1);
    UartDrv_Tx(g_bypass_buf->buff, length_input);
    g_bypass_buf->buffLen = 0;

    return length_input;
}

static void writeInfoNText(atCmdEngine_t *engine, const char *text, unsigned length)
{
    atDispatch_t *dispatch = atCmdGetDispatch(engine);
    if (atDispatchIsDataMode(dispatch) && atDispatchGetDataEngine(dispatch) != NULL)
    {
        atDataWrite(atDispatchGetDataEngine(dispatch), text, length);
    }
    else
    {
        atCmdRespInfoNText(engine, text, length);
    }
}

//Data: BT --> UART
/**
 * @brief   UART receive data from BT callback
 * @describe it will receive data from BT callback
 * @param buf---buffer
 *             length--- length of buffer
 *
 * @return void
 */
void _RecvBDataformBTCb(char *buf, unsigned int length)
{
    //ToDo:

    if (NULL != g_pstCmdEngine)
    {
        writeInfoNText(g_pstCmdEngine, buf, length);
    }

    return;
}
extern void (*BT_UART_FUCTION)(char *, unsigned int);

void BtTOUARTSetCallback(void (*RecvBDataTOUART)(char *, unsigned int))
{
    BT_UART_FUCTION = RecvBDataTOUART;
}

/**
 * @brief   start transpant mode
 * @describe it will sart transparent mode
 * @param engine
 * @return none
 */
static void start_transparent_mode(atCmdEngine_t *engine)
{
    atDispatch_t *dispatch = atCmdGetDispatch(engine);
    atDispatchSetModeSwitchHandler(dispatch, NULL); //ATModeSwitchHandler
    atEngineModeSwitch(AT_MODE_SWITCH_DATA_START, dispatch);
    atDataEngine_t *dataEngine = atDispatchGetDataEngine(dispatch);
    //set callback, from UART to BT
    atDataSetBypassMode(dataEngine, _RecvBDataformUARTCb, (void *)engine);
    //set callback, from BT to UART
    //ToDo:
    BtTOUARTSetCallback(_RecvBDataformBTCb);
    UART_SetControllerBqbMode(TRUE);
}

/**
 * @brief   stop transpant mode
 * @describe it will stop transparent mode
 * @param engine
 * @return none
 */
static void stop_transparent_mode(atCmdEngine_t *engine)
{
    atDispatch_t *dispatch = atCmdGetDispatch(engine);
    if (atDispatchIsDataMode(dispatch))
    {
        atDispatchSetModeSwitchHandler(dispatch, NULL); //ATModeSwitchHandler
        atEngineModeSwitch(AT_MODE_SWITCH_DATA_END, dispatch);
    }
    at_DataBufDestroy(g_bypass_buf);
    g_bypass_buf = NULL;
    BtTOUARTSetCallback(NULL);
    UART_SetControllerBqbMode(FALSE);
}

/**
 * @name    atCmdHandle_SPBQBTEST
 * @describe it used to finish the BT BQB test
 * @param   pParam: parameter of at cmd
 *
 * @return none
 */
void atCmdHandle_SPBQBTEST(atCommand_t *pParam)
{
    int bqb_en = -1;
    bool bParamRet = true;
    char cDebugBuf[64] = {'\0'};
    static int bqb_test_mode = 0; //bqb_test_mode=0 not in bqb mode  bqb_test_mode=1 in bqb mode
    switch (pParam->type)
    {
    case AT_CMD_SET:
    {
        bqb_en = atParamUintInRange(pParam->params[0], 0, 1, &bParamRet);
        if (false == bParamRet)
        {
            cDebugBuf[0] = '\0';
            sprintf(cDebugBuf, "%s(%d):Param[0] error.", __func__, __LINE__);
            OSI_LOGI(0, cDebugBuf);
            RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
        }
        if (1 == bqb_en)
        {
            if (0 == bqb_test_mode)
            {
                app_npi_bt_start();
                osiThreadSleep(1500); //wait BT init
                bqb_test_mode = 1;
            }
            else
            {
                OSI_LOGI(0, "BT is already in bqb mode!");
            }
            atCmdRespInfoText(pParam->engine, "OK"); //resopnse "OK" in com tool
            osiThreadSleep(300);
            g_pstCmdEngine = pParam->engine;
            start_transparent_mode(pParam->engine);
        }
        else if (0 == bqb_en)
        {
            OSI_LOGI(0, "BT BQB stop transparent mode!");
            stop_transparent_mode(pParam->engine);
            g_pstCmdEngine = NULL;
            if (1 == bqb_test_mode)
            {
                app_npi_bt_stop();
                osiThreadSleep(1500); //wait BT init
                bqb_test_mode = 0;
            }
            else
            {
                OSI_LOGI(0, "BT is not in bqb mode");
            }
            RETURN_OK(pParam->engine);
        }
        break;
    }
    default:
    {
        cDebugBuf[0] = '\0';
        sprintf(cDebugBuf, "%s(%d):exe not support.", __func__, __LINE__);
        OSI_LOGI(0, cDebugBuf);
        RETURN_CME_ERR(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
    }
}
