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
* 	获取utf8格式字符串的实际长度
* 
* Parameters:
* 	string	  [in] 	utf8格式的字符串
* 
* Return:int,字符串长度
*
*****************************************************************/
int utf8len(unsigned char *string);

/*****************************************************************
* Function: utf8_get_size
*
* Description:
* 	获取utf8格式字符串的实际长度
* 
* Parameters:
* 	source	  [in] 	utf8格式的字符串
*	num		  [in] 	utf8字符个数。 
*
* Return:size_t
*
*****************************************************************/
size_t utf8_get_size(unsigned char *source, size_t num);

/*****************************************************************
* Function: utf8toutf16
*
* Description:
* 	将utf8格式转换为utf16格式
* 
* Parameters:
* 	source	  [in] 	utf8格式的字符串
*	target	  [out] utf16格式的字符串 
*   size      [in]  utf8格式的字符串个数
*   len       [out] utf16格式的字符串个数
*
* Return:unsigned short*,utf16格式的字符串首地址
*
*****************************************************************/
unsigned short *utf8toutf16(unsigned char *source, unsigned short *target, size_t size,  int *len);


#endif //SMS_UTF_H

