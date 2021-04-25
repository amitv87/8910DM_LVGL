#ifndef BT_ABS_H
#define BT_ABS_H

#define BT_MUSIC_OVER_HFG

#include "sci_types.h"

/*Add for virtual SPP COM*/
#if 0
#include "com_drv.h"
#include "sci_service.h"
#endif
typedef uint32 (*UartCallback)(uint32 event);
/******************************************************************************
 * Return code of BT functions
 ******************************************************************************/
typedef uint32 BT_STATUS;

#define BT_SUCCESS 0
#define BT_ERROR 1
#define BT_PENDING 2
#define BT_BUSY 3
#define BT_NO_RESOURCE 4
#define BT_NOT_FOUND 5
#define BT_DEVICE_NOT_FOUND 6
#define BT_CONNECTION_FAILED 7
#define BT_TIMEOUT 8
#define BT_NO_CONNECTION 9
#define BT_INVALID_PARM 10
#define BT_NOT_SUPPORTED 11
#define BT_CANCELLED 12

#define BT_IN_PROGRESS 19
#define BT_RESTRICTED 20
#define BT_INVALID_HANDLE 30
#define BT_PACKET_TOO_SMALL 31
#define BT_NO_PAIR 32

#define BT_FAILED 0x41
#define BT_DISCONNECT 0x43
#define BT_NO_CONNECT 0x44
#define BT_IN_USE 0x45
#define BT_MEDIA_BUSY 0x46

#define BT_OFF 0x50

/******************************************************************************
 * STRUCTRUE FOR BLE
 ******************************************************************************/

typedef struct
{
    uint16 usAdvIntervalMin;
    uint16 usAdvIntervalMax;

} BT_ADV_PARAM;

/******************************************************************************
 * BT address
 ******************************************************************************/
#define BT_ADDRESS_SIZE 6
typedef struct _BT_ADDRESS
{
    uint8 addr[BT_ADDRESS_SIZE];
} BT_ADDRESS;

#ifdef BT_SPP_SUPPORT
typedef struct _BT_ADDRESS_STATUS
{
    BOOLEAN is_paired;
    BT_ADDRESS paired_addr;
} BT_ADDRESS_STATUS;
#endif
#define WHITE_NUM_MAX 7
typedef struct _st_white_list_info
{
    uint8 addr_type;
    uint8 addr[BT_ADDRESS_SIZE];
} st_white_list_info;

typedef struct _st_white_list
{
    uint8 is_used;
    st_white_list_info white_list_info;
} st_white_list;

typedef struct _st_scan_report_info
{
    UINT8 name_length;
    UINT8 name[MGR_MAX_REMOTE_NAME_LEN];
    uint8 addr_type;
    BT_ADDRESS bdAddress;
} st_scan_report_info;

typedef struct _st_at_scan_report_info
{
    uint8 num;
    st_scan_report_info report_info[10];
} st_at_scan_report_info;

/******************************************************************************
 * Hold Action
 *****************************************************************************/
/* Releases all held calls or sets User Determined User Busy 
 * (UDUB) for a waiting call. 
 */
#define BT_HOLD_RELEASE_HELD_CALLS 0x00

/* Releases all active calls (if any exist) and accepts the other 
 * (held or waiting) call.
 */
#define BT_HOLD_RELEASE_ACTIVE_CALLS 0x01

/* Places all active calls (if any exist) on hold and accepts the
 * other (held or waiting) call.
 */
#define BT_HOLD_HOLD_ACTIVE_CALLS 0x02

/* Adds a held call to the conversation. */
#define BT_HOLD_ADD_HELD_CALL 0x03

/* Connects the two calls and disconnects the AG from 
 * both calls (Explicit Call Transfer).
 */
#define BT_HOLD_EXPLICIT_TRANSFER 0x04

/* Conference call,Hold all other except the special call 
 * 
*/
#define BT_HOLD_EXCEPT_SPECIAL_CALL 0x05

/* Call Indicator Status*/
#define BT_NO_CALL_SETUP 0x00
#define BT_INCOMING_CALL_SETUP 0x01
#define BT_OUTGOING_CALL_SETUP 0x02
#define BT_OUTGOING_REMOTE_ALERT 0x03

/*bpp client transfer model*/
#define BT_SIMPLE_PUSH_TRANSFER_MODEL (FALSE)
#define BT_JOB_BASED_TRANSFER_MODEL (TRUE)

/******************************************************************************
 * BT service type
 ******************************************************************************/
typedef uint32 BT_SERVICE_TYPE;
#define BT_NO_SERVICE ((BT_SERVICE_TYPE)0x00000000)
#define BT_FTP_SERVICE ((BT_SERVICE_TYPE)0x00000001)
#define BT_OPP_SERVICE ((BT_SERVICE_TYPE)0x00000002)

/******************************************************************************
 * BT message
 ******************************************************************************/
#define BT_MSG_DATA_SIZE 254
#define BT_FILE_NAME_SIZE 256
#define BT_DEVICE_NAME_SIZE 41
#define BT_PHONE_NUMBER_SIZE 46

/******************************************************************************
 * File message
 ******************************************************************************/
typedef uint16 BT_FILE_MODE;

#define BT_FILE_MODE_READ 0x0001
#define BT_FILE_MODE_WRITE 0x0002
#define BT_FILE_MODE_HIDDEN 0x0004
#define BT_FILE_MODE_SYS 0x0008
#define BT_FILE_MODE_VOLUME 0x0010
#define BT_FILE_MODE_DIR 0x0020
#define BT_FILE_MODE_ARCH 0x0040

typedef struct _BT_DEVICE_INFO
{
    uint16 name[BT_DEVICE_NAME_SIZE];
    BT_ADDRESS addr;
    uint32 dev_class;
    uint32 rssi;
} BT_DEVICE_INFO;
typedef struct _BT_DEVICE_INF_EXT
{
    uint16 name[BT_DEVICE_NAME_SIZE];
    BT_ADDRESS addr;
    uint32 dev_class;
    uint8 ssp_numric_key[7]; // 6 valid bytes, add 1 more "\0"
} BT_DEVICE_INFO_EXT;
typedef struct _BT_FILE_INFO
{
    uint16 name[BT_FILE_NAME_SIZE]; /* In UNICODE */
    uint32 size;
    BT_DEVICE_INFO remote_dev;
    BT_FILE_MODE file_mode; //Directory Or File
} BT_FILE_INFO;

typedef struct _BT_DIAL_NUMBER
{
    uint8 phone_number[BT_PHONE_NUMBER_SIZE];
} BT_DIAL_NUMBER;

typedef struct _BT_DIAL_MEMORY
{
    uint32 memory_indicator;
} BT_DIAL_MEMORY;

typedef struct _BT_DIAL_ACTION
{
    uint32 hold_action;
    uint32 index;
} BT_DIAL_ACTION_T;

typedef struct _BT_A2DP_INFO
{
    BT_ADDRESS dev_addr;
} BT_A2DP_INFO_T;

typedef struct _BT_A2DP_STATE
{
    uint32 dev_state;
} BT_A2DP_ISTATE_T;

typedef struct _BT_FILE_TRANSFER
{
    uint32 transmitted_count;
} BT_FILE_TRANSFER_T;

typedef struct _BT_MSG_Tag
{
    uint32 msg_id;
    uint8 status;
    uint32 body_size;
    void *body_ptr;
} BT_MSG_T;

typedef struct _BT_HFG_INDICATOR
{
    BOOLEAN cell_service;
    uint8 active_calls;
    uint8 call_setup;
    uint8 hold_call;
    uint8 rxlevel;
    BOOLEAN is_roaming;
    uint8 batlevel;
} BT_HFG_INDICATOR_T;

typedef struct _BT_HFG_WB_FLAG
{
    BOOLEAN is_wb_call_flag; //TRUE means WB Call;  FALSE means NB CALL
    BOOLEAN is_nrec_flag;    // TRUE: The Bluetooth headset support NREC;  FALSE: The Bluetooth headset cannot support NREC;
} BT_HFG_WB_FLAG;

typedef struct _BT_NOTIFY_CONTROLLER_ADDR
{
    uint32 addr_value;
} BT_NOTIFY_CONTROLLER_ADDR;

typedef enum
{
    BT_FTS_RECEIVE_IDLE = 0,
    BT_FTS_RECEIVE_ACTIVE,
    BT_FTS_RECEIVE_DEACTIVING,
    BT_FTS_RECEIVE_CONNECTED,
    BT_FTS_RECEIVE_REQ,
    BT_FTS_RECEIVING,
    BT_FTS_GET_REQ,
    BT_FTS_GETING
} BT_FTP_SERVER_STATE_E;

typedef enum
{
    BT_OPP_SEND_FILE_TYPE_COMMON = 0,
    BT_OPP_SEND_FILE_TYPE_VCARD,
    BT_OPP_SEND_FILE_TYPE_VCALENDER,
    BT_OPP_SEND_FILE_TYPE_VNOTE,
    BT_OPP_SEND_FILE_TYPE_VMESSAGE,
} BT_OPP_SEND_FILE_TYPE_E;

// Definition for DUN event,this event will be handled by tcpip,
//if the USB is plug out,the tcpip will judge if the DUN is connected,
//if the DUN is connected,will not disconnect all ppp connection
typedef enum
{
    BT_DUN_CONNECTED = (20 << 8) | 1,
    BT_DUN_DISCONNECTED,
    BT_DUN_MSG_MAX_NUM
} BT_DUN_EVENT_E;

/*---------------------------------------------------------------------------*/
//indicator type
//#define BT_SERVICE_INDICATOR                                 ((uint8) 1)
//#define BT_CALL_STATUS_INDICATOR                             ((uint8) 2)
//#define BT_CALL_SETUP_STATUS_INDICATOR                       ((uint8) 3)
//#define BT_SIGNAL_STRENGTH_INDICATOR                         ((uint8) 4)
//#define BT_ROAM_INDICATOR                                    ((uint8) 5)
//#define BT_BATTERY_CHARGE_INDICATOR                          ((uint8) 6)
//#define BT_CALL_HELD_INDICATOR                               ((uint8) 7)

//indicator value for service
// service: Service availability indication, where:
//<value>=0 implies no service. No Home/Roam network available.
//<value>=1 implies presence of service. Home/Roam network available.
typedef enum
{
    BT_HFP_IND_SERVICE_NO_SERVICE = 0,
    BT_HFP_IND_SERVICE_NORMAL_SERVICE = 1
} BT_HFP_IND_SERVICE_STATUS_E;

//indicator value for call status
// call: Standard call status indicator, where:
//<value>=0 means no call active.
//<value>=1 means a call is active.
typedef enum
{
    BT_HFP_IND_CALL_NO_CALL_ACTIVE = 0,
    BT_HFP_IND_CALL_CALL_ACTIVE = 1
} BT_HFP_IND_CALL_STATUS_E;

//indicator value for call setup
// callsetup: Bluetooth proprietary call set up status indicator3. Support for this indicator is optional for the HF. When supported, this indicator shall be used in conjunction with, and as an extension of the standard call indicator. Possible values are as follows:
//<value>=0 means not currently in call set up.
//<value>=1 means an incoming call process ongoing.
//<value>=2 means an outgoing call set up is ongoing.
//<value>=3 means remote party being alerted in an outgoing call.
typedef enum
{
    BT_HFP_IND_CALLSETUP_NOT_IN_CALLSETUP = 0,
    BT_HFP_IND_CALLSETUP_INCOMING = 1,
    BT_HFP_IND_CALLSETUP_OUTGOING = 2,
    BT_HFP_IND_CALLSETUP_REMOTE_ALERTED = 3
} BT_HFP_IND_CALLSETUP_STATUS_E;

//indicator value for call held
//callheld: Bluetooth proprietary call hold status indicator. Support for this indicator is mandatory for the AG, optional for the HF. Possible values are as follows:
//0= No calls held
//1//1= Call is placed on hold or active/held calls swapped
//(The AG has both and active AND a held call)
//2= Call on hold, no active call
typedef enum
{
    BT_HFP_IND_CALLHELD_NO_CALL_HELD = 0,
    BT_HFP_IND_CALLHELD_ONE_HOLD_ONE_ACTIVE = 1,
    BT_HFP_IND_CALLHELD_ONLY_HOlD_NO_ACTIVE = 2,
} BT_HFP_IND_CALLHELD_STATUS_E;

//indicator value for signal
// signal: Signal Strength indicator, where:
//value>= ranges from 0 to 5

//indicator value for Roaming status
// roam: Roaming status indicator, where:
//<value>=0 means roaming is not active
//<value>=1 means a roaming is active
typedef enum
{
    BT_HFP_IND_ROAM_NOT_ROAMING = 0,
    BT_HFP_IND_ROAM_IN_ROAMING = 1,
} BT_HFP_IND_ROAM_STATUS_E;

//indicator value for Battery Charge
// battchg: Battery Charge indicator of AG, where:
//<value>=ranges from 0 to 5

typedef struct _BT_HFU_CIEV_T
{
    uint8 ind_type;  //indicator type,see BT_SERVICE_INDICATOR etc.
    uint8 ind_value; //see the above notation
} BT_HFU_CIEV_T;

#define BT_HFP_MAX_VOLUME_LEVEL 15
typedef struct
{
    uint8 gain; //0-15,0-Minimum gain,15-Maximum gain
} BT_HFU_SPK_VOL_T,
    BT_HFU_MIC_VOL_T;

/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * hfuCallStatus type
 *
 * Defines the current state of a call.
 */
typedef uint8 hfuCallStatus;

/** An active call exists.
 */
#define HFU_CALL_STATUS_ACTIVE 0

/** The call is held.
 */
#define HFU_CALL_STATUS_HELD 1

/** A call is outgoing.
 */
#define HFU_CALL_STATUS_DIALING 2

/** The remote parting is being alerted.
 */
#define HFU_CALL_STATUS_ALERTING 3

/** A call is incoming.
 */
#define HFU_CALL_STATUS_INCOMING 4

/** The call is waiting.  This state occurs only when the audio gateway
 *  supports 3-Way calling.
 */
#define HFU_CALL_STATUS_WAITING 5

/** No active call
 */
#define HFU_CALL_STATUS_NONE 0xFF

/* End of hfuCallStatus */
/*---------------------------------------------------------------------------
 * HfuCallMode type
 *
 * Defines the current mode of a call.
 */
typedef uint8 HfuCallMode;

/* Voice Call */
#define HFU_CALL_MODE_VOICE 0

/* Data Call */
#define HFU_CALL_MODE_DATA 1

/* FAX Call */
#define HFU_CALL_MODE_FAX 2

/* End of HfuCallMode */

/*---------------------------------------------------------------------------
 * BT_HFU_CLCC_T structure
 *
 *     Used to identify the listed calls on the audio gateway.  This must
 *     correlate to the currentCalls structure defined in
 *     AtHandsfreeResult.
 */
typedef struct
{

    /* Index of the call on the audio gateway (1 based) */
    uint8 index;

    /* 0 - Mobile Originated, 1 = Mobile Terminated */
    uint8 dir;

    /* Call state (see hfuCallStatus). */
    hfuCallStatus state;

    /* Call mode (see HfuCallMode). */
    HfuCallMode mode;

    /* 0 - Not Multiparty, 1 - Multiparty */
    uint8 multiParty;

    /* Phone number of the call */
    uint8 number[BT_PHONE_NUMBER_SIZE];

    /* Type of address */
    uint8 type;

} BT_HFU_CLCC_T;

typedef struct
{
    /* Phone number of the caller */
    uint8 number[BT_PHONE_NUMBER_SIZE];

    /* Type of address */
    uint8 type;

} BT_HFU_CALLERID_PARAMS_T;

/******************************************************************************
sides:
Description: Specifies how pages are to be imposed upon the 
sides of a selected medium for the job
BPP_ONE_SIDED                     "one-sided"
BPP_TWO_SIDED_LONG                "two-sided-long-edge"
BPP_TWO_SIDED_SHORT               "two-sided-short-edge"


printfQuality:
Description: Specifies the print quality requested for the job
#define BPP_QUALITY_DRAFT                 "draft"
#define BPP_QUALITY_NORMAL                "normal"
#define BPP_QUALITY_HIGH                  "high"

maxCopies:  
     
Description: Specifies the number of copies of the job to be printed

NumberUp:   
   
Description: Indicates the number of print-stream pages to 
impose upon a single side of an instance of a selected medium 
for the job.  
(NumberUp shall be > 0) 
Examples: 
1 = One page per side. 
2 = Two pages per side. 
4 = Four pages per side. 


orientation:
     
Description: Indicates the desired orientation for printed pages of the job. 
Values:    
portrait    
landscape    
reverse-landscape    
reverse-portrait 
******************************************************************************/
#define BPP_ONE_SIDED 0
#define BPP_TWO_SIDED_LONG 1
#define BPP_TWO_SIDED_SHORT 2

#define BPP_QUALITY_DRAFT 0
#define BPP_QUALITY_NORMAL 1
#define BPP_QUALITY_HIGH 2

#define BPP_ORI_PORTRAIT 0
#define BPP_ORI_LANDSCAPE 1
#define BPP_ORI_REV_LANDSCAPE 2
#define BPP_ORI_REV_PORTRAIT 3

typedef struct
{
    uint8 sides;
    uint8 printfQuality;
    uint8 numberUp;
    uint8 maxCopies;
    uint8 orientation;
    uint8 document_format[20];

} BT_BPP_PRINTING_ATTRIUE;

typedef enum
{
    SPP_PORT_SPEED_2400 = 0x00,
    SPP_PORT_SPEED_4800 = 0x01,
    SPP_PORT_SPEED_7200 = 0x02,
    SPP_PORT_SPEED_9600 = 0x03,
    SPP_PORT_SPEED_19200 = 0x04,
    SPP_PORT_SPEED_38400 = 0x05,
    SPP_PORT_SPEED_57600 = 0x06,
    SPP_PORT_SPEED_115200 = 0x07,
    SPP_PORT_SPEED_230400 = 0x08,
    SPP_PORT_SPEED_UNUSED = 0xFF
} BT_SPP_PORT_SPEED_E;

typedef enum
{
    BT_TESTMODE_NONE = 0,  // Leave Test mode
    BT_TESTMODE_SIG = 1,   // Enter EUT signal mode
    BT_TESTMODE_NONSIG = 2 // Enter Nonsignal mode
} BT_TEST_MODE_T;

typedef enum
{
    BT_CHIP_ID_NULL = 0,
    BT_CHIP_ID_RDA = 1,
    BT_CHIP_ID_BEKEN = 2,
    BT_CHIP_ID_MAX
} BT_CHIP_ID_T;

typedef struct _BT_NONSIG_DATA
{
    uint8 rssi;
    BT_STATUS status;
    uint32 pkt_cnt;
    uint32 pkt_err_cnt;
    uint32 bit_cnt;
    uint32 bit_err_cnt;
} BT_NONSIG_DATA;

typedef struct _BT_NONSIG_PACKET
{
    uint16 pac_type; //enum{ NULLpkt, POLLpkt, FHSpkt, DM1, DH1, HV1, HV2, HV3, DV, AUX1, DM3, DH3, EV4, EV5, DM5, DH5, IDpkt, INVALIDpkt, EDR_2DH1, EV3, EDR_2EV3, EDR_3EV3, EDR_3DH1, EDR_AUX1, EDR_2DH3, EDR_3DH3, EDR_2EV5, EDR_3EV5, EDR_2DH5, EDR_3DH5};
    uint16 pac_len;  //according to packet type, max length is {0, 0, 18, 17, 27, 10, 20, 30, 9, 29, 121, 183, 120, 180, 224, 339, 0, 0, 54, 30, 60, 90, 83, 29, 367, 552, 360, 540, 679, 1021}
    uint16 pac_cnt;  //0~65536
} BT_NONSIG_PACKET;

typedef struct _BT_NONSIG_POWER
{
    uint8 power_type;   //0 or 1
    uint16 power_value; //0~33
} BT_NONSIG_POWER;

typedef struct _BT_NONSIG_PARAM
{
    uint8 pattern; //00000000 ---1, 11111111 ---2, 10101010 ---3, PRBS9 ---4, 11110000 ---9
    uint8 channel; //255 or 0~78
    union {
        uint8 rx_gain; //0~32
        BT_NONSIG_POWER power;
    } item;
    BT_NONSIG_PACKET pac;
} BT_NONSIG_PARAM;

typedef void (*BT_NONSIGCALLBACK)(const BT_NONSIG_DATA *data);

#if 0
BOOLEAN BT_SetNonSigRxTestMode(BOOLEAN Classic_or_BLE, BOOLEAN enable, BT_NONSIG_PARAM *param, const BT_ADDRESS *addr);
BOOLEAN BT_SetNonSigTxTestMode(BOOLEAN Classic_or_BLE, BOOLEAN enable, BT_NONSIG_PARAM *param);
BOOLEAN BT_GetNonSigRxData(BOOLEAN Classic_or_BLE, BT_NONSIGCALLBACK callback);
#else
BOOLEAN BT_SetNonSigRxTestMode(BOOLEAN enable, BT_NONSIG_PARAM *param, const BT_ADDRESS *addr);
BOOLEAN BT_GetNonSigRxData(BT_NONSIGCALLBACK callback);
BOOLEAN BT_SetNonSigTxTestMode(BOOLEAN enable, BT_NONSIG_PARAM *param);
BOOLEAN BT_BLE_SetNonSigRxTestMode(BOOLEAN enable, BT_NONSIG_PARAM *param, const BT_ADDRESS *addr);
BOOLEAN BT_BLE_GetNonSigRxData(BT_NONSIGCALLBACK callback);
BOOLEAN BT_BLE_SetNonSigTxTestMode(BOOLEAN enable, BT_NONSIG_PARAM *param);
void BT_NonSig_RX_Data_Callback(UINT8 *pdata);
void BT_NonSig_RX_Data_Process(hci_data_t *hci_data);
#endif

typedef enum
{
    BT_PSKEY_EVENT,
    BT_VENDOR_ENABLE_EVENT,
    BT_RESET_EVENT,
    BT_VENDOR_DISABLE_EVENT
} BT_CMD_EVENT_T;
BOOLEAN BT_GetCmdEventFlag(BT_CMD_EVENT_T event);
void BT_SetCmdEventFlag(BT_CMD_EVENT_T event, BOOLEAN flag);

/******************************************************************************
 * BT Device class
 ******************************************************************************/
#define BT_DEVICE_CLASS_NONE 0x00000000
#define BT_DEVICE_CLASS_COMPUTER 0x00000001
#define BT_DEVICE_CLASS_PHONE 0x00000002
#define BT_DEVICE_CLASS_AUDIO 0x00000004
#define BT_DEVICE_CLASS_PRINTER 0x00000008

/******************************************************************************
 * BT message ID
 ******************************************************************************/

/******************************************************************************
 * BT service type
 ******************************************************************************/
#define BT_SERVICE_NONE 0x0000
#define BT_SERVICE_HANDFREE 0x0001
#define BT_SERVICE_HANDSET 0x0002
#define BT_SERVICE_OPP 0x0004
#define BT_SERVICE_FTP 0x0008
#define BT_SERVICE_A2DP 0x0010
#define BT_SERVICE_AVRCP 0x0020
#define BT_SERVICE_SPP 0x0040
#define BT_SERVICE_A2DP_SRC 0x0080
#define BT_SERVICE_AVRCP_TG 0x0100
#define BT_SERVICE_MAP_SERVER 0x0200
#define BT_SERVICE_PBAP_SERVER 0x0400
#define BT_SERVICE_HANDFREE_GATEWAY 0x0800
#define BT_SERVICE_HEADSET_GATEWAY 0x1000
#define BT_SERVICE_BLE_HID 0x3000
#define BT_SERVICE_ALL 0xFFFF

#define EXT_BT_UNKNOWN_SERVICE 0x00000000
#define EXT_BT_SPP_SERVICE 0x00000002
#define EXT_BT_DUN_SERVICE 0x00000004
#define EXT_BT_FAX_SERVICE 0x00000008
#define EXT_BT_LAP_SERVICE 0x00000010
#define EXT_BT_HS_SERVICE 0x00000020
#define EXT_BT_HF_SERVICE 0x00000040
#define EXT_BT_OPP_SERVICE 0x00000080
#define EXT_BT_FTP_SERVICE 0x00000100
#define EXT_BT_CTP_SERVICE 0x00000200
#define EXT_BT_ICP_SERVICE 0x00000400
#define EXT_BT_SYNC_SERVICE 0x00000800
#define EXT_BT_BPP_SERVICE 0x00001000
#define EXT_BT_BIP_SERVICE 0x00002000
#define EXT_BT_PANU_SERVICE 0x00004000
#define EXT_BT_NAP_SERVICE 0x00008000
#define EXT_BT_GN_SERVICE 0x00010000
#define EXT_BT_SAP_SERVICE 0x00020000
#define EXT_BT_AV_SERVICE 0x00040000
#define EXT_BT_AVRCP_SERVICE 0x00080000
#define EXT_BT_HID_SERVICE 0x00100000  /* HID */
#define EXT_BT_PBAP_SERVICE 0x00300000 /* PBAP */

/******************************************************************************
 * BT Visible type
 ******************************************************************************/
#define BT_SCAN_E uint32
#define BT_NONE_ENABLE 0x00
#define BT_INQUIRY_ENABLE 0x01
#define BT_PAGE_ENABLE 0x02
#define BT_PAGE_AND_INQUIRY_ENABLE 0x03

/******************************************************************************
 * BT Channel
 ******************************************************************************/
#define BT_CHANNEL_SPK 0
#define BT_CHANNEL_MIC 1

/******************************************************************************
 * Local Name length
 ******************************************************************************/
#define BT_LOCAL_NAME_LENGTH 14 /* Include the terminal 0 */

/******************************************************************************
 * Audio Connection Transfer
 ******************************************************************************/
#define BT_AUDIO_CONN_AG_TO_HF 0
#define BT_AUDIO_CONN_HF_TO_AG 1

/******************************************************************************
 * Call setup status
 ******************************************************************************/
#define BT_CALL_SETUP_IDLE 0
#define BT_CALL_SETUP_INCOMING 1
#define BT_CALL_SETUP_OUTGOING 2
#define BT_CALL_SETUP_OUTGOING_ALERTED 3

/***********************************************************************************
* BT Freqence Define
************************************************************************************/
#define BT_IDLE_CLK 0
#define BT_FTP_CLK 1
#define BT_HFG_CLK 2
#define BT_A2DP_CLK 3

/***********************************************************************************
* Uart Define
************************************************************************************/
#define BT_COM0 0
#define BT_COM1 1
#define BT_COM2 2
#define BT_COM3 3

#define BT_COM_MAX 4

/***********************************************************************************
* Uart Baudrate Define
************************************************************************************/
#define BT_BAUD_1200 1200
#define BT_BAUD_2400 2400
#define BT_BAUD_4800 4800
#define BT_BAUD_9600 9600
#define BT_BAUD_19200 19200
#define BT_BAUD_38400 38400
#define BT_BAUD_57600 57600
#define BT_BAUD_115200 115200
#define BT_BAUD_230400 230400
#define BT_BAUD_460800 460800
#define BT_BAUD_921600 921600
#define BT_BAUD_1382400 1382400
#define BT_BAUD_3250000 3250000

#define BT_SERVICE_INDICATOR ((uint8)1)
#define BT_CALL_STATUS_INDICATOR ((uint8)2)
#define BT_CALL_SETUP_STATUS_INDICATOR ((uint8)3)
#define BT_SIGNAL_STRENGTH_INDICATOR ((uint8)4)
#define BT_ROAM_INDICATOR ((uint8)5)
#define BT_BATTERY_CHARGE_INDICATOR ((uint8)6)
#define BT_CALL_HELD_INDICATOR ((uint8)7)

/* CIEV command - values used for ROAM_INDICATOR */
#define BT_ROAM_OFF ((uint8)0)
#define BT_ROAM_ON ((uint8)1)

#define BT_NO_SERVICE_VALUE ((uint8)0)
#define BT_SERVICE_PRESENT_VALUE ((uint8)1)

/******************************************************************************
 * BT callback function
 ******************************************************************************/
typedef void (*BT_CALLBACK)(const BT_MSG_T *msg);
typedef void (*BT_CALLBACK_STACK)(unsigned int msg_id, char status, void *data_ptr);
typedef void (*BLE_WRITE_UART_CALLBACK)(short dlen, uint8 *data);
/******************************************************************************
 * BT Config
 *  slot_number: 0 or 1 or 2
 *  config: 0x08a00004  //PCM_CLK = 128KHz, Master Mode, Slot total number = 1, 
 *          0x08800004  //PCM_CLK = 256KHz, Master Mode, Slot total number = 2, 
 *          0x08c00004  //PCM_CLK = 512KHz, Master Mode, Slot total number = 3, 
 *          0x08a00006  //PCM_CLK = 128KHz, Slave Mode, Slot total number = 1, 
 *          0x08800006  //PCM_CLK = 256KHz, Slave Mode, Slot total number = 2, 
 *          0x08c00006  //PCM_CLK = 512KHz, Slave Mode, Slot total number = 3, 
 *  format: 0x60   //16 bit
 *
 ******************************************************************************/
typedef struct _BT_PCM_CONFIG
{
    uint8 slot_number; /* starting from 0 */
    uint32 config;     //config the pcm of bt chip
    uint32 format;
} BT_PCM_CONFIG;

typedef struct _BT_CONFIG
{
    BT_ADDRESS bd_addr;
    uint16 local_name[BT_DEVICE_NAME_SIZE];
    uint32 port_num;
    uint32 baud_rate;
    BT_PCM_CONFIG pcm_config;
    uint16 bt_file_dir[16];
    uint32 bt_task_priority;
    uint32 bt_crystal_freq; //KHZ
    uint16 xtal_dac;
    uint32 host_nv_ctrl[2];
} BT_CONFIG;

extern BT_CONFIG g_BTConfig;

typedef uint32 BT_PBAP_OP_T;

#define BT_PBAPOP_NONE 0x0000               /* No current operation */
#define BT_PBAPOP_CONNECT 0x0001            /* Connect operation */
#define BT_PBAPOP_DISCONNECT 0x0002         /* Disconnect operation */
#define BT_PBAPOP_SET_PHONEBOOK 0x0004      /* Set Phonebook operation */
#define BT_PBAPOP_PULL 0x0008               /* Generic Pull operation  */
#define BT_PBAPOP_PULL_PHONEBOOK 0x0010     /* Pull Phonebook operation */
#define BT_PBAPOP_PULL_VCARD_LISTING 0x0020 /* Pull vCard listing operation */
#define BT_PBAPOP_PULL_VCARD_ENTRY 0x0040   /* Pull vCard entry operation */

typedef uint8 BT_PBAP_VCARD_FORMAT_T;

#define BT_VCARD_FORMAT_21 0x00 /* Version 2.1 format */
#define BT_VCARD_FORMAT_30 0x01 /* Version 3.0 format */

typedef struct _BT_PBAP_DATA_IND_T
{
    BT_PBAP_OP_T oper_type;
    BOOLEAN is_first_block;
    uint16 len;
    uint8 *buff;
} BT_PBAP_DATA_IND_T;

typedef struct _BT_PBAP_CLIENT_PARAMS_RX_T
{
    /* Number of new missed calls */
    uint8 newMissedCalls;
    /* Provides the size of the requested phonebook. The client 
     * should set its MaxListCount based on the phonebook size, 
     * if it is nonzero. 
     */
    uint16 phonebookSize;
} BT_PBAP_PARAMS_RX_T, BT_PBAP_CLIENT_PARAMS_RX_T;

//bit 0       VERSION                     vCard Version
//Bit 1       FN                          Formatted Name
//bit 2       N                           Structured Presentation of Name
//bit 3       PHOTO                       Associated Image or Photo
//bit 4       BDAY                        Birthday
//bit 5       ADR                         Delivery Address
//bit 6       LABEL                       Delivery
//bit 7       TEL                         Telephone Number
//bit 8       EMAIL                       Electronic Mail Address
//bit 9       MAILER                      Electronic Mail
//bit 10      TZ                          Time Zone
//bit 11      GEO                         Geographic Position
//bit 12      TITLE                       Job
//bit 13      ROLE                        Role within the Organization
//bit 14      LOGO                        Organization Logo
//bit 15      AGENT                       vCard of Person Representing
//bit 16      ORG                         Name of Organization
//bit 17      NOTE                        Comments
//bit 18      REV                         Revision
//bit 19      SOUND                       Pronunciation of Name
//bit 20      URL                         Uniform Resource Locator
//bit 21      UID                         Unique ID
//bit 22      KEY                         Public Encryption Key
//bit 23      NICKNAME                    Nickname
//bit 24      CATEGORIES                  Categories
//bit 25      PROID                       Product ID
//bit 26      CLASS                       Class information
//bit 27      SORT-STRING                 String used for sorting operations
//bit 28      X-IRMC-CALL-DATETIME        Time stamp
//bit 29~38                               Reserved for future use
//bit 39      Proprietary Filter          Indicates the usage of a proprietary filter
//bit 40 ~ 63                             Reserved for proprietary filter usage
//
//Mandatory attributes for vCard 2.1 are VERSION ,N and TEL.
//Mandatory attributes for vCard 3.0 are VERSION, N, FN and TEL.

#define BT_PBAP_FILTER_SIZE 8
typedef struct _BT_PBAP_VCARD_FILTER_T
{
    /* Array of 8 bytes for this 64-bit filter value */
    uint8 byte[BT_PBAP_FILTER_SIZE];
} BT_PBAP_VCARD_FILTER_T;

typedef uint8 BT_PBAP_VCARD_SEARCH_ATTRIB_T;

#define BT_PBAP_VCARD_SEARCH_ATTRIB_NAME 0x00   /* Search by Name */
#define BT_PBAP_VCARD_SEARCH_ATTRIB_NUMBER 0x01 /* Search by Number */
#define BT_PBAP_VCARD_SEARCH_ATTRIB_SOUND 0x02  /* Search by Sound */

typedef uint8 BT_PBAP_VCARD_SORT_T;

#define BT_PBAP_VCARD_SORT_ORDER_INDEXED 0x00    /* Indexed sorting */
#define BT_PBAP_VCARD_SORT_ORDER_ALPHA 0x01      /* Alphabetical sorting */
#define BT_PBAP_VCARD_SORT_ORDER_PHONETICAL 0x02 /* Phonetical sorting */

typedef uint8 BT_PBAP_RESP_CODE_T;

/* Group: Successful response codes */

#define BT_PBRC_CONTINUE 0x10       /* Continue */
#define BT_PBRC_STATUS_SUCCESS 0x20 /* Success */

/* Group: Failure response codes */

#define BT_PBRC_BAD_REQUEST 0x40         /* Bad Request */
#define BT_PBRC_UNAUTHORIZED 0x41        /* Unauthorized */
#define BT_PBRC_FORBIDDEN 0x43           /* Forbidden - operation is understood */
#define BT_PBRC_NOT_FOUND 0x44           /* Not Found */
#define BT_PBRC_NOT_ACCEPTABLE 0x46      /* Not Acceptable */
#define BT_PBRC_PRECONDITION_FAILED 0x4c /* Precondition Failed */
#define BT_PBRC_NOT_IMPLEMENTED 0x51     /* Not Implemented */
#define BT_PBRC_SERVICE_UNAVAILABLE 0x53 /* Service Unavailable */

#define BT_PBRC_LINK_DISCONNECT 0x80 /* Transport connection has been disconnected. */
#define BT_PBRC_CLIENT_RW_ERROR 0x81 /* The client suffered an Object Store read or write error. */
#define BT_PBRC_USER_ABORT 0x82      /* The operation was aborted because of a call to BT_PbapAbortClient(). */

#if OBEX_SESSION_SUPPORT == XA_ENABLED
#define BT_PBRC_SESSION_USER_ABORT 0x83 /* This status is only valid for a reliable OBEX session. */
#define BT_PBRC_SESSION_NO_SEQ_NUM 0x84 /* The client did not receive a Session Sequence Number header in the last server session response. */
#define BT_PBRC_SESSION_INVALID_PARMS 0x85
#define BT_PBRC_SESSION_ERROR 0x86
#endif /* OBEX_SESSION_SUPPORT == XA_ENABLED */

typedef struct _BT_PBAP_ABORT_INFO_T
{
    BT_PBAP_OP_T oper_type;
    BT_PBAP_RESP_CODE_T resp_code;
} BT_PBAP_ABORT_INFO_T;

typedef struct _BT_PBAP_TP_DISCONNECTED_INFO_T
{
    BT_PBAP_OP_T oper_type;
#if MTOBEX_STACK_VERSION <= 380
    BT_PBAP_RESP_CODE_T resp_code;
#endif
} BT_PBAP_TP_DISCONNECTED_INFO_T;

#define BT_PBAP_MAX_NAME_LEN 128
#define BT_PBAP_MAX_SEARCH_VALUE_LEN 255

typedef struct _BT_PBAP_PULL_PHONEBOOK_OPER_T
{
    char pbName[BT_PBAP_MAX_NAME_LEN];   /* Name of the phonebook to receive (*.vcf) */
    BT_PBAP_VCARD_FILTER_T vcard_filter; /* Filter of the required vCard fields */
    BT_PBAP_VCARD_FORMAT_T vcard_format; /* Format of vCard (2.1 or 3.0) */
    uint16 maxListCount;                 /* Maximum of vCard entries supported */
    uint16 listStartOffset;              /* Offset of the first vCard entry */
} BT_PBAP_PULL_PHONEBOOK_OPER_T;

typedef struct _BT_PBAP_PULL_VCARDLISTING_OPER_T
{
    char folderName[BT_PBAP_MAX_NAME_LEN];          /* Name of the folder to retrieve */
    BT_PBAP_VCARD_SORT_T order;                     /* Indexed/Alphabetical/Phonetical */
    BT_PBAP_VCARD_SEARCH_ATTRIB_T searchAttrib;     /* Name (0x00), Number (0x01), or Sound (0x02) */
    char searchValue[BT_PBAP_MAX_SEARCH_VALUE_LEN]; /* Absolute or partial */
    uint8 searchValueLen;                           /* Length of search value */
    uint16 maxListCount;                            /* Maximum of vCard entries supported */
    uint16 listStartOffset;                         /* Offset of the first vCard entry */
} BT_PBAP_PULL_VCARDLISTING_OPER_T;

typedef struct _BT_PBAP_PULL_VCARDENTRY_OPER_T
{
    char entryName[BT_PBAP_MAX_NAME_LEN]; /* Name of the entry to retrieve (*.vcf) */
    BT_PBAP_VCARD_FILTER_T vcard_filter;  /* Filter of the required vCard fields */
    BT_PBAP_VCARD_FORMAT_T vcard_format;  /* Format of vCard (2.1 or 3.0) */
} BT_PBAP_PULL_VCARDENTRY_OPER_T;

typedef struct _BT_PBAP_SERVER_PARAMS_RX_T
{
    BT_PBAP_OP_T oper_type;

    union {
        BT_PBAP_PULL_PHONEBOOK_OPER_T phonebook_oper;
        BT_PBAP_PULL_VCARDLISTING_OPER_T vcardlisting_oper;
        BT_PBAP_PULL_VCARDENTRY_OPER_T vcardentry_oper;
    } u;
} BT_PBAP_SERVER_PARAMS_RX_T;

/*---------------------------------------------------------------------------
 *
 *     This type is used to identify the portion of path information that
 *     is provided in the Phonebook Name.  For example, a Pull Phonebook
 *     operation supplies the entire absolute path name for the phonebook,
 *     while Pull Vcard Listing and Pull Vcard Entry provide only the name
 *     of the phonebook and the name of the vCard, respectively.
 */
typedef uint8 BT_PBAP_PB_NAME_TYPE_T;

#define PB_NAME_TYPE_ABSOLUTE 0 /* Absolute path information */
#define PB_NAME_TYPE_LOCAL 1    /* Local path information */

/* Retrieves the number of records in the phonebook object database. */
typedef uint32 (*PbapServer_GetPbSize)(
    char *PbName,
    BT_PBAP_PB_NAME_TYPE_T Type);

/* Returns the size of the requested phonebook entry from the currently selected phonebook object.
   0x00 a phonebook cannot be found, while 0xFFFFFFFF indicates a precondition failed case. */
typedef uint32 (*PbapServer_GetPbEntrySize)(
    const char *Name,
    BT_PBAP_VCARD_FILTER_T *Filter);

/* Returns the size of the entire phonebook object.
   0x00 a phonebook cannot be found, while 0xFFFFFFFF indicates a precondition failed case. */
typedef uint32 (*PbapServer_GetPbObjectSize)(
    const char *Name,
    BT_PBAP_PULL_PHONEBOOK_OPER_T *PullPb);

/* Retrieves the number of new missed records in the phonebook object database. */
typedef uint32 (*PbapServer_GetPbNewMissedSize)(
    char *PbName,
    BT_PBAP_PB_NAME_TYPE_T Type);

/* Returns an entire phonebook database of data. */
typedef BT_STATUS (*PbapServer_PbReadPhonebook)(
    BT_PBAP_PULL_PHONEBOOK_OPER_T *PullPb,
    uint8 *DataPtr,
    uint32 DataLen);

/* Returns a particular entry from a specific phonebook. */
typedef BT_STATUS (*PbapServer_PbReadVcardEntry)(
    BT_PBAP_PULL_VCARDENTRY_OPER_T *PullEntry,
    uint8 *DataPtr,
    uint32 DataLen);

/* Returns an XML vCard listing of a particular folder */
typedef BT_STATUS (*PbapServer_PbReadVcardListing)(
    BT_PBAP_PULL_VCARDLISTING_OPER_T *PullList,
    uint8 *DataPtr,
    uint16 *DataLen,
    BOOLEAN *More);

/* Sets the path for the current phonebook operation. */
typedef char *(*PbapServer_PbSetPathInfo)(
    const char *Path,
    BOOLEAN Backup,
    BOOLEAN Reset);

typedef struct _PBAP_SERVER_CALLBACK_T
{
    PbapServer_GetPbNewMissedSize PbGetNewMissedSize;

    PbapServer_GetPbSize PbGetPbSize;
    PbapServer_GetPbEntrySize PbGetPbEntrySize;
    PbapServer_GetPbObjectSize PbGetPbObjectSize;

    PbapServer_PbReadPhonebook PbReadPhonebook;
    PbapServer_PbReadVcardEntry PbReadVcardEntry;
    PbapServer_PbSetPathInfo PbSetPathInfo;
    PbapServer_PbReadVcardListing PbReadVcardListing;
} PBAP_SERVER_CALLBACK_T;

#ifdef BT_MAP_SUPPORT
#define BT_MAP_MAX_NAME_LEN 128
#define MAX_MSE_TIME_COUNT 40

typedef uint8 BtMapMsgTypeFlags;
#define BT_MAP_MESSAGE_TYPE_NONE 0x00     /* No filtering */
#define BT_MAP_MESSAGE_TYPE_SMS_GSM 0x01  /* Filter out SMS_GSM messages */
#define BT_MAP_MESSAGE_TYPE_SMS_CDMA 0x02 /* Filter out SMS_CDMA messages */
#define BT_MAP_MESSAGE_TYPE_EMAIL 0x04    /* Filter out EMAIL messages */
#define BT_MAP_MESSAGE_TYPE_MMS 0x08      /* Filter out MMS messages */
#define BT_MAP_MESSAGE_TYPE_MASK 0x0F

typedef uint8 BtMapReadStatusFlags;
#define BT_MAP_READ_STATUS_NONE 0x00       /* Get both Read and Unread Messages */
#define BT_MAP_READ_STATUS_UNREAD 0x01     /* Get Unread Messages Only */
#define BT_MAP_READ_STATUS_READ 0x02       /* Get Read Messages Only */
#define BT_MAP_READ_STATUS_MASK 0x03       /* Read Status Mask */
#define BT_MAP_READ_STATUS_NOT_ISSUED 0xFF /* Don't issue this optional header */

typedef uint8 BtMapPriorityFlags;
#define BT_MAP_PRIORITY_NONE 0x00       /* Get all priority messages */
#define BT_MAP_PRIORITY_HIGH 0x01       /* Get High Priority Messages Only */
#define BT_MAP_PRIORITY_NON_HIGH 0x02   /* Get non-High Priority Messages Only */
#define BT_MAP_PRIORITY_MASK 0x03       /* Priority Mask */
#define BT_MAP_PRIORITY_NOT_ISSUED 0xFF /* Don't issue this optional header */

typedef uint8 BtMapCharsetFlags;
#define BT_MAP_CHARSET_NATIVE 0x00 /* Native */
#define BT_MAP_CHARSET_UTF8 0x01   /* UTF-8 */

typedef uint8 BtMapFractionReqFlags;
#define BT_MAP_FRACTION_REQ_FIRST 0x00 /* First */
#define BT_MAP_FRACTION_REQ_NEXT 0x01  /* Next */
#define BT_MAP_FRACTION_REQ_NONE 0xFF  /* No Fractions */

typedef uint8 BtMapSetMsgStatusFlags;
#define BT_MAP_SET_MSG_STATUS_READ 0x00    /* Read Status */
#define BT_MAP_SET_MSG_STATUS_DELETED 0x01 /* Deleted Status */

typedef uint8 BtMapTransparentFlags;
#define BT_MAP_TRANS_OFF 0x00        /* Do not copy items to the sent folder */
#define BT_MAP_TRANS_ON 0x01         /* Copy items to the sent folder*/
#define BT_MAP_TRANS_NOT_ISSUED 0xFF /* Do not send this optional header */

typedef uint8 BtMapRetryFlags;
#define BT_MAP_RETRY_OFF 0x00        /* Do not retry failed item */
#define BT_MAP_RETRY_ON 0x01         /* Retry failed item */
#define BT_MAP_RETRY_NOT_ISSUED 0xFF /* Do not send this optional header */

typedef uint8 BT_MAP_RESP_CODE_T;
/* Group: Successful response codes */
#define BT_MRC_CONTINUE 0x10       /* Continue */
#define BT_MRC_STATUS_SUCCESS 0x20 /* Success */
/* Group: Failure response codes */
#define BT_MRC_BAD_REQUEST 0x40         /* Bad Request */
#define BT_MRC_UNAUTHORIZED 0x41        /* Unauthorized */
#define BT_MRC_FORBIDDEN 0x43           /* Forbidden - operation is understood */
#define BT_MRC_NOT_FOUND 0x44           /* Not Found */
#define BT_MRC_NOT_ACCEPTABLE 0x46      /* Not Acceptable */
#define BT_MRC_PRECONDITION_FAILED 0x4c /* Precondition Failed */
#define BT_MRC_NOT_IMPLEMENTED 0x51     /* Not Implemented */
#define BT_MRC_SERVICE_UNAVAILABLE 0x53 /* Service Unavailable */
#define BT_MRC_LINK_DISCONNECT 0x80     /* Transport connection has been disconnected. */

typedef uint8 BtMapFractionDeliverFlags;
#define BT_MAP_FRACTION_DELIVER_MORE 0x00 /* More Message Fractions Coming */
#define BT_MAP_FRACTION_DELIVER_LAST 0x01 /* Last Message Fraction */
#define BT_MAP_FRACTION_NOT_ISSUED 0xFF   /* Do not send this optional header */

typedef uint32 BT_MAP_OP_T;
#define BT_MAPOP_NONE 0x00                 /* No current operation */
#define BT_MAPOP_CONNECT 0x01              /* Connect operation (MAS service) */
#define BT_MAPOP_SERVER_CONNECT 0x02       /* Server Connect operation (MNS service) */
#define BT_MAPOP_DISCONNECT 0x03           /* Disconnect operation (MAS service) */
#define BT_MAPOP_SERVER_DISCONNECT 0x04    /* Server Disconnect operation (MNS service) */
#define BT_MAPOP_PULL 0x05                 /* Generic Pull operation */
#define BT_MAPOP_PULL_MESSAGE 0x06         /* Pull Messsage operation */
#define BT_MAPOP_PULL_FOLDER_LISTING 0x07  /* Get Folder Listing operation */
#define BT_MAPOP_PULL_MESSAGE_LISTING 0x08 /* Get Messages Listing operation */
#define BT_MAPOP_SET_FOLDER 0x09           /* Set Message operation */
#define BT_MAPOP_PUSH 0x0A                 /* Generic Push operation */
#define BT_MAPOP_PUSH_MESSAGE 0x0B         /* Push Message operation */
#define BT_MAPOP_SET_REGISTRATION 0x0C     /* Set Notification Registration operation */
#define BT_MAPOP_SEND_EVENT 0x0D           /* Send Event operation */
#define BT_MAPOP_UPDATE_INBOX 0x0E         /* Update Inbox operation */
#define BT_MAPOP_SET_MESSAGE_STATUS 0x0F   /* Set Message Status operation */

typedef uint32 BtMapParameterMask;
#define BT_MAP_PARAM_NONE 0x00000000             /* Get all required parameters */
#define BT_MAP_PARAM_SUBJECT 0x00000001          /* Subject Parameter           (bit 0)  */
#define BT_MAP_PARAM_DATETIME 0x00000002         /* Date Time Parameter         (bit 1)  */
#define BT_MAP_PARAM_SENDER_NAME 0x00000004      /* Sender Name Parameter       (bit 2)  */
#define BT_MAP_PARAM_SENDER_ADDR 0x00000008      /* Sender Address Parameter    (bit 3)  */
#define BT_MAP_PARAM_RECIPIENT_NAME 0x00000010   /* Recipient Name Parameter    (bit 4)  */
#define BT_MAP_PARAM_RECIPIENT_ADDR 0x00000020   /* Recipient Address Parameter (bit 5)  */
#define BT_MAP_PARAM_TYPE 0x00000040             /* Type Parameter              (bit 6)  */
#define BT_MAP_PARAM_SIZE 0x00000080             /* Size Parameter              (bit 7)  */
#define BT_MAP_PARAM_RECEPTION_STATUS 0x00000100 /* Reception Status Parameter  (bit 8)  */
#define BT_MAP_PARAM_TEXT 0x00000200             /* Text Parameter              (bit 9)  */
#define BT_MAP_PARAM_ATTACH_SIZE 0x00000400      /* Attachment Size Parameter   (bit 10) */
#define BT_MAP_PARAM_PRIORITY 0x00000800         /* Priority Parameter          (bit 11) */
#define BT_MAP_PARAM_READ 0x00001000             /* Read Parameter              (bit 12) */
#define BT_MAP_PARAM_SENT 0x00002000             /* Sent Parameter              (bit 13) */
#define BT_MAP_PARAM_PROTECTED 0x00004000        /* Protected Parameter         (bit 14) */
#define BT_MAP_PARAM_REPLYTO_ADDR 0x00008000     /* Reply-to Address Parameter  (bit 15) */
#define BT_MAP_PARAM_MASK 0x0000FFFF             /* Parameter Mask                       */
#define BT_MAP_PARAM_NOT_ISSUED 0xFFFFFFFF       /* Don't issue this optional header     */

typedef struct _BT_MAP_FILTER_T
{
    BtMapMsgTypeFlags FilterMessageType;
    const char *FilterPeriodBegin;
    const char *FilterPeriodEnd;
    BtMapReadStatusFlags FilterReadStatus;
    const char *FilterRecipient;
    const char *FilterOriginator;
    BtMapPriorityFlags FilterPriority;
} BT_MAP_FILTER_T;

typedef struct _BT_MAP_CLIENT_PARAMS_RX_T
{
    BT_MAP_OP_T oper;

    /* Group: Valid during Get Message operation */
    BtMapFractionDeliverFlags fractionDeliver;

    /* Group: Valid during Get Folder Listing operation */
    uint16 folderListingSize; /* Folder Listing Size */

    /* Group: Valid during Get Messages Listing operation */
    uint16 messageListingSize;        /* Message Listing Size */
    BOOLEAN newMessage;               /* New Message */
    char mseTime[MAX_MSE_TIME_COUNT]; /* MSE Time */
    uint8 mseTimeLen;                 /* Length of the MSE Time */
} BT_MAP_CLIENT_PARAMS_RX_T;

typedef struct _BT_MAP_DATA_IND_T
{
    /* Object name (null-terminated, ASCII or UTF-16) */
    BT_MAP_OP_T oper_type;
    uint8 *buff; /* Data pointer */
    uint16 len;  /* Length of data */
} BT_MAP_DATA_IND_T;

typedef struct _BT_MAP_TP_DISCONNECTED_INFO_T
{
    BT_MAP_OP_T oper_type;
#if MTOBEX_STACK_VERSION <= 380
    BT_MAP_RESP_CODE_T resp_code;
#endif
} BT_MAP_TP_DISCONNECTED_INFO_T;

typedef struct _BT_MAP_ABORT_INFO_T
{
    BT_MAP_OP_T oper_type;
    BT_MAP_RESP_CODE_T resp_code;
} BT_MAP_ABORT_INFO_T;

typedef struct _BT_MAP_GET_FOLDER_LISTING_OPER_T
{
    char folderName[BT_MAP_MAX_NAME_LEN]; /* Name of the folder to retrieve */
    uint16 MaxListCount;
    uint16 ListOffset;
} BT_MAP_GET_FOLDER_LISTING_OPER_T;

typedef struct _BT_MAP_GET_MESSAGE_LISTING_OPER_T
{
    char folderName[BT_MAP_MAX_NAME_LEN]; /* Name of the folder to retrieve */
    uint16 MaxListCount;
    uint16 ListStartOffset;
    uint8 SubjectLen;
    uint32 ParameterMask;
    BT_MAP_FILTER_T Filter;
} BT_MAP_GET_MESSAGE_LISTING_OPER_T;

typedef struct _BT_MAP_GET_MESSAGE_OPER_T
{
    char MessageHandleName[BT_MAP_MAX_NAME_LEN]; /* the handle of the message to retrieve*/
    BOOLEAN attachment;
    BtMapCharsetFlags charset;
    BtMapFractionReqFlags fraction_req;
} BT_MAP_GET_MESSAGE_OPER_T;

typedef struct _BT_MAP_SET_MESSAGE_STATUS_OPER_T
{
    char MessageHandleName[BT_MAP_MAX_NAME_LEN]; /* Name of the folder to retrieve */
    BtMapSetMsgStatusFlags statusInd;
    BOOLEAN statusValue;
} BT_MAP_SET_MESSAGE_STATUS_OPER_T;

typedef struct _BT_MAP_PUSH_MESSAGE_OPER_T
{
    char folderName[BT_MAP_MAX_NAME_LEN]; /* Name of the folder to retrieve */
    BtMapTransparentFlags transparent;
    BtMapRetryFlags retry;
    BtMapCharsetFlags charset;
    uint32 msgLen;
} BT_MAP_PUSH_MESSAGE_OPER_T;

typedef struct _BT_MAP_SET_NOTIFY_REG_OPER_T
{
    BOOLEAN notify_status;
} BT_MAP_SET_NOTIFY_REG_OPER_T;

typedef struct _BT_MAP_SERVER_PARAMS_RX_T
{
    BT_MAP_OP_T oper_type;

    union {
        BT_MAP_GET_FOLDER_LISTING_OPER_T getfolderlisting_oper;
        BT_MAP_GET_MESSAGE_LISTING_OPER_T getmessagelisting_oper;
        BT_MAP_GET_MESSAGE_OPER_T getmessage_oper;
        BT_MAP_SET_MESSAGE_STATUS_OPER_T setmessagestatus_oper;
        BT_MAP_PUSH_MESSAGE_OPER_T pushmessage_oper;
        BT_MAP_SET_NOTIFY_REG_OPER_T setnotifyreg_oper;
    } u;
} BT_MAP_SERVER_PARAMS_RX_T;

typedef struct _BT_MAP_MNS_SERVER_PARAMS_RX_T
{
    BT_MAP_OP_T oper;
    uint8 masInstanceId; /* MAS Instance Identifier */
} BT_MAP_MNS_SERVER_PARAMS_RX_T;

typedef struct _BT_MAP_MNS_SERVER_DATA_IND_T
{
    uint8 *buff; /* Data pointer */
    uint16 len;  /* Length of data */
} BT_MAP_MNS_SERVER_DATA_IND_T;

/* Returns an  push message data. */
typedef BT_STATUS (*MapClient_PushMessageCallBack)(
    uint8 *DataPtr,
    uint32 DataLen);

/* Retrieves the number of records in the phonebook object database. */
typedef uint32 (*MapServer_GetMessageFractionDeliver)(
    char *FolderName);

/* Retrieves the actual number of accessible folders in the current folder of the MSE. */
typedef uint32 (*MapServer_GetFolderListingSize)(void);

/* Returns a parameter info of the Get message listing oper. */
typedef BT_STATUS (*MapServer_GetMessageListingParam)(
    BOOLEAN *NewMessage,       //FALSE,no unread messages,TRUE,unread messages are present
    char *MSETime,             //the local time basis of the MSE and its UTC offset
    uint16 *MessageListingSize //the number of accessible messages in the corresponding folder
                               //fulfilling -if present - the filter parameters as described above
);

/* Return the handle that was assigned by the MSE device to the meassge that wash pushed by the MCE device.*/
typedef char *(*MapServer_PushMessageHandle)(void);

/*Returns the size of the entire current folder listing object.
   0x00 a folder cannot be found, while 0xFFFFFFFF indicates a precondition failed case.*/
typedef uint32 (*MapServer_GetFolderListingObjectSize)(BT_MAP_GET_FOLDER_LISTING_OPER_T getfolderlisting_oper);

/*Returns the size of the entire message listing object.
   0x00 a message cannot be found, while 0xFFFFFFFF indicates a precondition failed case.*/
typedef uint32 (*MapServer_GetMessageListingObjectSize)(BT_MAP_GET_MESSAGE_LISTING_OPER_T getmessagelisting_oper);

/*Returns the size of the entire message entry object.
   0x00 a message cannot be found, while 0xFFFFFFFF indicates a precondition failed case.*/
typedef uint32 (*MapServer_GetMessageObjectSize)(BT_MAP_GET_MESSAGE_OPER_T getmessage_oper);

/* Returns an entire phonebook database of data. */
typedef BT_STATUS (*MapServer_GetFolderListing)(
    BT_MAP_GET_FOLDER_LISTING_OPER_T *getfolderlisting_oper,
    uint8 *DataPtr,
    uint32 DataLen);

/* Returns an entire phonebook database of data. */
typedef BT_STATUS (*MapServer_GetMessageListing)(
    BT_MAP_GET_MESSAGE_LISTING_OPER_T *getmessagelisting_oper,
    uint8 *DataPtr,
    uint32 DataLen);

/* Returns an entire phonebook database of data. */
typedef BT_STATUS (*MapServer_GetMessage)(
    BT_MAP_GET_MESSAGE_OPER_T *getmessage_oper,
    uint8 *DataPtr,
    uint32 DataLen);

/* Sets the folder for the current msg box operation. */
typedef char *(*MapServer_SetFolderInfo)(
    const char *Folder,
    BOOLEAN Backup,
    BOOLEAN Reset);

/* Sets the folder for the current msg box operation. */
typedef void (*MapServer_SetMessageStatus)(void);

/* Sets the folder for the current msg box operation. */
typedef void (*MapServer_UpdateInboxInfo)(void);

typedef struct _MAP_SERVER_CALLBACK_T
{
    //for provide params request
    MapServer_GetMessageFractionDeliver MapGetMessageFractionDeliver;
    MapServer_GetFolderListingSize MapGetFolderListingSize;
    MapServer_GetMessageListingParam MapGetMessageListingParam;
    MapServer_PushMessageHandle MapPushMessageHandle;

    //for object length request
    MapServer_GetFolderListingObjectSize MapGetFolderListingObjectSize;
    MapServer_GetMessageListingObjectSize MapGetMessageListingObjectSize;
    MapServer_GetMessageObjectSize MapGetMessageObjectSize;

    //for data request
    MapServer_SetFolderInfo MapSetFolderInfo;
    MapServer_GetFolderListing MapGetFolderListing;
    MapServer_GetMessageListing MapGetMessageListing;
    MapServer_GetMessage MapGetMessage;
    MapServer_SetMessageStatus MapSetMessageStatus;
    MapServer_UpdateInboxInfo MapUpdateInboxInfo;
} MAP_SERVER_CALLBACK_T;
#endif

#if (defined(BT_AV_CT_SUPPORT) || defined(BT_A2DP_SUPPORT))
/***********************************************/
/*  sharing structures for CT and TG    begin  */

typedef struct _AvrcpPubPlayerSetting
{
    /*attrId    0x01    Equalizer ON/OFF status
     *          0x02    Repeat Mode status
     *          0x03    Shuffle ON/OFF status
     *          0x04    Scan ON/OFF status
     */
    uint8 attrId; /* Media Player Attribute ID */
    uint8 value;  /*Media Player Attribute value*/
} AvrcpPubPlayerSetting;

typedef struct _AvrcpPubTrackStruct
{
    /* The most significant 32 bits of the track index information.  */
    uint32 msU32;

    /* The least significant 32 bits of the track index information.  */
    uint32 lsU32;

} AvrcpPubTrackStruct;
/*  sharing structures for CT and TG    end    */
/***********************************************/
#endif

#ifdef BT_AV_CT_SUPPORT
#define MAX_TG_PLAYER_STRING_LEN 80

#if AVRCP_BROWSING_CONTROLLER == XA_ENABLED
#define AVRCP_NUM_MEDIA_ATTRIBUTES 7
#else
#define AVRCP_NUM_MEDIA_ELEMENTS 7
#endif
#define AVRCP_NUM_PLAYER_ATTRIBUTES 4
#define AVRCP_NUM_PLAYER_SETTINGS 4

#if AVRCP_BROWSING_CONTROLLER == XA_ENABLED
#define AVRCP_MAX_FOLDER_DEPTH 10
#define AVRCP_MAX_FOLDER_ITEM 10
#endif

typedef AvrcpPubPlayerSetting AvrcpCtPlayerSetting;
typedef AvrcpPubTrackStruct AvrcpCtTrackStruct;

typedef struct _AvrcpCtPlayStatus
{
    uint32 Length;   /*media length, in milliseconds, 0xFFFFFFFF stands for not support in TG*/
    uint32 Position; /*media position, in milliseconds, 0xFFFFFFFF stands for not support in TG*/

    /*media 0x00 : STOPPED
	 *		0x01 : PLAYING
	 *		0x02 : PAUSED
	 *		0x03: FWD_SEEK
	 *		0x04: REV_SEEK
	 *		0xFF : ERROR
	 */
    uint8 mediaStatus;
} AvrcpCtPlayStatus;

typedef struct _AvrcpCtElementAttributes
{
    /* The number of elements returned */
    uint8 numIds;

    /* An array of element value text information */
    struct
    {
        /* attrId	0x1	Title
		 * 			0x2	Artist Name
		 * 			0x3	Album Name
		 * 			0x4	Track Number
		 * 			0x5	Total Nunmber of Tracks
		 * 			0x6	Genre
		 * 			0x7	Playing Time(milliseconds)
		 */
        uint32 attrId;
        uint16 charSet;
        uint16 length;
        uint8 string[MAX_TG_PLAYER_STRING_LEN];
    }
#if AVRCP_BROWSING_CONTROLLER == XA_ENABLED
    txt[AVRCP_NUM_MEDIA_ATTRIBUTES];
#else
    txt[AVRCP_NUM_MEDIA_ELEMENTS];
#endif
} AvrcpCtElementAttributes;

typedef struct _AvrcpCtAttrIDValues
{
    /*attrId	0x01	Equalizer ON/OFF status
	 *			0x02	Repeat Mode status
	 *			0x03	Shuffle ON/OFF status
	 *			0x04	Scan ON/OFF status
     */
    uint8 attrId;

    /* The number of supported value IDs. */
    uint8 numIds;

    /* An array of value ID (1 byte each). */
    uint8 ids[AVRCP_NUM_PLAYER_ATTRIBUTES];
} AvrcpCtAttrIDValues;

typedef struct _AvrcpCtAttrStrings
{

    /* The number of attribute IDs returned. */
    uint8 numIds;

    /* An array of attribute text information */
    struct
    {
        /*attrId	0x01	Equalizer ON/OFF status
		 *			0x02	Repeat Mode status
	 	 *			0x03	Shuffle ON/OFF status
	 	 *			0x04	Scan ON/OFF status
    	 */
        uint8 attrId;
        uint16 charSet;
        uint8 length;
        char string[MAX_TG_PLAYER_STRING_LEN];
    } txt[AVRCP_NUM_PLAYER_ATTRIBUTES];
} AvrcpCtAttrStrings;

typedef struct _AvrcpCtSettingIDStrings
{
    /*attrId	0x01	Equalizer ON/OFF status
		 *			0x02	Repeat Mode status
	 	 *			0x03	Shuffle ON/OFF status
	 	 *			0x04	Scan ON/OFF status
    	 */
    uint8 attrId;

    /* The number of settings returned */
    uint8 numSettings;

    /* An array of value text information */
    struct
    {
        uint8 valueId;
        uint16 charSet;
        uint8 length;
        char string[MAX_TG_PLAYER_STRING_LEN];
    } txt[AVRCP_NUM_PLAYER_SETTINGS];

} AvrcpCtSettingIDStrings;

typedef struct _AvrcpCtPanelOperation
{
    /* op￡o0x0041	volume_up 
	 *	   0x0042	volume_down 
	 *	   0x0043	mute 
	 *	   0x0044	play 
	 *	   0x0045	stop
	 *	   0x0046	pause
	 *	   0x0047	record 
	 *	   0x0048	rewind 
	 *	   0x0049	fast forward 
	 *	   0x004a	eject 
	 *	   0x004b	forward 
	 *	   0x004c	backward
	 */
    uint16 Op;
    BOOLEAN press; /* ￡otrue stands for pressed, false stands for release*/
} AvrcpCtPanelOperation;

#if AVRCP_BROWSING_CONTROLLER == XA_ENABLED

typedef struct _AvrcpCtAddrPlayer
{
    uint16 playerId;
    uint16 uidCounter;
} AvrcpCtAddrPlayer;

typedef struct _AvrcpCtSearch
{
    uint16 uidCounter; /* Current UID counter */
    uint32 numItems;   /* Number of items found in the search */
} AvrcpCtSearch;

typedef struct _AvrcpCtFolder
{
    uint16 nameLen;
    char *name;
} AvrcpCtFolder; /* List of folder names */

typedef struct _AvrcpCtBrwsPlayer
{
    uint16 uidCounter;     /* Current UID counter */
    uint32 numItems;       /* Number of items in the current path */
    uint16 charSet;        /* Character set used by the player */
    uint8 fDepth;          /* Number of folder length/name pairs to follow */
    AvrcpCtFolder *folder; /* List of folder names */
} AvrcpCtBrwsPlayer;

typedef struct _AvrcpCtMediaPlayerItem
{
    /* Used Internally */
    uint16 playerId;

    /* Major Player Type:	AVRCP_MAJ_PLAYER_AUDIO      0x01
	 *						AVRCP_MAJ_PLAYER_VIDEO      0x02
	 *						AVRCP_MAJ_PLAYER_AUD_BCAST  0x04
	 *						AVRCP_MAJ_PLAYER_VID_BCAST  0x08 
	 */
    uint8 majorType;

    /* Player Sub Type:	AVRCP_PLAYER_SUB_AUDIO_BOOK  0x00000001
	 *					AVRCP_PLAYER_SUB_PODCAST     0x00000002
	 */
    uint32 subType;

    /* Current media play status:	AVRCP_MEDIA_STOPPED       0x00
	 *								AVRCP_MEDIA_PLAYING       0x01
	 *								AVRCP_MEDIA_PAUSED        0x02
	 *								AVRCP_MEDIA_FWD_SEEK      0x03
	 *								AVRCP_MEDIA_REV_SEEK      0x04
	 *								AVRCP_MEDIA_ERROR         0xFF
	 */
    uint8 mediaStatus;

    /* Player feature bitmask:	AVRCP_BRWS_0_FEAT_SELECT              0x01
	 *							AVRCP_BRWS_0_FEAT_UP                  0x02
	 *							AVRCP_BRWS_0_FEAT_DOWN                0x04
	 *							AVRCP_BRWS_0_FEAT_LEFT                0x08
	 *							AVRCP_BRWS_0_FEAT_RIGHT               0x10
	 *							AVRCP_BRWS_0_FEAT_RIGHT_UP            0x20
	 *							AVRCP_BRWS_0_FEAT_RIGHT_DOWN          0x40
	 *							AVRCP_BRWS_0_FEAT_LEFT_UP             0x80
	 *
	 *							AVRCP_BRWS_1_FEAT_LEFT_DOWN           0x01
	 *							AVRCP_BRWS_1_FEAT_ROOT_MENU           0x02
	 *							AVRCP_BRWS_1_FEAT_SETUP_MENU          0x04
	 *							AVRCP_BRWS_1_FEAT_CONTENTS_MENU       0x08
	 *							AVRCP_BRWS_1_FEAT_FAVORITE_MENU       0x10
	 *							AVRCP_BRWS_1_FEAT_EXIT                0x20
	 *							AVRCP_BRWS_1_FEAT_0                   0x40
	 *							AVRCP_BRWS_1_FEAT_1                   0x80

	 *							AVRCP_BRWS_2_FEAT_2                   0x01
	 *							AVRCP_BRWS_2_FEAT_3                   0x02
	 *							AVRCP_BRWS_2_FEAT_4                   0x04
	 *							AVRCP_BRWS_2_FEAT_5                   0x08
	 *							AVRCP_BRWS_2_FEAT_6                   0x10
	 *							AVRCP_BRWS_2_FEAT_7                   0x20
	 *							AVRCP_BRWS_2_FEAT_8                   0x40
	 *							AVRCP_BRWS_2_FEAT_9                   0x80

	 *							AVRCP_BRWS_3_FEAT_DOT                 0x01
	 *							AVRCP_BRWS_3_FEAT_ENTER               0x02
	 *							AVRCP_BRWS_3_FEAT_CLEAR               0x04
	 *							AVRCP_BRWS_3_FEAT_CHNL_UP             0x08
	 *							AVRCP_BRWS_3_FEAT_CHNL_DOWN           0x10
	 *							AVRCP_BRWS_3_FEAT_PREV_CHNL           0x20
	 *							AVRCP_BRWS_3_FEAT_SOUND_SEL           0x40
	 *							AVRCP_BRWS_3_FEAT_INPUT_SEL           0x80

	 *							AVRCP_BRWS_3_FEAT_DISPLAY_INFO        0x01
	 *							AVRCP_BRWS_4_FEAT_HELP                0x02
	 *							AVRCP_BRWS_4_FEAT_PG_UP               0x04
	 *							AVRCP_BRWS_4_FEAT_PG_DOWN             0x08
	 *							AVRCP_BRWS_4_FEAT_POWER               0x10
	 *							AVRCP_BRWS_4_FEAT_VOL_UP              0x20
	 *							AVRCP_BRWS_4_FEAT_VOL_DOWN            0x40
	 *							AVRCP_BRWS_4_FEAT_MUTE                0x80

	 *							AVRCP_BRWS_5_FEAT_PLAY                0x01
	 *							AVRCP_BRWS_5_FEAT_STOP                0x02
	 *							AVRCP_BRWS_5_FEAT_PAUSE               0x04
	 *							AVRCP_BRWS_5_FEAT_RECORD              0x08
	 *							AVRCP_BRWS_5_FEAT_REWIND              0x10
	 *							AVRCP_BRWS_5_FEAT_FF                  0x20
	 *							AVRCP_BRWS_5_FEAT_EJECT               0x40
	 *							AVRCP_BRWS_5_FEAT_FORWARD             0x80

	 *							AVRCP_BRWS_6_FEAT_BACKWARD            0x01
	 *							AVRCP_BRWS_6_FEAT_ANGLE               0x02
	 *							AVRCP_BRWS_6_FEAT_SUBPIC              0x04
	 *							AVRCP_BRWS_6_FEAT_F1                  0x08
	 *							AVRCP_BRWS_6_FEAT_F2                  0x10
	 *							AVRCP_BRWS_6_FEAT_F3                  0x20
	 *							AVRCP_BRWS_6_FEAT_F4                  0x40
	 *							AVRCP_BRWS_6_FEAT_F5                  0x80

	 *							AVRCP_BRWS_7_FEAT_VENDOR_UNIQUE       0x01
	 *							AVRCP_BRWS_7_FEAT_GROUP_NAV           0x02
	 *							AVRCP_BRWS_7_FEAT_ADVANCED_CTRL       0x04
	 *							AVRCP_BRWS_7_FEAT_BROWSING            0x08
	 *							AVRCP_BRWS_7_FEAT_SEARCHING           0x10
	 *							AVRCP_BRWS_7_FEAT_ADD_TO_PLAYING      0x20
	 *							AVRCP_BRWS_7_FEAT_UIDS_UNIQUE         0x40
	 *							AVRCP_BRWS_7_FEAT_BRWS_WHEN_ADDRESSED 0x80

	 *							AVRCP_BRWS_8_SEARCH_WHEN_ADDRESSED    0x01
	 *							AVRCP_BRWS_8_NOW_PLAYING              0x02
	 *							AVRCP_BRWS_8_UID_PERSISTENT           0x04
	 */
    uint8 features[16];

    /* Displayable character set */
    uint16 charSet;

    /* Player name length */
    uint16 nameLen;

    /* Player name */
    char name[MAX_TG_PLAYER_STRING_LEN];
} AvrcpCtMediaPlayerItem;

typedef struct _AvrcpCtFolderItem
{

    /* The UID of this media element */
    uint8 uid[8];

    /* Folder Type:		AVRCP_FOLDER_TYPE_MIXED     0x00
	 *					AVRCP_FOLDER_TYPE_TITLES    0x01
	 *					AVRCP_FOLDER_TYPE_ALBUMS    0x02
	 *					AVRCP_FOLDER_TYPE_ARTISTS   0x03
	 *					AVRCP_FOLDER_TYPE_GENRES    0x04
	 *					AVRCP_FOLDER_TYPE_PLAYLISTS 0x05
	 *					AVRCP_FOLDER_TYPE_YEARS     0x06
	 */
    uint8 folderType;

    /* Is Playable Flag:	AVRCP_IS_NOT_PLAYABLE  0x00
	 *						AVRCP_IS_PLAYABLE      0x01
	 */
    uint8 isPlayable;

    /* Character Set */
    uint16 charSet;

    /* The name length of this media element */
    uint16 nameLen;

    /* The name of this media element */
    char name[MAX_TG_PLAYER_STRING_LEN];

} AvrcpCtFolderItem;

typedef struct _AvrcpCtMediaElementItem
{

    /* The UID of this media element */
    uint8 uid[8];

    /* Media type (audio or video):		AVRCP_MEDIA_TYPE_AUDIO 0x00
	 *									AVRCP_MEIDA_TYPE_VIDEO 0x01
	 */
    uint8 mediaType;

    /* Character Set */
    uint16 charSet;

    /* The name length of this media element */
    uint16 nameLen;

    /* The name of this media element */
    char *name;

    /* Number of media attributes */
    uint8 numAttrs;

    /* Media Attributes */
    struct
    {

        /* Attribute ID:	AVRCP_MEDIA_ATTR_TITLE       0x00000001
		 *					AVRCP_MEDIA_ATTR_ARTIST      0x00000002
		 *					AVRCP_MEDIA_ATTR_ALBUM       0x00000003
		 *					AVRCP_MEDIA_ATTR_TRACK       0x00000004
		 *					AVRCP_MEDIA_ATTR_NUM_TRACKS  0x00000005
		 *					AVRCP_MEDIA_ATTR_GENRE       0x00000006
		 *					AVRCP_MEDIA_ATTR_DURATION    0x00000007
		 */
        uint32 attrId;

        /* Character Set */
        uint16 charSet;

        /* Attribute Length */
        uint16 valueLen;

        /* Attribute Name */
        uint8 value[MAX_TG_PLAYER_STRING_LEN];

    } attrs[AVRCP_NUM_MEDIA_ATTRIBUTES];
} AvrcpCtMediaElementItem;

typedef struct _AvrcpCtMediaItem
{
    /*Item Type:AVRCP_ITEM_MEDIA_PLAYER  0x01
	 *			AVRCP_ITEM_FOLDER        0x02
	 *			AVRCP_ITEM_MEDIA_ELEMENT 0x03
	 */
    uint8 itemType;

    /* The Media Item */
    union {
        AvrcpCtMediaPlayerItem player;
        AvrcpCtFolderItem folder;
        AvrcpCtMediaElementItem element;
    } item;

} AvrcpCtMediaItem;

typedef struct _AvrcpCtFldrItems
{
    uint16 uidCounter; /* Current UID counter */
    uint32 numItems;   /* Number of items in the current path */
    AvrcpCtMediaItem *mediaItem;

} AvrcpCtFldrItems;

#endif /* AVRCP_BROWSING_CONTROLLER == XA_ENABLED */

#endif /* BT_AV_CT_SUPPORT */

#ifdef BT_A2DP_SUPPORT
typedef AvrcpPubPlayerSetting AvrcpTgPlayerSetting;
typedef AvrcpPubTrackStruct AvrcpTgTrackStruct;

typedef struct _AvrcpTgMediaInfo
{
    /* attrId    0x1    Title
     *           0x2    Artist Name
     *           0x3    Album Name
     *           0x4    Track Number
     *           0x5    Total Nunmber of Tracks
     *           0x6    Genre
     *           0x7    Playing Time(milliseconds)
     */
    uint32 attrId;      /* Media element attribute ID */
    const char *string; /* The media element value text */
    uint16 length;      /* Length of the media element value text */
} AvrcpTgMediaInfo;

typedef struct _AvrcpTgEqString
{

    /* Index 0 == Off, Index 1 == On */
    const char *string[2];

    uint8 len[2];
} AvrcpTgEqString;

typedef struct _AvrcpTgRepeatString
{

    /* Index 0 == Off, Index 1 == Single, Index 2 == All, Index 3 == Group */
    const char *string[4];

    uint8 len[4];
} AvrcpTgRepeatString;

typedef struct _AvrcpTgShuffleString
{

    /* Index 0 == Off, Index 1 == All, Index 2 == Group */
    const char *string[3];

    uint8 len[3];
} AvrcpTgShuffleString;

typedef struct _AvrcpTgScanString
{

    /* Index 0 == Off, Index 1 == All, Index 2 == Group */
    const char *string[3];

    uint8 len[3];
} AvrcpTgScanString;

typedef struct _AvrcpTgPlayerStrings
{

    const char *attrString; /* Media Player Attribute Text */
    uint8 attrLen;          /* Length of the Attribute Text */

    union {
        AvrcpTgEqString eq;           /* Equalizer status */
        AvrcpTgRepeatString repeat;   /* Repeat mode status */
        AvrcpTgShuffleString shuffle; /* Shuffle mode status */
        AvrcpTgScanString scan;       /* Scan mode status */
    } setting;
} AvrcpTgPlayerStrings;
#endif /* BT_A2DP_SUPPORT */

typedef uint8 BT_HFU_GW_HOLD_FEATUERS_T;
/*---------------------------------------------------------------------------
 * BT_HFU_GW_HOLD_FEATUERS_T type
 *
 *  Bit mask specifying the gateway's 3-Way calling (hold) feature set.
 *  The service connection will be limited to the features advertised by the
 *  profile.
 */

/** Releases all held calls or sets User Determined User Busy
 * (UDUB) for a waiting call.
 */
#define BT_HFU_GW_HOLD_RELEASE_HELD_CALLS 0x01

/** Releases all active calls (if any exist) and accepts the other
 * (held or waiting) call.
 */
#define BT_HFU_GW_HOLD_RELEASE_ACTIVE_CALLS 0x02

/** Releases a specific call. */
#define BT_HFU_GW_HOLD_RELEASE_SPECIFIC_CALL 0x04

/** Places all active calls (if any exist) on hold and accepts the
 * other (held or waiting) call.
 */
#define BT_HFU_GW_HOLD_HOLD_ACTIVE_CALLS 0x08

/** Places a specific call on hold. */
#define BT_HFU_GW_HOLD_HOLD_SPECIFIC_CALL 0x10

/** Adds a held call to the conversation.
 */
#define BT_HFU_GW_HOLD_ADD_HELD_CALL 0x20

/** Connects the two calls and disconnects the AG from
 * both calls (Explicit Call Transfer).
 */
#define BT_HFU_GW_HOLD_CALL_TRANSFER 0x40

/* End of HfGwHoldFeatures */

#ifdef BT_SAP_SUPPORT
typedef struct _BT_SAP_TRANSFER_APDU_REQ_T
{
    BOOLEAN apdu7816Type;
    uint16 apduLength;
    uint8 *commandApdu;
} BT_SAP_TRANSFER_APDU_REQ_T;

typedef struct _BT_SAP_CONNECT_T
{
    uint16 maxMsgSize;
} BT_SAP_CONNECT_T;

typedef uint8 SapCardStatus;
#define SAP_CARD_STATUS_UNKNOWN_ERROR 0x00
#define SAP_CARD_STATUS_CARD_RESET 0x01
#define SAP_CARD_STATUS_CARD_NOT_ACCESSIBLE 0x02
#define SAP_CARD_STATUS_CARD_REMOVED 0x03
#define SAP_CARD_STATUS_CARD_INSERTED 0x04
#define SAP_CARD_STATUS_CARD_RECOVERED 0x05

typedef struct _BT_SAP_STATUS_IND_T
{
    SapCardStatus status;
} BT_SAP_STATUS_IND_T;

typedef uint8 SapDisconnectType;
#define SAP_DISCONNECT_TYPE_GRACEFUL 0x00
#define SAP_DISCONNECT_TYPE_IMMEDIATE 0x01

typedef struct _BT_SAP_DISCONNECT_IND_T
{
    SapDisconnectType disconnectType;
} BT_SAP_DISCONNECT_IND_T;

typedef uint8 SapResultCode;
#define SAP_RESULT_OK 0x00
#define SAP_RESULT_ERROR_NO_REASON 0x01
#define SAP_RESULT_ERROR_CARD_NOT_ACCESSIBLE 0x02
#define SAP_RESULT_ERROR_CARD_POWERED_OFF 0x03
#define SAP_RESULT_ERROR_CARD_REMOVED 0x04
#define SAP_RESULT_ERROR_CARD_POWERED_ON 0x05
#define SAP_RESULT_ERROR_DATA_NOT_AVAILABLE 0x06
#define SAP_RESULT_ERROR_NOT_SUPPORTED 0x07
#define SAP_RESULT_ERROR_RESPONSE 0x08

typedef struct _BT_SAP_TRANSFER_APDU_CNF_T
{
    SapResultCode resultCode;
    uint16 responseLength;
    uint8 *responseApdu;
} BT_SAP_TRANSFER_APDU_CNF_T;

typedef struct _BT_SAP_TRANSFER_ATR_CNF_T
{
    uint8 resultCode;
    uint16 atrLength;
    uint8 *atr;
} BT_SAP_TRANSFER_ATR_CNF_T;

typedef struct _BT_SAP_POWER_SIM_OFF_CNF_T
{
    SapResultCode resultCode;
} BT_SAP_POWER_SIM_OFF_CNF_T;

typedef struct _BT_SAP_POWER_SIM_ON_CNF_T
{
    SapResultCode resultCode;
} BT_SAP_POWER_SIM_ON_CNF_T;

typedef struct _BT_SAP_RESET_SIM_CNF_T
{
    SapResultCode resultCode;
} BT_SAP_RESET_SIM_CNF_T;

typedef struct _BT_SAP_TRANSFER_READER_STATUS_CNF_T
{
    SapResultCode resultCode;
    SapCardStatus cardReaderStatus;
} BT_SAP_TRANSFER_READER_STATUS_CNF_T;

typedef struct _BT_SAP_SET_TRANSFER_PROTOCOL_CNF_T
{
    SapResultCode resultCode;
} BT_SAP_SET_TRANSFER_PROTOCOL_CNF_T;

#endif

typedef struct
{
    int8 current_rssi;
} BT_READ_RSSI_T;

typedef enum
{
    BT_RSSI_LOW_ALERT,
    BT_RSSI_HIGH_ALERT
} BT_RSSI_ALERT_TYPE_T;

typedef struct
{
    BT_RSSI_ALERT_TYPE_T low_or_high_alert;
    int8 current_rssi;
} BT_RSSI_ALERT_T;

typedef struct _BT_AT_SPP_DATA
{
    UINT8 *buf;
    UINT16 dataLen;
} BT_AT_SPP_DATA;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/******************************************************************************
 * Management functions
 ******************************************************************************/
extern BT_CALLBACK_STACK g_bt_callback_stack_func;

/******************************************************************************
 * FUNCTION:		bt_register_at_callback_func
 * 
 * DESCRIPTION:
 *
 * PARAMETERS:
 *		func - 
 *
 * RETURNS:
 *
 * MESSAGES:
 * 		None.
 ******************************************************************************/

void bt_register_at_callback_func(BT_CALLBACK_STACK callback);

/******************************************************************************
 * FUNCTION:		BT_Init
 * 
 * DESCRIPTION:
 *
 * PARAMETERS:
 *		func - 
 *
 * RETURNS:
 *
 * MESSAGES:
 * 		None.
 ******************************************************************************/
BT_STATUS BT_Init(BT_CALLBACK callback);

/******************************************************************************
 * FUNCTION:		BT_Half_init
 * 
 * DESCRIPTION:
 *
 * PARAMETERS:
 *		func - 
 *
 * RETURNS:
 *
 * MESSAGES:
 * 		None.
 ******************************************************************************/
BT_STATUS BT_Half_init(void);

/******************************************************************************
 * FUNCTION:		BT_Deinit
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *		None.
 *
 * RETURNS:
 *
 * MESSAGES:
 *		None.
 ******************************************************************************/
BT_STATUS BT_Deinit(void);

/******************************************************************************
 * FUNCTION:        BT_TestInit
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *      None.
 *
 * RETURNS:
 *
 * MESSAGES:
 *      None.
 ******************************************************************************/
BT_STATUS BT_TestInit(void);

/******************************************************************************
 * FUNCTION:		BT_Start
 * 
 * DESCRIPTION:
 *
 * PARAMETERS:
 *		None.
 *
 * RETURNS:
 *		BT_SUCCESS -
 *		BT_PENDING -
 *
 * MESSAGES:
 *		ID_STATUS_ME_BT_ON_CFM
 *			status = BT_SUCCESS/BT_ERROR
 ******************************************************************************/
BT_STATUS BT_Start(void);

/******************************************************************************
 * FUNCTION:		BT_Stop
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *		None.
 *
 * RETURNS:
 *		BT_SUCCESS -
 *		BT_PENDING -
 *
 * MESSAGES:
 *		ID_STATUS_ME_BT_OFF_CFM
 *			status = BT_SUCCESS/BT_ERROR
 ******************************************************************************/
BT_STATUS BT_Stop(void);

/******************************************************************************
 * FUNCTION:		BT_GetState
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *		None.
 *
 * RETURNS:
 *		TRUE  -
 *		FALSE -
 *
 * MESSAGES:
 *		None.
 ******************************************************************************/
BOOLEAN BT_GetState(void);

/******************************************************************************
 * FUNCTION:        BT_IsEnabled
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *      None.
 *
 * RETURNS:
 *      TRUE  - In case if BT is ON.
 *      FALSE - In case if BT is OFF.
 *
 * MESSAGES:
 *      None.
 ******************************************************************************/
BOOLEAN BT_IsEnabled(void);

/******************************************************************************
 * FUNCTION:		BT_SearchDevice
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *		service_type - 
 *
 * RETURNS:
 *		BT_PENDING - 
 *
 * MESSAGES:
 *		ID_STATUS_DEV_INQ_RES
 *			data.dev_info
 *		
 *		ID_STATUS_DEV_INQ_ADDR_RES
 *			data.dev_info
 *
 *		ID_STATUS_DEV_INQ_CMP
 *
 *		ID_STATUS_DEV_INQ_CANCEL
 ******************************************************************************/
BT_STATUS BT_SearchDevice(uint32 service_type);

/******************************************************************************
 * FUNCTION:		BT_CancelSearch
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 * 		None.
 *
 * RETURNS:
 *		BT_PENDING - 
 *
 * MESSAGES:
 *		ID_STATUS_DEV_INQ_CANCEL
 ******************************************************************************/
BT_STATUS BT_CancelSearch(void);

/******************************************************************************
 * FUNCTION:		BT_GetSearchDeviceCount
 *
 * DESCRIPTION: only for ATC Test
 *
 * PARAMETERS:
 * 		None.
 *
 * RETURNS:
 *		the number of devices which is searched
 *
 * MESSAGES:
 *		
 ******************************************************************************/

uint32 BT_GetSearchDeviceCount(void);

/******************************************************************************
 * FUNCTION:        BT_PairDevice
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *      addr - 
 *
 * RETURNS:
 *      BT_PENDING - 
 *
 * MESSAGES:
 *      ID_STATUS_PAIR_COMPLETE
 *          status = BT_SUCCESS / BT_ERROR
 *
 *      ID_STATUS_DEV_PIN_REQ
 *          data.dev_info
 ******************************************************************************/
BT_STATUS BT_PairDevice(const BT_ADDRESS *addr);

/******************************************************************************
 * FUNCTION:        BT_InputPairPin
 * 
 * DESCRIPTION:
 *
 * PARAMETERS:
 *      addr   - 
 *      pin    -
 *      length -
 *
 * RETURNS:
 *      BT_PENDING - 
 *
 * MESSAGES:
 *      ID_STATUS_PAIR_COMPLETE
 *          status = BT_SUCCESS / BT_ERROR
 ******************************************************************************/
BT_STATUS BT_InputPairPin(const BT_ADDRESS *addr, const uint8 *pin, int length);

/******************************************************************************
 * FUNCTION:        BT_CancelPair
 * 
 * DESCRIPTION:
 *
 * PARAMETERS:
 *      addr - 
 *
 * RETURNS:
 *      BT_PENDING - 
 *
 * MESSAGES:
 *      ID_STATUS_PAIR_COMPLETE
 *          status = BT_ERROR
 ******************************************************************************/
BT_STATUS BT_CancelPair(const BT_ADDRESS *addr);

/******************************************************************************
 * FUNCTION:        BT_DisconnectCurrentLink
 * 
 * DESCRIPTION:
 *
 * PARAMETERS:
 *      addr - 
 *
 * RETURNS:
 *      BT_PENDING - in process
 *      BT_ERROR - bluetooth not on or hfu connection not started
 *      BT_DEVICE_NOT_FOUND - the addr provided is not in use
 *
 * MESSAGES:
 *
 ******************************************************************************/
BT_STATUS BT_DisconnectCurrentLink(const BT_ADDRESS *addr);

/******************************************************************************
 * FUNCTION:        BT_SetDeviceAuthorisation
 * 
 * DESCRIPTION:
 *
 * PARAMETERS:
 *      is_authorised - 
 *
 * RETURNS:
 *      BT_SUCESS - 
 *      BT_ERROR
 *
 * MESSAGES:
 *
 ******************************************************************************/
BT_STATUS BT_SetDeviceAuthorisation(const BT_ADDRESS *addr_ptr,
                                    const BOOLEAN is_authorised);
/******************************************************************************
 * FUNCTION:        BT_SetDeviceAuthorisation
 * 
 * DESCRIPTION:
 *
 * PARAMETERS:
 *      is_authorised - 
 *
 * RETURNS:
 *      BT_SUCESS - 
 *      BT_ERROR
 * MESSAGES:
 *
 ******************************************************************************/
BT_STATUS BT_GetDeviceAuthorisation(const BT_ADDRESS *addr_ptr,
                                    BOOLEAN *is_authorised_ptr);

BT_STATUS BT_AcceptAuthorisation(const BT_ADDRESS *addr_ptr, const BOOLEAN is_authorised);
/******************************************************************************
 * FUNCTION:        BT_GetPairedDeviceCount
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *      service_type -
 *
 * RETURNS:
 *      The count of paired devices of the specified service type.
 *
 * MESSAGES:
 *      None.
 ******************************************************************************/
int BT_GetPairedDeviceCount(uint32 service_type);

/******************************************************************************
 * FUNCTION:        BT_GetPairedDeviceInfo
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *      service_type -
 *      index        -
 *      info         -
 *
 * RETURNS:
 *
 * MESSAGES:
 *      None.
 ******************************************************************************/
BT_STATUS BT_GetPairedDeviceInfo(uint32 service_type, int index, BT_DEVICE_INFO *info);

/******************************************************************************
 * FUNCTION:        BT_GetPairedDeviceName
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *      addr - 
 *      name -
 *
 * RETURNS:
 *
 * MESSAGES:
 *      None.
 ******************************************************************************/
BT_STATUS BT_GetPairedDeviceName(const BT_ADDRESS *addr, uint16 *name);

/******************************************************************************
 * FUNCTION:        BT_SetPairedDeviceName
 * 
 * DESCRIPTION:
 *
 * PARAMETERS:
 *      addr - 
 *      name -
 *
 * RETURNS:
 *
 * MESSAGES:
 *      None.
 ******************************************************************************/
BT_STATUS BT_SetPairedDeviceName(const BT_ADDRESS *addr, const uint16 *name);

/******************************************************************************
 * FUNCTION:       BT_GetPairedDeviceService
 * 
 * DESCRIPTION:
 *
 * PARAMETERS:
 *      addr - 
 *
 * RETURNS:
 *      Service list of the specified device.
 *
 * MESSAGES:
 *      None.
 ******************************************************************************/
uint32 BT_GetPairedDeviceService(const BT_ADDRESS *addr);

uint32 BT_GetPairedDeviceServiceExt(const BT_ADDRESS *addr);

BT_STATUS BT_GetInquiryDeviceService(const BT_ADDRESS *addr, uint32 *service);

/******************************************************************************
 * FUNCTION:       BT_GetPairedDeviceService
 * 
 * DESCRIPTION:
 *
 * PARAMETERS:
 *      addr - 
 *
 * RETURNS:
 *      BT_PENDING -
 *
 * MESSAGES:
 *      
 ******************************************************************************/
BT_STATUS BT_RefreshPairedDeviceService(const BT_ADDRESS *addr);

/******************************************************************************
 * FUNCTION:        BT_RemovePairedDevice
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *      addr - 
 *
 * RETURNS:
 *      BT_PENDING -
 *
 * MESSAGES:
 *      ID_STATUS_PAIR_DELETED
 *          status = BT_SUCCESS / BT_ERROR
 ******************************************************************************/
BT_STATUS BT_RemovePairedDevice(const BT_ADDRESS *addr);

/******************************************************************************
 * FUNCTION:        BT_SetDefaultVisible
 *
 * DESCRIPTION:     This function is used for the first time to set the 
 *                  visible
 * PARAMETERS:      0:BT_NONE_ENABLE 1:BT_INQUIRY_ENABLE 2:BT_PAGE_ENABLE 3:BT_PAGE_AND_INQUIRY_ENABLE
 *      None.
 *
 * RETURNS:
 *      TRUE  -
 *      FALSE -
 *
 * MESSAGES:
 *      None.
 ******************************************************************************/
void BT_SetDefaultVisible(uint32 visible);

/******************************************************************************
 * FUNCTION:        BT_SetVisibility
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *      visible -
 *
 * RETURNS:
 *      BT_PENDING - 
 * 
 * MESSAGES:
 *      ID_STATUS_DISCOVER_REQ
 *          status
 *
 *      ID_STATUS_UN_DISCOVER_REQ
 *          status
 ******************************************************************************/
BT_STATUS BT_SetVisibility(BT_SCAN_E visible);

/******************************************************************************
 * FUNCTION:        BT_GetVisibility
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *      None.
 *
 * RETURNS:
 *      TRUE  -
 *      FALSE -
 *
 * MESSAGES:
 *      None.
 ******************************************************************************/
BT_SCAN_E BT_GetVisibility(void);
/******************************************************************************
 * FUNCTION:        BT_SetVisibilityUni
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *      visible -
 *
 * RETURNS:
 *      BT_PENDING - 
 * 
 * MESSAGES:
 *      ID_STATUS_DISCOVER_REQ
 *          status
 *
 *      ID_STATUS_UN_DISCOVER_REQ
 *          status
 ******************************************************************************/
BT_STATUS BT_SetVisibilityUni(BT_SCAN_E visible);
/******************************************************************************
 * FUNCTION:        BT_GetVisibilityUni
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *      None.
 *
 * RETURNS:
 *      TRUE  -
 *      FALSE -
 *
 * MESSAGES:
 *      None.
 ******************************************************************************/
BT_SCAN_E BT_GetVisibilityUni(void);

/******************************************************************************
 * FUNCTION:        BT_SetLocalName
 * 
 * DESCRIPTION:
 *
 * PARAMETERS:
 *      name - 
 * 
 * RETURNS:
 *      
 * MESSAGES:
 *      None.
 ******************************************************************************/
BT_STATUS BT_SetLocalName(const uint16 *name);

/******************************************************************************
 * FUNCTION:        BT_GetLocalName
 * 
 * DESCRIPTION:
 *
 * PARAMETERS:
 *      name -
 *
 * RETURNS:
 *
 * MESSAGES:
 *      None.
 ******************************************************************************/
BT_STATUS BT_GetLocalName(uint16 *name);
BT_STATUS BT_GetLibVersion(uint8 *ver_str);

BT_STATUS BT_SetSSPDebugMode(BOOLEAN on_off);
BOOLEAN BT_GetSSPDebugMode(void);

// to indicate layer1 do not shift PLL
typedef void (*BT_LAYER1_CALLBACK)(BOOLEAN bt_on_off);
void BT_LAYER1_Reg_Callback(BT_LAYER1_CALLBACK callback);

BT_STATUS BT_CIEVCommandIndicator(uint32 ciev_command, uint32 command_status); //mdy by qing.yu@for cr238605 cr238609

void BT_ClccCommandSend(uint8 idx, uint8 dir, uint8 status,
                        uint8 mode, uint8 mpty, uint8 *number, uint8 *call_type); //mdy by qing.yu@for cr

BT_STATUS BT_SetNameOfNetOperator(uint8 *name); //mdy by qing.yu@for cr238585

void BT_ATStatusSend(BOOLEAN status); //mdy by qing.yu@for cr238786 cr238787

void BT_SetLocalNumber(uint8 *number); //mdy by qing.yu@for cr238867

void BT_FreeEirBuffer(void);

/******************************************************************************
 * File transfer functions
 ******************************************************************************/

/******************************************************************************
 * FUNCTION:        Send File with opp function
 *
 * PARAMETERS:
 *      addr      -
 *      dev       -
 *      file_path -
 *
 * RETURNS:
 *      BT_PENDING -
 *
 * MESSAGES:
 *      
 ******************************************************************************/
BT_STATUS BT_OppSendFile(const BT_ADDRESS *addr, const uint16 *file_path);
BT_STATUS BT_OppSendFileExt(const BT_ADDRESS *addr, const uint16 *file_path, uint32 file_type);
BT_STATUS BT_CancelOppSendFile(void);
BT_STATUS BT_OppReceiveFile(const uint16 *file_path);
BT_STATUS BT_CancelOppReceiveFile(void);
BT_STATUS BT_OpsConnect(BOOLEAN is_enable);
BT_STATUS BT_FtsConnect(BOOLEAN is_enable);
BT_STATUS BT_FtpSendFile(const uint16 *file_name);
BT_STATUS BT_FtpReceiveFile(const uint16 *file_name);
BT_STATUS BT_CancelFtpTransfer(void);
BT_STATUS BT_CreateNewFolder(BOOLEAN is_enable);
BT_STATUS BT_DeleteFile(BOOLEAN is_enable);

BT_FTP_SERVER_STATE_E BT_FTSGetState(BT_ADDRESS addr);
void BT_FtsSetDefaulRoot(uint16 *root_path);
// ftc -- ftp client
/******************************************************************************
 * FUNCTION:		BT_FTC_ConnectServer, others ftc api please refer to api doc
 * 
 * PARAMETERS:
 *		addr - bluetooth address
 *
 * RETURNS:
 *      BT_STATUS
 * 
 * MESSAGES:
 ******************************************************************************/
BT_STATUS BT_FTC_ConnectServer(const BT_ADDRESS *addr);
BT_STATUS BT_FTC_CancelConnectServer(const BT_ADDRESS *addr);
BT_STATUS BT_FTC_SetPath(const uint16 *path);
BT_STATUS BT_FTC_GoBackPath(void);
BT_STATUS BT_FTC_GoRootPath(void);
BT_STATUS BT_FTC_RefreshPath(void);
BT_STATUS BT_FTC_AddNewFolder(const uint16 *path);
BT_STATUS BT_FTC_SendFile(const uint16 *file_name);
BT_STATUS BT_FTC_ReceiveFile(const uint16 *file_name);
BT_STATUS BT_FTC_DeleteObject(const uint16 *file_name);
BT_STATUS BT_FTC_CancelTransfer(void);
BT_STATUS BT_FTC_DisconnectServer(const BT_ADDRESS *addr);
/******************************************************************************
 * FUNCTION:		
 * 
 * PARAMETERS:
 *		@remote_file_name: only remote file name(unicode)
 *      @file_name: the whole path of file name ( dir + filename)(unicode)
 *
 * RETURNS:
 *      BT_STATUS
 * 
 * MESSAGES:
 ******************************************************************************/
BT_STATUS BT_FTC_ReceiveFileSaveAs(uint16 *remote_file_name, uint16 *file_name);

BT_STATUS BT_UpdateNV(void);
/*
    
*/
/******************************************************************************
 * FUNCTION:        BT_GetRunningService
 * 
 * DESCRIPTION:     Get Current Running Service
 *
 * PARAMETERS:
 *      name -
 *
 * RETURNS:   return Service Type such as  BT_SERVICE_OPP, BT_SERVICE_FTP, BT_SERVICE_A2DP
 *
 * MESSAGES:
 *      None.
*********************************************************************************/
uint32 BT_GetRunningService(void);

/*
    
*/
/******************************************************************************
 * FUNCTION:        BT_CloseRunningService
 * 
 * DESCRIPTION:     Close Current Running Service
 *
 * PARAMETERS:  Service Type such as  BT_SERVICE_OPP, BT_SERVICE_FTP, BT_SERVICE_A2DP
 *      name -
 *
 * RETURNS:   return 
 *
 * MESSAGES:
 *      None.
*********************************************************************************/

BT_STATUS BT_CloseRunningService(uint32 service);

/******************************************************************************
 * Call control functions
 ******************************************************************************/

/******************************************************************************
 * FUNCTION:		BT_ActivateHS
 * 
 * DESCRIPTION:
 *
 * PARAMETERS:
 *	addr - 
 *
 * RETURNS:
 *	BT_PENDING - 
 *
 * MESSAGES:
 *	ID_STATUS_HFAG_SLC_CONNECTED
 *		status
 ******************************************************************************/
BT_STATUS BT_ActivateHS(const BT_ADDRESS *addr);

/******************************************************************************
 * FUNCTION:        BT_CancelActivateHS
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *      None.
 *
 * RETURNS:
 *      BT_PENDING -
 *
 * MESSAGES:
 *      ID_STATUS_HFAG_SLC_CONNECTED
 *          status = BT_ERROR
 *      ID_STATUS_HFAG_SLC_DISCONNECTED
 ******************************************************************************/
BT_STATUS BT_CancelActivateHS(void);

/******************************************************************************
 * FUNCTION:		BT_DeactivateHS
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *	None.
 *
 * RETURNS:
 *	BT_PENDING - 
 *
 * MESSAGES: 
 *	ID_STATUS_HFAG_SLC_DISCONNECTED
 ******************************************************************************/
BT_STATUS BT_DeactivateHS(void);

/******************************************************************************
 * FUNCTION:		BT_GetActivatedHS
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *	addr - 
 *
 * RETURNS:
 *	TRUE  -
 *	FALSE -
 *
 * MESSAGES:
 *	None.
 ******************************************************************************/
BOOLEAN BT_GetActivatedHS(BT_ADDRESS *addr);

/******************************************************************************
 * FUNCTION:		BT_StartRing
 *
 * DESCRIPTION: 
 *
 * PARAMETERS:
 *	phone_num -
 *
 * RETURNS:
 *	BT_PENDING - 
 *
 * MESSAGES:
 ******************************************************************************/
BT_STATUS BT_StartRing(const uint8 *phone_num);

/******************************************************************************
 * FUNCTION:		BT_StopRing
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *	None.
 *
 * RETURNS:
 *	
 * MESSAGES:
 ******************************************************************************/
BT_STATUS BT_StopRing(void);

/******************************************************************************
 * FUNCTION:		BT_StartAlert
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *	None.
 *
 * RETURNS:
 *	BT_PENDING - 
 * 
 * MESSAGES:
 ******************************************************************************/
BT_STATUS BT_StartAlert(void);

/******************************************************************************
 * FUNCTION:        BT_RemoteAlerting
 *
 * DESCRPTION:
 *
 * PARAMETERS:
 *  None.
 *
 * RETURNS:
 *  BT_PENDING - 
 *
 * MESSAGES:
 ******************************************************************************/
BT_STATUS BT_RemoteAlerting(void);

/******************************************************************************
 * FUNCTION:		BT_StopAlert
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *	None.
 *
 * RETURNS:
 *
 * MESSAGES:
 ******************************************************************************/
BT_STATUS BT_StopAlert(void);

/******************************************************************************
 * FUNCTION:		BT_HfgCallIndStatus
 * 
 * DESCRIPTION:
 *
 * PARAMETERS:
 *	param.
 *
 * RETURNS:
 *	BT_PENDING - 
 * 
 * MESSAGES:
 *	
 *****************************************************************************/
BT_STATUS BT_HfgCallIndStatus(uint32 param);

/******************************************************************************
 * FUNCTION:		BT_CallConnected
 * 
 * DESCRIPTION:
 *
 * PARAMETERS:
 *	None.
 *
 * RETURNS:
 *	BT_PENDING - 
 * 
 * MESSAGES:
 *	ID_STATUS_HFAG_AUDIO_CONNECTED
 *****************************************************************************/
BT_STATUS BT_CallConnected(void);

/******************************************************************************
 * FUNCTION:		BT_CallDisconnected
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *	None.
 *
 * RETURNS:
 *	BT_PENDING - 
 *
 * MESSAGES:
 *	ID_STATUS_HFAG_AUDIO_DISCONNECTED
 ******************************************************************************/
BT_STATUS BT_CallDisconnected(void);

/******************************************************************************
 * FUNCTION:		BT_TransferAudioConn
 *
 * PARAMETERS:
 *	direction - BT_AUDIO_CONN_AG_TO_HF / BT_AUDIO_CONN_HF_TO_AG
 *
 * RETURNS:
 *	BT_PENDING -
 *
 * MESSAGES:
 *	ID_STATUS_HFAG_AUDIO_DISCONNECTED 
 *	ID_STATUS_HFAG_AUDIO_CONNECTED
 ******************************************************************************/
BT_STATUS BT_TransferAudioConn(uint8 direction);

#if 0
/******************************************************************************
 * FUNCTION:		BT_AudioStart
 *
 * DESCRIPTION: Begin to play the audio
 *
 * PARAMETERS:
 *	None.
 *
 * RETURNS:
 *	BT_PENDING - 
 *
 * MESSAGES:
 *	ID_STATUS_HFAG_AUDIO_CONNECTED
 ******************************************************************************/
BT_STATUS  BT_AudioStart(void);



/******************************************************************************
 * FUNCTION:		BT_AudioStop
 *
 * DESCRIPTION:  Stop a playing audio
 *
 * PARAMETERS:
 *	None.
 *
 * RETURNS:
 *	BT_PENDING - 
 *
 * MESSAGES:
 *	ID_STATUS_HFAG_AUDIO_DISCONNECTED
 ******************************************************************************/

BT_STATUS  BT_AudioStop(void);
#endif

/******************************************************************************
 * FUNCTION:        BT_SetVolume
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *      channel -
 *      value   -
 *
 * RETURNS:
 *
 * MESSAGES:
 ******************************************************************************/
BT_STATUS BT_SetVolume(uint8 channel, uint8 value);

/******************************************************************************
 * FUNCTION:        BT_GetVolume
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *      channel -
 *
 * RETURNS:
 *
 * MESSAGES:
 ******************************************************************************/
uint8 BT_GetVolume(uint8 channel);

/******************************************************************************
 * FUNCTION:		BT_SetCallStatus
 * 
 * PARAMETERS:
 *	cell_service -
 * 	active_calls -
 *	call_setup   - BT_CALL_SETUP_IDLE / BT_CALL_SETUP_INCOMING
 *		       / BT_CALL_SETUP_OUTGOING / BT_CALL_SETUP_OUTGOING_ALERTED	
 *
 * RETURNS:
 * 
 * MESSAGES:
 ******************************************************************************/
//mdy by taiping.lai@for cr238845 cr238847 cr238858
BT_STATUS BT_SetCallStatus(BOOLEAN cell_service, uint8 active_calls, uint8 hold_call, uint8 call_setup);

/******************************************************************************
 * FUNCTION:        BT_HFGSetCind
 *
 * PARAMETERS:
 *  bt_hfg_indicator - current status of the AG indicators 
 * RETURNS:
 *
 * MESSAGES:
 ******************************************************************************/
BT_STATUS BT_HFGSetCind(BT_HFG_INDICATOR_T bt_hfg_indicator);

/******************************************************************************
 * FUNCTION:		BT_GetConnStatus
 * 
 * PARAMETERS:
 *		addr -
 *		service_type - BT_SERVICE_HSHF
 *
 * RETURNS:
 * 
 * MESSAGES:
 ******************************************************************************/
BT_STATUS BT_GetConnStatus(BT_ADDRESS *addr, uint32 service_type);

/*
 *  Set Ftp root folder path
 *  path: the folder path
 *  attribute:the folder is read only or write only or read&write
 *  mode : "r"/"R" -- Read Only,  "w"/"W" -- Write Only, "rw" -- read and write
 */
BT_STATUS BT_SetFtpRootFolder(const uint16 *path, const char *mode);

/******************************************************************************
 * FUNCTION:        BT_EnterEutMode
 *
 * PARAMETERS:
 *      None.
 *
 * RETURNS:
 *
 * MESSAGES:
 ******************************************************************************/
BT_STATUS BT_EnterEutMode(void);

BT_STATUS BT_LeaveEutMode(void);

BOOLEAN BT_GetDutState(void);
/******************************************************************************
 * FUNCTION:		BT_SetBdAddr
 *
 * PARAMETERS:
 *		addr - a pointer to BT_ADDRESS struct
 *
 * RETURNS:
 *		BT_SUCCESS : Set BdAddr successfully.
 *
 * MESSAGES:
 *		None.
 ******************************************************************************/

BT_STATUS BT_SetBdAddr(BT_ADDRESS *addr);
/******************************************************************************
 * FUNCTION:		BT_GetBdAddr
 *
 * PARAMETERS:
 *		addr - a pointer to BT_ADDRESS struct
 *
 * RETURNS:
 *		None.
 *
 * MESSAGES:
 *		None.
 ******************************************************************************/
void BT_GetBdAddr(BT_ADDRESS *addr);

/*
 * By Eddie.li
 * Add the interfaces for SPP applications.
 *  
 */
/******************************************************************************
 *  NAME
 *     BT_SppConnectStatus
 *  
 *  PARAMETERS
 *     Void
 *     
 *  DESCRIPTION:
 *     Get the status of spp connect. 
 *       
 *  RETURNS
 *     True  : Connected
 *     False : Disconnected
 
******************************************************************************/
PUBLIC BOOLEAN BT_SppConnectStatus(void);
/******************************************************************************
 *  NAME
 *     BT_SppConnectStatusUni()
 *  
 *  PARAMETERS
 *     Void
 *     
 *  DESCRIPTION:
 *     Get the status of spp connect. 
 *       
 *  RETURNS
 *     True  : Connected
 *     False : Disconnected
 
******************************************************************************/
PUBLIC BOOLEAN BT_SppConnectStatusUni(const BT_ADDRESS *addr);

/*----------------------------------------------------------------------------*
 *  NAME
 *      BT_SppConnect
 *
 *  PARAM:
 *     bd_addr : BD_ADDR of the peer side.
 *      
 *       
 *  DESCRIPTION
 *      Connect the link that had been established.
 *
 *  RETURNS
 *      BT_PENDING : Waiting for the peer side responding.
 *
 *----------------------------------------------------------------------------*/
BT_STATUS BT_SppConnect(BT_ADDRESS *bd_addr, BT_SPP_PORT_SPEED_E spp_speed);

/*----------------------------------------------------------------------------*
 *  NAME
 *      BT_SppDisconnect
 *
 *  PARAM:
 *      N/A
 *      
 *       
 *  DESCRIPTION
 *      Disconnect the link that had been established.
 *
 *  RETURNS
 *      BT_SUCCESS : Initialize SPP COM successfully.
 *
 *----------------------------------------------------------------------------*/
BT_STATUS BT_SppDisconnect(void);

/*----------------------------------------------------------------------------*
 *  NAME
 *      BT_SppInitialize
 *
 *  PARAM:
 *      para     : UART initialize parameter.
 *      fun      : UART's callback funtion.
 *       
 *  DESCRIPTION
 *      Initialize the vitual SPP COM.
 *
 *  RETURNS
 *      UART_SUCCESS : Initialize SPP COM successfully.
 *
 *----------------------------------------------------------------------------*/
uint32 BT_SppInitialize(UartCallback func);

/*----------------------------------------------------------------------------*
 *  NAME
 *      BT_SppClose
 *
 *  PARAM:
 *       
 *  DESCRIPTION
 *      Close the vitual SPP COM .
 *
 *  RETURNS
 *       UART_SUCCESS : Close SPP COM successfully.
 *
 *----------------------------------------------------------------------------*/
uint32 BT_SppClose(void);

/*----------------------------------------------------------------------------*
 *  NAME
 *      BT_SppGetTxFifoCnt
 *
 *  PARAM:
 *       
 *  DESCRIPTION
 *      Get the virtual SPPCOM's fifo size.
 *
 *  RETURNS
 *      fifo_cnt : Fifo size of the virtual SPPCOM.
 *
 *----------------------------------------------------------------------------*/
uint32 BT_SppGetTxFifoCnt(void);

/*----------------------------------------------------------------------------*
 *  NAME
 *      BT_SppReadData
 *
 *  PARAM:
 *      buffer   (o) : The data read from the SPPCOM should be written to the buffer.
 *      length   (i) : The max buffer size. 
 *       
 *  DESCRIPTION
 *      Read data from the virtual SPP Com.
 *
 *  RETURNS
 *      count : Acctual read data length
 *
 *----------------------------------------------------------------------------*/
uint32 BT_SppReadData(uint8 *buffer, uint32 length);

/*----------------------------------------------------------------------------*
 *  NAME
 *      BT_SppWriteData
 *
 *  PARAM:
 *      buffer   (i) : The data that should be write to SPP COM.
 *      length   (i) : The max buffer size. 
 *       
 *  DESCRIPTION
 *      Write data to the virtual SPP COM.
 *
 *  RETURNS
 *      count : Acctual write data length
 *
 *----------------------------------------------------------------------------*/
uint32 BT_SppWriteData(const uint8 *buffer, uint32 length);

/*----------------------------------------------------------------------------*
 *  NAME
 *      BT_Spp_Tx_Int_Enable
 *
 *  PARAMETERS:
 *      is_enable: TRUE, enable the interrupt and FALSE, disable the interrupt.
 *
 *  DESCRIPTION
 *      When there are data to send, it should enable the Tx Interrupt.
 *
 *  RETURNS
 *      N/A
 *
 *----------------------------------------------------------------------------*/
void BT_Spp_Tx_Int_Enable(BOOLEAN is_enable);

/*----------------------------------------------------------------------------*
 *  NAME
 *      BT_Spp_Rx_Int_Enable
 *
 *  PARAMETERS:
 *      is_enable: TRUE, enable the interrupt and FALSE, disable the interrupt.
 *
 *  DESCRIPTION
 *      When there are data to recieve, it should enable the rx Interrupt.
 *
 *  RETURNS
 *      N/A
 *
 *----------------------------------------------------------------------------*/
void BT_Spp_Rx_Int_Enable(BOOLEAN is_enable);

/*----------------------------------------------------------------------------*
  *  NAME
  *      BT_SppGetConnectedDevAddr
  *
  *  PARAM:
  *      addr : save the connected SPP device address.
  *       
  *  DESCRIPTION
  *      Get the connected SPP device address
  *
  *  RETURNS
  *      BT_STATUS : BT SUCCESS,get the address successfully
  *                  BT ERROR,get the address failed for parameter error or SPP connection has not been setup
  *
  *----------------------------------------------------------------------------*/
PUBLIC BT_STATUS BT_SppGetConnectedDevAddr(BT_ADDRESS *addr);
BOOL BT_AllocMenSppData(BT_AT_SPP_DATA **pAtSppData, UINT16 dataLen);
void BT_FreeMenSppData(BT_AT_SPP_DATA *pAtSppData);

/******************************************************************************
 * FUNCTION:		BT_HFGOpenDevice
 * 
 * PARAMETERS:
 *	addr -
 *
 * 	service_type  -
 *
 * RETURNS:
 * 
 * MESSAGES:
 ******************************************************************************/
uint16 BT_HFGOpenDevice(void);

/******************************************************************************
 * FUNCTION:		BT_HFGCloseDevice
 * 
 * PARAMETERS:
 *	addr -
 *
 * 	service_type  -
 *
 * RETURNS:
 * 
 * MESSAGES:
 ******************************************************************************/
uint16 BT_HFGCloseDevice(void);

/******************************************************************************
 * FUNCTION:		BT_HFGPlay
 *
 * PARAMETERS:
 *
 * RETURNS:
 *	BT_PENDING -
 *
 * MESSAGES:
 *	ID_STATUS_HFAG_PLAY_CFM 
 ******************************************************************************/
BT_STATUS BT_HFGPlay(void);

/******************************************************************************
 * FUNCTION:		BT_HFGStop
 *
 * PARAMETERS:
 *
 * RETURNS:
 *	BT_PENDING -
 *
 * MESSAGES:
 *	ID_STATUS_HFAG_STOP_CFM  
 ******************************************************************************/
BT_STATUS BT_HFGStop(void);

/******************************************************************************
 * FUNCTION:		BT_HFG_GetMusicOrVoice
 * 
 * PARAMETERS:
 *	addr -
 *
 * 	service_type  -
 *
 * RETURNS:
 * 
 * MESSAGES:
 ******************************************************************************/
BOOLEAN BT_HFG_GetMusicOrVoice(void);

/******************************************************************************
 * FUNCTION:		BT_HFG_SetMusicOrVoice
 * 
 * PARAMETERS:
 *	addr -
 *
 * 	service_type  -
 *
 * RETURNS:
 * 
 * MESSAGES:
 ******************************************************************************/
uint16 BT_HFG_SetMusicOrVoice(BOOLEAN is_music);

BT_STATUS BT_TransferAudioConnFormusic(uint8 direction);

/*----------------------------------------------------------------------------*
 *  NAME
 *      BT_AcceptDunReq
 *
 *  PARAMETERS:
 *      N/A
 *
 *  DESCRIPTION
 *      Is called by the MMI to accept a DUN link req.
 *
 *  RETURNS
 *      N/A
 *
 *----------------------------------------------------------------------------*/
PUBLIC void BT_AcceptDunReq(void);

/*----------------------------------------------------------------------------*
 *  NAME
 *      BT_RejectDunReq
 *
 *  PARAMETERS:
 *      N/A
 *
 *  DESCRIPTION
 *      Is called by the MMI to reject a DUN link req and disconnect the 
 *      the dun link that has been created just now.
 *
 *  RETURNS
 *      N/A
 *
 *----------------------------------------------------------------------------*/
PUBLIC void BT_RejectDunReq(void);

/**---------------------------------------------------------------------------*
 ** FUNCTION                                                                   
 **     void SIO_UserPortBusy()                                                     
 **                                                                           
 ** DESCRIPTION                                                               
 **     Judge the user port is being used.                                        
 **                                                                            
 ** RETURN VALUE                                                             
 **     TURE: USED
 **     FASLE:NO USED                                             
 **                                                                     
 ** DEPENDENCIES                                                           
 **     None                                                                 
 **                                                                         
 **---------------------------------------------------------------------------*/
PUBLIC BOOLEAN SIO_UserPortBusy(void);

#if defined(BT_HS_HW_INT_TYPE_UART0)
/******************************************************************************
 * FUNCTION:		BT_GetUartorPcm
 * 
 * PARAMETERS:
 *	addr -
 *
 * 	service_type  -
 *
 * RETURNS:
 * 
 * MESSAGES:
 ******************************************************************************/
BOOLEAN BT_GetUartorPcm(void);

/******************************************************************************
 * FUNCTION:		BT_SetUartorPcm
 * 
 * PARAMETERS:
 *	addr -
 *
 * 	service_type  -
 *
 * RETURNS:
 * 
 * MESSAGES:
 ******************************************************************************/
uint16 BT_SetUartorPcm(BOOLEAN bt_is_uartorpcm);
#endif

/******************************************************************************
 * BT import functions
 *
 * The platform should implement these functions.
 ******************************************************************************/
void BTI_StartupChip(void);
void BTI_ShutdownChip(void);
void BTI_SaveBdAddr(const BT_ADDRESS *addr);
void BTI_SaveXtalFtrim(uint16 ftrim);

void BTI_AssertExtWakeup(void);
void BTI_DeassertExtWakeup(void);

void BT_GetConfigInfo(BT_CONFIG *config);
void BTI_ChangeSystemClk(uint8 type);
void BTI_RestoreSystemClk(uint8 type);
void BTI_GSMTable_WriteShareMemory(const uint8 *buffer, uint8 length);

BT_STATUS BT_PbapPullPhonebook(
    const char *PbName_ptr,
    BT_PBAP_VCARD_FILTER_T *Filter_ptr,
    BT_PBAP_VCARD_FORMAT_T vcard_format,
    uint16 MaxListCount,
    uint16 ListOffset);

BT_STATUS BT_PbapSetPhonebook(
    const char *Folder_ptr,
    BOOLEAN Backup,
    BOOLEAN Reset);

BT_STATUS BT_PbapPullVcardListing(
    const char *FolderName_ptr,
    BT_PBAP_VCARD_SEARCH_ATTRIB_T SearchType,
    const char *SearchValue_ptr,
    BT_PBAP_VCARD_SORT_T Order,
    uint16 MaxListCount,
    uint16 ListOffset);

BT_STATUS BT_PbapPullVcardEntry(
    const char *ObjectName_ptr,
    BT_PBAP_VCARD_FILTER_T *Filter_ptr,
    BT_PBAP_VCARD_FORMAT_T vcard_format);

BT_STATUS BT_PbapRegisterClient(void);

BT_STATUS BT_PbapDeregisterClient(void);

BT_STATUS BT_PbapConnect(const BT_ADDRESS *addr);

BT_STATUS BT_PbapDisconnect(void);

BT_STATUS BT_PbapClientTpDisconnect(void);

BT_STATUS BT_PbapAbortClient(void);

BT_STATUS BT_PbapRegisterServer(
    PBAP_SERVER_CALLBACK_T *server_callback_ptr);

BT_STATUS BT_PbapDeregisterServer(void);

BT_STATUS BT_PbapServerTpDisconnect(void);

BT_STATUS BT_PbapAbortServer(void);

BOOLEAN BT_PbapIsConnected(void);

/******************************************************************************
 * BT HFU functions
 *
 * The platform should implement these functions.
 ******************************************************************************/
BT_STATUS BT_HfuConnect(const BT_ADDRESS *addr);

BT_STATUS BT_HfuDisconnect(void);

BOOLEAN BT_HfuGetConnStatus(BT_ADDRESS *addr);

BOOLEAN BT_HfuGetScoStatus(void);

BT_STATUS BT_HfuDialNumber(const uint8 *phone_num);

BT_STATUS BT_HfuAnswerCall(void);

BT_STATUS BT_HfuListCurrentCalls(void);

BT_STATUS BT_HfuHangup(void);

BT_STATUS BT_HfuSendDtmf(uint8 dtmfTone);

BT_STATUS BT_HfuCallHold(uint8 hold_action);

BT_STATUS BT_HfuSwitchAudio(BOOLEAN is_audio_on);

BT_STATUS BT_HfuRedial(void);

BT_STATUS BT_HfuIsAGSupportVoiceRecognition(BOOLEAN *is_support);

BT_STATUS BT_HfuEnableVoiceRecognition(BOOLEAN is_enable);

BT_STATUS BT_HfuIsAGSupportThreeWayCall(BOOLEAN *is_support, BT_HFU_GW_HOLD_FEATUERS_T *GwHoldFeatures);

BT_STATUS BT_HfuMemoryDial(char *Location);

BT_STATUS BT_HfuIsAGSupportRelSpecCallIndex(BOOLEAN *is_support);

BT_STATUS BT_HfuReleaseSpecialCallIndex(uint8 callindex);

BT_STATUS BT_HfuIsAGSupportPrivateConsultMode(BOOLEAN *is_support);

BT_STATUS BT_HfuPrivateConsultaionMode(uint8 callindex);

#ifdef BT_MAP_SUPPORT

BT_STATUS BT_MapRegisterClient(MapClient_PushMessageCallBack client_callback_ptr);

BT_STATUS BT_MapDeregisterClient(void);

BT_STATUS BT_MapConnect(const BT_ADDRESS *addr);

BT_STATUS BT_MapDisconnect(void);

BT_STATUS BT_MapClientTpDisconnect(void);

BT_STATUS BT_MapAbortClient(void);

BT_STATUS BT_MapSetMsgFolder(
    const char *Folder_ptr,
    BOOLEAN Backup, //backup,equivalent to “cd ..” on some systems
    BOOLEAN Reset   //Reset to the root directory
);

BT_STATUS BT_MapGetFolderListing(
    uint16 MaxListCount, //the maximum number of folders listed in the <x-obex/folder-listing > object
    uint16 ListOffset);

BT_STATUS BT_MapGetMessagesListing(
    const char *FolderName_ptr,
    uint16 MaxListCount,
    uint16 ListOffset,
    uint8 SubjectLen,
    BtMapParameterMask ParameterMask,
    BT_MAP_FILTER_T Filter);

BT_STATUS BT_MapGetMessage(
    const char *ObjectName_ptr,
    BOOLEAN attachment,
    BtMapCharsetFlags charset,
    BtMapFractionReqFlags fraction_req);

BT_STATUS BT_MapSetMessageStatus(
    const char *ObjectName_ptr,
    BtMapSetMsgStatusFlags statusInd,
    BOOLEAN statusValue);

BT_STATUS BT_MapUpdateInbox(void);

BT_STATUS BT_MapPushMessage(
    const char *FolderName_ptr,
    BtMapTransparentFlags transparent,
    BtMapRetryFlags retry,
    BtMapCharsetFlags charset,
    uint32 msgLen);

BT_STATUS BT_MapSetNotifyRegistration(
    BOOLEAN notify_status);

BOOLEAN BT_MapIsConnected(void);

#ifdef BT_MAP_SERVER_SUPPORT
BT_STATUS BT_MapRegisterServer(
    MAP_SERVER_CALLBACK_T *server_callback_ptr);

BT_STATUS BT_MapDeregisterServer(void);

BT_STATUS BT_MapServerTpDisconnect(void);

BT_STATUS BT_MapAbortServer(void);
#endif

#endif

#ifdef BT_AV_CT_SUPPORT
BT_STATUS AVRCP_CT_Connect(BT_ADDRESS addr);
#endif

BT_STATUS AVRCP_CT_Disconnect(void);

#ifdef BT_A2DP_SUPPORT
BT_STATUS AVRCP_TG_Connect(BT_ADDRESS *pAddr);
#define AVRCP_TG_Connect(pAddr) AVRCP_CT_Connect(*(BT_ADDRESS *)(pAddr))

BT_STATUS AVRCP_TG_Disconnect(void);
#define AVRCP_TG_Disconnect() AVRCP_CT_Disconnect()
#endif

#ifdef BT_AV_CT_SUPPORT
BT_STATUS AVRCP_CT_SetMdaPlayerSettingValues(
    uint8 numSettings,
    AvrcpCtPlayerSetting setting[]);

BT_STATUS AVRCP_CT_GetMdaPlayerSettingValues(uint8 attrMask);

BT_STATUS AVRCP_CT_ListMdaPlayerSettingAttrs(void);

BT_STATUS AVRCP_CT_ListMdaPlayerSettingValues(uint8 attrId);

BT_STATUS AVRCP_CT_GetMdaPlayerSettingAttrTxt(uint8 attrMask);

BT_STATUS AVRCP_CT_GetMdaPlayerSettingValueTxt(uint8 attrId);

BT_STATUS AVRCP_CT_GetMdaPlayStatus(void);

BT_STATUS AVRCP_CT_GetMdaMediaInfo(uint8 mediaMask);

BT_STATUS AVRCP_CT_SetPanelKey(AvrcpCtPanelOperation op, BOOLEAN isAutoRelease); //modify for bug 341731

BT_STATUS AVRCP_CT_SetSupportedCharsets(
    uint8 numCharsets,
    uint16 *charsets);

BT_STATUS AVRCP_CT_InformMdaBatteryStatus(uint8 battStatus);

void AVRCP_CT_SetPosInterval(uint32 interval);

#if AVRCP_BROWSING_CONTROLLER == XA_ENABLED
/*browser function*/
BT_STATUS AVRCP_CT_SetAddressedPlayer(uint16 playerId);

BT_STATUS AVRCP_CT_PlayItem(uint8 scope, uint8 *uid, uint16 uidCounter);

BT_STATUS AVRCP_CT_AddToNowPlaying(uint8 scope, uint8 *uid, uint16 uidCounter);

BT_STATUS AVRCP_CT_Search(uint16 charSet, uint16 length, uint8 *searchStr);

BT_STATUS AVRCP_CT_SetBrowsedPlayer(uint16 playerId);

BT_STATUS AVRCP_CT_ChangePath(uint16 uidCounter, uint8 direction, uint8 *uid);

BT_STATUS AVRCP_CT_GetItemAttributes(uint16 uidCounter,
                                     uint8 *uid,
                                     uint8 scope,
                                     uint8 numAttrs,
                                     uint32 *attrList);

BT_STATUS AVRCP_CT_GetFolderItems(uint8 scope,
                                  uint32 start,
                                  uint32 end,
                                  uint8 numAttrs,
                                  uint32 *attrList);

BT_STATUS AVRCP_CT_SetAbsoluteVolume(uint8 volume);
#endif

#endif

#ifdef BT_A2DP_SUPPORT
BT_STATUS AVRCP_TG_SetMdaEventMask(uint8 eventMask);

BT_STATUS AVRCP_TG_SetMdaPlayerSettingsMask(uint8 attrMask);

BT_STATUS AVRCP_TG_SetMdaMediaAttributesMask(uint8 mediaMask);

BT_STATUS AVRCP_TG_SetMdaPlayerSetting(uint8 numSettings, AvrcpTgPlayerSetting *setting);

BT_STATUS AVRCP_TG_SetMdaPlayerStrings(uint8 attrId, AvrcpTgPlayerStrings *strings);

BT_STATUS AVRCP_TG_SetMdaMediaInfo(uint8 numElements, AvrcpTgMediaInfo *elemInfo);

BT_STATUS AVRCP_TG_SetMdaPlayStatus(uint8 playStatus);

BT_STATUS AVRCP_TG_SetMdaPlayPosition(uint32 position, uint32 length);

BT_STATUS AVRCP_TG_SetMdaBattStatus(uint8 battStatus);

BT_STATUS AVRCP_TG_SetMdaSystemStatus(uint8 sysStatus);

BT_STATUS AVRCP_TG_SetMdaTrack(AvrcpTgTrackStruct *trackP);

BT_STATUS AVRCP_TG_SignalMdaTrackStart(void);

BT_STATUS AVRCP_TG_SignalMdaTrackEnd(void);

BT_STATUS AVRCP_TG_SetCurrentMdaCharSet(uint16 charSet);

uint16 AVRCP_TG_GetCurrentMdaCharSet(void);
#endif

#ifdef BT_AV_SNK_SUPPORT
BT_STATUS A2DP_Sink_Connect(BT_ADDRESS addr);
BT_STATUS A2DP_Sink_Disconnect(void);
BT_STATUS A2DP_Sink_GetAudioConfig(uint32 *sampleRate, uint8 *channelNum);
#endif

#ifdef BT_SAP_SUPPORT
BT_STATUS BT_SapRegisterClient(void);
BT_STATUS BT_SapDeregisterClient(void);
BT_STATUS BT_SapConnect(const BT_ADDRESS *addr);
BT_STATUS BT_SapDisconnect(void);
BT_STATUS BT_SapTransferApdu(BT_SAP_TRANSFER_APDU_REQ_T apdu);
BT_STATUS BT_SapTransferAtr(void);
BT_STATUS BT_SapPowerSimOff(void);
BT_STATUS BT_SapPowerSimOn(void);
BT_STATUS BT_SapResetSim(void);
BT_STATUS BT_SapTransferCardStatus(void);
BT_STATUS BT_SapSetTransferProtocol(uint8 transportProtocol);
#endif

#ifdef BT_SPP_SUPPORT
#ifdef BT_SPP_2ND_CHANNEL_SUPPORT
PUBLIC BT_STATUS BT_Spp2Connect(BT_ADDRESS *bd_addr, BT_SPP_PORT_SPEED_E spp_speed);
PUBLIC BT_STATUS BT_Spp2Disconnect(void);
uint32 BT_Spp2WriteData(const uint8 *src_buf, uint32 data_len);
PUBLIC BT_STATUS BT_Spp2GetConnectedDevAddr(BT_ADDRESS *addr);
#endif
#endif

#ifdef BT_PS_BQB_TEST
//fuction for rfcomm test
void RfcTestSetTesterAddr(BT_ADDRESS *tester_addr);
void RfcTestCreateSession(void);
void RfcTestSendData(void);
void RfcTestDisconnect(void);
void RfcTestSppConnect(void);

//fuction for rfcomm test
void L2CAPTest_SetTesterAddr(BT_ADDRESS *tester_addr);
void L2CAPTest_CreatACL(void);
void L2CAPTest_Connect(void);
void L2CAPTest_DisConnect(void);
void L2CAPTest_DisconnectACL(uint8 reason);
void L2CAPTest_SendData(void);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#define ID_STATUS_INVALID 0xFFFFFFFFUL

//Profile Message define
#define ID_BT_MSG_BASE 0x10000000UL
#define ID_CM_MSG_BASE 0x10010000UL
#define ID_SC_MSG_BASE 0x10020000UL
#define ID_SD_MSG_BASE 0x10030000UL
#define ID_OPC_MSG_BASE 0x10040000UL
#define ID_OPS_MSG_BASE 0x10050000UL
#define ID_FTC_MSG_BASE 0x10060000UL
#define ID_FTS_MSG_BASE 0x10070000UL
#define ID_HFG_MSG_BASE 0x10080000UL
#define ID_A2DP_MSG_BASE 0x10090000UL
#define ID_AVRCP_MSG_BASE 0x100A0000UL
#define ID_DG_MSG_BASE 0x100B0000UL
#define ID_SPP_MSG_BASE 0x100C0000UL
#define ID_BPP_MSG_BASE 0x100D0000UL
#define ID_HFU_MSG_BASE 0x100E0000UL
#define ID_PBAP_MSG_BASE 0x100F0000UL
#define ID_MAP_MSG_BASE 0x10100000UL
#define ID_A2DP_SINK_BASE 0x10110000UL
#define ID_SAP_MSG_BASE 0x10120000UL
#define ID_SPP2_MSG_BASE 0x10130000UL
#define ID_HOGP_MSG_BASE 0x10140000UL
#define ID_PXP_MSG_BASE 0x10170000UL
#define ID_HID_MSG_BASE 0x10180000UL
#define ID_GATT_SERVER_MSG_BASE 0x101A0000UL
#define ID_AT_MSG_BASE 0x101B0000UL

/////////////////////////////////////////////////////////
/* Message values */
//BT Message Define
#define ID_STATUS_BT_ON_RES (ID_BT_MSG_BASE | 0x0001)
#define ID_STATUS_BT_OFF_RES (ID_BT_MSG_BASE | 0x0002)
#define ID_STATUS_BT_SERVICE_OFF_RES (ID_BT_MSG_BASE | 0x0003)
#define ID_STATUS_BT_READ_RSSI_RES (ID_BT_MSG_BASE | 0x0004)
#define ID_STATUS_BT_SET_RSSI_MONITOR_RES (ID_BT_MSG_BASE | 0x0005)
#define ID_STATUS_BT_RSSI_ALERT_RES (ID_BT_MSG_BASE | 0x0006)
#define ID_STATUS_BT_PRIVATE_KEY_FINISH_IND (ID_BT_MSG_BASE | 0x0007)
#define ID_STATUS_BT_REOPEN_IND (ID_BT_MSG_BASE | 0x0008)

#define ID_STATUS_BT_LINK_CONNECT_RES (ID_BT_MSG_BASE | 0x0020)
#define ID_STATUS_BT_LINK_DISC_RES (ID_BT_MSG_BASE | 0x0021)
#define ID_STATUS_BT_LINK_DISCONNECT_RES (ID_BT_MSG_BASE | 0x0022)
#define ID_STATUS_GATT_SERVICE_STARTED_RES (ID_BT_MSG_BASE | 0x0023)
#define ID_STATUS_GATT_MTU_EXCHANGE_RES (ID_BT_MSG_BASE | 0x0024)
#define ID_STATUS_UPDATE_NV (ID_BT_MSG_BASE | 0x0025)
#define ID_STATUS_A2DP_MSG_TO_APP (ID_BT_MSG_BASE | 0x0026)
#define ID_GATT_READ_DYMATIC_ATTRIBUTE_VALUE (ID_BT_MSG_BASE | 0x0027)
#define ID_GATT_SEND_DATA_FLAG_ERROR (ID_BT_MSG_BASE | 0x0028)
#define ID_STATUS_BLE_LINK_PAIRED (ID_BT_MSG_BASE | 0x0029)
#define ID_STATUS_BLE_LINK_CONNECTED (ID_BT_MSG_BASE | 0x002A)
#define ID_STATUS_BT_NON_SIG_RX_DATA (ID_BT_MSG_BASE | 0x002B)
#define ID_STATUS_BLE_FINDER_FLAG_RES (ID_BT_MSG_BASE | 0x002C)
#define ID_STATUS_BLE_LAST_HANDLE_IND (ID_BT_MSG_BASE | 0x002D)

//HFG Message define
#define ID_STATUS_HFAG_SLC_CONNECTED (ID_HFG_MSG_BASE | 0x0001)
#define ID_STATUS_HFAG_SLC_DISCONNECTED (ID_HFG_MSG_BASE | 0x0002)
#define ID_STATUS_HFAG_HFU_CALL_ANSWER (ID_HFG_MSG_BASE | 0x0003)
#define ID_STATUS_HFAG_HFU_CALL_HANGUP (ID_HFG_MSG_BASE | 0x0004)
#define ID_STATUS_HFAG_HFU_DIAL_NUMBER (ID_HFG_MSG_BASE | 0x0005)
#define ID_STATUS_HFAG_HFU_DIAL_MEMORY (ID_HFG_MSG_BASE | 0x0006)
#define ID_STATUS_HFAG_HFU_REDIAL (ID_HFG_MSG_BASE | 0x0007)
#define ID_STATUS_HFAG_HFU_HOLD (ID_HFG_MSG_BASE | 0x0008)
#define ID_STATUS_HFAG_AUDIO_CONNECTED (ID_HFG_MSG_BASE | 0x0009)
#define ID_STATUS_HFAG_AUDIO_DISCONNECTED (ID_HFG_MSG_BASE | 0x000A)
#define ID_STATUS_HFAG_HFU_STATUS_REQ (ID_HFG_MSG_BASE | 0x000B)
#define ID_STATUS_HS_SPK_VOL_CHANGE (ID_HFG_MSG_BASE | 0x000C)
#define ID_STATUS_HS_MIC_VOL_CHANGE (ID_HFG_MSG_BASE | 0x000D)
#define ID_STATUS_HFAG_PLAY_CFM (ID_HFG_MSG_BASE | 0x000E)
#define ID_STATUS_HFAG_STOP_CFM (ID_HFG_MSG_BASE | 0x000F)
#define ID_STATUS_HFAG_HFU_DTMF_NUMBER (ID_HFG_MSG_BASE | 0x0010)
#define ID_STATUS_HFAG_SLC_CONNECTED_IND (ID_HFG_MSG_BASE | 0x0011)
#define ID_STATUS_HFAG_HFU_CLCC (ID_HFG_MSG_BASE | 0x0012)
#define ID_STATUS_HFAG_CALL_TYPE_WB_NB (ID_HFG_MSG_BASE | 0x0013)

//CM message define
#define ID_STATUS_CM_INQUIRY_REQ (ID_CM_MSG_BASE | 0x0001)
#define ID_STATUS_CM_INQUIRY_RES (ID_CM_MSG_BASE | 0x0002)
#define ID_STATUS_CM_INQUIRY_FINISH_RES (ID_CM_MSG_BASE | 0x0003)
#define ID_STATUS_CM_CANCEL_INQUIRY_RES (ID_CM_MSG_BASE | 0x0004)
#define ID_STATUS_CM_VISIBLE_RES (ID_CM_MSG_BASE | 0x0005)
#define ID_STATUS_CM_DISVISIBLE_RES (ID_CM_MSG_BASE | 0x0006)
#define ID_STATUS_CM_SET_LOCAL_NAME_RES (ID_CM_MSG_BASE | 0x0007)
#define ID_STATUS_CM_READ_LOCAL_NAME_RES (ID_CM_MSG_BASE | 0x0008)
#define ID_STATUS_CM_READ_REMOTE_NAME_RES (ID_CM_MSG_BASE | 0x0009)
#define ID_STATUS_CM_READ_LOCAL_ADDR_RES (ID_CM_MSG_BASE | 0x000A)
#define ID_STATUS_CM_SET_LOCAL_ADDR_RES (ID_CM_MSG_BASE | 0x000B)
#define ID_STATUS_CM_SET_BLE_LOCAL_NAME_RES (ID_CM_MSG_BASE | 0x000C)

//SC Message Define
#define ID_STATUS_SC_PASSKEY_REQ (ID_SC_MSG_BASE | 0x0001)
#define ID_STATUS_SC_PAIR_RES (ID_SC_MSG_BASE | 0x0002)
#define ID_STATUS_SC_DEPAIR_RES (ID_SC_MSG_BASE | 0x0003)
#define ID_STATUS_SC_PAIR_IND (ID_SC_MSG_BASE | 0x0004)
#define ID_STATUS_SC_SSP_NUM_IND (ID_SC_MSG_BASE | 0x0005)
#define ID_STATUS_SC_AUTH_IND (ID_SC_MSG_BASE | 0x0006)     //20120815: keddy
#define ID_STATUS_SC_KBD_CONN_IND (ID_SC_MSG_BASE | 0x0007) //for ble keypad pair
#define ID_STATUS_SC_BLE_SLAVE_PAIR_IND (ID_SC_MSG_BASE | 0x0008)
//SD Message Define
#define ID_STATUS_SD_READ_SERVICES_RES (ID_SD_MSG_BASE | 0x0001)

//avrcp message  define
#define ID_STATUS_AVRCP_OPERATION_ID_FORWARD (ID_AVRCP_MSG_BASE | 0x0001)
#define ID_STATUS_AVRCP_OPERATION_ID_BACKWARD (ID_AVRCP_MSG_BASE | 0x0002)
#define ID_STATUS_AVRCP_OPERATION_ID_VOL_DOWN (ID_AVRCP_MSG_BASE | 0x0003)
#define ID_STATUS_AVRCP_OPERATION_ID_VOL_UP (ID_AVRCP_MSG_BASE | 0x0004)
#define ID_STATUS_AVRCP_OPERATION_ID_PLAY (ID_AVRCP_MSG_BASE | 0x0005)
#define ID_STATUS_AVRCP_OPERATION_ID_STOP (ID_AVRCP_MSG_BASE | 0x0006)
#define ID_STATUS_AVRCP_OPERATION_ID_PAUSE (ID_AVRCP_MSG_BASE | 0x0007)
#define ID_STATUS_AVRCP_CONNECT_COMPLETE (ID_AVRCP_MSG_BASE | 0x0008)
#define ID_STATUS_AVRCP_DISCONNECT_COMPLETE (ID_AVRCP_MSG_BASE | 0x0009)
#define ID_STATUS_AVRCP_COMMAND_RECEIVED (ID_AVRCP_MSG_BASE | 0x000A)

//avrcp ct message define
#define ID_STATUS_AVRCP_CT_CONNECT_COMPLETE (ID_AVRCP_MSG_BASE | 0x000B)
#define ID_STATUS_AVRCP_CT_DISCONNECT_COMPLETE (ID_AVRCP_MSG_BASE | 0x000C)
#define ID_STATUS_AVRCP_EVENT_PLAYBACK_CHANGED (ID_AVRCP_MSG_BASE | 0x000D)
#define ID_STATUS_AVRCP_EVENT_TRACK_CHANGED (ID_AVRCP_MSG_BASE | 0x000E)
#define ID_STATUS_AVRCP_EVENT_TRACK_REACHED_END (ID_AVRCP_MSG_BASE | 0x000F)
#define ID_STATUS_AVRCP_EVENT_TRACK_REACHED_START (ID_AVRCP_MSG_BASE | 0x0010)
#define ID_STATUS_AVRCP_EVENT_PLAYBACK_POS_CHANGED (ID_AVRCP_MSG_BASE | 0x0011)
#define ID_STATUS_AVRCP_EVENT_BATT_STATUS_CHANGED (ID_AVRCP_MSG_BASE | 0x0012)
#define ID_STATUS_AVRCP_EVENT_SYSTEM_STATUS_CHANGED (ID_AVRCP_MSG_BASE | 0x0013)
#define ID_STATUS_AVRCP_EVENT_PLAYER_APPLICATION_CHANGED (ID_AVRCP_MSG_BASE | 0x0014)
#define ID_STATUS_AVRCP_CT_PLAY_STATUS (ID_AVRCP_MSG_BASE | 0x0015)
#define ID_STATUS_AVRCP_CT_MEDIA_ATTRIBUTES (ID_AVRCP_MSG_BASE | 0x0016)
#define ID_STATUS_AVRCP_SUPPORTED_PLAYER_ATTRIBUTES (ID_AVRCP_MSG_BASE | 0x0017)
#define ID_STATUS_AVRCP_EXIST_PLAYER_ATTRIBUTES_VALUES (ID_AVRCP_MSG_BASE | 0x0018)
#define ID_STATUS_AVRCP_PLAYER_CURRENT_ATTRIBUTE (ID_AVRCP_MSG_BASE | 0x0019)
#define ID_STATUS_AVRCP_PLAYER_ATTRIBUTES_TXT (ID_AVRCP_MSG_BASE | 0x001A)
#define ID_STATUS_AVRCP_PLAYER_VALUES_TXT (ID_AVRCP_MSG_BASE | 0x001B)
#define ID_STATUS_AVRCP_PANEL_KEY_RSP (ID_AVRCP_MSG_BASE | 0x001C)
#define ID_STATUS_AVRCP_SET_PLAYER_VALUES (ID_AVRCP_MSG_BASE | 0x001D)
#define ID_STATUS_AVRCP_ABORT_METADATA (ID_AVRCP_MSG_BASE | 0x001E)
#define ID_STATUS_AVRCP_SET_CHARSETS (ID_AVRCP_MSG_BASE | 0x001F)
#define ID_STATUS_AVRCP_INFORM_BATT_STATUS (ID_AVRCP_MSG_BASE | 0x0020)

#if AVRCP_BROWSING_CONTROLLER == XA_ENABLED
//avrcp ct message define for browser
#define ID_STATUS_AVRCP_EVENT_AVAILABLE_PLAYERS_CHANGED (ID_AVRCP_MSG_BASE | 0x0021)
#define ID_STATUS_AVRCP_EVENT_ADDRESSED_PLAYER_CHANGED (ID_AVRCP_MSG_BASE | 0x0022)
#define ID_STATUS_AVRCP_EVENT_UIDS_CHANGED (ID_AVRCP_MSG_BASE | 0x0023)
#define ID_STATUS_AVRCP_EVENT_VOLUME_CHANGED (ID_AVRCP_MSG_BASE | 0x0024)
#define ID_STATUS_AVRCP_EVENT_PLAYING_CONTENT_CHANGED (ID_AVRCP_MSG_BASE | 0x0025)

#define ID_STATUS_AVRCP_SET_ADDRESSED_PLAYER (ID_AVRCP_MSG_BASE | 0x0026)
#define ID_STATUS_AVRCP_PLAY_ITEM (ID_AVRCP_MSG_BASE | 0x0027)
#define ID_STATUS_AVRCP_ADD_NOW_PLAYING (ID_AVRCP_MSG_BASE | 0x0028)
#define ID_STATUS_AVRCP_SEARCH (ID_AVRCP_MSG_BASE | 0x0029)
#define ID_STATUS_AVRCP_SET_BROWSED_PLAYER (ID_AVRCP_MSG_BASE | 0x002A)
#define ID_STATUS_AVRCP_CHANGE_PATH (ID_AVRCP_MSG_BASE | 0x002B)
#define ID_STATUS_AVRCP_GET_FOLDER_ITEMS (ID_AVRCP_MSG_BASE | 0x002C)
#define ID_STATUS_AVRCP_SET_ABSOLUTE_VOLUME (ID_AVRCP_MSG_BASE | 0x002D)
#endif

#ifdef BT_A2DP_SUPPORT
//avrcp tg message define
#define ID_STATUS_AVRCP_IND_TG_PLAY_POS_EVENT (ID_AVRCP_MSG_BASE | 0x002E)
#define ID_STATUS_AVRCP_IND_TG_PLAYER_VALUES_CHANGED (ID_AVRCP_MSG_BASE | 0x002F)
#define ID_STATUS_AVRCP_IND_TG_DISP_CHARSET_CHANGED (ID_AVRCP_MSG_BASE | 0x0030)
#define ID_STATUS_AVRCP_IND_TG_BATT_STATUS_CHANGED (ID_AVRCP_MSG_BASE | 0x0031)
#define ID_STATUS_AVRCP_OPERATION_ID_NEXT_GROUP (ID_AVRCP_MSG_BASE | 0x0032)
#define ID_STATUS_AVRCP_OPERATION_ID_PREV_GROUP (ID_AVRCP_MSG_BASE | 0x0033)
#endif
#define ID_STATUS_AVRCP_SEND_REWIND_OR_FASTFWD_SINGNAL (ID_AVRCP_MSG_BASE | 0x0034)

//a2dp message  define
#define ID_STATUS_A2DP_SRC_CONFIG_STREAM (ID_A2DP_MSG_BASE | 0x0001)
#define ID_STATUS_A2DP_SRC_OPEN_STREAM (ID_A2DP_MSG_BASE | 0x0002)
#define ID_STATUS_A2DP_SRC_START_STREAM (ID_A2DP_MSG_BASE | 0x0003)
#define ID_STATUS_A2DP_SRC_SUSPEND_STREAM (ID_A2DP_MSG_BASE | 0x0004)
#define ID_STATUS_A2DP_SRC_CLOSE_STREAM (ID_A2DP_MSG_BASE | 0x0005)
#define ID_STATUS_A2DP_SRC_RESUME_STREAM (ID_A2DP_MSG_BASE | 0x0006)
#define ID_STATUS_A2DP_SRC_REGISTER (ID_A2DP_MSG_BASE | 0x0007)
#define ID_STATUS_A2DP_CONNECTED (ID_A2DP_MSG_BASE | 0x0008)
#define ID_STATUS_A2DP_DISCONNECTED (ID_A2DP_MSG_BASE | 0x0009)
#define ID_STATUS_A2DP_SNK_START_STREAM (ID_A2DP_MSG_BASE | 0x000A)
#define ID_STATUS_A2DP_SNK_RESUME_STREAM (ID_A2DP_MSG_BASE | 0x000B)
#define ID_STATUS_A2DP_SNK_SUSPEND_STREAM (ID_A2DP_MSG_BASE | 0x000C)
#define ID_STATUS_A2DP_SRC_INIT (ID_A2DP_MSG_BASE | 0x000D)
#define ID_STATUS_A2DP_SRC_DEINIT (ID_A2DP_MSG_BASE | 0x000E)
#define ID_STATUS_A2DP_SRC_CONFIG_IND (ID_A2DP_MSG_BASE | 0x000F)
#define ID_STATUS_A2DP_DATA_SENT_RSP (ID_A2DP_MSG_BASE | 0x0010)
#define ID_STATUS_A2DP_DATA_CONSMD (ID_A2DP_MSG_BASE | 0x0011)
#define ID_STATUS_A2DP_ABORT_CFM (ID_A2DP_MSG_BASE | 0x0012)
#define ID_STATUS_A2DP_ABORT_IND (ID_A2DP_MSG_BASE | 0x0013)
#define ID_STATUS_A2DP_CONNECTED_IND (ID_A2DP_MSG_BASE | 0x0014)
#define ID_STATUS_A2DP_SRC_CODEC_INFO (ID_A2DP_MSG_BASE | 0x0015)
#define ID_STATUS_A2DP_SRC_REMOTE_CODEC_INFO (ID_A2DP_MSG_BASE | 0x0016)
#define ID_STATUS_A2DP_SRC_PRE_SBC_IND (ID_A2DP_MSG_BASE | 0x0017)
#define ID_STATUS_A2DP_SRC_CLEAR_BUF_IND (ID_A2DP_MSG_BASE | 0x0018)

//OPP Client message define
#define ID_STATUS_OPC_CONNECT_RES (ID_OPC_MSG_BASE | 0x0001)
#define ID_STATUS_OPC_SERVER_ABORT (ID_OPC_MSG_BASE | 0x0002)
#define ID_STATUS_OPC_PUT_ABORT (ID_OPC_MSG_BASE | 0x0003)
#define ID_STATUS_OPC_PUT_OBJECT (ID_OPC_MSG_BASE | 0x0004)
#define ID_STATUS_OPC_PUT_FINAL (ID_OPC_MSG_BASE | 0x0005)
#define ID_STATUS_OPC_DISCONNECT_IND (ID_OPC_MSG_BASE | 0x0006)
#define ID_STATUS_OPC_GET_DATA_FROM_APP (ID_OPC_MSG_BASE | 0x0007)
#define ID_STATUS_OPC_SEND_REQ (ID_OPC_MSG_BASE | 0x0008)
#define ID_STATUS_OPC_CLOSE_RES (ID_OPC_MSG_BASE | 0x0009)
#define ID_STATUS_OPP_RESTORE_CLK (ID_OPC_MSG_BASE | 0x000A)
#define ID_STATUS_OPP_CHANGE_CLK (ID_OPC_MSG_BASE | 0x000B)
#define ID_STATUS_OPS_CLOSE_RES (ID_OPC_MSG_BASE | 0x000C)

//OPP Server Message Define
#define ID_STATUS_OPS_PUT_REQ (ID_OPS_MSG_BASE | 0x0001)
#define ID_STATUS_OPS_CLIENT_ABORT (ID_OPS_MSG_BASE | 0x0002)
#define ID_STATUS_OPS_PUT_ABORT (ID_OPS_MSG_BASE | 0x0004)
#define ID_STATUS_OPS_PUT_OBJECT (ID_OPS_MSG_BASE | 0x0005)
#define ID_STATUS_OPS_PUT_FINAL (ID_OPS_MSG_BASE | 0x0006)
#define ID_STATUS_OPS_CONNECT_REQ (ID_OPS_MSG_BASE | 0x0007)
#define ID_STATUS_OPS_DISCONNECT_IND (ID_OPS_MSG_BASE | 0x0008)

//FTP Client message define
#define ID_STATUS_FTC_CONNECT_RES (ID_FTC_MSG_BASE | 0x0001)
#define ID_STATUS_FTC_SERVER_ABORT (ID_FTC_MSG_BASE | 0x0002)
#define ID_STATUS_FTC_PUT_ABORT (ID_FTC_MSG_BASE | 0x0003)
#define ID_STATUS_FTC_PUT_OBJECT (ID_FTC_MSG_BASE | 0x0004)
#define ID_STATUS_FTC_PUT_FINAL (ID_FTC_MSG_BASE | 0x0005)
#define ID_STATUS_FTC_GET_ABORT (ID_FTC_MSG_BASE | 0x0006)
#define ID_STATUS_FTC_GET_OBJECT (ID_FTC_MSG_BASE | 0x0007)
#define ID_STATUS_FTC_GET_FINAL (ID_FTC_MSG_BASE | 0x0008)
#define ID_STATUS_FTC_GET_LIST_FOLDER (ID_FTC_MSG_BASE | 0x0009)
#define ID_STATUS_FTC_ADD_FOLDER_RES (ID_FTC_MSG_BASE | 0x000A)
#define ID_STATUS_FTC_DEL_OBJ_RES (ID_FTC_MSG_BASE | 0x000B)
#define ID_STATUS_FTC_DISCONNECT_IND (ID_FTC_MSG_BASE | 0x000C)

//FTP Server Message Define
#define ID_STATUS_FTS_PUT_REQ (ID_FTS_MSG_BASE | 0x0001)
#define ID_STATUS_FTS_CLIENT_ABORT (ID_FTS_MSG_BASE | 0x0002)
#define ID_STATUS_FTS_PUT_ABORT (ID_FTS_MSG_BASE | 0x0003)
#define ID_STATUS_FTS_PUT_OBJECT (ID_FTS_MSG_BASE | 0x0004)
#define ID_STATUS_FTS_PUT_FINAL (ID_FTS_MSG_BASE | 0x0005)
#define ID_STATUS_FTS_GET_REQ (ID_FTS_MSG_BASE | 0x0006)
#define ID_STATUS_FTS_GET_ABORT (ID_FTS_MSG_BASE | 0x0007)
#define ID_STATUS_FTS_GET_OBJECT (ID_FTS_MSG_BASE | 0x0008)
#define ID_STATUS_FTS_GET_FINAL (ID_FTS_MSG_BASE | 0x0009)
#define ID_STATUS_FTS_ADD_FOLDER_REQ (ID_FTS_MSG_BASE | 0x000A)
#define ID_STATUS_FTS_DEL_REQ (ID_FTS_MSG_BASE | 0x000B)
#define ID_STATUS_FTS_UPDATE_ROOT (ID_FTS_MSG_BASE | 0x000C)
#define ID_STATUS_FTS_DEL_BUSY (ID_FTS_MSG_BASE | 0x000D)
#define ID_STATUS_FTS_CONNECT_REQ (ID_FTS_MSG_BASE | 0x000E)
#define ID_STATUS_FTS_DISCONNECT_IND (ID_FTS_MSG_BASE | 0x000F)

/* Added by eddie.li */
/* DUN Gateway Message Define */
#define ID_STATUS_DG_LINK_STATUS_RES (ID_DG_MSG_BASE | 0x0001)
#define ID_STATUS_DG_LINK_REQ (ID_DG_MSG_BASE | 0x0002)
#define ID_STATUS_DG_LINK_CLOSE_IND (ID_DG_MSG_BASE | 0x0003)
/* Added by eddie.li end.*/

/* Added by eddie.li */
#define ID_STATUS_SPP_LINK_STATUS_RES (ID_SPP_MSG_BASE | 0x0001)
#define ID_STATUS_SPP_DATA_RECIEVE_IND (ID_SPP_MSG_BASE | 0x0002)
#define ID_STATUS_SPP_DATA_RECIEVE_CFM (ID_SPP_MSG_BASE | 0x0003)
#define ID_STATUS_SPP_CONNECT_IND (ID_SPP_MSG_BASE | 0x0004)
#define ID_STATUS_SPP_DISCONNECT_IND (ID_SPP_MSG_BASE | 0x0005)
#define ID_STATUS_SPP_STATUS_IND (ID_SPP_MSG_BASE | 0x0006)
#define ID_STATUS_SPP_SERVICE_NAME_IND (ID_SPP_MSG_BASE | 0x0007)
#define ID_STATUS_SPP_DATA_RECEIVE_EX_IND (ID_SPP_MSG_BASE | 0x0008)
#define ID_STATUS_SPP_CONNECT_RES (ID_SPP_MSG_BASE | 0x0009)
#define ID_STATUS_SPP_DISCONNECT_RES (ID_SPP_MSG_BASE | 0x000A)
#define ID_STATUS_SPP_DATA_SEND_RES (ID_SPP_MSG_BASE | 0x000B)

/*  BPP MESSAGE GROUTP Begin*/
#define ID_STATUS_BPPC_CONNECT_CFM (ID_BPP_MSG_BASE | 0x0001)
#define ID_STATUS_BPPC_CONNECT_IND (ID_BPP_MSG_BASE | 0x0002)
#define ID_STATUS_BPPC_DISCONNECT_IND (ID_BPP_MSG_BASE | 0x0003)
#define ID_STATUS_BPPC_SEND_FINISH_CFM (ID_BPP_MSG_BASE | 0x0004)
#define ID_STATUS_BPPC_DOCUMENT_IND (ID_BPP_MSG_BASE | 0x0005)
#define ID_STATUS_BPPC_ABORT_CFM (ID_BPP_MSG_BASE | 0x0006)
#define ID_STATUS_BPPC_PRINTER_ATTRIBUTES_CFM (ID_BPP_MSG_BASE | 0x0007)
#define ID_STATUS_BPPC_JOB_ATTRIBUTES_IND (ID_BPP_MSG_BASE | 0x0008)
#define ID_STATUS_BPPC_JOB_ATTRIBUTES_CFM (ID_BPP_MSG_BASE | 0x0009)
#define ID_STATUS_BPPC_EVENT_IND (ID_BPP_MSG_BASE | 0x000A)
#define ID_STATUS_BPPC_PRINTER_ATTRIBUTES_IND (ID_BPP_MSG_BASE | 0x000B)
#define ID_STATUS_BPPC_JOB_CREATE_CFM (ID_BPP_MSG_BASE | 0x000C)
#define ID_STATUS_BPPC_REF_OBJ_IND (ID_BPP_MSG_BASE | 0x000D)

//HFU Message Define
#define ID_STATUS_HFU_SLC_CONNECTED (ID_HFU_MSG_BASE | 0x0001)
#define ID_STATUS_HFU_SLC_DISCONNECTED (ID_HFU_MSG_BASE | 0x0002)
#define ID_STATUS_HFU_AUDIO_CONNECTED (ID_HFU_MSG_BASE | 0x0003)
#define ID_STATUS_HFU_AUDIO_DISCONNECTED (ID_HFU_MSG_BASE | 0x0004)
#define ID_STATUS_HFU_CIND_IND (ID_HFU_MSG_BASE | 0x0005)
#define ID_STATUS_HFU_RING_IND (ID_HFU_MSG_BASE | 0x0006)
#define ID_STATUS_HFU_CLIP_IND (ID_HFU_MSG_BASE | 0x0007)
#define ID_STATUS_HFU_CIEV_IND (ID_HFU_MSG_BASE | 0x0008)
#define ID_STATUS_HFU_CLCC_RECORD_IND (ID_HFU_MSG_BASE | 0x0009)
#define ID_STATUS_HFU_DIAL_NUMBER_RESULT_IND (ID_HFU_MSG_BASE | 0x000a)
#define ID_STATUS_HFU_ATA_RESULT_IND (ID_HFU_MSG_BASE | 0x000b)
#define ID_STATUS_HFU_HANGUP_RESULT_IND (ID_HFU_MSG_BASE | 0x000c)
#define ID_STATUS_HFU_SEND_DTMF_RESULT_IND (ID_HFU_MSG_BASE | 0x000d)
#define ID_STATUS_HFU_CALLHOLD_RESULT_IND (ID_HFU_MSG_BASE | 0x000e)
#define ID_STATUS_HFU_CLCC_RESULT_IND (ID_HFU_MSG_BASE | 0x000f)
#define ID_STATUS_HFU_SPK_VOL_IND (ID_HFU_MSG_BASE | 0x0010)
#define ID_STATUS_HFU_MIC_VOL_IND (ID_HFU_MSG_BASE | 0x0011)
#define ID_STATUS_HFU_CCWA_IND (ID_HFU_MSG_BASE | 0x0012)
#define ID_STATUS_HFU_ENABLE_VOICE_RECOGN_RESULT_IND (ID_HFU_MSG_BASE | 0x0013)
#define ID_STATUS_HFU_VOICE_RECOGN_STATE_IND (ID_HFU_MSG_BASE | 0x0014)
#define ID_STATUS_HFU_REDIAL_RESULT_IND (ID_HFU_MSG_BASE | 0x0015)
#define ID_STATUS_HFU_MEMORY_DIAL_RESULT_IND (ID_HFU_MSG_BASE | 0x0016)

/*  PBAP MESSAGE GROUTP Begin*/
#define ID_STATUS_PBAP_TP_DISCONNECTED (ID_PBAP_MSG_BASE | 0x0002)
#define ID_STATUS_PBAP_COMPLETE (ID_PBAP_MSG_BASE | 0x0003)
#define ID_STATUS_PBAP_ABORT (ID_PBAP_MSG_BASE | 0x0004)
#define ID_STATUS_PBAP_PARAMS_RX (ID_PBAP_MSG_BASE | 0x0006)
#define ID_STATUS_PBAP_CLIENT_PARAMS_RX (ID_PBAP_MSG_BASE | 0x0006)
#define ID_STATUS_PBAP_SERVER_PARAMS_RX (ID_PBAP_MSG_BASE | 0x0007)
#define ID_STATUS_PBAP_DATA_IND (ID_PBAP_MSG_BASE | 0x000C)

/*  MAP MESSAGE GROUTP Begin*/
#define ID_STATUS_MAP_TP_DISCONNECTED (ID_MAP_MSG_BASE | 0x0001)
#define ID_STATUS_MAP_COMPLETE (ID_MAP_MSG_BASE | 0x0002)
#define ID_STATUS_MAP_ABORT (ID_MAP_MSG_BASE | 0x0003)
#define ID_STATUS_MAP_CLIENT_PARAMS_RX (ID_MAP_MSG_BASE | 0x0004)
#define ID_STATUS_MAP_SERVER_PARAMS_RX (ID_MAP_MSG_BASE | 0x0005)
#define ID_STATUS_MAP_DATA_IND (ID_MAP_MSG_BASE | 0x0006)
#define ID_STATUS_MAP_MNS_SERVER_CONNECTED (ID_MAP_MSG_BASE | 0x0007)
#define ID_STATUS_MAP_MNS_SERVER_PARAMS_RX (ID_MAP_MSG_BASE | 0x0008)
#define ID_STATUS_MAP_MNS_SERVER_DATA_IND (ID_MAP_MSG_BASE | 0x0009)
#define ID_STATUS_MAP_CALLBACK_CALL (ID_MAP_MSG_BASE | 0x000A)

/*  A2DP SINK MESSAGE GROUTP Begin*/
#define ID_STATUS_A2DP_SINK_CONNECT_COMPLETED (ID_A2DP_SINK_BASE | 0x0001)
#define ID_STATUS_A2DP_SINK_DISCONNECT_COMPLETED (ID_A2DP_SINK_BASE | 0x0002)
#define ID_STATUS_A2DP_SINK_STREAM_STARTED (ID_A2DP_SINK_BASE | 0x0003)
#define ID_STATUS_A2DP_SINK_STREAM_SUSPENDED (ID_A2DP_SINK_BASE | 0x0004)
#define ID_STATUS_A2DP_SINK_CDEC_INFO (ID_A2DP_SINK_BASE | 0x0005)

//SAP Message define
#define ID_STATUS_SAP_CONNECTED (ID_SAP_MSG_BASE | 0x0001)
#define ID_STATUS_SAP_DISCONNECTED (ID_SAP_MSG_BASE | 0x0002)
#define ID_STATUS_SAP_POWERON_SIM_CNF (ID_SAP_MSG_BASE | 0x0003)
#define ID_STATUS_SAP_POWEROFF_SIM_CNF (ID_SAP_MSG_BASE | 0x0004)
#define ID_STATUS_SAP_RESET_SIM_CNF (ID_SAP_MSG_BASE | 0x0005)
#define ID_STATUS_SAP_TRANSFER_APDU_CNF (ID_SAP_MSG_BASE | 0x0006)
#define ID_STATUS_SAP_TRANSFER_ATR_CNF (ID_SAP_MSG_BASE | 0x0007)
#define ID_STATUS_SAP_TRANSFER_READER_STATUS_CNF (ID_SAP_MSG_BASE | 0x0008)
#define ID_STATUS_SAP_SET_TRANSFER_PROTOCOL_CNF (ID_SAP_MSG_BASE | 0x0009)
#define ID_STATUS_SAP_DISCONNECT_IND (ID_SAP_MSG_BASE | 0x000a)
#define ID_STATUS_SAP_CARD_STATUS_IND (ID_SAP_MSG_BASE | 0x000b)

#ifdef BT_SPP_2ND_CHANNEL_SUPPORT
#define ID_STATUS_SPP2_LINK_STATUS_RES (ID_SPP2_MSG_BASE | 0x0001)
#define ID_STATUS_SPP2_DATA_RECIEVE_IND (ID_SPP2_MSG_BASE | 0x0002)
#define ID_STATUS_SPP2_DATA_RECIEVE_CFM (ID_SPP2_MSG_BASE | 0x0003)
#define ID_STATUS_SPP2_CONNECT_IND (ID_SPP2_MSG_BASE | 0x0004)
#define ID_STATUS_SPP2_DISCONNECT_IND (ID_SPP2_MSG_BASE | 0x0005)
#define ID_STATUS_SPP2_STATUS_IND (ID_SPP2_MSG_BASE | 0x0006)
#define ID_STATUS_SPP2_SERVICE_NAME_IND (ID_SPP2_MSG_BASE | 0x0007)
#endif

//HOGP Message Info
#define ID_STATUS_HOGP_NOTIFY_RECIEVE_IND (ID_HOGP_MSG_BASE | 0x0001)
#define ID_STATUS_HOGP_ENABLE (ID_HOGP_MSG_BASE | 0x0002)
#define ID_STATUS_HOGP_DISABLE (ID_HOGP_MSG_BASE | 0x0003)
#define ID_STATUS_HOGP_GET_BATTERY_LEVEL_IND (ID_HOGP_MSG_BASE | 0x0004)
//HID Message Info
#define ID_STATUS_HID_CONNECTED_IND (ID_HID_MSG_BASE | 0x0001)
#define ID_STATUS_HID_DISCONNECTED_IND (ID_HID_MSG_BASE | 0x0002)
// PXP AND FDM
#define ID_PXP_LLS_SERVICE_CONN_COMP (ID_PXP_MSG_BASE | 0X0001)
#define ID_PXP_LLS_SERVICE_DISCONN_COMP (ID_PXP_MSG_BASE | 0X0002)
#define ID_PXP_IAS_SERVICE_ALERT_LEVEL_IND (ID_PXP_MSG_BASE | 0X0003)
#define ID_STATUS_GATT_SERVER_REG_READ_RES (ID_GATT_SERVER_MSG_BASE | 0x0001)
#define ID_STATUS_GATT_SERVER_REG_WRITE_RES (ID_GATT_SERVER_MSG_BASE | 0x0002)
#define ID_STATUS_GATT_SERVER_DEREG_WRITE_RES (ID_GATT_SERVER_MSG_BASE | 0x0003)
#define ID_STATUS_GATT_SERVER_SERVICE_READ_RES (ID_GATT_SERVER_MSG_BASE | 0x0004)
#define ID_STATUS_GATT_SERVER_SERVICE_ADD_WRITE_RES (ID_GATT_SERVER_MSG_BASE | 0x0005)
#define ID_STATUS_GATT_SERVER_SERVICE_DEL_WRITE_RES (ID_GATT_SERVER_MSG_BASE | 0x0006)
#define ID_STATUS_GATT_SERVER_CHAR_WRITE_RES (ID_GATT_SERVER_MSG_BASE | 0x0007)
#define ID_STATUS_GATT_SERVER_DESC_WRITE_RES (ID_GATT_SERVER_MSG_BASE | 0x0008)
#define ID_STATUS_GATT_SERVER_SERVICE_START_WRITE_RES (ID_GATT_SERVER_MSG_BASE | 0x0009)
#define ID_STATUS_GATT_SERVER_SERVICE_STOP_WRITE_RES (ID_GATT_SERVER_MSG_BASE | 0x000A)
#define ID_STATUS_GATT_SERVER_INDICAT_NOTIFY_WRITE_RES (ID_GATT_SERVER_MSG_BASE | 0x000B)
#define ID_STATUS_GATT_SERVER_REQ_RECIEVE_IND (ID_GATT_SERVER_MSG_BASE | 0x000C)
#define ID_STATUS_GATT_SERVER_RSP_WRITE_RES (ID_GATT_SERVER_MSG_BASE | 0x000D)
#define ID_STATUS_GATT_SERVER_CONN_READ_RES (ID_GATT_SERVER_MSG_BASE | 0x000E)
#define ID_STATUS_GATT_SERVER_DISCONN_WRITE_RES (ID_GATT_SERVER_MSG_BASE | 0x000F)

#define ID_STATUS_AT_BLE_SET_PUBLIC_ADDR_RES (ID_AT_MSG_BASE | 0x0001)
#define ID_STATUS_AT_BLE_SET_RANDOM_ADDR_RES (ID_AT_MSG_BASE | 0x0002)
#define ID_STATUS_AT_BLE_ADD_WHITE_LIST_RES (ID_AT_MSG_BASE | 0x0003)
#define ID_STATUS_AT_BLE_REMOVE_WHITE_LIST_RES (ID_AT_MSG_BASE | 0x0004)
#define ID_STATUS_AT_BLE_CLEAR_WHITE_LIST_RES (ID_AT_MSG_BASE | 0x0005)
#define ID_STATUS_AT_BLE_CONNECT_RES (ID_AT_MSG_BASE | 0x0006)
#define ID_STATUS_AT_BLE_DISCONNECT_RES (ID_AT_MSG_BASE | 0x0007)
#define ID_STATUS_AT_BLE_UPDATA_CONNECT_RES (ID_AT_MSG_BASE | 0x0008)
#define ID_STATUS_AT_BLE_SET_ADV_PARA_RES (ID_AT_MSG_BASE | 0x0009)
#define ID_STATUS_AT_BLE_SET_ADV_ENABLE_RES (ID_AT_MSG_BASE | 0x000A)
#define ID_STATUS_AT_BLE_SET_SCAN_RSP_RES (ID_AT_MSG_BASE | 0x000B)
#define ID_STATUS_AT_BLE_SET_SCAN_PARA_RES (ID_AT_MSG_BASE | 0x000C)
#define ID_STATUS_AT_BLE_SET_SCAN_ENABLE_RES (ID_AT_MSG_BASE | 0x000D)
#define ID_STATUS_AT_BLE_SET_SCAN_DISABLE_RES (ID_AT_MSG_BASE | 0x000E)
#define ID_STATUS_AT_BLE_SET_SCAN_REPORT_RES (ID_AT_MSG_BASE | 0x000F)
#define ID_STATUS_AT_BLE_SET_SCAN_FINISH_RES (ID_AT_MSG_BASE | 0x00010)
#define ID_STATUS_AT_BLE_SET_ADV_DATA_RES (ID_AT_MSG_BASE | 0x0011)

#ifdef BLE_INCLUDED
/** Bluetooth Address */
typedef struct
{
    uint8 address[6];
} __attribute__((packed)) bt_bdaddr_t;

/** Bluetooth Device Name */
typedef struct
{
    uint8 name[249];
} __attribute__((packed)) bt_bdname_t;

/** Bluetooth Adapter Visibility Modes*/
typedef enum
{
    BT_SCAN_MODE_NONE,
    BT_SCAN_MODE_CONNECTABLE,
    BT_SCAN_MODE_CONNECTABLE_DISCOVERABLE
} bt_scan_mode_t;

/** Bluetooth Adapter State */
typedef enum
{
    BT_STATE_OFF,
    BT_STATE_ON,
    BT_RADIO_OFF,
    BT_RADIO_ON
} bt_state_t;

/** Bluetooth PinKey Code */
typedef struct
{
    uint8 pin[16];
} __attribute__((packed)) bt_pin_code_t;

typedef struct
{
    uint8 status;
    uint8 ctrl_state;   /* stack reported state */
    uint64 tx_time;     /* in ms */
    uint64 rx_time;     /* in ms */
    uint64 idle_time;   /* in ms */
    uint64 energy_used; /* a product of mA, V and ms */
} __attribute__((packed)) bt_activity_energy_info;

/** Bluetooth Adapter Discovery state */
typedef enum
{
    BT_DISCOVERY_STOPPED,
    BT_DISCOVERY_STARTED
} bt_discovery_state_t;

/** Bluetooth ACL connection state */
typedef enum
{
    BT_ACL_STATE_CONNECTED,
    BT_ACL_STATE_DISCONNECTED
} bt_acl_state_t;

/** Bluetooth 128-bit UUID */
typedef struct
{
    uint8 uu[16];
} bt_uuid_t;

/** Bluetooth SDP service record */
typedef struct
{
    bt_uuid_t uuid;
    uint16 channel;
    char name[256]; // what's the maximum length
} bt_service_record_t;

/** Bluetooth Remote Version info */
typedef struct
{
    int version;
    int sub_ver;
    int manufacturer;
} bt_remote_version_t;

typedef struct
{
    uint8 local_privacy_enabled;
    uint8 max_adv_instance;
    uint8 rpa_offload_supported;
    uint8 max_irk_list_size;
    uint8 max_adv_filter_supported;
    uint8 scan_result_storage_size_lobyte;
    uint8 scan_result_storage_size_hibyte;
    uint8 activity_energy_info_supported;
} bt_local_le_features_t;

/* Bluetooth Adapter and Remote Device property types */
typedef enum
{
    /* Properties common to both adapter and remote device */
    /**
     * Description - Bluetooth Device Name
     * Access mode - Adapter name can be GET/SET. Remote device can be GET
     * Data type   - bt_bdname_t
     */
    BT_PROPERTY_BDNAME = 0x1,
    /**
     * Description - Bluetooth Device Address
     * Access mode - Only GET.
     * Data type   - bt_bdaddr_t
     */
    BT_PROPERTY_BDADDR,
    /**
     * Description - Bluetooth Service 128-bit UUIDs
     * Access mode - Only GET.
     * Data type   - Array of bt_uuid_t (Array size inferred from property length).
     */
    BT_PROPERTY_UUIDS,
    /**
     * Description - Bluetooth Class of Device as found in Assigned Numbers
     * Access mode - Only GET.
     * Data type   - uint32.
     */
    BT_PROPERTY_CLASS_OF_DEVICE,
    /**
     * Description - Device Type - BREDR, BLE or DUAL Mode
     * Access mode - Only GET.
     * Data type   - bt_device_type_t
     */
    BT_PROPERTY_TYPE_OF_DEVICE,
    /**
     * Description - Bluetooth Service Record
     * Access mode - Only GET.
     * Data type   - bt_service_record_t
     */
    BT_PROPERTY_SERVICE_RECORD,

    /* Properties unique to adapter */
    /**
     * Description - Bluetooth Adapter scan mode
     * Access mode - GET and SET
     * Data type   - bt_scan_mode_t.
     */
    BT_PROPERTY_ADAPTER_SCAN_MODE,
    /**
     * Description - List of bonded devices
     * Access mode - Only GET.
     * Data type   - Array of bt_bdaddr_t of the bonded remote devices
     *               (Array size inferred from property length).
     */
    BT_PROPERTY_ADAPTER_BONDED_DEVICES,
    /**
     * Description - Bluetooth Adapter Discovery timeout (in seconds)
     * Access mode - GET and SET
     * Data type   - uint32
     */
    BT_PROPERTY_ADAPTER_DISCOVERY_TIMEOUT,

    /* Properties unique to remote device */
    /**
     * Description - User defined friendly name of the remote device
     * Access mode - GET and SET
     * Data type   - bt_bdname_t.
     */
    BT_PROPERTY_REMOTE_FRIENDLY_NAME,
    /**
     * Description - RSSI value of the inquired remote device
     * Access mode - Only GET.
     * Data type   - int32_t.
     */
    BT_PROPERTY_REMOTE_RSSI,
    /**
     * Description - Remote version info
     * Access mode - SET/GET.
     * Data type   - bt_remote_version_t.
     */

    BT_PROPERTY_REMOTE_VERSION_INFO,

    /**
     * Description - Local LE features
     * Access mode - GET.
     * Data type   - bt_local_le_features_t.
     */
    BT_PROPERTY_LOCAL_LE_FEATURES,

    BT_PROPERTY_REMOTE_DEVICE_TIMESTAMP = 0xFF,
} bt_property_type_t;

/** Bluetooth Adapter Property data structure */
typedef struct
{
    bt_property_type_t type;
    int len;
    void *val;
} bt_property_t;

/** Bluetooth Device Type */
typedef enum
{
    BT_DEVICE_DEVTYPE_BREDR = 0x1,
    BT_DEVICE_DEVTYPE_BLE,
    BT_DEVICE_DEVTYPE_DUAL
} bt_device_type_t;
/** Bluetooth Bond state */
typedef enum
{
    BT_BOND_STATE_NONE,
    BT_BOND_STATE_BONDING,
    BT_BOND_STATE_BONDED
} bt_bond_state_t;

/** Bluetooth SSP Bonding Variant */
typedef enum
{
    BT_SSP_VARIANT_PASSKEY_CONFIRMATION,
    BT_SSP_VARIANT_PASSKEY_ENTRY,
    BT_SSP_VARIANT_CONSENT,
    BT_SSP_VARIANT_PASSKEY_NOTIFICATION
} bt_ssp_variant_t;

typedef uint8 bt_service_id_t;

#define BT_MAX_NUM_UUIDS 32

/** GATT ID adding instance id tracking to the UUID */
typedef struct
{
    bt_uuid_t uuid;
    uint8 inst_id;
} btgatt_gatt_id_t;

/** GATT Service ID also identifies the service type (primary/secondary) */
typedef struct
{
    btgatt_gatt_id_t id;
    uint8 is_primary;
} btgatt_srvc_id_t;

/** Preferred physical Transport for GATT connection */
typedef enum
{
    BT_GATT_TRANSPORT_AUTO,
    BT_GATT_TRANSPORT_BREDR,
    BT_GATT_TRANSPORT_LE
} btgatt_transport_t;

typedef struct
{
    bt_bdaddr_t *bda1;
    bt_uuid_t *uuid1;
    uint16 u1;
    uint16 u2;
    uint16 u3;
    uint16 u4;
    uint16 u5;
} btgatt_test_params_t;

#define ALOGD SCI_TRACE_LOW
#define ALOGI SCI_TRACE_LOW
#define ALOGE SCI_TRACE_LOW
#define ALOGW SCI_TRACE_LOW

void BT_SecurityGrant(const BT_ADDRESS *addr);

#endif

BT_STATUS BT_SetAdvParam(BT_ADV_PARAM *pAdvParam);

BOOLEAN BT_ReadRawRssi(const BT_ADDRESS *addr);

BOOLEAN BT_SetRssiMonitorMode(BOOLEAN enable, int8 low_threashold, int8 high_threashold, const BT_ADDRESS *addr);

void ble_adv_enable(BOOLEAN adv_enable);

/******************************************************************************
 * FUNCTION:    bt_send_ble_data
 *
 * DESCRIPTION: for APP sending BLE data
 *
 * PARAMETERS:
 *      profile_index:      indicate which profile to send data
 *      connection_handle:  the handle when successful connection with peer device
 *      attribute_handle:   indicate the attribute handle to send data
 *      data:               the data to send     
 *      data_len:           length of data to send
 *      
 *
 * RETURNS: 
 *     status of sending ble data
 *
******************************************************************************/

void BT_HIDSendBTKey(uint32 keys_status, uint32 key_code, BOOLEAN flag);

extern BT_STATUS ble_find_me_cmd(void);

/***********************************ctrl_cmd_corresponding_define**********************************/
typedef struct
{
    uint8 len;
    uint8 arr[256];
} ctrl_notify_result_cmd_t;

extern ctrl_notify_result_cmd_t ctrl_notify_result_cmd;
extern BOOLEAN process_cmd_complete;
BT_STATUS ble_notify_user_ctrl_cmd_result(uint8 conn_id, uint8 argv);
BOOLEAN ble_get_connection_status(BT_ADDRESS *addr);
// ctrl_cmd_id define
#define USER_CTRL_CMD_OPCODE 0xFE
#define CTRL_RESET_CMD 0x00
#define CTRL_RING_CMD 0x01
#define CTRL_LANGUE_SET_CMD 0x02
#define CTRL_LOCAL_CONN_CMD 0x03
#define CTRL_GET_VERSION_CMD 0x04

typedef enum
{
    BT_SIMU_NONE,
    BT_SIMU_KPD,
    BT_SIMU_SPEAKER,
    BT_SIMU_MAX
} BT_SIMU_MODE_E;

typedef enum
{
    BT_ADV_DISABLE = 0x00,
    BT_ADV_ENABLE,
} bt_adv_state_t;

typedef enum
{
    BT_NONE_STATE = 0x00,
    BT_ADV_STATE,
    BT_SCAN_STATE,
    BT_CONN_STATE
} bt_at_state_t;

typedef enum
{
    BT_INQ_DISABLE = 0x00,
    BT_INQ_IDLE,
    BT_INQ_PENDING
} bt_inquiry_state_t;

typedef struct
{
    bdaddr_t addr;
    UINT8 rssi;
    UINT8 reserved;
    UINT32 classOfDevice;
    UINT8 device_name[MAX_BT_DEVICE_NAME];
} bt_inquiry_device_info_t;

typedef struct
{
    bt_inquiry_device_info_t *bt_inquiry_device_info_ptr;
    UINT16 maxResult;
    UINT16 deviceNum;
} bt_inquiry_result_t;
uint32 BT_GetSearchDeviceInfo(UINT8 *addr, UINT8 *name, UINT8 idx);
uint32 BT_GetSearchDevice_state(void);

#define BT_ADDR_CMP(src_addr, tar_addr, rlt)                                                                                   \
    {                                                                                                                          \
        rlt = 0;                                                                                                               \
        do                                                                                                                     \
        {                                                                                                                      \
            unsigned int src32 = (unsigned int)((src_addr[0] << 24) | (src_addr[1] << 16) | (src_addr[2] << 8) | src_addr[3]); \
            unsigned int tar32 = (unsigned int)((tar_addr[0] << 24) | (tar_addr[1] << 16) | (tar_addr[2] << 8) | tar_addr[3]); \
            if (src32 != tar32)                                                                                                \
            {                                                                                                                  \
                rlt = 1;                                                                                                       \
                break;                                                                                                         \
            }                                                                                                                  \
            unsigned short src16 = (unsigned short)((src_addr[4] << 8) | src_addr[5]);                                         \
            unsigned short tar16 = (unsigned short)((tar_addr[4] << 8) | tar_addr[5]);                                         \
            if (src16 != tar16)                                                                                                \
            {                                                                                                                  \
                rlt = 1;                                                                                                       \
                break;                                                                                                         \
            }                                                                                                                  \
        } while (0);                                                                                                           \
    }

//Function for the WCNIT Test AT
void BT_ATC_RegisterCallback(BT_CALLBACK callback);
void BT_ATC_DeregisterCallback(void);
void BT_ATC_SetTestMode(BOOLEAN flag, BT_SIMU_MODE_E simu_mode);
BOOLEAN BT_ATC_GetTestMode(BT_SIMU_MODE_E *sim_mode);
#ifdef BT_A2DP_SUPPORT
#ifdef BT_AV_CT_SUPPORT
void AVRCP_CT_Realse_PanelKey(uint16 data, void *ptr);
void AVRCP_CT_FASTFWD_OR_REWIND_PanelKey(AvrcpCtPanelOperation Op_Avrcp, uint32 ctrl_time);
void AVRCP_CT_Push_PanelKey(uint16 data, void *ptr);
#endif
#endif

extern void pbap_pullphonebook_req(void);
extern void pbap_pullvcardentry_req(void);
extern void pts_mode_enter(void);
extern void pts_mode_exit(void);

extern void BT_Self_Recover(uint32 value);
extern BT_STATUS BT_Stop_Ex(void);
extern BT_STATUS BT_SetGattNotify(const uint8 *value, const uint8 len);
extern BT_STATUS BT_GetGattNotify(uint8 *value, const uint8 len);
extern const char *BT_BLE_GetBTVersionInfo(void);
extern BT_TEST_MODE_T BT_GetTestMode(void);
extern void UART_SetControllerBqbMode(BOOLEAN is_ctrlr_bqb_mode);
extern void BT_SetTestMode(BT_TEST_MODE_T mode);
extern void bt_handle_adv_enable(bt_adv_state_t state);
extern BOOLEAN BT_BLE_GetNonSigRxData(BT_NONSIGCALLBACK callback);
//extern uint16_t UART_Write(const uint8_t *buffer, uint16_t length);
extern uint16_t UartDrv_Tx(const uint8_t *buffer, uint16_t length);
extern void app_read_register_gatt_server_id(void);
extern BOOLEAN app_register_gatt_server(uint8 *uuid, uint16 uuid_len);
extern BOOLEAN app_deregister_gatt_server(int server_if);
extern void app_read_user_service(void);
extern BOOLEAN app_add_user_service(int server_if, uint8 *uuid, uint16 uuid_len, uint16 handle_num, uint8 is_primary);
extern BOOLEAN app_remove_user_service(uint16 service_handle);
extern BOOLEAN app_add_user_character(uint16 service_handle, uint8 *uuid, uint16 uuid_len, int property, uint16 perm);
extern BOOLEAN app_add_user_discriptor(uint16 service_handle, uint8 *uuid, uint16 uuid_len, uint16 perm);
extern BOOLEAN app_start_user_service(uint16 service_handle, uint8 transport);
extern BOOLEAN app_stop_user_service(uint16 service_handle);
extern BOOLEAN app_send_indi_or_noti(int conn_id, uint16 att_handle, uint8 need_confirm, uint8 *value, uint16 value_length);
extern BOOLEAN app_send_response(int conn_id, uint32 trans_id, uint16 att_handle, uint8 *value, uint16 value_length);
extern void app_read_user_conn_id(void);
extern BOOLEAN app_disconnect_gatt_server(int conn_id);
extern uint8_t bt_handle_get_adv_enable_state(void);

//得到蓝牙版本信息
const char *BT_BLE_GetBTVersionInfo(void);

//设置蓝牙公共地址
uint8 BLE_SetPublicAddr(uint8 addr[6]);

//获取蓝牙公共地址
void BLE_GetPublicAddr(uint8 *addr);

//设置随机地址
uint8 BLE_SetRandomAddr(uint8 addr[6]);

//获取随机地址
void BLE_GetRandomAddr(uint8 *addr);

//增加白名单
uint8 BLE_AddWhiteList(st_white_list_info white_list_info);

//查询白名单数量
uint8 BLE_QuiryWhiteCount(void);

//获取白名单信息
uint8 BLE_QuiryWhiteListInfo(uint8 index, st_white_list_info *white_list_info);

//移出白名单
uint8 BLE_RemoveWhiteList(uint8 *addr);

//清除白名单
uint8 BLE_ClearWhiteList(void);

//设置名字
BT_STATUS BLE_SetLocalName(const uint16 *name);

//获取名字
BT_STATUS BLE_GetLocalName(uint16 *name);

//建立连接
uint8 BLE_Connect(uint8 type, uint8 addr[6]);

//得到连接状态
uint8 BLE_GetConnectState(uint8 *addr);

//断开连接
uint8 BLE_Disconnect(uint8 addr[6]);

//更新连接参数
uint8 BLE_UpdateConnect(uint16 handle, uint16 intervalMin, uint16 intervalMax, uint16 slaveLatency, uint16 timeoutMulti);

//设置广播数据
uint8 BLE_SetAdvParaDirect(uint16 advMin, uint16 advMax, uint8 advType, uint8 ownAddrType, uint8 directAddrType, bdaddr_t directAddr, uint8 advChannMap, uint8 advFilter);
uint8 BLE_SetAdvParaUnDirect(uint16 advMin, uint16 advMax, uint8 advType, uint8 ownAddrType, uint8 advChannMap, uint8 advFilter);
uint8 BLE_SetAdvPara(uint16 advMin, uint16 advMax, uint8 advType, uint8 ownAddrType, uint8 directAddrType, bdaddr_t directAddr, uint8 advChannMap, uint8 advFilter);
//设置广播数据
uint8 BLE_SetAdvData(uint8 len, char *data);

//set iBeacon data
uint8 BLE_SetBeaconData(char *uuid, char *major, char *minor);

//设置激活广播
uint8 BLE_SetAdvEnable(uint8 enable);

//设置扫描应答
uint8 BLE_SetScanRsp(UINT8 *data, UINT8 length);

//设置扫描参数
uint8 BLE_SetScanPara(UINT8 type, UINT16 interval, UINT16 window, UINT8 filter);

//设置扫描激活
uint8 BLE_SetScanEnable(UINT8 type);
void BT_BLE_HandleEvent(uint8 *pdu);

extern UINT8 app_le_scan_enable(void);
extern int mgr_le_scan_stop(void);
//extern int mgr_le_create_acl(bdaddr_t bdaddr, void *tid, UINT8 addrType);
extern void att_client_connect(bdaddr_t *address, UINT8 module, UINT8 addr_type);
extern int mgr_le_release_acl_fromaddr(bdaddr_t address);
int GATT_Tester_Connect(bdaddr_t address, UINT8 addr_type);
int gatt_tester_disconnect(bdaddr_t address);
void BT_BLE_SetAtCmdState(uint32 at_cmd_state);
uint32 BT_BLE_GetAtCmdState(void);
void BLE_RegisterWriteUartCallback(BLE_WRITE_UART_CALLBACK callback);
uint8 BLE_SendTpData(uint16 datalen, uint8 *data);

void BT_SendMsgToAPP(unsigned int msg_id, char status, void *data_ptr);

/**********************************************************************************************/
#endif /* BT_ABS_H */
