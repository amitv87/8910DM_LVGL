/* Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
 * All rights reserved.
 *
 * This software is supplied "AS IS" without any warranties.
 * RDA assumes no responsibility or liability for the use of the software,
 * conveys no license or title under any patent, copyright, or mask work
 * right to the product. RDA reserves the right to make changes in the
 * software without notification.  RDA also make no representation or
 * warranty that such application will be suitable for the specified use
 * without further testing or modification.
 */
#include "atr_config.h"
#include "cfw_config.h"
#include "cfw_chset.h"
#include "osi_log.h"
#include "at_cfg.h"
#include "at_cfw.h"
#include "cfw.h"
#include "cfw_errorcode.h"
#include "ml.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <inttypes.h>

#include "osi_api.h"
#include "vfs.h"
//#include "base.h"
//#include "cfw_cfg.h"
#include "cfw_event.h"

void AT_PBK_Init(uint8_t nSim)
{
    return;
}
void atCmdHandleCPBW(atCommand_t *cmd)
{
    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
}
void atCmdHandleCPBR(atCommand_t *cmd)
{
    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
}
void atCmdHandleCPBS(atCommand_t *cmd)
{
    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
}
void atCmdHandleCPBF(atCommand_t *cmd)
{
    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
}

bool AT_UnicodeBigEnd2Unicode(uint8_t *pUniBigData, uint8_t *pUniData, uint16_t nDataLen)
{
    /* check the pointer is NULL or not */
    if ((NULL == pUniData) || (NULL == pUniBigData))
    {
        return false;
    }

    /* there is odd chars and we make
       ** it even, discard the last char */
    if (nDataLen % 2)
    {
        nDataLen = nDataLen - 1;
    }

    while (nDataLen > 0)
    {
        pUniData[nDataLen - 2] = pUniBigData[nDataLen - 1];
        pUniData[nDataLen - 1] = pUniBigData[nDataLen - 2];

        nDataLen -= 2;
    }

    return true;
}

bool AT_PBK_ProcessOutputName(uint8_t *arrDest, uint8_t nDestLen, uint8_t *arrSource, uint8_t *pLen)
{
    char *pTempTextString = NULL; /* include one char: '\0' */
    uint8_t pUCSLittleEndData[128];

    if ((arrDest == NULL) || (arrSource == NULL))
    {
        OSI_LOGI(0x1000479e, "AT_PBK_ProcessOutputName: input pointer is NULL");
        return false;
    }

    if (0x80 == arrSource[0])
    {
        if (!(AT_UnicodeBigEnd2Unicode((uint8_t *)(&arrSource[1]), pUCSLittleEndData, (uint16_t)(*pLen - 1))))
        {
            OSI_LOGI(0, "AT_PBK_ProcessOutputName: BigEnd to LittleEnd exception");
            return false;
        }

        pTempTextString = mlConvertStr(pUCSLittleEndData, (int)(*pLen - 1), cs_ucs2, gAtSetting.cscs, NULL);
        if (pTempTextString == NULL)
        {
            OSI_LOGI(0, "AT_PBK_ProcessOutputName: ucs2 to local language exception");
            return false;
        }

        /*if (strlen(pTempTextString) > nDestLen)
        {
            OSI_LOGI(0, "AT_PBK_ProcessOutputName: text size > dest buffer size");
            return false;
        }*/

        memcpy(arrDest, (uint8_t *)pTempTextString, strlen(pTempTextString));

        *pLen = strlen(pTempTextString);

        // Free inout para resource
        free(pTempTextString);
    }
    else
    {
        if (*pLen > nDestLen)
        {
            OSI_LOGI(0, "AT_PBK_ProcessOutputName: source size > dest buffer size");
            return false;
        }

        memcpy(arrDest, arrSource, *pLen);
    }

    return true;
}

typedef struct
{
    uint16_t g_nListEntryMaxIndex;    /* Begin list entry index */
    uint16_t g_nListEntryMinIndex;    /* End list entry num */
    uint16_t g_nListEntryLastStep;    /* step length */
    uint16_t g_nListEntryStepCounter; /* list step counter */
    uint16_t g_nMaxNumberSize;        /* number length of liner file */
    uint8_t g_nMaxTextSize;           /* text length of liner file */
} CNUMAsyncCtx_t;

static void _cnumRspCB(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    uint8_t nSim = cfw_event->nFlag;
    uint32_t nErrorCode = ERR_SUCCESS;

    if (cfw_event->nType == 0xf0)
    {
        nErrorCode = AT_SetCmeErrorCode(cfw_event->nParam1);
        RETURN_CME_ERR(cmd->engine, nErrorCode);
    }
    if (cfw_event->nEventId == EV_CFW_SIM_GET_PBK_STRORAGE_INFO_RSP)
    {
        CFW_PBK_STRORAGE_INFO *pStorageInfo = NULL;
        pStorageInfo = (CFW_PBK_STRORAGE_INFO *)cfw_event->nParam1;
        CNUMAsyncCtx_t *async = (CNUMAsyncCtx_t *)malloc(sizeof(*async));
        if (async == NULL)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);

        atMemFreeLater(pStorageInfo);

        cmd->async_ctx = async;
        cmd->async_ctx_destroy = atCommandAsyncCtxFree;

        async->g_nListEntryLastStep = 0;
        async->g_nListEntryMaxIndex = 0;
        async->g_nListEntryMinIndex = 0;
        async->g_nListEntryStepCounter = 0;

        async->g_nListEntryMinIndex = 1;
        async->g_nListEntryMaxIndex = pStorageInfo->index;
        async->g_nListEntryStepCounter++;
        async->g_nMaxNumberSize = (pStorageInfo->iNumberLen) * 2;
        async->g_nMaxTextSize = pStorageInfo->txtLen;

        cmd->uti = cfwRequestUTI((osiEventCallback_t)_cnumRspCB, cmd);
        if ((async->g_nListEntryMaxIndex - async->g_nListEntryMinIndex + 1) <= (AT_PBK_LIST_ENTRY_STEP * async->g_nListEntryStepCounter))
        {
            async->g_nListEntryLastStep = 1;
            nErrorCode = CFW_SimListPbkEntries(CFW_PBK_ON, async->g_nListEntryMinIndex, async->g_nListEntryMaxIndex, cmd->uti, nSim);
        }
        else
        {
            nErrorCode = CFW_SimListPbkEntries(CFW_PBK_ON, async->g_nListEntryMinIndex, AT_PBK_LIST_ENTRY_STEP, cmd->uti, nSim);
        }
        if (ERR_SUCCESS != nErrorCode)
        {
            cfwReleaseUTI(cmd->uti);
            nErrorCode = AT_SetCmeErrorCode(nErrorCode);
            RETURN_CME_ERR(cmd->engine, nErrorCode);
        }
    }
    if (cfw_event->nEventId == EV_CFW_SIM_LIST_PBK_ENTRY_RSP)
    {
        CFW_SIM_PBK_ENTRY_INFO *pPbkEntryInfo = NULL;
        uint8_t nEntryCount = 0;
        char MarkCmdStampBuff[8] = {0};
        char PromptBuff[128] = {0};
        CNUMAsyncCtx_t *async = (CNUMAsyncCtx_t *)cmd->async_ctx;

        nEntryCount = cfw_event->nParam2;
        pPbkEntryInfo = (CFW_SIM_PBK_ENTRY_INFO *)cfw_event->nParam1;
        strcpy(MarkCmdStampBuff, "+CNUM");

        if (nEntryCount != 0)
        {
            atMemFreeLater(pPbkEntryInfo);
        }
        // ////////////////////////////////////////////////
        // **         have Entry found
        // ////////////////////////////////////////////////
        while (nEntryCount-- > 0)
        {
            char InternationBuff[2] = {0};

            uint8_t LocolLangNumber[SIM_PBK_NUMBER_SIZE * 2 + 1] = {0}; /* include one char: '\0' */
            //uint32_t nLocolLangNumLen = 0;

            uint8_t pLocolLangTextString[SIM_PBK_NAME_SIZE + 1] = {0}; /* include one char: '\0' */
            uint8_t pOutputTextString[SIM_PBK_NAME_SIZE + 1] = {0};    /* include one char: '\0' */
            pPbkEntryInfo->pNumber = (uint8_t *)pPbkEntryInfo + (uint32_t)sizeof(CFW_SIM_PBK_ENTRY_INFO);
            pPbkEntryInfo->pFullName = (uint8_t *)pPbkEntryInfo + (uint32_t)(sizeof(CFW_SIM_PBK_ENTRY_INFO) + SIM_PBK_NUMBER_SIZE);

            /* clear buffer of prompt  */
            memset(PromptBuff, 0, sizeof(PromptBuff));

            /* check the input number sizeless than Max number size or not
               ** if more than the Max size, then discard part of them*/

            if (2 * (pPbkEntryInfo->nNumberSize) > async->g_nMaxNumberSize)
            {
                pPbkEntryInfo->nNumberSize = async->g_nMaxNumberSize / 2;
            }

            /* check the input text length less than Max text size or not
               ** if more than the Max size, then discard part of them*/

            if (pPbkEntryInfo->iFullNameSize > async->g_nMaxTextSize)
            {
                pPbkEntryInfo->iFullNameSize = async->g_nMaxTextSize;
            }

            // BCD to asscii
            SUL_GsmBcdToAscii(pPbkEntryInfo->pNumber, pPbkEntryInfo->nNumberSize, LocolLangNumber);

            /* process text string from csw  */
            if (!(AT_PBK_ProcessOutputName(pOutputTextString, sizeof(pOutputTextString), pPbkEntryInfo->pFullName, &(pPbkEntryInfo->iFullNameSize))))
            {
                OSI_LOGI(0, "ListPbkEntry resp: some message node exception");

                /* get list next node, then continue */
                goto label_next_list_node;
            }

            /* buffer overflow */
            if (pPbkEntryInfo->iFullNameSize > sizeof(pLocolLangTextString))
            {
                OSI_LOGI(0, "ListPbkEntry resp:name size of message node more than local buffer len");

                /* get list next node, then continue */
                goto label_next_list_node;
            }

            /* save the string to local language string buffer */
            memcpy(pLocolLangTextString, pOutputTextString, pPbkEntryInfo->iFullNameSize);

            /* international or not */
            if (CFW_TELNUMBER_TYPE_INTERNATIONAL == pPbkEntryInfo->nType)
            {
                strcpy(InternationBuff, "+");
            }

            // ///////////////////////////////////////////////////////////
            /* Get the AT command type                  */
            // ///////////////////////////////////////////////////////////

            sprintf(PromptBuff, "%s: \"%s\",\"%s%s\",%u", MarkCmdStampBuff, pLocolLangTextString,
                    InternationBuff, LocolLangNumber, pPbkEntryInfo->nType);
            atCmdRespInfoText(cmd->engine, PromptBuff);
            // ///////////////////////////////////
            // Get the next list node
            // ///////////////////////////////////
        label_next_list_node:
            pPbkEntryInfo =
                (CFW_SIM_PBK_ENTRY_INFO *)((uint32_t)pPbkEntryInfo + sizeof(CFW_SIM_PBK_ENTRY_INFO) + SIM_PBK_EXTR_SIZE);
        }

        /*
           **check multi list entry finished or not
         */
        if (1 == async->g_nListEntryLastStep)
        {
            RETURN_OK(cmd->engine);
        }
        else
        {
            /* Length is reach the end point and finished this time */
            if ((async->g_nListEntryMaxIndex - async->g_nListEntryMinIndex + 1) <= AT_PBK_LIST_ENTRY_STEP * (async->g_nListEntryStepCounter + 1))
            {
                async->g_nListEntryLastStep = 1;
                cmd->uti = cfwRequestUTI((osiEventCallback_t)_cnumRspCB, cmd);
                nErrorCode = CFW_SimListPbkEntries(CFW_PBK_ON,
                                                   async->g_nListEntryMinIndex + AT_PBK_LIST_ENTRY_STEP * async->g_nListEntryStepCounter,
                                                   async->g_nListEntryMaxIndex,
                                                   cmd->uti, nSim);
            }

            /* have some items to be continue to list  */
            else
            {
                cmd->uti = cfwRequestUTI((osiEventCallback_t)_cnumRspCB, cmd);
                nErrorCode = CFW_SimListPbkEntries(CFW_PBK_ON,
                                                   async->g_nListEntryMinIndex + AT_PBK_LIST_ENTRY_STEP * async->g_nListEntryStepCounter,
                                                   async->g_nListEntryMinIndex + AT_PBK_LIST_ENTRY_STEP * (async->g_nListEntryStepCounter + 1) - 1,
                                                   cmd->uti, nSim);
                async->g_nListEntryStepCounter++;
            }
            if (ERR_SUCCESS != nErrorCode)
            {
                cfwReleaseUTI(cmd->uti);
                nErrorCode = AT_SetCmeErrorCode(nErrorCode);
                RETURN_CME_ERR(cmd->engine, nErrorCode);
            }
        }
    }
    return;
}

void atCmdHandleCNUM(atCommand_t *cmd)
{
    uint8_t nSim = atCmdGetSim(cmd->engine);

    if (cmd->type == AT_CMD_TEST)
    {
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_EXE)
    {
        uint32_t nGetStorageResult = ERR_SUCCESS;

        cmd->uti = cfwRequestUTI((osiEventCallback_t)_cnumRspCB, cmd);
        nGetStorageResult = CFW_SimGetPbkStrorageInfo(CFW_PBK_ON, cmd->uti, nSim);
        if (ERR_SUCCESS != nGetStorageResult)
        {
            cfwReleaseUTI(cmd->uti);
            nGetStorageResult = AT_SetCmeErrorCode(nGetStorageResult);
            RETURN_CME_ERR(cmd->engine, nGetStorageResult);
        }
        RETURN_FOR_ASYNC();
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}

#if 0
static char g_arrFindText[SIM_PBK_NAME_SIZE + 3] = {0}; /* Find text content buffer */
static uint16_t g_nFindStrLen = 0;                      /* Find text length */

static uint8_t g_nMaxNumberSize = 0;  /* Max number length, "on","ld","fd","sm" are the same value */
static uint8_t g_nMaxTextSize = 0;    /* Max PBK name length,  "on","ld","fd","sm" are the same value */
static uint16_t g_nMaxSMPBKIndex = 0; /* Max SM PBK(default pbk) index value */

#define API_AT_GETPBK 0
#define AT_SIM_REMAINLENGTH 14
#define MAX_ME_PHONE_ENTRIES 100
#define NV_ME_PBK_INFO_BASE 0
#define NV_ME_DEFAULT_VALUE 0xff

#define NV_ME_STORGE_POS NV_ME_PBK_INFO_BASE
#define NV_ME_STORGE_SIZE 1
#define NV_ME_TYPE_POS (NV_ME_STORGE_POS + NV_ME_STORGE_SIZE)
#define NV_ME_TYPE_SIZE 1
#define NV_ME_INDEX_POS (NV_ME_TYPE_POS + NV_ME_TYPE_SIZE)
#define NV_ME_INDEX_SIZE 4
#define NV_ME_TEL_POS (NV_ME_INDEX_POS + NV_ME_INDEX_SIZE)
#define NV_ME_TEL_SIZE ME_PBK_NUMBER_SIZE + 1
#define NV_ME_NAME_POS (NV_ME_TEL_POS + NV_ME_TEL_SIZE)
#define NV_ME_NAME_SIZE ME_PBK_NAME_SIZE + 3
#define NV_ME_TEL_LEN_POS (NV_ME_NAME_POS + NV_ME_NAME_SIZE)
#define NV_ME_TEL_LEN_SIZE 1
#define NV_ME_NAME_LEN_POS (NV_ME_TEL_LEN_POS + NV_ME_TEL_LEN_SIZE)
#define NV_ME_NAME_LEN_SIZE 1
#define NV_ME_BUFF_LEN (NV_ME_NAME_LEN_POS + NV_ME_NAME_LEN_SIZE)

#define NV_ME_WRITE_ADD 0x01
#define NV_ME_WRITE_DEL 0x02

#define EF_ENABLE 1
#define EF_DISABLE 0

AT_FDN_PBK_LIST *pgATFNDlist[CFW_SIM_COUNT] = {NULL};

//from cfw_sim_pbk_nv.h
//add for ME PBK
#define PBK_NV_VERSION 0x20171024
#define MAX_ME_PHONE_ENTRIES 100
#define NV_ME_PBK_INFO_BASE 0
#define NV_ME_DEFAULT_VALUE 0xff

#define ME_PBK_NUMBER_SIZE 10
#define ME_PBK_NAME_SIZE 14

#define NV_ME_STORGE_POS NV_ME_PBK_INFO_BASE
#define NV_ME_STORGE_SIZE 1
#define NV_ME_TYPE_POS (NV_ME_STORGE_POS + NV_ME_STORGE_SIZE)
#define NV_ME_TYPE_SIZE 1
#define NV_ME_INDEX_POS (NV_ME_TYPE_POS + NV_ME_TYPE_SIZE)
#define NV_ME_INDEX_SIZE 4
#define NV_ME_TEL_POS (NV_ME_INDEX_POS + NV_ME_INDEX_SIZE)
#define NV_ME_TEL_SIZE ME_PBK_NUMBER_SIZE + 1
#define NV_ME_NAME_POS (NV_ME_TEL_POS + NV_ME_TEL_SIZE)
#define NV_ME_NAME_SIZE ME_PBK_NAME_SIZE + 3
#define NV_ME_TEL_LEN_POS (NV_ME_NAME_POS + NV_ME_NAME_SIZE)
#define NV_ME_TEL_LEN_SIZE 1
#define NV_ME_NAME_LEN_POS (NV_ME_TEL_LEN_POS + NV_ME_TEL_LEN_SIZE)
#define NV_ME_NAME_LEN_SIZE 1
#define NV_ME_BUFF_LEN (NV_ME_NAME_LEN_POS + NV_ME_NAME_LEN_SIZE)

#define NV_ME_WRITE_ADD 0x01
#define NV_ME_WRITE_DEL 0x02

typedef struct
{
    uint8_t storage; /*0-none, 1-SIM, 2-ME*/
    uint8_t type;
    uint16_t index;
    uint8_t tel[ME_PBK_NUMBER_SIZE + 1];
    uint8_t name[ME_PBK_NAME_SIZE + 3];
    uint8_t tel_len;
    uint8_t name_len;
} NV_ME_PHB_INFO;

typedef uint8_t PBK_REC[NV_ME_BUFF_LEN];
typedef struct
{
    uint32_t version;
    uint8_t usenum;
    PBK_REC pbkRec[MAX_ME_PHONE_ENTRIES];
} NV_PBK;

NV_PBK g_pbk_nv = {0};
bool g_pbk_is_init = false;

//from cfw_sim_pbk_nv.c
#define MAKEUINT16(a, b) ((uint16_t)(((uint8_t)(a)) | ((uint16_t)((uint8_t)(b))) << 8))
#define CFW_NV_PBK_FILE "cfw_nv_pbk.bin"
static void PBKI2Buf(NV_ME_PHB_INFO *pbki, uint8_t *buf)
{
    memcpy(&buf[NV_ME_STORGE_POS], &(pbki->storage), NV_ME_STORGE_SIZE);
    memcpy(&buf[NV_ME_TYPE_POS], &(pbki->type), NV_ME_TYPE_SIZE);
    memcpy(&buf[NV_ME_INDEX_POS], &(pbki->index), NV_ME_INDEX_SIZE);
    memcpy(&buf[NV_ME_TEL_POS], &(pbki->tel), NV_ME_TEL_SIZE);
    memcpy(&buf[NV_ME_NAME_POS], &(pbki->name), NV_ME_NAME_SIZE);
    memcpy(&buf[NV_ME_TEL_LEN_POS], &(pbki->tel_len), NV_ME_TEL_LEN_SIZE);
    memcpy(&buf[NV_ME_NAME_LEN_POS], &(pbki->name_len), NV_ME_NAME_LEN_SIZE);
}

static void Buf2PBKI(uint8_t *buf, NV_ME_PHB_INFO *pbki)
{
    pbki->storage = buf[NV_ME_STORGE_POS];
    pbki->type = buf[NV_ME_TYPE_POS];
    pbki->index = MAKEUINT16((buf)[NV_ME_INDEX_POS],
                             (buf)[NV_ME_INDEX_POS + 1]);
    memcpy(pbki->tel, (uint8_t *)&buf[NV_ME_TEL_POS], NV_ME_TEL_SIZE);
    memcpy(pbki->name, (uint8_t *)&buf[NV_ME_NAME_POS], NV_ME_NAME_SIZE);
    pbki->tel_len = buf[NV_ME_TEL_LEN_POS];
    pbki->name_len = buf[NV_ME_NAME_LEN_POS];
}

static int32_t NV_UpdatePBK(void)
{
#ifndef CONFIG_CFW_SKIP_NV_SAVE_RESTORE
    const char *cfg_path = CFW_NV_PBK_FILE;
    ssize_t result = vfs_sfile_write(cfg_path, (void *)&g_pbk_nv, sizeof(NV_PBK));
    if (result <= 0)
        return -1;
#endif
    return 0;
}

static int32_t NV_READ_ME_PBK(uint32_t iIndex, uint8_t *pBuff)
{
    if (!g_pbk_is_init)
        return -1;

    memcpy(pBuff, g_pbk_nv.pbkRec[iIndex - 1], NV_ME_BUFF_LEN);
    return ERR_SUCCESS;
}

static int32_t NV_WRITE_ME_PBK(uint32_t iIndex, uint8_t *pBuff, uint8_t type)
{
    if (!g_pbk_is_init)
        return -1;

    int32_t result;
    uint8_t pBuff_r[NV_ME_BUFF_LEN];
    NV_ME_PHB_INFO sPBKI = {0};
    memcpy(pBuff_r, (uint8_t *)g_pbk_nv.pbkRec[iIndex - 1], NV_ME_BUFF_LEN);
    Buf2PBKI(pBuff_r, &sPBKI);
    if (sPBKI.index != (NV_ME_DEFAULT_VALUE << 8 | NV_ME_DEFAULT_VALUE) && type == NV_ME_WRITE_DEL)
    {
        g_pbk_nv.usenum -= 1;
    }
    else if (sPBKI.index == (NV_ME_DEFAULT_VALUE << 8 | NV_ME_DEFAULT_VALUE) && type == NV_ME_WRITE_ADD)
    {
        g_pbk_nv.usenum += 1;
    }

    memcpy((uint8_t *)g_pbk_nv.pbkRec[iIndex - 1], pBuff, NV_ME_BUFF_LEN);
    result = NV_UpdatePBK();
    if (result != 0)
    {
        return -1;
    }

    return ERR_SUCCESS;
}

static int32_t NV_GET_STORGE_ME_PBK(uint32_t iMaxEntries)
{
    if (!g_pbk_is_init)
        return -1;

    return g_pbk_nv.usenum;
}

char *strupr(char *pStr)
{
    char *p = pStr;

    if (p == NULL)
        return NULL;

    while (*p != '\0')
    {
        if (*p >= 'a' && *p <= 'z')
            *p = *p - 32;

        p++;
    }

    return pStr;
}

bool AT_PBK_String2Bytes(uint8_t *pDest, uint8_t *pSource, uint8_t *pLen)
{
    uint8_t nSourceLen = *pLen;
    uint8_t nTemp = 0;
    uint8_t nByteNumber = 0;

    if ((NULL == pSource) || (NULL == pDest))
    {
        OSI_LOGI(0x10003ef4, "AT_PBK_String2Bytes: pointer is NULL");
        return false;
    }

    OSI_LOGI(0x10003ef5, "AT_PBK_String2Bytes: nSourceLen = %u", nSourceLen);

    /* string char counter must be even */
    if (nSourceLen % 2)
    {
        OSI_LOGI(0, "AT_PBK_String2Bytes: source len is not even");
        return false;
    }

    while (nTemp < nSourceLen)
    {
        /* get high half byte */
        if ((pSource[nTemp] > 0x2f) && (pSource[nTemp] < 0x3a))
        {
            pDest[nByteNumber] = (pSource[nTemp] - '0') << 4;
        }
        else if ((pSource[nTemp] > 0x40) && (pSource[nTemp] < 0x47))
        {
            pDest[nByteNumber] = (pSource[nTemp] - 'A' + 10) << 4;
        }
        else if ((pSource[nTemp] > 0x60) && (pSource[nTemp] < 0x67))
        {
            pDest[nByteNumber] = (pSource[nTemp] - 'a' + 10) << 4;
        }
        else
        {
            OSI_LOGI(0x10003ef7, "high half byte more than 'F' of HEX: pSource[nTemp]: = %u ", pSource[nTemp]);
            return false;
        }

        /* get low half byte */
        nTemp++;

        if ((pSource[nTemp] > 0x2f) && (pSource[nTemp] < 0x3a))
        {
            pDest[nByteNumber] += (pSource[nTemp] - '0');
        }
        else if ((pSource[nTemp] > 0x40) && (pSource[nTemp] < 0x47))
        {
            pDest[nByteNumber] += (pSource[nTemp] - 'A' + 10);
        }
        else if ((pSource[nTemp] > 0x60) && (pSource[nTemp] < 0x67))
        {
            pDest[nByteNumber] += (pSource[nTemp] - 'a' + 10);
        }
        else
        {
            OSI_LOGI(0x10003ef8, "low half byte more than 'F' of HEX: pSource[nTemp]: = %u ", pSource[nTemp]);
            return false;
        }

        nTemp++;

        nByteNumber++;
    }

    pDest[nByteNumber] = '\0';

    *pLen = nByteNumber;

    return true;
}

void AT_FDN_AddRecord(uint8_t nIdx, CFW_SIM_PBK_ENTRY_INFO *pGetPBKEntry, uint8_t nSim)
{
    uint8_t realnum = 0;
    uint8_t i = 0;

    OSI_LOGI(0x100047ce, "AddRecord start nIdx:%d ,nSim:%d!\n", nIdx, nSim);

    realnum = pgATFNDlist[nSim]->nRealRecordNum;
    for (i = 0; i < realnum; i++)
    {
        if (nIdx == pgATFNDlist[nSim]->sRecord[i].nIndex)
        {
            break;
        }
    }
    memcpy(pgATFNDlist[nSim]->sRecord[i].nName, pGetPBKEntry->pFullName, pGetPBKEntry->iFullNameSize);
    memcpy(pgATFNDlist[nSim]->sRecord[i].nNumber, pGetPBKEntry->pNumber, pGetPBKEntry->nNumberSize);
    pgATFNDlist[nSim]->sRecord[i].nTpye = pGetPBKEntry->nType;
    if (i == realnum)
    {
        pgATFNDlist[nSim]->nRealRecordNum++;
        pgATFNDlist[nSim]->sRecord[i].nIndex = nIdx;
    }

    //just for testing
    if (pgATFNDlist[nSim]->nRealRecordNum > pgATFNDlist[nSim]->nTotalRecordNum)
    {
        //AT_ASSERT(0);
    }
    return;
}

void AT_FDN_DelRecord(uint8_t nIdx, uint8_t nSim)
{
    uint8_t realnum = 0;
    uint8_t nNext = 0;
    uint8_t i = 0;
    uint8_t j = 0;

    OSI_LOGI(0x100047cf, "DelRecord start nIdx:%d ,nSim:%d!\n", nIdx, nSim);
    //del FDN record
    realnum = pgATFNDlist[nSim]->nRealRecordNum;
    for (i = 0; i < realnum; i++)
    {
        if (nIdx == pgATFNDlist[nSim]->sRecord[i].nIndex)
        {
            break;
        }
    }
    if (i < realnum)
    {
        //del AT FDNlist Record
        pgATFNDlist[nSim]->nRealRecordNum--;
        for (j = i; j < realnum - 1; j++)
        {
            nNext = j + 1;
            memset(&pgATFNDlist[nSim]->sRecord[j], 0xff, sizeof(AT_FDN_PBK_RECORD));
            pgATFNDlist[nSim]->sRecord[j].nIndex = pgATFNDlist[nSim]->sRecord[nNext].nIndex;
            pgATFNDlist[nSim]->sRecord[j].nTpye = pgATFNDlist[nSim]->sRecord[nNext].nTpye;
            memcpy(pgATFNDlist[nSim]->sRecord[j].nName, pgATFNDlist[nSim]->sRecord[nNext].nName, AT_FDN_NAME_SIZE);
            memcpy(pgATFNDlist[nSim]->sRecord[j].nNumber, pgATFNDlist[nSim]->sRecord[nNext].nNumber, AT_FDN_NUMBER_SIZE);
        }
        //write last record to zero
        memset(&pgATFNDlist[nSim]->sRecord[realnum - 1], 0xff, sizeof(AT_FDN_PBK_RECORD));
    }
    return;
}

bool AT_PBK_IsValidPhoneNumber(uint8_t *arrNumber, uint8_t nNumberSize, bool *bIsInternational)
{
    uint8_t nTemp = 0;

    *bIsInternational = false;

    if ((NULL == arrNumber) || (0 == nNumberSize))

    {
        OSI_LOGI(0x100047a3, "AT_PBK_IsValidPhoneNumber: number or num size is 0");
        return false;
    }

    if (arrNumber[0] == '+')

    {
        *bIsInternational = true;
        nTemp = 1;
    }

    while (nTemp < nNumberSize)

    {
        if ((arrNumber[nTemp] != '#') && (arrNumber[nTemp] != '*') && ((arrNumber[nTemp] < '0') || (arrNumber[nTemp] > '9')))

        {
            OSI_LOGI(0x100047a4, "AT_PBK_IsValidPhoneNumber: invalid char in the number");
            return false;
        }

        nTemp++;
    }

    return true;
}

bool AT_PBK_SetPBKCurrentStorage(char *pStorageFlag)
{
    if (!strcmp(pStorageFlag, "SM"))

    {
        gAtSetting.tempStorage = CFW_PBK_SIM;
    }
    else if (!(char *)strcmp(pStorageFlag, "FD"))

    {
        gAtSetting.tempStorage = CFW_PBK_SIM_FIX_DIALLING;
    }
    else if (!(char *)strcmp(pStorageFlag, "LD"))

    {
        gAtSetting.tempStorage = CFW_PBK_SIM_LAST_DIALLING;
    }
    else if (!(char *)strcmp(pStorageFlag, "ON"))

    {
        gAtSetting.tempStorage = CFW_PBK_ON;
    }
    else if (!(char *)strcmp(pStorageFlag, "ME"))

    {
        gAtSetting.tempStorage = CFW_PBK_ME;
    }
    else

    {
        OSI_LOGI(0x100047a5, "AT_PBK_SetPBKCurrentStorage: storage invalid");
        return false;
    }

    return true;
}

bool AT_PBK_GetPBKCurrentStorage(uint16_t nStorageId, uint8_t *pStorageBuff)
{

    if (nStorageId == CFW_PBK_SIM)

    {
        strcpy((char *)pStorageBuff, "SM");
    }
    else if (nStorageId == CFW_PBK_ON)

    {
        strcpy((char *)pStorageBuff, "ON");
    }
    else if (nStorageId == CFW_PBK_SIM_FIX_DIALLING)

    {
        strcpy((char *)pStorageBuff, "FD");
    }
    else if (nStorageId == CFW_PBK_SIM_LAST_DIALLING)

    {
        strcpy((char *)pStorageBuff, "LD");
    }
    else if (nStorageId == CFW_PBK_ME)

    {
        strcpy((char *)pStorageBuff, "ME");
    }
    else

    {
        OSI_LOGI(0x100047a6, "AT_PBK_GetPBKCurrentStorage: storage invalid");
        return false;
    }

    return true;
}

bool AT_PBK_ProcessInputName(uint8_t *arrDest, uint8_t nDestLen, uint8_t *arrSource, uint8_t *pLen)
{

    if ((arrDest == NULL) || (arrSource == NULL))
    {
        OSI_LOGI(0x1000479b, "AT_PBK_ProcessInputName: input pointer is NULL");
        return false;
    }

    if (gAtSetting.cscs == cs_ucs2)
    {
        arrDest[0] = 0x80;

        if (*pLen > (nDestLen - 1))
        {
            OSI_LOGI(0x1000479c, "AT_PBK_ProcessInputName: source size > dest buffer size 1");
            return false;
        }

        strncpy((char *)&arrDest[1], (char *)arrSource, *pLen);

        *pLen += 1;
    }
    else
    {
        if (*pLen > nDestLen)
        {
            OSI_LOGI(0x1000479d, "AT_PBK_ProcessInputName: source size > dest buffer size 2");
            return false;
        }
        OSI_LOGI(0, "cpbw_len  = %d", *pLen);
        strncpy((char *)arrDest, (char *)arrSource, *pLen);
        OSI_LOGXI(OSI_LOGPAR_S, 0, "cpbw_len arrDest= %s", (char *)arrDest);
        OSI_LOGXI(OSI_LOGPAR_S, 0, "cpbw_len arrSource= %s", (char *)arrSource);
    }

    return true;
}

void AT_PBK_GetMEStorgeInfo(atCommand_t *cmd, uint8_t nSim)
{
    char nPromptBuff[64] = {0};
    if (strcmp("+CPBW", cmd->desc->name) == 0)

    {
        char nPromptBuff[64] = {0};
        sprintf(nPromptBuff, "+CPBW: (1-%u),%u,(129,145,161),%u",
                MAX_ME_PHONE_ENTRIES, ME_PBK_NUMBER_SIZE * 2, ME_PBK_NAME_SIZE);
        atCmdRespInfoText(cmd->engine, nPromptBuff);
        RETURN_OK(cmd->engine);
    }
    else if (strcmp("+CPBF", cmd->desc->name) == 0)

    {
        sprintf(nPromptBuff, "+CPBF: %u,%u", ME_PBK_NUMBER_SIZE * 2, ME_PBK_NAME_SIZE);
        atCmdRespInfoText(cmd->engine, nPromptBuff);
        RETURN_OK(cmd->engine);
    }

    else if (strcmp("+SPBR", cmd->desc->name) == 0)

    {
        sprintf(nPromptBuff, "+SPBR: (1-%u),%u,%u",
                MAX_ME_PHONE_ENTRIES, ME_PBK_NUMBER_SIZE * 2, ME_PBK_NAME_SIZE);
        atCmdRespInfoText(cmd->engine, nPromptBuff);
        RETURN_OK(cmd->engine);
    }
    else if (strcmp("+SPBW", cmd->desc->name) == 0)

    {
        sprintf(nPromptBuff, "+SPBW: (1-%u),%u,(129,145,161),%u",
                MAX_ME_PHONE_ENTRIES, ME_PBK_NUMBER_SIZE * 2, ME_PBK_NAME_SIZE);
        atCmdRespInfoText(cmd->engine, nPromptBuff);
        RETURN_OK(cmd->engine);
    }
    return;
}

void AT_PBK_ReadMEEntry(uint16_t nStartIndex, uint16_t nEndIndex, atCommand_t *cmd, uint8_t nSim)
{
    if ((nStartIndex > nEndIndex) || (nEndIndex > MAX_ME_PHONE_ENTRIES) || (nStartIndex < 1))

    {
        OSI_LOGI(0, "cpbr: (ME) wrong index value");
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
    }

    uint32_t iRet;
    NV_ME_PHB_INFO sEntryForME = {0};
    uint8_t pMeBuff[NV_ME_BUFF_LEN] = {0};
    uint8_t pPhoneNumber[SIM_PBK_NUMBER_SIZE * 2 + 4] = {0};
    char nPromptBuff[64] = {0};
    char InternationBuff[2] = {0};

    for (int i = nStartIndex; i <= nEndIndex; i++)

    {
        memset(InternationBuff, 0, 2);
        iRet = NV_READ_ME_PBK(i, pMeBuff);
        if (iRet == ERR_SUCCESS)

        {
            Buf2PBKI(pMeBuff, &sEntryForME);
            if (sEntryForME.index != (NV_ME_DEFAULT_VALUE << 8 | NV_ME_DEFAULT_VALUE))

            {
                SUL_GsmBcdToAscii(sEntryForME.tel, sEntryForME.tel_len, pPhoneNumber);
                if (CFW_TELNUMBER_TYPE_INTERNATIONAL == sEntryForME.type)
                    strcpy(InternationBuff, "+");
                sprintf(nPromptBuff, "+CPBR: %u,\"%s%s\",%u,\"%s\"", i,
                        InternationBuff, pPhoneNumber, sEntryForME.type, sEntryForME.name);
                atCmdRespInfoText(cmd->engine, nPromptBuff);
                RETURN_OK(cmd->engine);
            }
            else
                continue;
        }
        else

        {
            OSI_LOGI(0, "cpbw: NVRAM read failed!");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
    }

    RETURN_OK(cmd->engine);
}

void AT_PBK_WriteMEEntry(uint16_t nIndex, uint8_t nNumType, uint8_t *pBCDNumBuf, uint8_t nBCDNumLen,
                         uint8_t *pNameBuf, uint16_t nNameLen, atCommand_t *cmd, uint8_t nSim)
{
    if ((nIndex > MAX_ME_PHONE_ENTRIES) || (nIndex < 1))

    {
        OSI_LOGI(0, "cpbw: (ME) wrong index value");
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
    }

    uint32_t iRet;
    NV_ME_PHB_INFO sEntryForME = {0};
    uint8_t pMeBuff[NV_ME_BUFF_LEN] = {0};
    sEntryForME.index = nIndex;
    sEntryForME.type = nNumType;
    memcpy(sEntryForME.tel, pBCDNumBuf, nBCDNumLen);
    sEntryForME.tel_len = nBCDNumLen;
    memcpy(sEntryForME.name, pNameBuf, nNameLen);
    sEntryForME.name_len = nNameLen;
    PBKI2Buf(&sEntryForME, pMeBuff);

    iRet = NV_WRITE_ME_PBK(nIndex, pMeBuff, NV_ME_WRITE_ADD);
    if (iRet == ERR_SUCCESS)
        RETURN_OK(cmd->engine);
    else
    {
        OSI_LOGI(0, "cpbw: NVRAM write failed!");
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
    }
}

void AT_PBK_DeleteMEEntry(uint16_t nIndex, atCommand_t *cmd, uint8_t nSim)
{
    if ((nIndex > MAX_ME_PHONE_ENTRIES) || (nIndex < 1))

    {
        OSI_LOGI(0, "cpbw: (ME) wrong index value");
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
    }

    int32_t iRet;
    uint8_t pMeBuff[NV_ME_BUFF_LEN] = {0};
    memset(pMeBuff, NV_ME_DEFAULT_VALUE, NV_ME_BUFF_LEN);

    iRet = NV_WRITE_ME_PBK(nIndex, pMeBuff, NV_ME_WRITE_DEL);
    if (iRet == ERR_SUCCESS)
        RETURN_OK(cmd->engine);
    else
    {
        OSI_LOGI(0, "cpbw: NVRAM write failed!");
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
    }
}

void AT_PBK_FindMEEntry(atCommand_t *cmd, uint8_t nSim)
{
    uint8_t pTempbuff[SIM_PBK_NAME_SIZE + 1] = {0}; /* include one char: '\0' */
    uint8_t pMeBuff[NV_ME_BUFF_LEN] = {0};
    char pLocolLangTextString[SIM_PBK_NAME_SIZE + 1] = {0}; /* include one char: '\0' */
    uint8_t pOutputTextString[SIM_PBK_NAME_SIZE + 1] = {0}; /* include one char: '\0' */
    NV_ME_PHB_INFO sEntryForME = {0};
    uint8_t pPhoneNumber[SIM_PBK_NUMBER_SIZE * 2 + 4] = {0};
    char nPromptBuff[64] = {0};
    int32_t iRet;
    bool bFindPBKEntry = false;
    char InternationBuff[2] = {0};

    for (int i = 1; i <= MAX_ME_PHONE_ENTRIES; i++)

    {
        memset(pTempbuff, 0, SIM_PBK_NAME_SIZE + 1);
        memset(pLocolLangTextString, 0, SIM_PBK_NAME_SIZE + 1);
        memset(pOutputTextString, 0, SIM_PBK_NAME_SIZE + 1);
        memset(InternationBuff, 0, 2);
        iRet = NV_READ_ME_PBK(i, pMeBuff);
        if (iRet == ERR_SUCCESS)

        {
            Buf2PBKI(pMeBuff, &sEntryForME);
            if (sEntryForME.index != (NV_ME_DEFAULT_VALUE << 8 | NV_ME_DEFAULT_VALUE))

            {
                AT_PBK_ProcessOutputName(pOutputTextString, sizeof(pOutputTextString),
                                         sEntryForME.name, &(sEntryForME.name_len));
                memcpy(pLocolLangTextString, pOutputTextString, sEntryForME.name_len);
                memcpy(pTempbuff, pLocolLangTextString, strlen(pLocolLangTextString));
                if (strcmp(pLocolLangTextString, g_arrFindText) == 0 || strcmp(strupr(pLocolLangTextString), strupr(g_arrFindText)) == 0)

                {
                    bFindPBKEntry = true;
                }
                else if ((strstr(pLocolLangTextString, g_arrFindText) == pLocolLangTextString) || (strstr(strupr(pLocolLangTextString), strupr(g_arrFindText)) == pLocolLangTextString))

                {
                    bFindPBKEntry = true;
                }
                if (CFW_TELNUMBER_TYPE_INTERNATIONAL == sEntryForME.type)
                {
                    strcpy(InternationBuff, "+");
                }
                SUL_GsmBcdToAscii(sEntryForME.tel, sEntryForME.tel_len, pPhoneNumber);
                if (bFindPBKEntry)

                {
                    sprintf(nPromptBuff, "+CPBF: %u,\"%s%s\",%u,\"%s\"", sEntryForME.index,
                            InternationBuff, pPhoneNumber, sEntryForME.type, pTempbuff);
                    //atCmdRespOK(cmd->engine);
                    atCmdRespDefUrcText("OK");
                    bFindPBKEntry = false;
                }
            }
            else
            {
                continue;
            }
        }
        else
        {
            OSI_LOGI(0, "cpbf: NV read failed!");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
    }

    RETURN_OK(cmd->engine);
}

static void _onEV_CFW_SIM_GET_PBK_STRORAGE_INFO_RSP(const osiEvent_t *event)
{
    //EV_CFW_SIM_GET_PBK_STRORAGE_INFO_RSP
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    OSI_LOGI(0, "pbk_init EV_CFW_SIM_GET_PBK_STRORAGE_INFO_RSP");

    if (cfw_event->nType == 0)
    {
        OSI_LOGI(0, "pbk_init _InitSaveMaxSizeRsp: CfwEvent->nType == 0");
        CFW_PBK_STRORAGE_INFO *pPbkStorageInfo = (CFW_PBK_STRORAGE_INFO *)cfw_event->nParam1;
        g_nMaxNumberSize = (pPbkStorageInfo->iNumberLen) * 2;
        g_nMaxTextSize = pPbkStorageInfo->txtLen;
        g_nMaxSMPBKIndex = pPbkStorageInfo->index;
        OSI_LOGI(0, "g_nMaxNumberSize = %d,g_nMaxTextSize = %d,g_nMaxSMPBKIndex = %d", g_nMaxNumberSize, g_nMaxTextSize, g_nMaxSMPBKIndex);
    }
    else
        OSI_LOGI(0, "pbk_init _InitSaveMaxSizeRsp: wrong CfwEvent->nType");

    return;
}

void _GetFDNRecordRSP(const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    CFW_SIM_PBK_ENTRY_INFO *pRecord = NULL;
    uint8_t nSim = cfw_event->nFlag;

    if (pgATFNDlist[nSim] == NULL || pgATFNDlist[nSim]->nTotalRecordNum < 1)
    {
        OSI_LOGI(0x100047ba, "GetFDNRecord WARNING:FDN List is NULL !\n");
        return;
    }

    OSI_LOGI(0x100047bb, "GetFDNRecord start: nType:0x%x,nSim:%d!\n", cfw_event->nType, nSim);

    if (cfw_event->nType == 0)
    {
        if (pgATFNDlist[nSim]->nRealRecordNum >= pgATFNDlist[nSim]->nTotalRecordNum)
        {
            OSI_LOGI(0x100047bc, "GetFDNRecord ERROR:Get FDN Record is err !\n");
            return;
        }

        pRecord = (CFW_SIM_PBK_ENTRY_INFO *)cfw_event->nParam1;
        if (pRecord != NULL)
        {
            memcpy(pgATFNDlist[nSim]->sRecord[pgATFNDlist[nSim]->nRealRecordNum].nName, pRecord->pFullName, pRecord->iFullNameSize);
            memcpy(pgATFNDlist[nSim]->sRecord[pgATFNDlist[nSim]->nRealRecordNum].nNumber, pRecord->pNumber, pRecord->nNumberSize);
            pgATFNDlist[nSim]->sRecord[pgATFNDlist[nSim]->nRealRecordNum].nIndex = (uint8_t)(cfw_event->nParam2 & 0x000000ff);
            pgATFNDlist[nSim]->sRecord[pgATFNDlist[nSim]->nRealRecordNum].nTpye = pRecord->nType;
            pgATFNDlist[nSim]->nRealRecordNum++;
        }
        else
        {
            OSI_LOGI(0x100047bd, "GetFDNRecord WARNING:FDN Record is NULL !\n");
        }
        pgATFNDlist[nSim]->nCurrentRecordIndx++;

        //get next FDN record
        if (pgATFNDlist[nSim]->nCurrentRecordIndx < pgATFNDlist[nSim]->nTotalRecordNum)
        {
            if (ERR_SUCCESS != CFW_SimGetPbkEntry(CFW_PBK_SIM_FIX_DIALLING, pgATFNDlist[nSim]->nCurrentRecordIndx + 1, 0, nSim))
            {
                OSI_LOGI(0x100047be, "GetFDNRecord WARNING:Get FDN Record is fail !\n");
            }
        }
        else
        {
            OSI_LOGI(0x100047bf, "GetFDNRecord :Get FDN Record is end realRecord:%d!\n", pgATFNDlist[nSim]->nRealRecordNum);
        }
    }
    else if (cfw_event->nType == 0xF0)
    {
        OSI_LOGI(0x100047c0, "GetFDNRecord ERROR:Get FDN Record is fail !\n");
    }
    else
    {
        OSI_LOGI(0x100047c1, "GetFDNRecord ERROR:Get FDN para is fail !\n");
    }

    return;
}

void AT_PBK_Init(uint8_t nSim)
{
    OSI_LOGI(0x1000477d, "PBK init beginning .......");
    uint8_t nStorage = 0;
    atEventRegister(EV_CFW_SIM_GET_PBK_STRORAGE_INFO_RSP, _onEV_CFW_SIM_GET_PBK_STRORAGE_INFO_RSP);
    atEventRegister(EV_CFW_SIM_GET_PBK_ENTRY_RSP, _GetFDNRecordRSP);

    if (0 != CFW_CfgSetPbkStorage(CFW_PBK_SIM, nSim))
        return;

    /* Get the Max number size and name size and set request flag */
    OSI_LOGI(0x1000477f, "This request is getting max size of PBK number and PBK text!");
    //g_nInitStorageInfoReqFlag = 1;
    if (0 != CFW_SimGetPbkStrorageInfo(nStorage, 0, nSim))
        return;

    //AT_FDN_Init(nSim);
    //RPC_CFW_NV_PBK_Init();

    // Just for debug
    OSI_LOGI(0x10004780, "PBK init OK!");
}

static void _cpbwRspCB(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    char PromptBuff[64] = {0};

    if (cfw_event->nType == 0xf0)
    {
        OSI_LOGI(0, "_cpbsRspCB :CfwEvent.nType == 0xF0");
        RETURN_CME_CFW_ERR(cmd->engine, cfw_event->nParam1);
    }
    else if (cfw_event->nType == 0)
    {
        if (cfw_event->nEventId == EV_CFW_SIM_GET_PBK_STRORAGE_INFO_RSP)
        {
            CFW_PBK_STRORAGE_INFO *pPbkStorageInfo = (CFW_PBK_STRORAGE_INFO *)cfw_event->nParam1;
            sprintf(PromptBuff, "+CPBW: (1-%u),%u,(129,145,161),%u",
                    pPbkStorageInfo->index, (pPbkStorageInfo->iNumberLen) * 2, pPbkStorageInfo->txtLen);
            atCmdRespInfoText(cmd->engine, PromptBuff);
            RETURN_OK(cmd->engine);
        }
        if (cfw_event->nEventId == EV_CFW_SIM_ADD_PBK_RSP)
        {
            RETURN_OK(cmd->engine);
        }
        if (cfw_event->nEventId == EV_CFW_SIM_DELETE_PBK_ENTRY_RSP)
        {
            RETURN_OK(cmd->engine);
        }
    }
}

void atCmdHandleCPBW(atCommand_t *cmd)
{
#if 0
    CFW_SIM_PBK_ENTRY_INFO_V2 sEntryToBeAdded = {0};

    uint8_t pPhoneNumber[SIM_PBK_NUMBER_SIZE * 2 + 4] = {0}; /* include one '\0' and two ' " ' and one + */
    uint16_t nPhoneNumSize = 0;
    bool bInternationFlag = false;
    int32_t nOperationRet = ERR_SUCCESS;
    uint8_t nStorage = 0;
    uint8_t InputAsciiPhoneFullName[4 * SIM_PBK_NAME_SIZE + 3] = {0}; /* include one '\0' and two ' " ' */
    uint8_t InputBytesPhoneFullName[SIM_PBK_NAME_SIZE + 3] = {0};     /* include one '\0' and two ' " ' */
    uint8_t nSim = atCmdGetSim(cmd->engine);

    if (cmd->type == AT_CMD_TEST)
    {
        if (gAtSetting.tempStorage == CFW_PBK_ME)
        {
            char nPromptBuff[64] = {0};
            sprintf(nPromptBuff, "+CPBW: (1-%u),%u,(129,145,161),%u",
                    MAX_ME_PHONE_ENTRIES, ME_PBK_NUMBER_SIZE * 2, ME_PBK_NAME_SIZE);
            atCmdRespInfoText(cmd->engine, nPromptBuff);
            RETURN_OK(cmd->engine);
        }
        else
        {
            uint32_t ret = ERR_SUCCESS;

            cmd->uti = cfwRequestUTI((osiEventCallback_t)_cpbwRspCB, cmd);
            ret = CFW_SimGetPbkStrorageInfo(gAtSetting.tempStorage, cmd->uti, nSim);
            if (ERR_SUCCESS != ret)
            {
                cfwReleaseUTI(cmd->uti);
                RETURN_CME_CFW_ERR(cmd->engine, ret);
            }
            RETURN_FOR_ASYNC();
        }
    }
    else if (cmd->type == AT_CMD_SET)
    {
        // get cuurent storage
        nStorage = gAtSetting.tempStorage;
        bool paramok = true;

        if (!((nStorage == CFW_PBK_ON) || (nStorage == CFW_PBK_SIM_FIX_DIALLING) || (nStorage == CFW_PBK_SIM_LAST_DIALLING) || (nStorage == CFW_PBK_SIM) || (nStorage == CFW_PBK_ME)))
        {
            OSI_LOGI(0, "cpbw: input wrong storage");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        sEntryToBeAdded.phoneIndex = atParamDefInt(cmd->params[0], 0, &paramok);
        const char *InputText = atParamOptStr(cmd->params[1], &paramok);
        strncpy((char *)pPhoneNumber, InputText, SIM_PBK_NUMBER_SIZE * 2 + 4);
        OSI_LOGXI(OSI_LOGPAR_S, 0, "cpbw-InputText= %s", (char *)InputText);
        OSI_LOGXI(OSI_LOGPAR_S, 0, "cpbw-pPhoneNumber= %s", (char *)pPhoneNumber);
        nPhoneNumSize = strlen((char *)pPhoneNumber);
        sEntryToBeAdded.nType = atParamDefInt(cmd->params[2], CFW_TELNUMBER_TYPE_UNKNOWN, &paramok);
        if (!paramok || cmd->param_count > 4)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        if (cmd->param_count > 1)
        {
            nPhoneNumSize = strlen((char *)pPhoneNumber);
            if (!(AT_PBK_IsValidPhoneNumber(pPhoneNumber, nPhoneNumSize, &bInternationFlag)))
            {
                OSI_LOGI(0, "cpbw:param 2 invalid phone number");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
            if (bInternationFlag)
            {
                if (nPhoneNumSize > (g_nMaxNumberSize + 1))
                {
                    OSI_LOGI(0, "cpbw: param 2 - 1 > max size");
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
                sEntryToBeAdded.nNumberSize = SUL_AsciiToGsmBcd(&(pPhoneNumber[1]), nPhoneNumSize - 1, sEntryToBeAdded.pNumber);
            }
            else
            {
                if (nPhoneNumSize > g_nMaxNumberSize)
                {
                    OSI_LOGI(0, "cpbw: param 2 - 2 > max size");
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
                sEntryToBeAdded.nNumberSize = SUL_AsciiToGsmBcd(pPhoneNumber, nPhoneNumSize, sEntryToBeAdded.pNumber);
            }
        }
        if (cmd->param_count > 2)
        {
            if ((sEntryToBeAdded.nType != CFW_TELNUMBER_TYPE_INTERNATIONAL) && (sEntryToBeAdded.nType != CFW_TELNUMBER_TYPE_NATIONAL) && (sEntryToBeAdded.nType != CFW_TELNUMBER_TYPE_UNKNOWN))
            {
                OSI_LOGI(0, "cpbw:param 3 dial number type wrong");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
        }
        if (cmd->param_count > 3)
        {

            if (gAtSetting.cscs == cs_ucs2)
            {
                const char *Inputtext = atParamOptStr(cmd->params[3], &paramok);
                strncpy((char *)InputAsciiPhoneFullName, Inputtext, SIM_PBK_NAME_SIZE + 3);
                sEntryToBeAdded.iFullNameSize = strlen((char *)InputAsciiPhoneFullName);
                OSI_LOGI(0, "cpbw cs_ucs2 nPhoneFullNameSize = %d", sEntryToBeAdded.iFullNameSize);
                //ucs string char conter must be multiple of  4
                if (sEntryToBeAdded.iFullNameSize & 3)
                {
                    OSI_LOGI(0, "cpbw: input ucs ascii is not multiple of 4");
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }

                if (!(AT_PBK_String2Bytes(InputBytesPhoneFullName, InputAsciiPhoneFullName, (uint8_t *)&sEntryToBeAdded.iFullNameSize)))
                {
                    OSI_LOGI(0, "cpbw:param 4 AT_PBK_String2Bytes error");
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
                OSI_LOGI(0x10004797, "After StringToBytes: bytes len = %u, and input ascii data is: == > ", sEntryToBeAdded.iFullNameSize);
                sEntryToBeAdded.pFullName[0] = 0x80;
                if (sEntryToBeAdded.iFullNameSize > (SIM_PBK_NAME_SIZE + 3))
                {
                    OSI_LOGI(0x1000479c, "AT_PBK_ProcessInputName: source size > dest buffer size 1");
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
                if (sEntryToBeAdded.iFullNameSize >= SIM_PBK_NAME_SIZE)
                    sEntryToBeAdded.iFullNameSize = SIM_PBK_NAME_SIZE - 1;
                strncpy((char *)&sEntryToBeAdded.pFullName[1], (char *)InputBytesPhoneFullName, sEntryToBeAdded.iFullNameSize);
                sEntryToBeAdded.iFullNameSize += 1;
            }
            else
            {
                const char *Inputtext = atParamOptStr(cmd->params[3], &paramok);
                OSI_LOGXI(OSI_LOGPAR_S, 0, "cpbw-InputText= %s", Inputtext);
                strncpy((char *)sEntryToBeAdded.pFullName, Inputtext, strlen(Inputtext));
                OSI_LOGXI(OSI_LOGPAR_S, 0, "cpbw-InputAsciiPhoneFullName= %s", (char *)sEntryToBeAdded.pFullName);
                sEntryToBeAdded.iFullNameSize = strlen((char *)sEntryToBeAdded.pFullName);
                OSI_LOGI(0, "cpbw nPhoneFullNameSize = %d", sEntryToBeAdded.iFullNameSize);
            }

            OSI_LOGI(0x10004798, "cpbw:g_nMaxTextSize  = %u nPhoneFullNameSize = %u ", g_nMaxTextSize, sEntryToBeAdded.iFullNameSize);
            OSI_LOGXI(OSI_LOGPAR_S, 0, "cpbw-pPhoneFullName= %s", (char *)sEntryToBeAdded.pFullName);

            if (sEntryToBeAdded.iFullNameSize == 0)
            {

                memset(sEntryToBeAdded.pFullName, 0, sizeof(sEntryToBeAdded.pFullName));
                sEntryToBeAdded.iFullNameSize = 0;
            }
            else
            {
                if (!paramok)
                {
                    OSI_LOGI(0, "cpbw:param 4 error");
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }

                if (sEntryToBeAdded.iFullNameSize > g_nMaxTextSize)
                {
                    OSI_LOGI(0, "cpbw:input text too long");
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                }
            }
        }
        // Process only have nWriteEntryIndex
        if ((0 != sEntryToBeAdded.phoneIndex) && (1 == cmd->param_count))
        {
            if (CFW_PBK_ME == nStorage)
            {
                AT_PBK_DeleteMEEntry(sEntryToBeAdded.phoneIndex, cmd, nSim);
                return;
            }
            // if current pbk is "ON", delete operation is not allowed.
            /* get the UTI and free it after finished calling */
            cmd->uti = cfwRequestUTI((osiEventCallback_t)_cpbwRspCB, cmd);
            nOperationRet = CFW_SimDeletePbkEntry(nStorage, sEntryToBeAdded.phoneIndex, cmd->uti, nSim);
            if (nOperationRet != ERR_SUCCESS)
            {
                OSI_LOGI(0, "cpbf exe err");
                cfwReleaseUTI(cmd->uti);
                RETURN_CME_CFW_ERR(cmd->engine, nOperationRet);
            }
            RETURN_FOR_ASYNC();
        }

        // the number must not be NULL when the counters of param is more than one, if only have one param: index,
        //it not be zero, otherwise, error happened.bug 8315 related */
        else if ((0 == sEntryToBeAdded.phoneIndex) && (1 == cmd->param_count))
        {
            OSI_LOGI(0, "cpbw:only one param but index is zero");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }
        //according to spec, we check dial number first ,Judge the num type, if is 0, set default value
        if (bInternationFlag)
        {
            sEntryToBeAdded.nType = CFW_TELNUMBER_TYPE_INTERNATIONAL;
        }
        else
        {
            if (0 == sEntryToBeAdded.nType)
            {
                //no input num type
                sEntryToBeAdded.nType = CFW_TELNUMBER_TYPE_UNKNOWN;
            }
            else if (CFW_TELNUMBER_TYPE_INTERNATIONAL == sEntryToBeAdded.nType)
            {
                /* have no '+' , but the num type is 145, replace 129 with is  */
                sEntryToBeAdded.nType = CFW_TELNUMBER_TYPE_UNKNOWN;
            }
        }

        /* Set value to Entry written */
        CFW_SIM_PBK_ENTRY_INFO *sEntryToBeAddedv1 = (CFW_SIM_PBK_ENTRY_INFO *)&sEntryToBeAdded;

        OSI_LOGI(0, "cpbw sEntryToBeAdded.nNumberSize = %d", sEntryToBeAdded.nNumberSize);
        OSI_LOGI(0, "cpbw sEntryToBeAdded.iFullNameSize = %d", sEntryToBeAdded.iFullNameSize);
        OSI_LOGXI(OSI_LOGPAR_S, 0, "cpbw-sEntryToBeAdded.pNumber= %s", (char *)sEntryToBeAdded.pNumber);
        OSI_LOGXI(OSI_LOGPAR_S, 0, "cpbw-sEntryToBeAdded.pFullName= %s", (char *)sEntryToBeAdded.pFullName);

        // if current pbk is "LD", write operation is not allowed.
        if (nStorage == CFW_PBK_SIM_LAST_DIALLING)
        {
            OSI_LOGI(0, "CPBW error: LD phonebook can't be written!!!");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        // process add entry in the pbk
        if (CFW_PBK_SIM_FIX_DIALLING == nStorage)
        {
            AT_FDN_AddRecord(sEntryToBeAdded.phoneIndex, sEntryToBeAddedv1, nSim);
        }
        else if (CFW_PBK_ME == nStorage)
        {
            AT_PBK_WriteMEEntry(sEntryToBeAdded.phoneIndex, sEntryToBeAdded.nType, sEntryToBeAdded.pNumber, sEntryToBeAdded.nNumberSize,
                                sEntryToBeAdded.pFullName, sEntryToBeAdded.iFullNameSize, cmd, nSim);
            return;
        }
        cmd->uti = cfwRequestUTI((osiEventCallback_t)_cpbwRspCB, cmd);
        nOperationRet = CFW_SimAddPbkEntry_V2(nStorage, &sEntryToBeAdded, cmd->uti, nSim);
        if (nOperationRet != ERR_SUCCESS)
        {
            OSI_LOGI(0, "cpbf exe err");
            cfwReleaseUTI(cmd->uti);
            RETURN_CME_CFW_ERR(cmd->engine, nOperationRet);
        }
        RETURN_FOR_ASYNC();
    }
	else
	{
		quec_atCmdResp(cmd->engine, ATCI_RESULT_CODE_CME_ERROR, ERR_AT_CME_EXE_NOT_SURPORT);
	}

    return;
#endif
}

typedef struct
{
    uint16_t g_nListEntryMaxIndex;    /* Begin list entry index */
    uint16_t g_nListEntryMinIndex;    /* End list entry num */
    uint16_t g_nListEntryLastStep;    /* step length */
    uint16_t g_nListEntryStepCounter; /* list step counter */
} CPBRsyncCtx_t;

static void _cpbrRspCB(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    uint8_t nSim = cfw_event->nFlag;
    char PromptBuff[64] = {0};
    uint32_t nErrorCode = ERR_SUCCESS;

    if (cfw_event->nType == 0xf0)
    {
        OSI_LOGI(0, "_cpbsRspCB :CfwEvent.nType == 0xF0");
        RETURN_CME_CFW_ERR(cmd->engine, cfw_event->nParam1);
    }
    if (cfw_event->nEventId == EV_CFW_SIM_GET_PBK_STRORAGE_INFO_RSP)
    {
        CFW_PBK_STRORAGE_INFO *pPbkStorageInfo = (CFW_PBK_STRORAGE_INFO *)cfw_event->nParam1;
        sprintf(PromptBuff, "+CPBR: (1-%u),%u,%u",
                pPbkStorageInfo->index, (pPbkStorageInfo->iNumberLen) * 2, pPbkStorageInfo->txtLen);
        atCmdRespInfoText(cmd->engine, PromptBuff);
        RETURN_OK(cmd->engine);
    }
    if (cfw_event->nEventId == EV_CFW_SIM_LIST_PBK_ENTRY_RSP)
    {
        uint8_t nEntryCount = 0;
        char MarkCmdStampBuff[8] = {0};
        char PromptBuff[128] = {0};
        CPBRsyncCtx_t *async = (CPBRsyncCtx_t *)cmd->async_ctx;
        nEntryCount = cfw_event->nParam2;
        OSI_LOGI(0, "cpbr nEntryCount = %d", nEntryCount);

        CFW_SIM_PBK_ENTRY_INFO *pPbkEntryInfo = (CFW_SIM_PBK_ENTRY_INFO *)cfw_event->nParam1;
        OSI_LOGI(0, "cpbr nNumberSize = %d,iFullNameSize=%d", pPbkEntryInfo->nNumberSize, pPbkEntryInfo->iFullNameSize);
        strcpy(MarkCmdStampBuff, "+CPBR");
        if (nEntryCount != 0)
        {
            atMemFreeLater(pPbkEntryInfo);
        }

        //have Entry found
        while (nEntryCount-- > 0)
        {
            char InternationBuff[2] = {0};
            OSI_LOGI(0, "cpbr:nEntryCount = %d", nEntryCount);

            uint8_t LocolLangNumber[SIM_PBK_NUMBER_SIZE * 2 + 1] = {0}; /* include one char: '\0' */

            uint8_t pLocolLangTextString[SIM_PBK_NAME_SIZE + 1] = {0}; /* include one char: '\0' */
            uint8_t pOutputTextString[SIM_PBK_NAME_SIZE + 1] = {0};    /* include one char: '\0' */
            pPbkEntryInfo->pNumber = (uint8_t *)pPbkEntryInfo + (uint32_t)sizeof(CFW_SIM_PBK_ENTRY_INFO);
            pPbkEntryInfo->pFullName = (uint8_t *)pPbkEntryInfo + (uint32_t)(sizeof(CFW_SIM_PBK_ENTRY_INFO) + SIM_PBK_NUMBER_SIZE);

            /* clear buffer of prompt  */
            memset(PromptBuff, 0, sizeof(PromptBuff));

            /* check the input number sizeless than Max number size or not
               ** if more than the Max size, then discard part of them*/

            if (2 * (pPbkEntryInfo->nNumberSize) > g_nMaxNumberSize)
            {
                pPbkEntryInfo->nNumberSize = g_nMaxNumberSize / 2;
            }

            /* check the input text length less than Max text size or not
               ** if more than the Max size, then discard part of them*/

            if (pPbkEntryInfo->iFullNameSize > g_nMaxTextSize)
            {
                pPbkEntryInfo->iFullNameSize = g_nMaxTextSize;
            }

            // BCD to asscii
            SUL_GsmBcdToAscii(pPbkEntryInfo->pNumber, pPbkEntryInfo->nNumberSize, LocolLangNumber);

            /* process text string from csw  */
            if (!(AT_PBK_ProcessOutputName(pOutputTextString, sizeof(pOutputTextString), pPbkEntryInfo->pFullName, &(pPbkEntryInfo->iFullNameSize))))
            {
                OSI_LOGI(0, "ListPbkEntry resp: some message node exception");

                /* get list next node, then continue */
                goto label_next_list_node;
            }

            /* buffer overflow */
            if (pPbkEntryInfo->iFullNameSize > sizeof(pLocolLangTextString))
            {
                OSI_LOGI(0, "ListPbkEntry resp:name size of message node more than local buffer len");

                /* get list next node, then continue */
                goto label_next_list_node;
            }
            OSI_LOGXI(OSI_LOGPAR_S, 0, "cpbr pPbkEntryInfo->pFullName  = %s", (char *)pPbkEntryInfo->pFullName);
            OSI_LOGXI(OSI_LOGPAR_S, 0, "cpbr pPbkEntryInfo->pNumber  = %s", (char *)pPbkEntryInfo->pNumber);
            /* save the string to local language string buffer */
            memcpy(pLocolLangTextString, pOutputTextString, pPbkEntryInfo->iFullNameSize);

            /* international or not */
            if (CFW_TELNUMBER_TYPE_INTERNATIONAL == pPbkEntryInfo->nType)
            {
                strcpy(InternationBuff, "+");
            }
            /* Get the AT command type   */
            sprintf(PromptBuff, "%s: %u,\"%s%s\",%u,\"%s\"", MarkCmdStampBuff, pPbkEntryInfo->phoneIndex,
                    InternationBuff, LocolLangNumber, pPbkEntryInfo->nType, pLocolLangTextString);
            atCmdRespInfoText(cmd->engine, PromptBuff);
            /* Get the next list node*/
        label_next_list_node:
            pPbkEntryInfo =
                (CFW_SIM_PBK_ENTRY_INFO *)((uint32_t)pPbkEntryInfo + sizeof(CFW_SIM_PBK_ENTRY_INFO) + SIM_PBK_EXTR_SIZE);
        }

        /*check multi list entry finished or not*/
        if (1 == async->g_nListEntryLastStep)
        {
            RETURN_OK(cmd->engine);
        }
        else
        {
            /* Length is reach the end point and finished this time */
            if ((async->g_nListEntryMaxIndex - async->g_nListEntryMinIndex + 1) <= AT_PBK_LIST_ENTRY_STEP * (async->g_nListEntryStepCounter + 1))
            {
                async->g_nListEntryLastStep = 1;
                cmd->uti = cfwRequestUTI((osiEventCallback_t)_cpbrRspCB, cmd);
                nErrorCode = CFW_SimListPbkEntries(gAtSetting.tempStorage,
                                                   async->g_nListEntryMinIndex + AT_PBK_LIST_ENTRY_STEP * async->g_nListEntryStepCounter,
                                                   async->g_nListEntryMaxIndex,
                                                   cmd->uti, nSim);
            }

            /* have some items to be continue to list  */
            else
            {
                cmd->uti = cfwRequestUTI((osiEventCallback_t)_cpbrRspCB, cmd);
                nErrorCode = CFW_SimListPbkEntries(gAtSetting.tempStorage,
                                                   async->g_nListEntryMinIndex + AT_PBK_LIST_ENTRY_STEP * async->g_nListEntryStepCounter,
                                                   async->g_nListEntryMinIndex + AT_PBK_LIST_ENTRY_STEP * (async->g_nListEntryStepCounter + 1) - 1,
                                                   cmd->uti, nSim);
                async->g_nListEntryStepCounter++;
            }
            if (ERR_SUCCESS != nErrorCode)
            {
                cfwReleaseUTI(cmd->uti);
                nErrorCode = AT_SetCmeErrorCode(nErrorCode);
                RETURN_CME_ERR(cmd->engine, nErrorCode);
            }
        }
    }
    return;
}

void atCmdHandleCPBR(atCommand_t *cmd)
{
#if 0
    uint8_t nSim = atCmdGetSim(cmd->engine);
    uint16_t nIndexStartListEntry = 0;
    uint16_t nIndexEndListEntry = 0;
    uint32_t ret = ERR_SUCCESS;

    if (cmd->type == AT_CMD_TEST)
    {
        if (gAtSetting.tempStorage == CFW_PBK_ME)
        {
            char nPromptBuff[64] = {0};
            sprintf(nPromptBuff, "+CPBR: (1-%u),%u,%u",
                    MAX_ME_PHONE_ENTRIES, ME_PBK_NUMBER_SIZE * 2, ME_PBK_NAME_SIZE);
            atCmdRespInfoText(cmd->engine, nPromptBuff);
            RETURN_OK(cmd->engine);
        }
        else
        {
            uint32_t ret = ERR_SUCCESS;

            cmd->uti = cfwRequestUTI((osiEventCallback_t)_cpbrRspCB, cmd);
            ret = CFW_SimGetPbkStrorageInfo(gAtSetting.tempStorage, cmd->uti, nSim);
            if (ERR_SUCCESS != ret)
            {
                cfwReleaseUTI(cmd->uti);
                RETURN_CME_CFW_ERR(cmd->engine, ret);
            }
            RETURN_FOR_ASYNC();
        }
    }
    else if (cmd->type == AT_CMD_SET) //+CPBR=<index1>[,<index2>]
    {
        bool paramok = true;
        nIndexStartListEntry = atParamDefUint(cmd->params[0], 0, &paramok);
        nIndexEndListEntry = atParamDefUint(cmd->params[1], 0, &paramok);
        OSI_LOGI(0, "cpbr:paramok = %d, cmd->param_count = %d,  nIndexStartListEntry  = %d", paramok, cmd->param_count, nIndexStartListEntry);
        if (!paramok || cmd->param_count > 2 || (0 == nIndexStartListEntry))
        {
            OSI_LOGI(0, "cpbr: param 1 invalid");
            if ((cmd->param_count > 1) && (0 == nIndexEndListEntry))
                OSI_LOGI(0, "cpbr: param 2 invalid");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        /* only have one para */
        if (cmd->param_count == 1)
            nIndexEndListEntry = nIndexStartListEntry;

        if (gAtSetting.tempStorage == CFW_PBK_ME)
            AT_PBK_ReadMEEntry(nIndexStartListEntry, nIndexEndListEntry, cmd, nSim);
        else
        {
            /* check startindex and endIndex, and check "SM" pbk max index */
            if ((nIndexStartListEntry > nIndexEndListEntry) || (nIndexEndListEntry > g_nMaxSMPBKIndex))
            {
                OSI_LOGI(0, "cpbr: wrong index value");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }

            /* Reset the multi list param */
            CNUMAsyncCtx_t *async = (CNUMAsyncCtx_t *)malloc(sizeof(*async));
            if (async == NULL)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);

            cmd->async_ctx = async;
            cmd->async_ctx_destroy = atCommandAsyncCtxFree;

            async->g_nListEntryLastStep = 0;
            async->g_nListEntryMaxIndex = 0;
            async->g_nListEntryMinIndex = 0;
            async->g_nListEntryStepCounter = 0;

            async->g_nListEntryMinIndex = nIndexStartListEntry;
            async->g_nListEntryMaxIndex = nIndexEndListEntry;
            async->g_nListEntryStepCounter++;

            /* multi step list if need */
            cmd->uti = cfwRequestUTI((osiEventCallback_t)_cpbrRspCB, cmd);
            if ((nIndexEndListEntry - nIndexStartListEntry + 1) <= AT_PBK_LIST_ENTRY_STEP * async->g_nListEntryStepCounter)
            {
                async->g_nListEntryLastStep = 1;
                ret = CFW_SimListPbkEntries(gAtSetting.tempStorage, nIndexStartListEntry, nIndexEndListEntry, cmd->uti, nSim);
            }
            else
            {
                ret = CFW_SimListPbkEntries(gAtSetting.tempStorage, nIndexStartListEntry, nIndexStartListEntry + AT_PBK_LIST_ENTRY_STEP - 1, cmd->uti, nSim);
            }
            if (ERR_SUCCESS != ret)
            {
                cfwReleaseUTI(cmd->uti);
                RETURN_CME_CFW_ERR(cmd->engine, ret);
            }
        }
    }
    else
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
#endif
}

static void _cpbsRspCB(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    char PromptBuff[64] = {0};
    uint8_t StorageInfoBuff[8] = {0};
    char pStorageBuff[8] = {0};

    if (cfw_event->nType == 0xf0)
    {
        OSI_LOGI(0, "_cpbsRspCB :CfwEvent.nType == 0xF0");
        RETURN_CME_CFW_ERR(cmd->engine, cfw_event->nParam1);
    }
    else if (cfw_event->nType == 0)
    {
        if (cfw_event->nEventId == EV_CFW_SIM_GET_PBK_STRORAGE_RSP)
        {
            CFW_PBK_STORAGE *pPbkStorage = (CFW_PBK_STORAGE *)cfw_event->nParam1;
            if (!AT_PBK_GetPBKCurrentStorage(pPbkStorage->storageId, StorageInfoBuff))
            {
                OSI_LOGI(0, "cpbs: GetPbkStorage resp: GetCurrentStorage is false");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
            }
            sprintf(PromptBuff, "+CPBS: \"%s\",%u,%u", StorageInfoBuff, pPbkStorage->usedSlot, pPbkStorage->totalSlot);
            atCmdRespInfoText(cmd->engine, PromptBuff);
            free(pPbkStorage);
            RETURN_OK(cmd->engine);
        }
        if (cfw_event->nEventId == EV_CFW_SIM_GET_PBK_STRORAGE_INFO_RSP)
        {
            CFW_PBK_STRORAGE_INFO *pPbkStorageInfo = (CFW_PBK_STRORAGE_INFO *)cfw_event->nParam1;
            uint8_t *pBitmap = pPbkStorageInfo->Bitmap;
            uint16_t nIndex = pPbkStorageInfo->index;
            nIndex = (nIndex + 7) >> 3; //bytes of bitmap
            OSI_LOGI(0, "nIndex = %d, total = %d", nIndex, pPbkStorageInfo->index);
            uint16_t nUsed = 0;
            uint16_t i = 0;

            for (i = 0; i < nIndex; i++)
            {
                nUsed += pBitmap[i] & 0x01;
                OSI_LOGI(0, "===== pBitmap[i] = 0x%x =====", pBitmap[i]);
                OSI_LOGI(0, "nUsed = %d", nUsed);
                for (uint8_t j = 1; j < 8; j++)
                {
                    pBitmap[i] = pBitmap[i] >> 1;
                    nUsed += pBitmap[i] & 0x01;
                    OSI_LOGI(0, "pBitmap[%d] = 0x%x, j = %d", i, pBitmap[i], j);
                    OSI_LOGI(0, "nUsed = %d", nUsed);
                }
            }

            if (gAtSetting.tempStorage == CFW_PBK_ON)
            {
                strcpy(pStorageBuff, "ON");
            }
            else if (gAtSetting.tempStorage == CFW_PBK_SIM_FIX_DIALLING)
            {
                strcpy(pStorageBuff, "FD");
            }
            else if (gAtSetting.tempStorage == CFW_PBK_SIM_LAST_DIALLING)
            {
                strcpy(pStorageBuff, "LD");
            }
            else
            {
                strcpy(pStorageBuff, "SM");
            }
            OSI_LOGI(0x100047af, "nUsed = %d, nTotal = %d", nUsed, pPbkStorageInfo->index);
            sprintf(PromptBuff, "+CPBS: \"%s\",%u,%u", pStorageBuff, nUsed, pPbkStorageInfo->index);
            atCmdRespInfoText(cmd->engine, PromptBuff);
            RETURN_OK(cmd->engine);
        }
    }
}

void atCmdHandleCPBS(atCommand_t *cmd)
{
#if 0
    uint8_t nSim = atCmdGetSim(cmd->engine);
    char nPromptBuff[64] = {0};

    if (cmd->type == AT_CMD_TEST) //+CPBS=?: (list of supported <storage>s)
    {
        atCmdRespInfoText(cmd->engine, "+CPBS: (\"SM\",\"ON\",\"FD\",\"LD\",\"ME\")");
        RETURN_OK(cmd->engine);
    }
    else if (cmd->type == AT_CMD_SET) //+CPBS=<storage>[,<password>]
    {
        bool paramok = true;
        const char *buff = atParamStr(cmd->params[0], &paramok);
        if (!paramok || cmd->param_count != 1)
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

        /* to capital letter of input string */
        char *InputStoFlagBuff = (char *)buff;
        strupr((char *)InputStoFlagBuff);
        if (!AT_PBK_SetPBKCurrentStorage(InputStoFlagBuff))
        {
            OSI_LOGI(0, "cpbs: wrong storage");
            RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
        }

        OSI_LOGI(0, "CPBS: gAtSetting.tempStorage = %d", gAtSetting.tempStorage);
        OSI_LOGXI(OSI_LOGPAR_S, 0, "CPBS: InputStoFlagBuff = %s", InputStoFlagBuff);
        if (gAtSetting.tempStorage == CFW_PBK_SIM && CFW_GetSimType(nSim) == 1)
        {
            uint32_t ret = ERR_SUCCESS;
            cmd->uti = cfwRequestUTI((osiEventCallback_t)_cpbsRspCB, cmd);
            ret = CFW_SimGetPbkStrorageInfo(gAtSetting.tempStorage, cmd->uti, nSim);
            if (ERR_SUCCESS != ret)
            {
                cfwReleaseUTI(cmd->uti);
                RETURN_CME_CFW_ERR(cmd->engine, ret);
            }
            RETURN_FOR_ASYNC();
        }
        else if (gAtSetting.tempStorage == CFW_PBK_ME)
        {
            int32_t usenum = NV_GET_STORGE_ME_PBK(MAX_ME_PHONE_ENTRIES);
            if (usenum < 0)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
            sprintf(nPromptBuff, "+CPBS: \"ME\", %lu, %u", usenum, MAX_ME_PHONE_ENTRIES);
            atCmdRespInfoText(cmd->engine, nPromptBuff);
            RETURN_OK(cmd->engine);
        }
        else
        {
            uint32_t ret = ERR_SUCCESS;
            cmd->uti = cfwRequestUTI((osiEventCallback_t)_cpbsRspCB, cmd);
            ret = CFW_SimGetPbkStorage(gAtSetting.tempStorage, cmd->uti, nSim);
            if (ERR_SUCCESS != ret)
            {
                cfwReleaseUTI(cmd->uti);
                RETURN_CME_CFW_ERR(cmd->engine, ret);
            }
            RETURN_FOR_ASYNC();
        }
    }
    else if (cmd->type == AT_CMD_READ) //+CPBS: <storage>[,<used>,<total>]
    {
        OSI_LOGI(0x10004791, "CPBS: gAtSetting.tempStorage %d", gAtSetting.tempStorage);
        if (gAtSetting.tempStorage == CFW_PBK_SIM && CFW_GetSimType(nSim) == 1)
        {
            uint32_t ret = ERR_SUCCESS;

            cmd->uti = cfwRequestUTI((osiEventCallback_t)_cpbsRspCB, cmd);
            ret = CFW_SimGetPbkStrorageInfo(gAtSetting.tempStorage, cmd->uti, nSim);
            if (ERR_SUCCESS != ret)
            {
                cfwReleaseUTI(cmd->uti);
                RETURN_CME_CFW_ERR(cmd->engine, ret);
            }
            RETURN_FOR_ASYNC();
        }
        else if (gAtSetting.tempStorage == CFW_PBK_ME)
        {
            int32_t usenum = NV_GET_STORGE_ME_PBK(MAX_ME_PHONE_ENTRIES);
            if (usenum < 0)
            {
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
            sprintf(nPromptBuff, "+CPBS: \"ME\", %lu, %u", usenum, MAX_ME_PHONE_ENTRIES);
            atCmdRespInfoText(cmd->engine, nPromptBuff);
            RETURN_OK(cmd->engine);
        }
        else
        {
            uint32_t ret = ERR_SUCCESS;

            cmd->uti = cfwRequestUTI((osiEventCallback_t)_cpbsRspCB, cmd);
            ret = CFW_SimGetPbkStorage(gAtSetting.tempStorage, cmd->uti, nSim);
            if (ERR_SUCCESS != ret)
            {
                cfwReleaseUTI(cmd->uti);
                RETURN_CME_CFW_ERR(cmd->engine, ret);
            }
            RETURN_FOR_ASYNC();
        }
    }
    else
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
#endif
}

typedef struct
{
    uint16_t g_nListEntryMaxIndex;    /* Begin list entry index */
    uint16_t g_nListEntryMinIndex;    /* End list entry num */
    uint16_t g_nListEntryLastStep;    /* step length */
    uint16_t g_nListEntryStepCounter; /* list step counter */
    uint16_t g_nMaxNumberSize;        /* number length of liner file */
    uint8_t g_nMaxTextSize;           /* text length of liner file */
} CPBFsyncCtx_t;

static void _cpbfRspCB(atCommand_t *cmd, const osiEvent_t *event)
{
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    uint8_t nSim = cfw_event->nFlag;
    uint32_t nErrorCode = ERR_SUCCESS;
    bool bFindPBKEntry = false;
    if (cfw_event->nType == 0xf0)
    {
        OSI_LOGI(0, "_cpbfRspCB :CfwEvent.nType == 0xF0");
        RETURN_CME_CFW_ERR(cmd->engine, cfw_event->nParam1);
    }
    else if (cfw_event->nType == 0)
    {
        if (cfw_event->nEventId == EV_CFW_SIM_GET_FILE_STATUS_RSP)
        {
#define FCP_TEMPLATE_TAG 0x62
#define FILE_DESCRIPTOR_TAG 0x82
            uint8_t *pData = (uint8_t *)cfw_event->nParam1;
            uint8_t nRecordLength = 0;
            uint8_t offset = 0;
            if (CFW_GetSimType(nSim) == 1) //USIM card
            {
                if (pData[offset++] != FCP_TEMPLATE_TAG)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                offset++;
                if (pData[offset++] != FILE_DESCRIPTOR_TAG)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                if ((pData[offset] != 2) && (pData[offset] != 5))
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
                offset += 2;
                if (pData[offset++] != 0x21)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);

                nRecordLength = (pData[offset] << 8) | pData[offset + 1];
            }
            else
            {
                nRecordLength = pData[14];
                if (nRecordLength == 0)
                    RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
            char nRespone[30] = {0};
            sprintf(nRespone, "+CPBF: %d,%d", 20, nRecordLength - 14);
            atCmdRespInfoText(cmd->engine, nRespone);
            RETURN_OK(cmd->engine);
        }
        if (cfw_event->nEventId == EV_CFW_SIM_GET_PBK_STRORAGE_INFO_RSP)
        {
            OSI_LOGI(0, "cpbf nEventId = EV_CFW_SIM_GET_PBK_STRORAGE_INFO_RSP");
            CFW_PBK_STRORAGE_INFO *pStorageInfo = (CFW_PBK_STRORAGE_INFO *)cfw_event->nParam1;
            CPBFsyncCtx_t *async = (CPBFsyncCtx_t *)malloc(sizeof(*async));
            if (async == NULL)
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_NO_MEMORY);

            atMemFreeLater(pStorageInfo);

            cmd->async_ctx = async;
            cmd->async_ctx_destroy = atCommandAsyncCtxFree;

            async->g_nListEntryLastStep = 0;
            async->g_nListEntryMaxIndex = 0;
            async->g_nListEntryMinIndex = 0;
            async->g_nListEntryStepCounter = 0;

            async->g_nListEntryMinIndex = 1;
            async->g_nListEntryMaxIndex = pStorageInfo->index;
            async->g_nListEntryStepCounter++;
            async->g_nMaxNumberSize = g_nMaxNumberSize;
            async->g_nMaxTextSize = g_nMaxTextSize;

            cmd->uti = cfwRequestUTI((osiEventCallback_t)_cpbfRspCB, cmd);
            if (async->g_nListEntryMaxIndex <= AT_PBK_LIST_ENTRY_STEP * async->g_nListEntryStepCounter++)
            {
                async->g_nListEntryLastStep = 1;
                nErrorCode = CFW_SimListPbkEntries(gAtSetting.tempStorage, async->g_nListEntryMinIndex, async->g_nListEntryMaxIndex, cmd->uti, nSim);
            }
            else
            {
                nErrorCode = CFW_SimListPbkEntries(gAtSetting.tempStorage, async->g_nListEntryMinIndex, AT_PBK_LIST_ENTRY_STEP, cmd->uti, nSim);
            }
            if (ERR_SUCCESS != nErrorCode)
            {
                OSI_LOGI(0, "cpbf CFW_SimListPbkEntries fail");
                cfwReleaseUTI(cmd->uti);
                nErrorCode = AT_SetCmeErrorCode(nErrorCode);
                RETURN_CME_ERR(cmd->engine, nErrorCode);
            }
        }
        if (cfw_event->nEventId == EV_CFW_SIM_LIST_PBK_ENTRY_RSP)
        {
            OSI_LOGI(0, "cpbf nEventId = EV_CFW_SIM_LIST_PBK_ENTRY_RSP");
            uint8_t nEntryCount = 0;
            char MarkCmdStampBuff[8] = {0};
            char PromptBuff[128] = {0};
            CPBFsyncCtx_t *async = (CPBFsyncCtx_t *)cmd->async_ctx;
            nEntryCount = cfw_event->nParam2;
            OSI_LOGI(0, "cpbf nEntryCount = %d", nEntryCount);

            CFW_SIM_PBK_ENTRY_INFO *pPbkEntryInfo = (CFW_SIM_PBK_ENTRY_INFO *)cfw_event->nParam1;
            strcpy(MarkCmdStampBuff, "+CPBF");
            if (nEntryCount != 0)
            {
                atMemFreeLater(pPbkEntryInfo);
            }

            // have Entry found
            while (nEntryCount-- > 0)
            {
                char InternationBuff[2] = {0};

                uint8_t LocolLangNumber[SIM_PBK_NUMBER_SIZE * 2 + 1] = {0}; /* include one char: '\0' */
                //uint32_t nLocolLangNumLen = 0;

                uint8_t pLocolLangTextString[SIM_PBK_NAME_SIZE + 1] = {0}; /* include one char: '\0' */
                uint8_t pOutputTextString[SIM_PBK_NAME_SIZE + 1] = {0};    /* include one char: '\0' */
                pPbkEntryInfo->pNumber = (uint8_t *)pPbkEntryInfo + (uint32_t)sizeof(CFW_SIM_PBK_ENTRY_INFO);
                pPbkEntryInfo->pFullName = (uint8_t *)pPbkEntryInfo + (uint32_t)(sizeof(CFW_SIM_PBK_ENTRY_INFO) + SIM_PBK_NUMBER_SIZE);

                /* clear buffer of prompt  */
                memset(PromptBuff, 0, sizeof(PromptBuff));

                if (2 * (pPbkEntryInfo->nNumberSize) > async->g_nMaxNumberSize)
                {
                    pPbkEntryInfo->nNumberSize = async->g_nMaxNumberSize / 2;
                }

                if (pPbkEntryInfo->iFullNameSize > async->g_nMaxTextSize)
                {
                    pPbkEntryInfo->iFullNameSize = async->g_nMaxTextSize;
                }

                // BCD to asscii
                SUL_GsmBcdToAscii(pPbkEntryInfo->pNumber, pPbkEntryInfo->nNumberSize, LocolLangNumber);

                /* process text string from csw  */
                if (!(AT_PBK_ProcessOutputName(pOutputTextString, sizeof(pOutputTextString), pPbkEntryInfo->pFullName, &(pPbkEntryInfo->iFullNameSize))))
                {
                    OSI_LOGI(0, "cpbf ListPbkEntry resp: some message node exception");

                    /* get list next node, then continue */
                    goto label_next_list_node;
                }

                /* buffer overflow */
                if (pPbkEntryInfo->iFullNameSize > sizeof(pLocolLangTextString))
                {
                    OSI_LOGI(0, "cpbf ListPbkEntry resp:name size of message node more than local buffer len");

                    /* get list next node, then continue */
                    goto label_next_list_node;
                }

                /* save the string to local language string buffer */
                memcpy(pLocolLangTextString, pOutputTextString, pPbkEntryInfo->iFullNameSize);

                /* international or not */
                if (CFW_TELNUMBER_TYPE_INTERNATIONAL == pPbkEntryInfo->nType)
                {
                    strcpy(InternationBuff, "+");
                }

                uint8_t pTempbuff[SIM_PBK_NAME_SIZE + 1] = {0}; /* include one char: '\0' */

                /* buffer overflow */
                if (strlen((char *)pLocolLangTextString) > sizeof(pTempbuff))
                {
                    OSI_LOGI(0, "cpbf ListPbkEntry resp: local language text string is too large");
                    goto label_next_list_node;
                }
                memcpy(pTempbuff, pLocolLangTextString, strlen((char *)pLocolLangTextString));

                if ((strcmp((char *)pLocolLangTextString, (char *)g_arrFindText) == 0) || (strcmp(strupr((char *)pLocolLangTextString), strupr(g_arrFindText)) == 0))
                {
                    bFindPBKEntry = true;
                }

                /* have part of words matching at beginning */
                else if (((uint8_t *)strstr((char *)pLocolLangTextString, g_arrFindText) == (uint8_t *)pLocolLangTextString) ||
                         ((uint8_t *)strstr(strupr((char *)pLocolLangTextString), strupr(g_arrFindText)) == (uint8_t *)pLocolLangTextString))
                {
                    bFindPBKEntry = true;
                }

                if (bFindPBKEntry)
                {
                    sprintf(PromptBuff, "%s: %u,\"%s%s\",%u,\"%s\"", MarkCmdStampBuff, pPbkEntryInfo->phoneIndex,
                            InternationBuff, LocolLangNumber, pPbkEntryInfo->nType, pTempbuff);
                    atCmdRespInfoText(cmd->engine, PromptBuff);
                    bFindPBKEntry = false;
                }
                // Get the next list node
            label_next_list_node:
                pPbkEntryInfo =
                    (CFW_SIM_PBK_ENTRY_INFO *)((uint32_t)pPbkEntryInfo + sizeof(CFW_SIM_PBK_ENTRY_INFO) + SIM_PBK_EXTR_SIZE);
            }

            /*
           **check multi list entry finished or not
         */
            if (1 == async->g_nListEntryLastStep)
            {
                RETURN_OK(cmd->engine);
            }
            else
            {
                /* Length is reach the end point and finished this time */
                if ((async->g_nListEntryMaxIndex - async->g_nListEntryMinIndex + 1) <= AT_PBK_LIST_ENTRY_STEP * (async->g_nListEntryStepCounter + 1))
                {
                    async->g_nListEntryLastStep = 1;
                    cmd->uti = cfwRequestUTI((osiEventCallback_t)_cpbfRspCB, cmd);
                    nErrorCode = CFW_SimListPbkEntries(gAtSetting.tempStorage,
                                                       async->g_nListEntryMinIndex + AT_PBK_LIST_ENTRY_STEP * async->g_nListEntryStepCounter,
                                                       async->g_nListEntryMaxIndex,
                                                       cmd->uti, nSim);
                }

                /* have some items to be continue to list  */
                else
                {
                    cmd->uti = cfwRequestUTI((osiEventCallback_t)_cpbfRspCB, cmd);
                    nErrorCode = CFW_SimListPbkEntries(gAtSetting.tempStorage,
                                                       async->g_nListEntryMinIndex + AT_PBK_LIST_ENTRY_STEP * async->g_nListEntryStepCounter,
                                                       async->g_nListEntryMinIndex + AT_PBK_LIST_ENTRY_STEP * (async->g_nListEntryStepCounter + 1) - 1,
                                                       cmd->uti, nSim);
                    async->g_nListEntryStepCounter++;
                }
                if (ERR_SUCCESS != nErrorCode)
                {
                    cfwReleaseUTI(cmd->uti);
                    nErrorCode = AT_SetCmeErrorCode(nErrorCode);
                    RETURN_CME_ERR(cmd->engine, nErrorCode);
                }
            }
        }
    }
}
uint8_t object = 0;
void atCmdHandleCPBF(atCommand_t *cmd)
{
#if 0
    uint8_t nSim = atCmdGetSim(cmd->engine);
    uint8_t nSimType = CFW_GetSimType(nSim);
    uint8_t nEF = 0;
    bool paramok = true;
    switch (gAtSetting.tempStorage)
    {
    case CFW_PBK_SIM:
        if (nSimType == 1)
            nEF = CFW_GetSimAdnId(nSim);
        else
            nEF = 1; // API_SIM_EF_ADN;
        break;

    case CFW_PBK_SIM_FIX_DIALLING:
        if (nSimType == 1)
            nEF = 68; // API_USIM_EF_FDN;
        else
            nEF = 1; //API_SIM_EF_FDN;
        break;

    case CFW_PBK_SIM_LAST_DIALLING:
        nEF = 7; //API_SIM_EF_LND;
        break;

    case CFW_PBK_SIM_SERVICE_DIALLING:
        if (nSimType == 1)
            nEF = 80; //API_USIM_EF_SDN;
        else
            nEF = 9; //API_SIM_EF_SDN;
        break;

    case CFW_PBK_ON:
        if (nSimType == 1)
            nEF = 72; //API_USIM_EF_MSISDN;
        else
            nEF = 4; //API_SIM_EF_MSISDN;
        break;

    case CFW_PBK_ME:
        //do nothing
        break;

    default:
        OSI_LOGI(0, "cpbf: EXE ERROR");
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
    }

    if (cmd->type == AT_CMD_TEST)
    {
        uint32_t ret = 0;
        if (gAtSetting.tempStorage == CFW_PBK_ME)
            AT_PBK_GetMEStorgeInfo(cmd, nSim);
        else
        {
            cmd->uti = cfwRequestUTI((osiEventCallback_t)_cpbfRspCB, cmd);
            ret = CFW_SimGetFileStatus(nEF, cmd->uti, nSim);
            if (ret != ERR_SUCCESS)
            {
                OSI_LOGI(0, "cpbf test err");
                cfwReleaseUTI(cmd->uti);
                RETURN_CME_CFW_ERR(cmd->engine, ret);
            }
            RETURN_FOR_ASYNC();
        }
    }
    else if (cmd->type == AT_CMD_SET) //+CPBF=<findtext>
    {
        /* Before get find text , zero the buffer */
        memset(g_arrFindText, 0, sizeof(g_arrFindText));
        g_nFindStrLen = sizeof(g_arrFindText);

        if (gAtSetting.cscs == cs_ucs2)
        {
            char InputASCIIFindText[2 * SIM_PBK_NAME_SIZE + 3] = {0};     /* Input ascii string buffer */
            char UCSBigEndFindText[SIM_PBK_NAME_SIZE + 3] = {0};          /* UCS big ending string buffer */
            uint16_t nInputASCIIFindTextLen = sizeof(InputASCIIFindText); /* Input ascii string len */

            const char *InputText = atParamStr(cmd->params[0], &paramok);
            strncpy(InputASCIIFindText, InputText, 2 * SIM_PBK_NAME_SIZE + 3);
            if (!paramok || cmd->param_count != 1)
            {
                OSI_LOGI(0, "cpbf ucs2: input param error");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }

            OSI_LOGI(0x10004793, "cpbf nInputASCIIFindTextLen  = %u", nInputASCIIFindTextLen);
            nInputASCIIFindTextLen = strlen(InputASCIIFindText);
            /* ucs string char conter must be multiple of  4 */
            if (nInputASCIIFindTextLen & 3)
            {
                OSI_LOGI(0, "cpbf: input ucs ascii is not multiple of 4");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }

            if (!(AT_PBK_String2Bytes((uint8_t *)UCSBigEndFindText, (uint8_t *)InputASCIIFindText, (uint8_t *)&nInputASCIIFindTextLen)))
            {
                OSI_LOGI(0, "cpbf: AT_PBK_String2Bytes error");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }

            /* just for debug */
            OSI_LOGI(0x10004794, "UCSBigEndFindText == > ");
            // AT_TC_MEMBLOCK(g_sw_AT_PBK, UCSBigEndFindText, nInputASCIIFindTextLen, 16);
            char *pLocalTextString = mlConvertStr(UCSBigEndFindText, strlen(UCSBigEndFindText), ML_UTF16BE, ML_CP936, NULL);
            strncpy(g_arrFindText, pLocalTextString, SIM_PBK_NAME_SIZE + 3);
            g_nFindStrLen = strlen(g_arrFindText);
        }
        else
        {
            //general parser, get gsm string
            const char *InputText = atParamStr(cmd->params[0], &paramok);
            strncpy(g_arrFindText, InputText, SIM_PBK_NAME_SIZE + 3);
            g_nFindStrLen = strlen(g_arrFindText);

            /* verify the parser result */
            if (!paramok != ERR_SUCCESS)
            {
                OSI_LOGI(0, "cpbf non-ucs2: input param error");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }

            /* check the input text length less than Max text size or not */
            if (g_nFindStrLen > g_nMaxTextSize)
            {
                OSI_LOGI(0, "cpbf non-ucs2: too long text input");
                RETURN_CME_ERR(cmd->engine, ERR_AT_CME_PARAM_INVALID);
            }
        }

        /* just for debug */
        OSI_LOGXI(OSI_LOGPAR_S, 0, "Input string: g_arrFindText = %s,  ", g_arrFindText);
        OSI_LOGI(0, "Input string: g_nFindStrLen = %u, ", g_nFindStrLen);
        if (gAtSetting.tempStorage == CFW_PBK_ME)
        {
            AT_PBK_FindMEEntry(cmd, nSim);
        }
        else
        {
            cmd->uti = cfwRequestUTI((osiEventCallback_t)_cpbfRspCB, cmd);
            uint32_t ret = CFW_SimGetPbkStrorageInfo(gAtSetting.tempStorage, cmd->uti, nSim);
            if (ret != ERR_SUCCESS)
            {
                OSI_LOGI(0, "cpbf: CFW_SimGet");
                cfwReleaseUTI(cmd->uti);
                RETURN_CME_CFW_ERR(cmd->engine, ret);
            }
            RETURN_FOR_ASYNC();
        }
    }
    else
    {
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
#endif
}

#if 0
#include "at.h"
#include "at_module.h"
#include "at_cmd_pbk.h"
#include "at_cfg.h"
#include "cfw_sim_prv.h"
#include "cfw_sim_pbk_nv.h"
#include "at_cmd_sim.h"

/* declare function called from at_cmd_gc.c */
extern at_chset_t cfg_GetTeChset(void);

// //////////////////////////////////////////////////////////////////////////////////////////
//
// Define and desclare the variable
//
// //////////////////////////////////////////////////////////////////////////////////////////

static uint32_t g_PBK_CurCmdStamp = 0;                     /* This stamp is mark which command is executed */
static uint8_t g_arrFindText[SIM_PBK_NAME_SIZE + 3] = {0}; /* Find text content buffer */
static uint16_t g_nFindStrLen = 0;                         /* Find text length */

static uint8_t g_nMaxNumberSize = 0;          /* Max number length, "on","ld","fd","sm" are the same value */
static uint8_t g_nMaxTextSize = 0;            /* Max PBK name length,  "on","ld","fd","sm" are the same value */
static uint16_t g_nMaxSMPBKIndex = 0;         /* Max SM PBK(default pbk) index value */
static uint8_t g_nInitStorageInfoReqFlag = 1; /* When init, storage info request or not */

static uint16_t g_nListEntryMaxIndex = 0;    /* Begin list entry index */
static uint16_t g_nListEntryMinIndex = 0;    /* End list entry num */
static uint16_t g_nListEntryLastStep = 0;    /* step length */
static uint16_t g_nListEntryStepCounter = 0; /* list step counter */
static uint32_t g_nBeforeCNUMStorage = 0;    /* Save  the storage type before execute CNUM */

extern uint32_t cfw_SimParsePBKRecData(uint8_t *pData, CFW_SIM_PBK_ENTRY_INFO *pGetPBKEntry,
                                       void *proc, uint8_t txtlength, uint8_t nApiNum);
void AT_PBK_AsyncEventProcess_GetFDNRecord(CFW_EVENT CfwEvent);
extern bool CFW_GetSimType(CFW_SIM_ID nSimID);
extern uint8_t CFW_GetPbkInfo(CFW_SIM_SEARCH_PBK *pSearchPbk, CFW_SIM_PBK_INFO *pPbkInfo, uint16_t nIndex);
extern void AT_PBK_GetMEStorgeInfo(uint8_t nDLCI, uint8_t nSim);
extern void AT_PBK_ReadMEEntry(uint16_t nStartIndex, uint16_t nEndIndex, uint8_t nDLCI, uint8_t nSim);
extern uint8_t getADNID(uint8_t nIndex, CFW_SIM_ID nSimID);
extern void AT_PBK_FindMEEntry(uint8_t nDLCI, uint8_t nSim);
extern uint32_t CFW_SimSearchPbk(uint8_t nType, uint8_t nObject, uint8_t nMode, uint8_t *nPattern, uint16_t nUTI, CFW_SIM_ID nSimID);
extern void AT_PBK_DeleteMEEntry(uint16_t nIndex, uint8_t nDLCI, uint8_t nSim);
extern void AT_PBK_WriteMEEntry(uint16_t nIndex, uint8_t nNumType, uint8_t *pBCDNumBuf, uint8_t nBCDNumLen,
                                uint8_t *pNameBuf, uint16_t nNameLen, uint8_t nDLCI, uint8_t nSim);

AT_FDN_PBK_LIST *pgATFNDlist[CFW_SIM_COUNT] = {NULL};

void AT_PBK_Init(uint8_t nSim)
{
    OSI_LOGI(0x1000477d, "PBK init beginning .......");

    atEventRegister(EV_CFW_SIM_GET_FILE_STATUS_RSP, AT_PBK_AsyncEventProcess);
    atEventRegister(EV_CFW_SIM_SEARCH_PBK_RSP, AT_PBK_AsyncEventProcess);
    atEventRegister(EV_CFW_SIM_GET_PBK_STRORAGE_RSP, AT_PBK_AsyncEventProcess);
    atEventRegister(EV_CFW_SIM_GET_PBK_STRORAGE_INFO_RSP, AT_PBK_AsyncEventProcess);
    atEventRegister(EV_CFW_SIM_LIST_PBK_ENTRY_RSP, AT_PBK_AsyncEventProcess);
    atEventRegister(EV_CFW_SIM_DELETE_PBK_ENTRY_RSP, AT_PBK_AsyncEventProcess);
    atEventRegister(EV_CFW_SIM_ADD_PBK_RSP, AT_PBK_AsyncEventProcess);
    atEventRegister(EV_CFW_SIM_GET_PBK_ENTRY_RSP, AT_PBK_AsyncEventProcess);

    /* support multi language */
    // ML_Init();

    /* check set SIM as a default value */

    if (0 != CFW_CfgSetPbkStorage(CFW_PBK_SIM, nSim))
        return;

    OSI_LOGI(0x1000477e, "PBK init, the phonebook is: gATCurrentnTempStorage = %u",
             gATCurrentnTempStorage);

    /* Get the Max number size and name size and set request flag */
    OSI_LOGI(0x1000477f, "This request is getting max size of PBK number and PBK text!");
    //g_nInitStorageInfoReqFlag = 1;
    AT_PBK_GetStorageInfoRequest(TRUE, 0, nSim);

    AT_FDN_Init(nSim);
    NV_PBK_Init();

    // Just for debug
    OSI_LOGI(0x10004780, "PBK init OK!");
}

/******************************************************************************************
Function            :   AT_PBK_AsyncEventProcess
Description     :       ATS PBK message deliver fuctions, sometimes we used cmd stamp to
                    distinguish reponse of the different AT command
Called By           :   ATS moudle
Data Accessed   :
Data Updated    :
Input           :   COS_EVENT *pEvent
Output          :
Return          :   void
Others          :   build by unknown author
                    modify and add comment by wangqunyang 2008.04.05
*******************************************************************************************/
void AT_PBK_AsyncEventProcess(COS_EVENT *pEvent)
{
    // /////////////////////////////////////////////////////////////////////////////
    //
    // In order to display event the PBK received in this function, add trace of which event recieved.
    // add by wangqunyang 2008.04.15
    //
    // //////////////////////////////////////////////////////////////////////////////

    CFW_EVENT CfwEvent = {0};
    uint8_t nSim;

    AT_ASSERT(NULL != pEvent);
    AT_CosEvent2CfwEvent(pEvent, &CfwEvent);
    nSim = CfwEvent.nFlag;
    switch (CfwEvent.nEventId)
    {
    case EV_CFW_SIM_GET_FILE_STATUS_RSP:
    {
        if (AT_IsAsynCmdAvailable("+CPBF", g_PBK_CurCmdStamp, CfwEvent.nUTI) == FALSE)
            return;
        if (CfwEvent.nType != 0)
        {
        ERROR_PROCESS:
            AT_PBK_Result_Err(ERR_AT_CME_EXE_FAIL, NULL, 0, "PBK init: getpbkstorage error", 0, nSim);
            AT_ZERO_PARAM1(pEvent);
            return;
        }
#define FCP_TEMPLATE_TAG 0x62
#define FILE_DESCRIPTOR_TAG 0x82
        uint8_t *pData = (uint8_t *)CfwEvent.nParam1;
        uint8_t nRecordLength = 0;
        uint8_t offset = 0;
        if (CFW_GetSimType(nSim) == 1) //USIM card
        {
            if (pData[offset++] != FCP_TEMPLATE_TAG)
                goto ERROR_PROCESS;

            offset++;
            if (pData[offset++] != FILE_DESCRIPTOR_TAG)
                goto ERROR_PROCESS;

            if ((pData[offset] != 2) && (pData[offset] != 5))
                goto ERROR_PROCESS;
            offset += 2;
            if (pData[offset++] != 0x21)
                goto ERROR_PROCESS;

            nRecordLength = (pData[offset] << 8) | pData[offset + 1];
        }
        else
        {
            nRecordLength = pData[14];
            if (nRecordLength == 0)
                goto ERROR_PROCESS;
        }
        uint8_t nRespone[30] = {0};
        sprintf(nRespone, "+CPBF: %d,%d", 20, nRecordLength - 14);
        AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, nRespone, strlen(nRespone), CfwEvent.nUTI, nSim);
        break;
    }
    case EV_CFW_SIM_SEARCH_PBK_RSP:
    {
        CFW_SIM_SEARCH_PBK *pSearchPbk = (CFW_SIM_SEARCH_PBK *)CfwEvent.nParam1;
        if (CfwEvent.nType != 0)
        {
            AT_PBK_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, NULL, 0, "PBK init: getpbkstorage error", 0, nSim);
            return;
        }

        uint16_t i = 0;
        if (pSearchPbk == NULL)
        {
            AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, CfwEvent.nUTI, nSim);
        }
        else
        {
            CSW_TC_MEMBLOCK(CFW_SIM_TS_ID, pSearchPbk->nData, 100, 16);
            uint8_t nResponse[100] = {0};
            uint8_t len = 0;
            OSI_LOGI(0x10004783, "<<<<< Matched = %d, Total = %d>>>>>", pSearchPbk->nMatchRecords, pSearchPbk->nTotalRecords);

            for (i = 0; i < pSearchPbk->nMatchRecords; i++)
            {
                CFW_SIM_PBK_INFO pbkinfo;
                len = 0;
                uint8_t value = CFW_GetPbkInfo(pSearchPbk, &pbkinfo, i + 1);
                if (value == 0xFF)
                {
                    AT_PBK_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, NULL, 0, "Data is not correct!", 0, nSim);
                    return;
                }
                OSI_LOGI(0x10004784, "<<<<< pbkinfo.nIndex = %d, pbkinfo.nType = %d>>>>>", pbkinfo.nIndex, pbkinfo.nType);
                OSI_LOGXI(OSI_LOGPAR_SS, 0x10004785, "<<<<< pbkinfo.nAlpha = %s, pbkinfo.nNumber = %s>>>>>", pbkinfo.nAlpha, pbkinfo.nNumber);
                if (pbkinfo.nType == 145)
                    len = sprintf(nResponse, "+CPBF:%d, \"+%s\", %d, \"%s\"", pbkinfo.nIndex, pbkinfo.nNumber, pbkinfo.nType, pbkinfo.nAlpha);
                else
                {
                    if (strlen(pbkinfo.nAlpha) == 0)
                    {
                        if (strlen(pbkinfo.nNumber) == 0)
                            continue;
                        else
                            len = sprintf(nResponse, "+CPBF:%d, \"%s\", %d,\"\"\r\n", pbkinfo.nIndex, pbkinfo.nNumber, pbkinfo.nType);
                    }
                    else
                        len = sprintf(nResponse, "+CPBF:%d, \"%s\", %d, \"%s\"\r\n", pbkinfo.nIndex, pbkinfo.nNumber, pbkinfo.nType, pbkinfo.nAlpha);
                }
                AT_PBK_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, nResponse, len, CfwEvent.nUTI, nSim);
            }
        }
        AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, CfwEvent.nUTI, nSim);
    }
    break;

    /*************************************************************
    ** Requset: CFW_SimGetPbkStorage()
    ** CPBS: Read mode, because of need data of totalSlot and usedSlot
    ** CPBF:  Set mode
    ** CNUM: Exe mode
    ***************************************************************/
    case EV_CFW_SIM_GET_PBK_STRORAGE_RSP:

        OSI_LOGI(0x10004787, "<<<<<===== AT PBK Get Async Event:  EV_CFW_SIM_GET_PBK_STRORAGE_RSP\n ");
        if ((AT_IsAsynCmdAvailable("+CPBS", g_PBK_CurCmdStamp, CfwEvent.nUTI)) || (AT_IsAsynCmdAvailable("+CPBF", g_PBK_CurCmdStamp, CfwEvent.nUTI)) || (AT_IsAsynCmdAvailable("+CNUM", g_PBK_CurCmdStamp, CfwEvent.nUTI)))
        {
            AT_PBK_AsyncEventProcess_GetPbkStorage(CfwEvent);
        }
        else
        {
            AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, CfwEvent.nUTI, nSim);
            OSI_LOGI(0x10004788, "##### PBK recieved event, but not for our request, discard it !");
        }

        break;

    /*****************************************************
    ** Request: CFW_SimGetPbkStorageInfo()
    ** CPBS: Set mode
    ** CPBR: Test mode
    ** CPBF: Test mode
    ** CPBW: Test mode
    ** PBK init also call this function
    *****************************************************/
    case EV_CFW_SIM_GET_PBK_STRORAGE_INFO_RSP:
        OSI_LOGI(0x10004789, "<<<<<===== AT PBK Get Async Event:  EV_CFW_SIM_GET_PBK_STRORAGE_INFO_RSP\n ");
        if (1 == g_nInitStorageInfoReqFlag)
        {
            /* As PBK init , request for getting max number size, max text length
               ** and max pbk index value and process this response event */
            AT_PBK_AsyncEventProcess_InitSaveMaxSize(CfwEvent);
            g_nInitStorageInfoReqFlag = 0;
        }
        else if ((AT_IsAsynCmdAvailable("+CPBS", g_PBK_CurCmdStamp, CfwEvent.nUTI)) || (AT_IsAsynCmdAvailable("+CPBR", g_PBK_CurCmdStamp, CfwEvent.nUTI)) || (AT_IsAsynCmdAvailable("+CPBF", g_PBK_CurCmdStamp, CfwEvent.nUTI)) || (AT_IsAsynCmdAvailable("+CPBW", g_PBK_CurCmdStamp, CfwEvent.nUTI)) || (AT_IsAsynCmdAvailable("+CNUM", g_PBK_CurCmdStamp, CfwEvent.nUTI)) || (AT_IsAsynCmdAvailable("+SPBR", g_PBK_CurCmdStamp, CfwEvent.nUTI)) || (AT_IsAsynCmdAvailable("+SPBW", g_PBK_CurCmdStamp, CfwEvent.nUTI)))
        {
            AT_PBK_AsyncEventProcess_GetPbkStorageInfo(CfwEvent);
        }
        else
        {
            OSI_LOGI(0x10004788, "##### PBK recieved event, but not for our request, discard it !");
        }

        break;

    /*****************************************************
    ** Request: CFW_SimListPbkEntries()
    ** CPBR: Set mode
    ** CPBF:  Set mode
    ** CNUM: Exe mode
    *****************************************************/
    case EV_CFW_SIM_LIST_PBK_ENTRY_RSP:

        OSI_LOGI(0x1000478a, "<<<<<===== AT PBK Get Async Event:  EV_CFW_SIM_LIST_PBK_ENTRY_RSP\n ");
        if ((AT_IsAsynCmdAvailable("+CPBR", g_PBK_CurCmdStamp, CfwEvent.nUTI)) || (AT_IsAsynCmdAvailable("+SPBR", g_PBK_CurCmdStamp, CfwEvent.nUTI)) || (AT_IsAsynCmdAvailable("+SPBW", g_PBK_CurCmdStamp, CfwEvent.nUTI)) || (AT_IsAsynCmdAvailable("+CPBF", g_PBK_CurCmdStamp, CfwEvent.nUTI)) || (AT_IsAsynCmdAvailable("+CNUM", g_PBK_CurCmdStamp, CfwEvent.nUTI)))
        {
            AT_PBK_AsyncEventProcess_ListPbkEntry(CfwEvent);
        }
        else
        {
            OSI_LOGI(0x10004788, "##### PBK recieved event, but not for our request, discard it !");
        }

        break;

    /*****************************************************
    ** Request: CFW_SimDeletePbkEntry()
    ** CPBW: Set mode
    *****************************************************/
    case EV_CFW_SIM_DELETE_PBK_ENTRY_RSP:
        OSI_LOGI(0x1000478b, "<<<<<===== AT PBK Get Async Event:  EV_CFW_SIM_DELETE_PBK_ENTRY_RSP\n ");
        if (AT_IsAsynCmdAvailable("+CPBW", g_PBK_CurCmdStamp, CfwEvent.nUTI))
        {
            AT_PBK_AsyncEventProcess_DeletePbkEntry(CfwEvent);
        }
        else
        {
            OSI_LOGI(0x10004788, "##### PBK recieved event, but not for our request, discard it !");
        }

        break;

    /*****************************************************
    ** Request: CFW_SimAddPbkEntry()
    ** CPBW: Set mode
    ** CPBW: Exe mode
    *****************************************************/
    case EV_CFW_SIM_ADD_PBK_RSP:

        OSI_LOGI(0x1000478c, "<<<<<===== AT PBK Get Async Event:  EV_CFW_SIM_ADD_PBK_RSP\n ");
        if (AT_IsAsynCmdAvailable("+CPBW", g_PBK_CurCmdStamp, CfwEvent.nUTI))
        {
            AT_PBK_AsyncEventProcess_AddPbkEntry(CfwEvent);
        }
        else
        {
            OSI_LOGI(0x10004788, "##### PBK recieved event, but not for our request, discard it !");
        }

        break;
    case EV_CFW_SIM_GET_PBK_ENTRY_RSP:

        if (pgATFNDlist[nSim] != NULL)
        {
            AT_PBK_AsyncEventProcess_GetFDNRecord(CfwEvent);
        }
        else
        {
            OSI_LOGI(0x1000478d, "##### PBK recieved event, but FDN List is NULL, discard it !");
        }

        break;

    default:

        // do nothing for others
        OSI_LOGI(0x1000478e, "AT PBK-eventid-%d---not be processed!\n", CfwEvent.nEventId);
    }

    return;
}

#if 0
/******************************************************************************************
Function            :   AT_PBK_CmdFunc_CPBS
Description     :       AT CPBS command procedure function
Called By           :   ATS moudle
Data Accessed       :
Data Updated        :
Input           :   AT_CMD_PARA *pParam
Output          :
Return          :   void
Others          :   build by others
                    modify and add comment by wangqunyang 2008.04.05
*******************************************************************************************/
void AT_PBK_CmdFunc_CPBS(AT_CMD_PARA *pParam)
{
    uint8_t nPromptBuff[64] = {0};
    INT32 nOperationRet = ERR_SUCCESS;
    uint8_t nParamCount = 0;
    uint8_t InputStoFlagBuff[8] = {0};
    uint16_t InputBuffLen = 0;
    uint8_t nSim = AT_SIM_ID(pParam->nDLCI);
    if (g_nInitStorageInfoReqFlag == 1)
    {
        AT_PBK_Result_Err(ERR_AT_CME_OPERATION_NOT_SUPPORTED, NULL, 0, "PBK is initializing", pParam->nDLCI, nSim);
        return;
    }

    AT_ASSERT(NULL != pParam);
    g_PBK_CurCmdStamp = pParam->uCmdStamp;

    switch (pParam->iType)
    {
    case AT_CMD_TEST:
        AT_StrCpy(nPromptBuff, "+CPBS: (\"SM\",\"ON\",\"FD\",\"LD\",\"ME\")");
        AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, nPromptBuff, AT_StrLen(nPromptBuff), pParam->nDLCI, nSim);
        break;

    case AT_CMD_SET:

        nOperationRet = AT_Util_GetParaCount(pParam->pPara, &nParamCount);
        if ((nOperationRet != ERR_SUCCESS) || (nParamCount != 1))
        {
            AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbs: input param validate", pParam->nDLCI, nSim);
            return;
        }

        InputBuffLen = sizeof(InputStoFlagBuff);
        nOperationRet = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_STRING,
                                                InputStoFlagBuff, &InputBuffLen);

        if (nOperationRet != ERR_SUCCESS)
        {
            AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbs: param invalid", pParam->nDLCI, nSim);
            return;
        }

        /* to capital letter of input string */
        AT_StrUpr(InputStoFlagBuff);
        if (!AT_PBK_SetPBKCurrentStorage(InputStoFlagBuff))
        {
            AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbs: wrong storage", pParam->nDLCI, nSim);
            return;
        }

        /* Call common request function */

        OSI_LOGI(0x1000478f, "CPBS: gATCurrentnTempStorage = %d", gATCurrentnTempStorage);
        OSI_LOGXI(OSI_LOGPAR_S, 0x10004790, "CPBS: InputStoFlagBuff = %s", InputStoFlagBuff);
        if (gATCurrentnTempStorage == CFW_PBK_SIM && CFW_GetSimType(nSim) == 1)
            AT_PBK_GetStorageInfoRequest(TRUE, pParam->nDLCI, nSim);
        else if (gATCurrentnTempStorage == CFW_PBK_ME)
            AT_PBK_GetMEStorgeInfo(pParam->nDLCI, nSim);
        else
            AT_PBK_GetStorageInfoRequest(FALSE, pParam->nDLCI, nSim);
        break;

    case AT_CMD_READ:
    {
        /* Call common request function */
        OSI_LOGI(0x10004791, "CPBS: gATCurrentnTempStorage %d", gATCurrentnTempStorage);
        if (gATCurrentnTempStorage == CFW_PBK_SIM && CFW_GetSimType(nSim) == 1)
            AT_PBK_GetStorageInfoRequest(TRUE, pParam->nDLCI, nSim);
        else if (gATCurrentnTempStorage == CFW_PBK_ME)
            AT_PBK_GetMEStorgeInfo(pParam->nDLCI, nSim);
        else
            AT_PBK_GetStorageInfoRequest(FALSE, pParam->nDLCI, nSim);
    }
    break;
    default:
        AT_PBK_Result_Err(ERR_AT_CME_OPERATION_NOT_SUPPORTED, NULL, 0, "cpbs: wrong cmd mode", pParam->nDLCI, nSim);
        break;
    }

    return;
}
#endif

#if 0
/******************************************************************************************
Function            :   AT_PBK_CmdFunc_CPBR
Description     :       AT CPBR command procedure function
Called By           :   ATS moudle
Data Accessed   :
Data Updated        :
Input           :   AT_CMD_PARA *pParam
Output          :
Return          :   void
Others          :   build by others
                    modify and add comment by wangqunyang 2008.04.05
*******************************************************************************************/
void AT_PBK_CmdFunc_CPBR(AT_CMD_PARA *pParam)
{
    INT32 nOperationRet = ERR_SUCCESS;
    uint8_t nParamCount = 0;
    uint16_t nIndexStartListEntry = 0;
    uint16_t nIndexEndListEntry = 0;
    uint16_t nParamLen = 0;
    uint8_t nSim = AT_SIM_ID(pParam->nDLCI);

    AT_ASSERT(NULL != pParam);
    g_PBK_CurCmdStamp = pParam->uCmdStamp;
    if (g_nInitStorageInfoReqFlag == 1)
    {
        AT_PBK_Result_Err(ERR_AT_CME_OPERATION_NOT_SUPPORTED, NULL, 0, "PBK is initializing", pParam->nDLCI, nSim);
        return;
    }

    switch (pParam->iType)
    {
    case AT_CMD_TEST:
        if (gATCurrentnTempStorage == CFW_PBK_ME)
            AT_PBK_GetMEStorgeInfo(pParam->nDLCI, nSim);
        else
            AT_PBK_GetStorageInfoRequest(TRUE, pParam->nDLCI, nSim);

        break;

    case AT_CMD_SET:

        nOperationRet = AT_Util_GetParaCount(pParam->pPara, &nParamCount);
        if ((nOperationRet != ERR_SUCCESS) || (0 == nParamCount) || (nParamCount > 2))
        {
            AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbr: param > 2 ", pParam->nDLCI, nSim);
            return;
        }

        /* para1: nIndexStartListEntry */
        if (nParamCount > 0)
        {
            nParamLen = 2;
            nOperationRet = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT16,
                                                    &nIndexStartListEntry, &nParamLen);

            if ((nOperationRet != ERR_SUCCESS) || (0 == nIndexStartListEntry))
            {
                AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbr: param 1 invalid", pParam->nDLCI, nSim);
                return;
            }
        }

        /* para2: nIndexEndListEntry */
        if (nParamCount > 1)
        {
            nParamLen = 2;
            nOperationRet = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_UINT16,
                                                    &nIndexEndListEntry, &nParamLen);

            if ((nOperationRet != ERR_SUCCESS) || (0 == nIndexEndListEntry))
            {
                AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbr: param 2 invalid", pParam->nDLCI, nSim);
                return;
            }
        }

        /* only have one para */
        if (nParamCount == 1)
        {
            nIndexEndListEntry = nIndexStartListEntry;
        }
        if (gATCurrentnTempStorage == CFW_PBK_ME)
        {
            AT_PBK_ReadMEEntry(nIndexStartListEntry, nIndexEndListEntry, pParam->nDLCI, nSim);
            return;
        }
        /* check startindex and endIndex, and check "SM" pbk max index */
        if ((nIndexStartListEntry > nIndexEndListEntry) || (nIndexEndListEntry > g_nMaxSMPBKIndex))
        {
            AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbr: wrong index value", pParam->nDLCI, nSim);
            return;
        }

        /* Reset the multi list param */
        g_nListEntryMaxIndex = 0;
        g_nListEntryMinIndex = 0;
        g_nListEntryLastStep = 0;
        g_nListEntryStepCounter = 0;

        /* get the min index and max index */
        g_nListEntryMaxIndex = nIndexEndListEntry;
        g_nListEntryMinIndex = nIndexStartListEntry;

        g_nListEntryStepCounter++;

        /* multi step list if need */
        if ((nIndexEndListEntry - nIndexStartListEntry + 1) <= AT_PBK_LIST_ENTRY_STEP * g_nListEntryStepCounter)
        {
            g_nListEntryLastStep = 1;
            AT_PBK_ListPBKEntryRequest(nIndexStartListEntry, nIndexEndListEntry, FALSE, pParam->nDLCI, nSim);
        }
        else
        {
            AT_PBK_ListPBKEntryRequest(nIndexStartListEntry, nIndexStartListEntry + AT_PBK_LIST_ENTRY_STEP - 1, FALSE,
                                       pParam->nDLCI, nSim);
        }

        break;

    default:
        AT_PBK_Result_Err(ERR_AT_CME_OPERATION_NOT_SUPPORTED, NULL, 0, "cpbr: wrong cmd mode ", pParam->nDLCI, nSim);
        break;
    }

    return;
}
#endif

#if 0
/******************************************************************************************
Function            :   AT_PBK_CmdFunc_CPBR
Description     :       AT CPBF command procedure function
Called By           :   ATS moudle
Data Accessed   :
Data Updated    :
Input           :   AT_CMD_PARA *pParam
Output          :
Return          :   void
Others          :   build by others
                    modify and add comment by wangqunyang 2008.04.05
*******************************************************************************************/
uint8_t object = 0;
void AT_PBK_CmdFunc_CPBF(AT_CMD_PARA *pParam)
{
    INT32 nOperationRet = ERR_SUCCESS;
    uint8_t nParamCount = 0;

    uint8_t nSim = AT_SIM_ID(pParam->nDLCI);
    AT_ASSERT(NULL != pParam);
    g_PBK_CurCmdStamp = pParam->uCmdStamp;
    uint8_t nSimType = CFW_GetSimType(nSim);
    uint8_t nType = 0;
    uint8_t nEF = 0;
    switch (gATCurrentnTempStorage)
    {
    case CFW_PBK_SIM:
        nType = SEEK_SIM_ADN;
        if (nSimType == 1)
            nEF = getADNID(0, nSim);
        else
            nEF = 1; // API_SIM_EF_ADN;
        break;

    case CFW_PBK_SIM_FIX_DIALLING:
        nType = SEEK_SIM_FDN;
        if (nSimType == 1)
            nEF = 68; // API_USIM_EF_FDN;
        else
            nEF = 1; //API_SIM_EF_FDN;
        break;

    case CFW_PBK_SIM_LAST_DIALLING:
        nType = SEEK_SIM_LND;
        nEF = 7; //API_SIM_EF_LND;
        break;

    case CFW_PBK_SIM_SERVICE_DIALLING:
        nType = SEEK_SIM_SDN;
        if (nSimType == 1)
            nEF = 80; //API_USIM_EF_SDN;
        else
            nEF = 9; //API_SIM_EF_SDN;
        break;

    case CFW_PBK_ON:
        nType = SEEK_SIM_MSISDN;
        if (nSimType == 1)
            nEF = 72; //API_USIM_EF_MSISDN;
        else
            nEF = 4; //API_SIM_EF_MSISDN;
        break;

    case CFW_PBK_ME:
        //do nothing
        break;

    default:
        AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbf: EXE ERROR", pParam->nDLCI, nSim);
        return;
#if 0
    case CFW_PBK_EN:
        if(CFW_GetSimType(nSim) == 1)
            nType = API_USIM_EF_ECC;
        else
            nType = API_SIM_EF_ECC;
        break;
#endif
    }

    switch (pParam->iType)
    {
    case AT_CMD_TEST:
    {
        uint32_t value = 0;
        if (gATCurrentnTempStorage == CFW_PBK_ME)
            AT_PBK_GetMEStorgeInfo(pParam->nDLCI, nSim);
#if 0
        else if(gATCurrentnTempStorage == CFW_PBK_EN)
        {
            value = CFW_SimGetFileStatus(nType, pParam->nDLCI, nSim);
        }
#endif
        else
            value = CFW_SimGetFileStatus(nEF, pParam->nDLCI, nSim);

        if (value != ERR_SUCCESS)
            AT_PBK_Result_Err(value, NULL, 0, "cpbf: EXE ERROR", pParam->nDLCI, nSim);
        break;
    }
    case AT_CMD_SET:

//Test GetAlpha
#ifdef TEST_GET_ALPHA
    {
        uint8_t nEF[10] = {0};
        uint16_t nLen = 10;
        uint32_t value = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_STRING, nEF, &nLen);
        if (value != ERR_SUCCESS)
        {
            AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbf: first parameter is invalidate", pParam->nDLCI, nSim);
            return;
        }

        uint8_t nNumber[20] = {0};
        uint16_t nLength = 20;
        value = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_STRING, nNumber, &nLength);
        if (value != ERR_SUCCESS)
        {
            AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbf: second parameter is invalidate", pParam->nDLCI, nSim);
            return;
        }

        uint8_t nType = 0;
        if (memcmp(nEF, "ADN", 3) == 0)
            nType = SEEK_SIM_ADN;
        else if (memcmp(nEF, "FDN", 3) == 0)
            nType = SEEK_SIM_FDN;
        else if (memcmp(nEF, "MSISDN", 6) == 0)
            nType = SEEK_SIM_MSISDN;
        //    else if(memcmp(nEF,"BDN",3) == 0)
        //        nType = SEEK_SIM_BDN;
        else if (memcmp(nEF, "SDN", 3) == 0)
            nType = SEEK_SIM_SDN;
        else if (memcmp(nEF, "LND", 3) == 0)
            nType = SEEK_SIM_LND;
        else
        {
            AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbf: first parameter is not allowed", pParam->nDLCI, nSim);
            return;
        }

        uint8_t nObject[10] = {0};
        nLength = 10;
        value = AT_Util_GetParaWithRule(pParam->pPara, 2, AT_UTIL_PARA_TYPE_STRING, &nObject, &nLength);
        if (value != ERR_SUCCESS)
        {
            AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbf: third parameter is invalidate", pParam->nDLCI, nSim);
            return;
        }

        if (memcmp(nObject, "NAME", 4) == 0)
            object = SEEK_SIM_NAME;
        else if (memcmp(nObject, "NUMBER", 6) == 0)
            object = SEEK_SIM_NUMBER;
        else if (memcmp(nObject, "NULL", 4) == 0)
            object = SEEK_SIM_USED;
        else
        {
            AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbf: third parameter is not allowed", pParam->nDLCI, nSim);
            return;
        }
        OSI_LOGXI(OSI_LOGPAR_SSS, 0x10004792, "CPBF:nEF = %s, nNumber = %s, nObject = %s", nEF, nNumber, nObject);

        value = CFW_SimSearchPbk(nType, object, ALL_MATCH, nNumber, pParam->nDLCI, nSim);
        if (value != ERR_SUCCESS)
        {
            AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbf: CFW_SimGetPbkAlpha", pParam->nDLCI, nSim);
            return;
        }
        AT_PBK_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        return 0;
    }
#endif
        //End Testing

        nOperationRet = AT_Util_GetParaCount(pParam->pPara, &nParamCount);
        if ((nOperationRet != ERR_SUCCESS) || (nParamCount != 1))
        {
            AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbf: nParamCount !=1", pParam->nDLCI, nSim);
            return;
        }

        /* Before get find text , zero the buffer */
        AT_MemZero(g_arrFindText, sizeof(g_arrFindText));
        g_nFindStrLen = sizeof(g_arrFindText);

        /*
           ** UCS2 parser, get ucs2 string and transfer to bytes
         */
        if (cfg_GetTeChset() == cs_ucs2)
        {
            uint8_t InputASCIIFindText[2 * SIM_PBK_NAME_SIZE + 3] = {0};  /* Input ascii string buffer */
            uint8_t UCSBigEndFindText[SIM_PBK_NAME_SIZE + 3] = {0};       /* UCS big ending string buffer */
            uint8_t UCSLittleEndFindText[SIM_PBK_NAME_SIZE + 3] = {0};    /* UCS little ending string buffer */
            uint16_t nInputASCIIFindTextLen = sizeof(InputASCIIFindText); /* Input ascii string len */

            uint8_t *pLocalTextString = NULL;  /* Local language text string */
            uint32_t nLocalTextStringSize = 0; /* local language text string len */

            nOperationRet = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_STRING,
                                                    InputASCIIFindText, &nInputASCIIFindTextLen);

            /* verify the parser result */
            if (nOperationRet != ERR_SUCCESS)
            {
                AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbf ucs2: input param error ", pParam->nDLCI, nSim);
                return;
            }

            OSI_LOGI(0x10004793, "nInputASCIIFindTextLen  = %u", nInputASCIIFindTextLen);

            /* ucs string char conter must be multiple of  4 */
            if (nInputASCIIFindTextLen & 3)
            {
                AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbf: input ucs ascii is not multiple of 4",
                                  pParam->nDLCI, nSim);
                return;
            }

            if (!(AT_String2Bytes(UCSBigEndFindText, InputASCIIFindText, (uint8_t *)&nInputASCIIFindTextLen)))
            {
                AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbf: AT_String2Bytes error", pParam->nDLCI, nSim);
                return;
            }

            /* just for debug */
            OSI_LOGI(0x10004794, "UCSBigEndFindText == > ");
            AT_TC_MEMBLOCK(g_sw_AT_PBK, UCSBigEndFindText, nInputASCIIFindTextLen, 16);

            /* set current language types */
            AT_Set_MultiLanguage();

            /* transfer ucs2 big ending to little ending */
            if (!(AT_UnicodeBigEnd2Unicode(UCSBigEndFindText, UCSLittleEndFindText, (uint16_t)nInputASCIIFindTextLen)))
            {
                AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbf: ucs bigend to little ending wrong", pParam->nDLCI,
                                  nSim);
                return;
            }

            /* ucs2 to local language */
            if (ERR_SUCCESS !=
                ML_Unicode2LocalLanguage(UCSLittleEndFindText, (uint32_t)(nInputASCIIFindTextLen), &pLocalTextString,
                                         &nLocalTextStringSize, NULL))
            {
                AT_PBK_Result_Err(ERR_AT_CME_EXE_FAIL, NULL, 0, "cpbf: unicode to local language exception", pParam->nDLCI,
                                  nSim);
                return;
            }

            /* check copy string length */
            if (nLocalTextStringSize > g_nFindStrLen)
            {
                AT_PBK_Result_Err(ERR_AT_CME_EXE_FAIL, NULL, 0, "cpbf: local text len > find buffer len", pParam->nDLCI, nSim);
                return;
            }

            AT_MemCpy(g_arrFindText, pLocalTextString, nLocalTextStringSize);
            g_nFindStrLen = nLocalTextStringSize;

            /* check the input text length less than Max text size or not,
               ** the unicode first octet is: [80], or [81], or [82], so the length -1 */
            if (g_nFindStrLen > g_nMaxTextSize - 1)
            {
                AT_PBK_Result_Err(ERR_AT_CME_TEXT_LONG, NULL, 0, "cpbf ucs2: too long text input", pParam->nDLCI, nSim);
                return;
            }

            // Free inout para resource
            AT_FREE(pLocalTextString);
        }
        else
        {
            /*
               ** general parser, get gsm string
             */
            nOperationRet = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_STRING,
                                                    g_arrFindText, &g_nFindStrLen);

            /* verify the parser result */
            if (nOperationRet != ERR_SUCCESS)
            {
                AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbf non-ucs2: input param error ", pParam->nDLCI, nSim);
                return;
            }

            /* check the input text length less than Max text size or not */
            if (g_nFindStrLen > g_nMaxTextSize)
            {
                AT_PBK_Result_Err(ERR_AT_CME_TEXT_LONG, NULL, 0, "cpbf non-ucs2: too long text input ", pParam->nDLCI, nSim);
                return;
            }
        }

        /* just for debug */
        OSI_LOGI(0x10004795, "Input string: g_nFindStrLen = %u, and  find text is: ==> ", g_nFindStrLen);
        AT_TC_MEMBLOCK(g_sw_AT_PBK, g_arrFindText, g_nFindStrLen, 16);
        if (gATCurrentnTempStorage == CFW_PBK_ME)
        {
            AT_PBK_FindMEEntry(pParam->nDLCI, nSim);
        }
        else
        {
            /* CPBF: step 1: get current totalSlot */
            if (strlen(g_arrFindText) != 0)
                CFW_SimSearchPbk(nType, SEEK_SIM_NAME, WILDCARD_MATCH, g_arrFindText, pParam->nDLCI, nSim);
            else
                CFW_SimSearchPbk(nType, SEEK_SIM_USED, ALL_MATCH, g_arrFindText, pParam->nDLCI, nSim);
            AT_PBK_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_CR, 0, NULL, 0, pParam->nDLCI, nSim);
        }

        break;

    default:
        AT_PBK_Result_Err(ERR_AT_CME_OPERATION_NOT_SUPPORTED, NULL, 0, "cpbf: wrong cmd mode", pParam->nDLCI, nSim);
        break;
    }
    return;
}

/******************************************************************************************
Function            :   AT_PBK_CmdFunc_CPBW
Description     :       AT CPBW command procedure function
Called By           :   ATS moudle
Data Accessed       :
Data Updated        :
Input           :   AT_CMD_PARA *pParam
Output          :
Return          :   void
Others          :   build by others
                    modify and add comment by wangqunyang 2008.04.05
*******************************************************************************************/
void AT_PBK_CmdFunc_CPBW(AT_CMD_PARA *pParam)
{

    CFW_SIM_PBK_ENTRY_INFO sEntryToBeAdded = {0};

    uint8_t pPhoneFullName[SIM_PBK_NAME_SIZE + 3] = {0}; /* include one '\0' and two ' " ' */
    uint16_t nPhoneFullNameSize = 0;
    uint8_t pPhoneNumber[SIM_PBK_NUMBER_SIZE * 2 + 4] = {0}; /* include one '\0' and two ' " ' and one + */
    uint16_t nPhoneNumSize = 0;
    uint8_t nPhoneNumType = 0;
    uint16_t nPBKEntryIndex = 0;

    bool bInternationFlag = FALSE;
    uint8_t PhoneNumberBCDBuff[SIM_PBK_NUMBER_SIZE + 1] = {0}; /* include one '\0'  */
    uint8_t nBCDNumberLen = 0;

    INT32 nOperationRet = ERR_SUCCESS;
    uint8_t nParamCount = 0;
    uint8_t nStorage = 0;
    uint16_t nUTI = 0;
    uint16_t nParamLen = 0;

    /* For ucs2 pbk entry */
    uint8_t InputAsciiPhoneFullName[4 * SIM_PBK_NAME_SIZE + 3] = {0}; /* include one '\0' and two ' " ' */
    uint8_t InputBytesPhoneFullName[SIM_PBK_NAME_SIZE + 3] = {0};     /* include one '\0' and two ' " ' */
    uint8_t nSim = AT_SIM_ID(pParam->nDLCI);
    if (g_nInitStorageInfoReqFlag == 1)
    {
        AT_PBK_Result_Err(ERR_AT_CME_OPERATION_NOT_SUPPORTED, NULL, 0, "PBK is initializing", pParam->nDLCI, nSim);
        return;
    }

    AT_ASSERT(NULL != pParam);
    g_PBK_CurCmdStamp = pParam->uCmdStamp;

    switch (pParam->iType)
    {
    case AT_CMD_TEST:
        if (gATCurrentnTempStorage == CFW_PBK_ME)
            AT_PBK_GetMEStorgeInfo(pParam->nDLCI, nSim);
        else
            AT_PBK_GetStorageInfoRequest(TRUE, pParam->nDLCI, nSim);

        break;

    case AT_CMD_SET:
        nOperationRet = AT_Util_GetParaCount(pParam->pPara, &nParamCount);
        if ((nOperationRet != ERR_SUCCESS) || (nParamCount > 4) || (nParamCount == 0))
        {
            AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbw: param input wrong", pParam->nDLCI, nSim);
            return;
        }

        // get cuurent storage
        nStorage = gATCurrentnTempStorage;

        if (!((nStorage == CFW_PBK_ON) || (nStorage == CFW_PBK_SIM_FIX_DIALLING) || (nStorage == CFW_PBK_SIM_LAST_DIALLING) || (nStorage == CFW_PBK_SIM) || (nStorage == CFW_PBK_ME)))
        {
            AT_PBK_Result_Err(ERR_AT_CME_EXE_FAIL, NULL, 0, "cpbw: input wrong storage ", pParam->nDLCI, nSim);
            return;
        }

        // /////////////////////////////////////////////////////////////
        // input string have para1
        // /////////////////////////////////////////////////////////////
        if (nParamCount > 0)
        {
            nParamLen = 2;
            nOperationRet = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT16, &nPBKEntryIndex, &nParamLen);

            if (ERR_AT_UTIL_CMD_PARA_NULL == nOperationRet)
            {
                nPBKEntryIndex = 0; /* non para input, default value is set */
            }
            else
            {
                if (nOperationRet != ERR_SUCCESS)
                {
                    AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbw:param 1 invalid", pParam->nDLCI, nSim);
                    return;
                }
            }
        }

        // /////////////////////////////////////////////////////////////
        // input string have para2
        // /////////////////////////////////////////////////////////////
        if (nParamCount > 1)
        {
            nPhoneNumSize = sizeof(pPhoneNumber);
            nOperationRet = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_STRING, pPhoneNumber, &nPhoneNumSize);

            if (nOperationRet != ERR_SUCCESS)
            {
                AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbw:param 2 invalid", pParam->nDLCI, nSim);
                return;
            }

            if (!(AT_PBK_IsValidPhoneNumber(pPhoneNumber, nPhoneNumSize, &bInternationFlag)))
            {
                AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbw:param 2 invalid phone number", pParam->nDLCI, nSim);
                return;
            }

            if (bInternationFlag)
            {
                /* verify the max number size, include '+' char */
                if (nPhoneNumSize > (g_nMaxNumberSize + 1))
                {
                    AT_PBK_Result_Err(ERR_AT_CME_DAIL_STR_LONG, NULL, 0, "cpbw: param 2 - 1 > max size ", pParam->nDLCI, nSim);
                    return;
                }

                nBCDNumberLen = SUL_AsciiToGsmBcd(&(pPhoneNumber[1]), nPhoneNumSize - 1, PhoneNumberBCDBuff);
            }
            else
            {
                /* verify the max number size, without '+' char */
                if (nPhoneNumSize > g_nMaxNumberSize)
                {
                    AT_PBK_Result_Err(ERR_AT_CME_DAIL_STR_LONG, NULL, 0, "cpbw: param 2 - 2 > max size ", pParam->nDLCI, nSim);
                    return;
                }
                nBCDNumberLen = SUL_AsciiToGsmBcd(pPhoneNumber, nPhoneNumSize, PhoneNumberBCDBuff);
            }
        }

        // /////////////////////////////////////////////////////////////
        // input string have para3
        // /////////////////////////////////////////////////////////////
        if (nParamCount > 2)
        {
            nParamLen = 1;

            /* Reset the value */
            nPhoneNumType = 0;

            nOperationRet = AT_Util_GetParaWithRule(pParam->pPara, 2, AT_UTIL_PARA_TYPE_UINT8, &nPhoneNumType, &nParamLen);

            if (ERR_AT_UTIL_CMD_PARA_NULL == nOperationRet)
            {
                nPhoneNumType = CFW_TELNUMBER_TYPE_UNKNOWN; /* Default value is set */
            }
            else
            {
                if (nOperationRet != ERR_SUCCESS)
                {
                    AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbw:param 3 invalid", pParam->nDLCI, nSim);
                    return;
                }
            }

            if ((nPhoneNumType != CFW_TELNUMBER_TYPE_INTERNATIONAL) && (nPhoneNumType != CFW_TELNUMBER_TYPE_NATIONAL) && (nPhoneNumType != CFW_TELNUMBER_TYPE_UNKNOWN))
            {
                AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbw:param 3 dial number type wrong", pParam->nDLCI,
                                  nSim);
                return;
            }
        }

        // /////////////////////////////////////////////////////////////
        // input string have para4
        // /////////////////////////////////////////////////////////////
        if (nParamCount > 3)
        {

            /*
               ** UCS2 parser, get ucs2 string and transfer to bytes
             */
            if (cfg_GetTeChset() == cs_ucs2)
            {

                nPhoneFullNameSize = sizeof(InputAsciiPhoneFullName);
                nOperationRet = AT_Util_GetParaWithRule(pParam->pPara, 3, AT_UTIL_PARA_TYPE_STRING,
                                                        InputAsciiPhoneFullName, &nPhoneFullNameSize);

                /* ucs string char conter must be multiple of  4 */
                if (nPhoneFullNameSize & 3)
                {
                    AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbw: input ucs ascii is not multiple of 4",
                                      pParam->nDLCI, nSim);
                    return;
                }

                /* just for debug */
                OSI_LOGI(0x10004796, "Before StringToBytes: ascii string len = %u, and input ascii data is: == > ",
                         nPhoneFullNameSize);
                AT_TC_MEMBLOCK(g_sw_AT_PBK, InputAsciiPhoneFullName, nPhoneFullNameSize, 16);

                if (!(AT_String2Bytes(InputBytesPhoneFullName, InputAsciiPhoneFullName, (uint8_t *)&nPhoneFullNameSize)))
                {
                    AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbw:param 4 AT_String2Bytes error", pParam->nDLCI,
                                      nSim);
                    return;
                }

                /* just for debug */
                OSI_LOGI(0x10004797, "After StringToBytes: bytes len = %u, and input ascii data is: == > ", nPhoneFullNameSize);
                AT_TC_MEMBLOCK(g_sw_AT_PBK, InputBytesPhoneFullName, nPhoneFullNameSize, 16);
            }
            else
            {
                /*
                   ** general parser, get gsm string
                 */
                nPhoneFullNameSize = sizeof(InputBytesPhoneFullName);
                nOperationRet = AT_Util_GetParaWithRule(pParam->pPara, 3, AT_UTIL_PARA_TYPE_STRING,
                                                        InputBytesPhoneFullName, &nPhoneFullNameSize);
            }

            /* process the input PBK entry text string */
            if (!(AT_PBK_ProcessInputName(pPhoneFullName, sizeof(pPhoneFullName), InputBytesPhoneFullName, (uint8_t *)&nPhoneFullNameSize)))
            {
                AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbw:process input name error", pParam->nDLCI, nSim);
                return;
            }

            /* just for debug */
            OSI_LOGI(0x10004798, "g_nMaxTextSize  = %u nPhoneFullNameSize = %u ", g_nMaxTextSize, nPhoneFullNameSize);

            /* check the parse result and save the text at buffer */
            if (ERR_AT_UTIL_CMD_PARA_NULL == nOperationRet)
            {
                AT_MemZero(pPhoneFullName, sizeof(pPhoneFullName));
                ; /* text is null */
                nPhoneFullNameSize = 0;
            }
            else
            {
                /* check the input parameters */
                if (nOperationRet != ERR_SUCCESS)
                {
                    AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbw:param 4 error ", pParam->nDLCI, nSim);
                    return;
                }

                /* check the input text length less than Max text size or not */
                if (nPhoneFullNameSize > g_nMaxTextSize)
                {
                    AT_PBK_Result_Err(ERR_AT_CME_TEXT_LONG, NULL, 0, "cpbw:input text too long", pParam->nDLCI, nSim);
                    return;
                }
            }
        }

        /*
           ** Process only have nWriteEntryIndex
         */
        if ((0 != nPBKEntryIndex) && (1 == nParamCount))
        {
            if (CFW_PBK_ME == nStorage)
            {
                AT_PBK_DeleteMEEntry(nPBKEntryIndex, pParam->nDLCI, nSim);
                return;
            }
            // if current pbk is "ON", delete operation is not allowed.
#if 0
            if (CFW_PBK_ON == nStorage)
            {
                AT_PBK_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, NULL, 0, "CPBW error: ON phonebook can't be delete",
                                  pParam->nDLCI, nSim);
                return;
            }
#endif
            /* get the UTI and free it after finished calling */
            if (0 == (nUTI = AT_AllocUserTransID()))
            {
                AT_PBK_Result_Err(ERR_AT_CME_EXE_FAIL, NULL, 0, "CPBW: malloc UTI error", pParam->nDLCI, nSim);
                return;
            }
            nOperationRet = CFW_SimDeletePbkEntry(nStorage, nPBKEntryIndex, pParam->nDLCI, nSim);
            if (CFW_PBK_SIM_FIX_DIALLING == nStorage)
            {
                AT_FDN_DelRecord(nPBKEntryIndex, nSim);
            }

            AT_FreeUserTransID(nUTI);
            if (nOperationRet != ERR_SUCCESS)
            {
                nOperationRet = AT_SetCmeErrorCode(nOperationRet, TRUE);
                AT_PBK_Result_Err(nOperationRet, NULL, 0, "cpbw:CFW_SimDeletePbkEntry check param error", pParam->nDLCI, nSim);
                return;
            }

            AT_PBK_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_CR, 0, NULL, 0, pParam->nDLCI, nSim);
            return;
        }

        /* the number must not be NULL when the counters
           ** of param is more than one, if only have one param: index,
           ** it not be zero, otherwise, error happened.bug 8315 related */
        else if ((0 == nPBKEntryIndex) && (1 == nParamCount))
        {
            AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbw:only one param but index is zero", pParam->nDLCI,
                              nSim);
            return;
        }

        /* according to spec, we check dial number first */
        /* Judge the num type, if is 0, set default value */
        if (bInternationFlag)
        {
            nPhoneNumType = CFW_TELNUMBER_TYPE_INTERNATIONAL;
        }
        else
        {
            if (0 == nPhoneNumType)
            {
                /* no input num type  */
                nPhoneNumType = CFW_TELNUMBER_TYPE_UNKNOWN;
            }
            else if (CFW_TELNUMBER_TYPE_INTERNATIONAL == nPhoneNumType)
            {
                /* have no '+' , but the num type is 145, replace 129 with is  */
                nPhoneNumType = CFW_TELNUMBER_TYPE_UNKNOWN;
            }
            else
            {
                /* get the input num type  */
            }
        }

        /* Before write the entry printf the data, debug these */
        OSI_LOGI(0x10004799, "need to write pbk text: ==>");
        AT_TC_MEMBLOCK(g_sw_AT_PBK, pPhoneFullName, nPhoneFullNameSize, 16);

        /* Set value to Entry written */
        sEntryToBeAdded.phoneIndex = nPBKEntryIndex;
        sEntryToBeAdded.nType = nPhoneNumType;

        sEntryToBeAdded.pNumber = PhoneNumberBCDBuff;
        sEntryToBeAdded.nNumberSize = nBCDNumberLen;

        sEntryToBeAdded.pFullName = pPhoneFullName;
        sEntryToBeAdded.iFullNameSize = nPhoneFullNameSize;

        // if current pbk is "LD", write operation is not allowed.
        if (nStorage == CFW_PBK_SIM_LAST_DIALLING)
        {
            AT_PBK_Result_Err(ERR_AT_CME_OPERATION_NOT_ALLOWED, NULL, 0, "CPBW error: LD phonebook can't be written!!!",
                              pParam->nDLCI, nSim);
            return;
        }

        /* get the UTI and free it after finished calling */
        if (0 == (nUTI = AT_AllocUserTransID()))
        {
            AT_PBK_Result_Err(ERR_AT_CME_EXE_FAIL, NULL, 0, "CPBW: malloc UTI error", pParam->nDLCI, nSim);
            return;
        }

        // process add entry in the pbk
        if (CFW_PBK_SIM_FIX_DIALLING == nStorage)
        {
            AT_FDN_AddRecord(nPBKEntryIndex, &sEntryToBeAdded, nSim);
        }
        else if (CFW_PBK_ME == nStorage)
        {
            AT_PBK_WriteMEEntry(nPBKEntryIndex, nPhoneNumType, PhoneNumberBCDBuff, nBCDNumberLen,
                                pPhoneFullName, nPhoneFullNameSize, pParam->nDLCI, nSim);
            return;
        }
        nOperationRet = CFW_SimAddPbkEntry(nStorage, &sEntryToBeAdded, pParam->nDLCI, nSim);
        AT_FreeUserTransID(nUTI);
        if (nOperationRet != ERR_SUCCESS)
        {
            nOperationRet = AT_SetCmeErrorCode(nOperationRet, TRUE);
            AT_PBK_Result_Err(nOperationRet, NULL, 0, "CPBW: CFW_SimAddPbkEntry check param  error", pParam->nDLCI, nSim);
            return;
        }

        AT_PBK_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_CR, 0, NULL, 0, pParam->nDLCI, nSim);
        break;

    default:
        AT_PBK_Result_Err(ERR_AT_CME_OPERATION_NOT_SUPPORTED, NULL, 0, "cpbw: wrong cmd mode", pParam->nDLCI, nSim);
        break;
    }

    return;
}

/******************************************************************************************
Function            :   AT_PBK_CmdFunc_CNUM
Description     :       AT CNUM command,get own number
Called By           :   ATS moudle
Data Accessed       :
Data Updated        :
Input           :   AT_CMD_PARA *pParam
Output          :
Return          :   void
Others          :   build by others
                    modify and add comment by wangqunyang 2008.04.05
*******************************************************************************************/
void AT_PBK_CmdFunc_CNUM(AT_CMD_PARA *pParam)
{
    uint8_t nSim = AT_SIM_ID(pParam->nDLCI);
    AT_ASSERT(NULL != pParam);
    g_PBK_CurCmdStamp = pParam->uCmdStamp;

    if (pParam->iType == AT_CMD_TEST)
    {
        AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI, nSim);
        return;
    }
    else if (pParam->iType == AT_CMD_EXE)
    {
        /* CUNM step 1: */
        if (g_nInitStorageInfoReqFlag == 1)
        {
            AT_PBK_Result_Err(ERR_AT_CME_OPERATION_NOT_SUPPORTED, NULL, 0, "PBK is initializing", pParam->nDLCI, nSim);
        }

        g_nBeforeCNUMStorage = gATCurrentnTempStorage;
        /* we backup the current stroage to restore */
        gATCurrentnTempStorage = CFW_PBK_ON;
        AT_PBK_GetStorageInfoRequest(FALSE, pParam->nDLCI, nSim);
        return;
    }
    else
    {
        AT_PBK_Result_Err(ERR_AT_CME_OPERATION_NOT_SUPPORTED, NULL, 0, "cnum: wrong cmd mode", pParam->nDLCI, nSim);
        return;
    }
}
#endif

/******************************************************************************************
Function            :   AT_PBK_ProcessInputName
Description     :
Called By           :   AT_PBK_CmdFunc_CPBW()
Data Accessed       :
Data Updated    :
Input           :       uint8_t* arrDest, uint8_t* arrSource, uint8_t* pLen
Output          :
Return          :   uint32_t
Others          :   build  by wangqunyang 2008.05.22
*******************************************************************************************/
bool AT_PBK_ProcessInputName(uint8_t *arrDest, uint8_t nDestLen, uint8_t *arrSource, uint8_t *pLen)
{

    if ((arrDest == NULL) || (arrSource == NULL))
    {
        OSI_LOGI(0x1000479b, "AT_PBK_ProcessInputName: input pointer is NULL");
        return FALSE;
    }

    if (cfg_GetTeChset() == cs_ucs2)
    {
        arrDest[0] = 0x80;

        if (*pLen > (nDestLen - 1))
        {
            OSI_LOGI(0x1000479c, "AT_PBK_ProcessInputName: source size > dest buffer size 1");
            return FALSE;
        }

        AT_MemCpy(&arrDest[1], arrSource, *pLen);

        *pLen += 1;
    }
    else
    {
        if (*pLen > nDestLen)
        {
            OSI_LOGI(0x1000479d, "AT_PBK_ProcessInputName: source size > dest buffer size 2");
            return FALSE;
        }

        AT_MemCpy(arrDest, arrSource, *pLen);
    }

    return TRUE;
}

/******************************************************************************************
Function            :   AT_PBK_ProcessOutputName
Description     :
Called By           :   AT_PBK_AsyncEventProcess_ListPbkEntry()
Data Accessed       :
Data Updated    :
Input           :       uint8_t* arrDest, uint8_t* arrSource, uint8_t* pLen
Output          :
Return          :   uint32_t
Others          :   build by wangqunyang 2008.05.22
*******************************************************************************************/
bool AT_PBK_ProcessOutputName(uint8_t *arrDest, uint8_t nDestLen, uint8_t *arrSource, uint8_t *pLen)
{
    uint8_t *pTempTextString = NULL; /* include one char: '\0' */
    uint32_t nTextSzie = 0;
    uint8_t pUCSLittleEndData[142] = {0};

    if ((arrDest == NULL) || (arrSource == NULL))
    {
        OSI_LOGI(0x1000479e, "AT_PBK_ProcessOutputName: input pointer is NULL");
        return FALSE;
    }

    if (0x80 == arrSource[0])
    {

        AT_Set_MultiLanguage();

        if (!(AT_UnicodeBigEnd2Unicode((uint8_t *)(&arrSource[1]), pUCSLittleEndData, (uint16_t)(*pLen - 1))))
        {
            OSI_LOGI(0x1000479f, "AT_PBK_ProcessOutputName: BigEnd to LittleEnd exception");
            return FALSE;
        }

        if (ERR_SUCCESS !=
            ML_Unicode2LocalLanguage(pUCSLittleEndData, (uint32_t)(*pLen - 1), &pTempTextString, &nTextSzie, NULL))
        {
            OSI_LOGI(0x100047a0, "AT_PBK_ProcessOutputName: ucs2 to local language exception");
            return FALSE;
        }

        if (nTextSzie > nDestLen)
        {
            OSI_LOGI(0x100047a1, "AT_PBK_ProcessOutputName: text size > dest buffer size");
            return FALSE;
        }

        AT_MemCpy(arrDest, pTempTextString, nTextSzie);

        *pLen = nTextSzie;

        // Free inout para resource
        AT_FREE(pTempTextString);
    }
    else
    {
        if (*pLen > nDestLen)
        {
            OSI_LOGI(0x100047a2, "AT_PBK_ProcessOutputName: source size > dest buffer size");
            return FALSE;
        }

        AT_MemCpy(arrDest, arrSource, *pLen);
    }

    return TRUE;
}

/******************************************************************************************
Function            :   AT_PBK_IsValidPhoneNumber
Description     :       arrNumber[in]; pNumberSize[in]; bIsInternational[out]
                    legal phone number char: 0-9,*,#,+ (+ can only be the first position)
Called By           :   AT_PBK_CmdFunc_CPBW()
Data Accessed       :
Data Updated    :
Input           :       uint8_t * arrNumber, uint8_t nNumberSize, bool * bIsInternational
Output          :
Return          :   uint32_t
Others          :   build by others
                    modify and add comment by wangqunyang 2008.04.05
*******************************************************************************************/
bool AT_PBK_IsValidPhoneNumber(uint8_t *arrNumber, uint8_t nNumberSize, bool *bIsInternational)
{
    uint8_t nTemp = 0;

    *bIsInternational = FALSE;

    /* The number must not be NULL and  nNumberSize is not zero */
    if ((NULL == arrNumber) || (0 == nNumberSize))
    {
        OSI_LOGI(0x100047a3, "AT_PBK_IsValidPhoneNumber: number or num size is 0");
        return FALSE;
    }

    if (arrNumber[0] == '+')
    {
        *bIsInternational = TRUE;
        nTemp = 1;
    }

    while (nTemp < nNumberSize)
    {
        if ((arrNumber[nTemp] != '#') && (arrNumber[nTemp] != '*') && ((arrNumber[nTemp] < '0') || (arrNumber[nTemp] > '9')))
        {
            OSI_LOGI(0x100047a4, "AT_PBK_IsValidPhoneNumber: invalid char in the number");
            return FALSE;
        }

        nTemp++;
    }

    return TRUE;
}

/******************************************************************************************
Function            :   AT_PBK_SetPBKCurrentStorage
Description     :
Called By           :   PBK moudle
Data Accessed       :
Data Updated    :
Input           :   uint8_t *pStorageFlag
Output          :
Return          :   bool
Others          :   build by wangqunyang 2008.04.22
*******************************************************************************************/
bool AT_PBK_SetPBKCurrentStorage(uint8_t *pStorageFlag)
{
    if (AT_StrCmp(pStorageFlag, "SM") == 0)
    {
        gATCurrentnTempStorage = CFW_PBK_SIM;
    }
    else if (AT_StrCmp(pStorageFlag, "FD") == 0)
    {
        gATCurrentnTempStorage = CFW_PBK_SIM_FIX_DIALLING;
    }
    else if (AT_StrCmp(pStorageFlag, "LD") == 0)
    {
        gATCurrentnTempStorage = CFW_PBK_SIM_LAST_DIALLING;
    }
    else if (AT_StrCmp(pStorageFlag, "ON") == 0)
    {
        gATCurrentnTempStorage = CFW_PBK_ON;
    }
    else if (AT_StrCmp(pStorageFlag, "ME") == 0)
    {
        gATCurrentnTempStorage = CFW_PBK_ME;
    }
    else
    {
        OSI_LOGI(0x100047a5, "AT_PBK_SetPBKCurrentStorage: storage invalid");
        return FALSE;
    }

    return TRUE;
}

/******************************************************************************************
Function            :   AT_PBK_SetPBKCurrentStorage
Description     :
Called By           :   PBK moudle
Data Accessed       :
Data Updated    :
Input           :   uint8_t *pStorageFlag
Output          :
Return          :   bool
Others          :   build by wangqunyang 2008.04.22
*******************************************************************************************/
bool AT_PBK_GetPBKCurrentStorage(uint16_t nStorageId, uint8_t *pStorageBuff)
{

    if (nStorageId == CFW_PBK_SIM)
    {
        AT_StrCpy(pStorageBuff, "SM");
    }
    else if (nStorageId == CFW_PBK_ON)
    {
        AT_StrCpy(pStorageBuff, "ON");
    }
    else if (nStorageId == CFW_PBK_SIM_FIX_DIALLING)
    {
        AT_StrCpy(pStorageBuff, "FD");
    }
    else if (nStorageId == CFW_PBK_SIM_LAST_DIALLING)
    {
        AT_StrCpy(pStorageBuff, "LD");
    }
    else if (nStorageId == CFW_PBK_ME)
    {
        AT_StrCpy(pStorageBuff, "ME");
    }
    else
    {
        OSI_LOGI(0x100047a6, "AT_PBK_GetPBKCurrentStorage: storage invalid");
        return FALSE;
    }

    return TRUE;
}

void AT_PBK_GetMEStorgeInfo(uint8_t nDLCI, uint8_t nSim)
{
    INT32 usenum = 0;
    uint8_t nPromptBuff[64] = {0};
    if (AT_IsAsynCmdAvailable("+CPBS", g_PBK_CurCmdStamp, nDLCI))
    {
        usenum = NV_GET_STORGE_ME_PBK(MAX_ME_PHONE_ENTRIES);
        if (usenum < 0)
        {
            AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbs: NVRAM read failed!", nDLCI, nSim);
            return;
        }
        sprintf(nPromptBuff, "+CPBS: \"ME\", %u, %u", usenum, MAX_ME_PHONE_ENTRIES);
        AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, nPromptBuff, AT_StrLen(nPromptBuff), nDLCI, nSim);
    }
    else if (AT_IsAsynCmdAvailable("+CPBR", g_PBK_CurCmdStamp, nDLCI))
    {
        sprintf(nPromptBuff, "+CPBR: (1-%u),%u,%u",
                   MAX_ME_PHONE_ENTRIES, ME_PBK_NUMBER_SIZE * 2, ME_PBK_NAME_SIZE);
        AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, nPromptBuff, AT_StrLen(nPromptBuff), nDLCI, nSim);
    }
    else if (AT_IsAsynCmdAvailable("+CPBW", g_PBK_CurCmdStamp, nDLCI))
    {
        sprintf(nPromptBuff, "+CPBW: (1-%u),%u,(129,145,161),%u",
                   MAX_ME_PHONE_ENTRIES, ME_PBK_NUMBER_SIZE * 2, ME_PBK_NAME_SIZE);
        AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, nPromptBuff, AT_StrLen(nPromptBuff), nDLCI, nSim);
    }
    else if (AT_IsAsynCmdAvailable("+CPBF", g_PBK_CurCmdStamp, nDLCI))
    {
        sprintf(nPromptBuff, "+CPBF: %u,%u", ME_PBK_NUMBER_SIZE * 2, ME_PBK_NAME_SIZE);
        AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, nPromptBuff, AT_StrLen(nPromptBuff), nDLCI, nSim);
    }

    if (AT_IsAsynCmdAvailable("+SPBR", g_PBK_CurCmdStamp, nDLCI))
    {
        sprintf(nPromptBuff, "+SPBR: (1-%u),%u,%u",
                   MAX_ME_PHONE_ENTRIES, ME_PBK_NUMBER_SIZE * 2, ME_PBK_NAME_SIZE);
        AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, nPromptBuff, AT_StrLen(nPromptBuff), nDLCI, nSim);
    }
    else if (AT_IsAsynCmdAvailable("+SPBW", g_PBK_CurCmdStamp, nDLCI))
    {
        sprintf(nPromptBuff, "+SPBW: (1-%u),%u,(129,145,161),%u",
                   MAX_ME_PHONE_ENTRIES, ME_PBK_NUMBER_SIZE * 2, ME_PBK_NAME_SIZE);
        AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, nPromptBuff, AT_StrLen(nPromptBuff), nDLCI, nSim);
    }
    return;
}

void AT_PBK_ReadMEEntry(uint16_t nStartIndex, uint16_t nEndIndex, uint8_t nDLCI, uint8_t nSim)
{
    if ((nStartIndex > nEndIndex) || (nEndIndex > MAX_ME_PHONE_ENTRIES) || (nStartIndex < 1))
    {
        AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbr: (ME) wrong index value", nDLCI, nSim);
        return;
    }

    INT32 iRet;
    NV_ME_PHB_INFO sEntryForME = {0};
    uint8_t pMeBuff[NV_ME_BUFF_LEN] = {0};
    uint8_t pPhoneNumber[SIM_PBK_NUMBER_SIZE * 2 + 4] = {0};
    uint8_t nPromptBuff[64] = {0};
    uint8_t InternationBuff[2] = {0};

    for (int i = nStartIndex; i <= nEndIndex; i++)
    {
        SUL_ZeroMemory8(InternationBuff, 2);
        iRet = NV_READ_ME_PBK(i, pMeBuff);
        if (iRet == ERR_SUCCESS)
        {
            Buf2PBKI(pMeBuff, &sEntryForME);
            if (sEntryForME.index != (NV_ME_DEFAULT_VALUE << 8 | NV_ME_DEFAULT_VALUE))
            {
                SUL_GsmBcdToAscii(sEntryForME.tel, sEntryForME.tel_len, pPhoneNumber);
                if (CFW_TELNUMBER_TYPE_INTERNATIONAL == sEntryForME.type)
                {
                    AT_StrCpy(InternationBuff, "+");
                }
                sprintf(nPromptBuff, "+CPBR: %u,\"%s%s\",%u,\"%s\"", i,
                           InternationBuff, pPhoneNumber, sEntryForME.type, sEntryForME.name);
                AT_PBK_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, nPromptBuff, AT_StrLen(nPromptBuff), nDLCI, nSim);
            }
            else
                continue;
        }
        else
        {
            AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbw: NVRAM read failed!", nDLCI, nSim);
            return;
        }
    }

    AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, nDLCI, nSim);
    return;
}

void AT_PBK_WriteMEEntry(uint16_t nIndex, uint8_t nNumType, uint8_t *pBCDNumBuf, uint8_t nBCDNumLen,
                         uint8_t *pNameBuf, uint16_t nNameLen, uint8_t nDLCI, uint8_t nSim)
{
    if ((nIndex > MAX_ME_PHONE_ENTRIES) || (nIndex < 1))
    {
        AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbw: (ME) wrong index value", nDLCI, nSim);
        return;
    }

    INT32 iRet;
    NV_ME_PHB_INFO sEntryForME = {0};
    uint8_t pMeBuff[NV_ME_BUFF_LEN] = {0};
    sEntryForME.index = nIndex;
    sEntryForME.type = nNumType;
    SUL_MemCopy8(sEntryForME.tel, pBCDNumBuf, nBCDNumLen);
    sEntryForME.tel_len = nBCDNumLen;
    SUL_MemCopy8(sEntryForME.name, pNameBuf, nNameLen);
    sEntryForME.name_len = nNameLen;
    PBKI2Buf(&sEntryForME, pMeBuff);

    iRet = NV_WRITE_ME_PBK(nIndex, pMeBuff, NV_ME_WRITE_ADD);
    if (iRet == ERR_SUCCESS)
        AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, nDLCI, nSim);
    else
        AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbw: NVRAM write failed!", nDLCI, nSim);

    return;
}

void AT_PBK_DeleteMEEntry(uint16_t nIndex, uint8_t nDLCI, uint8_t nSim)
{
    if ((nIndex > MAX_ME_PHONE_ENTRIES) || (nIndex < 1))
    {
        AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbw: (ME) wrong index value", nDLCI, nSim);
        return;
    }

    INT32 iRet;
    uint8_t pMeBuff[NV_ME_BUFF_LEN] = {0};
    SUL_MemSet8(pMeBuff, NV_ME_DEFAULT_VALUE, NV_ME_BUFF_LEN);

    iRet = NV_WRITE_ME_PBK(nIndex, pMeBuff, NV_ME_WRITE_DEL);
    if (iRet == ERR_SUCCESS)
        AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, nDLCI, nSim);
    else
        AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbw: NVRAM write failed!", nDLCI, nSim);

    return;
}

void AT_PBK_FindMEEntry(uint8_t nDLCI, uint8_t nSim)
{
    uint8_t pTempbuff[SIM_PBK_NAME_SIZE + 1] = {0}; /* include one char: '\0' */
    uint8_t pMeBuff[NV_ME_BUFF_LEN] = {0};
    uint8_t pLocolLangTextString[SIM_PBK_NAME_SIZE + 1] = {0}; /* include one char: '\0' */
    uint8_t pOutputTextString[SIM_PBK_NAME_SIZE + 1] = {0};    /* include one char: '\0' */
    NV_ME_PHB_INFO sEntryForME = {0};
    uint8_t pPhoneNumber[SIM_PBK_NUMBER_SIZE * 2 + 4] = {0};
    uint8_t nPromptBuff[64] = {0};
    INT32 iRet;
    bool bFindPBKEntry = FALSE;
    uint8_t InternationBuff[2] = {0};

    for (int i = 1; i <= MAX_ME_PHONE_ENTRIES; i++)
    {
        SUL_ZeroMemory8(pTempbuff, SIM_PBK_NAME_SIZE + 1);
        SUL_ZeroMemory8(pLocolLangTextString, SIM_PBK_NAME_SIZE + 1);
        SUL_ZeroMemory8(pOutputTextString, SIM_PBK_NAME_SIZE + 1);
        SUL_ZeroMemory8(InternationBuff, 2);
        iRet = NV_READ_ME_PBK(i, pMeBuff);
        if (iRet == ERR_SUCCESS)
        {
            Buf2PBKI(pMeBuff, &sEntryForME);
            if (sEntryForME.index != (NV_ME_DEFAULT_VALUE << 8 | NV_ME_DEFAULT_VALUE))
            {
                AT_PBK_ProcessOutputName(pOutputTextString, sizeof(pOutputTextString),
                                         sEntryForME.name, &(sEntryForME.name_len));
                AT_MemCpy(pLocolLangTextString, pOutputTextString, sEntryForME.name_len);
                AT_MemCpy(pTempbuff, pLocolLangTextString, AT_StrLen(pLocolLangTextString));
                if ((!AT_StrCmp(pLocolLangTextString, g_arrFindText)) || (!AT_StrCmp(AT_StrUpr(pLocolLangTextString), AT_StrUpr(g_arrFindText))))
                {
                    bFindPBKEntry = TRUE;
                }
                else if ((strstr(pLocolLangTextString, g_arrFindText) == (INT8 *)pLocolLangTextString) || (strstr(AT_StrUpr(pLocolLangTextString), AT_StrUpr(g_arrFindText)) ==
                                                                                                           (INT8 *)pLocolLangTextString))
                {
                    bFindPBKEntry = TRUE;
                }
                if (CFW_TELNUMBER_TYPE_INTERNATIONAL == sEntryForME.type)
                {
                    AT_StrCpy(InternationBuff, "+");
                }
                SUL_GsmBcdToAscii(sEntryForME.tel, sEntryForME.tel_len, pPhoneNumber);
                if (bFindPBKEntry)
                {
                    sprintf(nPromptBuff, "+CPBF: %u,\"%s%s\",%u,\"%s\"", sEntryForME.index,
                               InternationBuff, pPhoneNumber, sEntryForME.type, pTempbuff);
                    AT_PBK_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, nPromptBuff, AT_StrLen(nPromptBuff), nDLCI, nSim);
                    bFindPBKEntry = FALSE;
                }
            }
            else
            {
                continue;
            }
        }
        else
        {
            AT_PBK_Result_Err(ERR_AT_CME_PARAM_INVALID, NULL, 0, "cpbf: NV read failed!", nDLCI, nSim);
            return;
        }
    }

    AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, nDLCI, nSim);
    return;
}

/******************************************************************************************
Function            :   AT_PBK_GetStorageInfo
Description     :
Called By           :   ATS PBK moudle
Data Accessed   :
Data Updated        :
Input           :   PBK_STORAEINFO_CALLER ePbkStorageInfoCaller, the flag mark call function with
                    CFW_SimGetPbkStrorageInfo() or function with CFW_SimGetPbkStorage()
Output          :
Return          :   void
Others          :   build by wangqunyang 2008.04.23
*******************************************************************************************/
void AT_PBK_GetStorageInfoRequest(bool bCallStorageInofFuncFlag, uint16_t nUTI, uint8_t nSim, bool in_cmd)
{
    uint8_t nStorage = 0;
    uint32_t nGetStorageResult = ERR_SUCCESS;
    uint16_t nUTI = nDLCI;

    nStorage = gATCurrentnTempStorage;

    if (bCallStorageInofFuncFlag)
    {
        OSI_LOGI(0x100047a7, "CFW_SimGetPbkStrorageInfo");
        nGetStorageResult = CFW_SimGetPbkStrorageInfo(nStorage, nUTI, nSim);
    }
    else
    {
        OSI_LOGI(0x100047a8, "CFW_SimGetPbkStorage");
        nGetStorageResult = CFW_SimGetPbkStorage(nStorage, nUTI, nSim);
    }

    if (in_cmd)
    {
        if (nGetStorageResult != 0)
            atCmdRespCmeError(engine, AT_SetCmeErrorCode(nGetStorageResult));
        else
            AT_SetAsyncTimerMux(engine, 100);
    }
}

/******************************************************************************************
Function            :   AT_PBK_ListPBKEntryRequest
Description     :       1. initial a request of list pbk
                    2. multi steps of list, and the step we can custom
Called By           :   ATS PBK moudle
Data Accessed   :
Data Updated        :
Input           :   uint8_t nIndexStart,
                    uint8_t nIndexEnd
                    bool bContinueFlag, according to AT_Notify2ATM(), we used different
                             macro: CMD_FUNC_CONTINUE or CMD_FUNC_SUCC_ASYN
Output          :
Return          :   void
Others          :   build by wangqunyang 2008.04.23
*******************************************************************************************/
void AT_PBK_ListPBKEntryRequest(uint16_t nIndexStart, uint16_t nIndexEnd, bool bContinueFlag, uint8_t nDLCI, uint8_t nSim)
{
    uint8_t nStorage = 0;
    uint32_t nListEntryResult = ERR_SUCCESS;
    uint16_t nUTI = nDLCI;

    nStorage = gATCurrentnTempStorage;

    OSI_LOGI(0x100047aa, "pbk list: nIndexStart = %u,  nIndexEnd = %u", nIndexStart, nIndexEnd);

    /* get the UTI and free it after finished calling */
    if (0 == (nUTI = AT_AllocUserTransID()))
    {
        AT_PBK_Result_Err(ERR_AT_CME_EXE_FAIL, NULL, 0, "pbk list entry:malloc UTI error", nDLCI, nSim);
        return;
    }
    nListEntryResult = CFW_SimListPbkEntries(nStorage, nIndexStart, nIndexEnd, nDLCI, nSim);
    AT_FreeUserTransID(nUTI);

    /* pbk list entry: param check failure */
    if (ERR_SUCCESS != nListEntryResult)
    {
        nListEntryResult = AT_SetCmeErrorCode(nListEntryResult, TRUE);
        AT_PBK_Result_Err(nListEntryResult, NULL, 0, "pbk list entry: param check failure ", nDLCI, nSim);
        return;
    }

    if (bContinueFlag)
    {
        AT_PBK_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_CR, 0, NULL, 0, nDLCI, nSim);
    }
    else
    {
        AT_PBK_Result_OK(CMD_FUNC_SUCC_ASYN, CMD_RC_CR, 0, NULL, 0, nDLCI, nSim);
    }

    return;
}

/******************************************************************************************
Function            :   AT_PBK_AsyncEventProcess_GetPbkStorage
Description     :
Called By           :   ATS moudle
Data Accessed       :
Data Updated        :
Input           :   CFW_EVENT CfwEvent
Output          :
Return          :   void
Others          :   build by others
                    modify and add comment by wangqunyang 2008.04.05
*******************************************************************************************/
void AT_PBK_AsyncEventProcess_GetPbkStorage(CFW_EVENT CfwEvent)
{
    uint8_t PromptBuff[64] = {0};
    uint8_t StorageInfoBuff[8] = {0};
    uint32_t nErrorCode = 0;

    CFW_PBK_STORAGE *pPbkStorage = NULL;
    uint8_t nStorage = 0;

    uint8_t nSim = CfwEvent.nFlag;
    switch (CfwEvent.nType)
    {
    case 0:

        pPbkStorage = (CFW_PBK_STORAGE *)CfwEvent.nParam1;
        nStorage = gATCurrentnTempStorage;

        if (!AT_PBK_GetPBKCurrentStorage(pPbkStorage->storageId, StorageInfoBuff))
        {
            AT_PBK_Result_Err(ERR_AT_CME_EXE_FAIL, NULL, 0, "GetPbkStorage resp: GetCurrentStorage is false", CfwEvent.nUTI,
                              nSim);
            return;
        };

        // //////////////////////////////////////////////////////////
        // CPBS Read mode
        // /////////////////////////////////////////////////////////
        if (AT_IsAsynCmdAvailable("+CPBS", g_PBK_CurCmdStamp, CfwEvent.nUTI))
        {
            sprintf(PromptBuff, "+CPBS: \"%s\",%u,%u", StorageInfoBuff, pPbkStorage->usedSlot, pPbkStorage->totalSlot);
            AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, PromptBuff, AT_StrLen(PromptBuff), CfwEvent.nUTI, nSim);
            return;
        }

        // //////////////////////////////////////////////////////////
        // CPBF Read mode
        // //////////////////////////////////////////////////////////
        else if (AT_IsAsynCmdAvailable("+CPBF", g_PBK_CurCmdStamp, CfwEvent.nUTI))
        {
            /* Reset the multi step list param */
            g_nListEntryMaxIndex = 0;
            g_nListEntryLastStep = 0;
            g_nListEntryMinIndex = 0;
            g_nListEntryStepCounter = 0;

            /* CPBF: step 2: list usedslot */
            /* get the total list number of entry */
            g_nListEntryMinIndex = 1;
            g_nListEntryMaxIndex = pPbkStorage->totalSlot;
            g_nListEntryStepCounter++;
            if (g_nListEntryMaxIndex <= AT_PBK_LIST_ENTRY_STEP * g_nListEntryStepCounter)
            {
                g_nListEntryLastStep = 1;
                AT_PBK_ListPBKEntryRequest(1, pPbkStorage->totalSlot, TRUE, CfwEvent.nUTI, nSim);
            }
            else
            {
                AT_PBK_ListPBKEntryRequest(1, AT_PBK_LIST_ENTRY_STEP, TRUE, CfwEvent.nUTI, nSim);
            }

            return;
        }

        // //////////////////////////////////////////////////////////
        // CNUM Read mode
        // //////////////////////////////////////////////////////////
        else if (AT_IsAsynCmdAvailable("+CNUM", g_PBK_CurCmdStamp, CfwEvent.nUTI))
        {
            if (!(nStorage == CFW_PBK_ON))
            {
                AT_PBK_Result_Err(ERR_AT_CME_EXE_FAIL, NULL, 0, "GetPbkStorage resp of cnum: storage is not 'ON' and failure",
                                  CfwEvent.nUTI, nSim);
                return;
            }

            /* Reset the multi step list param */
            g_nListEntryMaxIndex = 0;
            g_nListEntryLastStep = 0;
            g_nListEntryMinIndex = 0;
            g_nListEntryStepCounter = 0;

            /* CUNM step 2: */
            /* get the total list number of entry */
            g_nListEntryMinIndex = 1;
            g_nListEntryMaxIndex = pPbkStorage->totalSlot;
            g_nListEntryStepCounter++;
            if (g_nListEntryMaxIndex <= AT_PBK_LIST_ENTRY_STEP * g_nListEntryStepCounter)
            {
                g_nListEntryLastStep = 1;
                AT_PBK_ListPBKEntryRequest(1, pPbkStorage->totalSlot, TRUE, CfwEvent.nUTI, nSim);
            }
            else
            {
                AT_PBK_ListPBKEntryRequest(1, AT_PBK_LIST_ENTRY_STEP, TRUE, CfwEvent.nUTI, nSim);
            }

            return;
        }
        else
        {
            AT_PBK_Result_Err(ERR_AT_CME_EXE_FAIL, NULL, 0, "GetPbkStorage resp : wrong cmd stamp", CfwEvent.nUTI, nSim);
            return;
        }

        break;

    case 0xF0:

        nErrorCode = AT_SetCmeErrorCode(CfwEvent.nParam1, FALSE);
        AT_PBK_Result_Err(nErrorCode, NULL, 0, "GetPbkStorage resp :CfwEvent.nType == 0xF0", CfwEvent.nUTI, nSim);
        break;

    default:
        AT_PBK_Result_Err(ERR_AT_CME_EXE_FAIL, NULL, 0, "GetPbkStorage resp : unknown CfwEvent.nType", CfwEvent.nUTI, nSim);
        break;
    }

    return;
}

/******************************************************************************************
Function            :   AT_PBK_AsyncEventProcess_GetPbkStorageInfo
Description     :
Called By           :   ATS moudle
Data Accessed   :
Data Updated        :
Input           :   CFW_EVENT CfwEvent
Output          :
Return          :   void
Others          :   build by others
                    modify and add comment by wangqunyang 2008.04.05
*******************************************************************************************/
void AT_PBK_AsyncEventProcess_GetPbkStorageInfo(CFW_EVENT CfwEvent)
{
    uint8_t PromptBuff[64] = {0};
    uint8_t pStorageBuff[8] = {0};
    uint32_t nErrorCode = ERR_SUCCESS;
    CFW_PBK_STRORAGE_INFO *pPbkStorageInfo = {0};
    uint8_t nSim = CfwEvent.nFlag;
    OSI_LOGI(0x100047ab, "========AT_PBK_AsyncEventProcess_GetPbkStorageInfo========");
    switch (CfwEvent.nType)
    {
    case 0:

        pPbkStorageInfo = (CFW_PBK_STRORAGE_INFO *)CfwEvent.nParam1;

        if (AT_IsAsynCmdAvailable("+CPBS", g_PBK_CurCmdStamp, CfwEvent.nUTI))
        {
            uint8_t *pBitmap = pPbkStorageInfo->Bitmap;
            uint16_t nIndex = pPbkStorageInfo->index;
            nIndex = (nIndex + 7) >> 3; //bytes of bitmap
            OSI_LOGI(0x100047ac, "nIndex = %d, total = %d", nIndex, pPbkStorageInfo->index);
            uint16_t nUsed = 0;
            uint16_t i = 0;

            for (i = 0; i < nIndex; i++)
            {
                nUsed += pBitmap[i] & 0x01;
                OSI_LOGI(0x100047ad, "===== pBitmap[i] = 0x%x =====", pBitmap[i]);
                OSI_LOGI(0x100047ae, "nUsed = %d", nUsed);
                for (uint8_t j = 1; j < 8; j++)
                {
                    pBitmap[i] = pBitmap[i] >> 1;
                    nUsed += pBitmap[i] & 0x01;
                    //                    AT_TC(g_sw_AT_PBK, "pBitmap[%d] = 0x%x, j = %d", i, pBitmap[i], j);
                    //                    AT_TC(g_sw_AT_PBK, "nUsed = %d", nUsed);
                }
            }
            if (gATCurrentnTempStorage == CFW_PBK_ON)
            {
                AT_StrCpy(pStorageBuff, "ON");
            }
            else if (gATCurrentnTempStorage == CFW_PBK_SIM_FIX_DIALLING)
            {
                AT_StrCpy(pStorageBuff, "FD");
            }
            else if (gATCurrentnTempStorage == CFW_PBK_SIM_LAST_DIALLING)
            {
                AT_StrCpy(pStorageBuff, "LD");
            }
            else
            {
                AT_StrCpy(pStorageBuff, "SM");
            }
            OSI_LOGI(0x100047af, "nUsed = %d, nTotal = %d", nUsed, pPbkStorageInfo->index);
            sprintf(PromptBuff, "+CPBS: \"%s\",%u,%u", pStorageBuff, nUsed, pPbkStorageInfo->index);
            AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, PromptBuff, AT_StrLen(PromptBuff), CfwEvent.nUTI, nSim);
            return;
        }

        if (AT_IsAsynCmdAvailable("+CPBR", g_PBK_CurCmdStamp, CfwEvent.nUTI))
        {
            sprintf(PromptBuff, "+CPBR: (1-%u),%u,%u",
                       pPbkStorageInfo->index, (pPbkStorageInfo->iNumberLen) * 2, pPbkStorageInfo->txtLen);
            AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, PromptBuff, AT_StrLen(PromptBuff), CfwEvent.nUTI, nSim);
            return;
        }
        if (AT_IsAsynCmdAvailable("+SPBR", g_PBK_CurCmdStamp, CfwEvent.nUTI))
        {
            sprintf(PromptBuff, "+SPBR: (1-%u),%u,%u",
                       pPbkStorageInfo->index, (pPbkStorageInfo->iNumberLen) * 2, pPbkStorageInfo->txtLen);
            AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, PromptBuff, AT_StrLen(PromptBuff), CfwEvent.nUTI, nSim);
            return;
        }
        if (AT_IsAsynCmdAvailable("+CPBF", g_PBK_CurCmdStamp, CfwEvent.nUTI))
        {
            sprintf(PromptBuff, "+CPBF: %u,%u", (pPbkStorageInfo->iNumberLen) * 2, pPbkStorageInfo->txtLen);
            AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, PromptBuff, AT_StrLen(PromptBuff), CfwEvent.nUTI, nSim);
            return;
        }

        if (AT_IsAsynCmdAvailable("+CPBW", g_PBK_CurCmdStamp, CfwEvent.nUTI))
        {
            sprintf(PromptBuff, "+CPBW: (1-%u),%u,(129,145,161),%u",
                       pPbkStorageInfo->index, (pPbkStorageInfo->iNumberLen) * 2, pPbkStorageInfo->txtLen);
            AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, PromptBuff, AT_StrLen(PromptBuff), CfwEvent.nUTI, nSim);
            return;
        }
        if (AT_IsAsynCmdAvailable("+SPBW", g_PBK_CurCmdStamp, CfwEvent.nUTI))
        {
            sprintf(PromptBuff, "+SPBW: (1-%u),%u,(129,145,161),%u",
                       pPbkStorageInfo->index, (pPbkStorageInfo->iNumberLen) * 2, pPbkStorageInfo->txtLen);
            AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, PromptBuff, AT_StrLen(PromptBuff), CfwEvent.nUTI, nSim);
            return;
        }
        AT_PBK_Result_Err(ERR_AT_CME_EXE_FAIL, NULL, 0, "GetPbkStorageInfo resp: wrong cmd stamp", CfwEvent.nUTI, nSim);
        break;

    case 0xF0:
        nErrorCode = AT_SetCmeErrorCode(CfwEvent.nParam1, FALSE);
        AT_PBK_Result_Err(nErrorCode, NULL, 0, "GetPbkStorageInfo resp: CfwEvent.nType == 0xF0", CfwEvent.nUTI, nSim);
        break;

    default:
        AT_PBK_Result_Err(ERR_AT_CME_EXE_FAIL, NULL, 0, "GetPbkStorageInfo resp:: unknown CfwEvent.nType", CfwEvent.nUTI,
                          nSim);
        break;
    }

    return;
}

/******************************************************************************************
Function            :   AT_PBK_AsyncEventProcess_ListPbkEntry
Description     :
Called By           :   ATS moudle
Data Accessed   :
Data Updated        :
Input           :   CFW_EVENT CfwEvent
Output          :
Return          :   void
Others          :   build by others
                    modify and add comment by wangqunyang 2008.04.05
*******************************************************************************************/
void AT_PBK_AsyncEventProcess_ListPbkEntry(CFW_EVENT CfwEvent)
{
    uint8_t PromptBuff[128] = {0};
    uint32_t nErrorCode = 0;
    uint8_t nEntryCount = 0;

    CFW_SIM_PBK_ENTRY_INFO *pPbkEntryInfo = NULL;
    uint8_t MarkCmdStampBuff[8] = {0};
    bool bFindPBKEntry = FALSE;
    uint8_t nSim = CfwEvent.nFlag;

    switch (CfwEvent.nType)
    {
    case 0:
    {
        nEntryCount = CfwEvent.nParam2;
        pPbkEntryInfo = (CFW_SIM_PBK_ENTRY_INFO *)CfwEvent.nParam1;

        // ///////////////////////////////////////
        /* Get the AT command cmd stamp          */
        // ///////////////////////////////////////
        if (AT_IsAsynCmdAvailable("+CPBR", g_PBK_CurCmdStamp, CfwEvent.nUTI))
        {
            AT_StrCpy(MarkCmdStampBuff, "+CPBR");
        }
        else if (AT_IsAsynCmdAvailable("+CPBF", g_PBK_CurCmdStamp, CfwEvent.nUTI))
        {
            AT_StrCpy(MarkCmdStampBuff, "+CPBF");
        }
        else if (AT_IsAsynCmdAvailable("+CNUM", g_PBK_CurCmdStamp, CfwEvent.nUTI))
        {
            AT_StrCpy(MarkCmdStampBuff, "+CNUM");
        }
        else
        {
            AT_PBK_Result_Err(ERR_AT_CME_EXE_FAIL, NULL, 0, "ListPbkEntry resp: wrong cmd stamp", CfwEvent.nUTI, nSim);
            return;
        }

        /* Just for pbk debug */
        OSI_LOGI(0x100047b0, "-----> ListPbkEntry resp: Entry counter = %u ", nEntryCount);

        // ////////////////////////////////////////////////
        // **         have Entry found
        // ////////////////////////////////////////////////
        while (nEntryCount-- > 0)
        {

            uint8_t InternationBuff[2] = {0};

            uint8_t LocolLangNumber[SIM_PBK_NUMBER_SIZE * 2 + 1] = {0}; /* include one char: '\0' */
            uint32_t nLocolLangNumLen = 0;

            uint8_t pLocolLangTextString[SIM_PBK_NAME_SIZE + 1] = {0}; /* include one char: '\0' */
            uint8_t pOutputTextString[SIM_PBK_NAME_SIZE + 1] = {0};    /* include one char: '\0' */

            /* clear buffer of prompt  */
            AT_MemZero(PromptBuff, sizeof(PromptBuff));

            /* check the input number sizeless than Max number size or not
               ** if more than the Max size, then discard part of them
             */
            if (2 * (pPbkEntryInfo->nNumberSize) > g_nMaxNumberSize)
            {
                pPbkEntryInfo->nNumberSize = g_nMaxNumberSize / 2;
            }

            /* check the input text length less than Max text size or not
               ** if more than the Max size, then discard part of them
             */
            if (pPbkEntryInfo->iFullNameSize > g_nMaxTextSize)
            {
                pPbkEntryInfo->iFullNameSize = g_nMaxTextSize;
            }

            // BCD to asscii
            nLocolLangNumLen = SUL_GsmBcdToAscii(pPbkEntryInfo->pNumber, pPbkEntryInfo->nNumberSize, LocolLangNumber);

            /* debug for pbk fullname from message node */
            OSI_LOGI(0x100047b1, "ListPbkEntry resp: FullName from message node => ");
            AT_TC_MEMBLOCK(g_sw_AT_PBK, pPbkEntryInfo->pFullName, pPbkEntryInfo->iFullNameSize, 16);

            /* process text string from csw  */
            if (!(AT_PBK_ProcessOutputName(pOutputTextString, sizeof(pOutputTextString), pPbkEntryInfo->pFullName, &(pPbkEntryInfo->iFullNameSize))))
            {
                OSI_LOGI(0x100047b2, "ListPbkEntry resp: some message node exception");

                /* get list next node, then continue */
                goto label_next_list_node;
            }

            /* buffer overflow */
            if (pPbkEntryInfo->iFullNameSize > sizeof(pLocolLangTextString))
            {
                OSI_LOGI(0x100047b3, "ListPbkEntry resp:name size of message node more than local buffer len");

                /* get list next node, then continue */
                goto label_next_list_node;
            }

            /* save the string to local language string buffer */
            AT_MemCpy(pLocolLangTextString, pOutputTextString, pPbkEntryInfo->iFullNameSize);

            /* debug for local text name */
            OSI_LOGI(0x100047b4, "ListPbkEntry resp: after tranfer local text string => ");
            AT_TC_MEMBLOCK(g_sw_AT_PBK, pOutputTextString, pPbkEntryInfo->iFullNameSize, 16);

            /* international or not */
            if (CFW_TELNUMBER_TYPE_INTERNATIONAL == pPbkEntryInfo->nType)
            {
                AT_StrCpy(InternationBuff, "+");
            }

            // ///////////////////////////////////////////////////////////
            /* Get the AT command type                  */
            // ///////////////////////////////////////////////////////////
            if (!AT_StrCmp(MarkCmdStampBuff, "+CPBR"))
            {
                sprintf(PromptBuff, "%s: %u,\"%s%s\",%u,\"%s\"", MarkCmdStampBuff, pPbkEntryInfo->phoneIndex,
                           InternationBuff, LocolLangNumber, pPbkEntryInfo->nType, pLocolLangTextString);
                AT_PBK_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, PromptBuff, AT_StrLen(PromptBuff), CfwEvent.nUTI, nSim);
            }
            else if (!AT_StrCmp(MarkCmdStampBuff, "+CPBF"))
            {

                uint8_t pTempbuff[SIM_PBK_NAME_SIZE + 1] = {0}; /* include one char: '\0' */

                /* buffer overflow */
                if (AT_StrLen(pLocolLangTextString) > sizeof(pTempbuff))
                {
                    OSI_LOGI(0x100047b5, "ListPbkEntry resp: local language text string is too large");

                    /* get list next node, then continue */
                    goto label_next_list_node;
                }

                AT_MemCpy(pTempbuff, pLocolLangTextString, AT_StrLen(pLocolLangTextString));

                /*
                        If the find text buffer is NULL, return all entry
                                if(NULL == g_arrFindText)
                                {
                                    bFindPBKEntry = TRUE;;
                                }
                */

                if ((!AT_StrCmp(pLocolLangTextString, g_arrFindText)) || (!AT_StrCmp(AT_StrUpr(pLocolLangTextString), AT_StrUpr(g_arrFindText))))
                {
                    bFindPBKEntry = TRUE;
                }

                /* have part of words matching at beginning */
                else if ((strstr(pLocolLangTextString, g_arrFindText) == (INT8 *)pLocolLangTextString) || (strstr(AT_StrUpr(pLocolLangTextString), AT_StrUpr(g_arrFindText)) ==
                                                                                                           (INT8 *)pLocolLangTextString))
                {
                    bFindPBKEntry = TRUE;
                }

                /* there are item of entry founded */
                if (bFindPBKEntry)
                {
                    sprintf(PromptBuff, "%s: %u,\"%s%s\",%u,\"%s\"", MarkCmdStampBuff, pPbkEntryInfo->phoneIndex,
                               InternationBuff, LocolLangNumber, pPbkEntryInfo->nType, pTempbuff);

                    AT_PBK_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, PromptBuff, AT_StrLen(PromptBuff), CfwEvent.nUTI, nSim);
                    bFindPBKEntry = FALSE;
                }
            }
            else if (!AT_StrCmp(MarkCmdStampBuff, "+CNUM"))
            {
                sprintf(PromptBuff, "%s: \"%s\",\"%s%s\",%u", MarkCmdStampBuff, pLocolLangTextString,
                           InternationBuff, LocolLangNumber, pPbkEntryInfo->nType);

                AT_PBK_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_OK, 0, PromptBuff, AT_StrLen(PromptBuff), CfwEvent.nUTI, nSim);
            }
            else
            {
                AT_PBK_Result_Err(ERR_AT_CME_EXE_FAIL, NULL, 0, "ListPbkEntry resp: CmdStampBuff error", CfwEvent.nUTI, nSim);
                return;
            }

            // ///////////////////////////////////
            // Get the next list node
            // ///////////////////////////////////
        label_next_list_node:
            pPbkEntryInfo =
                (CFW_SIM_PBK_ENTRY_INFO *)((uint32_t)pPbkEntryInfo + sizeof(CFW_SIM_PBK_ENTRY_INFO) + SIM_PBK_EXTR_SIZE);
        }

        /*
           **check multi list entry finished or not
         */
        if (1 == g_nListEntryLastStep)
        {
            AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, CfwEvent.nUTI, nSim);
            /* Reset the multi list param */
            g_nListEntryMaxIndex = 0;
            g_nListEntryMinIndex = 0;
            g_nListEntryLastStep = 0;
            g_nListEntryStepCounter = 0;

            /* Restore the storage before execute AT + CNUM */
            if (!AT_StrCmp(MarkCmdStampBuff, "+CNUM"))
            {
                gATCurrentnTempStorage = g_nBeforeCNUMStorage;
                g_nBeforeCNUMStorage = 0;
            }
        }
        else
        {
            /* Length is reach the end point and finished this time */
            if ((g_nListEntryMaxIndex - g_nListEntryMinIndex + 1) <= AT_PBK_LIST_ENTRY_STEP * (g_nListEntryStepCounter + 1))
            {
                g_nListEntryLastStep = 1;
                AT_PBK_ListPBKEntryRequest(g_nListEntryMinIndex + AT_PBK_LIST_ENTRY_STEP * g_nListEntryStepCounter,
                                           g_nListEntryMaxIndex, TRUE, CfwEvent.nUTI, nSim);
            }

            /* have some items to be continue to list  */
            else
            {
                AT_PBK_ListPBKEntryRequest(g_nListEntryMinIndex + AT_PBK_LIST_ENTRY_STEP * g_nListEntryStepCounter,
                                           g_nListEntryMinIndex + AT_PBK_LIST_ENTRY_STEP * (g_nListEntryStepCounter + 1) - 1,
                                           TRUE, CfwEvent.nUTI, nSim);
                g_nListEntryStepCounter++;
            }
            AT_PBK_Result_OK(CMD_FUNC_CONTINUE, CMD_RC_CR, 0, NULL, 0, CfwEvent.nUTI, nSim);
        }

        break;
    }
    case 0xF0:
        nErrorCode = AT_SetCmeErrorCode(CfwEvent.nParam1, FALSE);
        AT_PBK_Result_Err(nErrorCode, NULL, 0, "ListPbkEntry resp: CfwEvent.nType == 0xF0", CfwEvent.nUTI, nSim);
        break;

    default:
        AT_PBK_Result_Err(ERR_AT_CME_EXE_FAIL, NULL, 0, "ListPbkEntry resp: unknown CfwEvent.nType", CfwEvent.nUTI, nSim);
        break;
    }

    return;
}

/******************************************************************************************
Function            :   AT_PBK_AsyncEventProcess_DeletePbkEntry
Description     :
Called By           :   ATS moudle
Data Accessed   :
Data Updated    :
Input           :   CFW_EVENT CfwEvent
Output          :
Return          :   void
Others          :   build by others
                    modify and add comment by wangqunyang 2008.04.05
*******************************************************************************************/
void AT_PBK_AsyncEventProcess_DeletePbkEntry(CFW_EVENT CfwEvent)
{
    uint32_t nErrorCode = 0;
    uint8_t nSim = CfwEvent.nFlag;

    if (CfwEvent.nType == 0)
    {
        AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, CfwEvent.nUTI, nSim);
        return;
    }
    else if (CfwEvent.nType == 0xF0)
    {
        nErrorCode = AT_SetCmeErrorCode(CfwEvent.nParam1, FALSE);
        AT_PBK_Result_Err(nErrorCode, NULL, 0, "DeletePbkEntry resp: CfwEvent.nType == 0xF0", CfwEvent.nUTI, nSim);
        return;
    }
    else
    {
        AT_PBK_Result_Err(ERR_AT_CME_EXE_FAIL, NULL, 0, "DeletePbkEntry resp: wrong CfwEvent.nType", CfwEvent.nUTI, nSim);
        return;
    }
}

/******************************************************************************************
Function            :   AT_PBK_AsyncEventProcess_AddPbkEntry
Description     :
Called By           :   ATS moudle
Data Accessed       :
Data Updated    :
Input           :   CFW_EVENT CfwEvent
Output          :
Return          :   void
Others          :   build by others
                    modify and add comment by wangqunyang 2008.04.05
*******************************************************************************************/
void AT_PBK_AsyncEventProcess_AddPbkEntry(CFW_EVENT CfwEvent)
{
    uint32_t nErrorCode = 0;
    uint8_t nSim = CfwEvent.nFlag;

    if (CfwEvent.nType == 0)
    {
        AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, CfwEvent.nUTI, nSim);
        return;
    }
    else if (CfwEvent.nType == 0xF0)
    {
        nErrorCode = AT_SetCmeErrorCode(CfwEvent.nParam1, FALSE);
        AT_PBK_Result_Err(nErrorCode, NULL, 0, "AddPbkEntry resp: CfwEvent.nType == 0xF0", CfwEvent.nUTI, nSim);
        return;
    }
    else
    {
        AT_PBK_Result_Err(ERR_AT_CME_EXE_FAIL, NULL, 0, "AddPbkEntry resp: wrong CfwEvent.nType", CfwEvent.nUTI, nSim);
        return;
    }
}

/******************************************************************************************
Function        :   AT_PBK_AsyncEventProcess_InitSaveMaxSize
Description     :   when PBK init, send request for get PBK max number size and
                max text length, so this fuction process csw response event
                and save the value to gobal variable
Called By       :   ATS moudle
Data Accessed       :
Data Updated        :
Input           :   CFW_EVENT CfwEvent
Output          :
Return          :   void
Others          :   build by wangqunyang 2008.05.13
*******************************************************************************************/
void AT_PBK_AsyncEventProcess_InitSaveMaxSize(CFW_EVENT CfwEvent)
{
    uint32_t nErrorCode = ERR_SUCCESS;
    CFW_PBK_STRORAGE_INFO *pPbkStorageInfo = {0};
    uint8_t nSim = CfwEvent.nFlag;

    switch (CfwEvent.nType)
    {
    case 0:
        pPbkStorageInfo = (CFW_PBK_STRORAGE_INFO *)CfwEvent.nParam1;
        /* save the max number size and max text length
           ** whether the storage is "on", "fd","ld" or "sm", the
           ** size and text length are the same, but the max index
           ** are different */
        g_nMaxNumberSize = (pPbkStorageInfo->iNumberLen) * 2;
        g_nMaxTextSize = pPbkStorageInfo->txtLen;

        /* here, the "SM" pbk(default pbk), max index value */
        g_nMaxSMPBKIndex = pPbkStorageInfo->index;

        // AT_PBK_Result_OK(CMD_FUNC_SUCC, CMD_RC_CR, 0, NULL, 0, CfwEvent.nUTI);

        /****************************************************************/
        // if called AT_PBK_Result_OK() and the CMD_RC_CR is effective, the
        // cursor enter new line when AT module beginning, just kill timer here
        /****************************************************************/
        break;

    case 0xF0:

        nErrorCode = AT_SetCmeErrorCode(CfwEvent.nParam1, FALSE);
        AT_PBK_Result_Err(nErrorCode, NULL, 0, "InitSaveMaxSize: CfwEvent.nType == 0xF0", CfwEvent.nUTI, nSim);
        break;

    default:
        AT_PBK_Result_Err(ERR_AT_CME_EXE_FAIL, NULL, 0, "InitSaveMaxSize: wrong CfwEvent.nType", CfwEvent.nUTI, nSim);
        break;
    }

    return;
}

void AT_FDN_Init(uint8_t nSimID)
{
    SIM_PBK_PARAM *pG_Pbk_Param = NULL;

    CFW_CfgSimGetPbkParam(&pG_Pbk_Param, nSimID);
    OSI_LOGI(0x100047b6, "AT_FDN_Init start: ADNStatus:%d,FDNTotal:%d,RecordSize:%d!\n", pG_Pbk_Param->bADNStatus, pG_Pbk_Param->nFDNTotalNum, pG_Pbk_Param->nFDNRecordSize);
    if (pG_Pbk_Param->nFDNTotalNum > 0)
    {
        pgATFNDlist[nSimID] = AT_MALLOC(sizeof(AT_FDN_PBK_LIST) + sizeof(AT_FDN_PBK_RECORD) * (pG_Pbk_Param->nFDNTotalNum - 1));
        if (NULL == pgATFNDlist[nSimID])
        {
            OSI_LOGI(0x100047b7, "AT_FDN_Init ERROR:FDN List Malloc is fail !\n");
            return;
        }
        AT_MemSet(pgATFNDlist[nSimID], 0xff, sizeof(AT_FDN_PBK_LIST) + sizeof(AT_FDN_PBK_RECORD) * (pG_Pbk_Param->nFDNTotalNum - 1));

        pgATFNDlist[nSimID]->nFDNSatus = pG_Pbk_Param->bADNStatus ? EF_DISABLE : EF_ENABLE;
        pgATFNDlist[nSimID]->nCurrentRecordIndx = 0;
        pgATFNDlist[nSimID]->nTotalRecordNum = pG_Pbk_Param->nFDNTotalNum;
        pgATFNDlist[nSimID]->nRealRecordNum = 0;

        //get fist FDN record
        if (ERR_SUCCESS != CFW_SimGetPbkEntry(CFW_PBK_SIM_FIX_DIALLING, 1, 0, nSimID))
        {
            OSI_LOGI(0x100047b8, "AT_FDN_Init WARNING:Get FDN Record is fail !\n");
        }
    }
    else
    {
        OSI_LOGI(0x100047b9, "AT_FDN_Init WARNING:FDN Record is NULL !\n");
    }
    return;
}

void AT_PBK_AsyncEventProcess_GetFDNRecord(CFW_EVENT CfwEvent)
{
    CFW_SIM_PBK_ENTRY_INFO *pRecord = NULL;
    uint8_t nSim = CfwEvent.nFlag;

    if (pgATFNDlist[nSim]->nTotalRecordNum < 1)
    {
        OSI_LOGI(0x100047ba, "GetFDNRecord WARNING:FDN List is NULL !\n");
    }

    OSI_LOGI(0x100047bb, "GetFDNRecord start: nType:0x%x,nSim:%d!\n", CfwEvent.nType, nSim);

    if (CfwEvent.nType == 0)
    {
        if (pgATFNDlist[nSim]->nRealRecordNum >= pgATFNDlist[nSim]->nTotalRecordNum)
        {
            OSI_LOGI(0x100047bc, "GetFDNRecord ERROR:Get FDN Record is err !\n");
            return;
        }

        pRecord = (CFW_SIM_PBK_ENTRY_INFO *)CfwEvent.nParam1;
        if (pRecord != NULL)
        {
            AT_MemCpy(pgATFNDlist[nSim]->sRecord[pgATFNDlist[nSim]->nRealRecordNum].nName, pRecord->pFullName, pRecord->iFullNameSize);
            AT_MemCpy(pgATFNDlist[nSim]->sRecord[pgATFNDlist[nSim]->nRealRecordNum].nNumber, pRecord->pNumber, pRecord->nNumberSize);
            pgATFNDlist[nSim]->sRecord[pgATFNDlist[nSim]->nRealRecordNum].nIndex = (uint8_t)(CfwEvent.nParam2 & 0x000000ff);
            pgATFNDlist[nSim]->sRecord[pgATFNDlist[nSim]->nRealRecordNum].nTpye = pRecord->nType;
            pgATFNDlist[nSim]->nRealRecordNum++;
        }
        else
        {
            OSI_LOGI(0x100047bd, "GetFDNRecord WARNING:FDN Record is NULL !\n");
        }
        pgATFNDlist[nSim]->nCurrentRecordIndx++;

        //get next FDN record
        if (pgATFNDlist[nSim]->nCurrentRecordIndx < pgATFNDlist[nSim]->nTotalRecordNum)
        {
            if (ERR_SUCCESS != CFW_SimGetPbkEntry(CFW_PBK_SIM_FIX_DIALLING, pgATFNDlist[nSim]->nCurrentRecordIndx + 1, 0, nSim))
            {
                OSI_LOGI(0x100047be, "GetFDNRecord WARNING:Get FDN Record is fail !\n");
            }
        }
        else
        {
            OSI_LOGI(0x100047bf, "GetFDNRecord :Get FDN Record is end realRecord:%d!\n", pgATFNDlist[nSim]->nRealRecordNum);
        }
    }
    else if (CfwEvent.nType == 0xF0)
    {
        OSI_LOGI(0x100047c0, "GetFDNRecord ERROR:Get FDN Record is fail !\n");
    }
    else
    {
        OSI_LOGI(0x100047c1, "GetFDNRecord ERROR:Get FDN para is fail !\n");
    }

    return;
}

void AT_FDN_SetStatus(uint8_t nSim)
{
    SIM_PBK_PARAM *pG_Pbk_Param = NULL;
    uint32_t nRet = ERR_SUCCESS;
    nRet = CFW_CfgSimGetPbkParam(&pG_Pbk_Param, nSim);
    if (ERR_SUCCESS == nRet)
    {
        OSI_LOGI(0x100047c6, "pgATFNDlist[nSim] = %d", pgATFNDlist[nSim]);
        OSI_LOGI(0x100047c7, "pG_Pbk_Param = %d", pG_Pbk_Param);
        if ((pG_Pbk_Param == NULL) || (pgATFNDlist[nSim] == NULL))
            return;
        if (pG_Pbk_Param->nFDNTotalNum > 0)
        {
            pgATFNDlist[nSim]->nFDNSatus = pG_Pbk_Param->bADNStatus ? EF_DISABLE : EF_ENABLE;
            OSI_LOGI(0x100047c8, "SetStatus ADNStatus:%d ,FDNSatus:%d,nSim:%d!\n", pG_Pbk_Param->bADNStatus, pgATFNDlist[nSim]->nFDNSatus, nSim);
        }
        else
        {
            OSI_LOGI(0x100047c9, "SetStatus ERROR no FDN number!!!\n");
        }
    }
    else
    {
        OSI_LOGI(0x100047ca, "SetStatus ERROR ret=%d, nSim:%d\n", nRet, nSim);
    }
    return;
}
void AT_FDN_UpdateRecord(uint8_t nIdx, uint8_t nRecordSize, uint8_t *pData, uint8_t nSim)
{
    OSI_LOGI(0x100047cb, "UpdateRecord start nIdx:%d ,nRecordSize:%d,nSim:%d!\n", nIdx, nRecordSize, nSim);
    AT_FDN_UPDATERECORD *pUpdateFDN = (AT_FDN_UPDATERECORD *)AT_MALLOC(SIZEOF(AT_FDN_UPDATERECORD));
    if (pUpdateFDN == NULL)
    {
        OSI_LOGI(0x100047cc, "UpdateRecord ERROR:malloc UpdateFDN mem is fail !\n");
        return;
    }

    CFW_SIM_PBK_ENTRY_INFO *pGetPBKEntry = (CFW_SIM_PBK_ENTRY_INFO *)AT_MALLOC(SIZEOF(CFW_SIM_PBK_ENTRY_INFO) + SIM_PBK_EXTR_SIZE);
    if (pGetPBKEntry == NULL)
    {
        OSI_LOGI(0x100047cd, "UpdateRecord ERROR:malloc GetPBKEntry mem is fail !\n");
        AT_FREE(pUpdateFDN);
        return;
    }

    AT_MemSet(pUpdateFDN, 0xff, SIZEOF(AT_FDN_UPDATERECORD));
    pUpdateFDN->nCurrRecInd = nIdx;
    pUpdateFDN->nRecordSize = nRecordSize;

    //get FDN record form android
    pGetPBKEntry->pNumber = &pGetPBKEntry->iFullNameSize + (uint32_t)SIZEOF(CFW_SIM_PBK_ENTRY_INFO);
    pGetPBKEntry->pFullName = &pGetPBKEntry->iFullNameSize + (uint32_t)(SIZEOF(CFW_SIM_PBK_ENTRY_INFO) + SIM_PBK_NUMBER_SIZE);
    uint8_t txtlength = pUpdateFDN->nRecordSize - AT_SIM_REMAINLENGTH;
    cfw_SimParsePBKRecData(pData, pGetPBKEntry, pUpdateFDN, txtlength, API_AT_GETPBK);
    AT_TC_MEMBLOCK(g_sw_AT_PBK, (uint8_t *)pGetPBKEntry, (SIZEOF(CFW_SIM_PBK_ENTRY_INFO) + SIM_PBK_EXTR_SIZE), 16);
    if ((0 == pGetPBKEntry->iFullNameSize) && (0 == pGetPBKEntry->nNumberSize))
    {
        //del FDN record
        AT_FDN_DelRecord(nIdx, nSim);
    }
    else
    {
        //add or edit record
        AT_FDN_AddRecord(nIdx, pGetPBKEntry, nSim);
    }

    AT_FREE(pUpdateFDN);
    AT_FREE(pGetPBKEntry);
    return;
}
void AT_FDN_AddRecord(uint8_t nIdx, CFW_SIM_PBK_ENTRY_INFO *pGetPBKEntry, uint8_t nSim)
{
    uint8_t realnum = 0;
    uint8_t i = 0;

    OSI_LOGI(0x100047ce, "AddRecord start nIdx:%d ,nSim:%d!\n", nIdx, nSim);

    realnum = pgATFNDlist[nSim]->nRealRecordNum;
    for (i = 0; i < realnum; i++)
    {
        if (nIdx == pgATFNDlist[nSim]->sRecord[i].nIndex)
        {
            break;
        }
    }
    AT_MemCpy(pgATFNDlist[nSim]->sRecord[i].nName, pGetPBKEntry->pFullName, pGetPBKEntry->iFullNameSize);
    AT_MemCpy(pgATFNDlist[nSim]->sRecord[i].nNumber, pGetPBKEntry->pNumber, pGetPBKEntry->nNumberSize);
    pgATFNDlist[nSim]->sRecord[i].nTpye = pGetPBKEntry->nType;
    if (i == realnum)
    {
        pgATFNDlist[nSim]->nRealRecordNum++;
        pgATFNDlist[nSim]->sRecord[i].nIndex = nIdx;
    }

    //just for testing
    if (pgATFNDlist[nSim]->nRealRecordNum > pgATFNDlist[nSim]->nTotalRecordNum)
    {
        AT_ASSERT(0);
    }
    return;
}
void AT_FDN_DelRecord(uint8_t nIdx, uint8_t nSim)
{
    uint8_t realnum = 0;
    uint8_t nNext = 0;
    uint8_t i = 0;
    uint8_t j = 0;

    OSI_LOGI(0x100047cf, "DelRecord start nIdx:%d ,nSim:%d!\n", nIdx, nSim);
    //del FDN record
    realnum = pgATFNDlist[nSim]->nRealRecordNum;
    for (i = 0; i < realnum; i++)
    {
        if (nIdx == pgATFNDlist[nSim]->sRecord[i].nIndex)
        {
            break;
        }
    }
    if (i < realnum)
    {
        //del AT FDNlist Record
        pgATFNDlist[nSim]->nRealRecordNum--;
        for (j = i; j < realnum - 1; j++)
        {
            nNext = j + 1;
            AT_MemSet(&pgATFNDlist[nSim]->sRecord[j], 0xff, SIZEOF(AT_FDN_PBK_RECORD));
            pgATFNDlist[nSim]->sRecord[j].nIndex = pgATFNDlist[nSim]->sRecord[nNext].nIndex;
            pgATFNDlist[nSim]->sRecord[j].nTpye = pgATFNDlist[nSim]->sRecord[nNext].nTpye;
            AT_MemCpy(pgATFNDlist[nSim]->sRecord[j].nName, pgATFNDlist[nSim]->sRecord[nNext].nName, AT_FDN_NAME_SIZE);
            AT_MemCpy(pgATFNDlist[nSim]->sRecord[j].nNumber, pgATFNDlist[nSim]->sRecord[nNext].nNumber, AT_FDN_NUMBER_SIZE);
        }
        //write last record to zero
        AT_MemSet(&pgATFNDlist[nSim]->sRecord[realnum - 1], 0xff, SIZEOF(AT_FDN_PBK_RECORD));
    }
    return;
}
#endif
#endif //first #if 0
