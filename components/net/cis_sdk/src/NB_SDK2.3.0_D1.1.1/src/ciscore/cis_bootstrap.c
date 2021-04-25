/*******************************************************************************
 *
 * Copyright (c) 2015 Sierra Wireless and others.
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
 *    Pascal Rieux - Please refer to git log
 *    Bosch Software Innovations GmbH - Please refer to git log
 *    David Navarro, Intel Corporation - Please refer to git log
 *    Baijie & LongRong, China Mobile - Please refer to git log
 *
 *******************************************************************************/
#include "cis_api.h"
#include "cis_internals.h"
#include "cis_log.h"
#include "std_object/std_object.h"




#define PRV_QUERY_BUFFER_LENGTH 200


static void prv_handleBootstrapReply(st_transaction_t * transaction,void * message);
static void prv_requestBootstrap(st_context_t * context,st_server_t * bootstrapServer);
static coap_status_t prv_checkServerStatus(st_server_t * serverP);
static void prv_tagServer(st_context_t * contextP,uint16_t id);


void bootstrap_step(st_context_t * context,
                    cis_time_t currentTime)
{
    st_server_t * targetP;
    targetP = context->bootstrapServer;
    if (targetP != NULL)
    {
        LOGD("bootstrap step status: %s", STR_STATUS(targetP->status));
        switch (targetP->status)
        {
        case STATE_UNCREATED:
            break;
        case STATE_CONNECTED:
            {
                targetP->registration = currentTime + context->lifetime;
                targetP->status = STATE_BS_HOLD_OFF;
                prv_requestBootstrap(context, targetP);
            }
            break;
        case STATE_BS_HOLD_OFF:
            break;
        default:
            break;
        }
        LOGD("bootstrap step status finish(%d): %s", targetP->status,STR_STATUS(targetP->status));
    }
}

coap_status_t bootstrap_handleFinish(st_context_t * context,
                                     void * fromSessionH)
{
    st_server_t * bootstrapServer;
	bootstrapServer = context->bootstrapServer;
    if (bootstrapServer != NULL
     && bootstrapServer->status == STATE_BS_PENDING)
    {
        LOGD("Bootstrap server status changed to STATE_BS_FINISHED");
        bootstrapServer->status = STATE_BS_FINISHED;
        return COAP_204_CHANGED;
    }
    return COAP_IGNORE;
}

void bootstrap_init(st_context_t * context)
{
    st_server_t * bootstrapServer;
    bootstrapServer = management_makeServerList(context,true);
    if (bootstrapServer != NULL)
    {
        bootstrapServer->status = STATE_UNCREATED;
        context->bootstrapServer = bootstrapServer;
    }else{
        LOGE("ERROR:bootstrap_init failed.");
    }
}

/*
 * Reset the bootstrap servers statuses
 *
 * handle LWM2M Servers the client is registered to
 *
 */
void bootstrap_create(st_context_t * context)
{

    st_server_t * bootstrapServer;
    bootstrapServer = context->bootstrapServer;
    
    if (bootstrapServer == NULL)
    {
        LOGE("ERROR:bootstrapServer create failed,server is NULL");
    }

    management_createNetwork(context,bootstrapServer);
    if(bootstrapServer->sessionH == NULL){
        bootstrapServer->status = STATE_CREATE_FAILED;
    }
}

void  bootstrap_connect(st_context_t * context)
{

    st_server_t * bootstrapServer;
    bootstrapServer = context->bootstrapServer;
    if (bootstrapServer == NULL)
    {
        LOGE("ERROR:bootstrapServer create failed,server is NULL");
    }
    if(!management_connectServer(context,bootstrapServer))
    {
        bootstrapServer->status = STATE_CONNECT_FAILED;
    }

}


void  bootstrap_destory(st_context_t * context)
{
    st_transaction_t * transacP = NULL;
    transacP = context->transactionList;
    while (transacP != NULL)
    {
        st_transaction_t * transacP_tmp = transacP->next;
        if (transacP->server == context->bootstrapServer){
            LOGD("Remove unavailable bootstrap transaction!");
            transaction_remove(context, transacP);
        }
        transacP = transacP_tmp;
    }

    st_server_t * targetP;
    targetP = context->bootstrapServer;
    management_destoryServer(context,targetP);
    context->bootstrapServer = NULL;
}


/*
 * Returns STATE_BS_PENDING if at least one bootstrap is still pending
 * Returns STATE_BS_FINISHED if at least one bootstrap succeeded and no bootstrap is pending
 * Returns STATE_BS_FAILED if all bootstrap failed.
 */
et_status_t bootstrap_getStatus(st_context_t * context)
{
    et_status_t bs_status;
    bs_status = STATE_BS_FAILED;


    st_server_t * targetP;
    
    targetP = context->bootstrapServer;
    if (targetP == NULL)
    {
        return bs_status;
    }

    bs_status = targetP->status;
    switch (targetP->status)
    {
    case STATE_BS_FINISHED:
        break;
    case STATE_BS_INITIATED:
        break;
    case STATE_BS_HOLD_OFF:
        break;
    case STATE_BS_PENDING:
        bs_status = STATE_BS_PENDING;
        break;
    case STATE_BS_FAILED:
    case STATE_CREATE_FAILED:
    case STATE_CONNECT_FAILED:
        bs_status = STATE_BS_FAILED;
        break;
    default:
        break;
    }

    return bs_status;
}





coap_status_t bootstrap_handleCommand(st_context_t * contextP,
                                      st_uri_t * uriP,
                                      st_server_t * serverP,
                                      coap_packet_t * message,
                                      coap_packet_t * response)
{
    coap_status_t result = COAP_400_BAD_REQUEST;

    et_media_type_t format;
    LOGD("Code: %02X", message->code);
    
    format = utils_convertMediaType(message->content_type);

    result = prv_checkServerStatus(serverP);
    if (result != COAP_NO_ERROR) return result;
    
    LOG_URI("bootstrap command uri",uriP);
    
    switch (message->code)
    {
    case COAP_PUT:
        {
            st_data_t * dataP = NULL;
            int size = 0;
            int i;
            st_object_t * objectP;
            st_object_t * securityObjP = NULL;

            if (!CIS_URI_IS_SET_INSTANCE(uriP))
            {
                result = COAP_501_NOT_IMPLEMENTED;
                break;
            }
            
            if(uriP->objectId != CIS_SECURITY_OBJECT_ID)
            {
                result = CIS_COAP_404_NOT_FOUND;
                break;
            }

            for (objectP = contextP->objectList; objectP != NULL; objectP = objectP->next)
            {
                if (objectP->objID == CIS_SECURITY_OBJECT_ID)
                {
                    securityObjP = objectP;
                }
            }
            if(securityObjP == NULL)
            {
                result = COAP_404_NOT_FOUND;
                break;
            }

            if (message->payload_len == 0 || message->payload == 0)
            {
                result = COAP_400_BAD_REQUEST;
            }
            else
            {
                size = data_parse(uriP, message->payload, message->payload_len, format, &dataP);
                if (size == 0)
                {
                    result = COAP_500_INTERNAL_SERVER_ERROR;
                    break;
                }

                for (i = 0 ; i < size ; i++)
                {						
                        
                    result = std_object_write_handler(contextP,uriP->instanceId,1,&dataP[i],securityObjP);
                    if (uriP->objectId == CIS_SECURITY_OBJECT_ID
                    && result == COAP_204_CHANGED)
                    {
                        prv_tagServer(contextP, dataP[i].id);
                    }

                    if(result != COAP_204_CHANGED) // Stop object create or write when result is error
                    {
                        break;
                    }
                         
                }
                data_free(size, dataP);
            }
            
        }
        break;

    case COAP_DELETE:
        {
            if (CIS_URI_IS_SET_RESOURCE(uriP))
            {
                result = COAP_400_BAD_REQUEST;
            }
            else
            {
                result = COAP_501_NOT_IMPLEMENTED; 
            }
        }
        break;
    case COAP_GET:
    case COAP_POST:
    default:
        result = COAP_400_BAD_REQUEST;
        break;
    }


    if (result == COAP_202_DELETED
     || result == COAP_204_CHANGED)
    {
        if (serverP->status != STATE_BS_PENDING)
        {
            serverP->status = STATE_BS_FAILED;
            
        }
    }
    LOGD("bootstrap result server status: %s", STR_STATUS(serverP->status));

    return result;
}


///////////////////////////////////////////////////////////////////////////////////////
static void prv_handleBootstrapReply(st_transaction_t * transaction,void * message)
{
    st_server_t * bootstrapServer = (st_server_t *)transaction->server;
    coap_packet_t * coapMessage = (coap_packet_t *)message;

    LOGD("bootstrap reply status:%d",bootstrapServer->status);
    if (bootstrapServer->status == STATE_BS_INITIATED)
    {
        if (NULL != coapMessage && COAP_204_CHANGED == coapMessage->code)
        {
            LOGD("Received ACK/2.04, Bootstrap pending...");
            bootstrapServer->status = STATE_BS_PENDING;
        }
        else
        {
            LOGE("bootstrap failed..");
            bootstrapServer->status = STATE_BS_FAILED;
        }
    }
}

// start a device initiated bootstrap
static void prv_requestBootstrap(st_context_t * context,
    st_server_t * bootstrapServer)
{
    char query[PRV_QUERY_BUFFER_LENGTH];
    int query_length = 0;
    int res;


    query_length = utils_stringCopy(query, PRV_QUERY_BUFFER_LENGTH, "?ep=");
    if (query_length < 0)
    {
        bootstrapServer->status = STATE_BS_FAILED;
        return;
    }
    res = utils_stringCopy(query + query_length, PRV_QUERY_BUFFER_LENGTH - query_length, context->endpointName);
    if (res < 0)
    {
        bootstrapServer->status = STATE_BS_FAILED;
        return;
    }
    query_length += res;

    if (bootstrapServer->sessionH == NULL)
    {
        //bootstrapServer->sessionH =  management_createConnect(context,bootstrapServer);
        LOGE("Bootstrap request failed. session is null");
    }

    if (bootstrapServer->sessionH != NULL)
    {
        st_transaction_t * transaction = NULL;

        LOGD("Bootstrap server connection opened");

        transaction = transaction_new(COAP_TYPE_CON, COAP_POST, NULL, NULL, context->nextMid++, 4, NULL);
        if (transaction == NULL)
        {
            bootstrapServer->status = STATE_BS_FAILED;
            return;
        }

        coap_set_header_uri_path(transaction->message, "/"URI_BOOTSTRAP_SEGMENT);
        coap_set_header_uri_query(transaction->message, query);
#if CIS_ENABLE_AUTH
//    if(context->isDM!=TRUE&&context->authCode!=NULL)
#if CIS_ENABLE_DM
    if(context->isDM!=TRUE&&context->authCode!=NULL)
#else
	 if(context->authCode!=NULL)
#endif
       coap_set_header_auth_code( transaction->message, context->authCode );
    #endif
        transaction->callback = prv_handleBootstrapReply;
        transaction->userData = (void *)context;
        transaction->server = bootstrapServer;
        context->transactionList = (st_transaction_t *)CIS_LIST_ADD(context->transactionList, transaction);
        if (transaction_send(context, transaction) == true)
        {
            LOGD("CI bootstrap requested to BS server");
            bootstrapServer->status = STATE_BS_INITIATED;
        }
    }
    else
    {
        LOGD("Connecting bootstrap server failed");
        bootstrapServer->status = STATE_BS_FAILED;
    }
}




static coap_status_t prv_checkServerStatus(st_server_t * serverP)
{
    LOGD("check server status: %s", STR_STATUS(serverP->status));

    switch (serverP->status)
    {
    case STATE_BS_HOLD_OFF:
        serverP->status = STATE_BS_PENDING;
        LOGD("Status changed to: %s", STR_STATUS(serverP->status));
        break;

    case STATE_BS_INITIATED:
        // The ACK was probably lost
        serverP->status = STATE_BS_PENDING;
        LOGD("Status changed to: %s", STR_STATUS(serverP->status));
        break;

    case STATE_UNCREATED:
        // server initiated bootstrap
    case STATE_BS_PENDING:
        // do nothing
        break;

    case STATE_BS_FINISHED:
    case STATE_BS_FAILED:
    default:
        LOGW("check server status returning COAP_IGNORE");
        return COAP_IGNORE;
    }

    return COAP_NO_ERROR;
}


static void prv_tagServer(st_context_t * contextP,
                          uint16_t id)
{
    st_server_t * targetP;
	//TODO:
    targetP = (st_server_t *)CIS_LIST_FIND(contextP->bootstrapServer, id);
    if (targetP == NULL)
    {
        targetP = (st_server_t *)CIS_LIST_FIND(contextP->server, id);
    }
    if (targetP != NULL)
    {
        targetP->dirty = true;
    }
}


