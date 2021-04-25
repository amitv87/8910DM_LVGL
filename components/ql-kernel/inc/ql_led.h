/*=================================================================

						EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN			  WHO		  WHAT, WHERE, WHY
------------	 -------	 -------------------------------------------------------------------------------

=================================================================*/


#ifndef QL_LED_H
#define QL_LED_H

#include "ql_api_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * Macro Definition
 ===========================================================================*/
#define QL_LED_ERRCODE_BASE (QL_COMPONENT_BSP_LED<<16)

/*===========================================================================
 * Enum
 ===========================================================================*/
/**
 * LED errcode
 */
typedef enum
{
    QL_LED_SUCCESS = QL_SUCCESS,

    QL_LED_EXECUTE_ERR                  = 1|QL_LED_ERRCODE_BASE,
    QL_LED_INVALID_PARAM_ERR,
    QL_LED_CB_NULL_ERR
}ql_errcode_led;

/*===========================================================================
 * Struct
 ===========================================================================*/

/*===========================================================================
 * Typedef
 ===========================================================================*/
typedef void (*ql_ledcfg_callback)(unsigned int ind_type, uint32_t net_status, uint32_t net_type);

/*===========================================================================
 * Functions declaration
 ===========================================================================*/
/**
 * @brief user's led config callback register
 * @param ledcfg_cb : set callback by user
 * @return
 *       QL_LED_CB_NULL_ERR
 *       QL_LED_SUCCESS
 */
ql_errcode_led ql_ledcfg_callback_register(ql_ledcfg_callback ledcfg_cb);


#ifdef __cplusplus
} /*"C" */
#endif

#endif /* QL_LED_H */


