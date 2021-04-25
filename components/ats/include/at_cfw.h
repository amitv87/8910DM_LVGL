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

#ifndef __AT_CFW_H__
#define __AT_CFW_H__

#include "at_engine.h"
#include "cfw.h"
#include "cfw_event.h"
#include "cfw_dispatch.h"
#include "at_command.h"
#include "at_response.h"

#ifdef __cplusplus
extern "C" {
#endif

// For at module initializing
// TODO: delete them, use the SA flags instead.
typedef enum
{
    AT_MODULE_INIT_NO_SIM,
    AT_MODULE_INIT_SIM_AUTH_READY,
    AT_MODULE_INIT_SIM_AUTH_WAIT_PUK1,
    AT_MODULE_INIT_SIM_AUTH_WAIT_PIN1,
    AT_MODULE_INIT_SAT_OK,
    AT_MODULE_INIT_SAT_ERROR,
    AT_MODULE_INIT_NORMAL_SIM_CARD,
    AT_MODULE_INIT_CHANGED_SIM_CARD,
    AT_MODULE_INIT_NORMAL_CHANGED_SIM_CARD,
    AT_MODULE_INIT_TEST_SIM_CARD,
    AT_MODULE_INIT_SIM_SMS_OK,
    AT_MODULE_INIT_SIM_SMS_ERROR,
    AT_MODULE_INIT_SIM_PBK_OK,
    AT_MODULE_INIT_SIM_PBK_ERROR,
    AT_MODULE_INIT_ME_SMS_OK,
    AT_MODULE_INIT_ME_SMS_ERROR,
    AT_MODULE_INIT_ME_PBK_OK,
    AT_MODULE_INIT_ME_PBK_ERROR,
    AT_MODULE_INIT_BATT_OK,
    AT_MODULE_NOT_INIT = 0xff
} AT_MODULE_INIT_STATUS_T;

#define AT_AUTO_ACT_CID 5

#define AT_MUXSOCKET_NUM 8
#define AT_BLUETOOTH_CHANNEL 9

#define CLASS_TYPE_B 0
#define CLASS_TYPE_CG 1

#define AT_GPRS_ATT_NOTREG 0
#define AT_GPRS_ATT_HOME_OK 1
#define AT_GPRS_ATT_TRYING 2
#define AT_GPRS_ATT_DNY 3
#define AT_GPRS_ATT_UNKNOWN 4
#define AT_GPRS_ATT_ROAM_OK 5

#define AT_ACTIVE_STATUS_READY 0
#define AT_ACTIVE_STATUS_UNAVAILABLE 1
#define AT_ACTIVE_STATUS_UNKNOWN 2
#define AT_ACTIVE_STATUS_RINGING 3
#define AT_ACTIVE_STATUS_CALL_IN_PROGRESS 4
#define AT_ACTIVE_STATUS_ASLEEP 5

#define CC_STATE_NULL 0x00
#define CC_STATE_ACTIVE 0x01
#define CC_STATE_HOLD 0x02
#define CC_STATE_WAITING 0x04
#define CC_STATE_INCOMING 0x08
#define CC_STATE_DIALING 0x10
#define CC_STATE_ALERTLING 0x20
#define CC_STATE_RELEASE 0x40

#define AT_SMS_BODY_BUFF_LEN 176

#define AT_CC_MAX_NUM 7
#define ATD_PARA_MAX_LEN 41

typedef enum AT_DATA_STATE_T
{
    AT_PPP_STATE_END,
    AT_PPP_STATE_START,
    AT_PPP_STATE_ESCAPE,
    AT_PPP_STATE_RESUME
} AT_DATA_STATE_T;

typedef struct
{
    CFW_EVENT noSim[CONFIG_NUMBER_OF_SIM]; // DBS
    CFW_EVENT sim;
    CFW_EVENT sat;
    CFW_EVENT simCard;
    CFW_EVENT sms[CONFIG_NUMBER_OF_SIM];
    CFW_EVENT pbk;
} CFW_INIT_INFO;

#define AT_PBK_LIST_ENTRY_STEP 4
#define AT_FDN_NAME_SIZE (SIM_PBK_NAME_SIZE + 1)
#define AT_FDN_NUMBER_SIZE (SIM_PBK_NUMBER_SIZE + 1)

typedef enum
{
    COPS_MODE_AUTOMATIC = 0,
    COPS_MODE_MANUAL = 1,
    COPS_MODE_DEREGISTER = 2,
    COPS_MODE_SET_ONLY = 3,
    COPS_MODE_MANUAL_AUTOMATIC = 4,
    COPS_MODE_UNDEFINED = 5,
} AT_COPS_MODE;

typedef enum
{
    COPS_FORMAT_LONG = 0,
    COPS_FORMAT_SHORT = 1,
    COPS_FORMAT_NUMERIC = 2,
    COPS_FORMAT_UNDEFINED = 3
} AT_COPS_FORMAT;

typedef enum
{
    COPS_ACT_GSM = 0,
    COPS_ACT_GSM_COMPACT = 1,
    COPS_ACT_UTRAN = 2,
    COPS_ACT_GSM_EGPRS = 3,
    COPS_ACT_UTRAN_HSDPA = 4,
    COPS_ACT_UTRAN_HSUPA = 5,
    COPS_ACT_UTRAN_HSDPA_HSUPA = 6,
    COPS_ACT_EUTRAN = 7,
    COPS_ACT_ECGSM = 8,
    COPS_ACT_NBIOT = 9,
    COPS_ACT_UNDEFINED = 10
} AT_COPS_ACT;

typedef struct
{
    uint8_t nIndex;
    uint8_t nTpye;
    uint8_t nNumber[AT_FDN_NUMBER_SIZE];
    uint8_t nName[AT_FDN_NAME_SIZE];
} AT_FDN_PBK_RECORD;

typedef struct
{
    uint8_t nFDNSatus;
    uint8_t nTotalRecordNum;
    uint8_t nRealRecordNum;
    uint8_t nCurrentRecordIndx;
    AT_FDN_PBK_RECORD sRecord[1];
} AT_FDN_PBK_LIST;

typedef struct
{
    uint8_t battchg;
    uint8_t signal;
    uint8_t service;
    uint8_t sounder;
    uint8_t message;
    uint8_t call;
    uint8_t roam;
    uint8_t smsfull;
} atCindValue_t;

typedef enum
{
    AT_CHANNEL_CC_NONE,
    AT_CHANNEL_CC_DIALING,
    AT_CHANNEL_CC_ONLINE
} atChannelCCState_t;

typedef struct
{
    AT_MODULE_INIT_STATUS_T init_status[CONFIG_NUMBER_OF_SIM];
    CFW_INIT_INFO init_info;
    char g_gc_pwroff; //0: power on,1: power off, 2:reset
    atCmdEngine_t *pwroff_engine;
    bool setComFlag[CONFIG_NUMBER_OF_SIM];
    bool powerCommFlag[CONFIG_NUMBER_OF_SIM];
    bool gbPowerCommMsgFlag[CONFIG_NUMBER_OF_SIM];
    bool smsReady;
    bool pbkReady;
    bool copsFlag[CONFIG_NUMBER_OF_SIM];

    struct
    {
        atChannelCCState_t state;
        atCmdEngine_t *engine;

        bool alert_flag;
        bool emc_dailing;
        bool ims_call;
        bool last_dail_present;
        CFW_DIALNUMBER_V2 last_dial;
        char extension[ATD_PARA_MAX_LEN + 1];
        int extension_len;
        int extension_pos;
        osiTimer_t *extension_timer;
        osiTimer_t *alert_timer;
    } cc;

    char g_pCeer[64];
    uint8_t g_cfg_cfun[2]; // switch on/off; 5:current status is on
    uint8_t g_uClassType;

    struct
    {
        bool sms_init_done;

        uint8_t ring_count;
        uint8_t cgreg_val;
        uint8_t cereg_val;
        uint8_t cscon_mode;
        uint8_t cc_call;
        uint8_t cc_sounder;
        uint8_t sms_newsms;
        uint8_t sms_memfull; // [0] ME memfull, [1] SM memfull
        uint8_t ussd;
        uint32_t edrx_value;
        uint32_t edrx_ptw;
        AT_FDN_PBK_LIST *fdn_list;
        AT_Gprs_CidInfo cid_info[AT_PDPCID_MAX + 1];
        CFW_TFT_SET tft_set[AT_PDPCID_MAX + 1];
        osiTimer_t *ring_timer;
        osiTimer_t *newsms_timer;
        bool is_waiting_cnma;
    } sim[CONFIG_NUMBER_OF_SIM];
} atCfwCtx_t;

extern atCfwCtx_t gAtCfwCtx;

typedef struct _AT_CS_STAUTS
{
    uint32_t nParam1;
    uint8_t nType;
} AT_CS_STAUTS;

// #define NW_PREFERRED_OPERATOR_FORMAT_NUMERIC    2
#define NW_PREFERRED_OPERATOR_FORMAT_ALPHANUMERIC_LONG 0

// #define NW_GETPREFERREDOPERATORS_CALLER_READ 0
#define NW_GETPREFERREDOPERATORS_CALLER_SET 1

#define CFW_REGISTER_EVENT(ev) ev, _on##ev

void atCfwInit(void);
void atCfwPowerOn(const osiEvent_t *event);
void atCfwGcInit(void);
void atCfwNwInit(void);
void atCfwCcInit(void);
void atCfwGprsInit(void);
uint32_t atCfwSmsInit(uint16_t nUTI, uint8_t nSim);
void AT_PBK_Init(uint8_t nSim);
void AT_SS_Init(void);

/**
 * whether cc is active (dialing or online) in any channel
 *
 * \return
 *      - true if any channel is in dailing or online
 *      - false if all channels are offline
 */
bool atCfwCcIsActive(void);

/**
 * whether cc is active (dialing or online) in specified channel
 *
 * \param engine    the command engine
 * \return
 *      - true if the channel is in dailing or online
 *      - false if the channel is offline
 */
bool atCfwCcIsChannelActive(atCmdEngine_t *engine);

/**
 * whether cc is permitted in specified channel
 *
 * CC can only work in one channel at any time. So, if cc is active
 * and the specified channel is not the active channel, cc is not
 * allowed.
 *
 * \param engine    the command engine
 * \return
 *      - true if cc is permitted on this channel
 *      - false if cc is not allowed on this channel
 */
bool atCfwCcIsPermitted(atCmdEngine_t *engine);

/**
 * whether cc is permitted in specified channel
 *
 * Besides the channel is permitted for cc, this channel can't be
 * in dialing state.
 *
 * \param engine    the command engine
 * \return
 *      - true if dail is permitted
 *      - false if dail is not allowed
 */
bool atCfwCcIsDialPermitted(atCmdEngine_t *engine);

/**
 * get the dialing command engine
 *
 * \return
 *      - dailing command engine
 *      - NULL if no channels are in dailing mode
 */
atCmdEngine_t *atCfwDialingEngine(void);

/**
 * set cc to offline state
 *
 * When cc is already in offline state, nothing will be done
 */
void atCfwCcSetOffline(void);

/**
 * set cc state to dialing state on specified channel
 *
 * \param engine        the command engine
 * \param emc           true for emergency call
 */
void atCfwCcSetDialing(atCmdEngine_t *engine, bool emc);

/**
 * dail finish on the dailing channel
 *
 * After dail channel dailing finished, the channel will come
 * to online state.
 *
 * It is used in asynchronous event handling. So, there are no
 * parameter for the channel.
 */
void atCfwCcDailFinished(void);

/**
 * set cc state to dialing state on specified channel
 *
 * It is used at MT call, and answer successed.
 *
 * \param engine        the command engine
 */
void atCfwCcSetOnline(atCmdEngine_t *engine);

#define AT_SetCmeErrorCode(n) atCfwToCmeError(n)

atCindValue_t atCfwGetIndicator(uint8_t nSim);
uint32_t atCfwPhoneActiveStatus(uint8_t nSim);

void atCfwGprsSetPCid(uint8_t cid, uint8_t pcid, uint8_t nsim);

void atEnterPPPState(atCommand_t *cmd, uint8_t cid);
typedef enum
{
    AT_DISABLE_COMM = 0x00, //don't start
    AT_ENABLE_COMM,         // start stack
    AT_CHECK_COMM,
} AT_COMM_MODE;

/**
 * Check whether dial number is prohibited by FDN feature
 *
 * @param bcd       dial number BCD
 * @param bcd_len   dial number BCD length
 * @param nType     dial number type
 * @param nSim      SIM
 * @return
 *      - true if FDN feature is enabled and the dial number not in FDN list
 */
bool atFDNProhibit(const uint8_t *bcd, uint8_t bcd_len, uint8_t nTpye, uint8_t nSim);

bool atCheckCfwEvent(const osiEvent_t *event, uint32_t expected);

void atCfwSimHotPlugCB(int num, bool connect);

#define RETURN_CMS_CFW_ERR(engine, cfw_err) AT_CMD_RETURN(atCmdRespCmsError(engine, atCfwToCmsError(cfw_err)))
#define RETURN_CME_CFW_ERR(engine, cfw_err) AT_CMD_RETURN(atCmdRespCmeError(engine, atCfwToCmeError(cfw_err)))

void _onEV_CFW_SAT_CMDTYPE_IND(const osiEvent_t *event);
void _onEV_CFW_SAT_RESPONSE_RSP(const osiEvent_t *event);

void atSetPocMasterCard(CFW_SIM_ID nSimID);
CFW_SIM_ID atGetPocMasterCard(void);

#ifdef __cplusplus
}
#endif
#endif
