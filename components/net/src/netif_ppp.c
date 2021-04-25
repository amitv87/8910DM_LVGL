#include "lwipopts.h"

#if 0
#include "at_cfw.h"
#include "sockets.h"
#include "netif/ppp/pppos.h"
#include "cfw.h"
#include "at_command.h"
#include "at_engine.h"
#include "at_response.h"
#include "cfw_errorcode.h"
#include "netif_ppp.h"
#include "osi_log.h"
#include "netmain.h"

#define AT_GPRS_DEA CFW_GPRS_DEACTIVED
#define AT_GPRS_ACT CFW_GPRS_ACTIVED

#define AT_GPRS_ATT CFW_GPRS_ATTACHED
#define AT_GPRS_DAT CFW_GPRS_DETACHED

#define AT_MUXSOCKET_NUM 8

#define ATTACH_TYPE (1 /*API_PS_ATTACH_GPRS*/ | 8 /*API_PS_ATTACH_FOR*/)

ctx_cb_t *g_ppp_ctx[AT_MUXSOCKET_NUM];

uint16_t Att_UTI;

uint16_t Act_UTI;

uint8_t IN_cid;
uint8_t IN_Sim;

uint8_t isPppActived;

extern atCfwCtx_t gAtCfwCtx;

atDataEngine_t *local_data_engine;

static int8_t do_active(uint8_t nDlc, uint8_t nSim, uint8_t nCid);
static void set_address(ctx_cb_t *ctx_p);
static void cfwActRsp_CB(void *ctx, const osiEvent_t *event);

void AT_CosEvent2CfwEvent(const osiEvent_t *pCosEvent, CFW_EVENT *pCfwEvent)
{
    *pCfwEvent = *(const CFW_EVENT *)pCosEvent;
}

static void pppDestory(void)
{

    osiEvent_t *ev = (osiEvent_t *)malloc(sizeof(osiEvent_t));
    ev->id = EV_PPP_CFW_GPRS_DEACT;
    ev->param1 = IN_cid;
    ev->param2 = IN_Sim;
    osiEventSend(netGetTaskID(), ev);
}

static void cfwAttRsp_CB(void *ctx, const osiEvent_t *event)
{

    if (isPppActived == 0)
    {
        return;
    }
    CFW_EVENT CfwEv;
    uint8_t nSim;
    AT_CosEvent2CfwEvent(event, &CfwEv);
    nSim = CfwEv.nFlag;
    //uint8_t nCid = event->param1;
    uint8_t nDlc = CfwEv.nUTI;

    //OSI_LOGI(0x1000165e, "attact_rsp nCid =%d", nCid);
    if (CfwEv.nEventId == EV_CFW_GPRS_ATT_RSP)
    {
        if (CFW_GPRS_ATTACHED == CfwEv.nType)
        {
            if (g_ppp_ctx[nDlc] == NULL)
            {
                return;
            }
            if (g_ppp_ctx[nDlc]->nCid == 0)
            {
                return;
            }
            g_ppp_ctx[nDlc]->gprsState = PPP_ACTIVING;
            if (do_active(Act_UTI, nSim, g_ppp_ctx[nDlc]->nCid) == ERR_SUCCESS)
                cfwUtiCBDispatchRegister(gAtCfwCtx.resp_man, Act_UTI, cfwActRsp_CB, g_ppp_ctx[nDlc]);
        }
    }
}
static void cfwActRsp_CB(void *ctx, const osiEvent_t *event)
{
    if (isPppActived == 0)
    {
        return;
    }

    OSI_LOGI(0x10007581, "cfwActRsp_CB");
    CFW_EVENT CfwEv;
    uint8_t nSim;
    AT_CosEvent2CfwEvent(event, &CfwEv);
    nSim = CfwEv.nFlag;
    uint8_t nCid = event->param1;
    uint8_t nDlc = CfwEv.nUTI;

    if (g_ppp_ctx[nDlc] == NULL)
    {
        return;
    }
    if (g_ppp_ctx[nDlc]->nCid == 0)
    {
        return;
    }

    switch (CfwEv.nEventId)
    {
    case EV_CFW_GPRS_CXT_ACTIVE_IND:
        //OSI_LOGI(0x10001660, "ppp_attact_rsp got EV_CFW_GPRS_CXT_ACTIVE_IND");
        break;

    case EV_CFW_GPRS_CXT_DEACTIVE_IND:
        // OSI_LOGI(0x10001661, "ppp_attact_rsp got EV_CFW_GPRS_CXT_DEACTIVE_IND");
        if (g_ppp_ctx[nDlc] != NULL)
        {
            g_ppp_ctx[nDlc]->gprsState = PPP_DETACHED;
            ppp_close(g_ppp_ctx[nDlc]->nPppPcb, 0); //FIXME  if pdp_rej received before lcp config req comes from peer
        }
        break;

    case EV_CFW_GPRS_ACT_RSP:
    {
        OSI_LOGI(0x10001662, "ppp_attact_rsp EV_CFW_GPRS_ACT_RSP nType: %d", CfwEv.nType);

        if (CFW_GPRS_ACTIVED == CfwEv.nType)
        {
            if (g_ppp_ctx[nDlc] != NULL)
            {
                OSI_LOGI(0x10007582, "set addr: nDlc = %d", nDlc);
                g_ppp_ctx[nDlc]->gprsState = PPP_ACTIVED;
                set_address(g_ppp_ctx[nDlc]);
            }
            else
            {
                //Already fail, Treminating
                OSI_LOGI(0x10007583, "CFW_GPRS_DEACTIVED");
                CFW_GprsAct(CFW_GPRS_DEACTIVED, nCid, Act_UTI, nSim);
                cfwUtiCBDispatchRegister(gAtCfwCtx.resp_man, Act_UTI, cfwActRsp_CB, g_ppp_ctx[nDlc]);
            }

            break;
        }
        else
        {
            if (g_ppp_ctx[nDlc] != NULL)
            {
                g_ppp_ctx[nDlc]->gprsState = PPP_DETACHED;
                ppp_close(g_ppp_ctx[nDlc]->nPppPcb, 0);
            }
        }
    }
    break;
    default:
        OSI_LOGI(0x10001663, "ppp_attact_rsp unprocessed event:%d", event->id);
        break;
    }
}

//static void ppp_attact_rsp(osiEvent_t *pEvent);
extern struct netif *getGprsNetIf(uint8_t nSim, uint8_t nCid);

static u32_t output_cb(ppp_pcb *pcb, u8_t *data, u32_t len, void *ctx)
{
    OSI_LOGI(0x10007584, "output_cb enter\n");
    //return uart_write(UART, data, len);
    ctx_cb_t *ctx_p = (ctx_cb_t *)ctx;
    sys_arch_dump(data, len);
    OSI_LOGI(0x10007585, "output_cb lcp_fsm:%d, lpcp_fsm:%d, gprs status:%d\n", pcb->lcp_fsm.state, pcb->ipcp_fsm.state, ctx_p->gprsState);
    atDataWrite(local_data_engine, data, len);
    //sys_arch_printf("output_cb uart_SendData ret =%d\n", len);
    return len;
}

void AT_PppTerminateInd(atCmdEngine_t *engine)
{
    OSI_LOGI(0x10007586, "AT_PppTerminateInd enter\n");
    struct atDispatch *d = atCmdGetDispatch(engine);
    //sys_arch_printf("AT_PppTerminateInd nDLCI = %d,isPppActived=%d", PARA->engine->channel_id, isPppActived);
    isPppActived = 0;
    atEngineModeSwitch(AT_MODE_SWITCH_DATA_END, d);
}

void CFW_PppTermInd(atCmdEngine_t *engine)
{
    OSI_LOGI(0x10007587, "CFW_PppTermInd enter\n");
    AT_PppTerminateInd(engine);
}

static void linkstate_cb(ppp_pcb *pcb, int err_code, void *ctx)
{
    OSI_LOGI(0x10007588, "linkstate_cb enter\n");
    ctx_cb_t *ctx_p = (ctx_cb_t *)ctx;
    //sys_arch_printf("linkstate_cb pcb=0x%x err_code=%d nDlc=%d\n", pcb, err_code, ctx_p->nDlc);
    if (err_code == 0)
    {
    }
    else /* if (err_code == PPPERR_CONNECT) */
    {
        isPppActived = 0;
        CFW_PppTermInd(ctx_p->engine);
        pppDestory();
        ppp_free(pcb);
        free(g_ppp_ctx[ctx_p->nDlc]);
        g_ppp_ctx[ctx_p->nDlc] = NULL;
    }
}

static void set_address(ctx_cb_t *ctx_p)
{
    OSI_LOGI(0x10007589, "set_address enter\n");
    struct netif *g_gprsIf = NULL;
    g_gprsIf = getGprsNetIf(ctx_p->nSimId, ctx_p->nCid);
    //sys_arch_printf("set_address g_gprsIf=0x%x\n", g_gprsIf);
    if (g_gprsIf != NULL)
    {
        ip4_addr_t addr;
        /* Set our address */
        IP4_ADDR(&addr, 192, 168, 0, 1);
        ppp_set_ipcp_ouraddr(ctx_p->nPppPcb, &addr);
        ctx_p->nPppPcb->ipcp_wantoptions.accept_local = 1;

        /* Set peer(his) address */
        //IP4_ADDR(&addr, 192,168,0,2);
        //IP4_ADDR(&addr, 0xC0,0xC8,0x01,0x15);
        ppp_set_ipcp_hisaddr(ctx_p->nPppPcb, netif_ip4_addr(g_gprsIf));

        /* Set primary DNS server */
        //IP4_ADDR(&addr, 192,168,10,20);
        ppp_set_ipcp_dnsaddr(ctx_p->nPppPcb, 0, ip_2_ip4(dns_getserver(0)));

        /* Set secondary DNS server */
        //IP4_ADDR(&addr, 192,168,10,21);
        ppp_set_ipcp_dnsaddr(ctx_p->nPppPcb, 1, ip_2_ip4(dns_getserver(1)));
        ctx_p->gprs_netif = g_gprsIf;
    }
}

static void ppp_notify_phase_cb(ppp_pcb *pcb, u8_t phase, void *ctx)
{
    OSI_LOGI(0x1000758a, "ppp_notify_phase_cb enter\n");
    ctx_cb_t *ctx_p = (ctx_cb_t *)ctx;
    //OSI_LOGI(0, "ppp_notify_phase_cb pcb=0x%x phase=%d,gprs status:%d", pcb, phase, ctx_p->gprsState);
    OSI_LOGI(0x1000758b, "status lcp_fsm:%d, lpcp_fsm:%d\n", pcb->lcp_fsm.state, pcb->ipcp_fsm.state);
#if PPP_IPV6_SUPPORT
    OSI_LOGI(0x1000758c, "status lcp_fsm:%d, ipv6cp_fsm:%d\n", pcb->lcp_fsm.state, pcb->ipv6cp_fsm.state);
#endif
    switch (phase)
    {

    case PPP_PHASE_NETWORK:
        if (pcb->lcp_fsm.state == PPP_FSM_STOPPING)
        {
            //Terminating
            CFW_GprsAct(CFW_GPRS_DEACTIVED, ctx_p->nCid, Act_UTI, ctx_p->nSimId);
            cfwUtiCBDispatchRegister(gAtCfwCtx.resp_man, Act_UTI, cfwActRsp_CB, ctx_p);
        }
        else if (pcb->lcp_fsm.state == PPP_FSM_OPENED)
        {
            if (pcb->ipcp_fsm.state == PPP_FSM_CLOSED
#if PPP_IPV6_SUPPORT
                || pcb->ipv6cp_fsm.state == PPP_FSM_CLOSED
#endif
            )
            {
                u8_t retryCount = 0;
                struct netif *g_gprsIf = getGprsNetIf(ctx_p->nSimId, ctx_p->nCid);
                while (ctx_p->gprsState > PPP_DETACHED && g_gprsIf == NULL && retryCount++ <= 20)
                {
                    OSI_LOGI(0x1000758d, "ppp_notify_phase_cb waiting for gprs actived:%d\n", retryCount);
                    osiThreadSleep(500);
                    g_gprsIf = getGprsNetIf(ctx_p->nSimId, ctx_p->nCid);
                }
                //sys_arch_printf("ppp_notify_phase_cb g_gprsIf=0x%x\n", g_gprsIf);
                if (g_gprsIf != NULL)
                {
                    OSI_LOGI(0x1000758e, "ppp_notify_phase_cb set_address\n");
                    set_address(ctx_p);
                }
            }
        }
        break;
    /* Session is down (either permanently or briefly) */
    case PPP_PHASE_DEAD:
        //d_set(PPP_LED, LED_OFF);
        break;

    /* We are between two sessions */
    case PPP_PHASE_HOLDOFF:
        //led_set(PPP_LED, LED_SLOW_BLINK);
        break;

    /* Session just started */
    case PPP_PHASE_INITIALIZE:
        //led_set(PPP_LED, LED_FAST_BLINK);
        break;

    /* Session is running */
    case PPP_PHASE_RUNNING:
        //led_set(PPP_LED, LED_ON);
        break;

    default:
        break;
    }
}

static int8_t do_active(uint8_t nUTI, uint8_t nSim, uint8_t nCid)
{
    OSI_LOGI(0x1000758f, "do_active enter\n");
    return CFW_GprsAct(CFW_GPRS_ACTIVED, nCid, nUTI, nSim);
}
#if 0
static void ppp_attact_rsp(osiEvent_t *pEvent)
{
    //OSI_LOGXI(OSI_LOGPAR(S, I, I, I, I), 0x1000165d, "ppp_attact_rsp Got %s: %d,0x%x,0x%x,0x%x",
          //TS_GetEventName(pEvent->nEventId), pEvent->nEventId, pEvent->nParam1, pEvent->nParam2, pEvent->nParam3);

    CFW_EVENT CfwEv;
    uint8_t nSim;
    AT_CosEvent2CfwEvent(pEvent, &CfwEv);
    nSim = CfwEv.nFlag;
    uint8_t nCid = pEvent->param1;
    uint8_t nDlc = CfwEv.nUTI;

    //OSI_LOGI(0x1000165e, "attact_rsp nCid =%d", nCid);
    if (CfwEv.nEventId == EV_CFW_GPRS_ATT_RSP)
    {
        if (CFW_GPRS_ATTACHED == CfwEv.nType)
        {
            g_ppp_ctx[nDlc]->gprsState = PPP_ACTIVING;
            if (do_active(CfwEv.nUTI, nSim, g_ppp_ctx[nDlc]->nCid) != ERR_SUCCESS)
            {
                //OSI_LOGI(0x1000165f, "ppp_attact_rsp Activate PDP error while ppp");
            }
        }
    }
    else
    {
        switch (CfwEv.nEventId)
        {
        case EV_CFW_GPRS_CXT_ACTIVE_IND:
            //OSI_LOGI(0x10001660, "ppp_attact_rsp got EV_CFW_GPRS_CXT_ACTIVE_IND");
            break;

        case EV_CFW_GPRS_CXT_DEACTIVE_IND:
           // OSI_LOGI(0x10001661, "ppp_attact_rsp got EV_CFW_GPRS_CXT_DEACTIVE_IND");
            if (g_ppp_ctx[nDlc] != NULL)
            {
                g_ppp_ctx[nDlc]->gprsState = PPP_DETACHED;
                ppp_close(g_ppp_ctx[nDlc]->nPppPcb,0);  //FIXME  if pdp_rej received before lcp config req comes from peer
            }
            break;

        case EV_CFW_GPRS_ACT_RSP:
        {
            OSI_LOGI(0x10001662, "ppp_attact_rsp EV_CFW_GPRS_ACT_RSP nType: %d", CfwEv.nType);
            if (CFW_GPRS_ACTIVED == CfwEv.nType)
            {
                if (g_ppp_ctx[nDlc] != NULL)
                {
                    g_ppp_ctx[nDlc]->gprsState = PPP_ACTIVED;
                    set_address(g_ppp_ctx[nDlc]);
                }
                else
                    //Already fail, Treminating
                    CFW_GprsAct(CFW_GPRS_DEACTIVED ,nCid,Act_UTI,nSim);
                    cfwUtiCBDispatchRegister(gAtCfwCtx->resp_man,Act_UTI, cfwActRsp_CB, g_ppp_ctx[nDlc]);
                break;
            }
            else
            {
                if (g_ppp_ctx[nDlc] != NULL)
                {
                    g_ppp_ctx[nDlc]->gprsState = PPP_DETACHED;
                    ppp_close(g_ppp_ctx[nDlc]->nPppPcb,0);
                }
            }
        }
        break;
        default:
            OSI_LOGI(0x10001663, "ppp_attact_rsp unprocessed event:%d", pEvent->id);
            break;
        }
    }
    free(pEvent);
}
#endif
static int ppp_GprsActive(uint16_t nDlc, uint8_t nSimID, uint8_t nCid)
{
    OSI_LOGI(0x10007590, "ppp_GprsActive enter\n");
    int iResult = 0;
    //uint8_t nSim = AT_SIM_ID(nDlc);
    uint8_t nSim = nSimID;
    uint8_t m_uAttState = CFW_GPRS_DETACHED;
    uint8_t uPDPState = 0;
    OSI_LOGI(0x10001664, "ppp_GprsActive cid:%d nSimID:%d\n", nCid, nSimID);

    if (CFW_GetGprsAttState(&m_uAttState, nSim) != ERR_SUCCESS)
    {
        OSI_LOGI(0x10001665, "ppp_GprsActive, get GPRS attach state error\n");
        return -1;
    }
    if (AT_GPRS_DAT == m_uAttState)
    {
        g_ppp_ctx[nDlc]->gprsState = PPP_ATTCHING;
        //ATTACH AND ACTIVE
        iResult = CFW_GprsAtt(AT_GPRS_ATT, Att_UTI, nSim);
        if (iResult != ERR_SUCCESS)
        {
            OSI_LOGI(0x10001666, "ppp_GprsActive, attach failed: %d\n", iResult);
            return -1;
        }
        if (cfwUtiCBDispatchRegister(gAtCfwCtx.resp_man, Att_UTI, cfwAttRsp_CB, g_ppp_ctx[nDlc]) == false)
        {
            return -1;
        }

        return 0;
    }
    else
    {
        g_ppp_ctx[nDlc]->gprsState = PPP_ATTACHED;
        if (nCid != 0 && CFW_GetGprsActState(nCid, &uPDPState, nSim) != ERR_SUCCESS)
        {
            OSI_LOGI(0x10001667, "ppp_GprsActive, get GPRS act state error\n");
            return -1;
        }
        if (uPDPState == CFW_GPRS_ACTIVED)
        {
            g_ppp_ctx[nDlc]->gprsState = PPP_ACTIVED;
            return 1;
        }
        else
        {
            g_ppp_ctx[nDlc]->gprsState = PPP_ACTIVING;
            if (do_active(Act_UTI, nSim, nCid) != ERR_SUCCESS)
            {
                OSI_LOGI(0x10001668, "ppp_GprsActive, activate failed: %d\n", iResult);
                return -1;
            }
            cfwUtiCBDispatchRegister(gAtCfwCtx.resp_man, Act_UTI, cfwActRsp_CB, g_ppp_ctx[nDlc]);
        }
        return 0;
    }
}

void ppp_StartGprs(u8_t *usrname, u8_t *usrpwd, void *ctx)
{
    OSI_LOGI(0x10007591, "ppp_StartGprs enter\n");
    ctx_cb_t *ctx_p = (ctx_cb_t *)ctx;
    u16_t nDlc = ctx_p->nDlc;
    u8_t nSimId = ctx_p->nSimId;
    u8_t nCid = ctx_p->nCid;
    //OSI_LOGXI(OSI_LOGPAR_ISS, 0x10001669, "ppp_StartGprs cid:%d usrname:%s,usrpwd:%s\n",nCid,usrname,usrpwd);
    OSI_LOGI(0x1000166a, "ppp_StartGprs g_ppp_ctx[%d]->gprsState=%d\n", nDlc, g_ppp_ctx[nDlc]->gprsState);
    if (g_ppp_ctx[nDlc]->gprsState == PPP_ACTIVED)
        set_address(g_ppp_ctx[nDlc]);
    else
    {
        if (nCid == 0)
        {
            if (CFW_GetFreeCID(&nCid, nSimId) != ERR_SUCCESS)
            {
                OSI_LOGI(0x1000166b, "ppp_StartGprs, no cid allowed for TCPIP\n");
                goto ret;
            }
            ctx_p->nCid = nCid;
        }
        if (usrname != NULL && usrpwd != NULL)
        {
            CFW_GPRS_PDPCONT_INFO_V2 sPdpCont;
            memset(&sPdpCont, 0, sizeof(sPdpCont));
            if (ERR_SUCCESS != CFW_GprsSetPdpCxtV2(nCid, &sPdpCont, nSimId))
            {
                OSI_LOGI(0x1000166c, "ppp_StartGprs(), CFW_GprsGetPdpCxt error");
#ifdef LTE_NBIOT_SUPPORT
                strncpy((char *)sPdpCont.pApn, "CTNB", strlen("CTNB") + 1);
#else
                strncpy((char *)sPdpCont.pApn, "CMWAP", strlen("CMWAP") + 1);
#endif
                sPdpCont.nApnSize = strlen((char *)sPdpCont.pApn);
                sPdpCont.nPdpType = CFW_GPRS_PDP_TYPE_IP;
#ifdef LTE_NBIOT_SUPPORT
                sPdpCont.PdnType = 1;
#endif
            }

            sPdpCont.nApnUserSize = strlen((char *)usrname);
            strncpy((char *)sPdpCont.pApnUser, (char *)usrname, sPdpCont.nApnUserSize + 1);
            sPdpCont.nApnPwdSize = strlen((char *)usrpwd);
            strncpy((char *)sPdpCont.pApnPwd, (char *)usrpwd, sPdpCont.nApnPwdSize + 1);
            if (CFW_GprsSetPdpCxtV2(nCid, &sPdpCont, nSimId) != ERR_SUCCESS)
            {
                OSI_LOGI(0x1000166d, "ppp_StartGprs(), CFW_GprsSetPdpCxt error");
                goto ret;
            }
        }
        if (ppp_GprsActive(nDlc, nSimId, nCid) > 0)
        {
            set_address(g_ppp_ctx[nDlc]);
        }
    }
ret:
    free(usrname);
    free(usrpwd);
}

int startPppIf(atCommand_t *PARA, u8_t nSimId, u8_t nCid)
{
    OSI_LOGI(0x10007592, "startPppIf enter\n");
    struct netif *pppif;
    atDispatch_t *d = atCmdGetDispatch(PARA->engine);
    u8_t nDlc = atCmdGetChannel_id(d);
    OSI_LOGI(0x10007593, "startPppIf nDlc = %d\n", nDlc);
    Att_UTI = cfwRequestUTI();
    Act_UTI = cfwRequestUTI();

    if (nDlc >= AT_MUXSOCKET_NUM) //if devno > 0 || mux_id >= AT_MUXSOCKET_NUM, will return -1( to be further FIX)
        return -1;
    if (g_ppp_ctx[nDlc] == NULL)
    {
        g_ppp_ctx[nDlc] = malloc(sizeof(ctx_cb_t));
        memset(g_ppp_ctx[nDlc], 0, sizeof(ctx_cb_t));
    }
    g_ppp_ctx[nDlc]->nDlc = nDlc;
    g_ppp_ctx[nDlc]->engine = PARA->engine;
    g_ppp_ctx[nDlc]->nSimId = nSimId;
    g_ppp_ctx[nDlc]->nCid = nCid;
    g_ppp_ctx[nDlc]->gprsState = PPP_ACTIVED;
    pppif = getGprsNetIf(nSimId, nCid);
    if (pppif == NULL)
    {
        return -1;
    }
    g_ppp_ctx[nDlc]->nPppPcb = pppos_create(pppif, output_cb, linkstate_cb, (void *)g_ppp_ctx[nDlc]);
    if (g_ppp_ctx[nDlc]->nPppPcb == NULL)
    {
        free(g_ppp_ctx[nDlc]);
        free(pppif);
        g_ppp_ctx[nDlc] = NULL;
        pppif = NULL;
        isPppActived = 0;
        atEngineModeSwitch(AT_MODE_SWITCH_DATA_END, d);
        return -1;
    }
    pppif->state = g_ppp_ctx[nDlc]->nPppPcb;
    ppp_set_notify_phase_callback(g_ppp_ctx[nDlc]->nPppPcb, ppp_notify_phase_cb);

#if PAP_FOR_SIM_AUTH && PPP_AUTH_SUPPORT
    /* Auth configuration, this is pretty self-explanatory */
    ppp_set_auth(g_ppp_ctx[nDlc]->nPppPcb, PPPAUTHTYPE_ANY, "login", "password");

    /* Require peer to authenticate */
    ppp_set_auth_required(g_ppp_ctx[nDlc]->nPppPcb, 1);
#endif

    /*
     * Only for PPPoS, the PPP session should be up and waiting for input.
     *
     * Note: for PPPoS, ppp_connect() and ppp_listen() are actually the same thing.
     * The listen call is meant for future support of PPPoE and PPPoL2TP server
     * mode, where we will need to negotiate the incoming PPPoE session or L2TP
     * session before initiating PPP itself. We need this call because there is
     * two passive modes for PPPoS, ppp_set_passive and ppp_set_silent.
     */
    ppp_set_silent(g_ppp_ctx[nDlc]->nPppPcb, 1);

    /*
     * Initiate PPP listener (i.e. wait for an incoming connection), can only
     * be called if PPP session is in the dead state (i.e. disconnected).
     */
    ppp_listen(g_ppp_ctx[nDlc]->nPppPcb);
    return 0;
}

struct netif *getPppNetIf(uint8_t nDlc)
{
    OSI_LOGI(0x10007594, "getPppNetIf enter\n");
    if (g_ppp_ctx[nDlc] == NULL || g_ppp_ctx[nDlc]->nPppPcb == NULL)
        return NULL;
    OSI_LOGI(0x10007595, "getPppNetIf FINISH\n");
    return g_ppp_ctx[nDlc]->nPppPcb->netif;
}

int CFW_PppSendData(uint8_t *pData, uint16_t nDataSize, uint8_t MuxId)
{
    OSI_LOGI(0x10007596, "CFW_PppSendData enter MuxId = %d\n", MuxId);
    if (NULL == g_ppp_ctx[MuxId])
    {
        return ERR_INVALID_HANDLE;
    }
    sys_arch_dump(pData, nDataSize);
    pppos_input(g_ppp_ctx[MuxId]->nPppPcb, pData, nDataSize);
    return 1;
}
static void _pppactGprsActRsp(atCommand_t *cmd, const osiEvent_t *event)
{
   OSI_LOGI(0x10007597, "enter _pppactGprsActRsp");
    osiEvent_t *ev = (osiEvent_t *)malloc(sizeof(osiEvent_t));
    atDispatch_t *d = atCmdGetDispatch(cmd->engine);
    ev->id = EV_PPP_CFW_GPRS_ACT;
    ev->param1 = IN_cid;
    ev->param2 = IN_Sim;
    ev->param3 = atCmdGetChannel_id(d); //channel_id and ACT STATE may be active or deactive, it should be got form cmd
    //sys_arch_printf("finish _pppactGprsActRsp ev->param3 chanid = %d",ev->param3);
    osiEventSend(netGetTaskID(), ev);
    OSI_LOGI(0x10007598, "pre enter startPppIf");
    osiThreadSleep(1000);
    OSI_LOGI(0x10007599, "enter startPppIf");
    startPppIf(cmd, IN_Sim, IN_cid);
}

static void _pppattGprsAttRsp(atCommand_t *cmd, const osiEvent_t *event)
{
    OSI_LOGI(0x1000759a, "enter _pppattGprsAttRsp");
    OSI_LOGI(0x1000759b, "enter event id = %ld p1 = %ld p2 = %ld p3 = %ld",
                    event->id, event->param1, event->param2, event->param3);
    const CFW_EVENT *cfw_event = (const CFW_EVENT *)event;
    uint ret;
    if (cfw_event->nParam1 != 0)
        RETURN_CME_ERR(cmd->engine, ERR_AT_CME_EXE_FAIL);
    uint16_t nUTI = cfwRequestUTI();
    ret = CFW_GprsAct(CFW_GPRS_ACTIVED, IN_cid, nUTI, IN_Sim);
    if (ret != 0)
        RETURN_CME_CFW_ERR(cmd->engine, ret);

    atSetPendingUtiCmd(cmd, nUTI, _pppactGprsActRsp);
}

void GprsActCreateNetif(atCommand_t *PARA, uint8_t nSim, uint8_t nCid)
{
    OSI_LOGI(0x1000759c, "enter GprsActCreateNetif");
    uint ret;
    uint16_t nUTI = cfwRequestUTI();
    IN_cid = nCid;
    IN_Sim = nSim;
    OSI_LOGI(0x1000759d, "enter CFW_GprsAtt");
    ret = CFW_AttDetach(CFW_GPRS_ATTACHED, nUTI, ATTACH_TYPE, nSim);
    OSI_LOGI(0x1000759e, "finish CFW_GprsAtt ret = %d", ret);
    if (ret != 0)
        RETURN_CME_CFW_ERR(cmd->engine, ret);

    atSetPendingUtiCmd(PARA, nUTI, _pppattGprsAttRsp);
}
// This function response "CONNECT" and change to data staues.
void AT_PppProcess(atCommand_t *PARA, uint8_t nSim, uint8_t nCid)
{
    OSI_LOGI(0x1000759f, "enter AT_PppProcess");
    //OSI_LOGI(0, "AT_PppProcess:%d,%d,%d,isPppActived=%d", nDLCI, nSim, nCid, isPppActived);
    atDispatch_t *d = atCmdGetDispatch(PARA->engine);
   OSI_LOGI(0x100075a0, "enter atEngineModeSwitch");
    atEngineModeSwitch(AT_MODE_SWITCH_DATA_START, d);
    OSI_LOGI(0x100075a1, "finish atEngineModeSwitch");
    isPppActived = 1;
    atDataEngine_t *engine = atDispatchGetDataEngine(d);
    local_data_engine = engine;
    atDataSetPPPMode(engine);
    OSI_LOGI(0x100075a2, "finish atDataSetPPPMode");
    GprsActCreateNetif(PARA, nSim, nCid);
}

void AT_PppClose(uint8_t nDLCI)
{
    isPppActived = 0;
    OSI_LOGI(0x100075a3, "AT_PppClose enter\n");
    if (g_ppp_ctx[nDLCI] != NULL)
    {
        isPppActived = 0;
        g_ppp_ctx[nDLCI]->gprsState = PPP_DETACHED;
        ppp_close(g_ppp_ctx[nDLCI]->nPppPcb, 0); //FIXME  if pdp_rej received before lcp config req comes from peer
    }
}

void ppp_ip_output(struct pbuf *p, ctx_cb_t *ctx)
{
    OSI_LOGI(0x100075a4, "ppp_ip_output enter\n");
    struct netif *gprs_netif = ctx->gprs_netif;
    if (p != NULL && gprs_netif != NULL)
    {
#if LWIP_IPV6
        if (IP_HDR_GET_VERSION(p->payload) == 6)
            gprs_netif->output_ip6(gprs_netif, p, NULL);
        else
#endif
            gprs_netif->output(gprs_netif, p, NULL);
    }
    if (p)
        pbuf_free(p);
}

bool ppp_ip_input(struct pbuf *p, struct netif *gprs_netif, uint8_t nDLCI)
{
    //int ret;
    OSI_LOGI(0x100075a5, "ppp_ip_input enter\n");
    OSI_LOGI(0x100075a6, "ppp_ip_input nDLCI = %d\n", nDLCI);
    struct netif *ppp_netif = getPppNetIf(nDLCI);
    if (ppp_netif == NULL || g_ppp_ctx[nDLCI]->gprs_netif != gprs_netif)
        return false;

    if (p != NULL && gprs_netif != NULL)
    {
#if LWIP_IPV6
        if (IP_HDR_GET_VERSION(p->payload) == 6)
            ppp_netif->input_ip6(ppp_netif, p, NULL);
        else
#endif
            ppp_netif_output_ip4(ppp_netif, p, NULL);
    }
    if (p)
        pbuf_free(p);
    return true;
}

#endif
