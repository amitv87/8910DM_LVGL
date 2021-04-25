/**
 * @file    bt_types.h
 * @brief    -
 * @details
 * @mainpage
 * @version  V1.0.0
 * @date     2019-10-11
 * @license  Copyright (C) 2019 Unisoc Communications Inc
 */


#ifndef __BT_HAL_HCI_TYPES_H__
#define __BT_HAL_HCI_TYPES_H__

/**the range of values for each type*/
#define MAX_INT8                    127
#define MIN_INT8                    (-MAX_INT8 - 1)
#define MAX_UINT8                   255
#define MIN_UINT8                   0
#define MAX_INT16                   32767
#define MIN_INT16                   (-MAX_INT16 - 1)
#define MAX_UINT16                  65535
#define MIN_UINT16                  0
#define MAX_INT32                   2147483647L
#define MIN_INT32                   (-MAX_INT32 - 1)
#define MAX_UINT32                  4294967295U
#define MIN_UINT32                  0U

/**commom error definitions*/
#define pENOERROR       0
#define pENOMEMORY      1
#define pENORESOURCE    2
#define pEINVALIDPARM   3
#define pEWOULDBLOCK    4
#define pELOCKERROR     5
#define pEIOERROR       6
#define pENOTSUPPORTED  7
#define pEUNKNOWNERROR  8

#define SIZE_OF_BDADDR      6	///< the number of bytes of the address
#define SIZE_OF_LINKKEY     16	///< the number of bytes of the linkkey
#define SIZE_OF_DHKEY   24
#define LINKKEY_SIZE 16
#define LE_MAX_REMOTE_NAME_LEN 32

typedef unsigned long long      UINT64;
typedef unsigned int            UINT32;
typedef unsigned short          UINT16;
typedef long long               INT64;
typedef int                     INT32;
typedef short                   INT16;
typedef unsigned char           UINT8;
typedef char                    INT8;
typedef unsigned char           BOOL;
typedef unsigned int            HANDLE;

typedef unsigned short          channel_id;

/**Bluetooth address structure*/
typedef struct
{
    UINT8 bytes[SIZE_OF_BDADDR];  /**< Bluetooth address */
} bdaddr_t;

typedef struct
{
    UINT8 *buf;
    UINT8 transport; ///< the type of the data_buf
    UINT16 dataLen;
    UINT16 bufLen;
} data_buf_t;

typedef struct
{
    UINT8 *data;
    UINT8 transport;			///< not use
    UINT16 len;
    UINT16 size;
    UINT16 flags;				///< flow direction of data:host to control or control to host
    UINT16 hci_handle_flags;	///< handle of ACL packet
    UINT8 hci_type;				///< the type of the hci_data
    UINT8 *tail;				///< used by controll to free buffer
} hci_data_t;


typedef hci_data_t hci_cmd_t;

#ifndef false
#define false 0
#endif
#ifndef true
#define true 1
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef BD_ADDR_CMP
#define BD_ADDR_CMP(x,y) !memcmp(((x).bytes), ((y).bytes), SIZE_OF_BDADDR)
#endif


#define BT_HOST_BUF_HCI_HEADER_PRESENT 0x0001
#define BT_HOST_BUF_USE_RESV_ACL_MEM   0x0002
/* is this packet a packet allocated on the way out or way in? */
/* 1 is a packet from the queues */
#define HCI_BUF_OUTGOING_PACKET_MASK    0x0004
#define HCI_BUF_INCOMING_PACKET_MASK    0x0008

#define BT_HOST_BUF_URGENCY_PACKET       0x0010

#define BT_HANDLE_FLAG_PICONET_BROADCAST 0x8000
#define BT_HANDLE_FLAG_ACTIVE_BROADCAST  0x4000
#define BT_HANDLE_FLAG_START_FRAGMENT	0x2000
#define BT_HANDLE_FLAG_CONTINUE_FRAGMENT	0x1000

#define BT_USER_ENDED_CONNECTION		0x13
#define BT_LOW_RESOURCES				0x14
#define BT_ABOUT_TO_POWER_OFF        0x15
#define BT_TERMINATED_BY_LOCAL_HOST	0x16

#define MAX_PIN_LEN                  16
#define DEFAULT_FLUSH_TIMEOUT        0x0000
#define MAX_FLUSH_TIMEOUT            0x07FF
#define DEFAULT_LINK_SUP_TIMEOUT     0x7D00
#define MAX_DEFAULT_LINK_SUP_TIMEOUT 0xFFFF

/* The local and remote names are truncated to the defines below - see GAP spec. */
#define MGR_MAX_REMOTE_NAME_LEN             32
#define MGR_MAX_LOCAL_NAME_LEN              32


enum
{
    ///Public BD address
    ADDR_TYPE_PUBLIC                   = 0x00,
    ///Random BD Address
    ADDR_TYPE_RAND,
    /// Controller generates Resolvable Private Address based on the
    /// local IRK from resolving list. If resolving list contains no matching
    /// entry, use public address.
    ADDR_TYPE_RPA_OR_PUBLIC,
    /// Controller generates Resolvable Private Address based on the
    /// local IRK from resolving list. If resolving list contains no matching
    /// entry, use random address.
    ADDR_TYPE_RPA_OR_RAND,
};


/**All state of bluetooth*/
typedef enum
{
    BT_SUCCESS,                        /**< The status is success */
    BT_FAIL,                               /**< The status is fail */
    BT_DONE,                             /**< The status is done */
    BT_PENDING,                        /**< The status is pending */
    BT_INVALIDPARAM,              /**< One or more input parameters are invalidparam */
    BT_SECFAIL,                         /**< The security connection is fail */
    BT_FLOWCTRL,                     /**< The status is flow control */
    BT_NORESOURCES,               /**< The resources are not enough */
    BT_UNSUPPORTED,               /**< The device or capability are not supported */
    BT_HWERROR,                      /**< There are one or more error in hard ware */
    BT_HOSTERROR,                   /**< There are one or more error in host */
    BT_UNKNOWNERROR,           /**< There are one or more error in unknow place */
    BT_NOT_READY,                   /**< The BT is not ready to do things */
    BT_RETRY,                           /**< The BT is ready to do things */
    BT_AUTHORISATIONFAIL,    /**< The authorisation is fail */
    BT_AUTHENTICATIONFAIL,   /**< The authentication is fail */
    BT_ENCRYPTFAIL,                /**< There are one or more error in encryption */
    BT_TIMEOUT,                       /**< The status is timeout */
    BT_PROTOERROR,               /**< There are one or more protoerror */
    BT_DISALLOWED,                 /**< The status is not allowed */
    BT_BUFTOOSMALL,               /**< The buff is too small to do things */
    BT_DISCONNECTED,             /**< The bt is disconnect with other necessary devices */
    BT_INVALIDERRORCODE,     /**< There are one or more invalid error code */
} bt_status_t;

#define HCI_TYPE_CMD 0x01  ///< HCI CMD
#define HCI_TYPE_ACL 0x02  ///< ACL
#define HCI_TYPE_SCO 0x03  ///< SCO
#define HCI_TYPE_EVT 0x04  ///< HCI Event

#define BT_GIAC_LAP  0x9E8B33
#define BT_LIAC_LAP  0x9E8B00
#ifndef GIAC_LAP
#define GIAC_LAP  BT_GIAC_LAP
#endif

#ifndef BD_ADDR_CMP /* An optimised version may be defined in platform_operations.h */
#define BD_ADDR_CMP(x,y) !memcmp(((x).bytes), ((y).bytes), SIZE_OF_BDADDR)
#endif

#define CASE_RETURN_STR(const) case const: return #const;

#define MAX_BT_DEVICE_NAME      32  ///< max length of device name
#define MAX_LE_DEVICE_NAME		22

#define BT_LE_MESH_SUPPORT		0

#define BT_SLOTS_TO_MSEC(slots) ((slots) * 5 / 8)
#define BT_MSEC_TO_SLOTS(msec)  ((UINT32)((msec) * 8 / 5))



/* Class of Device
*  | Major Service Classes | Major Device Classes | Minor Device Class field |
*  | BIT 23 - 13           | BIT 12 - 8           | BIT 7 = 2                |
*/
/* Major Service Classes: bits 23-13 */
#define COD_SERVICE_LIMITED_DISCOVERY_MODE                          (1 << 13) // 0x002000
#define COD_SERVICE_POSITIONING                                     (1 << 16) // 0x010000, Location identification
#define COD_SERVICE_NETWORKING                                      (1 << 17) // 0x020000, LAN, Ad hoc, ...
#define COD_SERVICE_RENDERING                                       (1 << 18) // 0x040000, Printing, Speaker, ...
#define COD_SERVICE_CAPTURING                                       (1 << 19) // 0x080000, Scaner
#define COD_SERVICE_OBJECT_TRANSFER                                 (1 << 20) // 0x100000, v-Inbox, v-Folder,..
#define COD_SERVICE_AUDIO                                           (1 << 21) // 0x200000, Speaker, Microphone, Headset service, ...
#define COD_SERVICE_TELEPHONY                                       (1 << 22) // 0x400000, Cordless telephony, Modem, Headset service, ...
#define COD_SERVICE_INFORMATION                                     (1 << 23) // 0x800000,  WEB-servcer, WAP-server, ...

/* Major Device Classes: bits 12-8 */
#define COD_MAJOR_DEVICE_MISCELLANEOUS                              0x000000
#define COD_MAJOR_DEVICE_COMPUTER                                   0x000100 // desktop, notebook, PDA, organizer, ...
#define COD_MAJOR_DEVICE_PHONE                                      0x000200 // cellular, cordiess, pay phone, modem, ...
#define COD_MAJOR_DEVICE_LANACCESPOINT                              0x000300 // LAN/Network Access point
#define COD_MAJOR_DEVICE_AUDIO                                      0x000400 // Aduio/Video (Headset, speaker, stereo, video display, VCR), ...
#define COD_MAJOR_DEVICE_PERIPHERAL                                 0x000500 // mouse, joystick, keyboard, ...
#define COD_MAJOR_DEVICE_IMAGING                                    0x000600 // printer,scaner, camera, display, ...
#define COD_MAJOR_DEVICE_WEARABLE                                   0x000700
#define COD_MAJOR_DEVICE_TOY                                        0x000800
#define COD_MAJOR_DEVICE_HEALTH                                     0x000900
#define COD_MAJOR_DEVICE_UNCATEGORIZED                              0x001F00 // device code not specified

/* Minor Device Classes: bits 7-2 */
/* Minor Device Class Field - Computer Major Class */
#define COD_MINOR_DEVICE_COMPUTER_UNCATEGORIZED                     0x000000 // device code not specified
#define COD_MINOR_DEVICE_COMPUTER_DESKTOP                           0x000004 // Desktop workstation
#define COD_MINOR_DEVICE_COMPUTER_SERVER                            0x000008 // Server-class computer
#define COD_MINOR_DEVICE_COMPUTER_LAPTOP                            0x00000C // Laptop
#define COD_MINOR_DEVICE_COMPUTER_HANDHELD_PC                       0x000010 // Handhel PC/PDA(clamshell)
#define COD_MINOR_DEVICE_COMPUTER_PALMSIZED_PC                      0x000014 // Palm-Size PC/PDA
#define COD_MINOR_DEVICE_COMPUTER_WEARABLE                          0x000018 // Wearble computer (watch size)
#define COD_MINOR_DEVICE_COMPUTER_TABLET                            0x00001C // Tablet

/* Minor Device Class Field - Phone Major Class */
#define COD_MINOR_DEVICE_PHONE_UNCATEGORIZED                        0x000000
#define COD_MINOR_DEVICE_PHONE_CELLULAR                             0x000004
#define COD_MINOR_DEVICE_PHONE_CORDLESS                             0x000008
#define COD_MINOR_DEVICE_PHONE_SMARTPHONE                           0x00000C
#define COD_MINOR_DEVICE_PHONE_WIREDMODEMORGW                       0x000010 // Wired modem or voice gateway
#define COD_MINOR_DEVICE_PHONE_COMMON_ISDN_ACCESS                   0x000014

/* Minor Device Class Field - LAN AP Major Class: bits 7 - 5*/
#define COD_MINOR_DEVICE_LANAP_FULLYAVAILABLE                       0x000000
#define COD_MINOR_DEVICE_LANAP_1_17_UTILISED                        0x000004
#define COD_MINOR_DEVICE_LANAP_17_33_UTILISED                       0x000008
#define COD_MINOR_DEVICE_LANAP_33_50_UTILISED                       0x00000C
#define COD_MINOR_DEVICE_LANAP_50_67_UTILISED                       0x000010
#define COD_MINOR_DEVICE_LANAP_67_83_UTILISED                       0x000014
#define COD_MINOR_DEVICE_LANAP_83_99_UTILISED                       0x000018
#define COD_MINOR_DEVICE_LANAP_NO_SERVICE_AVAILABLE                 0x00001C
/* Minor Device Class Field - LAN AP Major Class: bits 4 - 2*/
#define COD_MINOR_DEVICE_LANAP_UNCATEGORIZED                        0x000000

/* Minor Device Class Field - Audio/Video Major Class */
#define COD_MINOR_DEVICE_AUDIO_UNCATEGORIZED                        0x000000
#define COD_MINOR_DEVICE_AUDIO_WEARABLE_HEADSET_DEVICE              0x000004
#define COD_MINOR_DEVICE_AUDIO_HANDSFREE_DEVICE                     0x000008
#define COD_MINOR_DEVICE_AUDIO_MICROPHONE                           0x000010
#define COD_MINOR_DEVICE_AUDIO_LOUDSPEAKER                          0x000014
#define COD_MINOR_DEVICE_AUDIO_HEADPHONES                           0x000018
#define COD_MINOR_DEVICE_AUDIO_PORTABLE_AUDIO                       0x00001c
#define COD_MINOR_DEVICE_AUDIO_CAR_AUDIO                            0x000020
#define COD_MINOR_DEVICE_AUDIO_SETTOP_BOX                           0x000024
#define COD_MINOR_DEVICE_AUDIO_HIFI_AUDIO                           0x000028
#define COD_MINOR_DEVICE_AUDIO_VCR                                  0x00002C
#define COD_MINOR_DEVICE_AUDIO_VIDEO_CAMERA                         0x000030
#define COD_MINOR_DEVICE_AUDIO_CAMCORDER                            0x000034
#define COD_MINOR_DEVICE_AUDIO_VIDEO_MONITOR                        0x000038
#define COD_MINOR_DEVICE_AUDIO_VIDEO_DISPLAY_AND_LOUDSPEAKER        0x00003C
#define COD_MINOR_DEVICE_AUDIO_VIDEO_CONFERENCING                   0x000040
#define COD_MINOR_DEVICE_AUDIO_GAMING_TOY                           0x000040




#endif
