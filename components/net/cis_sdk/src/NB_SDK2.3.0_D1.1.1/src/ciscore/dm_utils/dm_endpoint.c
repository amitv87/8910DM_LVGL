/*
 *  FIPS-197 compliant AES implementation
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS 
 *  The AES block cipher was designed by Vincent Rijmen and Joan Daemen.
 *
 *  http://csrc.nist.gov/encryption/aes/rijndael/Rijndael.pdf
 *  http://csrc.nist.gov/publications/fips/fips197/fips-197.pdf
 */



#include <string.h>
#include <stddef.h>
#include "cis_internals.h"
#if CIS_ENABLE_DM
#include "cis_api.h"
#include "dm_endpoint.h"
#include "sha256.h"
#include "j_base64.h"
#include "aes.h"
#if _MSC_VER
#define snprintf _snprintf
#endif

void dmSdkInit(void *DMconfig)
{
    //to init g_opt
    Options *g_opt = (Options *) DMconfig;
    cis_memset(g_opt->szCMEI_IMEI, 0x00, sizeof(g_opt->szCMEI_IMEI));
    cissys_getIMEI(g_opt->szCMEI_IMEI, sizeof(g_opt->szCMEI_IMEI));

    cis_memset(g_opt->szIMSI, 0x00, sizeof(g_opt->szIMSI));
    cissys_getIMSI(g_opt->szIMSI, sizeof(g_opt->szIMSI));
}

#if 1

#define AES_BLOCK_SIZE 16

int my_aes_encrypt(char* enckey, char* encbuf, char* decbuf, int inlen, int* outlen)

{
    char key[34] = { 0 }; // = "12345678"
    uint8_t iv[16] = { 0 };

    cis_memset(key,0,sizeof(key));
    cis_memcpy(key, enckey, strlen(enckey));

    int nLen = inlen;//input_string.length();
    int nBei;
    if ((!encbuf) || (!decbuf))
    {
        return -1;
    }

    nBei = nLen / AES_BLOCK_SIZE + 1;
    int nTotal = nBei * AES_BLOCK_SIZE;

    uint8_t *enc_s = ( unsigned char*)cissys_malloc(nTotal);

#ifdef DEBUG_INFO
    printf("de enc_s=%p,size=%d,len=%d\n",enc_s,nTotal,nLen);
#endif

    if (enc_s==NULL)
    {
        printf("enc_s mem err\n");
        return -1;
    }

    int nNumber;
    if (nLen % AES_BLOCK_SIZE > 0)
    {
        nNumber = nTotal - nLen;
    }
    else
    {
        nNumber = AES_BLOCK_SIZE;
    }

    cis_memset(enc_s, nNumber, nTotal);
    cis_memcpy(enc_s, encbuf, nLen);

    mbedtls_aes_context ctx;
    dm_mbedtls_aes_init( &ctx );
    dm_mbedtls_aes_setkey_enc(&ctx, (const unsigned char *)key, 256);
    dm_mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_ENCRYPT, nBei * AES_BLOCK_SIZE, iv, enc_s, (unsigned char *)decbuf );
    dm_mbedtls_aes_free(&ctx );

    *outlen = nBei * AES_BLOCK_SIZE;

    cis_free(enc_s);
    enc_s = NULL;

    return 0;
}
#endif

void my_sha256(const char *src,int srclen, char *resultT )
{
    dm_mbedtls_sha256_ret( src,srclen,resultT,0);
    return ;
}


int genDmRegEndpointName(char ** data,void *dmconfig)
{
#define EP_MEM_SIZE  (264)
    char key[64] = {0};
    char *szEpname = NULL;
    char *ciphertext = NULL;
    char *szStars="****";
    char *name = "";
    int  ciphertext_len;
    int  ret = 0;
    char *base64tmp = NULL;
    char *epnametmp = NULL;
    int  i = 0;
    char *passwd ="00000000000000000000000000000000";
    char iv[16] = { 0 };
    Options *g_opt = (Options *)dmconfig;

    cis_memset(iv, 0, 16);
    for (i = 0; i < 16; ++i)
    {
        iv[i] = 0;
    }

    szEpname=( char *)cissys_malloc(EP_MEM_SIZE);
    if (szEpname == NULL)
    {
        printf("mem err r1\n");
        ret=-1;
        goto out;
    }

    ciphertext=( char *)cissys_malloc(EP_MEM_SIZE);
    if (ciphertext == NULL)
    {
        printf("mem err r2\n");
        ret=-1;
        goto out;
    }

    cis_memset(ciphertext, 0, EP_MEM_SIZE);
    cis_memset(szEpname, 0, EP_MEM_SIZE);
    sprintf(szEpname,"%s-%s-%s-%s",
        strlen((const char *)(g_opt->szCMEI_IMEI)) > 0 ? g_opt->szCMEI_IMEI:szStars,
        strlen((const char *)(g_opt->szCMEI_IMEI2)) > 0 ? g_opt->szCMEI_IMEI2:szStars,
        strlen((const char *)(g_opt->szIMSI)) > 0 ? g_opt->szIMSI:szStars,
        strlen((const char *)(g_opt->szDMv)) > 0 ? g_opt->szDMv:szStars);

    if (strlen((const char *)name) <= 0)
    {
        name=szEpname;
    }

    if (strlen((const char *)(g_opt->szPwd)) > 0)
    {
        passwd = g_opt->szPwd;
    }
    else
    {
        printf("pwd is null,use default pwd is:%s~\n", passwd);
    }

    my_sha256(passwd,strlen((const char *)passwd),key);

    /* A 128 bit IV */
    /* Buffer for the decrypted text */
    char *plaintext =  "plaintext";
    plaintext = name;

    /* Encrypt the plaintext */
    my_aes_encrypt((char *)key, (char *)plaintext, ciphertext, strlen((const char *)plaintext), &ciphertext_len);

    name = ciphertext; //???????
    //base64
    char *testbase64 = "123456789";//MTIzNDU2Nzg5
    testbase64 = name;

    base64tmp = (char *)cissys_malloc(EP_MEM_SIZE);//szEpname is free now,use again;
    if (base64tmp == NULL)
    {
        printf("mem err r4\n");ret=-1;
        goto out;
    }
    cis_memset(base64tmp, 0, EP_MEM_SIZE);

    unsigned char *encData = 0;
    int encDataLen = 0;
    ret = j_base64_encode((unsigned char *)testbase64, ciphertext_len, &encData, (unsigned int *)&encDataLen);
    cis_memcpy(base64tmp, encData, encDataLen);
    j_base64_free(encData, encDataLen);

    epnametmp = ( char *)cissys_malloc(EP_MEM_SIZE*4);
    if (epnametmp == NULL)
    {
        printf("mem err\n,3");ret=-1;
        goto out;
    }
    cis_memset(epnametmp, 0, EP_MEM_SIZE*4);

    snprintf((char *)epnametmp, EP_MEM_SIZE*4,"L@#@%s@#@%s@#@%s@#@%s", base64tmp, g_opt->szAppKey, g_opt->szCMEI_IMEI, g_opt->szDMv);
    name = epnametmp; //?????????

    *data = (char *)cissys_malloc(strlen((const char *)name)+1);
    if (*data == NULL)
    {
        ret=-1;
        goto out;
    }

    cis_memset(*data, 0, strlen((const char *)name)+1);
    cis_memcpy(*data, name, strlen((const char *)name));
    ret = 0;

out:
    if (szEpname)
    {
        cis_free(szEpname);
        szEpname = NULL;
    }

    if (ciphertext)
    {
        cis_free(ciphertext);
        ciphertext = NULL;
    }

    if (epnametmp)
    {
        cis_free(epnametmp);
        epnametmp = NULL;
    }

    if (base64tmp)
    {
        cis_free(base64tmp);
        base64tmp = NULL;
    }
    return ret;
}

int genDmUpdateEndpointName(char **data,void *dmconfig)
{
#define EP_MEM_SIZE  (264)
    char key[64] = {0};
    char *szEpname = NULL;
    char *ciphertext = NULL;
    char *szStars="****";
    char * name = "";
    int  ciphertext_len;
    char *base64tmp = NULL;
    char *epnametmp = NULL;
    int ret = -1;
    char *passwd = "00000000000000000000000000000000";
    Options *g_opt=(Options *)dmconfig;

    szEpname=( char *)cissys_malloc(EP_MEM_SIZE);
    if (szEpname == NULL)
    {
        printf("mem err u1\n");
        ret=-1;
        goto out;
    }

    ciphertext=( char *)cissys_malloc(EP_MEM_SIZE);
    if (ciphertext == NULL)
    {
        printf("mem err u2\n");
        ret=-1;
        goto out;
    }

    cis_memset(ciphertext,0,EP_MEM_SIZE);
    cis_memset(szEpname,0,EP_MEM_SIZE);
    snprintf((char *)szEpname,EP_MEM_SIZE,"%s-%s-%s-%s",
        strlen((const char *)(g_opt->szCMEI_IMEI))>0?g_opt->szCMEI_IMEI:szStars,
        strlen((const char *)(g_opt->szCMEI_IMEI2))>0?g_opt->szCMEI_IMEI2:szStars,
        strlen((const char *)(g_opt->szIMSI))>0?g_opt->szIMSI:szStars,szStars);

    if (strlen((const char *)name) <= 0)
    {
        name = szEpname;
    }

    if (strlen((const char *)(g_opt->szPwd)) > 0)
    {
        passwd = g_opt->szPwd;
    }
    else
    {
        printf("pwd is null,use default pwd is:%s~\n", passwd);
    }

    my_sha256(passwd,strlen((const char *)passwd),key);

    /* A 128 bit IV */
    /* Buffer for the decrypted text */
    char *plaintext =  "plaintext";

    plaintext = name;

    /* Encrypt the plaintext */
    my_aes_encrypt((char *)key, plaintext, ciphertext, strlen((const char *)plaintext), &ciphertext_len);
    name = ciphertext; //???????

    //base64
     char *testbase64="123456789";//MTIzNDU2Nzg5
    testbase64 = name;
    base64tmp=( char *)cissys_malloc(EP_MEM_SIZE);//szEpname is free now,use again;
    if (base64tmp==NULL)
    {
        printf("mem err u4\n");
        goto out;
    }

    cis_memset(base64tmp, 0, EP_MEM_SIZE);
    unsigned char *encData = 0;
    int encDataLen = 0;
    ret= j_base64_encode((unsigned char *)testbase64, ciphertext_len, &encData,(unsigned int *)&encDataLen);
    cis_memcpy(base64tmp, encData, encDataLen);
    j_base64_free(encData, encDataLen);

    epnametmp = (char *)cissys_malloc(EP_MEM_SIZE*4);
    if (epnametmp == NULL)
    {
        printf("mem err u3\n");ret=-1;
        goto out;
    }
    cis_memset(epnametmp,0,EP_MEM_SIZE*4);

    sprintf((char *)epnametmp,"L@#@%s@#@%s@#@%s@#@%s", base64tmp, g_opt->szAppKey, g_opt->szCMEI_IMEI, g_opt->szDMv);

    name = epnametmp;
    *data = (char *)cissys_malloc(strlen((const char *)name)+1);
    if (*data == NULL)
    {
        ret=-1;
        goto out;
    }

    cis_memset(*data, 0, strlen((const char *)name)+1);
    cis_memcpy(*data, name, strlen((const char *)name));
    ret = 0;

out:
    if (szEpname)
    {
        cis_free(szEpname);
        szEpname=NULL;
    }

    if (ciphertext)
    {
        cis_free(ciphertext);
        ciphertext=NULL;
    }

    if (epnametmp)
    {
        cis_free(epnametmp);
        epnametmp=NULL;
    }

    if (base64tmp)
    {
        cis_free(base64tmp);
        base64tmp=NULL;
    }
    return ret;
}

int prv_getDmUpdateQueryLength(st_context_t * contextP,
                                          st_server_t * server)
{
    int index;

    index = strlen("epi=");
    index += strlen((const char *)(contextP->DMprivData));
    return index + 1;
}

int prv_getDmUpdateQuery(st_context_t * contextP,
                                    st_server_t * server,
                                    uint8_t * buffer,
                                    size_t length)
{
    int index;
    int res;

    index = utils_stringCopy((char *)buffer, length, "epi=");
    if (index < 0) return 0;
    res = utils_stringCopy((char *)buffer + index, length - index, (const char *)(contextP->DMprivData));
    if (res < 0) return 0;
    index += res;

    if(index < (int)length)
    {
        buffer[index++] = '\0';
    }
    else
    {
        return 0;
    }

    return index;
}

int dm_read_info_encode(char *szin,char **szout, char *pwd)
{
#define EP_MEM_SIZE  (264)
    char key[64] = {0};
    char *szEpname = NULL;
    char *ciphertext = NULL;
    char *name = "";
    int  ciphertext_len;
    int  ret = 0;
    char *base64tmp = NULL;
    char *epnametmp = NULL;
    char *passwd ="00000000000000000000000000000000";

    szEpname=(char *)cissys_malloc(EP_MEM_SIZE);
    if (szEpname == NULL)
    {
        ret = -1;
        goto out;
    }

    ciphertext=(char *)cissys_malloc(EP_MEM_SIZE);
    if (ciphertext == NULL)
    {
        ret = -1;
        goto out;
    }

    cis_memset(ciphertext, 0, EP_MEM_SIZE);
    cis_memset(szEpname, 0, EP_MEM_SIZE);
    sprintf(szEpname, "%s", szin);

    if (strlen((const char *)name) <= 0)
    {
        name = szEpname;
    }

    if (strlen((const char *)(pwd)) > 0)
    {
        passwd = pwd;
    }
    else
    {
        LOGD("dm_read_info_encode pwd is null, use default passwd");
    }

    my_sha256(passwd,strlen((const char *)passwd),key);

    /* A 128 bit IV */
    /* Buffer for the decrypted text */
    char *plaintext =  "plaintext";
    plaintext = name;

    /* Encrypt the plaintext */
    my_aes_encrypt((char *)key, (char *)plaintext, ciphertext, strlen((const char *)plaintext), &ciphertext_len);

    name = ciphertext; //???????
    //base64
    char *testbase64 = "123456789";//MTIzNDU2Nzg5
    testbase64 = name;

    base64tmp = (char *)cissys_malloc(EP_MEM_SIZE);//szEpname is free now,use again;
    if (base64tmp == NULL)
    {
        LOGD("mem err r4\n");ret=-1;
        goto out;
    }
    cis_memset(base64tmp, 0, EP_MEM_SIZE);

    unsigned char *encData = 0;
    int encDataLen = 0;
    ret = j_base64_encode((unsigned char *)testbase64, ciphertext_len, &encData, (unsigned int *)&encDataLen);
    cis_memcpy(base64tmp, encData, encDataLen);
    j_base64_free(encData, encDataLen);

    epnametmp = ( char *)cissys_malloc(EP_MEM_SIZE*4);
    if (epnametmp == NULL)
    {
        ret = -1;
        goto out;
    }
    cis_memset(epnametmp, 0, EP_MEM_SIZE*4);

    snprintf((char *)epnametmp, EP_MEM_SIZE*4,"%s", base64tmp);
    name = epnametmp;

    *szout = (char *)cissys_malloc(strlen((const char *)name)+1);
    if (*szout == NULL)
    {
        ret=-1;
        goto out;
    }

    cis_memset(*szout, 0, strlen((const char *)name)+1);
    cis_memcpy(*szout, name, strlen((const char *)name));
    ret = 0;
out:
    if (szEpname)
    {
        cis_free(szEpname);
        szEpname = NULL;
    }

    if (ciphertext)
    {
        cis_free(ciphertext);
        ciphertext = NULL;
    }

    if (epnametmp)
    {
        cis_free(epnametmp);
        epnametmp = NULL;
    }

    if (base64tmp)
    {
        cis_free(base64tmp);
        base64tmp = NULL;
    }
    return ret;
}

#endif
