/*	$NetBSD: usbcdc.h,v 1.9 2004/10/23 13:24:24 augustss Exp $	*/
/*	$FreeBSD$	*/

/*-
 * Copyright (c) 1998 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Lennart Augustsson (lennart@augustsson.net) at
 * Carlstedt Research & Technology.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _USB_CDC_H_
#define	_USB_CDC_H_

#include <stdint.h>

#define	UDESCSUB_CDC_HEADER	0
#define	UDESCSUB_CDC_CM		1	/* Call Management */
#define	UDESCSUB_CDC_ACM	2	/* Abstract Control Model */
#define	UDESCSUB_CDC_DLM	3	/* Direct Line Management */
#define	UDESCSUB_CDC_TRF	4	/* Telephone Ringer */
#define	UDESCSUB_CDC_TCLSR	5	/* Telephone Call */
#define	UDESCSUB_CDC_UNION	6
#define	UDESCSUB_CDC_CS		7	/* Country Selection */
#define	UDESCSUB_CDC_TOM	8	/* Telephone Operational Modes */
#define	UDESCSUB_CDC_USBT	9	/* USB Terminal */
#define	UDESCSUB_CDC_NCT	10
#define	UDESCSUB_CDC_PUF	11
#define	UDESCSUB_CDC_EUF	12
#define	UDESCSUB_CDC_MCMF	13
#define	UDESCSUB_CDC_CCMF	14
#define	UDESCSUB_CDC_ENF	15	/* ethernet desc */
#define	UDESCSUB_CDC_ANF	16

#define UCDC_PROTO_NONE			0
#define UCDC_ACM_PROTO_AT_V25TER		1
#define UCDC_ACM_PROTO_AT_PCCA101		2
#define UCDC_ACM_PROTO_AT_PCCA101_WAKE	3
#define UCDC_ACM_PROTO_AT_GSM		4
#define UCDC_ACM_PROTO_AT_3G			5
#define UCDC_ACM_PROTO_AT_CDMA		6
#define UCDC_ACM_PROTO_VENDOR		0xff

#define USB_CDC_PROTO_EEM			7

#define USB_CDC_NCM_PROTO_NTB			1
#define USB_CDC_MBIM_PROTO_NTB			2

struct usb_cdc_header_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint16_t bcdCDC;
} __attribute__((packed));

struct usb_cdc_cm_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bmCapabilities;
#define	USB_CDC_CM_DOES_CM		0x01
#define	USB_CDC_CM_OVER_DATA		0x02
    uint8_t bDataInterface;
} __attribute__((packed));

struct usb_cdc_acm_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bmCapabilities;
#define	USB_CDC_ACM_HAS_FEATURE		0x01
#define	USB_CDC_ACM_HAS_LINE		0x02
#define	USB_CDC_ACM_HAS_BREAK		0x04
#define	USB_CDC_ACM_HAS_NETWORK_CONN	0x08
} __attribute__((packed));

struct usb_cdc_union_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bMasterInterface;
    uint8_t bSlaveInterface[1];
} __attribute__((packed));

struct usb_cdc_ethernet_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t iMacAddress;
    uint32_t bmEthernetStatistics;
    uint16_t wMaxSegmentSize;
    uint16_t wNumberMCFilters;
    uint8_t bNumberPowerFilters;
} __attribute__((packed));

#define	UCDC_SEND_ENCAPSULATED_COMMAND	0x00
#define	UCDC_GET_ENCAPSULATED_RESPONSE	0x01
#define	UCDC_SET_COMM_FEATURE		0x02
#define	UCDC_GET_COMM_FEATURE		0x03
#define	UCDC_ABSTRACT_STATE		0x01
#define	UCDC_COUNTRY_SETTING		0x02
#define	UCDC_CLEAR_COMM_FEATURE		0x04
#define	UCDC_SET_LINE_CODING		0x20
#define	UCDC_GET_LINE_CODING		0x21
#define	UCDC_SET_CONTROL_LINE_STATE	0x22
#define	UCDC_LINE_DTR			0x0001
#define	UCDC_LINE_RTS			0x0002
#define	UCDC_SEND_BREAK			0x23
#define	UCDC_BREAK_ON			0xffff
#define	UCDC_BREAK_OFF			0x0000

struct usb_cdc_abstract_state {
    uint16_t wState;
#define	UCDC_IDLE_SETTING		0x0001
#define	UCDC_DATA_MULTIPLEXED		0x0002
} __attribute__((packed));

#define	UCDC_ABSTRACT_STATE_LENGTH	2

struct usb_cdc_line_state {
    uint32_t dwDTERate;
    uint8_t bCharFormat;
#define	UCDC_STOP_BIT_1			0
#define	UCDC_STOP_BIT_1_5		1
#define	UCDC_STOP_BIT_2			2
    uint8_t bParityType;
#define	UCDC_PARITY_NONE		0
#define	UCDC_PARITY_ODD			1
#define	UCDC_PARITY_EVEN		2
#define	UCDC_PARITY_MARK		3
#define	UCDC_PARITY_SPACE		4
    uint8_t bDataBits;
} __attribute__((packed));

#define	UCDC_LINE_STATE_LENGTH		7

struct usb_cdc_notification {
    uint8_t bmRequestType;
#define	UCDC_NOTIFICATION		0xa1
    uint8_t bNotification;
#define	UCDC_N_NETWORK_CONNECTION	0x00
#define	UCDC_N_RESPONSE_AVAILABLE	0x01
#define	UCDC_N_AUX_JACK_HOOK_STATE	0x08
#define	UCDC_N_RING_DETECT		0x09
#define	UCDC_N_SERIAL_STATE		0x20
#define	UCDC_N_CALL_STATE_CHANGED	0x28
#define	UCDC_N_LINE_STATE_CHANGED	0x29
#define	UCDC_N_CONNECTION_SPEED_CHANGE	0x2a
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
    uint8_t data[16];
} __attribute__((packed));

#define	UCDC_NOTIFICATION_LENGTH	8

/*
 * Bits set in the SERIAL STATE notification (first byte of data)
 */

#define	UCDC_N_SERIAL_OVERRUN		0x40
#define	UCDC_N_SERIAL_PARITY		0x20
#define	UCDC_N_SERIAL_FRAMING		0x10
#define	UCDC_N_SERIAL_RI		0x08
#define	UCDC_N_SERIAL_BREAK		0x04
#define	UCDC_N_SERIAL_DSR		0x02
#define	UCDC_N_SERIAL_DCD		0x01

/* Serial state bit masks */
#define	UCDC_MDM_RXCARRIER		0x01
#define	UCDC_MDM_TXCARRIER		0x02
#define	UCDC_MDM_BREAK			0x04
#define	UCDC_MDM_RING			0x08
#define	UCDC_MDM_FRAMING_ERR		0x10
#define	UCDC_MDM_PARITY_ERR		0x20
#define	UCDC_MDM_OVERRUN_ERR		0x40

/*
 * Network Control Model, NCM16 + NCM32, protocol definitions
 */
struct usb_ncm16_hdr {
    uint32_t dwSignature;
    uint16_t wHeaderLength;
    uint16_t wSequence;
    uint16_t wBlockLength;
    uint16_t wDptIndex;
} __attribute__((packed));

struct usb_ncm16_dp {
    uint16_t wFrameIndex;
    uint16_t wFrameLength;
} __attribute__((packed));

struct usb_ncm16_dpt {
    uint32_t dwSignature;
    uint16_t wLength;
    uint16_t wNextNdpIndex;
    struct usb_ncm16_dp dp[0];
} __attribute__((packed));

struct usb_ncm32_hdr {
    uint32_t dwSignature;
    uint16_t wHeaderLength;
    uint16_t wSequence;
    uint32_t dwBlockLength;
    uint32_t dwDptIndex;
} __attribute__((packed));

struct usb_ncm32_dp {
    uint32_t dwFrameIndex;
    uint32_t dwFrameLength;
} __attribute__((packed));

struct usb_ncm32_dpt {
    uint32_t dwSignature;
    uint16_t wLength;
    uint16_t wReserved6;
    uint32_t dwNextNdpIndex;
    uint32_t dwReserved12;
    struct usb_ncm32_dp dp[0];
} __attribute__((packed));

/* Communications interface class specific descriptors */

#define	UCDC_NCM_FUNC_DESC_SUBTYPE	0x1A

struct usb_ncm_func_descriptor {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bcdNcmVersion[2];
    uint8_t bmNetworkCapabilities;
#define	UCDC_NCM_CAP_FILTER	0x01
#define	UCDC_NCM_CAP_MAC_ADDR	0x02
#define	UCDC_NCM_CAP_ENCAP	0x04
#define	UCDC_NCM_CAP_MAX_DATA	0x08
#define	UCDC_NCM_CAP_CRCMODE	0x10
#define	UCDC_NCM_CAP_MAX_DGRAM	0x20
} __attribute__((packed));

/* Communications interface specific class request codes */

#define	UCDC_NCM_SET_ETHERNET_MULTICAST_FILTERS			0x40
#define	UCDC_NCM_SET_ETHERNET_POWER_MGMT_PATTERN_FILTER		0x41
#define	UCDC_NCM_GET_ETHERNET_POWER_MGMT_PATTERN_FILTER		0x42
#define	UCDC_NCM_SET_ETHERNET_PACKET_FILTER			0x43
#define	UCDC_NCM_GET_ETHERNET_STATISTIC				0x44
#define	UCDC_NCM_GET_NTB_PARAMETERS				0x80
#define	UCDC_NCM_GET_NET_ADDRESS				0x81
#define	UCDC_NCM_SET_NET_ADDRESS				0x82
#define	UCDC_NCM_GET_NTB_FORMAT					0x83
#define	UCDC_NCM_SET_NTB_FORMAT					0x84
#define	UCDC_NCM_GET_NTB_INPUT_SIZE				0x85
#define	UCDC_NCM_SET_NTB_INPUT_SIZE				0x86
#define	UCDC_NCM_GET_MAX_DATAGRAM_SIZE				0x87
#define	UCDC_NCM_SET_MAX_DATAGRAM_SIZE				0x88
#define	UCDC_NCM_GET_CRC_MODE					0x89
#define	UCDC_NCM_SET_CRC_MODE					0x8A

struct usb_ncm_parameters {
    uint16_t wLength;
    uint16_t bmNtbFormatsSupported;
#define	UCDC_NCM_FORMAT_NTB16	0x0001
#define	UCDC_NCM_FORMAT_NTB32	0x0002
    uint32_t dwNtbInMaxSize;
    uint16_t wNdpInDivisor;
    uint16_t wNdpInPayloadRemainder;
    uint16_t wNdpInAlignment;
    uint16_t wReserved14;
    uint32_t dwNtbOutMaxSize;
    uint16_t wNdpOutDivisor;
    uint16_t wNdpOutPayloadRemainder;
    uint16_t wNdpOutAlignment;
    uint16_t wNtbOutMaxDatagrams;
} __attribute__((packed));

/* Communications interface specific class notification codes */
#define	UCDC_NCM_NOTIF_NETWORK_CONNECTION	0x00
#define	UCDC_NCM_NOTIF_RESPONSE_AVAILABLE	0x01
#define	UCDC_NCM_NOTIF_CONNECTION_SPEED_CHANGE	0x2A

#endif /* _USB_CDC_H_ */

