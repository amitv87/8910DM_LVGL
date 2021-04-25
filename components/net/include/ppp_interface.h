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

#ifndef _PPP_INTERFACE_H_
#define _PPP_INTERFACE_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
#define GPRS_UNACTIVE 5 // The same as PPPERR_USER in ppp.h

typedef struct pppSession pppSession_t;

typedef int (*pppOutputCallback_t)(void *ctx, uint8_t *data, uint32_t size);

typedef void (*pppEndCallback_t)(void *ctx, uint8_t error);

//pppFlowControlCallback_t 0 - Not DLFolwControl; 1-DLFolwControl Soft; 2-DLFolwControl Hard
typedef int (*pppFlowControlCallback_t)(void *ctx);

#if PPP_AUTHGPRS_SUPPORT
//refer to SPCSS00727655, add ppp CHAP auth function
uint8_t getPppSessionActAttUTI(pppSession_t *ppp);
#endif
#ifdef CONFIG_QUEC_PROJECT_FEATURE_PPP
bool quec_ppp_close_in_dialing(pppSession_t *ppp);
#endif


pppSession_t *pppSessionCreate(uint8_t cid, uint8_t sim, osiThread_t *dl_thread,
                               pppOutputCallback_t output_cb, void *output_cb_ctx,
                               pppEndCallback_t end_cb, void *end_cb_ctx,
                               pppFlowControlCallback_t flowctrl_cb, void *flowctrl_cb_ctx,
                               int _iActiavedbyPPP);
bool pppSessionDelete(pppSession_t *ppp);
bool pppSessionClose(pppSession_t *ppp, uint8_t termflag);
int pppSessionSuspend(pppSession_t *ppp);
int pppSessionResume(pppSession_t *ppp);
int pppSessionInput(pppSession_t *ppp, const void *data, size_t size);
void pppSessionDeleteByNetifDestoryed(uint8_t nSimId, uint8_t nCid);
void pppSessionDeleteBySimCid(uint8_t nSimId, uint8_t nCid, uint8_t forceClose);
#ifdef __cplusplus
}
#endif
#endif
