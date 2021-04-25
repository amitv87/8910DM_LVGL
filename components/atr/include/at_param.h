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

#ifndef _AT_PARAM_H_
#define _AT_PARAM_H_

#include <stdint.h>
#include <stdbool.h>
#include "osi_vsmap.h"

/**
 * parsed AT command parameter
 */
typedef struct
{
    uint8_t type;    ///< parameter type, used by AT engine internally
    uint16_t length; ///< value length
    char value[1];   ///< value, the real size is variable
} atCmdParam_t;

/**
 * extract uint parameter
 *
 * When \a *paramok is false, it will do nothing. On failure, \a *param
 * will be set to false on return.
 *
 * @param param     parameter pointer
 * @param paramok   in/out parameter parsing ok flag
 * @return
 *      - uint parameter
 *      - 0 on failed
 *          - \a *paramok is false at input
 *          - \a param is empty
 *          - \a param is not integer number
 */
uint32_t atParamUint(atCmdParam_t *param, bool *paramok);

/**
 * extract optional uint parameter
 *
 * When \a *paramok is false, it will do nothing. On failure, \a *param
 * will be set to false on return.
 *
 * When \a param is empty, \a defval is returned. Otherwise, it is the
 * same as \a atParamUint.
 *
 * @param param     parameter pointer
 * @param defval    default value
 * @param paramok   in/out parameter parsing ok flag
 * @return
 *      - uint parameter
 *      - 0 on failed
 *          - \a *paramok is false at input
 *          - \a param is not integer number
 */
uint32_t atParamDefUint(atCmdParam_t *param, uint32_t defval, bool *paramok);

/**
 * extract uint parameter, and check range
 *
 * When \a *paramok is false, it will do nothing. On failure, \a *param
 * will be set to false on return.
 *
 * @param param     parameter pointer
 * @param minval    minimum valid value, inclusive
 * @param maxval    maximum valid value, inclusive
 * @param paramok   in/out parameter parsing ok flag
 * @return
 *      - uint parameter
 *      - 0 on failed
 *          - \a *paramok is false at input
 *          - \a param is empty
 *          - \a param is not integer number
 *          - \a param is not in range
 */
uint32_t atParamUintInRange(atCmdParam_t *param, uint32_t minval, uint32_t maxval, bool *paramok);

/**
 * extract optional uint parameter, and check range
 *
 * When \a *paramok is false, it will do nothing. On failure, \a *param
 * will be set to false on return.
 *
 * When \a param is empty, \a defval is returned. Otherwise, it is the
 * same as \a atParamUintInRange. The \a defval is not required in the
 * range.
 *
 * @param param     parameter pointer
 * @param defval    default value
 * @param minval    minimum valid value, inclusive
 * @param maxval    maximum valid value, inclusive
 * @param paramok   in/out parameter parsing ok flag
 * @return
 *      - uint parameter
 *      - 0 on failed
 *          - \a *paramok is false at input
 *          - \a param is not integer number
 *          - \a param is not in range
 */
uint32_t atParamDefUintInRange(atCmdParam_t *param, uint32_t defval, uint32_t minval, uint32_t maxval, bool *paramok);

/**
 * extract uint parameter, and check in list
 *
 * When \a *paramok is false, it will do nothing. On failure, \a *param
 * will be set to false on return.
 *
 * @param param     parameter pointer
 * @param list      array of valid values
 * @param count     valid value count
 * @param paramok   in/out parameter parsing ok flag
 * @return
 *      - uint parameter
 *      - 0 on failed
 *          - \a *paramok is false at input
 *          - \a param is empty
 *          - \a param is not integer number
 *          - \a param is not in list
 */
uint32_t atParamUintInList(atCmdParam_t *param, const uint32_t *list, unsigned count, bool *paramok);

/**
 * extract optional uint parameter, and check in list
 *
 * When \a *paramok is false, it will do nothing. On failure, \a *param
 * will be set to false on return.
 *
 * When \a param is empty, \a defval is returned. Otherwise, it is the
 * same as \a atParamUintInList. The \a defval is not required in the
 * list.
 *
 * @param param     parameter pointer
 * @param defval    default value
 * @param list      array of valid values
 * @param count     valid value count
 * @param paramok   in/out parameter parsing ok flag
 * @return
 *      - uint parameter
 *      - 0 on failed
 *          - \a *paramok is false at input
 *          - \a param is not integer number
 *          - \a param is not in list
 */
uint32_t atParamDefUintInList(atCmdParam_t *param, uint32_t defval, const uint32_t *list, unsigned count, bool *paramok);

/**
 * extract int parameter
 *
 * When \a *paramok is false, it will do nothing. On failure, \a *param
 * will be set to false on return.
 *
 * @param param     parameter pointer
 * @param paramok   in/out parameter parsing ok flag
 * @return
 *      - int parameter
 *      - 0 on failed
 *          - \a *paramok is false at input
 *          - \a param is empty
 *          - \a param is not integer number
 */
int atParamInt(atCmdParam_t *param, bool *paramok);

/**
 * extract optional int parameter
 *
 * When \a *paramok is false, it will do nothing. On failure, \a *param
 * will be set to false on return.
 *
 * When \a param is empty, \a defval is returned. Otherwise, it is the
 * same as \a atParamUint.
 *
 * @param param     parameter pointer
 * @param defval    default value
 * @param paramok   in/out parameter parsing ok flag
 * @return
 *      - int parameter
 *      - 0 on failed
 *          - \a *paramok is false at input
 *          - \a param is not integer number
 */
int atParamDefInt(atCmdParam_t *param, int defval, bool *paramok);

/**
 * extract int parameter, and check range
 *
 * When \a *paramok is false, it will do nothing. On failure, \a *param
 * will be set to false on return.
 *
 * @param param     parameter pointer
 * @param minval    minimum valid value, inclusive
 * @param maxval    maximum valid value, inclusive
 * @param paramok   in/out parameter parsing ok flag
 * @return
 *      - int parameter
 *      - 0 on failed
 *          - \a *paramok is false at input
 *          - \a param is empty
 *          - \a param is not integer number
 *          - \a param is not in range
 */
int atParamIntInRange(atCmdParam_t *param, int minval, int maxval, bool *paramok);

/**
 * extract optional int parameter, and check range
 *
 * When \a *paramok is false, it will do nothing. On failure, \a *param
 * will be set to false on return.
 *
 * When \a param is empty, \a defval is returned. Otherwise, it is the
 * same as \a atParamUintInRange. The \a defval is not required in the
 * range.
 *
 * @param param     parameter pointer
 * @param defval    default value
 * @param minval    minimum valid value, inclusive
 * @param maxval    maximum valid value, inclusive
 * @param paramok   in/out parameter parsing ok flag
 * @return
 *      - int parameter
 *      - 0 on failed
 *          - \a *paramok is false at input
 *          - \a param is not integer number
 *          - \a param is not in range
 */
int atParamDefIntInRange(atCmdParam_t *param, int defval, int minval, int maxval, bool *paramok);

/**
 * extract int parameter, and check in list
 *
 * When \a *paramok is false, it will do nothing. On failure, \a *param
 * will be set to false on return.
 *
 * @param param     parameter pointer
 * @param list      array of valid values
 * @param count     valid value count
 * @param paramok   in/out parameter parsing ok flag
 * @return
 *      - int parameter
 *      - 0 on failed
 *          - \a *paramok is false at input
 *          - \a param is empty
 *          - \a param is not integer number
 *          - \a param is not in list
 */
int atParamIntInList(atCmdParam_t *param, const int *list, unsigned count, bool *paramok);

/**
 * extract optional int parameter, and check in list
 *
 * When \a *paramok is false, it will do nothing. On failure, \a *param
 * will be set to false on return.
 *
 * When \a param is empty, \a defval is returned. Otherwise, it is the
 * same as \a atParamUintInList. The \a defval is not required in the
 * list.
 *
 * @param param     parameter pointer
 * @param defval    default value
 * @param list      array of valid values
 * @param count     valid value count
 * @param paramok   in/out parameter parsing ok flag
 * @return
 *      - int parameter
 *      - 0 on failed
 *          - \a *paramok is false at input
 *          - \a param is not integer number
 *          - \a param is not in list
 */
int atParamDefIntInList(atCmdParam_t *param, int defval, const int *list, unsigned count, bool *paramok);

/**
 * extract uint64 parameter
 *
 * When \a *paramok is false, it will do nothing. On failure, \a *param
 * will be set to false on return.
 *
 * @param param     parameter pointer
 * @param paramok   in/out parameter parsing ok flag
 * @return
 *      - uint parameter
 *      - 0 on failed
 *          - \a *paramok is false at input
 *          - \a param is empty
 *          - \a param is not integer number
 */
uint64_t atParamUint64(atCmdParam_t *param, bool *paramok);

/**
 * extract optional uint64 parameter
 *
 * When \a *paramok is false, it will do nothing. On failure, \a *param
 * will be set to false on return.
 *
 * When \a param is empty, \a defval is returned. Otherwise, it is the
 * same as \a atParamUint.
 *
 * @param param     parameter pointer
 * @param defval    default value
 * @param paramok   in/out parameter parsing ok flag
 * @return
 *      - uint parameter
 *      - 0 on failed
 *          - \a *paramok is false at input
 *          - \a param is not integer number
 */
uint64_t atParamDefUint64(atCmdParam_t *param, uint64_t defval, bool *paramok);

/**
 * extract uint64 parameter, and check range
 *
 * When \a *paramok is false, it will do nothing. On failure, \a *param
 * will be set to false on return.
 *
 * @param param     parameter pointer
 * @param minval    minimum valid value, inclusive
 * @param maxval    maximum valid value, inclusive
 * @param paramok   in/out parameter parsing ok flag
 * @return
 *      - uint parameter
 *      - 0 on failed
 *          - \a *paramok is false at input
 *          - \a param is empty
 *          - \a param is not integer number
 *          - \a param is not in range
 */
uint64_t atParamUint64InRange(atCmdParam_t *param, uint64_t minval, uint64_t maxval, bool *paramok);

/**
 * extract optional uint64 parameter, and check range
 *
 * When \a *paramok is false, it will do nothing. On failure, \a *param
 * will be set to false on return.
 *
 * When \a param is empty, \a defval is returned. Otherwise, it is the
 * same as \a atParamUintInRange. The \a defval is not required in the
 * range.
 *
 * @param param     parameter pointer
 * @param defval    default value
 * @param minval    minimum valid value, inclusive
 * @param maxval    maximum valid value, inclusive
 * @param paramok   in/out parameter parsing ok flag
 * @return
 *      - uint parameter
 *      - 0 on failed
 *          - \a *paramok is false at input
 *          - \a param is not integer number
 *          - \a param is not in range
 */
uint64_t atParamDefUint64InRange(atCmdParam_t *param, uint64_t defval, uint64_t minval, uint64_t maxval, bool *paramok);

/**
 * extract uint64 parameter, and check in list
 *
 * When \a *paramok is false, it will do nothing. On failure, \a *param
 * will be set to false on return.
 *
 * @param param     parameter pointer
 * @param list      array of valid values
 * @param count     valid value count
 * @param paramok   in/out parameter parsing ok flag
 * @return
 *      - uint parameter
 *      - 0 on failed
 *          - \a *paramok is false at input
 *          - \a param is empty
 *          - \a param is not integer number
 *          - \a param is not in list
 */
uint64_t atParamUint64InList(atCmdParam_t *param, const uint64_t *list, unsigned count, bool *paramok);

/**
 * extract optional uint64 parameter, and check in list
 *
 * When \a *paramok is false, it will do nothing. On failure, \a *param
 * will be set to false on return.
 *
 * When \a param is empty, \a defval is returned. Otherwise, it is the
 * same as \a atParamUintInList. The \a defval is not required in the
 * list.
 *
 * @param param     parameter pointer
 * @param defval    default value
 * @param list      array of valid values
 * @param count     valid value count
 * @param paramok   in/out parameter parsing ok flag
 * @return
 *      - uint parameter
 *      - 0 on failed
 *          - \a *paramok is false at input
 *          - \a param is not integer number
 *          - \a param is not in list
 */
uint64_t atParamDefUint64InList(atCmdParam_t *param, uint64_t defval, const uint64_t *list, unsigned count, bool *paramok);

/**
 * extract int64 parameter
 *
 * When \a *paramok is false, it will do nothing. On failure, \a *param
 * will be set to false on return.
 *
 * @param param     parameter pointer
 * @param paramok   in/out parameter parsing ok flag
 * @return
 *      - int parameter
 *      - 0 on failed
 *          - \a *paramok is false at input
 *          - \a param is empty
 *          - \a param is not integer number
 */
int64_t atParamInt64(atCmdParam_t *param, bool *paramok);

/**
 * extract optional int64 parameter
 *
 * When \a *paramok is false, it will do nothing. On failure, \a *param
 * will be set to false on return.
 *
 * When \a param is empty, \a defval is returned. Otherwise, it is the
 * same as \a atParamUint.
 *
 * @param param     parameter pointer
 * @param defval    default value
 * @param paramok   in/out parameter parsing ok flag
 * @return
 *      - int parameter
 *      - 0 on failed
 *          - \a *paramok is false at input
 *          - \a param is not integer number
 */
int64_t atParamDefInt64(atCmdParam_t *param, int64_t defval, bool *paramok);

/**
 * extract int64 parameter, and check range
 *
 * When \a *paramok is false, it will do nothing. On failure, \a *param
 * will be set to false on return.
 *
 * @param param     parameter pointer
 * @param minval    minimum valid value, inclusive
 * @param maxval    maximum valid value, inclusive
 * @param paramok   in/out parameter parsing ok flag
 * @return
 *      - int parameter
 *      - 0 on failed
 *          - \a *paramok is false at input
 *          - \a param is empty
 *          - \a param is not integer number
 *          - \a param is not in range
 */
int64_t atParamInt64InRange(atCmdParam_t *param, int64_t minval, int64_t maxval, bool *paramok);

/**
 * extract optional int64 parameter, and check range
 *
 * When \a *paramok is false, it will do nothing. On failure, \a *param
 * will be set to false on return.
 *
 * When \a param is empty, \a defval is returned. Otherwise, it is the
 * same as \a atParamUintInRange. The \a defval is not required in the
 * range.
 *
 * @param param     parameter pointer
 * @param defval    default value
 * @param minval    minimum valid value, inclusive
 * @param maxval    maximum valid value, inclusive
 * @param paramok   in/out parameter parsing ok flag
 * @return
 *      - int parameter
 *      - 0 on failed
 *          - \a *paramok is false at input
 *          - \a param is not integer number
 *          - \a param is not in range
 */
int64_t atParamDefInt64InRange(atCmdParam_t *param, int64_t defval, int64_t minval, int64_t maxval, bool *paramok);

/**
 * extract int64 parameter, and check in list
 *
 * When \a *paramok is false, it will do nothing. On failure, \a *param
 * will be set to false on return.
 *
 * @param param     parameter pointer
 * @param list      array of valid values
 * @param count     valid value count
 * @param paramok   in/out parameter parsing ok flag
 * @return
 *      - int parameter
 *      - 0 on failed
 *          - \a *paramok is false at input
 *          - \a param is empty
 *          - \a param is not integer number
 *          - \a param is not in list
 */
int64_t atParamInt64InList(atCmdParam_t *param, const int64_t *list, unsigned count, bool *paramok);

/**
 * extract optional int64 parameter, and check in list
 *
 * When \a *paramok is false, it will do nothing. On failure, \a *param
 * will be set to false on return.
 *
 * When \a param is empty, \a defval is returned. Otherwise, it is the
 * same as \a atParamUintInList. The \a defval is not required in the
 * list.
 *
 * @param param     parameter pointer
 * @param defval    default value
 * @param list      array of valid values
 * @param count     valid value count
 * @param paramok   in/out parameter parsing ok flag
 * @return
 *      - int parameter
 *      - 0 on failed
 *          - \a *paramok is false at input
 *          - \a param is not integer number
 *          - \a param is not in list
 */
int64_t atParamDefInt64InList(atCmdParam_t *param, int64_t defval, const int64_t *list, unsigned count, bool *paramok);

/**
 * extract string parameter
 *
 * When \a *paramok is false, it will do nothing. On failure, \a *param
 * will be set to false on return.
 *
 * String parameter is parameter started and ended with double quotation.
 * The escape defined in V.250 is \\HH. For example, \30 for '0'.
 *
 * It is possible that the internal storage will be changed during this
 * function call. So, don't call other parameter APIs for the same
 * parameter after it is called. However, this function itself can be
 * called again.
 *
 * The output pointer will be valid till the parameter itself is deleted.
 *
 * The output string is ended with null byte for end of string.
 *
 * @param param     parameter pointer
 * @param paramok   in/out parameter parsing ok flag
 * @return
 *      - parsed string
 *      - NULL on failed
 *          - \a *paramok is false at input
 *          - \a param is empty
 *          - \a param is not string (start and end with double quotation)
 */
const char *atParamStr(atCmdParam_t *param, bool *paramok);

/**
 * extract hex parameter from raw text
 *
 * When \a *paramok is false, it will do nothing. On failure, \a *param
 * will be set to false on return.
 *
 * Raw text parameter is parameter started and ended without double quotation.
 * The escape defined in V.250 is \\HH. For example, \30 for '0'.
 *
 * It is possible that the internal storage will be changed during this
 * function call. So, don't call other parameter APIs for the same
 * parameter after it is called. However, this function itself can be
 * called again.
 *
 * The output pointer will be valid till the parameter itself is deleted.
 *
 * The output string is ended with null byte for end of string.
 *
 * @param param     parameter pointer
 * @param len     the length of the parameter
 * @param paramok   in/out parameter parsing ok flag
 * @return
 *      - parsed string
 *      - NULL on failed
 *          - \a *paramok is false at input
 *          - \a param is empty
 *          - \a param is string (start and end with double quotation)
 */
const char *atParamHexRawText(atCmdParam_t *param, uint32_t len, bool *paramok);
/**
 * extract string parameter
 *
 * When \a *paramok is false, it will do nothing. On failure, \a *param
 * will be set to false on return.
 *
 * String parameter is parameter started and ended with double quotation.
 * The escape defined in V.250 is \\HH. For example, \30 for '0'.
 *
 * It is possible that the internal storage will be changed during this
 * function call. So, don't call other parameter APIs for the same
 * parameter after it is called. However, this function itself can be
 * called again.
 *
 * The output pointer will be valid till the parameter itself is deleted.
 *
 * The output string is ended with null byte for end of string.
 *
 * @param param     parameter pointer
 * @param paramok   in/out parameter parsing ok flag
 * @return
 *      - parsed string
 *      - NULL on failed
 *          - \a *paramok is false at input
 *          - \a param is empty
 *          - \a param is not string (start and end with double quotation)
 */
const char *atParamOptStr(atCmdParam_t *param, bool *paramok);
/**
 * extract optional string parameter
 *
 * When \a *paramok is false, it will do nothing. On failure, \a *param
 * will be set to false on return.
 *
 * When \a param is empty, \a defval is returned. Otherwise, it is the
 * same as \a atParamStr.
 *
 * It is possible that the internal storage will be changed during this
 * function call. So, don't call other parameter APIs for the same
 * parameter after it is called. However, this function itself can be
 * called again.
 *
 * @param param     parameter pointer
 * @param defval    default value
 * @param paramok   in/out parameter parsing ok flag
 * @return
 *      - parsed string
 *      - NULL on failed
 *          - \a *paramok is false at input
 *          - \a param is not string (start and end with double quotation)
 */
const char *atParamDefStr(atCmdParam_t *param, const char *defval, bool *paramok);

/**
 * extract raw parameter
 *
 * When \a *paramok is false, it will do nothing. On failure, \a *param
 * will be set to false on return.
 *
 * The raw parameter is not parsed. For example, the double quotation of
 * string parameter will be kept, and the escape in string parameter is
 * untouched.
 *
 * The output string is ended with null byte as end of string.
 *
 * @param param     parameter pointer
 * @param paramok   in/out parameter parsing ok flag
 * @return
 *      - parsed string
 *      - NULL on failed
 *          - \a *paramok is false at input
 *          - \a param is empty
 */
const char *atParamRawText(atCmdParam_t *param, bool *paramok);

/**
 * extract floating point parameter
 *
 * When \a *paramok is false, it will do nothing. On failure, \a *param
 * will be set to false on return.
 *
 * @param param     parameter pointer
 * @param paramok   in/out parameter parsing ok flag
 * @return
 *      - floating point parameter
 *      - NULL on failed
 *          - \a *paramok is false at input
 *          - \a param is empty
 *          - \a param is not double
 */
double atParamDouble(atCmdParam_t *param, bool *paramok);

/**
 * extract DTMF parameter
 *
 * When \a *paramok is false, it will do nothing. On failure, \a *param
 * will be set to false on return.
 *
 * There are 2 kinds of DTMF parameter:
 * - one DTMF character without double quotation
 * - multiple DTMF chaeacters with double quotation
 * In both case, the return value is DTMF character string with null byte
 * as the string end.
 *
 * It will not check whether DTMF characters inside the string is valid
 * DTMF character. Application should validate them.
 *
 * It is possible that the internal storage will be changed during this
 * function call. So, don't call other parameter APIs for the same
 * parameter after it is called. However, this function itself can be
 * called again.
 *
 * @param param     parameter pointer
 * @param paramok   in/out parameter parsing ok flag
 * @return
 *      - parsed DTMF string
 *      - NULL on failed
 *          - \a *paramok is false at input
 *          - \a param is empty
 *          - \a param is not DTMF
 */
const char *atParamDtmf(atCmdParam_t *param, bool *paramok);

/**
 * extract string parameter and return mapped uint
 *
 * When \a *paramok is false, it will do nothing. On failure, \a *param
 * will be set to false on return.
 *
 * It is just a wrapper to:
 * - extract string parameter
 * - map to uint
 *
 * @param param     parameter pointer
 * @param vsmap     integer/string map, ended with NULL string value
 * @param paramok   in/out parameter parsing ok flag
 * @return
 *      - uint parameter
 *      - NULL on failed
 *          - \a *paramok is false at input
 *          - \a param is empty
 *          - \a param is not in map
 */
uint32_t atParamUintByStrMap(atCmdParam_t *param, const osiValueStrMap_t *vsmap, bool *paramok);

/**
 * extract optional string parameter and return mapped uint
 *
 * When \a *paramok is false, it will do nothing. On failure, \a *param
 * will be set to false on return.
 *
 * When \a param is empty, \a defval is returned. Otherwise, it is the
 * same as \a atParamUintByStrMap. The \a defval is not required in the
 * map.
 *
 * @param param     parameter pointer
 * @param defval    default value
 * @param vsmap     integer/string map, ended with NULL string value
 * @param paramok   in/out parameter parsing ok flag
 * @return
 *      - uint parameter
 *      - NULL on failed
 *          - \a *paramok is false at input
 *          - \a param is not in map
 */
uint32_t atParamDefUintByStrMap(atCmdParam_t *param, uint32_t defval, const osiValueStrMap_t *vsmap, bool *paramok);

/**
 * extract uint parameter by hex string
 *
 * When \a *paramok is false, it will do nothing. On failure, \a *param
 * will be set to false on return.
 *
 * The valid parameter is the output of:
 * \code{.cpp}
 * printf("\"%x\"", value)
 * \endcode
 *
 * For example, parameter "12345" will be parsed as 0x12345.
 *
 * @param param     parameter pointer
 * @param paramok   in/out parameter parsing ok flag
 * @return
 *      - uint parameter
 *      - NULL on failed
 *          - \a *paramok is false at input
 *          - \a param is empty
 *          - \a param is not the needed format
 */
uint32_t atParamHexStrUint(atCmdParam_t *param, bool *paramok);

/**
 * trim tailing characters at the end of parameter
 *
 * This shall be called before the parameter isn't parsed. \p atParamRawText
 * won't parse the parameter, so it is safe to call it after \p atParamRawText.
 *
 * It will change the internal storage of parameter directly. So, only call
 * it when absolutely needed, and you know what you are doing.
 *
 * @param param     parameter pointer
 * @param paramok   in/out parameter parsing ok flag
 * @return
 *      - true on success
 *      - false on failed
 *          - \p param type is not raw text
 *          - the length of parameter is shorter than \p len
 */
bool atParamTrimTail(atCmdParam_t *param, uint32_t len);

/**
 * check whether the parameter is empty
 *
 * Both not specified and skipped parameter are empty. For example:
 * - AT+CMD=123
 * - AT+CMD=123,
 * - AT+CMD=123,,456
 *
 * In all of the above cases, params[1] is empty.
 *
 * @param param     parameter pointer
 * @return
 *      - true if the parameter is empty
 *      - false otherwise
 */
bool atParamIsEmpty(atCmdParam_t *param);

/**
 * extract hex int64_t parameter, and check range
 *
 * When \a *paramok is false, it will do nothing. On failure, \a *param
 * will be set to false on return.
 *
 * @param param     parameter pointer
 * @param minval    minimum valid value, inclusive
 * @param maxval    maximum valid value, inclusive
 * @param paramok   in/out parameter parsing ok flag
 * @return
 *      - uint parameter
 *      - 0 on failed
 *          - \a *paramok is false at input
 *          - \a param is empty
 *          - \a param is not integer number
 *          - \a param is not in range
 */
int64_t atParamHexInt64InRange(atCmdParam_t *param, int64_t minval, int64_t maxval, bool *paramok);

#endif
