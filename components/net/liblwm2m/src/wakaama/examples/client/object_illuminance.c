#include "liblwm2m.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LWM2M_ILLUMINANCE_SENSOR_VALUE_ID 5700
#define LWM2M_ILLUMINANCE_UNITS_ID 5701
#define LWM2M_ILLUMINANCE_MIN_MEASURED_VALUE_ID 5601
#define LWM2M_ILLUMINANCE_MAX_MEASURED_VALUE_ID 5602
#define LWM2M_ILLUMINANCE_MIN_RANGE_VALUE_ID 5603
#define LWM2M_ILLUMINANCE_MAX_RANGE_VALUE_ID 5604
#define LWM2M_ILLUMINANCE_RESET_MIN_AND_MAX_MEASURED_VALUE_ID 5605

typedef struct _illuminance_instance_
{
    struct _illuminance_instance_ *next;
    uint16_t instanceId;
    float sensorValue;
    char *units;
    float minMeasuredValue;
    float maxMeasuredValue;
    float minRangeValue;
    float maxRangeValue;
} illuminance_instance_t;

static uint8_t prv_get_value(lwm2m_data_t *dataP,
                             illuminance_instance_t *targetP)
{
    switch (dataP->id)
    {
    case LWM2M_ILLUMINANCE_SENSOR_VALUE_ID:
        lwm2m_data_encode_float(targetP->sensorValue, dataP);
        return COAP_205_CONTENT;

    case LWM2M_ILLUMINANCE_UNITS_ID:
        lwm2m_data_encode_string(targetP->units, dataP);
        return COAP_205_CONTENT;

    case LWM2M_ILLUMINANCE_MIN_MEASURED_VALUE_ID:
        lwm2m_data_encode_float(targetP->minMeasuredValue, dataP);
        return COAP_205_CONTENT;

    case LWM2M_ILLUMINANCE_MAX_MEASURED_VALUE_ID:
        lwm2m_data_encode_float(targetP->maxMeasuredValue, dataP);
        return COAP_205_CONTENT;

    case LWM2M_ILLUMINANCE_MIN_RANGE_VALUE_ID:
        lwm2m_data_encode_float(targetP->minRangeValue, dataP);
        return COAP_205_CONTENT;

    case LWM2M_ILLUMINANCE_MAX_RANGE_VALUE_ID:
        lwm2m_data_encode_float(targetP->maxRangeValue, dataP);
        return COAP_205_CONTENT;

    default:
        return COAP_404_NOT_FOUND;
    }
}

static uint8_t prv_illuminance_read(uint16_t instanceId,
                                    int *numDataP,
                                    lwm2m_data_t **dataArrayP,
                                    lwm2m_object_t *objectP)
{
    illuminance_instance_t *targetP;
    uint8_t result;
    int i;

    targetP = (illuminance_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
    if (NULL == targetP)
        return COAP_404_NOT_FOUND;

    // is the server asking for the full instance ?
    if (*numDataP == 0)
    {
        uint16_t resList[] = {
            LWM2M_ILLUMINANCE_SENSOR_VALUE_ID,
            LWM2M_ILLUMINANCE_UNITS_ID,
            LWM2M_ILLUMINANCE_MIN_MEASURED_VALUE_ID,
            LWM2M_ILLUMINANCE_MAX_MEASURED_VALUE_ID,
            LWM2M_ILLUMINANCE_MIN_RANGE_VALUE_ID,
            LWM2M_ILLUMINANCE_MAX_RANGE_VALUE_ID};
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

static uint8_t prv_illuminance_discover(uint16_t instanceId,
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
            LWM2M_ILLUMINANCE_SENSOR_VALUE_ID,
            LWM2M_ILLUMINANCE_UNITS_ID,
            LWM2M_ILLUMINANCE_MIN_MEASURED_VALUE_ID,
            LWM2M_ILLUMINANCE_MAX_MEASURED_VALUE_ID,
            LWM2M_ILLUMINANCE_MIN_RANGE_VALUE_ID,
            LWM2M_ILLUMINANCE_MAX_RANGE_VALUE_ID};
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
            case LWM2M_ILLUMINANCE_SENSOR_VALUE_ID:
            case LWM2M_ILLUMINANCE_UNITS_ID:
            case LWM2M_ILLUMINANCE_MIN_MEASURED_VALUE_ID:
            case LWM2M_ILLUMINANCE_MAX_MEASURED_VALUE_ID:
            case LWM2M_ILLUMINANCE_MIN_RANGE_VALUE_ID:
            case LWM2M_ILLUMINANCE_MAX_RANGE_VALUE_ID:
                break;
            default:
                result = COAP_404_NOT_FOUND;
            }
        }
    }

    return result;
}

static uint8_t prv_set_float_value(lwm2m_data_t *dataArray,
                                   float *data)
{
    uint8_t result;
    double value = 0;

    if (1 == lwm2m_data_decode_float(dataArray, (double *)&value))
    {
        if (value >= 0 && value <= 0xFFFFFFFF)
        {
            *data = (float)value;
            result = COAP_204_CHANGED;
        }
        else
        {
            result = COAP_406_NOT_ACCEPTABLE;
        }
    }
    else
    {
        result = COAP_400_BAD_REQUEST;
    }
    return result;
}

static uint8_t prv_illuminance_write(uint16_t instanceId,
                                     int numData,
                                     lwm2m_data_t *dataArray,
                                     lwm2m_object_t *objectP)
{
    illuminance_instance_t *targetP;
    int i;
    uint8_t result;

    targetP = (illuminance_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
    if (NULL == targetP)
    {
        return COAP_404_NOT_FOUND;
    }

    i = 0;
    do
    {
        switch (dataArray[i].id)
        {
        case LWM2M_ILLUMINANCE_SENSOR_VALUE_ID:
            result = prv_set_float_value(dataArray + i, &(targetP->sensorValue));
            break;

        case LWM2M_ILLUMINANCE_UNITS_ID:
            if (dataArray[i].type == LWM2M_TYPE_STRING && dataArray[i].value.asBuffer.length > 0)
            {
                strncpy(targetP->units, (char *)dataArray[i].value.asBuffer.buffer, dataArray[i].value.asBuffer.length);
                result = COAP_204_CHANGED;
            }
            else
            {
                result = COAP_400_BAD_REQUEST;
            }
            break;

        case LWM2M_ILLUMINANCE_MIN_MEASURED_VALUE_ID:
            result = prv_set_float_value(dataArray + i, &(targetP->minMeasuredValue));
            break;

        case LWM2M_ILLUMINANCE_MAX_MEASURED_VALUE_ID:
            result = prv_set_float_value(dataArray + i, &(targetP->maxMeasuredValue));
            break;

        case LWM2M_ILLUMINANCE_MIN_RANGE_VALUE_ID:
            result = prv_set_float_value(dataArray + i, &(targetP->minRangeValue));
            break;

        case LWM2M_ILLUMINANCE_MAX_RANGE_VALUE_ID:
            result = prv_set_float_value(dataArray + i, &(targetP->maxRangeValue));
            break;
        default:
            return COAP_404_NOT_FOUND;
        }
        i++;
    } while (i < numData && result == COAP_204_CHANGED);

    return result;
}

static uint8_t prv_illuminance_delete(uint16_t id,
                                      lwm2m_object_t *objectP)
{
    illuminance_instance_t *illuminanceInstance;

    objectP->instanceList = lwm2m_list_remove(objectP->instanceList, id, (lwm2m_list_t **)&illuminanceInstance);
    if (NULL == illuminanceInstance)
        return COAP_404_NOT_FOUND;

    lwm2m_free(illuminanceInstance);

    return COAP_202_DELETED;
}

static uint8_t prv_illuminance_create(uint16_t instanceId,
                                      int numData,
                                      lwm2m_data_t *dataArray,
                                      lwm2m_object_t *objectP)
{
    illuminance_instance_t *illuminanceInstance;
    uint8_t result;

    illuminanceInstance = (illuminance_instance_t *)lwm2m_malloc(sizeof(illuminance_instance_t));
    if (NULL == illuminanceInstance)
        return COAP_500_INTERNAL_SERVER_ERROR;
    memset(illuminanceInstance, 0, sizeof(illuminance_instance_t));

    illuminanceInstance->instanceId = instanceId;
    objectP->instanceList = LWM2M_LIST_ADD(objectP->instanceList, illuminanceInstance);

    result = prv_illuminance_write(instanceId, numData, dataArray, objectP);

    if (result != COAP_204_CHANGED)
    {
        (void)prv_illuminance_delete(instanceId, objectP);
    }
    else
    {
        result = COAP_201_CREATED;
    }

    return result;
}

static uint8_t prv_illuminance_execute(uint16_t instanceId,
                                       uint16_t resourceId,
                                       uint8_t *buffer,
                                       int length,
                                       lwm2m_object_t *objectP)

{
    illuminance_instance_t *targetP;

    targetP = (illuminance_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
    if (NULL == targetP)
        return COAP_404_NOT_FOUND;

    switch (resourceId)
    {
    case LWM2M_ILLUMINANCE_RESET_MIN_AND_MAX_MEASURED_VALUE_ID:
        return COAP_204_CHANGED;
    default:
        return COAP_405_METHOD_NOT_ALLOWED;
    }
}

static void clean_illuminance_object(lwm2m_object_t *object)
{
    while (object->instanceList != NULL)
    {
        illuminance_instance_t *illuminanceInstance = (illuminance_instance_t *)object->instanceList;
        object->instanceList = object->instanceList->next;
        lwm2m_free(illuminanceInstance);
    }
    lwm2m_free(object);
}

lwm2m_object_t *get_illuminance_object(float sensorValue,
                                       char *units, float minMeasuredValue, float maxMeasuredValue,
                                       float minRangeValue, float maxRangeValue)
{
    lwm2m_object_t *illuminanceObj;

    illuminanceObj = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));

    if (NULL != illuminanceObj)
    {
        illuminance_instance_t *illuminanceInstance;

        memset(illuminanceObj, 0, sizeof(lwm2m_object_t));

        illuminanceObj->objID = 3303;

        // Manually create an hardcoded server
        illuminanceInstance = (illuminance_instance_t *)lwm2m_malloc(sizeof(illuminance_instance_t));
        if (NULL == illuminanceInstance)
        {
            lwm2m_free(illuminanceObj);
            return NULL;
        }

        memset(illuminanceInstance, 0, sizeof(illuminance_instance_t));
        illuminanceInstance->instanceId = 0;
        illuminanceInstance->sensorValue = sensorValue;
        illuminanceInstance->minMeasuredValue = minMeasuredValue;
        illuminanceInstance->maxMeasuredValue = maxMeasuredValue;
        illuminanceInstance->minRangeValue = minRangeValue;
        illuminanceInstance->maxRangeValue = maxRangeValue;
        illuminanceInstance->units = lwm2m_strdup(units);

        illuminanceObj->instanceList = LWM2M_LIST_ADD(illuminanceObj->instanceList, illuminanceInstance);
        illuminanceObj->readFunc = prv_illuminance_read;
        illuminanceObj->discoverFunc = prv_illuminance_discover;
        illuminanceObj->createFunc = prv_illuminance_create;
        illuminanceObj->deleteFunc = prv_illuminance_delete;
        illuminanceObj->executeFunc = prv_illuminance_execute;
        illuminanceObj->cleanFunc = clean_illuminance_object;
    }

    return illuminanceObj;
}

