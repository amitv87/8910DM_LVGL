/*=================================================================

						EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN			  WHO		  WHAT, WHERE, WHY
------------	 -------	 -------------------------------------------------------------------------------

=================================================================*/


#ifndef QUEC_LED_TASK_H
#define QUEC_LED_TASK_H

#include "quec_common.h"
#include "cfw_event.h"
#include "quec_rtos.h"

#if defined (CONFIG_QUEC_PROJECT_FEATURE_LEDCFG) || defined(CONFIG_QUEC_PROJECT_FEATURE_NW)

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * Macro Definition
 ===========================================================================*/

/*===========================================================================
 * Struct
 ===========================================================================*/
typedef struct
{
    uint8_t pdp_active_count;
    uint8_t current_status;
    uint8_t previous_status;
    uint64_t last_count;
    quec_timer_t *timer; 
}quec_netlight_context_s;

typedef struct
{
	uint64_t u64TotalRxCnt;
	uint64_t u64TotalTxCnt; 
}quec_gdcnt_info_s;


/*===========================================================================
 * Functions declaration
 ===========================================================================*/
void quec_ledcfg_event_send(uint8_t index, uint8_t net_type);

void quec_data_transmit_event_send(uint32_t ul_size, uint32_t dl_size);

void quec_ledcfg_init(void);

void quec_netstatuslight_switch(uint8_t net_status);

void quec_netlight_deal_call_change(uint8_t is_dialing);

void quec_netlight_deal_network_change(uint8_t is_reg, uint8_t nSim);

void quec_netlight_deal_pdp_activing(uint8_t is_activing);

void quec_netlight_deal_pdp_active_fail(uint8_t is_active_fail);

void quec_netlight_deal_pdp_change(uint8_t is_active);

void quec_netlight_deal_socket_connect(void);

void quec_netlight_datatransmit_check(void);

bool quec_get_dataCount(quec_gdcnt_info_s * gdcnt_info);

void quec_netlight_deal_data_transmit(void);

void quec_set_increase_dataCount(uint32_t ul_size, uint32_t dl_size);



#ifdef __cplusplus
} /*"C" */
#endif

#endif  /* CONFIG_QUEC_PROJECT_FEATURE_LEDCFG */

#endif /* QUEC_LED_TASK_H */


