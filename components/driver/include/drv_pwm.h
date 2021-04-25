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

#ifndef _DRV_PWM_H_
#define _DRV_PWM_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    LGP_PER_125MS,
    LGP_PER_250MS,
    LGP_PER_500MS,
    LGP_PER_1000MS,
    LGP_PER_1500MS,
    LGP_PER_2000MS,
    LGP_PER_2500MS,
    LGP_PER_3000MS,
} drvLpgPer_t;

typedef enum
{
    LGP_ONTIME_UNDEFINE,
    LGP_ONTIME_15_6MS,
    LGP_ONTIME_31_2MS,
    LGP_ONTIME_46_8MS,
    LGP_ONTIME_62MS,
    LGP_ONTIME_78MS,
    LGP_ONTIME_94MS,
    LGP_ONTIME_110MS,
    LGP_ONTIME_125MS,
    LGP_ONTIME_140MS,
    LGP_ONTIME_156MS,
    LGP_ONTIME_172MS,
    LGP_ONTIME_188MS,
    LGP_ONTIME_200MS,
    LGP_ONTIME_218MS,
    LGP_ONTIME_234MS
} drvLpgOntime_t;

typedef enum
{
    PWL_ID_0,
    PWL_ID_1
} drvPwlId_t;

typedef struct drvPwm drvPwm_t;

/**
 * @brief This function can acquire a pwm structure
 * @param name  pwm name,such as DRV_NAME_PWM1
 * @return
 *      - NULL  if fail
 *      - other success
 */
drvPwm_t *drvPwmAcquire(uint32_t name);

/**
 * @brief This functiton set pwt period count and clk prescaler
 *        output_clk = 200Mhz/(prescaler+1)/(prescaler*8 + 1)
 *        **notice** : now clk_lock must be set,otherwise the clock will switch between 
 *        26M and 200M.In this case , system will not sleep.
 * @param d             pwm structure
 * @param period_count  period load data,must less than 2047.if period_count is 0,
 *                      pwt can't work.
 * @param prescaler     pwm clock prescaler,clk = 200Mhz/(prescaler+1)
 * @param duty          duty must less than 1023,duty/period_count is the duty
 *                      ratio.duty can't be 0 or pwt will stop.
 */
void drvPwtSetPeriodDuty(drvPwm_t *d, uint8_t clk_div, uint32_t pwm_count, uint32_t duty);

/**
 * @brief pwt output start
 * @param d     pwm structure
 * @return
 *      - false  if fail
 *      - true success
 */
bool drvPwtStart(drvPwm_t *d);

/**
 * @brief This function stop pwl output
 * @param d     pwm structure
 * @return
 *      - false  if fail
 *      - true success
 */
bool drvPwtStop(drvPwm_t *d);

/**
 * @brief This function makes the lpg modul output
   @param d         pwm structure
 * @param period    lpg period ,from 125ms to 3s
 * @param ontime    lpg on time ,from 15.6ms to 234ms
 * @return
 *      - false  if fail
 *      - true success
 */
bool drvLgpStart(drvPwm_t *d, drvLpgPer_t period, drvLpgOntime_t ontime);

/**
 * @brief This function stop the lpg modul output
 * @param d         pwm structure
 * @return
 *      - false  if fail
 *      - true success
 */
bool drvLgpStop(drvPwm_t *d);

/**
 * @brief set the averge on time for selected PWL output,
 *        thus, the intergrated output represents a certain luminosity level
 * @param d         pwm structure
 * @param pwl_id    pwl0 and pwl1
 * @param level     0 will force the pwl output to 0 while a value
 *                  of 0xff will force the output to 1.
 * @return
 *      - false  if fail
 *      - true success
 */
bool drvPwlStart(drvPwm_t *d, drvPwlId_t pwl_id, uint8_t level);

/**
 * @brief This function stop the pwl modul output
 * @param d         pwm structure
 * @param pwl_id    pwl0 and pwl1
 * @return
 *      - false  if fail
 *      - true success
 */
bool drvPwlStop(drvPwm_t *d, drvPwlId_t pwl_id);

/**
 * @brief This function release pwm controller
 * @param d     pwm structure
 */
void drvPwmRelease(drvPwm_t *d);

#endif