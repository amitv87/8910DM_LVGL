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

#ifndef _MAL_API_H_
#define _MAL_API_H_

#include "osi_api.h"
#include "cfw.h"

OSI_EXTERN_C_BEGIN

/**
 * \brief SIM authentication state
 */
typedef enum
{
    MAL_SIM_AUTH_READY = 0,                 ///< Phone is not waiting for any password.
    MAL_SIM_AUTH_PIN1_READY = 1,            ///< Phone is not waiting for PIN1 password.
    MAL_SIM_AUTH_SIMPIN = 2,                ///< Phone is waiting for the SIM Personal Identification Number (PIN)
    MAL_SIM_AUTH_SIMPUK = 3,                ///< Phone is waiting for the SIM Personal Unlocking Key (PUK).
    MAL_SIM_AUTH_PHONE_TO_SIMPIN = 4,       ///< Phone is waiting for the phone-to-SIM card password.
    MAL_SIM_AUTH_PHONE_TO_FIRST_SIMPIN = 5, ///< Phone is waiting for the phone-to-first-SIM card PIN.
    MAL_SIM_AUTH_PHONE_TO_FIRST_SIMPUK = 6, ///< Phone is waiting for the phone-to-first-SIM card PUK.
    MAL_SIM_AUTH_SIMPIN2 = 7,               ///< Phone is waiting for the SIM PIN2.
    MAL_SIM_AUTH_SIMPUK2 = 8,               ///< Phone is waiting for the SIM PUK2.
    MAL_SIM_AUTH_NETWORKPIN = 9,            ///< Phone is waiting for the network personalization PIN.
    MAL_SIM_AUTH_NETWORKPUK = 10,           ///< Phone is waiting for the network personalization PUK.
    MAL_SIM_AUTH_NETWORK_SUBSETPIN = 11,    ///< Phone is waiting for the network subset personalization PIN.
    MAL_SIM_AUTH_NETWORK_SUBSETPUK = 12,    ///< Phone is waiting for the network subset personalization PUK.
    MAL_SIM_AUTH_PROVIDERPIN = 13,          ///< Phone is waiting for the service provider personalization PIN.
    MAL_SIM_AUTH_PROVIDERPUK = 14,          ///< Phone is waiting for the service provider personalization PUK.
    MAL_SIM_AUTH_CORPORATEPIN = 15,         ///< Phone is waiting for the corporate personalization PIN.
    MAL_SIM_AUTH_CORPORATEPUK = 16,         ///< Phone is waiting for the corporate personalization PUK.
    MAL_SIM_AUTH_NOSIM = 17,                ///< No SIM inserted.
    MAL_SIM_AUTH_PIN1BLOCK = 18,
    MAL_SIM_AUTH_PIN2BLOCK = 19,
    MAL_SIM_AUTH_PIN1_DISABLE = 20,
    MAL_SIM_AUTH_SIM_PRESENT = 21,
    MAL_SIM_AUTH_SIM_PHASE = 22,
    MAL_SIM_AUTH_SIM_END = 23,
    MAL_SIM_AUTH_PIN2_DISABLE = 24,
} malSimState_t;

/**
 * \brief initialize modem abstraction layer excution engine
 */
void malInit(void);

/**
 * \brief (+CPIN?)
 *
 * \param sim       SIM index, ignored in single SIM version
 * \param state     SIM state
 * \return
 *      - 0 on success
 *      - error code on fail
 */
unsigned malSimGetState(int sim, malSimState_t *state, unsigned *remaintries);

int sim_channel_open(uint8_t *dfname, uint8_t *channel_id, uint16_t timeout, uint8_t sim_id);
int sim_channel_transmit(uint8_t channel_id, uint8_t *apdu, uint16_t apduLen,
                         uint8_t *resp, uint16_t *respLen, uint16_t timeout, uint8_t sim_id);
int sim_channel_close(uint8_t channel_id, uint16_t timeout, uint8_t sim_id);

/**
 * @brief (*****Command COPS_V2)
 *
 * Description:this api is used to get Available network operator.
 *
 * @param operatorInfo_v2      operator information,operatorInfo_v2 need to be free after use
 * @param operatorCount        operator number
 * @param nSim                     SIM index, ignored in single SIM version
 * @return
 *      - 0 on success
 *      - error code on fail
 */
unsigned simVsimGetEO(CFW_NW_OPERATOR_INFO_V2 **operatorInfo_v2, uint16_t *operatorCount, uint8_t nSim);

/**
 * @brief (Exe Command CVSIMRESET)
 *
 * Description:this api is used to reset vsim.
 *
 * @param nSim                     SIM index, ignored in single SIM version
 * @return
 *      - 0 on success
 *      - error code on fail
 */
unsigned simVsimReset(uint8_t nSim);
OSI_EXTERN_C_END
#endif
