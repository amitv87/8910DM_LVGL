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

#ifndef _CFW_ERROR_CODE_ID_H_
#define _CFW_ERROR_CODE_ID_H_

// SMS DM error code define begin.
/******************************************************************************/
// Sms dm uninitalize.
#define ERR_SMS_DM_UNINIT 0x3300001
/******************************************************************************/

/******************************************************************************/
// Parameter invalid.
#define ERR_SMS_DM_INVALID_PARAMETER 0x3300002
/******************************************************************************/

/******************************************************************************/
// Read medium failed.
#define ERR_SMS_DM_MEDIA_READ_FAILED 0x3300003
/******************************************************************************/

/******************************************************************************/
// Write medium failed.
#define ERR_SMS_DM_MEDIA_WRITE_FAILED 0x3300004
/******************************************************************************/

/******************************************************************************/
// The space is full.
#define ERR_SMS_DM_SPACE_FULL 0x3300005
/******************************************************************************/

/******************************************************************************/
// The index is invalid.
#define ERR_SMS_DM_INVALID_INDEX 0x3300006
/******************************************************************************/

/******************************************************************************/
// Alloc memory failed.
#define ERR_SMS_DM_NOT_ENOUGH_MEMORY 0x3300007
/******************************************************************************/

/******************************************************************************/
// The media space error.
#define ERR_SMS_DM_NOT_ENOUGH_MEDIA_SPACE 0x3300008
/******************************************************************************/

/******************************************************************************/
// The device not register.
#define ERR_SMS_DM_NOT_REGISTER 0x3300009
/******************************************************************************/

/******************************************************************************/
// DM unknow error.
#define ERR_SMS_DM_ERROR 0x33000010
/******************************************************************************/

#endif // _H_
