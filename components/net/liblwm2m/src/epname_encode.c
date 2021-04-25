#include "epname_encode.h"
#include "j_aes.h"
#include "j_base64.h"
#include "liblwm2m.h"

static uint8_t g_simId = 0;

int to_encode(unsigned char *szin, unsigned char *szout, unsigned char *szPwd)
{
    unsigned char *name = szin;
    //AES CBC
    /* A 256 bit key */
    unsigned char *passwd = (unsigned char *)"00000000000000000000000000000000";
    if (strlen((char *)szPwd) > 0)
    {
        passwd = szPwd;
    }

    unsigned char key[64] = {0};
    memset(key, 0, sizeof(key));
    char shaout[256];
    memset(shaout, 0, sizeof(shaout));
    StrSHA256((const char *)passwd, strlen((char *)passwd), shaout);
    HexStrToByte((const char *)shaout, strlen(shaout), key);

    /* A 128 bit IV */
    unsigned char iv[16] = {0};
    memset(iv, 0, 16);
    int i = 0;
    for (i = 0; i < 16; ++i)
    {
        iv[i] = 0;
    }
    unsigned char ciphertext[264] = {0};
    /* Buffer for the decrypted text */
    //unsigned char decryptedtext[264]={0};

    int /*decryptedtext_len,*/ ciphertext_len = 0;
    unsigned char *plaintext = (unsigned char *)"The quick brown fox jumps over the lazy dog1234";
    plaintext = name;
    //加密

    ciphertext_len = EncryptionAES((char *)plaintext, strlen((char *)plaintext), key, ciphertext);

    name = ciphertext; //是否使用加密后的值

    //base64
    unsigned char *testbase64 = (unsigned char *)"123456789"; //MTIzNDU2Nzg5
    testbase64 = name;

    unsigned char *encData = 0;
    //unsigned char *decData=0;
    int encDataLen = 0;
    //int decDataLen=0;
    j_base64_encode(testbase64, (unsigned int)ciphertext_len, &encData, ((unsigned int *)(&encDataLen)));
    memcpy(szout, encData, encDataLen);

    j_base64_free((void *)encData, (size_t)encDataLen);

    return encDataLen;
}

void generate_encode_register_epname(uint8_t simid, uint8_t *pwd, uint8_t *appKey, uint8_t *version, uint8_t *epnametmp)
{
    uint8_t imei[16] = {0};
    uint8_t imsi[16] = {0};

    //zhangyi del for porting 20180710
    //uint8_t imeisize = 16;
    //uint8_t imsisize = 16;

    g_simId = simid;

    //zhangyi del for porting 20180710
    //getSimImei(simid,imei,&imeisize);
    //getSimImsi(simid,imsi,&imsisize);

    const uint8_t *szStars = (uint8_t *)"****";
    uint8_t szEpname[256] = {0};
    sprintf((char *)szEpname, "%s-%s-%s-%s",
            strlen((char *)imei) > 0 ? imei : szStars,
            szStars,
            strlen((char *)imsi) > 0 ? imsi : szStars,
            version);

    unsigned char szEpnameCode[256] = {0};
    memset(szEpnameCode, 0, sizeof(szEpnameCode));

    to_encode(szEpname, szEpnameCode, pwd);
    sprintf((char *)epnametmp, "L@#@%s@#@%s", szEpnameCode, appKey);
}

void generate_encode_update_epname(uint8_t *pwd, uint8_t *appKey, uint8_t *szEPI)
{
    uint8_t imei[16] = {0};
    uint8_t imsi[16] = {0};

    //zhangyi del for porting 20180710
    //uint8_t imeisize = 16;
    //uint8_t imsisize = 16;
    //getSimImei(g_simId,imei,&imeisize);
    //getSimImsi(g_simId,imsi,&imsisize);

    const uint8_t *szStars = (uint8_t *)"****";
    unsigned char szEPIsrc[256] = {0};
    sprintf((char *)szEPIsrc, "%s-%s-%s-%s",
            strlen((char *)imei) > 0 ? imei : szStars,
            szStars,
            strlen((char *)imsi) > 0 ? imsi : szStars,
            szStars);

    uint8_t epitmp[256] = {0};
    memset(epitmp, 0, sizeof(epitmp));
    to_encode(szEPIsrc, epitmp, pwd);
    sprintf((char *)szEPI, "L@#@%s@#@%s", epitmp, appKey);
}
