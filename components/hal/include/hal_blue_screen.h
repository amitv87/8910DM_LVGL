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

#ifndef _HAL_BLUE_SCREEN_H_
#define _HAL_BLUE_SCREEN_H_

#include "osi_compiler.h"

OSI_EXTERN_C_BEGIN

/**
 * \brief save blue screen context
 *
 * The context format and \p ctx format are implementation dependent,
 * and platform dependent. It must match the exception handler.
 *
 * It should be only called in the beginning of blue screen.
 *
 * It may be called in bootloader (without OS) stage, and applocation
 * (with OS) stage. The format should be the same.
 *
 * \param ctx pointer of exception context
 */
void halBlueScreenSaveContext(void *ctx);

/**
 * \brief save blue screen info string
 *
 * The blue screen info string can be fetched by \p halBlueScreenGetInfo.
 *
 * \param ctx pointer of exception context
 */
void halBlueScreenSaveInfo(void *ctx);

/**
 * \brief get blue screen info
 *
 * \return
 *      - blue screen info string
 */
const char *halBlueScreenGetInfo(void);

/**
 * \brief save blue screen info into bscore buffer
 *
 * \param ctx pointer of exception context
 */
void halBscoreBufSave(void *ctx);

/**
 * \brief get bscore buffer
 *
 * \return
 *      - blue screen core buffer pointer and size
 *      - (0, 0) if there are no blue screen core buffer
 */
osiBuffer_t halGetBscoreBuf(void);

/**
 * \brief clear bscore buffer
 *
 * This should be called after the bscore buffer is processed.
 *
 * \code{.cpp}
 *      osiBuffer_t buf = halGetBscoreBuf();
 *      if (buf.size > 0) {
 *          // Save bscore buffer, or send out the buffer,
 *          // or copy to another place to be processed later.
 *          halClearBscoreBuf();
 *      }
 * \endcode
 */
void halClearBscoreBuf(void);

OSI_EXTERN_C_END
#endif
