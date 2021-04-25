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

#ifndef _NETDEV_INTERFACE_NAT_LAN_H_
#define _NETDEV_INTERFACE_NAT_LAN_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "osi_compiler.h"

OSI_EXTERN_C_BEGIN

#include "drv_ether.h"

/**
 * \brief netdev disconnect process
 */
void netdevLanDisconnect();

/**
 * \brief netdev connect process
 */
void netdevLanConnect();

/**
 * \brief netdev process on data channel active
 */
void netdevLanNetUp(uint8_t nSimID, uint8_t nCID);

/**
 * \brief netdev process on data channel deactive
 */
void netdevLanNetDown(uint8_t nSimID, uint8_t nCID);

OSI_EXTERN_C_END

#endif
