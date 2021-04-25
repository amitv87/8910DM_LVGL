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

#ifndef _RPC_HELLO_H_
#define _RPC_HELLO_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief trivial function to return the input parameter
 *
 * The function itself is boring. It is intended to be added to RPC
 * function list. And it can be called by peers. It is just for debug
 * purpose.
 *
 * @param n     input integer
 * @return      input plus 'A'
 */
uint32_t rpcHelloOnAP(uint32_t n);

/**
 * @brief trivial function to return the input parameter
 *
 * The function itself is boring. The *real* function is located in
 * CP, and when this API is called in AP, it will be sent to CP
 * through (the default) RPC channel.
 *
 * @param n     input integer
 * @return      input plus 'C'
 */
uint32_t rpcHelloOnCP(uint32_t n);

#ifdef __cplusplus
}
#endif
#endif
