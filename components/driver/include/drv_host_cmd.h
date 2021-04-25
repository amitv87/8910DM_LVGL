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

#ifndef _DRV_HOST_CMD_H_
#define _DRV_HOST_CMD_H_

#include "osi_api.h"
#include "drv_debug_port.h"

OSI_EXTERN_C_BEGIN

/**
 * \brief opaque data struct for host command engine
 */
typedef struct drvHostCmdEngine drvHostCmdEngine_t;

/**
 * \brief create host command engine
 *
 * \param port debug port
 * \return
 *      - host command engine
 *      - NULL on error, out of memory
 */
drvHostCmdEngine_t *drvHostCmdEngineCreate(drvDebugPort_t *port);

OSI_EXTERN_C_END
#endif
