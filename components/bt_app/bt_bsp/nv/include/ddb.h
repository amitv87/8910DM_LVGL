#ifndef BT_DDB_H
#define BT_DDB_H

#include "bt_abs.h"
#include "sci_types.h"
#include "bt_app.h"

#define INPUT
#define OUTPUT
#define INOUT
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#define WHITE_NUM_MAX 7
#define DB_MAX_PAIRED_DEVICE_COUNT 40
#define XA_DISABLED 0 /* Disable the feature */
#define XA_ENABLED 1  /* Enable the feature */
#define LE_SMP_DEVICE_INFO_NUM 6 /* the max number of ble smp paired device info saved in nv */

#define HOST_NV_CTRL_HCI_CAP 0x00000001
#define HOST_NV_CTRL_SSP 0x00000002
#define HOST_NV_CTRL_RFCOM_FC 0x00000004
#define HOST_NV_CTRL_A2DP_RATE 0x00000008
#define HOST_NV_CTRL_INQUIRY_GAP 0x00000010 // for keep earphone link alive
#define HOST_NV_CTRL_DEFAULT_VISIABLE 0x00000020
#define HOST_NV_CTRL_EXIT_SNIFF_FOR_AUDIO_AND_DATA 0x00000040
#define HOST_NV_CTRL_ALLOW_SCAN_WHILE_CON 0x00000080
#define MAX_FRIENDLY_NAME_LEN (40)
#define MAX_DEVICE_NAME_SIZE (MAX_FRIENDLY_NAME_LEN + 1)

#define NV_REF_EXT_BASE_ID 420 // 420-449 for ref
#define NV_PS_EXT_BASE_ID 470  // 470-489 for ps
#define NV_L1_EXT_BASE_ID 490  // 490-499 for l1
#define MN_ATNV_USER_BASE 450  // 400--449 for platform use; 450--469 for at use

#define _DB_IN_NV   //DB save to NV area
#define NV_MAIGC_VALUE      0x00891000
#define NV_BT_NAME_DEFAULT  "8910dm"

#if defined(GPS_SUPPORT) && defined(GPS_CHIP_VER_GREENEYE2)
#define NV_GPS_GE2_ITEM_NUM 50
#else
#define NV_GPS_GE2_ITEM_NUM 0
#endif

#ifdef FOTA_SUPPORT_ADUPS
#define NV_ADUPS_ITEM_NUM 8
#else
#define NV_ADUPS_ITEM_NUM 0
#endif

#define NV_GPS_GE2_BASE_ID 500 //for GPS
#define MN_NV_ADUPS_BASE NV_GPS_GE2_BASE_ID + NV_GPS_GE2_ITEM_NUM
#define NV_MODEM_NV_BASE_ID 600
#define NV_MODEM_NV_MAX_NUM 50
#define MN_NV_USER_BASE 650           // from which id for MMI use
#define NV_TD_CALIBRATION_BASE 1300   // 1300-1302 for td calibration
#define NV_TD_PS_EXT_BASE_ID 2000     // 2000-2049 reserved for 3g ps
#define MN_NV_TD_USER_BASE 3200       // from which id for MMI use,mn use
#define MN_NEW_SMS_IN_NV_MAX_NUM 1000 // the newset max sms num in NV
#define MAX_NV_USER_ITEM_NUM 2400     // max item number for MMI useage£¬reserved£¬LAST_NV_ITEM is used in nor platform
#define MN_SMS_IN_NV_MAX_NUM 200      // the max sms num in NV
#define NV_MAX_PHONE_BOOK_RECORDS 100
#define NV_MAX_LN_RECORDS 10

#ifdef UMEM_SUPPORT
#ifdef UMEM_NEW_MECHNISM
#define NV_UDISK_MAX_NUM 8192
#endif
#endif

typedef struct
{
    UINT24 lap;
    UINT8 uap;
    UINT16 nap;
} BD_ADDR_T;

typedef BOOLEAN BOOLEAN;
typedef uint32 classOfDevice_t;
typedef BD_ADDR_T deviceAddr_t;
typedef uint8 deviceLinkkey_t[SIZE_OF_LINKKEY];
typedef char deviceName_t[MAX_DEVICE_NAME_SIZE];

typedef enum _NVITEM_ERR
{
    NVERR_None = 0, /* Success */
    NVERR_System,   /* System error, e.g. hardware failure */
    NVERR_INVALID_Param,
    NVERR_NO_ENOUGH_Resource,
    NVERR_NOT_Exist,
    NVERR_ACCESS_Deny,
    NVERR_Incompatible,
    NVERR_NOT_Opened
} NVITEM_ERR_E;

typedef enum // NV items identify
{
    NV_IDENTIFIER_BASE = 0, // Don't remove or modify this line
                            // NV_IDETIFIER_BASE MUST = 0,
                            // And others MUST != 0

    // Following Item ID is used by NVEdit.exe, so don't modify
    // or change the sequence of following items, otherwise need
    // modify the relative part in NVEdit.exe.
    NV_L1_DOWNLOAD_PARAMETER = 1,
    NV_L1_CALIBRATION_PARAMETER = 2,
    NV_CLASSMARK = 3,
    NV_DSP_CODE = 4,
    NV_IMEI = 5,
    NV_AUDIO_DEVMOD_NUM = 6,
    NV_AUDIO = 7,
    NV_DSP_CONSTANT_DATA = 8,

    // Above item can't be modified. and the content of them
    // can be read anytime, We store in separate space.
    NV_FIXED_END = NV_DSP_CONSTANT_DATA, // = 0x08

    // network selection
    NV_PLMN_SELECT_MODE = 9,
    NV_USER_SERVICE_TYPE = 10,
    NV_PREF_MODE = 11,
    NV_ROAMING_MODE = 12,
    NV_BAND_SELECT = 13,

    // handset capability
    NV_BEARER_CAPABILITY = 14,
    NV_MODULE_SERIAL_NUMBER = 15,
    NV_ORIGINAL_DATE = 16,
    NV_2BIS_BA = 17,
    NV_IMEISVN = 18,
    NV_ASSERT_SMS_DEST_ADDR = 19,
    NV_ME_SMS_SUPPORTED = 20,
    NV_OS_PARAMETER = 21,
    NV_REF_PARAMETER = 22,
    NV_CHARGE = 23,

    NV_UDISK_CTRL_INFO = 24,
    NV_DOWNLOAD_RESERVED_19 = 25, //reserved!!!
    NV_SAVE_SIM_FIRST_FLAG = 26,
    NV_SELECT_BAND_STAGE = 27,
    NV_RX_EXPIATE = 28,
    NV_REPLACE_SMS_SUPPORTED = 29,

    NV_PCS_DCS_BAND_FLAG = 30,
    NV_DSP_OVERRUN_DEAL_FLAG = 31,

    // SMS setting
    NV_SMS_STATUS_REPORT_REQ = 32,
    NV_SMS_TP_MR = 33,
    NV_DEFAULT_PARAM_RECORD = 34,

    NV_VOICE_MAIL_ENABLE_FLAG = 35,
    NV_SMS_TP_MR1 = 36,

    NV_SIM_IMSI = 37,
    NV_SIM_GPRS_KC = 38,
    NV_SIM_GPRS_LOCI = 39,
    NV_STK_PROFILE = 40,

    // for at module items
    NV_DOWNLOAD_RESERVED_29 = 41,
    NV_DOWNLOAD_RESERVED_2A = 42, //reserved!!!
    NV_DOWNLOAD_RESERVED_2B = 43, //reserved!!!
    NV_SMS_IN_NV_MAX_NUM = 44,

    // sms short message content(include status TP_SCAddr and TPDU)
    NV_SMS_MESSAGE_ITEM = 45,
    NV_SMS_MESSAGE_ITEM_LAST_ENTRY = NV_SMS_MESSAGE_ITEM + MN_SMS_IN_NV_MAX_NUM - 1,

    // for record NV items used , total 100
    NV_PHONE_BOOK_MAIN, //
    NV_PHONE_BOOK_MAIN_LAST_ENTRY = NV_PHONE_BOOK_MAIN + NV_MAX_PHONE_BOOK_RECORDS - 1,
    // phone Book , missed dn , total 10
    NV_PHONE_BOOK_MDN, //
    NV_PHONE_BOOK_MDN_LAST_ENTRY = NV_PHONE_BOOK_MDN + NV_MAX_LN_RECORDS - 1,
    // phone book,   recent dn , total 10
    NV_PHONE_BOOK_RDN, //
    NV_PHONE_BOOK_RDN_LAST_ENTRY = NV_PHONE_BOOK_RDN + NV_MAX_LN_RECORDS - 1,
    // phone book,   last dialed dn , total 10
    NV_PHONE_BOOK_LND,
    NV_PHONE_BOOK_LND_LAST_ENTRY = NV_PHONE_BOOK_LND + NV_MAX_LN_RECORDS - 1,

    NV_UPDATE_FLAG = 375,
    NV_VOICE_MAIL_ENABLE_FLAG1 = 376, // for dual sim
    NV_IMEI1 = 377,                   // for dual sim
    NV_PLMN_SELECT_MODE1 = 378,       // for dual sim

    //for gprs use from 379 to 396
    NV_GPRS_USE_BASE = 379,
    NV_GPRS_PDP_CONTEXT_ID = 380,
    NV_TCPIP_DNS_ID = 381,
    NV_TCPIP_PPP_ID = 382,
    NV_GPRS_PROTOCOL_ID = 383,
    NV_GPRS_GMM_RAI_ID = 384,
    NV_GPRS_PCLINK_CFG_ID = 385,
    NV_VOICE_MAIL_ADDR_SYS1 = 386, //Line1, Line2, Data, Fax
    NV_VOICE_MAIL_ADDR_SYS2 = 387, //Line1, Line2, Data, Fax
    NV_VOICE_MAIL_ADDR_SYS3 = 388, //Line1, Line2, Data, Fax
    NV_USED_389 = 389,
    NV_IMEI2 = 390,                   // for TRI sim
    NV_PLMN_SELECT_MODE2 = 391,       // for TRI sim
    NV_VOICE_MAIL_ENABLE_FLAG2 = 392, // for TRI sim
    NV_2BIS_BA1 = 393,                // for dual sim
    NV_SIM_IMSI_1 = 394,              // for dual sim
    NV_SIM_GPRS_KC_1 = 395,           // for dual sim
    NV_SIM_GPRS_LOCI_1 = 396,         // for dual sim

    //attention!!!!
    NV_PHASE_CHECK_BACKUP = 397,     // phase check data backup
    NV_FIXNV_COPY_ID = 398,          // fix nv backup function in running nv
    NV_2BIS_BA2 = 399,               // for dual sim
    NV_PROD_PARAM = 400,             // used for products' parameters define.
    NV_BT_CONFIG = 401,              // bt config nv
    NV_BT_ADDRESS = 402,             // bt adress nv
    NV_GPS_PARAM = 403,              // gps param nv
    NV_TV_RSSI_COMP = 404,           // cmmb cali param
    NV_MULTI_LANGUE_CFG = 405,       // for multi langue config
    NV_KURO_DID = 406,               // reserved for kuro
    NV_OMA_DRM_INFO_ID = 407,        // for OMA DRM
    NV_OMA_DRM_DEFAULT_KEY_ID = 408, // add for OMA DRM
    NV_WIFI_CONFIG = 409,            // for wifi config information

    // Saving Recently five Assert Info.
    NV_ASSERT_INFO0 = 410,
    NV_ASSERT_INFO1 = 411,
    NV_ASSERT_INFO2 = 412,
    NV_ASSERT_INFO3 = 413,
    NV_ASSERT_INFO4 = 414,

    NV_AUTOSMS_XFILE_CONFIG = 415,
    NV_BT_BLE = 416,
    NV_FM_CONFIG = 417,
    NV_PTS_MODE = 418,
    NV_SIMLOCK_SIGN_ID = 419, //extended by simlock

    NV_REF_BASE = NV_REF_EXT_BASE_ID,             // extended by REF, 420
    NV_REF_EQ_HEADSET = NV_REF_EXT_BASE_ID + 1,   //421
    NV_REF_EQ_HEADFREE = NV_REF_EXT_BASE_ID + 2,  //422
    NV_REF_EQ_HANDSET = NV_REF_EXT_BASE_ID + 3,   //423
    NV_REF_EQ_HANDSFREE = NV_REF_EXT_BASE_ID + 4, //424
    NV_ARM_AUDIO_MODE_NUM = 425,
    NV_ARM_AUDIO_MODE_PARA = 426,
    NV_REF_EQ_TUNABLE = 427,
    NV_ARM_NXP_MODE_PARA = 428,
    NV_ARM_NXP_MODE_NUM = 429,
    NV_DSP_CODEC_INFO = 430,
    NV_DSP_CODEC_CONFIG = 432,
    NV_ENG_TEST_RESULT_ID = 431, // to save the test result of the engine mode
    FOTA_UPDATE_FLAG = 433,      //fota update flag, value 0 config data in fixed nv will be used, 1 use data in   s_flash_cfg_nv_addr
    //434 reserved
    NV_TP_PARAM = 435, // tp calibration parameter

    NV_EQ_SET_INFO = 439,
    NV_UNTUNABLE_EQ_SET_PARA = 440,
    NV_TUNABLE_EQ_SET_PARA = 441,
    NV_BT_SPRD = 442,         // Bluetooth for SPRD
    NV_REF_FOR_CHIP = 443,    // chip drv
    NV_REF_FOR_AV = 444,      // audio/video
    NV_REF_FOR_DRIVER = 445,  // external device driver: bt/fm/wifi/gps...
    NV_REF_FOR_MISC = 446,    // miscellaneous
    NV_REF_FOR_LICENSE = 447, // halo license storage
    NV_REF_IN_FIXNV = 448,    // need access/backup in FIXNV

    //428--448 reserved
    NV_REF_MAX = MN_ATNV_USER_BASE - 1, //449

    NV_AT_BASE = MN_ATNV_USER_BASE, // extended by ATC,
    NV_AT_MAX = NV_PS_EXT_BASE_ID - 1,

    NV_PS_BASE = NV_PS_EXT_BASE_ID,               //470     // extended by PS,
    NV_IMEI3 = NV_PS_BASE + 14,                   //484  keep no change!!
    NV_LOGSAVE_IQ_SWITCH = NV_L1_EXT_BASE_ID - 2, //488
    NV_LOGSAVE_SETTING = NV_L1_EXT_BASE_ID - 1,   //489
    NV_PS_MAX = NV_LOGSAVE_SETTING,

    NV_L1_BASE = NV_L1_EXT_BASE_ID, // extended by L1, limited to 10

    NV_L1_MAX = NV_GPS_GE2_BASE_ID - 1,
    //for ge2
    NV_GPS_GE2_BASE = NV_GPS_GE2_BASE_ID,
    NV_GPS_GE2_MAX = MN_NV_ADUPS_BASE - 1,

#ifdef HAS_MODEM_NV
    NV_MODEM_NV_BASE = NV_MODEM_NV_BASE_ID,
    NV_MODEM_STATIC_NV = NV_MODEM_NV_BASE + 1,
    NV_MODEM_PHY_NV = NV_MODEM_NV_BASE + 2,
    NV_MODEM_DYNAMIC_NV = NV_MODEM_NV_BASE + 3,
    NV_MODEM_STATIC_CARD2_NV = NV_MODEM_NV_BASE + 4,
    NV_MODEM_PHY_CARD2_NV = NV_MODEM_NV_BASE + 5,
    NV_MODEM_DYNAMIC_CARD2_NV = NV_MODEM_NV_BASE + 6,

    //if need nv modify in running,please set the id to hehind 630
    NV_MODEM_RF_NV = 620,
    NV_8910_FIXNV_START = 621,
    NV_MODEM_RF_CALIB_GSM = NV_8910_FIXNV_START,
    NV_MODEM_RF_CALIB_LTE = 622,
    NV_MODEM_RF_GOLDEN_BOARD = 623,
    NV_8910_FIXNV_END = NV_MODEM_RF_GOLDEN_BOARD,
    NV_MODEM_CFW_NV = 631,
    NV_MODEM_AUD_CALIB = 632,

    NV_MDOEM_NV_MAX = NV_MODEM_NV_BASE_ID + NV_MODEM_NV_MAX_NUM - 1,
#endif

    NV_USER_BASE = MN_NV_USER_BASE, // 650 now in 2.5g ps other may change

    NV_TD_PS_BASE = NV_TD_PS_EXT_BASE_ID, // extended by 3G PS
    NV_TD_PS_MAX = MN_NV_TD_USER_BASE - 1,

    /* NV item for TD calibration*/
    NV_L1_CALIBRATION_PARAMETER_TD = NV_TD_CALIBRATION_BASE + 0x2,
    NV_L1_CALIBRATION_PARAMETER2_TD = NV_TD_CALIBRATION_BASE + 0x3,

#ifdef UMEM_SUPPORT
#ifdef UMEM_NEW_MECHNISM
    NV_UDISK_BASE = NV_USER_BASE + MAX_NV_USER_ITEM_NUM,
#endif
#endif

#ifdef UMEM_SUPPORT
#ifdef UMEM_NEW_MECHNISM
    LAST_NV_ITEM_NUM = NV_USER_BASE + MAX_NV_USER_ITEM_NUM + NV_UDISK_MAX_NUM /* used in nor platfomr, important!!!!*/
#endif
#else
    LAST_NV_ITEM_NUM = NV_USER_BASE + MAX_NV_USER_ITEM_NUM /* used in nor platfomr, important!!!!*/
#endif

} NV_ITEM_ID_E;


typedef struct
{
    BD_ADDR_T deviceAddr;
    uint8 linkkeyValid;
    uint8 linkkeyLen;
    uint8 LinkKeyType;
    deviceLinkkey_t Linkkey;
    deviceName_t remoteName;
    BOOLEAN is_remoteName_local_set;
    classOfDevice_t classOfDevice;
    uint32 knownServices11_00_31;
    uint32 knownServices11_32_63;
    uint32 knownServices12_00_31;
    uint32 knownServices13_00_31;
    uint8 authorised;
} scDbDeviceRecord_t;

BOOLEAN BT_UpdateAddrToNV(BT_ADDRESS *addr);
BT_STATUS DB_GetLocalNameUTF8(uint8 *name);

BT_STATUS DB_GetLeNameUTF8(uint8 *name);

uint32 GetHostNvCtrl(void);
BOOLEAN DB_Init(void);

uint16 DB_SetLocalName(const uint16 *local_name);
uint16 DB_GetLocalName(uint16 *local_name);
BT_STATUS DB_GetLocalNameUTF8(uint8 *name);
uint16 DB_GetLeName(uint16 *le_name);
BT_STATUS DB_GetLeNameUTF8(uint8 *name);

void DB_SaveLocalNameAndToUTF8(const uint16 *local_name, uint8 *name_utf8);
void DB_SetVisibility(uint8 visibility);
uint8 DB_GetVisibility(void);
uint8 DB_RemovePairedInfo(bdaddr_t addr);
uint8 DB_GetPairedNum(uint32 service_type);
uint8  DB_GetPairedInfoFormIndex(uint8 index, bt_dm_device_t *bt_dm_device);
void DB_GetLocalPublicAddr(BT_ADDRESS* bd_addr);
uint32 UTF8toUnicode(const uint8 *utf8, uint16 utf8_length, uint16 *unicode, uint16 unicode_length);
uint32 UnicodetoUTF8(const uint16 *unicode, uint16 unicode_length, uint8 *utf8, uint16 utf8_length);
uint16 PF_WStrCmp(const uint16 *Str1, const uint16 *Str2);
uint16 *PF_WStrCpy( uint16 *dst, const uint16 *src);
uint16 *PF_WStrCat(uint16 *dst, const uint16 *src);
uint16 PF_WStrLen(const uint16 *Str);
char PF_ToLower(char ch);
const uint16 *PF_WStrStr(const uint16 *string, const uint16 *strCharSet);
void Wchars_BeToHost(uint16 *wstr);
void Wchars_HostToBe(uint16 *wstr);

uint8 DB_GetWhiteNun(void);
uint8 DB_GetWhiteInfo(uint8 index, st_white_list_info *white_list_info);
uint8 DB_AddWhiteInfo(st_white_list_info *WhiteInfo);
uint8 DB_RemoveWhiteInfo(st_white_list_info *WhiteInfo);
uint8 DB_GetAddrTypeFromAddr(st_white_list_info *WhiteInfo);
void DB_ClearWhitelist(void);
BT_STATUS BT_UpdateNV(void);

NVITEM_ERR_E EFS_NvitemRead(INPUT uint16 ItemID, INPUT uint16 cchSize,
                            OUTPUT uint8 *pBuf);
NVITEM_ERR_E EFS_NvRead(uint16 identifier, uint16 count, uint8 *buf_ptr);
NVITEM_ERR_E NV_UpdateCali(uint16 ItemID, uint16 cchSize, const void *pBuf);

uint8 DB_SaveSmpKeyInfo(sm_le_device_info_t *device_info);
uint8 DB_RemoveSmpKeyInfo(bdaddr_t addr);

void DB_GetSmpKeyInfo(sm_le_device_info_t *device_list);
#endif
