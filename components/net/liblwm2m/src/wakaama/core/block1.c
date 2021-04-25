/*******************************************************************************
 *
 * Copyright (c) 2016 Intel Corporation and others.
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
 *    Simon Bernard - initial API and implementation
 *
 *******************************************************************************/
/*
 Copyright (c) 2016 Intel Corporation

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
*/
#include "internals.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

extern uint32_t write_fota_upgrade_data(uint32_t block_num, uint8_t  block_more, uint8_t * data, uint16_t datalen);
// the maximum payload transferred by block1 we accumulate per server
#define MAX_BLOCK1_SIZE 4096

uint8_t coap_block1_handler(lwm2m_block1_data_t ** pBlock1Data,
                            uint16_t mid,
                            uint8_t * buffer,
                            size_t length,
                            uint16_t blockSize,
                            uint32_t blockNum,
                            bool blockMore,
                            uint8_t ** outputBuffer,
                            size_t * outputLength)
{
    lwm2m_block1_data_t * block1Data = *pBlock1Data;;

    // manage new block1 transfer
    if (blockNum == 0)
    {
       // we already have block1 data for this server, clear it
       if (block1Data != NULL)
       {
           lwm2m_free(block1Data->block1buffer);
       }
       else
       {
           block1Data = lwm2m_malloc(sizeof(lwm2m_block1_data_t));
           *pBlock1Data = block1Data;
           if (NULL == block1Data) return COAP_500_INTERNAL_SERVER_ERROR;
       }

       block1Data->block1buffer = lwm2m_malloc(length);
       block1Data->block1bufferSize = length;

       // write new block in buffer
       memcpy(block1Data->block1buffer, buffer, length);
       block1Data->lastmid = mid;
    }
    // manage already started block1 transfer
    else
    {
       if (block1Data == NULL)
       {
           // we never receive the first block
           // TODO should we clean block1 data for this server ?
           return COAP_408_REQ_ENTITY_INCOMPLETE;
       }

       // If this is a retransmission, we already did that.
       if (block1Data->lastmid != mid)
       {
          uint8_t * oldBuffer = block1Data->block1buffer;
          size_t oldSize = block1Data->block1bufferSize;

          if (block1Data->block1bufferSize != blockSize * blockNum)
          {
              // we don't receive block in right order
              // TODO should we clean block1 data for this server ?
              return COAP_408_REQ_ENTITY_INCOMPLETE;
          }

          // is it too large?
          if (block1Data->block1bufferSize + length >= MAX_BLOCK1_SIZE) {
              return COAP_413_ENTITY_TOO_LARGE;
          }
          // re-alloc new buffer
          block1Data->block1bufferSize = oldSize+length;
          block1Data->block1buffer = lwm2m_malloc(block1Data->block1bufferSize);
          if (NULL == block1Data->block1buffer) return COAP_500_INTERNAL_SERVER_ERROR;
          memcpy(block1Data->block1buffer, oldBuffer, oldSize);
          lwm2m_free(oldBuffer);

          // write new block in buffer
          memcpy(block1Data->block1buffer + oldSize, buffer, length);
          block1Data->lastmid = mid;
       }
    }

    if (blockMore)
    {
        *outputLength = -1;
        return COAP_231_CONTINUE;
    }
    else
    {
        // buffer is full, set output parameter
        // we don't free it to be able to send retransmission
        *outputLength = block1Data->block1bufferSize;
        *outputBuffer = block1Data->block1buffer;

        return NO_ERROR;
    }
}

void free_block1_buffer(lwm2m_block1_data_t * block1Data)
{
    if (block1Data != NULL)
    {
        // free block1 buffer
        lwm2m_free(block1Data->block1buffer);
        block1Data->block1bufferSize = 0 ;

        // free current element
        lwm2m_free(block1Data);
    }
}
extern lwm2m_fota_state_t g_fota_state;
extern bool check_fota_file_sanity();
uint8_t lwm2m_fota_block2_handler(lwm2m_context_t * contextP,
    uint16_t mid,
    uint8_t * buffer,
    size_t length,
    uint16_t blockSize,
    uint32_t blockNum,
    bool blockMore,
    coap_packet_t * message)
{
    uint8_t coapError = NO_ERROR;
    lwm2m_fota_t * fotaContext = &contextP->fota_context;
    LOG_ARG("lwm2m_fota_block2_handler block2Num %d blockSize %d blockMore %d g_fota_state %d\r\n",blockNum,blockSize,blockMore,g_fota_state);
    if(g_fota_state != LWM2M_FOTA_STATE_DOWNLOADING)
        return COAP_IGNORE;
    if(blockNum == 0)
    {
        uint8_t * token = NULL;
        if(coap_get_header_token(message,(const uint8_t **)(&token)) == 0)
        {
            LOG("lwm2m_fota_block2_handler get token err\r\n");
            return COAP_IGNORE;
        }

        if(memcmp(fotaContext->token, token, COAP_TOKEN_LEN)!=0)
        {
            LOG("lwm2m_fota_block2_handler token err\r\n");
            return COAP_IGNORE;
        }
        coapError = write_fota_upgrade_data(blockNum, blockMore, buffer, length);
        if(coapError != COAP_NO_ERROR){
           LOG("lwm2m_fota_block2_handler write_fota_upgrade_data err\r\n");
           notify_fota_state(LWM2M_FOTA_STATE_IDLE, LWM2M_FOTA_RESULT_NOT_ENOUGH_FLASH, contextP->ref);
           return COAP_500_INTERNAL_SERVER_ERROR;
        }
        fotaContext->block2bufferSize = length;
        fotaContext->lastmid = mid;
        fotaContext->block2Num = ++blockNum;
    }
    else
    {
       if(fotaContext->lastmid != mid)
       {
            uint8_t * token = NULL;
            if(coap_get_header_token(message,(const uint8_t **)&token) == 0)
            {
                LOG("lwm2m_fota_block2_handler get token err\r\n");
                return COAP_IGNORE;
            }

            if(memcmp(fotaContext->token, token, COAP_TOKEN_LEN)!=0)
            {
                LOG("lwm2m_fota_block2_handler token err\r\n");
                return COAP_IGNORE;
            }
            coapError = write_fota_upgrade_data(blockNum, blockMore, buffer, length);
            if(coapError != COAP_NO_ERROR){
               LOG("lwm2m_fota_block2_handler write_fota_upgrade_data err\r\n");
               notify_fota_state(LWM2M_FOTA_STATE_IDLE, LWM2M_FOTA_RESULT_NOT_ENOUGH_FLASH, contextP->ref);
               return COAP_500_INTERNAL_SERVER_ERROR;
            }
            fotaContext->block2bufferSize += length;
            fotaContext->lastmid = mid;
            fotaContext->block2Num = ++blockNum;
       }
    }

    if(blockMore)
    {
        lwm2m_start_fota_download(fotaContext->uri, contextP->ref);
        if(blockNum %10 == 1)
        {
            notify_fota_state(LWM2M_FOTA_STATE_DOWNLOADING, LWM2M_FOTA_RESULT_INIT, contextP->ref);
        }
    }else
    {

        LOG_ARG("lwm2m_fota_block2_handler fota download success size %d\r\n",fotaContext->block2bufferSize);
        if(check_fota_file_sanity())
        {
            notify_fota_state(LWM2M_FOTA_STATE_DOWNLOADED, LWM2M_FOTA_RESULT_INIT, contextP->ref);
        }else
        {
            notify_fota_state(LWM2M_FOTA_STATE_IDLE, LWM2M_FOTA_RESULT_INTEGRITY_FAILUER, contextP->ref);
        }
    }

    return coapError;
}
