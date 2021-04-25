/* Copyright (C) 2019 RDA Technologies Limited and/or its affiliates("RDA").
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

#ifndef _SRV_TRACE_H_
#define _SRV_TRACE_H_

#include "osi_compiler.h"

OSI_EXTERN_C_BEGIN

/**
 * \brief options for fstrace
 */
typedef enum
{
    SRV_FSTRACE_OPT_AP_ENABLE = (1 << 0),  ///< enable aplog
    SRV_FSTRACE_OPT_PRO_ENABLE = (1 << 1), ///< enable profile
    SRV_FSTRACE_OPT_BS_ENABLE = (1 << 2),  ///< enable blue screen dump
    SRV_FSTRACE_OPT_CP_ENABLE = (1 << 3),  ///< enable cplog
    SRV_FSTRACE_OPT_ZSP_ENABLE = (1 << 4), ///< enable zsplog
    SRV_FSTRACE_OPT_MOS_ENABLE = (1 << 5), ///< enable moslog
} srvFstraceOptions_t;

/**
 * \brief init bt trace output via ap
 */
bool srvBtTraceInit(void);

/**
 * \brief initialize file system trace output
 *
 * There is a separated thread for file system trace. Even there are multiple
 * trace data sources, all of them will be done in the same thread.
 *
 * \param option fstrace options, refer to \p srvFstraceOptions_t.
 * \return
 *      - true on success
 *      - false on fail, invalid parameters or out of memory
 */
bool srvFstraceInit(unsigned option);

OSI_EXTERN_C_END
#endif
