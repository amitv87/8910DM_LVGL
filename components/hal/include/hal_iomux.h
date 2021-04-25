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

#ifndef _HAL_IOMUX_H_
#define _HAL_IOMUX_H_

#include "hal_config.h"
#include "osi_compiler.h"

#if defined(CONFIG_SOC_8910)
#include "8910/hal_iomux_pin.h"
#elif defined(CONFIG_SOC_8811)
#include "8811/hal_iomux_pin.h"
#elif defined(CONFIG_SOC_8955)
#include "8955/hal_iomux_pin.h"
#elif defined(CONFIG_SOC_8909)
#include "8909/hal_iomux_pin.h"
#endif

OSI_EXTERN_C_BEGIN

/**
 * \brief helper for GPIO function
 *
 * It will be ensured that GPIO function macros are contiguous.
 */
#define HAL_IOMUX_FUN_GPIO(n) (HAL_IOMUX_FUN_GPIO_0 + (n))

/**
 * \brief helper for GPIO pad
 *
 * It will be ensured that GPIO pad macros are contiguous. The shift is
 * magic, due to pad index is start from bit12.
 */
#define HAL_IOMUX_PAD_GPIO(n) (HAL_IOMUX_PAD_GPIO_0 + ((n) << 12))

/**
 * \brief force pull up/down options
 *
 * Each function has default pull up/down property. In most cases, the
 * default property should be used. When it is really needed to override
 * the default property, *force* property can be set.
 *
 * For 8910, all pads can support pull up strength 1 and 2. And only a part
 * of pads can support strength 3. The typical resistance is 20k/4.7k/1.8k
 * for the three level of pull up strength.
 *
 * Refer to datasheet about the pull up/down resistance, and supported
 * pull up strengthes.
 */
typedef enum
{
    /** not force pull up/down, use function default */
    HAL_IOMUX_PULL_DEFAULT = 0,
    /** force pull, with neither pull up nor pull down */
    HAL_IOMUX_FORCE_PULL_NONE,
    /** force pull down */
    HAL_IOMUX_FORCE_PULL_DOWN,
    /** force pull up */
    HAL_IOMUX_FORCE_PULL_UP,
    /** force pull up with weakest strength (largest resistance) */
    HAL_IOMUX_FORCE_PULL_UP_1 = HAL_IOMUX_FORCE_PULL_UP,
    /** force pull up with smaller resistance */
    HAL_IOMUX_FORCE_PULL_UP_2,
    /** force pull up with smaller resistance */
    HAL_IOMUX_FORCE_PULL_UP_3,
} halIomuxPullType_t;

/**
 * \brief force output options
 *
 * Each function has default direction property. In most cases, the
 * default property should be used. When it is really needed to override
 * the default property, *force* property can be set.
 */
typedef enum
{
    /** not force output, use function default */
    HAL_IOMUX_INOUT_DEFAULT = 0,
    /** force input */
    HAL_IOMUX_FORCE_INPUT,
    /** force output */
    HAL_IOMUX_FORCE_OUTPUT,
    /** force output high */
    HAL_IOMUX_FORCE_OUTPUT_HI,
    /** force output low */
    HAL_IOMUX_FORCE_OUTPUT_LO,
} halIomuxInoutType_t;

/**
 * @brief IOMUX module initialization
 */
void halIomuxInit(void);

/**
 * \brief set initial iomux configuration
 *
 * It will be called in \p halIomuxInit. Usually, it is not needed to be
 * called again.
 *
 * \return
 *      - true on success
 *      - false on invalid initial configuration
 */
bool halIomuxSetInitConfig(void);

/**
 * \brief set pad function
 *
 * \p fun_pad can be:
 * - function: It will set function to default pad, with default function
 *   properties.
 * - (function pad): it will set function to specified pad, with default
 *   function properties.
 *
 * When default function properties doesn't match pad properties (for
 * example, pad doesn't support the driving strength of the function default
 * property), the closest pad property will be applied, and not be regarded
 * as error.
 *
 * \param fun_pad function and pad
 * \return
 *      - true on sucess
 *      - false on fail, invalid parameter
 */
bool halIomuxSetFunction(unsigned fun_pad);

/**
 * \brief set pad driving strength
 *
 * \p fun_pad can be:
 * - function: It will change function default pad properties
 * - pad: Just use the pad
 * - function and pad: Just use the pad, and ignore function
 *
 * When the driving strength doesn't match pad properties, it will be
 * regarded as an error, and return false.
 *
 * \param fun_pad function and pad
 * \param driving driving strength
 * \return
 *      - true on sucess
 *      - false on fail, invalid parameter
 */
bool halIomuxSetPadDriving(unsigned fun_pad, unsigned driving);

/**
 * \brief set pad pull up/down
 *
 * Refer to \p halIomuxSetPadDriving for parameter \p fun_pad.
 *
 * When the pull up/down doesn't match pad properties, it will be
 * regarded as an error, and return false.
 *
 * \param fun_pad function and pad
 * \param pull pull up/down type
 * \return
 *      - true on sucess
 *      - false on fail, invalid parameter
 */
bool halIomuxSetPadPull(unsigned fun_pad, halIomuxPullType_t pull);

/**
 * \brief set pad input/output type
 *
 * Refer to \p halIomuxSetPadDriving for parameter \p fun_pad.
 *
 * \param fun_pad function and pad
 * \param inout input/output type
 * \return
 *      - true on sucess
 *      - false on fail, invalid parameter
 */
bool halIomuxSetPadInout(unsigned fun_pad, halIomuxInoutType_t inout);

/**
 * \brief get fun_pad of specified function
 *
 * Though it should be avoid to assign function to multiple pads, it is
 * possible that a function has already assigned to multiple pads.
 *
 * When \p pads is NULL, it will only return the count. When \p pads is
 * not NULL, it should be large enough to hold all pads. The returned values
 * in \p pads is in format of \p fun_pad.
 *
 * \param fun function
 * \param pads fun_pad have selected the specified function
 * \return count of pads with specified function
 */
unsigned halIomuxGetFunPads(unsigned fun, unsigned *pads);

/**
 * \brief get fun_pad of specified pad
 *
 * The returned value contains both function and pad information, and can
 * be used as parameter for \p halIomuxSetFunction and etc. In case to
 * change pad function temporally, this can be used to get current
 * configuration. For example:
 *
 * \code{.cpp}
 * unsigned fun_pad = halIomuxGetPadFun(pad);
 * halIomuxSetFunction(another_fun); // set another function
 * // ...
 * halIomuxSetFunction(fun_pad); // restore previous configuration
 * \endcode
 *
 * \param pad pad
 * \return fun_pad of the specified pad, or 0 on invalid parameter
 */
unsigned halIomuxGetPadFun(unsigned pad);

/**
 * \brief function name string
 *
 * Most likely, it will only be used for debug purpose.
 *
 * \param fun function
 * \return
 *      - function name string
 *      - empty string ("") for invalid parameter
 */
const char *halIomuxFunName(unsigned fun);

/**
 * \brief pad name string
 *
 * Most likely, it will only be used for debug purpose.
 *
 * \param pad pad
 * \return
 *      - pad name string
 *      - empty string ("") for invalid parameter
 */
const char *halIomuxPadName(unsigned pad);

/**
 * \brief show iomux information in log
 *
 * Most likely, it will only be used for debug purpose.
 */
void halIomuxShowInfo(void);

OSI_EXTERN_C_END
#endif
