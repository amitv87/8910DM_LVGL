
#include "ats_config.h"

#ifdef CONFIG_AT_SSIM_SUPPORT

#define EFUSE_UNIQUE_ID_LOW_INDEX (24)

#define SSIM_DIRECROTY "/ssim"
#define AUTH_FILE_NAME "auth"
#define SSIM_FILE "sim_file"
#define SSIM_PLMN "sim_plmn"

#define AUTH_FILE_PATH SSIM_DIRECROTY "/" AUTH_FILE_NAME
#define SSIM_FILE_PATH SSIM_DIRECROTY "/" SSIM_FILE
#define SSIM_PLMN_PATH SSIM_DIRECROTY "/" SSIM_PLMN

#define PASSWORD_LENGTH 12
#define AUTH_CODE_LENGTH 32
#define PLMN_LENGTH 7

#define SSIM_C_PARAMTER_LENGTH 16
#define SSIM_KEY_LEN 16
#define SSIM_OP_LEN 16
#define SSIM_OPC_LEN 16
#define SSIM_IMSI_LEN 16
#define SSIM_IMEI_LEN 16

#define SOFT_SIM_LENGTH_5 5
#define SOFT_SIM_LENGTH_6 6
#define SOFT_SIM_MSINH3B_LENGTH 3
#define SOFT_SIM_IMEI_LENGTH 15
#define SOFT_SIM_IMSI_LENGTH 15

typedef struct
{
    uint8_t mncLen;
    uint8_t plmn[PLMN_LENGTH];
} SSIM_PLMN_T;

typedef struct
{
    uint8_t imei[SSIM_IMEI_LEN];
    uint8_t password[PASSWORD_LENGTH + 1];
    uint8_t authValue[AUTH_CODE_LENGTH + 1];
} AUTH_CONTEXT_T;

typedef struct
{
    uint32_t enSoftUsimAlg;
    uint8_t aucKey[SSIM_KEY_LEN];
    uint8_t aucOp[SSIM_OP_LEN];
} SOFT_AUTH_PARAM_STRU;

typedef struct
{
    uint8_t R1;
    uint8_t R2;
    uint8_t R3;
    uint8_t R4;
    uint8_t R5;
    uint8_t aucRsv[3];
    uint8_t C1[SSIM_C_PARAMTER_LENGTH];
    uint8_t C2[SSIM_C_PARAMTER_LENGTH];
    uint8_t C3[SSIM_C_PARAMTER_LENGTH];
    uint8_t C4[SSIM_C_PARAMTER_LENGTH];
    uint8_t C5[SSIM_C_PARAMTER_LENGTH];
} SIM_AMRC_STRU;

typedef struct
{
    uint32_t bitOpcSoftUsimEnable : 1; // 1: VALID; 0: INVALID*, default : 0
    uint32_t bitOpRsv : 31;
    uint8_t aucOpc[SSIM_OPC_LEN];
} USIM_SOFT_OPC_PARAM_STRU;

typedef struct
{
    uint8_t mncLen;
    uint8_t imsi[SSIM_IMSI_LEN];
    SIM_AMRC_STRU sim_amrc_stru;
    SOFT_AUTH_PARAM_STRU soft_auth_param_stru;
    USIM_SOFT_OPC_PARAM_STRU auth_opc;
    unsigned char hashCode[32];
} SSIM_FILE_INFO_T;

#define JUDGE_KEY_LENGTH(_PARAM_) (strlen(_PARAM_) != (SSIM_KEY_LEN * 2))
#define JUDGE_OP_LENGTH(_PARAM_) (strlen(_PARAM_) != (SSIM_OP_LEN * 2))
#define JUDGE_OPC_LENGTH(_PARAM_) (strlen(_PARAM_) != (SSIM_OPC_LEN * 2))
#define JUDGE_C_LENGTH(_PARAM_) (strlen(_PARAM_) != (SSIM_C_PARAMTER_LENGTH * 2))

#define PARSE_KEY_HEX_DATA !_paramHexDataParse((uint8_t *)ssim_file_info.soft_auth_param_stru.aucKey, (uint8_t *)key, SSIM_KEY_LEN * 2)
#define PARSE_OP_HEX_DATA !_paramHexDataParse((uint8_t *)ssim_file_info.soft_auth_param_stru.aucOp, (uint8_t *)op, SSIM_OP_LEN * 2)
#define PARSE_OPC_HEX_DATA !_paramHexDataParse((uint8_t *)ssim_file_info.auth_opc.aucOpc, (uint8_t *)opc, SSIM_OPC_LEN * 2)

#define PARSE_C1_HEX_DATA !_paramHexDataParse((uint8_t *)ssim_file_info.sim_amrc_stru.C1, (uint8_t *)c1, SSIM_C_PARAMTER_LENGTH * 2)
#define PARSE_C2_HEX_DATA !_paramHexDataParse((uint8_t *)ssim_file_info.sim_amrc_stru.C2, (uint8_t *)c2, SSIM_C_PARAMTER_LENGTH * 2)
#define PARSE_C3_HEX_DATA !_paramHexDataParse((uint8_t *)ssim_file_info.sim_amrc_stru.C3, (uint8_t *)c3, SSIM_C_PARAMTER_LENGTH * 2)
#define PARSE_C4_HEX_DATA !_paramHexDataParse((uint8_t *)ssim_file_info.sim_amrc_stru.C4, (uint8_t *)c4, SSIM_C_PARAMTER_LENGTH * 2)
#define PARSE_C5_HEX_DATA !_paramHexDataParse((uint8_t *)ssim_file_info.sim_amrc_stru.C5, (uint8_t *)c5, SSIM_C_PARAMTER_LENGTH * 2)

#define PARSE_HEX_DATA PARSE_KEY_HEX_DATA || PARSE_OP_HEX_DATA || PARSE_OPC_HEX_DATA || PARSE_C1_HEX_DATA || PARSE_C2_HEX_DATA || PARSE_C3_HEX_DATA || PARSE_C4_HEX_DATA || PARSE_C5_HEX_DATA
#define PARSE_HEX_DATA_NO_KEY PARSE_OP_HEX_DATA || PARSE_OPC_HEX_DATA || PARSE_C1_HEX_DATA || PARSE_C2_HEX_DATA || PARSE_C3_HEX_DATA || PARSE_C4_HEX_DATA || PARSE_C5_HEX_DATA

extern int mbedtls_sha256_ret(const unsigned char *input,
                              size_t ilen,
                              unsigned char output[32],
                              int is224);

int _saveFileData(const char *path, void *data, size_t size);

int _getFileData(const char *path, void *savedAuthCode, size_t size);

bool _paramHexDataParse(uint8_t *dest, uint8_t *src, uint16_t length);

void _imsiStrConvertBCD(uint8_t *dest, uint8_t *src, ssize_t srcLen);

bool _isSoftSimDigit(const char *data, int length);

int _generateSha256AuthCode(unsigned char *password, unsigned char *outputAuthcode);

#endif