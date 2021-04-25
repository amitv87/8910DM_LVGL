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

#ifndef _USB_CCID_MESSAGE_H_
#define _USB_CCID_MESSAGE_H_

#include <stdint.h>

#define CCID_RDRTOPC_NOTIFY_SLOT_CHANGE 0x50
#define CCID_RDRTOPC_HARDWARE_ERROR 0x51

#define ABDATA_SIZE 512

// refer to ccid spec 6.3
struct ccid_notification {
	uint8_t bMessageType;
	union
	{
		uint8_t buf[3];
		struct {
			uint8_t bSlot;
			uint8_t bSeq;
			uint8_t bHardwareErrorCode;
		};
	};
} __attribute__((packed));

struct ccid_bulk_in_header {
	uint8_t bMessageType;
	uint32_t dwLength;
	uint8_t bSlot;
	uint8_t bSeq;
	uint8_t bStatus;
	uint8_t bError;
	uint8_t bSpecific;
	uint8_t abData[ABDATA_SIZE];
	uint8_t bSizeToSend;
} __attribute__((packed));

struct ccid_bulk_out_header {
	uint8_t bMessageType;
	uint32_t dwLength;
	uint8_t bSlot;
	uint8_t bSeq;
	uint8_t bSpecific_0;
	uint8_t bSpecific_1;
	uint8_t bSpecific_2;
	uint8_t APDU[ABDATA_SIZE];
} __attribute__((packed));

#endif