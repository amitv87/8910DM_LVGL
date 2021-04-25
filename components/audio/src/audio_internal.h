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

#ifndef _AUDIO_INTERNAL_H_
#define _AUDIO_INTERNAL_H_

#include "osi_api.h"
#include "audio_types.h"

OSI_EXTERN_C_BEGIN

/**
 * \brief get the mutex used in audio device
 *
 * It is weird, and it is a concept deeply bound with current design. Audio
 * application will run on single thread, and it will call API of audio
 * application. Such as, audio player API will be called in audio thread when
 * data is requested. Single mutex can avoid mutual lock.
 *
 * Mutal lock may occur when audio application uses separated mutex.
 *
 * \code{.cpp}
 *      // audio player
 *      osiMutexLock(player_lock);
 *      osiMutexLock(audio_device_lock);
 *
 *      // audio thread
 *      osiMutexLock(audio_device_lock);
 *      osiMutexLock(player_lock);
 * \endcode
 *
 * \return
 *      - mutex in audio device
 */
osiMutex_t *audevMutex(void);

OSI_EXTERN_C_END
#endif
