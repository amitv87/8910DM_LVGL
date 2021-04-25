/* -*- Mode: C; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

/* coap-client -- simple CoAP client
 *
 * Copyright (C) 2010--2016 Olaf Bergmann <bergmann@tzi.org>
 *
 * This file is part of the CoAP library libcoap. Please see README for terms of
 * use.
 */
//#define exit(x) do{ sys_arch_printf("exit with %d\n",x);result = -1;goto exit;} while(0)

#include "coap_config.h"

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

u32_t coap_client_payload_size();

int coap_client_main(int argc, char **argv) ;

u8_t coap_client_set_payload(void *data,unsigned int payload_length, unsigned char payload_type);

void coap_client_set_datahander(coap_incoming_data_cb_t handler,void *param); 
