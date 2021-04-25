
#include "ats_config.h"
#include "at_cfw.h"
#include "at_command.h"
#include "at_engine.h"
#include "at_response.h"
#include "osi_log.h"

#include <ctype.h>
#include "cfw_chset.h"
#include "drv_efuse.h"
#include "nvm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vfs.h>

#ifdef CONFIG_AT_SSIM_SUPPORT

#include "at_cmd_ssim.h"

static uint8_t g_flag = 0;

static osiTimer_t *g_tmrID;

int _isSoftSimImeiValid(const char *nImei, int mLength)
{
#if 0
    int i = 0;
    int even = 1;
    int sum = 0;
    int double_digit;
    uint8_t p;
#endif
    if (!_isSoftSimDigit(nImei, mLength))
        return -1;

    return 0;
#if 0
    for (i = mLength - 2; i >= 0; i--)
    {
        p = nImei[i] - '0';
        if (even)
        {
            double_digit = p * 2;
            sum += double_digit / 10 + double_digit % 10;
        }
        else
        {
            sum += p;
        }

        even = !even;
    }

    int result = 0;
    result = sum * 9 % 10;
    OSI_LOGI(0, "_isSoftSimImeiValid result: %d", result);
    if (result == -1)
        return -1;
    else
        return result;
#endif
}

bool _isSoftSimImsiValid(const char *nImsi, int nLength)
{
    return _isSoftSimDigit(nImsi, nLength);
}

// Save soft sim info to local
bool _saveSoftSimInfoToLocal(SSIM_FILE_INFO_T ssim_file_info)
{
    unsigned char *tempCode = (unsigned char *)&(ssim_file_info.sim_amrc_stru);
    unsigned char temp[33] = {0};
    // generate hash code
    int ret = mbedtls_sha256_ret((const unsigned char *)tempCode, strlen((char *)tempCode), temp, 0);
    memcpy(ssim_file_info.hashCode, temp, 32);

    ret = _saveFileData(SSIM_FILE_PATH, &ssim_file_info, sizeof(SSIM_FILE_INFO_T));
    if (ret == -1)
    {
        OSI_LOGE(0, "atCmdHandleSSIMINFO AT_CMD_SET save sim file fail");
        return false;
    }
    return true;
}

// Save soft sim info to stack
void _saveSoftSimInfoToStack(SSIM_FILE_INFO_T ssim_file_info, uint8_t nSimID)
{
    CFW_SSIM_FILE_INFO_T cfw_ssim_file_info = {0};
    cfw_ssim_file_info.mncLen = ssim_file_info.mncLen;
    memcpy(cfw_ssim_file_info.imsi, ssim_file_info.imsi, SSIM_IMSI_LEN);
    memcpy(&cfw_ssim_file_info.usim_soft_auth_param_stru, &ssim_file_info.soft_auth_param_stru, sizeof(SOFT_AUTH_PARAM_STRU));
    memcpy(&cfw_ssim_file_info.auth_opc, &ssim_file_info.auth_opc, sizeof(USIM_SOFT_OPC_PARAM_STRU));
    memcpy(&cfw_ssim_file_info.sim_amrc_stru, &ssim_file_info.sim_amrc_stru, sizeof(SIM_AMRC_STRU));

    uint32_t ret = CFW_SaveSsimFileInfo(cfw_ssim_file_info, nSimID);
    OSI_LOGI(0, "_saveSoftSimInfoToStack ret: %d", ret);
}

// Save soft sim info
bool _saveSoftSimInfo(SSIM_FILE_INFO_T ssim_file_info, uint8_t nSimID)
{

    if (!_saveSoftSimInfoToLocal(ssim_file_info))
    {
        return false;
    }

    _saveSoftSimInfoToStack(ssim_file_info, nSimID);
    return true;
}

// Soft sim file info default value
void _generateSimFileInitValue(SSIM_FILE_INFO_T *dest)
{
    SSIM_FILE_INFO_T ssim_file_info = {0};

    ssim_file_info.sim_amrc_stru.R1 = 0x40;
    ssim_file_info.sim_amrc_stru.R2 = 0x00;
    ssim_file_info.sim_amrc_stru.R3 = 0x20;
    ssim_file_info.sim_amrc_stru.R4 = 0x40;
    ssim_file_info.sim_amrc_stru.R5 = 0x60;

    ssim_file_info.auth_opc.bitOpRsv = 31;
    ssim_file_info.auth_opc.bitOpcSoftUsimEnable = 0;

    const char *key = "29CB11D3436EBEE37DF2C5D25E8AED52";
    const char *op = "00000000000000000000000000000000";
    const char *opc = "00000000000000000000000000000000";
    const char *c1 = "00000000000000000000000000000000";
    const char *c2 = "00000000000000000000000000000001";
    const char *c3 = "00000000000000000000000000000002";
    const char *c4 = "00000000000000000000000000000004";
    const char *c5 = "00000000000000000000000000000008";

    if (PARSE_HEX_DATA)
    {
        return;
    }

    memcpy(dest, &ssim_file_info, sizeof(SSIM_FILE_INFO_T));
}

// create default auth info begin
bool _generateDefaultAuthInfo(AUTH_CONTEXT_T *authContext, uint8_t nSimID)
{
    // create ssim directory
    vfs_mkpath(SSIM_DIRECROTY, 0);

    // get local imei
    uint8_t nImei[16] = {
        0,
    };
    int nImeiLen = nvmReadImei(nSimID, (nvmImei_t *)&nImei[0]);
    OSI_LOGI(0, "_generateDefaultAuthInfo nImeiLen: %d", nImeiLen);
    if (nImeiLen != -1)
    {
        char tmpPwd[13] = {0x00};
        memcpy(tmpPwd, "IMEI", 4);
        memcpy(tmpPwd + 4, nImei, 4);
        memcpy(tmpPwd + 8, nImei + 11, 4);

        int ret;
        unsigned char outputAuthcode[AUTH_CODE_LENGTH + 1] = {0};

        ret = _generateSha256AuthCode((unsigned char *)tmpPwd, outputAuthcode);
        OSI_LOGI(0, "_generateDefaultAuthInfo ret: %d", ret);

        memcpy(authContext->password, tmpPwd, PASSWORD_LENGTH);
        memcpy(authContext->imei, nImei, 15);
        memcpy(authContext->authValue, outputAuthcode, AUTH_CODE_LENGTH);
        vfs_file_write(AUTH_FILE_PATH, authContext, sizeof(AUTH_CONTEXT_T));
        OSI_LOGI(0, "_generateDefaultAuthInfo create default auth info success");
    }
    else
    {
        OSI_LOGE(0, "_generateDefaultAuthInfo no local imei");
        return false;
    }
    return true;
}

// for CP call AP, generate default sim info
bool _generateSoftSimDefaultInfo(CFW_SSIM_FILE_INFO_T *cfw_ssim_file_info, CFW_SIM_ID nSimID)
{
    OSI_LOGI(0, "_generateSoftSimDefaultInfo");

    // make sure ssim directory exist
    vfs_mkpath(SSIM_DIRECROTY, 0);

    // create default auth info
    AUTH_CONTEXT_T authContext = {0};
    _generateDefaultAuthInfo(&authContext, nSimID);

    SSIM_FILE_INFO_T ssim_file_info = {0};
    _generateSimFileInitValue(&ssim_file_info);
    if (!_saveSoftSimInfoToLocal(ssim_file_info))
    {
        OSI_LOGE(0, "_generateSoftSimDefaultInfo save soft sim info fail");
    }

    memcpy(&cfw_ssim_file_info->usim_soft_auth_param_stru, &ssim_file_info.soft_auth_param_stru, sizeof(SOFT_AUTH_PARAM_STRU));
    memcpy(&cfw_ssim_file_info->sim_amrc_stru, &ssim_file_info.sim_amrc_stru, sizeof(SIM_AMRC_STRU));
    memcpy(&cfw_ssim_file_info->auth_opc, &ssim_file_info.auth_opc, sizeof(USIM_SOFT_OPC_PARAM_STRU));

    return true;
}

void _generateSoftSimDefaultPlmnFile()
{
    const uint8_t plmn[7] = {'4', '6', '0', '0', '6', '0'};

    SSIM_PLMN_T ssimPlmn;
    ssimPlmn.mncLen = 3;
    memcpy(ssimPlmn.plmn, plmn, SOFT_SIM_LENGTH_6);

    _saveFileData(SSIM_PLMN_PATH, &ssimPlmn, sizeof(SSIM_PLMN_T));
}

bool _generateSoftSimImsiInfo(SSIM_FILE_INFO_T *ssim_file_info, uint8_t nSimId)
{
    // get saved plmn
    SSIM_PLMN_T ssimPlmn = {0};
    _getFileData(SSIM_PLMN_PATH, &ssimPlmn, sizeof(SSIM_PLMN_T));

    uint8_t tempPlmn[PLMN_LENGTH] = {0x00};
    if (ssimPlmn.mncLen == 2)
    {
        memcpy(tempPlmn, ssimPlmn.plmn, SOFT_SIM_LENGTH_5);
        tempPlmn[5] = '0';
    }
    else
    {
        memcpy(tempPlmn, ssimPlmn.plmn, SOFT_SIM_LENGTH_6);
    }

    // Get saved IMEI from auth cofnig file
    uint8_t tempImei[16] = {
        0,
    };
    int nImeiLen = nvmReadImei(nSimId, (nvmImei_t *)&tempImei[0]);
    if (nImeiLen == -1)
    {
        return false;
    }

    uint8_t nImsi[16] = {
        0,
    };
    uint8_t nKey[33] = {
        0,
    };

    // Generate new IMSI
    uint32_t ret = CFW_GenerateSoftSimImsi(tempPlmn, tempImei, nImsi, nKey);
    OSI_LOGI(0, "_generateSoftSimImsiInfo generate new imsi ret: %d", ret);
    if (ret != 0)
    {
        OSI_LOGE(0, "_generateSoftSimImsiInfo generate imsi fail");
        return false;
    }

    if (strlen((char *)nImsi) != SOFT_SIM_IMSI_LENGTH)
    {
        OSI_LOGE(0, "_generateSoftSimImsiInfo wrong imsi length");
        return false;
    }

    if (!_isSoftSimImsiValid((const char *)nImsi, SOFT_SIM_IMSI_LENGTH))
    {
        OSI_LOGE(0, "_generateSoftSimImsiInfo invalid imsi");
        return false;
    }

    _paramHexDataParse(ssim_file_info->soft_auth_param_stru.aucKey, nKey, SSIM_KEY_LEN * 2);
    _imsiStrConvertBCD(ssim_file_info->imsi, (uint8_t *)nImsi, SSIM_IMSI_LEN - 1);
    ssim_file_info->mncLen = ssimPlmn.mncLen;

    return true;
}

void _generateSoftSimAllFileInfo(uint8_t nSimId, bool isCreateAuthInfo)
{
    // create ssim directory
    vfs_mkpath(SSIM_DIRECROTY, 0);

    if (isCreateAuthInfo)
    {
        AUTH_CONTEXT_T authContext;
        _generateDefaultAuthInfo(&authContext, nSimId);
    }

    _generateSoftSimDefaultPlmnFile();

    SSIM_FILE_INFO_T ssim_file_info = {0};
    _generateSimFileInitValue(&ssim_file_info);
    _generateSoftSimImsiInfo(&ssim_file_info, nSimId);
    _saveSoftSimInfoToLocal(ssim_file_info);
    _saveSoftSimInfoToStack(ssim_file_info, nSimId);
}

bool _isImeiValid(CFW_SIM_ID nSimID)
{
    uint8_t nImei[16] = {
        0,
    };
    int nImeiLen = nvmReadImei(nSimID, (nvmImei_t *)&nImei[0]);
    if (nImeiLen == -1)
    {
        return false;
    }

    int i = 0;
    for (; i < 15; i++)
    {
        if (nImei[i] != '0')
        {
            return true;
        }
    }
    return false;
}

bool _isImsiValid(SSIM_FILE_INFO_T ssimFileInfo)
{
    if (ssimFileInfo.imsi[0] == 0)
    {
        return false;
    }
    return true;
}

uint32_t CFW_GetSSimFileInfoFromAP(CFW_SSIM_FILE_INFO_T *cfw_ssim_file_info, CFW_SIM_ID nSimID)
{
    OSI_LOGI(0, "CFW_GetSSimFileInfoFromAP");

    SSIM_FILE_INFO_T ssim_file_info = {0};
    int ret = _getFileData(SSIM_FILE_PATH, &ssim_file_info, sizeof(SSIM_FILE_INFO_T));
    OSI_LOGI(0, "CFW_GetSSimFileInfoFromAP ret: %d", ret);
    if (ret != 0)
    {
        // device doesn't have sim file info, it need to create default info
        if (_isImeiValid(nSimID) && !_isImsiValid(ssim_file_info))
        {
            _generateSoftSimDefaultInfo(cfw_ssim_file_info, nSimID);
            return 0;
        }
        return 1;
    }
    else
    {
        cfw_ssim_file_info->mncLen = ssim_file_info.mncLen;
        memcpy(cfw_ssim_file_info->imsi, ssim_file_info.imsi, SSIM_IMSI_LEN);
        memcpy(&cfw_ssim_file_info->usim_soft_auth_param_stru, &ssim_file_info.soft_auth_param_stru, sizeof(SOFT_AUTH_PARAM_STRU));
        memcpy(&cfw_ssim_file_info->sim_amrc_stru, &ssim_file_info.sim_amrc_stru, sizeof(SIM_AMRC_STRU));
        memcpy(&cfw_ssim_file_info->auth_opc, &ssim_file_info.auth_opc, sizeof(USIM_SOFT_OPC_PARAM_STRU));
    }
    return 0;
}

// CP generate imsi with default plmn and device imei, they need to be saved to AP
uint32_t CFW_SetSoftSimDefaultInfo(uint8_t *nImsi, uint8_t *nKey, uint8_t *nPlmn)
{
    SSIM_FILE_INFO_T ssim_file_info = {0};
    int ret = _getFileData(SSIM_FILE_PATH, &ssim_file_info, sizeof(SSIM_FILE_INFO_T));
    OSI_LOGI(0, "CFW_SetSoftSimDefaultInfo ret: %d", ret);
    if (ret != 0)
    {
        return 1;
    }

    SSIM_PLMN_T ssimPlmn = {0};

    if (strlen((char *)nPlmn) == SOFT_SIM_LENGTH_5)
    {
        ssim_file_info.mncLen = 2;
        ssimPlmn.mncLen = 2;
    }
    else
    {
        ssim_file_info.mncLen = 3;
        ssimPlmn.mncLen = 3;
    }

    // save imsi
    _imsiStrConvertBCD(ssim_file_info.imsi, (uint8_t *)nImsi, SSIM_IMSI_LEN - 1);
    // save key
    _paramHexDataParse(ssim_file_info.soft_auth_param_stru.aucKey, nKey, SSIM_KEY_LEN * 2);
    // save sim info
    _saveSoftSimInfoToLocal(ssim_file_info);
    // save plmn
    memcpy(ssimPlmn.plmn, nPlmn, strlen((char *)nPlmn));
    _saveFileData(SSIM_PLMN_PATH, &ssimPlmn, sizeof(SSIM_PLMN_T));

    return 0;
}

int _generateSha256AuthCode(unsigned char *password, unsigned char *outputAuthcode)
{
    uint32_t id_low;
    bool low = drvEfuseRead(true, RDA_EFUSE_DOUBLE_BLOCK_UNIQUE_ID_LOW_INDEX, &id_low);
    if (!low)
    {
        OSI_LOGE(0, "_generateSha256AuthCode get efuse chip id error");
    }

    OSI_LOGI(0, "_generateSha256AuthCode id_low: %d", id_low);
    uint8_t chipID[8] = {0};
    cfwBcdToDialString((const uint8_t *)&id_low, 4, chipID);

    // chipid+password begin
    char *tempCode = NULL;
    uint16_t totalLength = strlen((const char *)chipID) + PASSWORD_LENGTH + 1 + 1;
    tempCode = (char *)malloc(totalLength);
    if (tempCode == NULL)
    {
        return -1;
    }
    memset(tempCode, 0x00, totalLength);

    uint16_t length = 0;
    length = strlen((const char *)chipID);
    memcpy(tempCode, chipID, length);
    memcpy(tempCode + length, "+", 1);
    length += 1;
    memcpy(tempCode + length, password, PASSWORD_LENGTH);
    // chipid+password end

    // hash with Sha256
    int ret = mbedtls_sha256_ret((const unsigned char *)tempCode, strlen((char *)tempCode), outputAuthcode, 0);
    memset(tempCode, 0x00, totalLength);
    free(tempCode);
    OSI_LOGI(0, "_generateSha256AuthCode ret: %d", ret);
    return ret;
}

int _getFileData(const char *path, void *savedAuthCode, size_t size)
{
    ssize_t fd = vfs_file_read(path, savedAuthCode, size);
    if (fd <= 0)
    {
        OSI_LOGE(0, "_getFileData file doesn't exist");
        return -1;
    }
    return 0;
}

int _saveFileData(const char *path, void *data, size_t size)
{
    ssize_t fd = vfs_file_write(path, data, size);
    if (fd == -1)
    {
        OSI_LOGE(0, "_saveFileData save unsuccefully");
        return -1;
    }
    return 0;
}

static void _authCloseTimeout(atCommand_t *cmd)
{
    OSI_LOGI(0, "_authCloseTimeout");
    g_flag = 0;
    if (g_tmrID != NULL)
    {
        osiTimerStop(g_tmrID);
        osiTimerDelete(g_tmrID);
        g_tmrID = NULL;
    }
}

static int _paramHex2Num(uint8_t c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'A' && c <= 'F')
        return 0xa + (c - 'A');
    if (c >= 'a' && c <= 'f')
        return 0xa + (c - 'a');
    return -1;
}

bool _paramHexDataParse(uint8_t *dest, uint8_t *src, uint16_t length)
{
    OSI_LOGI(0, "_paramHexDataParse in(%d): %s", length, src);

    const uint8_t *pv = src;
    uint16_t n;
    for (n = 0; n < length; n++)
    {
        if (_paramHex2Num(*pv++) < 0)
        {
            OSI_LOGI(0, "_paramHexDataParse invalid character");
            return false;
        }
    }

    uint8_t *s = src;
    length /= 2;
    for (n = 0; n < length; n++)
    {
        int num0 = _paramHex2Num(s[0]);
        int num1 = _paramHex2Num(s[1]);
        s += 2;
        dest[n] = (num0 << 4) | num1;
    }
    return true;
}

void _imsiStrConvertBCD(uint8_t *dest, uint8_t *src, ssize_t srcLen)
{
    int i;
    int j = 0;
    for (i = 1; j < srcLen; i++)
    {
        if (i == 1)
        {
            dest[i] = 0x09;
        }
        else
        {
            dest[i] = (src[j++] - '0') & 0x0F;
        }
        dest[i] |= (src[j++] - '0') << 4;
    }
    dest[0] = i - 1;
}

static uint8_t _ssimHexToAscii(uint8_t v)
{
    if (v >= 0 && v <= 9)
        return v + '0';
    if (v >= 0xa && v <= 0xf)
        return v + 'a' - 10;
    return 0;
}

// for op, key, C1, C2, C3, C4, C5
static void _generateUint8ArrayStr(char *dest, uint8_t *src, uint8_t srcLength)
{
    uint8_t i, j;
    for (j = 0, i = 0; i < SSIM_OP_LEN; i++)
    {
        dest[j++] = _ssimHexToAscii((src[i] >> 4) & 0x0F);
        dest[j++] = _ssimHexToAscii(src[i] & 0x0F);
    }
}

bool _isSoftSimDigit(const char *data, int length)
{
    int i;
    for (i = 0; i < length; i++)
    {
        if (!((data[i] >= '0') && (data[i] <= '9')))
        {
            return false;
        }
    }
    return true;
}

void _softSimQuit()
{
    if (g_flag)
    {
        g_flag = 0;
        if (g_tmrID != NULL)
        {
            osiTimerStop(g_tmrID);
            osiTimerDelete(g_tmrID);
            g_tmrID = NULL;
        }
    }
}

void atCmdHandleSSIMAUTH(atCommand_t *cmd)
{
    if (AT_CMD_SET == cmd->type)
    {
        _softSimQuit();

        // +SSIMAUTH=<imei>,<password>
        bool paramok = true;

        if (cmd->param_count != 2)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        const char *imei = atParamStr(cmd->params[0], &paramok);
        if (!paramok || strlen(imei) != 15)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        int result = _isSoftSimImeiValid(imei, SOFT_SIM_IMEI_LENGTH);
        if (result == -1)
        {
            OSI_LOGE(0, "atCmdHandleSSIMAUTH exception");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

#if 0
        if ((result + '0') != imei[14])
        {
            OSI_LOGE(0, "atCmdHandleSSIMAUTH imei is invalid");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
#endif

        const char *password = atParamStr(cmd->params[1], &paramok);
        if (!paramok || strlen(password) != PASSWORD_LENGTH)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        // get save auth info
        AUTH_CONTEXT_T savedAuthContext;
        ssize_t fd = vfs_file_read(AUTH_FILE_PATH, &savedAuthContext, sizeof(AUTH_CONTEXT_T));
        OSI_LOGI(0, "atCmdHandleSSIMAUTH fd: %d", fd);
        if (fd == -1)
        {
            OSI_LOGE(0, "atCmdHandleSSIMAUTH no auth info, need to create new default auth info");
            _generateDefaultAuthInfo(&savedAuthContext, atCmdGetSim(cmd->engine));
        }

        // compare imei
        if (memcmp(imei, savedAuthContext.imei, SOFT_SIM_IMEI_LENGTH) != 0)
        {
            OSI_LOGE(0, "atCmdHandleSSIMAUTH The input imei is invalid");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        int ret = 0;
        unsigned char outputAuthcode[AUTH_CODE_LENGTH + 1] = {0};
        ret = _generateSha256AuthCode((unsigned char *)password, (unsigned char *)outputAuthcode);

        OSI_LOGI(0, "atCmdHandleSSIMAUTH ret: %d", ret);
        if (ret == -1)
        {
            OSI_LOGE(0, "atCmdHandleSSIMAUTH generate auth code error");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        // begin compare the authcode
        if (memcmp(savedAuthContext.authValue, outputAuthcode, AUTH_CODE_LENGTH) != 0)
        {
            OSI_LOGE(0, "atCmdHandleSSIMAUTH auth is invalid");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        else
        {
            OSI_LOGI(0, "atCmdHandleSSIMAUTH auth success");
        }

        g_flag = 1;
        g_tmrID = osiTimerCreate(atEngineGetThreadId(), (osiCallback_t)_authCloseTimeout, (void *)cmd);
        osiTimerStart(g_tmrID, 2 * 60 * 1000);
        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        char pOutStr[20] = {
            0x00,
        };

        sprintf(pOutStr, "+SSIMAUTH: %d", g_flag);
        atCmdRespInfoText(cmd->engine, pOutStr);
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}

void atCmdHandleSSIMAUTHEXIT(atCommand_t *cmd)
{
    switch (cmd->type)
    {
    case AT_CMD_EXE:
        _softSimQuit();
        RETURN_OK(cmd->engine);
        break;
    default:
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}

void atCmdHandleSSIMCHANGEPW(atCommand_t *cmd)
{
    if (!g_flag)
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
    }

    if (AT_CMD_SET == cmd->type)
    {
        // +SSIMCHANGEPW=<oldpassword>,<newpassword>
        bool paramok = true;

        if (cmd->param_count != 2)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        const char *oldpassword = atParamStr(cmd->params[0], &paramok);
        const char *newpassword = atParamStr(cmd->params[1], &paramok);
        if (!paramok || strlen(oldpassword) != PASSWORD_LENGTH || strlen(newpassword) != PASSWORD_LENGTH)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        // get saved authcode
        AUTH_CONTEXT_T savedAuthContext;
        int ret = _getFileData(AUTH_FILE_PATH, &savedAuthContext, sizeof(AUTH_CONTEXT_T));
        if (ret == -1)
        {
            OSI_LOGE(0, "atCmdHandleSSIMCHANGEPW file doesn't exist");
            _generateDefaultAuthInfo(&savedAuthContext, atCmdGetSim(cmd->engine));
        }

        // generate old password hash sha256
        unsigned char oldOutputAuthcode[AUTH_CODE_LENGTH + 1] = {0};
        ret = _generateSha256AuthCode((unsigned char *)oldpassword, (unsigned char *)oldOutputAuthcode);
        if (ret == -1)
        {
            OSI_LOGE(0, "atCmdHandleSSIMCHANGEPW get auth info fail");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        // check whether the old password is or not right
        if (memcmp(oldOutputAuthcode, savedAuthContext.authValue, AUTH_CODE_LENGTH) != 0)
        {
            OSI_LOGE(0, "atCmdHandleSSIMCHANGEPW old password is wrong");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        // generate new password hash sha256
        unsigned char newOutputAuthcode[AUTH_CODE_LENGTH + 1] = {0};
        ret = _generateSha256AuthCode((unsigned char *)newpassword, (unsigned char *)newOutputAuthcode);
        if (ret == -1)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        // save the authcode
        memset(savedAuthContext.authValue, 0x00, AUTH_CODE_LENGTH + 1);
        memcpy(savedAuthContext.authValue, newOutputAuthcode, AUTH_CODE_LENGTH);
        ret = _saveFileData(AUTH_FILE_PATH, &savedAuthContext, sizeof(AUTH_CONTEXT_T));
        if (ret == -1)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        _softSimQuit();
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}

void atCmdHandleSSIMINFO(atCommand_t *cmd)
{
    if (!g_flag)
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
    }

    char pOutStr[512] = {
        0x00,
    };

    if (AT_CMD_SET == cmd->type)
    {
        // +SSIMINFO=<mnc_len>,<imsi>,<op>,<key>,<opc>,<R1>,<R2>,<R3>,<R4>,<R5>,<C1>,<C2>,<C3>,<C4>,<C5>

        // get save sim file info
        SSIM_FILE_INFO_T ssim_file_info = {0};
        int ret = _getFileData(SSIM_FILE_PATH, &ssim_file_info, sizeof(SSIM_FILE_INFO_T));
        if (ret != 0)
        {
            _generateSoftSimAllFileInfo(atCmdGetSim(cmd->engine), false);
            _getFileData(SSIM_FILE_PATH, &ssim_file_info, sizeof(SSIM_FILE_INFO_T));
        }

        // get saved plmn
        SSIM_PLMN_T ssimPlmn = {0};
        _getFileData(SSIM_PLMN_PATH, &ssimPlmn, sizeof(SSIM_PLMN_T));

        bool paramok = true;

        if ((!atParamIsEmpty(cmd->params[0]) && atParamIsEmpty(cmd->params[1])) || (atParamIsEmpty(cmd->params[0]) && !atParamIsEmpty(cmd->params[1])))
        {
            sprintf(pOutStr, "mncLen and IMSI should be empty or not empty at the same time");
            atCmdRespInfoText(cmd->engine, pOutStr);
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        if (!atParamIsEmpty(cmd->params[0]))
        {
            uint8_t mncLen = atParamUintInRange(cmd->params[0], 2, 3, &paramok);
            if (!paramok)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
            ssim_file_info.mncLen = mncLen;
            ssimPlmn.mncLen = mncLen;
        }

        // get imsi
        if (!atParamIsEmpty(cmd->params[1]))
        {
            const char *imsi = atParamStr(cmd->params[1], &paramok);
            if (!paramok)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }

            if (strlen(imsi) != SOFT_SIM_IMSI_LENGTH || !_isSoftSimImsiValid(imsi, SOFT_SIM_IMSI_LENGTH))
            {
                OSI_LOGE(0, "atCmdHandleSSIMINFO the parameter of imsi lenght is valid or format is valid");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            }

            memset(ssimPlmn.plmn, 0x00, PLMN_LENGTH);
            memcpy(ssimPlmn.plmn, imsi, ssimPlmn.mncLen == 2 ? SOFT_SIM_LENGTH_5 : SOFT_SIM_LENGTH_6);
            _imsiStrConvertBCD(ssim_file_info.imsi, (uint8_t *)imsi, SSIM_IMSI_LEN - 1);
        }

        // get OP value
        if (!atParamIsEmpty(cmd->params[2]))
        {
            const char *op = atParamStr(cmd->params[2], &paramok);
            if (!paramok)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }

            if (JUDGE_OP_LENGTH(op) || !_paramHexDataParse(ssim_file_info.soft_auth_param_stru.aucOp, (uint8_t *)op, SSIM_OP_LEN * 2))
            {
                OSI_LOGE(0, "atCmdHandleSSIMINFO OP length is invalid");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
        }

        // get KEY value
        if (!atParamIsEmpty(cmd->params[3]))
        {
            const char *key = atParamStr(cmd->params[3], &paramok);
            if (!paramok)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }

            if (JUDGE_KEY_LENGTH(key) || !_paramHexDataParse(ssim_file_info.soft_auth_param_stru.aucKey, (uint8_t *)key, SSIM_KEY_LEN * 2))
            {
                OSI_LOGE(0, "atCmdHandleSSIMINFO KEY length is invalid");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
        }

        // get OPC value
        if (!atParamIsEmpty(cmd->params[4]))
        {
            const char *opc = atParamStr(cmd->params[4], &paramok);
            if (!paramok)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }

            if (JUDGE_OPC_LENGTH(opc) || !_paramHexDataParse(ssim_file_info.auth_opc.aucOpc, (uint8_t *)opc, SSIM_OPC_LEN * 2))
            {
                OSI_LOGE(0, "atCmdHandleSSIMINFO OPC length is invalid");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
        }

        // get R1 value
        if (!atParamIsEmpty(cmd->params[5]))
        {
            uint8_t r1 = atParamUintInRange(cmd->params[5], 0, 255, &paramok);
            if (!paramok)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
            ssim_file_info.sim_amrc_stru.R1 = r1;
        }

        // get R2 value
        if (!atParamIsEmpty(cmd->params[6]))
        {
            uint8_t r2 = atParamUintInRange(cmd->params[6], 0, 255, &paramok);
            if (!paramok)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
            ssim_file_info.sim_amrc_stru.R2 = r2;
        }

        // get R3 value
        if (!atParamIsEmpty(cmd->params[7]))
        {
            uint8_t r3 = atParamUintInRange(cmd->params[7], 0, 255, &paramok);
            if (!paramok)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
            ssim_file_info.sim_amrc_stru.R3 = r3;
        }

        // get R4 value
        if (!atParamIsEmpty(cmd->params[8]))
        {
            uint8_t r4 = atParamUintInRange(cmd->params[8], 0, 255, &paramok);
            if (!paramok)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
            ssim_file_info.sim_amrc_stru.R4 = r4;
        }

        // get R5 value
        if (!atParamIsEmpty(cmd->params[9]))
        {
            uint8_t r5 = atParamUintInRange(cmd->params[9], 0, 255, &paramok);
            if (!paramok)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
            ssim_file_info.sim_amrc_stru.R5 = r5;
        }

        // get C1 value
        if (!atParamIsEmpty(cmd->params[10]))
        {
            const char *c1 = atParamStr(cmd->params[10], &paramok);
            if (!paramok)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }

            if (JUDGE_C_LENGTH(c1) || !_paramHexDataParse(ssim_file_info.sim_amrc_stru.C1, (uint8_t *)c1, SSIM_C_PARAMTER_LENGTH * 2))
            {
                OSI_LOGE(0, "atCmdHandleSSIMINFO c1 length is invalid");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
        }

        // get C2 value
        if (!atParamIsEmpty(cmd->params[11]))
        {
            const char *c2 = atParamStr(cmd->params[11], &paramok);
            if (!paramok)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }

            if (JUDGE_C_LENGTH(c2) || !_paramHexDataParse(ssim_file_info.sim_amrc_stru.C2, (uint8_t *)c2, SSIM_C_PARAMTER_LENGTH * 2))
            {
                OSI_LOGE(0, "atCmdHandleSSIMINFO c2 length is invalid");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
        }

        // get C3 value
        if (!atParamIsEmpty(cmd->params[12]))
        {
            const char *c3 = atParamStr(cmd->params[12], &paramok);
            if (!paramok)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }

            if (JUDGE_C_LENGTH(c3) || !_paramHexDataParse(ssim_file_info.sim_amrc_stru.C3, (uint8_t *)c3, SSIM_C_PARAMTER_LENGTH * 2))
            {
                OSI_LOGE(0, "atCmdHandleSSIMINFO c3 length is invalid");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
        }

        // get C4 value
        if (!atParamIsEmpty(cmd->params[13]))
        {
            const char *c4 = atParamStr(cmd->params[13], &paramok);
            if (!paramok)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }

            if (JUDGE_C_LENGTH(c4) || !_paramHexDataParse(ssim_file_info.sim_amrc_stru.C4, (uint8_t *)c4, SSIM_C_PARAMTER_LENGTH * 2))
            {
                OSI_LOGE(0, "atCmdHandleSSIMINFO c4 length is invalid");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
        }

        // get C5 value
        if (!atParamIsEmpty(cmd->params[14]))
        {
            const char *c5 = atParamStr(cmd->params[14], &paramok);
            if (!paramok)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }

            if (JUDGE_C_LENGTH(c5) || !_paramHexDataParse(ssim_file_info.sim_amrc_stru.C5, (uint8_t *)c5, SSIM_C_PARAMTER_LENGTH * 2))
            {
                OSI_LOGE(0, "atCmdHandleSSIMINFO c5 length is invalid");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
        }

        ssize_t fd = _saveFileData(SSIM_PLMN_PATH, &ssimPlmn, sizeof(SSIM_PLMN_T));
        if (fd == -1)
        {
            OSI_LOGE(0, "atCmdHandleSSIMINFO AT_CMD_SET save plmn file fail");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        if (!_saveSoftSimInfo(ssim_file_info, atCmdGetSim(cmd->engine)))
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        RETURN_OK(cmd->engine);
    }
    else if (AT_CMD_READ == cmd->type)
    {
        uint8_t i, j;

        SSIM_FILE_INFO_T ssim_file_info = {0};
        int ret = _getFileData(SSIM_FILE_PATH, &ssim_file_info, sizeof(SSIM_FILE_INFO_T));
        if (ret != 0)
        {
            OSI_LOGE(0, "atCmdHandleSSIMINFO AT_CMD_READ no local sim info");
            _generateSoftSimAllFileInfo(atCmdGetSim(cmd->engine), false);
            _getFileData(SSIM_FILE_PATH, &ssim_file_info, sizeof(SSIM_FILE_INFO_T));
        }

        char imei[16] = {
            0x00,
        };
        for (j = 0, i = 1; i <= ssim_file_info.imsi[0]; i++)
        {
            if (i == 1)
            {
                imei[j++] = ((ssim_file_info.imsi[i] >> 4) & 0x0F) + '0';
                continue;
            }

            imei[j++] = (ssim_file_info.imsi[i] & 0x0F) + '0';
            imei[j++] = ((ssim_file_info.imsi[i] >> 4) & 0x0F) + '0';
        }

        char op[33] = {
            0x00,
        };
        _generateUint8ArrayStr(op, ssim_file_info.soft_auth_param_stru.aucOp, SSIM_OP_LEN);

        char opc[33] = {
            0x00,
        };
        _generateUint8ArrayStr(opc, ssim_file_info.auth_opc.aucOpc, SSIM_OPC_LEN);

        char key[33] = {
            0x00,
        };
        _generateUint8ArrayStr(key, ssim_file_info.soft_auth_param_stru.aucKey, SSIM_KEY_LEN);

        char c1[33] = {
            0x00,
        };
        _generateUint8ArrayStr(c1, ssim_file_info.sim_amrc_stru.C1, SSIM_C_PARAMTER_LENGTH);

        char c2[33] = {
            0x00,
        };
        _generateUint8ArrayStr(c2, ssim_file_info.sim_amrc_stru.C2, SSIM_C_PARAMTER_LENGTH);

        char c3[33] = {
            0x00,
        };
        _generateUint8ArrayStr(c3, ssim_file_info.sim_amrc_stru.C3, SSIM_C_PARAMTER_LENGTH);

        char c4[33] = {
            0x00,
        };
        _generateUint8ArrayStr(c4, ssim_file_info.sim_amrc_stru.C4, SSIM_C_PARAMTER_LENGTH);

        char c5[33] = {
            0x00,
        };
        _generateUint8ArrayStr(c5, ssim_file_info.sim_amrc_stru.C5, SSIM_C_PARAMTER_LENGTH);

        // +SSIMINFO=<mnc_len>,<imsi>,<op>,<key>,<opc>,<R1>,<R2>,<R3>,<R4>,<R5>,<C1>,<C2>,<C3>,<C4>,<C5>
        sprintf(pOutStr, "+SSIMINFO: %d,\"%s\",\"%s\",\"%s\",\"%s\",%x,%x,%x,%x,%x,\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"", ssim_file_info.mncLen, imei, op, key, opc, ssim_file_info.sim_amrc_stru.R1, ssim_file_info.sim_amrc_stru.R2, ssim_file_info.sim_amrc_stru.R3, ssim_file_info.sim_amrc_stru.R4, ssim_file_info.sim_amrc_stru.R5, c1, c2, c3, c4, c5);
        atCmdRespInfoText(cmd->engine, pOutStr);
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}

void atCmdHandleSSIMPLMN(atCommand_t *cmd)
{
    char pOutStr[200] = {
        0x00,
    };
    char tempStr[7] = {
        0x00,
    };

    if (cmd->type == AT_CMD_SET)
    {
        if (!g_flag)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        // AT+SSIMPLMN=<plmn>
        if (cmd->param_count != 1)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        bool paramok = true;
        const char *plmn = atParamStr(cmd->params[0], &paramok);
        if (!paramok)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        // judge plmn format and length
        if ((strlen(plmn) != SOFT_SIM_LENGTH_5 && strlen(plmn) != SOFT_SIM_LENGTH_6) || !_isSoftSimDigit(plmn, strlen(plmn)))
        {
            OSI_LOGE(0, "atCmdHandleSSIMPLMN AT_CMD_SET wrong plmn length(%d), or wrong formmat", strlen(plmn));
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        // get saved plmn
        SSIM_PLMN_T ssimPlmn = {0};
        _getFileData(SSIM_PLMN_PATH, &ssimPlmn, sizeof(SSIM_PLMN_T));

        memset(ssimPlmn.plmn, 0x00, PLMN_LENGTH);
        if (strlen(plmn) == SOFT_SIM_LENGTH_5)
        {
            ssimPlmn.mncLen = 2;
            memcpy(ssimPlmn.plmn, plmn, SOFT_SIM_LENGTH_5);
        }
        else
        {
            ssimPlmn.mncLen = 3;
            memcpy(ssimPlmn.plmn, plmn, SOFT_SIM_LENGTH_6);
        }

        // save plmn
        int save = _saveFileData(SSIM_PLMN_PATH, &ssimPlmn, sizeof(SSIM_PLMN_T));
        if (save != 0)
        {
            OSI_LOGE(0, "atCmdHandleSSIMPLMN AT_CMD_SET save plmn fail");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        SSIM_FILE_INFO_T ssim_file_info = {0};
        ssim_file_info.mncLen = ssimPlmn.mncLen;
        _generateSimFileInitValue(&ssim_file_info);
        _generateSoftSimImsiInfo(&ssim_file_info, atCmdGetSim(cmd->engine));

        if (!_saveSoftSimInfo(ssim_file_info, atCmdGetSim(cmd->engine)))
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        // Update plmn end
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_READ)
    {
        SSIM_PLMN_T ssimPlmn = {0};
        int ret = _getFileData(SSIM_PLMN_PATH, &ssimPlmn, sizeof(SSIM_PLMN_T));
        if (ret != 0)
        {
            OSI_LOGE(0, "atCmdHandleSSIMPLMN AT_CMD_READ no plmn");
            _generateSoftSimAllFileInfo(atCmdGetSim(cmd->engine), false);
            _getFileData(SSIM_PLMN_PATH, &ssimPlmn, sizeof(SSIM_PLMN_T));
        }

        if (ssimPlmn.mncLen == 2)
        {
            memcpy(tempStr, ssimPlmn.plmn, SOFT_SIM_LENGTH_5);
        }
        else
        {
            memcpy(tempStr, ssimPlmn.plmn, SOFT_SIM_LENGTH_6);
        }
        sprintf(pOutStr, "+SSIMPLMN: %s", tempStr);
        atCmdRespInfoText(cmd->engine, pOutStr);
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}

void atCmdHandleSSIMFILEHASH(atCommand_t *cmd)
{
    char pOutStr[512] = {
        0x00,
    };

    if (!g_flag)
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
    }

    if (cmd->type == AT_CMD_READ)
    {
        SSIM_FILE_INFO_T ssim_file_info = {0};
        int ret = _getFileData(SSIM_FILE_PATH, &ssim_file_info, sizeof(SSIM_FILE_INFO_T));
        OSI_LOGI(0, "atCmdHandleSSIMFILEHASH AT_CMD_READ ret: %d", ret);
        if (ret != 0)
        {
            _generateSoftSimAllFileInfo(atCmdGetSim(cmd->engine), false);
            _getFileData(SSIM_FILE_PATH, &ssim_file_info, sizeof(SSIM_FILE_INFO_T));
        }

        char temp[200] = {0x00};
        uint8_t i, j;
        for (i = 0, j = 0; i < 32 && j < 200; i++)
        {
            uint8_t value1 = ssim_file_info.hashCode[i] >> 4;
            uint8_t value2 = ssim_file_info.hashCode[i] & 0x0F;

            if (value1 > 9)
            {
                temp[j++] = value1 + 'A' - 10;
            }
            else
            {
                temp[j++] = value1 + '0';
            }

            if (value2 > 9)
            {
                temp[j++] = value2 + 'A' - 10;
            }
            else
            {
                temp[j++] = value2 + '0';
            }
        }

        sprintf(pOutStr, "+SSIMFILEHASH: %s", temp);
        atCmdRespInfoText(cmd->engine, pOutStr);
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}

void atCmdHandleSSIMMSIN(atCommand_t *cmd)
{
    if (!g_flag)
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
    }

    char pOutStr[200] = {
        0x00,
    };
    char tempStr[7] = {
        0x00,
    };

    if (cmd->type == AT_CMD_SET)
    {
        // AT+SSIMMSIN=<MSINH3B>
        if (cmd->param_count != 1)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        bool paramok = true;
        const char *MSINH3B = atParamStr(cmd->params[0], &paramok);
        if (!paramok)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        if (strlen(MSINH3B) != SOFT_SIM_MSINH3B_LENGTH || !_isSoftSimDigit(MSINH3B, SOFT_SIM_MSINH3B_LENGTH))
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        SSIM_FILE_INFO_T ssim_file_info = {0};
        int ret = _getFileData(SSIM_FILE_PATH, &ssim_file_info, sizeof(SSIM_FILE_INFO_T));
        OSI_LOGI(0, "atCmdHandleSSIMMSIN AT_CMD_SET ret: %d", ret);
        if (ret != 0)
        {
            _generateSoftSimAllFileInfo(atCmdGetSim(cmd->engine), false);
            _getFileData(SSIM_FILE_PATH, &ssim_file_info, sizeof(SSIM_FILE_INFO_T));
        }

        // Example
        // real imsi
        // 460900894033857
        // The actual data format for imsi
        // { 0x08,0x49,0x6, 0x09, 0x80, 0x49, 0x30, 0x83, 0x75, 0x0 }

        if (ssim_file_info.mncLen == 2)
        {
            ssim_file_info.imsi[4] = 0x00;
            ssim_file_info.imsi[4] = _paramHex2Num(MSINH3B[0]);
            ssim_file_info.imsi[4] |= (_paramHex2Num(MSINH3B[1])) << 4;
            ssim_file_info.imsi[5] &= 0xF0;
            ssim_file_info.imsi[5] |= _paramHex2Num(MSINH3B[2]);
        }
        else
        {
            ssim_file_info.imsi[4] &= 0x0F;
            ssim_file_info.imsi[4] |= (_paramHex2Num(MSINH3B[0])) << 4;
            ssim_file_info.imsi[5] = 0x00;
            ssim_file_info.imsi[5] = _paramHex2Num(MSINH3B[1]);
            ssim_file_info.imsi[5] |= (_paramHex2Num(MSINH3B[2])) << 4;
        }

        if (!_saveSoftSimInfo(ssim_file_info, atCmdGetSim(cmd->engine)))
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
        }

        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_READ)
    {
        SSIM_FILE_INFO_T ssim_file_info = {0};
        int ret = _getFileData(SSIM_FILE_PATH, &ssim_file_info, sizeof(SSIM_FILE_INFO_T));
        OSI_LOGI(0, "atCmdHandleSSIMMSIN AT_CMD_READ ret: %d", ret);
        if (ret != 0)
        {
            _generateSoftSimAllFileInfo(atCmdGetSim(cmd->engine), false);
            _getFileData(SSIM_FILE_PATH, &ssim_file_info, sizeof(SSIM_FILE_INFO_T));
        }

        if (ssim_file_info.mncLen == 2)
        {
            tempStr[0] = (ssim_file_info.imsi[4] & 0x0F) + '0';
            tempStr[1] = (ssim_file_info.imsi[4] >> 4) + '0';
            tempStr[2] = (ssim_file_info.imsi[5] & 0x0F) + '0';
        }
        else
        {
            tempStr[0] = (ssim_file_info.imsi[4] >> 4) + '0';
            tempStr[1] = (ssim_file_info.imsi[5] & 0x0F) + '0';
            tempStr[2] = (ssim_file_info.imsi[5] >> 4) + '0';
        }

        sprintf(pOutStr, "+SSIMMSIN: %s", tempStr);
        atCmdRespInfoText(cmd->engine, pOutStr);
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}

void atCmdHandleSSIMDELFILE(atCommand_t *cmd)
{
    // Clean all nv information
    if (cmd->type == AT_CMD_SET)
    {

        // +SSIMDELFILE=<mode>
        bool paramok = true;

        if (cmd->param_count != 1)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        uint8_t mode = atParamDefUintInRange(cmd->params[0], 1, 0, 1, &paramok);
        if (!paramok)
        {
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        vfs_unlink(SSIM_FILE_PATH);
        vfs_unlink(SSIM_PLMN_PATH);
        if (mode == 1)
        {
            vfs_unlink(AUTH_FILE_PATH);
        }

#if 0 // Delete statck soft sim info
        SSIM_FILE_INFO_T ssim_file_info = {0};
        _saveSoftSimInfoToStack(ssim_file_info, atCmdGetSim(cmd->engine));
#endif

        _softSimQuit();
        RETURN_OK(cmd->engine);
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}
#else
uint32_t CFW_GetSSimFileInfoFromAP(CFW_SSIM_FILE_INFO_T *ssim_info, CFW_SIM_ID nSimID)
{
    return 1;
}
uint32_t CFW_SetSoftSimDefaultInfo(uint8_t *nImsi, uint8_t *nKey, uint8_t *nPlmn)
{
    return 1;
}
#endif
