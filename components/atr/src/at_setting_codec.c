/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
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

#include "at_cfg.h"
#include "osi_compiler.h"
#include "pb_decode.h"
#include "pb_encode.h"
#include "pb_util.h"
#include "at_setting_codec.h"
#include "at_setting.pb.h"
#include "quec_proj_config.h"

bool atCfgTcpipDecode(at_tcpip_setting_t *p, const void *buf, size_t size)
{
    const pb_field_t *fields = PB_AtTcpipSetting_fields;
    PB_AtTcpipSetting pbdata;
    PB_AtTcpipSetting *pbs = &pbdata;

    PB_DEC_STRING(p->cipApn, cipApn);
    PB_DEC_STRING(p->cipUserName, cipUserName);
    PB_DEC_STRING(p->cipPassWord, cipPassWord);

    pb_istream_t is = pb_istream_from_buffer(buf, size);
    if (!pb_decode(&is, fields, pbs))
        return false;

    PB_OPT_DEC_ASSIGN(p->cipSCONT_unSaved, cipSCONT_unSaved);
    PB_OPT_DEC_ASSIGN(p->cipMux_multiIp, cipMux_multiIp);
    PB_OPT_DEC_ASSIGN(p->cipHEAD_addIpHead, cipHEAD_addIpHead);
    PB_OPT_DEC_ASSIGN(p->cipSHOWTP_dispTP, cipSHOWTP_dispTP);
    PB_OPT_DEC_ASSIGN(p->cipSRIP_showIPPort, cipSRIP_showIPPort);
    PB_OPT_DEC_ASSIGN(p->cipATS_time, cipATS_time);
    PB_OPT_DEC_ASSIGN(p->cipSPRT_sendPrompt, cipSPRT_sendPrompt);
    PB_OPT_DEC_ASSIGN(p->cipQSEND_quickSend, cipQSEND_quickSend);
    PB_OPT_DEC_ASSIGN(p->cipMODE_transParent, cipMODE_transParent);
    PB_OPT_DEC_ASSIGN(p->cipCCFG_NmRetry, cipCCFG_NmRetry);
    PB_OPT_DEC_ASSIGN(p->cipCCFG_WaitTm, cipCCFG_WaitTm);
    PB_OPT_DEC_ASSIGN(p->cipCCFG_SendSz, cipCCFG_SendSz);
    PB_OPT_DEC_ASSIGN(p->cipCCFG_esc, cipCCFG_esc);
    PB_OPT_DEC_ASSIGN(p->cipCCFG_Rxmode, cipCCFG_Rxmode);
    PB_OPT_DEC_ASSIGN(p->cipCCFG_RxSize, cipCCFG_RxSize);
    PB_OPT_DEC_ASSIGN(p->cipCCFG_Rxtimer, cipCCFG_Rxtimer);
    PB_OPT_DEC_ASSIGN(p->cipDPDP_detectPDP, cipDPDP_detectPDP);
    PB_OPT_DEC_ASSIGN(p->cipDPDP_interval, cipDPDP_interval);
    PB_OPT_DEC_ASSIGN(p->cipDPDP_timer, cipDPDP_timer);
    PB_OPT_DEC_ASSIGN(p->cipCSGP_setGprs, cipCSGP_setGprs);
    PB_OPT_DEC_ASSIGN(p->cipRDTIMER_rdsigtimer, cipRDTIMER_rdsigtimer);
    PB_OPT_DEC_ASSIGN(p->cipRDTIMER_rdmuxtimer, cipRDTIMER_rdmuxtimer);
    PB_OPT_DEC_ASSIGN(p->cipRXGET_manualy, cipRXGET_manualy);
    return true;
}

int atCfgTcpipEncode(const at_tcpip_setting_t *p, void *buf, size_t size)
{
    const pb_field_t *fields = PB_AtTcpipSetting_fields;
    PB_AtTcpipSetting pbdata = {};
    PB_AtTcpipSetting *pbs = &pbdata;

    PB_ENC_STRING(p->cipApn, cipApn);
    PB_ENC_STRING(p->cipUserName, cipUserName);
    PB_ENC_STRING(p->cipPassWord, cipPassWord);

    PB_OPT_ENC_ASSIGN(p->cipSCONT_unSaved, cipSCONT_unSaved);
    PB_OPT_ENC_ASSIGN(p->cipMux_multiIp, cipMux_multiIp);
    PB_OPT_ENC_ASSIGN(p->cipHEAD_addIpHead, cipHEAD_addIpHead);
    PB_OPT_ENC_ASSIGN(p->cipSHOWTP_dispTP, cipSHOWTP_dispTP);
    PB_OPT_ENC_ASSIGN(p->cipSRIP_showIPPort, cipSRIP_showIPPort);
    PB_OPT_ENC_ASSIGN(p->cipATS_time, cipATS_time);
    PB_OPT_ENC_ASSIGN(p->cipSPRT_sendPrompt, cipSPRT_sendPrompt);
    PB_OPT_ENC_ASSIGN(p->cipQSEND_quickSend, cipQSEND_quickSend);
    PB_OPT_ENC_ASSIGN(p->cipMODE_transParent, cipMODE_transParent);
    PB_OPT_ENC_ASSIGN(p->cipCCFG_NmRetry, cipCCFG_NmRetry);
    PB_OPT_ENC_ASSIGN(p->cipCCFG_WaitTm, cipCCFG_WaitTm);
    PB_OPT_ENC_ASSIGN(p->cipCCFG_SendSz, cipCCFG_SendSz);
    PB_OPT_ENC_ASSIGN(p->cipCCFG_esc, cipCCFG_esc);
    PB_OPT_ENC_ASSIGN(p->cipCCFG_Rxmode, cipCCFG_Rxmode);
    PB_OPT_ENC_ASSIGN(p->cipCCFG_RxSize, cipCCFG_RxSize);
    PB_OPT_ENC_ASSIGN(p->cipCCFG_Rxtimer, cipCCFG_Rxtimer);
    PB_OPT_ENC_ASSIGN(p->cipDPDP_detectPDP, cipDPDP_detectPDP);
    PB_OPT_ENC_ASSIGN(p->cipDPDP_interval, cipDPDP_interval);
    PB_OPT_ENC_ASSIGN(p->cipDPDP_timer, cipDPDP_timer);
    PB_OPT_ENC_ASSIGN(p->cipCSGP_setGprs, cipCSGP_setGprs);
    PB_OPT_ENC_ASSIGN(p->cipRDTIMER_rdsigtimer, cipRDTIMER_rdsigtimer);
    PB_OPT_ENC_ASSIGN(p->cipRDTIMER_rdmuxtimer, cipRDTIMER_rdmuxtimer);
    PB_OPT_ENC_ASSIGN(p->cipRXGET_manualy, cipRXGET_manualy);
    return pbEncodeToMem(fields, pbs, buf, size);
}

int atCfgGlobalEncode(const atSetting_t *p, void *buf, size_t size)
{
    const pb_field_t *fields = pbAtGlobalCfg_fields;
    pbAtGlobalCfg pbdata = {};
    pbAtGlobalCfg *pbs = &pbdata;

    PB_ENC_ASSIGN(p->profile, profile);
    return pbEncodeToMem(fields, pbs, buf, size);
}

bool atCfgGlobalDecode(atSetting_t *p, const void *buf, size_t size)
{
    const pb_field_t *fields = pbAtGlobalCfg_fields;
    pbAtGlobalCfg pbdata;
    pbAtGlobalCfg *pbs = &pbdata;

    pb_istream_t is = pb_istream_from_buffer(buf, size);
    if (!pb_decode(&is, fields, pbs))
        return false;

    PB_DEC_ASSIGN(p->profile, profile);
    return true;
}

static bool atCfgSimProfileEncode(pb_ostream_t *stream, const pbArrayEncodeParam_t *param, unsigned idx)
{
    const pb_field_t *fields = pbAtSimProfileCfg_fields;
    atSimSetting_t *p = &((atSimSetting_t *)param->data)[idx];
    pbAtSimProfileCfg pbdata = {};
    pbAtSimProfileCfg *pbs = &pbdata;

    PB_OPT_ENC_ASSIGN(p->creg, creg);
    PB_OPT_ENC_ASSIGN(p->cgreg, cgreg);
    PB_OPT_ENC_ASSIGN(p->cereg, cereg);
    PB_OPT_ENC_ASSIGN(p->cscon, cscon);
    PB_OPT_ENC_ASSIGN(p->cgerep_mode, cgerep_mode);
    PB_OPT_ENC_ASSIGN(p->cgerep_bfr, cgerep_bfr);
    PB_OPT_ENC_ASSIGN(p->clip, clip);
    PB_OPT_ENC_ASSIGN(p->clir, clir);
    PB_OPT_ENC_ASSIGN(p->colp, colp);
    PB_OPT_ENC_ASSIGN(p->cssu, cssu);
    PB_OPT_ENC_ASSIGN(p->cssi, cssi);
    PB_OPT_ENC_ASSIGN(p->ccwa, ccwa);
    PB_OPT_ENC_ASSIGN(p->cgauto, cgauto);
    PB_OPT_ENC_ASSIGN(p->cr, cr);
    PB_OPT_ENC_ASSIGN(p->s0, s0);
    PB_OPT_ENC_ASSIGN(p->ecsq, ecsq);
    PB_OPT_ENC_ASSIGN(p->csdh, csdh);
    PB_OPT_ENC_ASSIGN(p->cmgf, cmgf);
#ifdef CONFIG_QUEC_PROJECT_FEATURE_SMS
	PB_OPT_ENC_ASSIGN(p->cpms_mem1, cpms_mem1);
	PB_OPT_ENC_ASSIGN(p->cpms_mem2, cpms_mem2);
	PB_OPT_ENC_ASSIGN(p->cpms_mem3, cpms_mem3);
#endif
    PB_OPT_ENC_ASSIGN(p->cnmi_mode, cnmi_mode);
    PB_OPT_ENC_ASSIGN(p->cnmi_mt, cnmi_mt);
    PB_OPT_ENC_ASSIGN(p->cnmi_bm, cnmi_bm);
    PB_OPT_ENC_ASSIGN(p->cnmi_ds, cnmi_ds);
    PB_OPT_ENC_ASSIGN(p->cnmi_bfr, cnmi_bfr);
    PB_OPT_ENC_ASSIGN(p->volte, volte);
    PB_OPT_ENC_ASSIGN(p->flight_mode, flight_mode);
    PB_OPT_ENC_ASSIGN(p->cfgNvFlag, cfgNvFlag);
    return pb_encode_submessage(stream, fields, pbs);
}

static bool atCfgSimProfileDecode(pb_istream_t *stream, pbArrayDecodeParam_t *param)
{
    const pb_field_t *fields = pbAtSimProfileCfg_fields;
    atSimSetting_t *p = &((atSimSetting_t *)param->data)[param->idx];
    pbAtSimProfileCfg pbdata;
    pbAtSimProfileCfg *pbs = &pbdata;

    if (!pb_decode(stream, fields, pbs))
        return false;

    PB_OPT_DEC_ASSIGN(p->creg, creg);
    PB_OPT_DEC_ASSIGN(p->cgreg, cgreg);
    PB_OPT_DEC_ASSIGN(p->cereg, cereg);
    PB_OPT_DEC_ASSIGN(p->cscon, cscon);
    PB_OPT_DEC_ASSIGN(p->cgerep_mode, cgerep_mode);
    PB_OPT_DEC_ASSIGN(p->cgerep_bfr, cgerep_bfr);
    PB_OPT_DEC_ASSIGN(p->clip, clip);
    PB_OPT_DEC_ASSIGN(p->clir, clir);
    PB_OPT_DEC_ASSIGN(p->colp, colp);
    PB_OPT_DEC_ASSIGN(p->cssu, cssu);
    PB_OPT_DEC_ASSIGN(p->cssi, cssi);
    PB_OPT_DEC_ASSIGN(p->ccwa, ccwa);
    PB_OPT_DEC_ASSIGN(p->cgauto, cgauto);
    PB_OPT_DEC_ASSIGN(p->cr, cr);
    PB_OPT_DEC_ASSIGN(p->s0, s0);
    PB_OPT_DEC_ASSIGN(p->ecsq, ecsq);
    PB_OPT_DEC_ASSIGN(p->csdh, csdh);
    PB_OPT_DEC_ASSIGN(p->cmgf, cmgf);
#ifdef CONFIG_QUEC_PROJECT_FEATURE_SMS
	PB_OPT_DEC_ASSIGN(p->cpms_mem1, cpms_mem1);
	PB_OPT_DEC_ASSIGN(p->cpms_mem2, cpms_mem2);
	PB_OPT_DEC_ASSIGN(p->cpms_mem3, cpms_mem3);
#endif
    PB_OPT_DEC_ASSIGN(p->cnmi_mode, cnmi_mode);
    PB_OPT_DEC_ASSIGN(p->cnmi_mt, cnmi_mt);
    PB_OPT_DEC_ASSIGN(p->cnmi_bm, cnmi_bm);
    PB_OPT_DEC_ASSIGN(p->cnmi_ds, cnmi_ds);
    PB_OPT_DEC_ASSIGN(p->cnmi_bfr, cnmi_bfr);
    PB_OPT_DEC_ASSIGN(p->volte, volte);
    PB_OPT_DEC_ASSIGN(p->flight_mode, flight_mode);
    PB_OPT_DEC_ASSIGN(p->cfgNvFlag, cfgNvFlag);
    return true;
}

int atCfgProfileEncode(const atSetting_t *p, void *buf, size_t size)
{
    const pb_field_t *fields = pbAtProfileCfg_fields;
    pbAtProfileCfg pbdata = {};
    pbAtProfileCfg *pbs = &pbdata;

    PB_ENC_ARRAY(p->sim, sim, atCfgSimProfileEncode);

    PB_OPT_ENC_ASSIGN(p->chsetting.atv, atv);
    PB_OPT_ENC_ASSIGN(p->chsetting.ate, ate);
    PB_OPT_ENC_ASSIGN(p->chsetting.atq, atq);
    PB_OPT_ENC_ASSIGN(p->chsetting.crc, crc);
    PB_OPT_ENC_ASSIGN(p->chsetting.s3, s3);
    PB_OPT_ENC_ASSIGN(p->chsetting.s4, s4);
    PB_OPT_ENC_ASSIGN(p->chsetting.s5, s5);
    PB_OPT_ENC_ASSIGN(p->chsetting.cmee, cmee);
    PB_OPT_ENC_ASSIGN(p->chsetting.andc, andc);
    PB_OPT_ENC_ASSIGN(p->chsetting.andd, andd);

    PB_ENC_ASSIGN(p->ifc_rx, ifc_rx);
    PB_ENC_ASSIGN(p->ifc_tx, ifc_tx);
    PB_ENC_ASSIGN(p->icf_format, icf_format);
    PB_ENC_ASSIGN(p->icf_parity, icf_parity);
    PB_ENC_ASSIGN(p->ipr, ipr);

    PB_OPT_ENC_ASSIGN(p->cmer_ind, cmer_ind);
    PB_OPT_ENC_ASSIGN(p->csclk, csclk);
    PB_OPT_ENC_ASSIGN(p->cscs, cscs);
    PB_OPT_ENC_ASSIGN(p->vtd, vtd);
    PB_OPT_ENC_ASSIGN(p->ctzu, ctzu);
    PB_OPT_ENC_ASSIGN(p->ctzr, ctzr);
    PB_OPT_ENC_ASSIGN(p->csdf_mode, csdf_mode);
    PB_OPT_ENC_ASSIGN(p->csdf_auxmode, csdf_auxmode);
    PB_OPT_ENC_ASSIGN(p->gprsAuto, gprsAuto);
    PB_OPT_ENC_ASSIGN(p->tempStorage, tempStorage);
    PB_OPT_ENC_ASSIGN(p->drx_mode, drx_mode);
    PB_OPT_ENC_ASSIGN(p->self_register, self_register);
    PB_OPT_ENC_ASSIGN(p->psm_mode, psm_mode);
    PB_OPT_ENC_ASSIGN(p->mcuNotifySleepMode, mcuNotifySleepMode);
    PB_OPT_ENC_ASSIGN(p->mcuNotifySleepDelayMs, mcuNotifySleepDelayMs);
    PB_OPT_ENC_ASSIGN(p->csta, csta);
    PB_OPT_ENC_ASSIGN(p->csvm, csvm);
    PB_OPT_ENC_ASSIGN(p->detectMBS, detectMBS);
    PB_ENC_STRING(p->virtual_sim_card, virtual_sim_card);

    return pbEncodeToMem(fields, pbs, buf, size);
}

bool atCfgProfileDecode(atSetting_t *p, const void *buf, size_t size)
{
    const pb_field_t *fields = pbAtProfileCfg_fields;
    pbAtProfileCfg pbdata = {};
    pbAtProfileCfg *pbs = &pbdata;

    PB_DEC_ARRAY(p->sim, sim, atCfgSimProfileDecode);
    PB_DEC_STRING(p->virtual_sim_card, virtual_sim_card);

    pb_istream_t is = pb_istream_from_buffer(buf, size);
    if (!pb_decode(&is, fields, pbs))
        return false;

    PB_OPT_DEC_ASSIGN(p->chsetting.atv, atv);
    PB_OPT_DEC_ASSIGN(p->chsetting.ate, ate);
    PB_OPT_DEC_ASSIGN(p->chsetting.atq, atq);
    PB_OPT_DEC_ASSIGN(p->chsetting.crc, crc);
    PB_OPT_DEC_ASSIGN(p->chsetting.s3, s3);
    PB_OPT_DEC_ASSIGN(p->chsetting.s4, s4);
    PB_OPT_DEC_ASSIGN(p->chsetting.s5, s5);
    PB_OPT_DEC_ASSIGN(p->chsetting.cmee, cmee);
    PB_OPT_DEC_ASSIGN(p->chsetting.andc, andc);
    PB_OPT_DEC_ASSIGN(p->chsetting.andd, andd);
    PB_DEC_ASSIGN(p->ifc_rx, ifc_rx);
    PB_DEC_ASSIGN(p->ifc_tx, ifc_tx);
    PB_DEC_ASSIGN(p->icf_format, icf_format);
    PB_DEC_ASSIGN(p->icf_parity, icf_parity);
    PB_DEC_ASSIGN(p->ipr, ipr);
    PB_OPT_DEC_ASSIGN(p->cmer_ind, cmer_ind);
    PB_OPT_DEC_ASSIGN(p->csclk, csclk);
    PB_OPT_DEC_ASSIGN(p->cscs, cscs);
    PB_OPT_DEC_ASSIGN(p->vtd, vtd);
    PB_OPT_DEC_ASSIGN(p->ctzu, ctzu);
    PB_OPT_DEC_ASSIGN(p->ctzr, ctzr);
    PB_OPT_DEC_ASSIGN(p->csdf_mode, csdf_mode);
    PB_OPT_DEC_ASSIGN(p->csdf_auxmode, csdf_auxmode);
    PB_OPT_DEC_ASSIGN(p->gprsAuto, gprsAuto);
    PB_OPT_DEC_ASSIGN(p->tempStorage, tempStorage);
    PB_OPT_DEC_ASSIGN(p->drx_mode, drx_mode);
    PB_OPT_DEC_ASSIGN(p->self_register, self_register);
    PB_OPT_DEC_ASSIGN(p->psm_mode, psm_mode);
    PB_OPT_DEC_ASSIGN(p->mcuNotifySleepMode, mcuNotifySleepMode);
    PB_OPT_DEC_ASSIGN(p->mcuNotifySleepDelayMs, mcuNotifySleepDelayMs);
    PB_OPT_DEC_ASSIGN(p->csta, csta);
    PB_OPT_DEC_ASSIGN(p->csvm, csvm);
    PB_OPT_DEC_ASSIGN(p->detectMBS, detectMBS);

    return true;
}

OSI_UNUSED static bool atCfgCidInfoEncode(pb_ostream_t *stream, const pbArrayEncodeParam_t *param, unsigned idx)
{
    const pb_field_t *fields = pbAtCidInfoCfg_fields;
    AT_Gprs_CidInfo *p = &((AT_Gprs_CidInfo *)param->data)[idx];
    pbAtCidInfoCfg pbdata = {};
    pbAtCidInfoCfg *pbs = &pbdata;

    PB_ENC_STRING(p->uStateStr, uStateStr);
    PB_ENC_STRING(p->pApn, pApn);
    PB_ENC_STRING(p->pPdpAddr, pPdpAddr);
    PB_ENC_STRING(p->uaUsername, uaUsername);
    PB_ENC_STRING(p->uaPassword, uaPassword);
    PB_ENC_STRING(p->szOperatorName, szOperatorName);

    PB_OPT_ENC_ASSIGN(p->uCid, uCid);
    PB_OPT_ENC_ASSIGN(p->nDLCI, nDLCI);
    PB_OPT_ENC_ASSIGN(p->uState, uState);
    PB_OPT_ENC_ASSIGN(p->nPdpType, nPdpType);
    PB_OPT_ENC_ASSIGN(p->nDComp, nDComp);
    PB_OPT_ENC_ASSIGN(p->nHComp, nHComp);
    PB_OPT_ENC_ASSIGN(p->nApnSize, nApnSize);
    PB_OPT_ENC_ASSIGN(p->nAuthProt, nAuthProt);
    PB_OPT_ENC_ASSIGN(p->nPdpAddrSize, nPdpAddrSize);
    PB_OPT_ENC_ASSIGN(p->nUsernameSize, nUsernameSize);
    PB_OPT_ENC_ASSIGN(p->nPasswordSize, nPasswordSize);
#ifdef LTE_NBIOT_SUPPORT
    PB_OPT_ENC_ASSIGN(p->uPCid, uPCid);
    PB_OPT_ENC_ASSIGN(p->apnacAer, apnacAer);
    PB_OPT_ENC_ASSIGN(p->apnacUnit, apnacUnit);
    PB_OPT_ENC_ASSIGN(p->apnacRate, apnacRate);
#endif
    return pb_encode_submessage(stream, fields, pbs);
}

OSI_UNUSED static bool atCfgCidInfoDecode(pb_istream_t *stream, pbArrayDecodeParam_t *param)
{
    const pb_field_t *fields = pbAtCidInfoCfg_fields;
    AT_Gprs_CidInfo *p = &((AT_Gprs_CidInfo *)param->data)[param->idx];
    pbAtCidInfoCfg pbdata;
    pbAtCidInfoCfg *pbs = &pbdata;

    PB_DEC_STRING(p->uStateStr, uStateStr);
    PB_DEC_STRING(p->pApn, pApn);
    PB_DEC_STRING(p->pPdpAddr, pPdpAddr);
    PB_DEC_STRING(p->uaUsername, uaUsername);
    PB_DEC_STRING(p->uaPassword, uaPassword);
    PB_DEC_STRING(p->szOperatorName, szOperatorName);

    if (!pb_decode(stream, fields, pbs))
        return false;

    PB_OPT_DEC_ASSIGN(p->uCid, uCid);
    PB_OPT_DEC_ASSIGN(p->nDLCI, nDLCI);
    PB_OPT_DEC_ASSIGN(p->uState, uState);
    PB_OPT_DEC_ASSIGN(p->nPdpType, nPdpType);
    PB_OPT_DEC_ASSIGN(p->nDComp, nDComp);
    PB_OPT_DEC_ASSIGN(p->nHComp, nHComp);
    PB_OPT_DEC_ASSIGN(p->nApnSize, nApnSize);
    PB_OPT_DEC_ASSIGN(p->nAuthProt, nAuthProt);
    PB_OPT_DEC_ASSIGN(p->nPdpAddrSize, nPdpAddrSize);
    PB_OPT_DEC_ASSIGN(p->nUsernameSize, nUsernameSize);
    PB_OPT_DEC_ASSIGN(p->nPasswordSize, nPasswordSize);
#ifdef LTE_NBIOT_SUPPORT
    PB_OPT_DEC_ASSIGN(p->uPCid, uPCid);
    PB_OPT_DEC_ASSIGN(p->apnacAer, apnacAer);
    PB_OPT_DEC_ASSIGN(p->apnacUnit, apnacUnit);
    PB_OPT_DEC_ASSIGN(p->apnacRate, apnacRate);
#endif
    return true;
}

static bool atCfgSimAutoSaveEncode(pb_ostream_t *stream, const pbArrayEncodeParam_t *param, unsigned idx)
{
    const pb_field_t *fields = pbAtSimAutoSaveCfg_fields;
    atSimSetting_t *p = &((atSimSetting_t *)param->data)[idx];
    pbAtSimAutoSaveCfg pbdata = {};
    pbAtSimAutoSaveCfg *pbs = &pbdata;

    PB_ENC_STRING(p->cops_oper, cops_oper);
    PB_ENC_STRING(p->cpol_oper, cpol_oper);

    PB_OPT_ENC_ASSIGN(p->cops_format, cops_format);
    PB_OPT_ENC_ASSIGN(p->cops_mode, cops_mode);
    PB_OPT_ENC_ASSIGN(p->cpol_format, cpol_format);
    PB_OPT_ENC_ASSIGN(p->cpol_index, cpol_index);
    PB_OPT_ENC_ASSIGN(p->cops_act, cops_act);
    return pb_encode_submessage(stream, fields, pbs);
}

static bool atCfgSimAutoSaveDecode(pb_istream_t *stream, pbArrayDecodeParam_t *param)
{
    const pb_field_t *fields = pbAtSimAutoSaveCfg_fields;
    atSimSetting_t *p = &((atSimSetting_t *)param->data)[param->idx];
    pbAtSimAutoSaveCfg pbdata;
    pbAtSimAutoSaveCfg *pbs = &pbdata;

    PB_DEC_STRING(p->cops_oper, cops_oper);
    PB_DEC_STRING(p->cpol_oper, cpol_oper);

    if (!pb_decode(stream, fields, pbs))
        return false;

    PB_OPT_DEC_ASSIGN(p->cops_format, cops_format);
    PB_OPT_DEC_ASSIGN(p->cops_mode, cops_mode);
    PB_OPT_DEC_ASSIGN(p->cpol_format, cpol_format);
    PB_OPT_DEC_ASSIGN(p->cpol_index, cpol_index);
    PB_OPT_DEC_ASSIGN(p->cops_act, cops_act);
    return true;
}

int atCfgAutoSaveEncode(const atSetting_t *p, void *buf, size_t size)
{
    const pb_field_t *fields = pbAtAutoSaveCfg_fields;
    pbAtAutoSaveCfg pbdata = {};
    pbAtAutoSaveCfg *pbs = &pbdata;

    PB_ENC_ARRAY(p->sim, sim, atCfgSimAutoSaveEncode);
    PB_ENC_ARRAY2D(p->g_staAtGprsCidInfo, g_staAtGprsCidInfo, atCfgCidInfoEncode);
    PB_OPT_ENC_ASSIGN(p->timezone, timezone);
    PB_OPT_ENC_ASSIGN(p->timezone_dst, timezone_dst);
    PB_OPT_ENC_ASSIGN(p->bcchmode, bcchmode);
    PB_OPT_ENC_ASSIGN(p->callmode, callmode);
    PB_OPT_ENC_ASSIGN(p->smsmode, smsmode);
    PB_OPT_ENC_ASSIGN(p->poc_user_sim, poc_user_sim);
    PB_ENC_STRING(p->virtual_sim_card, virtual_sim_card);
    PB_ENC_STRING(p->CgClassType, CgClassType);
    return pbEncodeToMem(fields, pbs, buf, size);
}

bool atCfgAutoSaveDecode(atSetting_t *p, const void *buf, size_t size)
{
    const pb_field_t *fields = pbAtAutoSaveCfg_fields;
    pbAtAutoSaveCfg pbdata = {};
    pbAtAutoSaveCfg *pbs = &pbdata;

    PB_DEC_ARRAY(p->sim, sim, atCfgSimAutoSaveDecode);
    PB_DEC_ARRAY2D(p->g_staAtGprsCidInfo, g_staAtGprsCidInfo, atCfgCidInfoDecode);
    PB_DEC_STRING(p->virtual_sim_card, virtual_sim_card);
    PB_DEC_STRING(p->CgClassType, CgClassType);

    pb_istream_t is = pb_istream_from_buffer(buf, size);
    if (!pb_decode(&is, fields, pbs))
        return false;

    PB_OPT_DEC_ASSIGN(p->timezone, timezone);
    PB_OPT_DEC_ASSIGN(p->timezone_dst, timezone_dst);
    PB_OPT_DEC_ASSIGN(p->bcchmode, bcchmode);
    PB_OPT_DEC_ASSIGN(p->callmode, callmode);
    PB_OPT_DEC_ASSIGN(p->smsmode, smsmode);
    PB_OPT_DEC_ASSIGN(p->poc_user_sim, poc_user_sim);
    return true;
}
