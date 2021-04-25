/**  
  @file
  ql_api_qr_code.h

  @brief
  This file provides the definitions for qr code API functions.

*/
/*=================================================================

						EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN			  WHO		  WHAT, WHERE, WHY
------------	 -------	 -------------------------------------------------------------------------------

=================================================================*/


#ifndef QL_API_DECODER_H
#define QL_API_DECODER_H

#include "ql_api_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * Macro Definition
 ===========================================================================*/
#define QL_DECODER_ERRCODE_BASE (QL_COMPONENT_BSP_DECODER<<16)
    
#define DECODER_INITIAL_FAILED  0
#define DECODER_INITIAL  		1
    
#define DECODER_ERROR			0
#define DECODER_SUCCESS			1
#define DECODER_ERROR_NONE		1

/*===========================================================================
 * Enum
===========================================================================*/
typedef enum
{
    QL_DECODER_SUCCESS = QL_SUCCESS,

    QL_DECODER_INIT_ERR                 = 1|QL_COMPONENT_BSP_DECODER,
    QL_DECODER_ERR,
    QL_DECODER_GET_RESULT_ERR,
    QL_DECODER_GET_RESULT_LENGTH_ERR,
}ql_errcode_decoder_e;

/*===========================================================================
 * Variate
 ===========================================================================*/

/*===========================================================================
 * Functions
 ===========================================================================*/

/*****************************************************************
* Function: ql_qr_decoder_init
*
* Description: Initialize the decoder
* 
* Parameters:
*
* Return:
* 	ql_errcode_decoder_e 
*
*****************************************************************/
ql_errcode_decoder_e ql_qr_decoder_init (void);

/*****************************************************************
* Function: ql_qr_get_decoder_result
*
* Description: get the decode result
* 
* Parameters:
*   *result     [out]   the buf used to save the result
* Return:
* 	ql_errcode_decoder_e 
*
*****************************************************************/
ql_errcode_decoder_e ql_qr_get_decoder_result (unsigned char *result);

/*****************************************************************
* Function: ql_qr_image_decoder
*
* Description: decode the image
* 
* Parameters:
*   *img_buffer     [in]    the image need to be decoded
*   whidth          [in]    the width of the image
*   height          [in]    the height of the image
* Return:
* 	ql_errcode_decoder_e 
*
*****************************************************************/
ql_errcode_decoder_e ql_qr_image_decoder (uint16_t *img_buffer, uint32_t width, uint32_t height);



#ifdef __cplusplus
    } /*"C" */
#endif

#endif /* QL_API_DECODER_H*/

