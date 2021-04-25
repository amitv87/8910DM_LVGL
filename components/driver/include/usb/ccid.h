/* Copyright (C) 2020 RDA Technologies Limited and/or its affiliates("RDA").
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

#ifndef _USB_CCID_H_
#define _USB_CCID_H_

#include <stdint.h>

#define CCID_DESCRIPTOR_LENGTH 0x36
typedef struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bcdCCID;   // 0110h
    uint8_t bMaxSlotIndex;
    uint8_t bVoltageSupport;
    uint32_t dwProtocols;
    uint32_t dwDefaultClock;
    uint32_t dwMaximumClock;
    uint8_t bNumClockSupported;
    uint32_t dwDataRate;
    uint32_t dwMaxDataRate;
    uint8_t bNumDataRatesSupported;
    uint32_t dwMaxIFSD;
    uint32_t dwSynchProtocols;
    uint32_t dwMechanical;
    uint32_t dwFeatures;
    uint32_t dwMaxCCIDMessageLength;
    uint8_t bClassGetResponse;
    uint8_t bClassEnvelope;
    uint16_t wLcdLayout;
    uint8_t bPINSupport;
    uint8_t bMaxCCIDBusySlots;
} __attribute__((packed)) ccidClassDesc_t;

#define CCID_0110   0x0110
#define CCID_DESCRIPTOR_TYPE 0x21
#define SMART_CARD_DEVICE_CLASS 0x0b

// Table 5.3-1 Summary of CCID Class Specific Request
#define CCIDGENERICREQ_ABORT                    0x01
#define CCIDGENERICREQ_GET_CLOCK_FREQUENCIES    0x02
#define CCIDGENERICREQ_GET_DATA_RATES           0x03

// voltage
#define CCID_VOLTAGE_5V  (1 << 0)
#define CCID_VOLTAGE_3V  (1 << 1)
#define CCID_VOLTAGE_1V8 (1 << 2)

// mechanical
#define CCID_MECHANICAL_NO_SPECIAL       (0)
#define CCID_MECHANICAL_CARD_ACCEPT      (1 << 0)
#define CCID_MECHANICAL_CARD_EJECTION    (1 << 1)
#define CCID_MECHANICAL_CARD_CAPTURE     (1 << 2)
#define CCID_MECHANICAL_CARD_LOCK_UNLOCK (1 << 3)

// feature
#define CCID_FEATURE_NO_SPECIAL (0)
// Automatic parameter configuration based on ATR data
#define CCID_FEATURE_AUTO_PCONF (1 << 1)
// Automatic activation of ICC on inserting
#define CCID_FEATURE_AUTO_ACTIV (1 << 2)
// Automatic ICC voltage selection
#define CCID_FEATURE_AUTO_VOLT  (1 << 3)
// Automatic ICC clock frequency change
#define CCID_FEATURE_AUTO_CLOCK (1 << 4)
// Automatic baud rate change
#define CCID_FEATURE_AUTO_BAUD  (1 << 5)
// Automatic parameters negotiation made by the CCID
#define CCID_FEATURE_AUTO_PNEGO (1 << 6)
// Automatic PPS made by the CCID according to the active paramters
#define CCID_FEATURE_AUTO_PPS   (1 << 7)
// CCID can set ICC in clock stop mode
#define CCID_FEATURE_ICCSTOP    (1 << 8)
// NAD value other the 00 accepted (T=1 protocol in use)
#define CCID_FEATURE_NAD        (1 << 9)
// Automatic IFSD exchange as first exchange (T=1 protocol in use)
#define CCID_FEATURE_AUTO_IFSD  (1 << 10)
// TPDU level exchanges with CCID
#define CCID_FEATURE_EXC_TPDU   (1 << 16)
// Short APDU level exchange with CCID
#define CCID_FEATURE_EXC_SAPDU  (1 << 17)
// Short and Extended APDI level exchange with CCID
#define CCID_FEATURE_EXC_APDU   (1 << 18)
// USB Wake up signaling supported on card insertion and removal
#define CCID_FEATURE_WAKEUP     (1 << 20)

// protocol
#define CCID_PROTOCOL_NO_SEL 0x00
#define CCID_PROTOCOL_T0     0x01
#define CCID_PROTOCOL_T1     0x02

// pin support
#define CCID_PINSUPPORT_NONE         0x00
#define CCID_PINSUPPORT_VERIFICATION (1 << 1)
#define CCID_PINSUPPORT_MODIFICATION (1 << 2)

#endif
