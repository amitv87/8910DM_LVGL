/**  
  @file
  ql_i2c.h

  @brief
  This file provides the definitions for i2c API functions.

*/
/*=================================================================

						EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN			  WHO		  WHAT, WHERE, WHY
------------	 -------	 -------------------------------------------------------------------------------

=================================================================*/


#ifndef QL_I2C_H
#define QL_I2C_H

#include "ql_api_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * Macro Definition
===========================================================================*/
#define i2c_max_count 3

#define QL_I2C_ERRCODE_BASE (QL_COMPONENT_BSP_I2C<<16)

/*===========================================================================
 * Enum
===========================================================================*/
typedef enum
{
    STANDARD_MODE = 0,  //Standard mode (100K)
    FAST_MODE = 1,      //Fast mode (400K)
} ql_i2c_mode_e;

typedef enum
{
    i2c_1 = 0,  //i2c channel 1
    i2c_2,      //i2c channel 2
    i2c_3,      //i2c channel 3
}ql_i2c_channel_e;

typedef enum
{
    QL_I2C_SUCCESS = QL_SUCCESS,

    QL_I2C_INIT_ERR                 = 1|QL_I2C_ERRCODE_BASE,
    QL_I2C_WRITE_ERR,
    QL_I2C_READ_ERR,
    QL_I2C_RELEASE_ERR,
}ql_errcode_i2c_e;

/*===========================================================================
 * Struct
===========================================================================*/

/*===========================================================================
 * Function
===========================================================================*/

/*****************************************************************
* Function: ql_I2cInit
*
* Description: Initialize the i2c master
* 
* Parameters:
*   i2c_no        [in]    the i2c channel 
*   fastMode      [in]    the i2c speed mode
*
* Return:
* 	QL_I2C_SUCCESS
*	QL_I2C_INIT_ERR 
*
*****************************************************************/
ql_errcode_i2c_e ql_I2cInit(ql_i2c_channel_e i2c_no, ql_i2c_mode_e Mode);

/*****************************************************************
* Function: ql_I2cWrite
*
* Description: i2c master write
* 
* Parameters:
*   i2c_no        [in]    the i2c channel 
*   slave         [in]    the i2c slave address
*   addr          [in]    the i2c slave regiser address
*   data          [in]    the data need to be sent
*   length        [in]    the length of the data
*
* Return:
* 	QL_I2C_SUCCESS
*	QL_I2C_WRITE_ERR
*
*****************************************************************/
ql_errcode_i2c_e ql_I2cWrite(ql_i2c_channel_e i2c_no, uint8_t slave, uint8_t addr, uint8_t *data, uint32_t length);

/*****************************************************************
* Function: ql_I2cRead
*
* Description: i2c master read
* 
* Parameters:
*   i2c_no        [in]    the i2c channel 
*   slave         [in]    the i2c slave address
*   addr          [in]    the i2c slave regiser address
*   buf           [out]   the data that was read
*   length        [in]    the length of the data
*
* Return:
* 	QL_I2C_SUCCESS
*	QL_I2C_READ_ERR
*
*****************************************************************/
ql_errcode_i2c_e ql_I2cRead(ql_i2c_channel_e i2c_no, uint8_t slave, uint8_t addr, uint8_t *buf, uint32_t length);

/*****************************************************************
* Function: ql_I2cRelease
*
* Description: i2c master release
* 
* Parameters:
*   i2c_no        [in]    the i2c channel 
*
* Return:
* 	QL_I2C_SUCCESS
*	QL_I2C_RELEASE_ERR
*
*****************************************************************/
ql_errcode_i2c_e ql_I2cRelease(ql_i2c_channel_e i2c_no);




#ifdef __cplusplus
   } /*"C" */
#endif
   
#endif /* QL_I2C_H */
   




