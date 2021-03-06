/**  
  @file
  ql_sms_utf.h

  @brief
  This file provides the definitions for sms API functions.

*/
/*============================================================================
  Copyright (c) 2020 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
 =============================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


WHEN        WHO            WHAT, WHERE, WHY
----------  ------------   ----------------------------------------------------
            marvin          create

=============================================================================*/


#ifndef QL_SMS_UTF_H
#define QL_SMS_UTF_H

#include <stddef.h>
//#include <sys/types.h>

/*========================================================================
 *	Typedef Definition
 *========================================================================*/

typedef unsigned int UTF32;
/* at least 32 bits */
typedef unsigned short UTF16;
/* at least 16 bits */
typedef unsigned char UTF8;
/* typically 8 bits */
typedef unsigned char Boolean; /* 0 or 1 */

/*========================================================================
 *	Marco Definition
 *========================================================================*/

/* Some fundamental constants */
#define UNI_REPLACEMENT_CHAR (UTF32)0x0000FFFD
#define UNI_MAX_BMP (UTF32)0x0000FFFF
#define UNI_MAX_UTF16 (UTF32)0x0010FFFF
#define UNI_MAX_UTF32 (UTF32)0x7FFFFFFF
#define UNI_MAX_LEGAL_UTF32 (UTF32)0x0010FFFF

/*========================================================================
 *	Enumeration Definition
 *========================================================================*/

typedef enum {
    conversionOK, /* conversion successful */
            sourceExhausted, /* partial character in source, but hit end */
            targetExhausted, /* insuff. room in target for conversion */
            sourceIllegal        /* source sequence is illegal/malformed */
} ConversionResult;

typedef enum {
    strictConversion = 0,
    lenientConversion
} ConversionFlags;

/*========================================================================
 *	function Definition
 *========================================================================*/

/*****************************************************************
* Function: utf8len
*
* Description:
* 	??????utf8??????????????????????????????
* 
* Parameters:
* 	string	  [in] 	utf8??????????????????
* 
* Return:int,???????????????
*
*****************************************************************/
int utf8len(unsigned char *string);

/*****************************************************************
* Function: utf8_get_size
*
* Description:
* 	??????utf8??????????????????????????????
* 
* Parameters:
* 	source	  [in] 	utf8??????????????????
*	num		  [in] 	utf8??????????????? 
*
* Return:size_t
*
*****************************************************************/
size_t utf8_get_size(unsigned char *source, size_t num);

/*****************************************************************
* Function: utf8toutf16
*
* Description:
* 	???utf8???????????????utf16??????
* 
* Parameters:
* 	source	  [in] 	utf8??????????????????
*	target	  [out] utf16?????????????????? 
*   size      [in]  utf8????????????????????????
*   len       [out] utf16????????????????????????
*
* Return:unsigned short*,utf16???????????????????????????
*
*****************************************************************/
unsigned short *utf8toutf16(unsigned char *source, unsigned short *target, size_t size,  int *len);


#endif //SMS_UTF_H

