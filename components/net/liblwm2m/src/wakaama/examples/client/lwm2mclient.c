/*******************************************************************************
 *
 * Copyright (c) 2013, 2014 Intel Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * The Eclipse Distribution License is available at
 *    http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    David Navarro, Intel Corporation - initial API and implementation
 *    Benjamin Cabé - Please refer to git log
 *    Fabien Fleutot - Please refer to git log
 *    Simon Bernard - Please refer to git log
 *    Julien Vermillard - Please refer to git log
 *    Axel Lorente - Please refer to git log
 *    Toby Jaffey - Please refer to git log
 *    Bosch Software Innovations GmbH - Please refer to git log
 *    Pascal Rieux - Please refer to git log
 *    Christian Renz - Please refer to git log
 *    Ricky Liu - Please refer to git log
 *
 *******************************************************************************/

/*
 Copyright (c) 2013, 2014 Intel Corporation

 Redistribution and use in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:

     * Redistributions of source code must retain the above copyright notice,
       this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
     * Neither the name of Intel Corporation nor the names of its contributors
       may be used to endorse or promote products derived from this software
       without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 THE POSSIBILITY OF SUCH DAMAGE.

 David Navarro <david.navarro@intel.com>
 Bosch Software Innovations GmbH - Please refer to git log

*/

#include "lwm2mclient.h"
#include "liblwm2m.h"
#include "internals.h"

#include "commandline.h"
#if(defined WITH_TINYDTLS) 
#include "dtlsconnection.h"
#elif (defined WITH_MBEDDTLS)
#include "mbedconnection.h"
#else
#include "connection.h"
#endif


#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#include "lwm2m_api.h"

#include "osi_log.h"

#define MAX_PACKET_SIZE 1024
#define DEFAULT_SERVER_IPV6 "[::1]"
#define DEFAULT_SERVER_IPV4 "127.0.0.1"

int g_reboot = 0;

#define OBJ_COUNT 10

// only backup security and server objects
# define BACKUP_OBJECT_COUNT 2
lwm2m_object_t * backupObjectArray[BACKUP_OBJECT_COUNT];

typedef struct
{
    lwm2m_object_t * securityObjP;
    lwm2m_object_t * serverObject;
    int sock;
#if (defined WITH_TINYDTLS) || (defined WITH_MBEDDTLS)
    dtls_connection_t * connList;  
#else
    connection_t * connList;
#endif
    lwm2m_context_t * lwm2mH;
    int addressFamily;
} client_data_t;

#if (defined WITH_TINYDTLS) || (defined WITH_MBEDDTLS)

extern mbedtls_ssl_context * g_sslContext;
#endif

static int prv_quit(char * buffer,
                     void * user_data)
{
    fprintf(stderr, "lwm2m prv_quit !\n");
    lwm2m_context_t * lwm2mH = (lwm2m_context_t *)user_data;
    lwm2mH->quit = 1;
    return 1;
}

int handle_value_changed(lwm2m_context_t * lwm2mH,
                          lwm2m_uri_t * uri,
                          int type,
                          const char * value,
                          size_t valueLength)
{
    lwm2m_object_t * object = (lwm2m_object_t *)LWM2M_LIST_FIND(lwm2mH->objectList, uri->objectId);

    if (NULL != object)
    {
        if (object->writeFunc != NULL)
        {
            lwm2m_data_t * dataP;
            int result = 0;;

            dataP = lwm2m_data_new(1);
            if (dataP == NULL)
            {
                fprintf(stderr, "Internal allocation failure !\n");
                return -1;
            }
            dataP->id = uri->resourceId;

            ipso_res_t *ipso_res = NULL;
            lwm2m_get_ipso_obj(uri->objectId,uri->resourceId,&ipso_res,lwm2mH->ref);
            if (ipso_res == NULL)
            {
                lwm2m_data_free(1, dataP);
                return -1;
            }

            if (lwm2m_is_dynamic_ipso(lwm2mH->ref))
            {
                dataP->type = ipso_res->value_type = type;
            }
            else
            {
                dataP->type = ipso_res->value_type;
            }
            if ((dataP->type+3) == LWM2M_TYPE_STRING)
                lwm2m_data_encode_nstring(value, valueLength, dataP);
            else if ((dataP->type+3) == LWM2M_TYPE_OPAQUE)
                lwm2m_data_encode_opaque((uint8_t *)value, valueLength, dataP);
            else if ((dataP->type+3) == LWM2M_TYPE_INTEGER)
                lwm2m_data_encode_int(atoi(value), dataP);
            else if ((dataP->type+3) == LWM2M_TYPE_FLOAT) {
                double vd;
                utils_textToFloat((uint8_t *)value, valueLength, &vd);
                lwm2m_data_encode_float(vd, dataP);
            }
            else if ((dataP->type+3) == LWM2M_TYPE_BOOLEAN)
                lwm2m_data_encode_bool(atoi(value), dataP);
            
            if (uri->objectId <= LWM2M_CONN_STATS_OBJECT_ID)
                result = object->writeFunc(uri->instanceId, 1, dataP, object);
            else if (object->notifyFunc != NULL)
                result = object->notifyFunc(uri->instanceId, 1, dataP, object);
            
            if (COAP_405_METHOD_NOT_ALLOWED == result)
            {
                switch (uri->objectId)
                {
                case LWM2M_DEVICE_OBJECT_ID:
                    result = device_change(dataP, object);
                    break;
                default:
                    break;
                }
            }

            if (COAP_204_CHANGED != result)
            {
                fprintf(stderr, "Failed to change value!\n");
                result = -1;
            }
            else
            {
                fprintf(stderr, "value changed!\n");
                lwm2m_resource_value_changed(lwm2mH, uri);
                result = 0;
            }
            lwm2m_data_free(1, dataP);
            return result;
        }
        else
        {
            fprintf(stderr, "write not supported for specified resource!\n");
        }
    }
    else
    {
        fprintf(stderr, "Object not found !\n");
    }
    return -1;
}

#if(defined WITH_TINYDTLS) || (defined WITH_MBEDDTLS)
void * lwm2m_connect_server(uint16_t secObjInstID,
                            void * userData)
{
  client_data_t * dataP;
  lwm2m_list_t * instance;
  dtls_connection_t * newConnP = NULL;
  dataP = (client_data_t *)userData;
  lwm2m_object_t  * securityObj = dataP->securityObjP;
  
  instance = LWM2M_LIST_FIND(dataP->securityObjP->instanceList, secObjInstID);
  if (instance == NULL) return NULL;
  
  
  newConnP = connection_create(dataP->connList, dataP->sock, securityObj, instance->id, dataP->lwm2mH, dataP->addressFamily);
  if (newConnP == NULL)
  {
      fprintf(stderr, "Connection creation failed.\n");
      return NULL;
  }
  
  dataP->connList = newConnP;
  return (void *)newConnP;
}
#else
void * lwm2m_connect_server(uint16_t secObjInstID,
                            void * userData)
{
    client_data_t * dataP;
    char * uri;
    char * host;
    char * port;
    connection_t * newConnP = NULL;

    dataP = (client_data_t *)userData;

    uri = get_server_uri(dataP->securityObjP, secObjInstID);

    if (uri == NULL) return NULL;

    // parse uri in the form "coaps://[host]:[port]"
    if (0==strncmp(uri, "coaps://", strlen("coaps://"))) {
        host = uri+strlen("coaps://");
    }
    else if (0==strncmp(uri, "coap://",  strlen("coap://"))) {
        host = uri+strlen("coap://");
    }
    else {
        goto exit;
    }
    port = strrchr(host, ':');
    if (port == NULL) goto exit;
    // remove brackets
    if (host[0] == '[')
    {
        host++;
        if (*(port - 1) == ']')
        {
            *(port - 1) = 0;
        }
        else goto exit;
    }
    // split strings
    *port = 0;
    port++;

    fprintf(stderr, "Opening connection to server at %s:%s\r\n", host, port);
    newConnP = connection_create(dataP->connList, dataP->sock, host, port, dataP->addressFamily);
    if (newConnP == NULL) {
        fprintf(stderr, "Connection creation failed.\r\n");
    }
    else {
        newConnP->lwm2mH = dataP->lwm2mH;
        dataP->connList = newConnP;
    }

exit:
    lwm2m_free(uri);
    return (void *)newConnP;
}
#endif

lwm2m_ret_t QuitHandshake(){
#if(defined WITH_TINYDTLS) || (defined WITH_MBEDDTLS)

    if(g_sslContext == NULL){
        fprintf(stderr, "QuitHandshake g_sslContext == NULL\r\n");
        return 0;
    }
    g_sslContext->isquit = 1; 
#endif
    return 0;
}

void lwm2m_close_connection(void * sessionH,
                            void * userData)
{
    client_data_t * app_data;
#if(defined WITH_TINYDTLS) || (defined WITH_MBEDDTLS)
    dtls_connection_t * targetP;
#else
    connection_t * targetP;
#endif

    app_data = (client_data_t *)userData;
#if(defined WITH_TINYDTLS) || (defined WITH_MBEDDTLS)
    targetP = (dtls_connection_t *)sessionH;
#else
    targetP = (connection_t *)sessionH;
#endif

    OSI_LOGI(0x100076ae, "M2M# lwm2m_close_connection targetP:%p", targetP);
    OSI_LOGI(0x100076af, "M2M# lwm2m_close_connection app_data->connList:%p", app_data->connList);

    if (targetP == app_data->connList)
    {
        app_data->connList = targetP->next;
        //lwm2m_free(targetP);
        targetP->next = NULL;
        connection_free(targetP);
    }
    else
    {
#if(defined WITH_TINYDTLS) || (defined WITH_MBEDDTLS)
        dtls_connection_t * parentP;
#else
        connection_t * parentP;
#endif

        parentP = app_data->connList;
        while (parentP != NULL && parentP->next != targetP)
        {
            OSI_LOGI(0x100076b0, "M2M# lwm2m_close_connection parentP:%p", parentP);
            parentP = parentP->next;
        }
        if (parentP != NULL)
        {
            parentP->next = targetP->next;
            targetP->next = NULL;
            //lwm2m_free(targetP);
            connection_free(targetP);
        }
    }
}

static int prv_output_servers(char * buffer,
                               void * user_data)
{
    lwm2m_context_t * lwm2mH = (lwm2m_context_t *) user_data;
    lwm2m_server_t * targetP;

    targetP = lwm2mH->bootstrapServerList;

    if (lwm2mH->bootstrapServerList == NULL)
    {
        fprintf(stdout, "No Bootstrap Server.\r\n");
    }
    else
    {
        fprintf(stdout, "Bootstrap Servers:\r\n");
        for (targetP = lwm2mH->bootstrapServerList ; targetP != NULL ; targetP = targetP->next)
        {
            fprintf(stdout, " - Security Object ID %d", targetP->secObjInstID);
            fprintf(stdout, "\tHold Off Time: %lu s", (unsigned long)targetP->lifetime);
            fprintf(stdout, "\tstatus: ");
            switch(targetP->status)
            {
            case STATE_DEREGISTERED:
                fprintf(stdout, "DEREGISTERED\r\n");
                break;
            case STATE_BS_HOLD_OFF:
                fprintf(stdout, "CLIENT HOLD OFF\r\n");
                break;
            case STATE_BS_INITIATED:
                fprintf(stdout, "BOOTSTRAP INITIATED\r\n");
                break;
            case STATE_BS_PENDING:
                fprintf(stdout, "BOOTSTRAP PENDING\r\n");
                break;
            case STATE_BS_FINISHED:
                fprintf(stdout, "BOOTSTRAP FINISHED\r\n");
                break;
            case STATE_BS_FAILED:
                fprintf(stdout, "BOOTSTRAP FAILED\r\n");
                break;
            default:
                fprintf(stdout, "INVALID (%d)\r\n", (int)targetP->status);
            }
        }
    }

    if (lwm2mH->serverList == NULL)
    {
        fprintf(stdout, "No LWM2M Server.\r\n");
    }
    else
    {
        fprintf(stdout, "LWM2M Servers:\r\n");
        for (targetP = lwm2mH->serverList ; targetP != NULL ; targetP = targetP->next)
        {
            fprintf(stdout, " - Server ID %d", targetP->shortID);
            fprintf(stdout, "\tstatus: ");
            switch(targetP->status)
            {
            case STATE_DEREGISTERED:
                fprintf(stdout, "DEREGISTERED\r\n");
                break;
            case STATE_REG_PENDING:
                fprintf(stdout, "REGISTRATION PENDING\r\n");
                break;
            case STATE_REGISTERED:
                fprintf(stdout, "REGISTERED\tlocation: \"%s\"\tLifetime: %lus\r\n", targetP->location, (unsigned long)targetP->lifetime);
                break;
            case STATE_REG_UPDATE_PENDING:
                fprintf(stdout, "REGISTRATION UPDATE PENDING\r\n");
                break;
            case STATE_DEREG_PENDING:
                fprintf(stdout, "DEREGISTRATION PENDING\r\n");
                break;
            case STATE_REG_FAILED:
                fprintf(stdout, "REGISTRATION FAILED\r\n");
                break;
            default:
                fprintf(stdout, "INVALID (%d)\r\n", (int)targetP->status);
            }
        }
    }
    return 0;
}

static int prv_change(char * buffer,
                       void * user_data)
{
    lwm2m_context_t * lwm2mH = (lwm2m_context_t *) user_data;
    lwm2m_uri_t uri;
    //char * end = NULL;
    int result;
    fprintf(stderr, "prv_change buffer=%s",buffer);

    int value[3];
    lwm2m_parse_buffer(buffer,value,3,NULL);
    uint8_t *urlstring = (uint8_t *)(value[0]); buffer += strlen((char *)value[0])+1;
    lwm2m_tpye_t type = (lwm2m_tpye_t)atoi((const char *)value[1]);buffer += strlen((char *)value[1])+1;
    uint32_t value_length = (uint32_t)atoi((const char *)value[2]);buffer += strlen((char *)value[2])+1;

    result = lwm2m_stringToUri((char *)urlstring, strlen((char *)urlstring), &uri);
    if (result == 0) goto syntax_error;

    if (value_length == 0)
    {
        fprintf(stderr, "report change!\n");
        lwm2m_resource_value_changed(lwm2mH, &uri);
    }
    else
    {
        if (handle_value_changed(lwm2mH, &uri, type, buffer, value_length)<0)
            goto syntax_error;
    }
    return 0;
syntax_error:
    fprintf(stdout, "Syntax error !\n");
    return -1;
}

static int prv_object_list(char * buffer,
                            void * user_data)
{
    lwm2m_context_t * lwm2mH = (lwm2m_context_t *)user_data;
    lwm2m_object_t * objectP;

    for (objectP = lwm2mH->objectList; objectP != NULL; objectP = objectP->next)
    {
        if (objectP->instanceList == NULL)
        {
            fprintf(stdout, "/%d ", objectP->objID);
        }
        else
        {
            lwm2m_list_t * instanceP;

            for (instanceP = objectP->instanceList; instanceP != NULL ; instanceP = instanceP->next)
            {
                fprintf(stdout, "/%d/%d  ", objectP->objID, instanceP->id);
            }
        }
        fprintf(stdout, "\r\n");
    }
    return 0;
}

static int prv_instance_dump(lwm2m_object_t * objectP,
                              uint16_t id)
{
    int numData;
    lwm2m_data_t * dataArray;
    uint16_t res;

    numData = 0;
    res = objectP->readFunc(id, &numData, &dataArray, objectP);
    if (res != COAP_205_CONTENT)
    {
        printf("Error ");
        print_status(stdout, res);
        printf("\r\n");
        return -1;
    }

    dump_tlv(stdout, numData, dataArray, 0);
    return 0;
}


static int prv_object_dump(char * buffer,
                            void * user_data)
{
    lwm2m_context_t * lwm2mH = (lwm2m_context_t *) user_data;
    lwm2m_uri_t uri;
    char * end = NULL;
    int result;
    lwm2m_object_t * objectP;

    end = get_end_of_arg(buffer);
    if (end[0] == 0) goto syntax_error;

    result = lwm2m_stringToUri(buffer, end - buffer, &uri);
    if (result == 0) goto syntax_error;
    if (uri.flag & LWM2M_URI_FLAG_RESOURCE_ID) goto syntax_error;

    objectP = (lwm2m_object_t *)LWM2M_LIST_FIND(lwm2mH->objectList, uri.objectId);

    if (objectP == NULL)
    {
        fprintf(stdout, "Object not found.\n");
        return -1;
    }

    if (uri.flag & LWM2M_URI_FLAG_INSTANCE_ID)
    {
        prv_instance_dump(objectP, uri.instanceId);
    }
    else
    {
        lwm2m_list_t * instanceP;

        for (instanceP = objectP->instanceList; instanceP != NULL ; instanceP = instanceP->next)
        {
            fprintf(stdout, "Instance %d:\r\n", instanceP->id);
            prv_instance_dump(objectP, instanceP->id);
            fprintf(stdout, "\r\n");
        }
    }
    return 0;

syntax_error:
    fprintf(stdout, "Syntax error !\n");
    return -1;
}

static int prv_set_flag(char * buffer,
                            void * user_data)
{
    lwm2m_context_t * lwm2mH = (lwm2m_context_t *)user_data;
    if (buffer[0] == 0) goto syntax_error;

    uint8_t flag = (uint8_t) atoi(buffer);
    lwm2mH->sendflag = flag;
    return 1;
syntax_error:
    fprintf(stdout, "Syntax error !\n");
    return 1;
}

void update_lifetime(lwm2m_context_t * lwm2mH, uint16_t shortServerID, uint32_t lifetime);

static int prv_update(char * buffer,
                       void * user_data)
{
    lwm2m_context_t * lwm2mH = (lwm2m_context_t *)user_data;
    if (buffer[0] == 0) goto syntax_error;
    uint32_t value[2];    
    lwm2m_parse_buffer(buffer,value,2,NULL);
    uint32_t lifetime = atoi((const char *)value[0]);
    uint8_t withObjects = atoi((const char *)value[1]);

    uint16_t shortServerID = 0;
    lwm2m_server_t *targetP;
    targetP = lwm2mH->serverList;
    if (targetP != NULL)
    {
        shortServerID = targetP->shortID;
    }
 
    update_lifetime(lwm2mH, shortServerID, lifetime);
    int res = lwm2m_update_registration(lwm2mH, shortServerID, withObjects);
    if (res != 0)
    {
        fprintf(stdout, "Registration update error: ");
        print_status(stdout, res);
        fprintf(stdout, "\r\n");
        return -1;
    }    
    return 0;

syntax_error:
    fprintf(stdout, "Syntax error !\n");
    return -1;
}
extern int32_t getBatteryLevel();
static int s_fota_num_retry = 0;
static int s_last_fota_num = 0;
static long s_fota_download_tv;
extern lwm2m_fota_state_t g_fota_state;

static long s_fota_download_start_time;
static int s_long_time_download_index = 0;

static int prv_download_fota(char * buffer,
                       void * user_data)
{
    lwm2m_context_t * lwm2mH = (lwm2m_context_t *)user_data;
    int value[1];
    lwm2m_parse_buffer(buffer,value,1,NULL);
    uint16_t mid = lwm2mH->nextMID++;
    uint8_t *uri = (uint8_t *)value[0];
    coap_packet_t message;
    coap_init_message(&message, COAP_TYPE_CON, COAP_GET, mid);
    coap_set_header_uri_path(&message, (const char *)uri);
    // generate a token
    uint8_t temp_token[COAP_TOKEN_LEN];
    time_t tv_sec = lwm2m_gettime();

    // initialize first 6 bytes, leave the last 2 random
    temp_token[0] = mid;
    temp_token[1] = mid >> 8;
    temp_token[2] = tv_sec;
    temp_token[3] = tv_sec >> 8;
    temp_token[4] = tv_sec >> 16;
    temp_token[5] = tv_sec >> 24;
    // use just the provided amount of bytes
    coap_set_header_token(&message, temp_token, COAP_TOKEN_LEN);
    size_t allocLen;

    allocLen = coap_serialize_get_size(&message);
    if (allocLen == 0) return -1;

    if(lwm2mH->fota_upgrade_observed == NULL || lwm2mH->fota_upgrade_observed->watcherList == NULL)
        return 2;
    lwm2m_watcher_t * watcherP = lwm2mH->fota_upgrade_observed->watcherList;
    if (watcherP == NULL || watcherP->active != true)
        return 2;

    LOG_ARG("prv_download_fota  block2Num %d long_time_index %d",lwm2mH->fota_context.block2Num,s_long_time_download_index);
    if(s_last_fota_num != lwm2mH->fota_context.block2Num)
    {
        s_fota_num_retry = 0;
    }else
    {
        s_fota_num_retry++;
    }
    s_last_fota_num = lwm2mH->fota_context.block2Num;
    s_fota_download_tv = lwm2m_gettime();
    if(s_long_time_download_index %20 < 15 || lwm2mH->fota_context.block2Num == 0)
    {
        memcpy(lwm2mH->fota_context.token ,temp_token, COAP_TOKEN_LEN);
        if(lwm2mH->fota_context.uri == NULL)
        {
            lwm2mH->fota_context.uri = lwm2m_malloc(strlen((const char*)uri)+1);
            strcpy((char*)(lwm2mH->fota_context.uri), (const char*)uri);
        }
        coap_set_header_content_type(&message,LWM2M_CONTENT_OPAQUE);
        coap_set_header_block2(&message, lwm2mH->fota_context.block2Num, 0, REST_MAX_CHUNK_SIZE);
        lwm2mH->sendflag = 0;
        if(lwm2mH->fota_context.block2Num == 0)
        {
            s_fota_download_start_time = lwm2m_gettime();
        }

        LOG_ARG("message_send COAP_GET lwm2mH->fota_context.block2Num %d",lwm2mH->fota_context.block2Num);
        message_send(lwm2mH, &message, watcherP->server->sessionH);
    }
    return 1;
}

static int prv_notify_fota(char * buffer,
                       void * user_data)
{
    lwm2m_context_t * lwm2mH = (lwm2m_context_t *)user_data;
    int value[2];
    lwm2m_parse_buffer(buffer, value, 2, NULL);
    uint32_t fota_state = (uint32_t)atoi((const char *)value[0]);
    uint32_t fota_resulte = (uint32_t)atoi((const char *)value[1]);
    coap_packet_t message;
    LOG_ARG("prv_notify_fota %d %d",fota_state, fota_resulte);

    if(fota_state == LWM2M_FOTA_STATE_IDLE)
    {
        switch (fota_resulte)
        {
            case LWM2M_FOTA_RESULT_CONNECTION_LOST:
            {
                lwm2mPostEvent(lwm2mH->ref, FOTA_DOWNLOADING_IND, 0, lwm2mH->fota_context.block2bufferSize, 0);
                lwm2mPostEvent(lwm2mH->ref, FOTA_DOWNLOAD_FAILED_IND, 0, 0, 0);
            }
            break;
            case LWM2M_FOTA_RESULT_INTEGRITY_FAILUER:
            {
                lwm2mPostEvent(lwm2mH->ref, FOTA_DOWNLOADING_IND, 0, lwm2mH->fota_context.block2bufferSize, 0);
                lwm2mPostEvent(lwm2mH->ref, FOTA_DOWNLOAD_SUCCESS_IND, 0, 0, 0);
                lwm2mPostEvent(lwm2mH->ref, FOTA_PACKAGE_CHECK_IND, 0, -1, 0);
            }
            break;
            case LWM2M_FOTA_RESULT_SUCCESS:
            {
                lwm2mPostEvent(lwm2mH->ref, FOTA_UPGRADE_OK_IND, 0, 0, 0);
            }
            break;
            default:
            break;
        }
    }
    else if(fota_state == LWM2M_FOTA_STATE_DOWNLOADING)
    {
        lwm2mPostEvent(lwm2mH->ref, FOTA_DOWNLOADING_IND, 0, lwm2mH->fota_context.block2bufferSize, 0);
    }
    else if(fota_state == LWM2M_FOTA_STATE_DOWNLOADED)
    {
        lwm2mPostEvent(lwm2mH->ref, FOTA_DOWNLOADING_IND, 0, lwm2mH->fota_context.block2bufferSize, 0);
        lwm2mPostEvent(lwm2mH->ref, FOTA_DOWNLOAD_SUCCESS_IND, 0, 0, 0);
        lwm2mPostEvent(lwm2mH->ref, FOTA_PACKAGE_CHECK_IND, 0, 1, 0);
    }

    lwm2m_observed_t * targetP = NULL;
    lwm2m_watcher_t * watcherP = NULL;
    for (targetP = lwm2mH->observedList ; targetP != NULL ; targetP = targetP->next)
    {
        if(targetP->uri.objectId == LWM2M_FIRMWARE_UPDATE_OBJECT_ID && targetP->uri.instanceId == 0 && targetP->uri.resourceId == 3)
        {
             watcherP = targetP->watcherList;
             break;
        }
    }
    LOG_ARG("prv_notify_fota watcherP %p",watcherP);
    if(watcherP == NULL) return -1;
    char send_buffer[10];
    snprintf(send_buffer, 9,"%ld", fota_state);

    coap_init_message(&message, COAP_TYPE_NON, COAP_205_CONTENT, 0);
    coap_set_header_content_type(&message, watcherP->format);
    coap_set_payload(&message, send_buffer, strlen(send_buffer));

    watcherP->lastTime = lwm2m_gettime();
    watcherP->lastMid = lwm2mH->nextMID++;
    message.mid = watcherP->lastMid;

    coap_set_header_token(&message, watcherP->token, watcherP->tokenLen);
    coap_set_header_observe(&message, watcherP->counter++);
    (void)message_send(lwm2mH, &message, watcherP->server->sessionH);
    OSI_LOGI(0x100076b1, "prv_notify_fota");

    if(fota_state == LWM2M_FOTA_STATE_UPDATING)
    {
        osiThreadSleep(1500);
        notify_fota_state(LWM2M_FOTA_STATE_IDLE, LWM2M_FOTA_RESULT_SUCCESS, lwm2mH->ref);
    }
    return 1;
}

static void update_battery_level(lwm2m_context_t * context)
{
    static time_t next_change_time = 0;
    time_t tv_sec;

    tv_sec = lwm2m_gettime();
    if (tv_sec < 0) return;

    if (next_change_time < tv_sec)
    {
        char value[15];
        int valueLength;
        lwm2m_uri_t uri;
        int level = (int)getBatteryLevel();//rand() % 100;

        if (0 > level) level = -level;
        if (lwm2m_stringToUri("/3/0/9", 6, &uri))
        {
            valueLength = sprintf(value, "%d", level);
            fprintf(stderr, "New Battery Level: %d\n", level);
            handle_value_changed(context, &uri, LWM2M_TYPE_INTEGER - 3, value, valueLength);
        }
        level = rand() % 20;
        if (0 > level) level = -level;
        next_change_time = tv_sec + level + 10;
    }
}

bool add_mult_objinsts(lwm2m_context_t *contextP, uint16_t objid, uint32_t instCount, uint8_t *bitmap);

static int prv_add(char * buffer,
                    void * user_data)
{
    lwm2m_context_t * lwm2mH = (lwm2m_context_t *)user_data;
    int value[4];    
    lwm2m_parse_buffer(buffer,value,4,NULL);
    uint16_t objid = (uint16_t)atoi((const char *)value[0]);
    uint32_t instCount = (uint32_t)atoi((const char *)value[1]);
    uint8_t *bitmap = (uint8_t *)value[2];
    uint16_t resCount = (uint16_t)atoi((const char *)value[3]);
    fprintf(stdout, "objid=%d,instCount=%d,bitmap=%s,resCount=%d\r\n",objid,(unsigned int)instCount,bitmap,resCount);
    
    if (!lwm2m_isObjSupport(objid, lwm2mH->ref))
        lwm2m_registNewObj(objid, resCount, lwm2mH->ref);

    if (!add_mult_objinsts(lwm2mH,objid,instCount,bitmap))
    {
        fprintf(stdout, "Creating object %d failed.\r\n" ,objid);
        return -1;
    }
    return 0;
}

static int prv_remove(char * buffer,
                       void * user_data)
{
    lwm2m_context_t * lwm2mH = (lwm2m_context_t *)user_data;
    int res;
    int value[1];
    lwm2m_parse_buffer(buffer,value,1,NULL);
    uint16_t objid = (uint16_t)atoi((const char *)value[0]);

    res = lwm2m_remove_object(lwm2mH, objid);
    if (res != 0)
    {
        fprintf(stdout, "Removing object %d failed: ", objid);
        print_status(stdout, res);
        fprintf(stdout, "\r\n");
        return -1;
    }
    else
    {
        fprintf(stdout, "Object %d removed.\r\n", objid);
    }
    return 0;
}

#ifdef LWM2M_BOOTSTRAP

static int prv_initiate_bootstrap(char * buffer,
                                   void * user_data)
{
    lwm2m_context_t * lwm2mH = (lwm2m_context_t *)user_data;
    lwm2m_server_t * targetP;

    // HACK !!!
    lwm2mH->state = STATE_BOOTSTRAP_REQUIRED;
    targetP = lwm2mH->bootstrapServerList;
    while (targetP != NULL)
    {
        targetP->lifetime = 0;
        targetP = targetP->next;
    }
    return 0;
}

static int prv_display_objects(char * buffer,
                                void * user_data)
{
    lwm2m_context_t * lwm2mH = (lwm2m_context_t *)user_data;
    lwm2m_object_t * object;

    for (object = lwm2mH->objectList; object != NULL; object = object->next){
        if (NULL != object) {
            switch (object->objID)
            {
            case LWM2M_SECURITY_OBJECT_ID:
                display_security_object(object);
                break;
            case LWM2M_SERVER_OBJECT_ID:
                display_server_object(object);
                break;
            case LWM2M_ACL_OBJECT_ID:
                break;
            case LWM2M_DEVICE_OBJECT_ID:
                display_device_object(object);
                break;
            case LWM2M_CONN_MONITOR_OBJECT_ID:
                break;
            case LWM2M_FIRMWARE_UPDATE_OBJECT_ID:
                display_firmware_object(object);
                break;
            case LWM2M_LOCATION_OBJECT_ID:
                display_location_object(object);
                break;
            case LWM2M_CONN_STATS_OBJECT_ID:
                break;
            case TEST_OBJECT_ID:
                display_test_object(object);
                break;
            }
        }
    }
    return 0;
}

static int prv_display_backup(char * buffer,
        void * user_data)
{
   int i;
   for (i = 0 ; i < BACKUP_OBJECT_COUNT ; i++) {
       lwm2m_object_t * object = backupObjectArray[i];
       if (NULL != object) {
           switch (object->objID)
           {
           case LWM2M_SECURITY_OBJECT_ID:
               display_security_object(object);
               break;
           case LWM2M_SERVER_OBJECT_ID:
               display_server_object(object);
               break;
           default:
               break;
           }
       }
   }
   return 0;
}

static void prv_backup_objects(lwm2m_context_t * context)
{
    uint16_t i;
    lwm2m_object_t * objectSrc = NULL;

    for (i = 0; i < BACKUP_OBJECT_COUNT; i++) {
        if (NULL != backupObjectArray[i]) {
            switch (backupObjectArray[i]->objID)
            {
            case LWM2M_SECURITY_OBJECT_ID:
            case LWM2M_SERVER_OBJECT_ID:
                backupObjectArray[i]->cleanFunc(backupObjectArray[i]);
                lwm2m_free(backupObjectArray[i]);
                break;
            default:
                break;
            }
        }
        backupObjectArray[i] = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));
        memset(backupObjectArray[i], 0, sizeof(lwm2m_object_t));
    }

    /*
     * Backup content of objects 0 (security) and 1 (server)
     */
    objectSrc = (lwm2m_object_t *)LWM2M_LIST_FIND(context->objectList, LWM2M_SECURITY_OBJECT_ID);
    if(objectSrc != NULL)
        copy_security_object(backupObjectArray[0], objectSrc);
    objectSrc = (lwm2m_object_t *)LWM2M_LIST_FIND(context->objectList, LWM2M_SERVER_OBJECT_ID);
    if(objectSrc != NULL)
        copy_server_object(backupObjectArray[1], objectSrc);
}
#if 0
static void prv_restore_objects(lwm2m_context_t * context)
{
    lwm2m_object_t * targetP;

    /*
     * Restore content  of objects 0 (security) and 1 (server)
     */
    targetP = (lwm2m_object_t *)LWM2M_LIST_FIND(context->objectList, LWM2M_SECURITY_OBJECT_ID);
    // first delete internal content
    targetP->cleanFunc(targetP);
    // then restore previous object
    copy_security_object(targetP, backupObjectArray[0]);

    targetP = (lwm2m_object_t *)LWM2M_LIST_FIND(context->objectList, LWM2M_SERVER_OBJECT_ID);
    // first delete internal content
    targetP->cleanFunc(targetP);
    // then restore previous object
    copy_server_object(targetP, backupObjectArray[1]);

    // restart the old servers
    fprintf(stdout, "[BOOTSTRAP] ObjectList restored\r\n");
}
#endif
static void update_bootstrap_info(lwm2m_client_state_t * previousBootstrapState,
        lwm2m_context_t * context)
{
    if (*previousBootstrapState != context->state)
    {
        *previousBootstrapState = context->state;
        switch(context->state)
        {
            case STATE_BOOTSTRAPPING:
#ifdef WITH_LOGS
                fprintf(stdout, "[BOOTSTRAP] backup security and server objects\r\n");
#endif
                prv_backup_objects(context);
                break;
            default:
                break;
        }
    }
}

static void close_backup_object()
{
    int i;
    for (i = 0; i < BACKUP_OBJECT_COUNT; i++) {
        if (NULL != backupObjectArray[i]) {
            switch (backupObjectArray[i]->objID)
            {
            case LWM2M_SECURITY_OBJECT_ID:
            case LWM2M_SERVER_OBJECT_ID:
                backupObjectArray[i]->cleanFunc(backupObjectArray[i]);
                lwm2m_free(backupObjectArray[i]);
                memset(backupObjectArray[i], 0, sizeof(lwm2m_object_t));
                backupObjectArray[i] = NULL;
                break;
            default:
                break;
            }
        }
    }
}
#endif

void print_usage(void)
{
    fprintf(stdout, "Usage: lwm2mclient [OPTION]\r\n");
    fprintf(stdout, "Launch a LWM2M client.\r\n");
    fprintf(stdout, "Options:\r\n");
    fprintf(stdout, "  -n NAME\tSet the endpoint name of the Client. Default: testlwm2mclient\r\n");
    fprintf(stdout, "  -l PORT\tSet the local UDP port of the Client. Default: 56830\r\n");
    fprintf(stdout, "  -h HOST\tSet the hostname of the LWM2M Server to connect to. Default: localhost\r\n");
    fprintf(stdout, "  -p PORT\tSet the port of the LWM2M Server to connect to. Default: "LWM2M_STANDARD_PORT_STR"\r\n");
    fprintf(stdout, "  -4\t\tUse IPv4 connection. Default: IPv6 connection\r\n");
    fprintf(stdout, "  -t TIME\tSet the lifetime of the Client. Default: 300\r\n");
    fprintf(stdout, "  -b\t\tBootstrap requested.\r\n");
    fprintf(stdout, "  -c\t\tChange battery level over time.\r\n");
#if(defined WITH_TINYDTLS) || (defined WITH_MBEDDTLS)  
    fprintf(stdout, "  -i STRING\tSet the device management or bootstrap server PSK identity. If not set use none secure mode\r\n");
    fprintf(stdout, "  -s HEXSTRING\tSet the device management or bootstrap server Pre-Shared-Key. If not set use none secure mode\r\n");    
#endif
    fprintf(stdout, "\r\n");
}

void clean_all_objects(lwm2m_context_t * context);

int lwm2mclient_main(int argc, char *argv[])
{
    int ref = -1;
    int res = -1;
    uint8_t isquit = 0;
    int ipc_socket = -1;
    client_data_t data;
    int result;
    lwm2m_context_t * lwm2mH = NULL;
    const char * localPort = "56830";
    const char * server = NULL;
    const char * serverPort = LWM2M_STANDARD_PORT_STR;
    char * name = "testlwm2mclient";
    int lifetime = 300;
    int timeout = 60;
    int batterylevelchanging = 0;
    time_t reboot_time = 0;
    int opt;
    bool bootstrapRequested = false;
    bool serverPortChanged = false;

    struct addrinfo hints;
    struct addrinfo *servinfo = NULL;
    // for DM standard begin
    char *dmKey = NULL;
    char *dmAppKey = NULL;
    char *dmVersion = NULL;
    // end

    uint8_t sendFlag = 0;

#ifdef LWM2M_BOOTSTRAP
    lwm2m_client_state_t previousState = STATE_INITIAL;
#endif

    char * pskId = NULL;
    char * psk = NULL;
    uint16_t pskLen = 0;
    char * pskBuffer = NULL;
    char serverUri[50];
    int serverId = 1;
    lwm2m_object_t * objArray[7] = {NULL};

    /*
     * The function start by setting up the command line interface (which may or not be useful depending on your project)
     *
     * This is an array of commands describes as { name, description, long description, callback, userdata }.
     * The firsts tree are easy to understand, the callback is the function that will be called when this command is typed
     * and in the last one will be stored the lwm2m context (allowing access to the server settings and the objects).
     */
    command_desc_t commands[] =
    {
            {"list", "List known servers.", NULL, prv_output_servers, NULL},
            {"change", "Change the value of resource.", " change URI [DATA]\r\n"
                                                        "   URI: uri of the resource such as /3/0, /3/0/2\r\n"
                                                        "   DATA: (optional) new value\r\n", prv_change, NULL},
            {"update", "Trigger a registration update", " update SERVER\r\n"
                                                        "   SERVER: short server id such as 123\r\n", prv_update, NULL},
#ifdef LWM2M_BOOTSTRAP
            {"bootstrap", "Initiate a DI bootstrap process", NULL, prv_initiate_bootstrap, NULL},
            {"dispb", "Display current backup of objects/instances/resources\r\n"
                    "\t(only security and server objects are backupped)", NULL, prv_display_backup, NULL},
#endif
            {"ls", "List Objects and Instances", NULL, prv_object_list, NULL},
#ifdef LWM2M_BOOTSTRAP
            {"disp", "Display current objects/instances/resources", NULL, prv_display_objects, NULL},
#endif
            {"dump", "Dump an Object", "dump URI"
                                       "URI: uri of the Object or Instance such as /3/0, /1\r\n", prv_object_dump, NULL},
            {"add", "Add support of object 31024", NULL, prv_add, NULL},
            {"rm", "Remove support of object 31024", NULL, prv_remove, NULL},
            {"quit", "Quit the client gracefully.", NULL, prv_quit, NULL},
            {"setflag", "Set the flag of sending data", NULL, prv_set_flag, NULL},
            {"download_fota", "Down load coap URI data", NULL, prv_download_fota, NULL},
            {"notify_fota", "Notify fota event", NULL, prv_notify_fota, NULL},

            {"^C", "Quit the client abruptly (without sending a de-register message).", NULL, NULL, NULL},

            COMMAND_END_LIST
    };

    memset(&data, 0, sizeof(client_data_t));
    data.addressFamily = AF_INET6;
    char cmdline[255]="";
    for (int i=0;i<argc;i++){
      strcat(cmdline,argv[i]);
      strcat(cmdline," ");
    }
    fprintf(stderr, "%s\n",cmdline);

    opt = 1;
    
    while (opt < argc)
    {
    
    fprintf(stderr, "opt=%d,argc=%d,argv[opt]=%s\n,%c,%d",opt,argc,argv[opt],argv[opt][0],argv[opt][2]);
        if (argv[opt] == NULL
            || argv[opt][0] != '-'
            || argv[opt][2] != 0)
        {
            print_usage();
            return 0;
        }
        
        fprintf(stderr, "%c\n",argv[opt][1]);
        switch (argv[opt][1])
        {
        
        case 'b':
            bootstrapRequested = true;
            if (!serverPortChanged) serverPort = LWM2M_BSSERVER_PORT_STR;
            break;
        case 'c':
            batterylevelchanging = 1;
            break;
        case 't':
            opt++;
            if (opt >= argc)
            {
                print_usage();
                return 0;
            }
            if (1 != sscanf(argv[opt], "%d", &lifetime))
            {
                print_usage();
                return 0;
            }
            break;
#if(defined WITH_TINYDTLS) || (defined WITH_MBEDDTLS)
        case 'i':
            opt++;
            if (opt >= argc)
            {
                print_usage();
                return 0;
            }
            pskId = argv[opt];
            break;
        case 's':
            opt++;
            if (opt >= argc)
            {
                print_usage();
                return 0;
            }
            psk = argv[opt];
            break;
#endif                        
        case 'n':
            opt++;
            if (opt >= argc)
            {
                print_usage();
                return 0;
            }
            name = argv[opt];
            break;
        case 'l':
            opt++;
            if (opt >= argc)
            {
                print_usage();
                return 0;
            }
            localPort = argv[opt];
            break;
        case 'h':
            opt++;
            if (opt >= argc)
            {
                print_usage();
                return 0;
            }
            server = argv[opt];
            break;
        case 'p':
            opt++;
            if (opt >= argc)
            {
                print_usage();
                return 0;
            }
            serverPort = argv[opt];
            serverPortChanged = true;
            break;
        case '4':
            data.addressFamily = AF_INET;
            break;
        case 'f':
            opt++;
            ref = atoi(argv[opt]);
            break;
        case 'k':
            opt++;
            ipc_socket = atoi(argv[opt]);
            break;
        case 'o':
            opt++;
            timeout = atoi(argv[opt]);
            break;
        case 'u':
            opt++;
            sendFlag= atoi(argv[opt]);
            break;
        // for DM standard begin
        case 'w':
            opt++;
            if (opt >= argc)
            {
                print_usage();
                return 0;
            }
            dmKey = argv[opt];
            break;
        case 'a':
            opt++;
            if (opt >= argc)
            {
                print_usage();
                return 0;
            }
            dmAppKey = argv[opt];
            break;
        case 'v':
            opt++;
            if (opt >= argc)
            {
                print_usage();
                return 0;
            }
            dmVersion = argv[opt];
            break;
        // end
        default:
            print_usage();
            return 0;
        }
        opt += 1;
    }

    if (!server)
    {
        server = (AF_INET == data.addressFamily ? DEFAULT_SERVER_IPV4 : DEFAULT_SERVER_IPV6);
    }
    /*
     * Invalid domain name check in advance to avoid enter the main func endless loop 
     */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = data.addressFamily;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    if (0 != getaddrinfo(server, serverPort, &hints, &servinfo) || servinfo == NULL)
    {
        fprintf(stderr, "getaddrinfo return failed %s %s\n",server, serverPort);
        if(servinfo != NULL)
            freeaddrinfo(servinfo);
        return -1;
    }
    freeaddrinfo(servinfo);
    servinfo = NULL;
    /*
     *This call an internal function that create an IPV6 socket on the port 5683.
     */
     
    uint8_t portstr[20]={0};
    res = utils_intToText(atoi(localPort)+ref,portstr,strlen(localPort));
    if (res <= 0) return -1;

    /*
     * Now the main function fill an array with each object, this list will be later passed to liblwm2m.
     * Those functions are located in their respective object file.
     */
#if(defined WITH_TINYDTLS) || (defined WITH_MBEDDTLS)
    if (psk != NULL)
    {
        int i;
        int len_tmp = 0;
        len_tmp = strlen(psk) % 2;
        pskLen = len_tmp ? (strlen(psk) / 2 + 1): (strlen(psk) / 2);
        pskBuffer = malloc(pskLen);

        if (NULL == pskBuffer)
        {
            fprintf(stderr, "Failed to create PSK binary buffer\r\n");
            return -1;
        }
        memset(pskBuffer, 0, pskLen);
        // Hex string to binary
        char *h = psk;
        char *b = pskBuffer;
        char xlate[] = "0123456789ABCDEF";
        if(len_tmp == 1) {
            char *l = strchr(xlate, toupper((unsigned char)(*h)));
            *b = l - xlate;//((l - xlate) << 4);//
            h++;
            b++;
        }
        for ( i = 0; i < (pskLen - len_tmp); h += 2, ++b, i++)
        {
            char *l = strchr(xlate, toupper((unsigned char)(*h)));
            char *r = strchr(xlate, toupper((unsigned char)(*(h+1))));

            if (!r || !l)
            {
                fprintf(stderr, "Failed to parse Pre-Shared-Key HEXSTRING\r\n");
                goto error_exit;
            }

            *b = ((l - xlate) << 4) + (r - xlate);
        }
    }
#endif

#if(defined WITH_TINYDTLS) || (defined WITH_MBEDDTLS)
    if (psk != NULL || pskId != NULL)
        sprintf (serverUri, "coaps://%s:%s", server, serverPort);
    else
        sprintf (serverUri, "coap://%s:%s", server, serverPort);
#else
    sprintf (serverUri, "coap://%s:%s", server, serverPort);
#endif
    /*
     * The liblwm2m library is now initialized with the functions that will be in
     * charge of communication
     */

    lwm2mH = lwm2m_init(&data);
    if (NULL == lwm2mH)
    {
        fprintf(stderr, "lwm2m_init() failed\r\n");
        goto error_exit;
    }

#ifdef LWM2M_BOOTSTRAP
    objArray[0] = get_security_object(serverId, serverUri, pskId, pskBuffer, pskLen, bootstrapRequested);
#else
    objArray[0] = get_security_object(serverId, serverUri, pskId, pskBuffer, pskLen, false);
#endif
    if (NULL == objArray[0])
    {
        fprintf(stderr, "Failed to create security object\r\n");
        goto error_exit;
    }
    data.securityObjP = objArray[0];

    objArray[1] = get_server_object(serverId, "U", lifetime, false);
    if (NULL == objArray[1])
    {
        fprintf(stderr, "Failed to create server object\r\n");
        goto error_exit;
    }
    
    objArray[2] = get_object_device(lwm2mH);
    if (NULL == objArray[2])
    {
        fprintf(stderr, "Failed to create Device object\r\n");
        goto error_exit;
    }

    objArray[3] = get_object_firmware(lwm2mH);
    if (NULL == objArray[3])
    {
        fprintf(stderr, "Failed to create Firmware object\r\n");
        goto error_exit;
    }

    objArray[4] = get_object_conn_m(lwm2mH);
    if (NULL == objArray[4])
    {
        fprintf(stderr, "Failed to create connectivity monitoring object\r\n");
        goto error_exit;
    }

#if 0
    objArray[4] = get_object_location(lwm2mH);
    if (NULL == objArray[4])
    {
        fprintf(stderr, "Failed to create location object\r\n");
        return -1;
    }

    objArray[5] = get_test_object(lwm2mH);
    if (NULL == objArray[5])
    {
        fprintf(stderr, "Failed to create test object\r\n");
        return -1;
    }

    objArray[5] = get_object_conn_m(lwm2mH);
    if (NULL == objArray[5])
    {
        fprintf(stderr, "Failed to create connectivity monitoring object\r\n");
        return -1;
    }

    objArray[6] = get_object_conn_s(lwm2mH);
    if (NULL == objArray[6])
    {
        fprintf(stderr, "Failed to create connectivity statistics object\r\n");
        return -1;
    }

    int instId = 0;
    objArray[7] = acc_ctrl_create_object(lwm2mH);
    if (NULL == objArray[7])
    {
        fprintf(stderr, "Failed to create Access Control object\r\n");
        return -1;
    }
    else if (acc_ctrl_obj_add_inst(objArray[7], instId, 3, 0, serverId)==false)
    {
        fprintf(stderr, "Failed to create Access Control object instance\r\n");
        return -1;
    }
    else if (acc_ctrl_oi_add_ac_val(objArray[7], instId, 0, 0b000000000001111)==false)
    {
        fprintf(stderr, "Failed to create Access Control ACL default resource\r\n");
        return -1;
    }
    else if (acc_ctrl_oi_add_ac_val(objArray[7], instId, 999, 0b000000000000001)==false)
    {
        fprintf(stderr, "Failed to create Access Control ACL resource for serverId: 999\r\n");
        return -1;
    }

    float sensorValue = 1.1;
    float minMeasuredValue = 1.5;
    float maxMeasuredValue = 10.5;
    float minRangeValue = 2.0;
    float maxRangeValue = 5.0;

    objArray[8] = get_temperature_object(sensorValue, "Cel", minMeasuredValue, maxMeasuredValue, minRangeValue, maxRangeValue);
    if (NULL == objArray[8])
    {
        fprintf(stderr, "Failed to create temperature object\r\n");
        return -1;
    }

    objArray[9] = get_illuminance_object(sensorValue, "lx", minMeasuredValue, maxMeasuredValue, minRangeValue, maxRangeValue);
    if (NULL == objArray[9])
    {
        fprintf(stderr, "Failed to create illuminance object\r\n");
        return -1;
    }
#endif

    lwm2mH->ref = ref;
    lwm2mH->sendflag = sendFlag;
#if(defined WITH_TINYDTLS) || (defined WITH_MBEDDTLS)
    data.lwm2mH = lwm2mH;
#endif
    
    /*
     * We configure the liblwm2m library with the name of the client - which shall be unique for each client -
     * the number of objects we will be passing through and the objects array
     */
    result = lwm2m_configure(lwm2mH, name, NULL, NULL, 5, objArray);
    if (result != 0)
    {
        fprintf(stderr, "lwm2m_configure() failed: 0x%X\r\n", result);
        goto error_exit;
    }

    // for DM standard begin
    if (dmKey != NULL)
    {
        lwm2mH->dmSecretKey= lwm2m_strdup(dmKey);
    }

    if (dmAppKey!= NULL)
    {
        lwm2mH->dmAppkey= lwm2m_strdup(dmAppKey);
    }

    if (dmVersion != NULL)
    {
        lwm2mH->dmVersion= lwm2m_strdup(dmVersion);
    }
    // end

    /**
     * Initialize value changed callback.
     */
    init_value_change(lwm2mH);

    /*
     * As you now have your lwm2m context complete you can pass it as an argument to all the command line functions
     * precedently viewed (first point)
     */
    for (int i = 0 ; commands[i].name != NULL ; i++)
    {
        commands[i].userData = (void *)lwm2mH;
    }
    fprintf(stdout, "LWM2M Client \"%s\" started on port %s\r\n", name, portstr);
    fprintf(stdout, "> "); fflush(stdout);
    /*
     * We now enter in a while loop that will handle the communications from the server
     */
    while (0 == lwm2m_configs[ref]->isquit)//lwm2mH->quit
    {
        struct timeval tv;
        fd_set readfds;
        fd_set exceptfds;
        int maxfd;

        if (g_reboot)
        {
            time_t tv_sec;

            tv_sec = lwm2m_gettime();

            if (0 == reboot_time)
            {
                reboot_time = tv_sec + 15;
            }
            if (reboot_time < tv_sec)
            {
                /*
                 * Message should normally be lost with reboot ...
                 */
                fprintf(stderr, "reboot time expired, rebooting ...");
                system_reboot();
            }
            else
            {
                tv.tv_sec = 1;
            }
        }
        else if (batterylevelchanging) 
        {
            update_battery_level(lwm2mH);
            tv.tv_sec = 5;
        }
        else 
        {
            tv.tv_sec = timeout;
        }

        if(g_fota_state == LWM2M_FOTA_STATE_DOWNLOADING)
        {
            long now = lwm2m_gettime();
            LOG_ARG("LWM2M_FOTA_STATE_DOWNLOADING  now %d long_time_index %d start_time %d",now,s_long_time_download_index, s_fota_download_start_time);
            if(s_fota_download_start_time != 0)
            {
                s_long_time_download_index = (now - s_fota_download_start_time)/10;
            }

            if(s_long_time_download_index %20 < 15)
            {
                if(now - s_fota_download_tv > 20)
                {
                    if(lwm2mH->fota_context.uri != NULL)
                    {
                        LOG_ARG("LWM2M_FOTA_STATE_DOWNLOADING s_fota_num_retry %d", s_fota_num_retry);
                        lwm2m_start_fota_download(lwm2mH->fota_context.uri, lwm2mH->ref);
                        s_fota_download_tv = lwm2m_gettime();
                    }
                }
                if(s_fota_num_retry > 3)
                {
                    notify_fota_state(LWM2M_FOTA_STATE_IDLE, LWM2M_FOTA_RESULT_CONNECTION_LOST, lwm2mH->ref);
                }
            }
            tv.tv_sec = 2;
        }
        tv.tv_usec = 0;

        FD_ZERO(&readfds);
        FD_ZERO(&exceptfds);
        FD_SET(ipc_socket, &readfds);

        if(data.sock > 0)
        {
            FD_SET(data.sock, &readfds);
            FD_SET(data.sock, &exceptfds);
        }
        else
        {
            fprintf(stderr, "Trying to bind LWM2M Client to port %s\r\n", portstr);
            data.sock = create_socket((const char *)portstr, data.addressFamily);
            if (data.sock > 0)
            {
                fprintf(stderr, "opening data socket success: %d\r\n", data.sock);
                FD_SET(data.sock, &readfds);
                FD_SET(data.sock, &exceptfds);
            }
            else
            {
                fprintf(stderr, "opening data socket[%d] failed: %d\r\n", data.sock, errno);
            }
        }

        maxfd = (data.sock > ipc_socket ? data.sock : ipc_socket);
        maxfd = maxfd + 1;

        /*
         * This function does two things:
         *  - first it does the work needed by liblwm2m (eg. (re)sending some packets).
         *  - Secondly it adjusts the timeout value (default 60s) depending on the state of the transaction
         *    (eg. retransmission) and the time between the next operation
         */
        result = lwm2m_step(lwm2mH, &(tv.tv_sec));
        fprintf(stdout, " -> State: ");
        switch (lwm2mH->state)
        {
        case STATE_INITIAL:
            fprintf(stdout, "STATE_INITIAL\r\n");
            break;
        case STATE_BOOTSTRAP_REQUIRED:
            connection_free(data.connList);
            data.connList = NULL;
            lwm2m_delete_context_list(lwm2mH);
            lwm2mH->state = STATE_INITIAL;
            result = 0;

            fprintf(stdout, "STATE_BOOTSTRAP_REQUIRED\r\n");
            break;
        case STATE_BOOTSTRAPPING:
            fprintf(stdout, "STATE_BOOTSTRAPPING\r\n");
            break;
        case STATE_REGISTER_REQUIRED:
            connection_free(data.connList);
            data.connList = NULL;
            lwm2m_delete_context_list(lwm2mH);
            lwm2mH->state = STATE_INITIAL;
            result = 0;

            fprintf(stdout, "STATE_REGISTER_REQUIRED\r\n");
            break;
        case STATE_REGISTERING:
            fprintf(stdout, "STATE_REGISTERING\r\n");
            break;
        case STATE_READY:
            fprintf(stdout, "STATE_READY\r\n");
            break;
        default:
            fprintf(stdout, "Unknown...\r\n");
            break;
        }
        if (result != 0)
        {
            fprintf(stderr, "lwm2m_step() failed: 0x%X\r\n", result);
#if 0
#ifdef LWM2M_BOOTSTRAP
            if(previousState == STATE_BOOTSTRAPPING)
            {
#ifdef WITH_LOGS
                fprintf(stdout, "[BOOTSTRAP] restore security and server objects\r\n");
#endif
                prv_restore_objects(lwm2mH);
                lwm2mH->state = STATE_INITIAL;
            }
            else goto error_exit;
#else
            goto error_exit;
#endif
#endif
            goto error_exit;
        }
#ifdef LWM2M_BOOTSTRAP
        update_bootstrap_info(&previousState, lwm2mH);
#endif
        /*
         * This part will set up an interruption until an event happen on SDTIN or the socket until "tv" timed out (set
         * with the precedent function)
         */
        fprintf(stderr, "lwm2m_select timeout:%lld",tv.tv_sec);
        result = select(maxfd, &readfds, NULL, &exceptfds, &tv);
        fprintf(stderr, "lwm2m_select result = %d",result);
        if (result < 0)
        {
            osiThreadSleep(tv.tv_sec*1000);
            
            if (errno != EINTR)
            {
              fprintf(stderr, "Error in select(): %d %s\r\n", errno, strerror(errno));
            }
        }
        else if (result > 0)
        {
            uint8_t buffer[MAX_PACKET_SIZE];
            int numBytes = 0;

            if (FD_ISSET(data.sock, &exceptfds))
            {
                fprintf(stderr,"Error set data.sock");

                // close the invalid sock
                close(data.sock);
                data.sock = 0;

                // reset the timeout for re-connection
                timeout = 30;

                // reset the lwm2m status to initial
                connection_free(data.connList);
                data.connList = NULL;
                lwm2m_delete_context_list(lwm2mH);
                lwm2mH->state = STATE_INITIAL;

                // notify the app return to at
                lwm2mPostEvent(lwm2mH->ref, SERVER_REGCMD_RESULT_IND, 0, 0, 0);
            }
            /*
             * If an event happens on the socket
             */
            if (FD_ISSET(data.sock, &readfds))
            {
                struct sockaddr_storage addr;
                socklen_t addrLen;

                addrLen = sizeof(addr);

                /*
                 * We retrieve the data received
                 
                 */
                  
                 if (psk != NULL || pskId != NULL)
                    numBytes = recvfrom(data.sock, buffer, MAX_PACKET_SIZE, MSG_PEEK, (struct sockaddr *)&addr, &addrLen);
                            
                else 
                    numBytes = recvfrom(data.sock, buffer, MAX_PACKET_SIZE, 0, (struct sockaddr *)&addr, &addrLen);

                if (0 > numBytes)
                {
                    fprintf(stderr, "Error in recvfrom(): %d %s\r\n", errno, strerror(errno));

                    if(errno == 9)
                    {
                        // close the invalid sock
                        close(data.sock);
                        data.sock = 0;

                        // reset the timeout for re-connection
                        timeout = 30;

                        // reset the lwm2m status to initial
                        connection_free(data.connList);
                        data.connList = NULL;
                        lwm2m_delete_context_list(lwm2mH);
                        lwm2mH->state = STATE_INITIAL;

                        // notify the app return to at
                        lwm2mPostEvent(lwm2mH->ref, SERVER_REGCMD_RESULT_IND, 0, 0, 0);
                    }
                }
                else if (0 < numBytes)
                {
#if defined(LWIP_IPV4_ON) && defined(LWIP_IPV6_ON)
                    char s[INET6_ADDRSTRLEN];
#else
#ifdef LWIP_IPV4_ON
                    char s[INET_ADDRSTRLEN];
#else
                    char s[INET6_ADDRSTRLEN];
#endif
#endif
                    in_port_t port = 0;

#if defined (WITH_TINYDTLS) || defined (WITH_MBEDDTLS) 
                    dtls_connection_t * connP;
#else
                    connection_t * connP;
#endif
#ifdef LWIP_IPV4_ON
                    if (AF_INET == addr.ss_family)
                    {
                        struct sockaddr_in *saddr = (struct sockaddr_in *)&addr;
                        inet_ntop(saddr->sin_family, &saddr->sin_addr, s, INET_ADDRSTRLEN);
                        port = saddr->sin_port;
                    }
#endif
#ifdef LWIP_IPV6_ON
                    if (AF_INET6 == addr.ss_family)
                    {
                        struct sockaddr_in6 *saddr = (struct sockaddr_in6 *)&addr;
                        inet_ntop(saddr->sin6_family, &saddr->sin6_addr, s, INET6_ADDRSTRLEN);
                        port = saddr->sin6_port;
                    }
#endif
                    fprintf(stderr, "%d bytes received from [%s]:%hu\r\n", numBytes, s, ntohs(port));

                    /*
                     * Display it in the STDERR
                     */
                    output_buffer(stderr, buffer, numBytes, 0);

                    connP = connection_find(data.connList, &addr, addrLen);
                    if (connP != NULL)
                    {
                        /*
                         * Let liblwm2m respond to the query depending on the context
                         */
#if defined (WITH_TINYDTLS) || defined (WITH_MBEDDTLS) 
                        if (0 != connection_handle_packet(connP, buffer, numBytes))
                        {
                             printf("error handling message %d\n",result);
                        }
#else
                        lwm2m_handle_packet(lwm2mH, buffer, numBytes, connP);

#endif
                        LOG_ARG("numBytes=%d",numBytes);
                        conn_s_updateRxStatistic(lwm2mH, numBytes, false);
                    }
                    else
                    {
                        fprintf(stderr, "received bytes ignored!\r\n");
                    }
                }
                else
                {
                    fprintf(stderr, "0 in recvfrom()\r\n");
                }
            }

            /*
             * If the event happened on the SDTIN
             */
            if (FD_ISSET(ipc_socket, &readfds))
            {
                numBytes = read(ipc_socket, buffer, MAX_PACKET_SIZE - 1);

                if (numBytes > 1)
                {
                    buffer[numBytes] = 0;
                    /*
                     * We call the corresponding callback of the typed command passing it the buffer for further arguments
                     */
                     int ret = handle_command(commands, (char*)buffer);
                    if (ret < 0)
                        lwm2mPostEvent(lwm2mH->ref,OBJ_OPERATE_RSP_IND,-1,-1,0);
                    else if (ret == 0)
                        lwm2mPostEvent(lwm2mH->ref,OBJ_OPERATE_RSP_IND,0,-1,0);
                    else
                        fprintf(stderr, "commands:%s ignore reponse:%d",(char *)commands,ret);
                }
                if (lwm2mH->quit == 0)
                {
                    fprintf(stdout, "\r\n> ");
                    fflush(stdout);
                }
                else
                {
                    fprintf(stdout, "\r\n");
                }
            }
        }
    }
    isquit = lwm2m_configs[ref]->isquit;//lwm2mH->quit;

error_exit:

    /*
     * Finally when the loop is left smoothly - asked by user in the command line interface - we unregister our client from it
     */
#if(defined WITH_TINYDTLS) || (defined WITH_MBEDDTLS)
    free(pskBuffer);
#endif
fprintf(stdout,"exit");

#ifdef LWM2M_BOOTSTRAP
    close_backup_object();
#endif
    if(lwm2mH != NULL)
    {
        clean_all_objects(lwm2mH);
        lwm2m_close(lwm2mH);
    }
    if(objArray[0])
        lwm2m_free(objArray[0]);
    if(objArray[1])
        lwm2m_free(objArray[1]);
    if(objArray[2])
        lwm2m_free(objArray[2]);
    if(objArray[3])
        lwm2m_free(objArray[3]);
    if(objArray[4])
        lwm2m_free(objArray[4]);
    close(data.sock);

#ifdef MEMORY_TRACE
    if (isquit == 1)
    {
        trace_print(0, 1);
    }
#endif

    return isquit==0?-1:1;
}

