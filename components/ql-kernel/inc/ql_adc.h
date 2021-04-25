/*=================================================================

						EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN			  WHO		  WHAT, WHERE, WHY
------------	 -------	 -------------------------------------------------------------------------------

=================================================================*/


#ifndef QL_ADC_H
#define QL_ADC_H


#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * Macro Definition
 ===========================================================================*/
 #include "ql_api_common.h"
 
/*========================================================================
*  Enumeration Definition
*========================================================================*/

/****************************  error code about ADC    ***************************/
typedef enum
{
	QL_ADC_SUCCESS  			= 0,
	QL_ADC_GET_VALUE_ERROR 		= 50  | (QL_COMPONENT_BSP_ADC << 16),
	QL_ADC_MEM_ADDR_NULL_ERROR	= 60  | (QL_COMPONENT_BSP_ADC << 16),
} ql_errcode_adc_e;

/*===========================================================================
 * Struct
 ===========================================================================*/

/*===========================================================================
 * Functions declaration
 ===========================================================================*/

/*****************************************************************
* Function: ql_adc_get_volt
*
* Description:  get adc value of voltage by ADC0 or ADC1
* 
* Parameters:
* 	adc_id	  		[in] 	input adcid
*
* Return:
* 	volt_value	succeed
*	-1 	        failure
*
*****************************************************************/
ql_errcode_adc_e ql_adc_get_volt(int ql_adc_channel_id, int *adc_value);

#ifdef __cplusplus
} /*"C" */
#endif

#endif /* QL_ADC_H */


