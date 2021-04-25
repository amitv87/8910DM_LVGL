#ifdef LWIP_HTTP_TEST
#ifndef _HTTP_JSON_H
#define _HTTP_JSON_H
#include "cJSON.h"
#include "mupnp/util/log.h"

char *httpgenerateJsonString(char *regver, char *meid, char *modelsms,
                             char *swver, char *SIM1Iccid, char *SIM1LteImsi);
#endif
#endif