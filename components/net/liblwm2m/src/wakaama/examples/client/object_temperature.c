
#include "internals.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LWM2M_TEMPERATURE_SENSOR_VALUE_ID 5700
#define LWM2M_TEMPERATURE_UNITS_ID 5701
#define LWM2M_TEMPERATURE_MIN_MEASURED_VALUE_ID 5601
#define LWM2M_TEMPERATURE_MAX_MEASURED_VALUE_ID 5602
#define LWM2M_TEMPERATURE_MIN_RANGE_VALUE_ID 5603
#define LWM2M_TEMPERATURE_MAX_RANGE_VALUE_ID 5604
#define LWM2M_TEMPERATURE_RESET_MIN_AND_MAX_MEASURED_VALUE_ID 5605

typedef struct _temperature_instance_
{
    struct _temperature_instance_ *next;
    uint16_t instanceId;
    double sensorValue;
    char *units;
    double minMeasuredValue;
    double maxMeasuredValue;
    double minRangeValue;
    double maxRangeValue;
} temperature_instance_t;

static uint8_t prv_get_value(lwm2m_data_t *dataP,
                             temperature_instance_t *targetP)
{
    switch (dataP->id)
    {
    case LWM2M_TEMPERATURE_SENSOR_VALUE_ID:
        lwm2m_data_encode_float(targetP->sensorValue, dataP);
        return COAP_205_CONTENT;

    case LWM2M_TEMPERATURE_UNITS_ID:
        lwm2m_data_encode_string(targetP->units, dataP);
        return COAP_205_CONTENT;

    case LWM2M_TEMPERATURE_MIN_MEASURED_VALUE_ID:
        lwm2m_data_encode_float(targetP->minMeasuredValue, dataP);
        return COAP_205_CONTENT;

    case LWM2M_TEMPERATURE_MAX_MEASURED_VALUE_ID:
        lwm2m_data_encode_float(targetP->maxMeasuredValue, dataP);
        return COAP_205_CONTENT;

    case LWM2M_TEMPERATURE_MIN_RANGE_VALUE_ID:
        lwm2m_data_encode_float(targetP->minRangeValue, dataP);
        return COAP_205_CONTENT;

    case LWM2M_TEMPERATURE_MAX_RANGE_VALUE_ID:
        lwm2m_data_encode_float(targetP->maxRangeValue, dataP);
        return COAP_205_CONTENT;

    default:
        return COAP_404_NOT_FOUND;
    }
}

static uint8_t prv_temperature_read(uint16_t instanceId,
                                    int *numDataP,
                                    lwm2m_data_t **dataArrayP,
                                    lwm2m_object_t *objectP)
{
    temperature_instance_t *targetP;
    uint8_t result;
    int i;

    targetP = (temperature_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
    if (NULL == targetP)
        return COAP_404_NOT_FOUND;

    // is the temperature asking for the full instance ?
    if (*numDataP == 0)
    {
        uint16_t resList[] = {
            LWM2M_TEMPERATURE_SENSOR_VALUE_ID,
            LWM2M_TEMPERATURE_UNITS_ID,
            LWM2M_TEMPERATURE_MIN_MEASURED_VALUE_ID,
            LWM2M_TEMPERATURE_MAX_MEASURED_VALUE_ID,
            LWM2M_TEMPERATURE_MIN_RANGE_VALUE_ID,
            LWM2M_TEMPERATURE_MAX_RANGE_VALUE_ID};
        int nbRes = sizeof(resList) / sizeof(uint16_t);

        *dataArrayP = lwm2m_data_new(nbRes);
        if (*dataArrayP == NULL)
            return COAP_500_INTERNAL_SERVER_ERROR;
        *numDataP = nbRes;
        for (i = 0; i < nbRes; i++)
        {
            (*dataArrayP)[i].id = resList[i];
        }
    }

    i = 0;
    do
    {
        result = prv_get_value((*dataArrayP) + i, targetP);
        i++;
    } while (i < *numDataP && result == COAP_205_CONTENT);

    return result;
}

static uint8_t prv_temperature_discover(uint16_t instanceId,
                                        int *numDataP,
                                        lwm2m_data_t **dataArrayP,
                                        lwm2m_object_t *objectP)
{
    uint8_t result;
    int i;

    result = COAP_205_CONTENT;

    // is the server asking for the full object ?
    if (*numDataP == 0)
    {
        uint16_t resList[] = {
            LWM2M_TEMPERATURE_SENSOR_VALUE_ID,
            LWM2M_TEMPERATURE_UNITS_ID,
            LWM2M_TEMPERATURE_MIN_MEASURED_VALUE_ID,
            LWM2M_TEMPERATURE_MAX_MEASURED_VALUE_ID,
            LWM2M_TEMPERATURE_MIN_RANGE_VALUE_ID,
            LWM2M_TEMPERATURE_MAX_RANGE_VALUE_ID,
            LWM2M_TEMPERATURE_RESET_MIN_AND_MAX_MEASURED_VALUE_ID};
        int nbRes = sizeof(resList) / sizeof(uint16_t);

        *dataArrayP = lwm2m_data_new(nbRes);
        if (*dataArrayP == NULL)
            return COAP_500_INTERNAL_SERVER_ERROR;
        *numDataP = nbRes;
        for (i = 0; i < nbRes; i++)
        {
            (*dataArrayP)[i].id = resList[i];
        }
    }
    else
    {
        for (i = 0; i < *numDataP && result == COAP_205_CONTENT; i++)
        {
            switch ((*dataArrayP)[i].id)
            {
            case LWM2M_TEMPERATURE_SENSOR_VALUE_ID:
            case LWM2M_TEMPERATURE_UNITS_ID:
            case LWM2M_TEMPERATURE_MIN_MEASURED_VALUE_ID:
            case LWM2M_TEMPERATURE_MAX_MEASURED_VALUE_ID:
            case LWM2M_TEMPERATURE_MIN_RANGE_VALUE_ID:
            case LWM2M_TEMPERATURE_MAX_RANGE_VALUE_ID:
            case LWM2M_TEMPERATURE_RESET_MIN_AND_MAX_MEASURED_VALUE_ID:
                break;
            default:
                result = COAP_404_NOT_FOUND;
            }
        }
    }

    return result;
}

static uint8_t prv_temperature_write(uint16_t instanceId,
                                     int numData,
                                     lwm2m_data_t *dataArray,
                                     lwm2m_object_t *objectP)
{
    temperature_instance_t *targetP;
    int i;

    targetP = (temperature_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
    if (NULL == targetP)
    {
        return COAP_404_NOT_FOUND;
    }

    i = 0;
    do
    {
        switch (dataArray[i].id)
        {
        case LWM2M_TEMPERATURE_SENSOR_VALUE_ID:
            if (1 != lwm2m_data_decode_float(dataArray + i, &(targetP->sensorValue)))
            {
                return COAP_400_BAD_REQUEST;
            }
            break;

        case LWM2M_TEMPERATURE_UNITS_ID:
            if (dataArray[i].type == LWM2M_TYPE_STRING && dataArray[i].value.asBuffer.length > 0)
            {
                targetP->units = (char *)lwm2m_malloc(dataArray[i].value.asBuffer.length);
                if (targetP->units == NULL)
                {
                    return COAP_400_BAD_REQUEST;
                }
                strncpy(targetP->units, (char *)dataArray[i].value.asBuffer.buffer, dataArray[i].value.asBuffer.length);
                break;
            }
            else
            {
                return COAP_400_BAD_REQUEST;
            }
            break;
        case LWM2M_TEMPERATURE_MIN_MEASURED_VALUE_ID:
            if (1 != lwm2m_data_decode_float(dataArray + i, &(targetP->minMeasuredValue)))
            {
                return COAP_400_BAD_REQUEST;
            }
            break;

        case LWM2M_TEMPERATURE_MAX_MEASURED_VALUE_ID:
            if (1 != lwm2m_data_decode_float(dataArray + i, &(targetP->maxMeasuredValue)))
            {
                return COAP_400_BAD_REQUEST;
            }
            break;

        case LWM2M_TEMPERATURE_MIN_RANGE_VALUE_ID:
            if (1 != lwm2m_data_decode_float(dataArray + i, &(targetP->minRangeValue)))
            {
                return COAP_400_BAD_REQUEST;
            }
            break;

        case LWM2M_TEMPERATURE_MAX_RANGE_VALUE_ID:
            if (1 != lwm2m_data_decode_float(dataArray + i, &(targetP->maxRangeValue)))
            {
                return COAP_400_BAD_REQUEST;
            }

            break;
        default:
            return COAP_404_NOT_FOUND;
        }
        i++;
    } while (i < numData);

    return COAP_204_CHANGED;
}

static uint8_t prv_temperature_delete(uint16_t id,
                                      lwm2m_object_t *objectP)
{
    temperature_instance_t *temperatureInstance;

    objectP->instanceList = lwm2m_list_remove(objectP->instanceList, id, (lwm2m_list_t **)&temperatureInstance);
    if (NULL == temperatureInstance)
        return COAP_404_NOT_FOUND;

    lwm2m_free(temperatureInstance);

    return COAP_202_DELETED;
}

static uint8_t prv_temperature_create(uint16_t instanceId,
                                      int numData,
                                      lwm2m_data_t *dataArray,
                                      lwm2m_object_t *objectP)
{
    temperature_instance_t *temperatureInstance;
    uint8_t result;

    temperatureInstance = (temperature_instance_t *)lwm2m_malloc(sizeof(temperature_instance_t));
    if (NULL == temperatureInstance)
        return COAP_500_INTERNAL_SERVER_ERROR;
    memset(temperatureInstance, 0, sizeof(temperature_instance_t));

    temperatureInstance->instanceId = instanceId;
    objectP->instanceList = LWM2M_LIST_ADD(objectP->instanceList, temperatureInstance);

    result = prv_temperature_write(instanceId, numData, dataArray, objectP);

    if (result != COAP_204_CHANGED)
    {
        (void)prv_temperature_delete(instanceId, objectP);
    }
    else
    {
        result = COAP_201_CREATED;
    }

    return result;
}

static uint8_t prv_temperature_execute(uint16_t instanceId,
                                       uint16_t resourceId,
                                       uint8_t *buffer,
                                       int length,
                                       lwm2m_object_t *objectP)
{
    temperature_instance_t *targetP;

    targetP = (temperature_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
    if (NULL == targetP)
        return COAP_404_NOT_FOUND;

    switch (resourceId)
    {
    case LWM2M_TEMPERATURE_RESET_MIN_AND_MAX_MEASURED_VALUE_ID:
        // TODO
        return COAP_204_CHANGED;
    default:
        return COAP_405_METHOD_NOT_ALLOWED;
    }
}

static void clean_temperature_object(lwm2m_object_t *object)
{
    while (object->instanceList != NULL)
    {
        temperature_instance_t *temperatureInstance = (temperature_instance_t *)object->instanceList;
        object->instanceList = object->instanceList->next;
        lwm2m_free(temperatureInstance);
    }
    lwm2m_free(object);
}

lwm2m_object_t *get_temperature_object(float sensorValue,
                                       const char *units, float minMeasuredValue, float maxMeasuredValue,
                                       float minRangeValue, float maxRangeValue)
{
    lwm2m_object_t *temperatureObj;
    temperatureObj = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));
    if (NULL != temperatureObj)
    {
        memset(temperatureObj, 0, sizeof(lwm2m_object_t));
        temperatureObj->objID = LWM2M_TEMPERATURE_OBJECT_ID;

        // Manually create an hardcoded server
        temperature_instance_t *temperatureInstance;
        temperatureInstance = (temperature_instance_t *)lwm2m_malloc(sizeof(temperature_instance_t));
        if (NULL == temperatureInstance)
        {
            lwm2m_free(temperatureObj);
            return NULL;
        }

        memset(temperatureInstance, 0, sizeof(temperature_instance_t));
        temperatureInstance->instanceId = 0;
        temperatureInstance->sensorValue = sensorValue;
        temperatureInstance->units = lwm2m_strdup(units);
        temperatureInstance->minMeasuredValue = minMeasuredValue;
        temperatureInstance->maxMeasuredValue = maxMeasuredValue;
        temperatureInstance->minRangeValue = minRangeValue;
        temperatureInstance->maxRangeValue = maxRangeValue;

        temperatureObj->instanceList = LWM2M_LIST_ADD(temperatureObj->instanceList, temperatureInstance);

        temperatureObj->readFunc = prv_temperature_read;
        temperatureObj->createFunc = prv_temperature_create;
        temperatureObj->deleteFunc = prv_temperature_delete;
        temperatureObj->executeFunc = prv_temperature_execute;
        temperatureObj->discoverFunc = prv_temperature_discover;
        temperatureObj->cleanFunc = clean_temperature_object;
    }

    return temperatureObj;
}

