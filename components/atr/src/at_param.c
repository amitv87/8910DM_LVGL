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

#include "at_param.h"
#include "at_parse.h"
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>

uint32_t atParamUint(atCmdParam_t *param, bool *paramok)
{
    if (!*paramok || param == NULL || param->type != AT_CMD_PARAM_TYPE_RAW)
        goto failed;

    char *endptr = NULL;
    errno = 0; // clear errno due to there are no error return value
    if (param->value[0] == '-')
        goto failed;
    uint32_t val = strtoul(param->value, &endptr, 10);
    if (*endptr != '\0' || errno != 0)
        goto failed;

    return val;

failed:
    *paramok = false;
    return 0;
}

uint32_t atParamDefUint(atCmdParam_t *param, uint32_t defval, bool *paramok)
{
    if (atParamIsEmpty(param))
        return defval;

    return atParamUint(param, paramok);
}

uint32_t atParamUintInRange(atCmdParam_t *param, uint32_t minval, uint32_t maxval, bool *paramok)
{
    uint32_t val = atParamUint(param, paramok);
    if (*paramok && osiIsUintInRange(val, minval, maxval))
        return val;

    *paramok = false;
    return 0;
}

uint32_t atParamDefUintInRange(atCmdParam_t *param, uint32_t defval, uint32_t minval, uint32_t maxval, bool *paramok)
{
    if (atParamIsEmpty(param))
        return defval;

    return atParamUintInRange(param, minval, maxval, paramok);
}

uint32_t atParamUintInList(atCmdParam_t *param, const uint32_t *list, unsigned count, bool *paramok)
{
    uint32_t val = atParamUint(param, paramok);
    if (*paramok && osiIsUintInList(val, list, count))
        return val;

    *paramok = false;
    return 0;
}

uint32_t atParamDefUintInList(atCmdParam_t *param, uint32_t defval, const uint32_t *list, unsigned count, bool *paramok)
{
    if (atParamIsEmpty(param))
        return defval;

    return atParamUintInList(param, list, count, paramok);
}

int atParamInt(atCmdParam_t *param, bool *paramok)
{
    if (!*paramok || param == NULL || param->type != AT_CMD_PARAM_TYPE_RAW)
        goto failed;

    char *endptr = NULL;
    errno = 0; // clear errno due to there are no error return value
    int val = strtol(param->value, &endptr, 10);
    if (*endptr != '\0' || errno != 0)
        goto failed;

    return val;

failed:
    *paramok = false;
    return 0;
}

int atParamDefInt(atCmdParam_t *param, int defval, bool *paramok)
{
    if (atParamIsEmpty(param))
        return defval;

    return atParamInt(param, paramok);
}

int atParamIntInRange(atCmdParam_t *param, int minval, int maxval, bool *paramok)
{
    int val = atParamInt(param, paramok);
    if (*paramok && osiIsIntInRange(val, minval, maxval))
        return val;

    *paramok = false;
    return 0;
}

int atParamDefIntInRange(atCmdParam_t *param, int defval, int minval, int maxval, bool *paramok)
{
    if (atParamIsEmpty(param))
        return defval;

    return atParamIntInRange(param, minval, maxval, paramok);
}

int atParamIntInList(atCmdParam_t *param, const int *list, unsigned count, bool *paramok)
{
    int val = atParamInt(param, paramok);
    if (*paramok && osiIsIntInList(val, list, count))
        return val;

    *paramok = false;
    return 0;
}

int atParamDefIntInList(atCmdParam_t *param, int defval, const int *list, unsigned count, bool *paramok)
{
    if (atParamIsEmpty(param))
        return defval;

    return atParamIntInList(param, list, count, paramok);
}

uint64_t atParamUint64(atCmdParam_t *param, bool *paramok)
{
    if (!*paramok || param == NULL || param->type != AT_CMD_PARAM_TYPE_RAW)
        goto failed;

    char *endptr = NULL;
    errno = 0; // clear errno due to there are no error return value
    uint64_t val = strtoull(param->value, &endptr, 10);
    if (*endptr != '\0' || errno != 0)
        goto failed;

    return val;

failed:
    *paramok = false;
    return 0;
}

uint64_t atParamDefUint64(atCmdParam_t *param, uint64_t defval, bool *paramok)
{
    if (atParamIsEmpty(param))
        return defval;

    return atParamUint64(param, paramok);
}

uint64_t atParamUint64InRange(atCmdParam_t *param, uint64_t minval, uint64_t maxval, bool *paramok)
{
    uint64_t val = atParamUint64(param, paramok);
    if (*paramok && osiIsUint64InRange(val, minval, maxval))
        return val;

    *paramok = false;
    return 0;
}

uint64_t atParamDefUint64InRange(atCmdParam_t *param, uint64_t defval, uint64_t minval, uint64_t maxval, bool *paramok)
{
    if (atParamIsEmpty(param))
        return defval;

    return atParamUint64InRange(param, minval, maxval, paramok);
}

uint64_t atParamUint64InList(atCmdParam_t *param, const uint64_t *list, unsigned count, bool *paramok)
{
    uint64_t val = atParamUint64(param, paramok);
    if (*paramok && osiIsUint64InList(val, list, count))
        return val;

    *paramok = false;
    return 0;
}

uint64_t atParamDefUint64InList(atCmdParam_t *param, uint64_t defval, const uint64_t *list, unsigned count, bool *paramok)
{
    if (atParamIsEmpty(param))
        return defval;

    return atParamUint64InList(param, list, count, paramok);
}

int64_t atParamInt64(atCmdParam_t *param, bool *paramok)
{
    if (!*paramok || param == NULL || param->type != AT_CMD_PARAM_TYPE_RAW)
        goto failed;

    char *endptr = NULL;
    errno = 0; // clear errno due to there are no error return value
    int64_t val = strtoll(param->value, &endptr, 10);
    if (*endptr != '\0' || errno != 0)
        goto failed;

    return val;

failed:
    *paramok = false;
    return 0;
}

int64_t atParamDefInt64(atCmdParam_t *param, int64_t defval, bool *paramok)
{
    if (atParamIsEmpty(param))
        return defval;

    return atParamInt64(param, paramok);
}

int64_t atParamInt64InRange(atCmdParam_t *param, int64_t minval, int64_t maxval, bool *paramok)
{
    int64_t val = atParamInt64(param, paramok);
    if (*paramok && osiIsInt64InRange(val, minval, maxval))
        return val;

    *paramok = false;
    return 0;
}

int64_t atParamDefInt64InRange(atCmdParam_t *param, int64_t defval, int64_t minval, int64_t maxval, bool *paramok)
{
    if (atParamIsEmpty(param))
        return defval;

    return atParamInt64InRange(param, minval, maxval, paramok);
}

int64_t atParamInt64InList(atCmdParam_t *param, const int64_t *list, unsigned count, bool *paramok)
{
    int64_t val = atParamInt64(param, paramok);
    if (*paramok && osiIsInt64InList(val, list, count))
        return val;

    *paramok = false;
    return 0;
}

int64_t atParamDefInt64InList(atCmdParam_t *param, int64_t defval, const int64_t *list, unsigned count, bool *paramok)
{
    if (atParamIsEmpty(param))
        return defval;

    return atParamInt64InList(param, list, count, paramok);
}

// =============================================================================
// at_ParamHex2Num ([0-9a-f] -> number)
// =============================================================================
static inline int atParamHex2Num(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'A' && c <= 'F')
        return 0xa + (c - 'A');
    if (c >= 'a' && c <= 'f')
        return 0xa + (c - 'a');
    return -1;
}

static const char *atParamStrFromRaw(atCmdParam_t *param, bool *paramok)
{
    if (param->length < 2 ||
        param->value[0] != '"' ||
        param->value[param->length - 1] != '"')
        goto failed;

    // param->value must be started and ended with double-quote
    char *s = param->value + 1;
    char *d = param->value;
    uint16_t length = param->length - 2;
    param->length = 0;
    while (length > 0)
    {
        char c = *s++;
        length--;
        // It is more permissive than SPEC.
        if (c == '\\' && length >= 2 && isalnum((int)s[0]) && isalnum((int)s[1]))
        {
            *d++ = (atParamHex2Num(s[0]) << 4) | atParamHex2Num(s[1]);
            s += 2;
            length -= 2;
        }
        else
        {
            *d++ = c;
        }
        param->length++;
    }
    *d = '\0';
    param->type = AT_CMD_PARAM_TYPE_PARSED_STRING;
    return param->value;

failed:
    *paramok = false;
    return "";
}

const char *atParamStr(atCmdParam_t *param, bool *paramok)
{
    if (!*paramok || param == NULL)
        goto failed;
    if (param->type == AT_CMD_PARAM_TYPE_PARSED_STRING)
        return param->value;
    if (param->type == AT_CMD_PARAM_TYPE_RAW)
        return atParamStrFromRaw(param, paramok);

failed:
    *paramok = false;
    return "";
}

static bool atParamHexDataParse(atCmdParam_t *param)
{
    if (param->type == AT_CMD_PARAM_TYPE_PARSED_STRING)
        return true;

    if (param->length % 2 != 0)
        return false;

    const char *pv = param->value;
    for (unsigned n = 0; n < param->length; n++)
    {
        if (atParamHex2Num(*pv++) < 0)
            return false;
    }

    char *s = param->value;
    char *d = param->value;
    param->length /= 2;
    for (unsigned n = 0; n < param->length; n++)
    {
        int num0 = atParamHex2Num(s[0]);
        int num1 = atParamHex2Num(s[1]);
        *d++ = (num0 << 4) | num1;
        s += 2;
    }
    *d = 0;
    param->type = AT_CMD_PARAM_TYPE_PARSED_STRING;
    return true;
}

const char *atParamHexRawText(atCmdParam_t *param, uint32_t len, bool *paramok)
{
    if (!*paramok || param == NULL)
        goto failed;

    if (param->type == AT_CMD_PARAM_TYPE_PARSED_STRING)
        return param->value;

    if (param->type != AT_CMD_PARAM_TYPE_RAW)
        goto failed;

    if (param->length == (len * 2) && atParamHexDataParse(param))
        return param->value;

failed:
    *paramok = false;
    return "";
}

const char *atParamOptStr(atCmdParam_t *param, bool *paramok)
{
    if (*paramok && atParamIsEmpty(param))
        return "";

    return atParamStr(param, paramok);
}

const char *atParamDefStr(atCmdParam_t *param, const char *defval, bool *paramok)
{
    if (atParamIsEmpty(param))
        return defval;

    return atParamStr(param, paramok);
}

const char *atParamRawText(atCmdParam_t *param, bool *paramok)
{
    if ((!*paramok) || (param == NULL) || (param->type != AT_CMD_PARAM_TYPE_RAW))
        goto failed;

    return param->value;

failed:
    *paramok = false;
    return "";
}

double atParamDouble(atCmdParam_t *param, bool *paramok)
{
    if ((!*paramok) || (param == NULL) || (param->type != AT_CMD_PARAM_TYPE_RAW))
        goto failed;

    char *endptr = NULL;
    errno = 0; // clear errno due to there are no error return value
    double value = strtod(param->value, &endptr);
    if (*endptr != '\0' || errno != 0)
        goto failed;
    return value;

failed:
    *paramok = false;
    return 0.0;
}

const char *atParamDtmf(atCmdParam_t *param, bool *paramok)
{
    if (!*paramok || param == NULL)
        goto failed;
    if (param->type == AT_CMD_PARAM_TYPE_PARSED_DTMF)
        return param->value;
    if (param->type == AT_CMD_PARAM_TYPE_RAW &&
        param->length == 1)
        return param->value;
    if (param->type == AT_CMD_PARAM_TYPE_RAW &&
        param->length >= 2 &&
        param->value[0] == '"' &&
        param->value[param->length - 1] == '"')
    {
        int np = 0;
        for (int n = 1; n < param->length - 1; n++)
        {
            if (param->value[n] == ',' || param->value[n] == ' ')
                continue;
            param->value[np++] = param->value[n];
        }
        param->length = np;
        param->value[np] = 0;
        param->type = AT_CMD_PARAM_TYPE_PARSED_DTMF;
        return param->value;
    }

failed:
    *paramok = false;
    return "";
}

uint32_t atParamUintByStrMap(atCmdParam_t *param, const osiValueStrMap_t *svmap, bool *paramok)
{
    if (!*paramok)
        goto failed;

    const char *s = atParamStr(param, paramok);
    if (!*paramok)
        goto failed;

    const osiValueStrMap_t *m = osiVsmapFindByIStr(svmap, s);
    if (m == NULL)
        goto failed;

    return m->value;

failed:
    *paramok = false;
    return 0;
}

uint32_t atParamDefUintByStrMap(atCmdParam_t *param, uint32_t defval, const osiValueStrMap_t *svmap, bool *paramok)
{
    if (atParamIsEmpty(param))
        return defval;
    return atParamUintByStrMap(param, svmap, paramok);
}

uint32_t atParamHexStrUint(atCmdParam_t *param, bool *paramok)
{
    if (!*paramok)
        goto failed;

    unsigned val;
    int chars;
    if (sscanf(param->value, "\"%x\"%n", &val, &chars) != 1)
        goto failed;
    if (param->value[chars] != '\0')
        goto failed;

    return val;

failed:
    *paramok = false;
    return 0;
}

bool atParamTrimTail(atCmdParam_t *param, uint32_t length)
{
    if (param->type != AT_CMD_PARAM_TYPE_RAW || param->length < length)
        return false;

    param->length -= length;
    param->value[param->length] = '\0';
    return true;
}

bool atParamIsEmpty(atCmdParam_t *param)
{
    return param == NULL || param->type == AT_CMD_PARAM_TYPE_EMPTY;
}

int64_t atParamHexInt64(atCmdParam_t *param, bool *paramok)
{
    if (!*paramok || param == NULL || param->type != AT_CMD_PARAM_TYPE_RAW)
        goto failed;

    char *endptr = NULL;
    errno = 0; // clear errno due to there are no error return value
    int64_t val = strtoll(param->value, &endptr, 16);
    if (*endptr != '\0' || errno != 0)
        goto failed;

    return val;

failed:
    *paramok = false;
    return 0;
}

int64_t atParamHexInt64InRange(atCmdParam_t *param, int64_t minval, int64_t maxval, bool *paramok)
{
    int64_t val = atParamHexInt64(param, paramok);
    if (*paramok && osiIsInt64InRange(val, minval, maxval))
        return val;

    *paramok = false;
    return 0;
}

