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

#ifndef _OSI_PROFILE_H_
#define _OSI_PROFILE_H_

#ifdef __cplusplus
extern "C" {
#endif

/** profile code for entering blue screen */
#define PROFCODE_BLUE_SCREEN 0x2f02

/** profile code for panic */
#define PROFCODE_PANIC 0x2f58

/** profile code for enter/exit light sleep (WFI) */
#define PROFCODE_LIGHT_SLEEP 0x3703

/** profile code for deep light sleep */
#define PROFCODE_DEEP_SLEEP 0x3702

/** profile code for suspend/resume */
#define PROFCODE_SUSPEND 0x3704

/** profile code for enter/exit flash erase */
#define PROFCODE_FLASH_ERASE 0x3705

/** profile code for enter/exit flash program */
#define PROFCODE_FLASH_PROGRAM 0x3706

/** profile code for enter/exit WCN sleep */
#define PROFCODE_WCN_SLEEP 0x3707

/** flag to indicate exit event */
#define PROFCODE_EXIT_FLAG 0x8000

/** start for irq event */
#define PROFCODE_IRQ_START 0x3f00

/** end for irq event */
#define PROFCODE_IRQ_END 0x3f7f

/** start for thread event */
#define PROFCODE_THREAD_START 0x3f80

/** end for thread event */
#define PROFCODE_THREAD_END 0x3fdf

/** start for thread event */
#define PROFCODE_JOB_START 0x3fe0

/** end for thread event */
#define PROFCODE_JOB_END 0x3ffe

/** profile mode */
typedef enum
{
    /** normal mode, drop oldest at full */
    OSI_PROFILE_MODE_NORMAL,
    /** stop profile when buffer is full */
    OSI_PROFILE_MODE_STOP_ON_FULL,
} osiProfileMode_t;

/**
 * \brief profile initialization
 *
 * It should be called in early boot stage, to initialize profile data
 * structure.
 */
void osiProfileInit(void);

/**
 * \brief set profile mode
 *
 * \p OSI_PROFILE_MODE_STOP_ON_FULL is useful when current events are
 * interested, and avoid existed recorded events are replaced by later
 * events.
 *
 * At initialization, profile mode is \p OSI_PROFILE_MODE_NORMAL. In case
 * it is wanted to profiling boot procedure, the initial mode can be set
 * to \p OSI_PROFILE_MODE_STOP_ON_FULL by hacking source codes.
 *
 * Each time this is called, the profile buffer write pointer will be
 * changed to the beginning .
 *
 * \param mode profile mode
 */
void osiProfileSetMode(osiProfileMode_t mode);

/**
 * \brief insert a profile event
 *
 * \p code should follow profile code convention.
 *
 * \param code profile event
 */
void osiProfileCode(unsigned code);

/**
 * \brief insert a profile entering event
 *
 * \param code profile event
 */
void osiProfileEnter(unsigned code);

/**
 * \brief insert a profile exiting event
 *
 * \param code profile event
 */
void osiProfileExit(unsigned code);

/**
 * \brief insert a profile thread entering event
 *
 * \param id thread id
 */
void osiProfileThreadEnter(unsigned id);

/**
 * \brief insert a profile thread exiting event
 *
 * \param id thread id
 */
void osiProfileThreadExit(unsigned id);

/**
 * \brief insert a profile ISR entering event
 *
 * \param id interrupt id
 */
void osiProfileIrqEnter(unsigned id);

/**
 * \brief insert a profile ISR exiting event
 *
 * \param id interrupt id
 */
void osiProfileIrqExit(unsigned id);

/**
 * \brief get linear buffer from pos
 *
 * The original design of profile buffer doesn't consider consumer.
 * And then this API is weird. Caller should keep record the last
 * fetch position.
 *
 * \param pos the starting position to be fetched
 * \return linear profile data buffer
 */
osiBuffer_t osiProfileLinearBuf(uint32_t pos);

/**
 * \brief get the latest profile data
 *
 * \param mem memory for output profile data
 * \param size maximum output size
 * \return profile data copied
 */
unsigned osiProfileGetLastest(uint32_t *mem, unsigned size);

#ifdef __cplusplus
}
#endif
#endif
