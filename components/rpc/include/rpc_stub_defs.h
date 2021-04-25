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

#ifndef _RPC_STUB_DEFS_H_
#define _RPC_STUB_DEFS_H_

#include "rpc_daemon.h"
#include "rpc_config.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CHECK_IPAR_SIZE(size)                                                           \
    static_assert((size) <= CONFIG_RPC_PACKET_SIZE_MAX - sizeof(rpcCallHeader_t),       \
                  "RPC packet size should be smaller than CONFIG_RPC_PACKET_SIZE_MAX"); \
    static_assert((size) % 8 == 0, "RPC packet size should be 8 bytes aligned")
#define CHECK_OPAR_SIZE(size)                                                           \
    static_assert((size) <= CONFIG_RPC_PACKET_SIZE_MAX - sizeof(rpcRespHeader_t),       \
                  "RPC packet size should be smaller than CONFIG_RPC_PACKET_SIZE_MAX"); \
    static_assert((size) % 8 == 0, "RPC packet size should be 8 bytes aligned")

#ifdef __cplusplus
}
#endif
#endif
