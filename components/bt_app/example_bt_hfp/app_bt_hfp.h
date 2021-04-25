#ifndef APP_BT_HFP_H
#define APP_BT_HFP_H

/* second argument of hf_call_request */
enum hfp_call_type
{
    /* use by hf_call_request */
    HFP_CALL_INCOMMING,
    HFP_CALL_OUTGOING,
    HFP_CALL_START,
    HFP_CALL_ALERT,
    HFP_CALL_RELEASE,
    HFP_CALL_NUM,
    HFP_CALL_MEM,
    HFP_CALL_REDIAL,
    HFP_CALL_SWAP,
    HFP_CALL_HOLD,

    /* use by hf_call_answer */
    HFP_CALL_ACCEPT,
    HFP_CALL_REJECT,

    /* use by hf_call_hold */
    HFP_CALL_HELD,
    HFP_CALL_WAIT,
    HFP_CALL_ACTIVE,
    /* use by hf_call_status */
    HFP_SERVICE_INDICATOR,
    HFP_CALL_INDICATOR,
    HFP_CALLSETUP_INDICATOR,
    HFP_CALLHOLD_INDICATOR,
    HFP_BATTCHG_INDICATOR,
    HFP_SIGNAL_INDICATOR,
    HFP_ROAM_INDICATOR,

    /* use by hf_voice_recognition */
    HFP_VR_START,
    HFP_VR_STOP
};

#endif