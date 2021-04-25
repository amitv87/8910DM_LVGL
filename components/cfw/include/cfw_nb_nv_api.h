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

#ifndef _CFW_NB_NV_API_H_
#define _CFW_NB_NV_API_H_

#include <stdint.h>
#include <stdbool.h>
#include "cfw_config.h"

#ifdef __cplusplus
extern "C" {
#endif

int nvmWriteStatic(void);
uint8_t nbiot_nvGetVersionControl(void);
void nbiot_nvSetVersionControl(uint8_t version);
uint8_t nbiot_nvGetPdnAutoAttach(void);
void nbiot_nvSetPdnAutoAttach(uint8_t pdn_auto_attach);
uint8_t nbiot_nvGetDirectIpMode(void);
void nbiot_nvSetDirectIpMode(uint8_t value);
uint8_t nbiot_nvGetEdrxEnable(void);
void nbiot_nvSetEdrxEnable(uint8_t enable);
uint8_t nbiot_nvGetEdrxValue(void);
void nbiot_nvSetEdrxValue(uint8_t value);
uint8_t nbiot_nvGetEdrxPtw(void);
void nbiot_nvSetEdrxPtw(uint8_t ptw);
uint8_t nbiot_nvGetPsmEnable(void);
void nbiot_nvSetPsmEnable(uint8_t psmEnable);
uint8_t nbiot_nvGetPsmT3412(void);
void nbiot_nvSetPsmT3412(uint8_t t3412Val);
uint8_t nbiot_nvGetPsmT3324(void);
void nbiot_nvSetPsmT3324(uint8_t t3324Val);
uint8_t nbiot_nvGetCiotReport(void);
void nbiot_nvSetCiotReport(uint8_t ciotReport);
uint8_t nbiot_nvGetCiotNonip(void);
void nbiot_nvSetCiotNonip(uint8_t nonip);
uint8_t nbiot_nvGetCiotCpciot(void);
void nbiot_nvSetCiotCpciot(uint8_t cpciot);
uint8_t nbiot_nvGetCiotUpciot(void);
void nbiot_nvSetCiotUpciot(uint8_t upciot);
uint8_t nbiot_nvGetCiotErwopdn(void);
void nbiot_nvSetCiotErwopdn(uint8_t erwopdn);
uint8_t nbiot_nvGetCiotSmsWoCombAtt(void);
void nbiot_nvSetCiotSmsWoCombAtt(uint8_t sms_wo_comb_att);
uint8_t nbiot_nvGetCiotApnRateCtrl(void);
void nbiot_nvSetCiotApnRateCtrl(uint8_t apn_rate_control);
uint8_t nbiot_nvGetCiotEpco(void);
void nbiot_nvSetCiotEpco(uint8_t epco);
uint8_t nbiot_nvGetCpBackOffEnable(void);
void nbiot_nvSetCpBackOffEnable(uint8_t enable);
uint8_t nbiot_nvGetCiotRoam(void);
void nbiot_nvSetCiotRoam(uint8_t roam);
uint8_t nbiot_nvGetDefaultPdnType(void);
void nbiot_nvSetDefaultPdnType(uint8_t pdnType);
uint8_t *nbiot_nvGetDefaultApn(uint8_t i);
void nbiot_nvSetDefaultApn(uint8_t i, const char *apn, uint8_t apnLen);
uint8_t nbiot_nvGetHccpEnable(void);
void nbiot_nvSetHccpEnable(uint8_t enable);
uint16_t nbiot_nvGetHccpMaxcid(void);
void nbiot_nvSetHccpMaxcid(uint16_t maxCid);
uint8_t nbiot_nvGetHccpProfile(void);
void nbiot_nvSetHccpProfile(uint8_t profile);
uint8_t nbiot_nvGetAttachWithImsi(void);
void nbiot_nvSetAttachWithImsi(uint8_t attachWithImsi);
uint8_t nbiot_nvGetNasSigLowPri(void);
void nbiot_nvSetNasSigLowPri(uint8_t nasSigLowPri);
uint8_t nbiot_nvGetNasSigDualPri(void);
void nbiot_nvSetNasSigDualPri(uint8_t overrideNasSigLowPri);
uint8_t nbiot_nvGetExceptionDataReporting(void);
void nbiot_nvSetExceptionDataReporting(uint8_t exceptionDataReportingAllowed);
uint8_t nbiot_nvGetT3245Behave(void);
void nbiot_nvSetT3245Behave(uint8_t t3245Behaviour);
uint8_t nbiot_nvGetPsDataOff();
void nbiot_nvSetPsDataOff(uint8_t psdataOff);
uint16_t nbiot_nvGetExemptedServiceType();
void nbiot_nvSetExemptedServiceType(uint16_t exempted_service_type);
uint8_t nbiot_nvGetDcnIdEnable(void);
void nbiot_nvSetDcnIdEnable(uint8_t enable);
uint8_t nbiot_nvGetDefaultDcnIdValidFlag(void);
void nbiot_nvSetDefaultDcnIdValidFlag(uint8_t enable);
uint16_t nbiot_nvGetDcnIdValue(void);
void nbiot_nvSetDcnIdValue(uint16_t dcnid);
uint8_t nbiot_nvSetUserHplmn(uint8_t *mcc, uint8_t *mnc);
uint8_t nbiot_nvGetUserHplmn(uint8_t *mcc, uint8_t *mnc);
void nbiot_nvClearUserHplmn();
uint8_t nbiot_nvGetMultiUeOptFlag(void);
void nbiot_nvSetMultiUeOptFlag(uint8_t multiUeOptFlag);
void nbiot_nvSetDualmodeFlag(uint8_t dualmodeFlag);
uint8_t nbiot_nvGetDualmodeFlag();
void nbiot_nvSetDualmodeFastSwitch(uint8_t dualmodeFastSwitch);
uint8_t nbiot_nvGetDualmodeFastSwitch();
void nbiot_nvSetRatPriority(uint8_t ratPriority);
uint8_t nbiot_nvGetRatPriority();
void nbiot_nvSetNbGoodCellSignal(uint8_t NbGoodCellSignal);
uint8_t nbiot_nvGetNbGoodCellSignal();
void nbiot_nvSetGsmGoodCellSignal(uint8_t GsmGoodCellSignal);
uint8_t nbiot_nvGetGsmGoodCellSignal();
void nbiot_nvSetlossCovBackOffMaxCnt(uint8_t lossCovBackOffMaxCnt);
uint8_t nbiot_nvGetlossCovBackOffMaxCnt();
void nbiot_nvSetLossCovLen(uint32_t lossCovLen);
uint32_t nbiot_nvGetLossCovLen();
void nbiot_nvSetRatChangeLen(uint32_t ratChangeLen);
uint32_t nbiot_nvGetRatChangeLen();
void nbiot_nvSetActProcLen(uint32_t actProcLen);
uint32_t nbiot_nvGetActProcLen();
void nbiot_nvSetProhibitNbRecoverLen(uint32_t prohibitNbRecoverLen);
uint32_t nbiot_nvGetProhibitNbRecoverLen();
void nbiot_nvSetNasCellSelectLen(uint32_t nasCellSelectLen);
uint32_t nbiot_nvGetNasCellSelectLen();
void nbiot_nvSetRrcCellSelectLen(uint32_t rrcCellSelectLen);
uint32_t nbiot_nvGetRrcCellSelectLen();
void nbiot_nvSetDualLossCovLen(uint32_t dualLossCovLen);
uint32_t nbiot_nvGetDualLossCovLen();
void nbiot_nvSetLogRank(uint8_t logrank);
uint8_t nbiot_nvGetLogRank();
void nbiot_nvSetLogModule(uint32_t module);
uint32_t nbiot_nvGetLogModule();

#ifdef __cplusplus
}
#endif
#endif
