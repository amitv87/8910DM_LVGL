/**  @file
  quec_ppp.h

  @brief
  This file is used to define version information for different Quectel Project.

*/

/*================================================================
  Copyright (c) 2020 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
=================================================================*/
/*=================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN              WHO         WHAT, WHERE, WHY
------------     -------     -------------------------------------------------------------------------------

=================================================================*/



#ifndef QUEC_PPP_H
#define QUEC_PPP_H

#include "quec_common.h"


#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_QUEC_PROJECT_FEATURE_PPP


/*========================================================================
 *  Macro Definition
 *========================================================================*/



/*========================================================================
 *  Enumeration Definition
 *========================================================================*/


/*========================================================================
 *	Type Definition
 *========================================================================*/




/*========================================================================
 *	function Definition
 *========================================================================*/
void quec_ppp_data_mode_end(atDispatch_t *ch);

void quec_ppp_dtr_rising_handler(atDispatch_t *ch);

void quec_ppp_mode_switch_handler(atDispatch_t *th, atModeSwitchCause_t cause);


#endif

#ifdef __cplusplus
} /*"C" */
#endif

#endif /* QUEC_PPP_H */






