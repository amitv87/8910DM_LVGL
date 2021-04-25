#ifndef _NETIF_PPP_H_
#define _NETIF_PPP_H_

//#include "sockets.h"
#include "netif/ppp/pppos.h"
#include "cfw.h"
#include "at_command.h"
#include "at_engine.h"
#include "at_response.h"
#include "cfw_errorcode.h"

#ifdef CONFIG_NET_LWIP_PPP_ON

typedef enum
{
    PPP_DETACHED,
    PPP_ATTCHING,
    PPP_ATTACHED,
    PPP_DEACTIVED,
    PPP_ACTIVING,
    PPP_ACTIVED,
} gprs_state_t;

typedef struct __ctx_cb
{
    ppp_pcb *nPppPcb;
    //struct atCmdEngine *engine;
    struct netif *gprs_netif;
    u8_t nCid;
    u8_t nSimId;
    u8_t nDlc;
    u8_t nUti;
    gprs_state_t gprsState;
} ctx_cb_t;

void ppp_ip_output(struct pbuf *p, void *ctx);
bool ppp_ip_input(struct pbuf *p, struct netif *gprs_netif, uint8_t nDLCI);
void ppp_StartGprs(uint8_t *usrname, uint8_t *usrpwd, void *ctx);
err_t ppp_netif_output_ip4(struct netif *netif, struct pbuf *pb, const ip4_addr_t *ipaddr);
void AT_PppProcess(atCommand_t *PARA, uint8_t nSim, uint8_t nCid);
#endif
#endif
