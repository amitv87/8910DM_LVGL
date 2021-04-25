/*=================================================================

						EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN			  WHO		  WHAT, WHERE, WHY
------------	 -------	 -------------------------------------------------------------------------------

=================================================================*/


#ifndef _ADCDEMO_H
#define _ADCDEMO_H


#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * Enumeration Definition
 ===========================================================================*/
 typedef enum
{
#ifdef CONFIG_QL_PROJECT_DEF_EC200U
	QL_DEMO_ADC0 = 2,						//	DEMO_ADC0
	QL_DEMO_ADC1 = 3,						//	DEMO_ADC1
	QL_DEMO_ADC2 = 4,						//	DEMO_ADC2
#else
	QL_DEMO_ADC0 = 1,						//	DEMO_ADC0
	QL_DEMO_ADC1 = 2,						//	DEMO_ADC1
	QL_DEMO_ADC2 = 3,						//	DEMO_ADC2
	QL_DEMO_ADC3 = 4,						//	DEMO_ADC3
#endif
}ql_adc_demo_channel_e;

/*===========================================================================
 * Struct
 ===========================================================================*/
typedef int    QlADCStatus;
typedef void * ql_task_t;

/*===========================================================================
 * Functions declaration
 ===========================================================================*/
void ql_adc_app_init(void);

#ifdef __cplusplus
} /*"C" */
#endif

#endif /* _ADCDEMO_H */


