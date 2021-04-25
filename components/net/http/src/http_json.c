#ifdef LWIP_HTTP_TEST

#include "cJSON.h"
#include "mupnp/util/log.h"

char *httpgenerateJsonString(char *regver, char *meid, char *modelsms,
                             char *swver, char *SIM1Iccid, char *SIM1LteImsi)
{
    cJSON *cjson = NULL;
    cjson = cJSON_CreateObject();
    if (NULL == cjson)
    {
        mupnp_log_debug_s("JSON error ++ NULL");
        return NULL;
    }

    cJSON_AddStringToObject(cjson, "REGVER", regver);
    cJSON_AddStringToObject(cjson, "MEID", meid);
    cJSON_AddStringToObject(cjson, "MODELSMS", modelsms);
    cJSON_AddStringToObject(cjson, "SWVER", swver);
    cJSON_AddStringToObject(cjson, "SIM1ICCID", SIM1Iccid);
    cJSON_AddStringToObject(cjson, "SIM1LTEIMSI", SIM1LteImsi);

    char *out = cJSON_PrintUnformatted(cjson);
    mupnp_log_debug_s("JSON++ %s", out);
    cJSON_Delete(cjson);
    return out;
}
#endif