/*================================================================
  Copyright (c) 2020 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
=================================================================*/
/*=================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN              WHO         WHAT, WHERE, WHY
------------     -------     -------------------------------------------------------------------------------

=================================================================*/



#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ql_api_osi.h"
#include "ql_api_nw.h"

#include "ql_log.h"
#include "ql_api_datacall.h"
#include "sockets.h"
#include "lwip/ip_addr.h"
#include "lwip/ip6_addr.h"

#include "lwip/netdb.h"
#include "lwip/netif.h"
#include "lwip/inet.h"
#include "lwip/tcp.h"

#include "ql_ssl.h"

#define QL_SSL_LOG_LEVEL	            QL_LOG_LEVEL_INFO
#define QL_SSL_LOG(msg, ...)			QL_LOG(QL_SSL_LOG_LEVEL, "ql_SSL", msg, ##__VA_ARGS__)
#define QL_SSL_LOG_PUSH(msg, ...)	    QL_LOG_PUSH("ql_SSL", msg, ##__VA_ARGS__)


static ql_task_t ssl_task = NULL;

#define TEST_HOST_ADDR   "www.baidu.com" 
#define TEST_HOST_PORT   443 

#define TEST_SEND_DATA   "GET /index.html HTTP/1.1\r\n\
                          HOST: www.baidu.com\r\n\
                          User-Agent: QuecOpen\r\n\
                          Connection: Keep-Alive\r\n\r\n\0"

#define DTLS_TEST_HOST_ADDR "220.180.239.212"
#define DTLS_TEST_HOST_PORT 8252

static void ssl_test_tls_with_noblocking(int profile_idx)
{
	ql_data_call_info_s info;
	struct sockaddr_in local4, server_ipv4;
	struct sockaddr_in6 local6, server_ipv6;
	struct addrinfo *pres = NULL;
	char ip_addr_str[64] = {0};
	int socket_fd = -1;
	int flags = 0;
	int ret = 0;
	fd_set read_fds;
	fd_set write_fds;
	fd_set exp_fds;
	int  fd_changed;
	int  connected = 0;
	int  ssl_hs_completed = 0;
	ql_ssl_config  ssl_cfg;
	ql_ssl_context ssl_ctx;

	ql_get_data_call_info(0, profile_idx, &info);
	
	ret = getaddrinfo_with_pcid(TEST_HOST_ADDR, NULL, NULL, &pres, (uint32)profile_idx);
	if (ret < 0 || pres == NULL) 
	{
		QL_SSL_LOG("DNS getaddrinfo failed! ret=%d; pres=%p!\n",ret,pres);
		return;
	}

	QL_SSL_LOG("local ip ver:%d", info.ip_version);
	
	if(info.ip_version == QL_PDP_TYPE_IPV6){
		struct addrinfo *temp = NULL;
		for(temp = pres; temp != NULL; temp = temp->ai_next){
			if(temp->ai_family == AF_INET6){
				socket_fd = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
				if(socket_fd < 0){
					break;
				}
				flags |= O_NONBLOCK;
				fcntl(socket_fd, F_SETFL,flags);
				local6.sin6_family = AF_INET6;
			  	local6.sin6_port = 0;
			    local6.sin6_len = sizeof(struct sockaddr_in6);
				inet6_aton(ip6addr_ntoa(&info.v6.addr.ip), &local6.sin6_addr);
				ret = bind(socket_fd,(struct sockaddr *)&local6,sizeof(struct sockaddr));
		 		if(ret != 0){
					close(socket_fd);
					socket_fd = -1;
					break;
				}
				memcpy(&server_ipv6, (struct sockaddr_in6 *)temp->ai_addr, sizeof(struct sockaddr_in6));
				server_ipv6.sin6_family = AF_INET6;
				server_ipv6.sin6_port = htons(TEST_HOST_PORT);
				inet_ntop(AF_INET6, &server_ipv6.sin6_addr, ip_addr_str, sizeof(ip_addr_str));
				QL_SSL_LOG("By DNS,server_ipv6.sin_addr: %s\n", ip_addr_str);
				ret = connect(socket_fd, (struct sockaddr *)&server_ipv6, sizeof(server_ipv6));
				if((ret < 0 && lwip_get_error(socket_fd) == EINPROGRESS) || ret ==0){
					if(ret == 0)
						connected = 1;
					break;
				}else{
					close(socket_fd);
					socket_fd = -1;
					continue;
				}
			}
		}
	}else{
		struct addrinfo *temp = NULL;
		for(temp = pres; temp != NULL; temp = temp->ai_next){
			QL_SSL_LOG("temp->ai_family:%d", temp->ai_family);
			if(temp->ai_family == AF_INET){
				struct sockaddr_in * sin_res = (struct sockaddr_in *)temp->ai_addr;
				socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				QL_SSL_LOG("socket fd:%d",socket_fd);
				if(socket_fd < 0){
					break;
				}
				flags |= O_NONBLOCK;
				fcntl(socket_fd, F_SETFL,flags);
				local4.sin_family = AF_INET;
			    local4.sin_port = 0;
				inet_aton(ip4addr_ntoa(&info.v4.addr.ip), &local4.sin_addr);	
				ret = bind(socket_fd,(struct sockaddr *)&local4,sizeof(struct sockaddr));
				QL_SSL_LOG("bind ret:%d, errno:%d",ret, lwip_get_error(socket_fd));
		 		if(ret != 0){
					close(socket_fd);
					socket_fd = -1;
					break;
				}
				memset(&server_ipv4, 0x00, sizeof(struct sockaddr_in));
				server_ipv4.sin_addr = sin_res->sin_addr;
				server_ipv4.sin_family = AF_INET;
				server_ipv4.sin_port = htons(TEST_HOST_PORT);
				inet_ntop(AF_INET, &server_ipv4.sin_addr, ip_addr_str, sizeof(ip_addr_str));
				QL_SSL_LOG("By DNS,server_ipv4.sin_addr: %s\n", ip_addr_str);
				ret = connect(socket_fd, (struct sockaddr *)&server_ipv4, sizeof(server_ipv4));				
				QL_SSL_LOG("connect ret:%d, errno:%d",ret, lwip_get_error(socket_fd));
				if((ret < 0 && lwip_get_error(socket_fd) == EINPROGRESS) || ret ==0){
					if(ret == 0)
						connected = 1;
					break;
				}else{
					close(socket_fd);
					socket_fd = -1;
					continue;
				}
			}
		}
	}
	QL_SSL_LOG("socket fd:%d",socket_fd);
	if(socket_fd < 0)
		return;

	FD_ZERO(&read_fds);
	FD_ZERO(&write_fds);
	FD_ZERO(&exp_fds);
	
	FD_SET(socket_fd, &write_fds);	
	FD_SET(socket_fd, &exp_fds);
	
	while(1){
		fd_changed = select(socket_fd+1, &read_fds, &write_fds, &exp_fds, NULL);
		if(fd_changed > 0){
			if(FD_ISSET(socket_fd, &write_fds)){
				FD_CLR(socket_fd, &write_fds);				
				if(connected== 0){
					int value = 0;
					int len = 0;
						
					len = sizeof(value);
					getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, &value, &len);
						
					QL_SSL_LOG("errno: %d", value);
					if(value == 0 || value == EISCONN ){
						int *ciphersuit = NULL;	

						connected = 1;
						QL_SSL_LOG("tcp socket connected and ready to write!!!!");
						do{
							ret = ql_ssl_conf_init(&ssl_cfg);
							ciphersuit = malloc(sizeof(int)*2);
							if(ciphersuit == NULL){
								ret = QL_SSL_ERROR_OUT_OF_MEM;
								break;
							}
								
							ciphersuit[0] = QL_SSL_TLS_RSA_WITH_RC4_128_MD5;
							ciphersuit[1] = QL_SSL_TLS_RSA_WITH_RC4_128_SHA;
							
							ql_ssl_conf_set(&ssl_cfg, QL_SSL_CONF_VERSION, QL_SSL_VERSION_ALL);
							ql_ssl_conf_set(&ssl_cfg, QL_SSL_CONF_TRANSPORT, QL_SSL_TLS_PROTOCOL);
							ql_ssl_conf_set(&ssl_cfg, QL_SSL_CONF_AUTHMODE, QL_SSL_VERIFY_NULL);
							ql_ssl_conf_set(&ssl_cfg, QL_SSL_CONF_HS_TIMEOUT, 60);
							ql_ssl_conf_set(&ssl_cfg, QL_SSL_CONF_CIPHERSUITE, ciphersuit);
							if((ret =ql_ssl_new(&ssl_ctx)) != QL_SSL_SUCCESS){
								break;
							}
							ql_ssl_set_hostname(&ssl_ctx, TEST_HOST_ADDR);
							ql_ssl_set_socket_fd(&ssl_ctx, socket_fd);
							if((ret= ql_ssl_setup(&ssl_ctx, &ssl_cfg)) != QL_SSL_SUCCESS){
								break;
							}
							ret = ql_ssl_handshark(&ssl_ctx);
							
						}while(0);
						if(!(ret == QL_SSL_SUCCESS || ret == QL_SSL_ERROR_WANT_READ||ret == QL_SSL_ERROR_WANT_WRITE)){
							QL_SSL_LOG("ssl handshark failed!!!!");
							ql_ssl_conf_free(&ssl_cfg);
							ql_ssl_free(&ssl_ctx);
							shutdown(socket_fd, SHUT_WR);
							break;
						}else if(ret == QL_SSL_ERROR_WANT_READ){
							FD_SET(socket_fd, &read_fds);	
						}else if(ret == QL_SSL_ERROR_WANT_WRITE){
							FD_SET(socket_fd, &write_fds);	
						}else if(ret == QL_SSL_SUCCESS){
							ssl_hs_completed = 1;
							QL_SSL_LOG("SSL HandShark completed !!!!");
							
							ql_ssl_write(&ssl_ctx, (const unsigned char *)TEST_SEND_DATA, strlen(TEST_SEND_DATA));
							
							FD_SET(socket_fd, &read_fds);
						}
					}else{
						QL_SSL_LOG("tcp socket connected failure!!!!");
						break;
					}
				}else{
					QL_SSL_LOG("tcp socket ready to write!!!!");
					if(ssl_hs_completed == 0){
						ret = ql_ssl_handshark(&ssl_ctx);
						if(!(ret == QL_SSL_SUCCESS || ret == QL_SSL_ERROR_WANT_READ||ret == QL_SSL_ERROR_WANT_WRITE)){
							ql_ssl_conf_free(&ssl_cfg);
							ql_ssl_free(&ssl_ctx);
							shutdown(socket_fd, SHUT_WR);
							break;
						}else if(ret == QL_SSL_ERROR_WANT_READ){
							FD_SET(socket_fd, &read_fds);	
						}else if(ret == QL_SSL_ERROR_WANT_WRITE){
							FD_SET(socket_fd, &write_fds);	
						}else if(ret == QL_SSL_SUCCESS){
							ssl_hs_completed = 1;
							QL_SSL_LOG("SSL HandShark completed !!!!");
							
							ql_ssl_write(&ssl_ctx, (const unsigned char *)TEST_SEND_DATA, strlen(TEST_SEND_DATA));							
							FD_SET(socket_fd, &read_fds);							
						}
					}
				}
			}
			if(FD_ISSET(socket_fd, &read_fds)){
				FD_CLR(socket_fd, &read_fds);
				QL_SSL_LOG("tcp socket ready to read!!!!");
				if(ssl_hs_completed == 0){
					ret = ql_ssl_handshark(&ssl_ctx);
					if(!(ret == QL_SSL_SUCCESS || ret == QL_SSL_ERROR_WANT_READ||ret == QL_SSL_ERROR_WANT_WRITE)){
						ql_ssl_conf_free(&ssl_cfg);
						ql_ssl_free(&ssl_ctx);
						shutdown(socket_fd, SHUT_WR);
						break;
					}else if(ret == QL_SSL_ERROR_WANT_READ){
						FD_SET(socket_fd, &read_fds);	
					}else if(ret == QL_SSL_ERROR_WANT_WRITE){
						FD_SET(socket_fd, &write_fds);	
					}else if(ret == QL_SSL_SUCCESS){
						ssl_hs_completed = 1;
						QL_SSL_LOG("SSL HandShark completed !!!!");
							
						ql_ssl_write(&ssl_ctx, (const unsigned char *)TEST_SEND_DATA, strlen(TEST_SEND_DATA));
							
						FD_SET(socket_fd, &read_fds);
					}
				}else{
					unsigned char test_recv_buf[120] = {0};
					int  end = 0;
					do{
						ret =ql_ssl_read(&ssl_ctx, test_recv_buf,120);
						if(ret > 0){
							QL_SSL_LOG("recv: %s", test_recv_buf);
						}else{
							QL_SSL_LOG("ret:%d",ret);
							if(ret == QL_SSL_ERROR_WANT_READ){
								FD_SET(socket_fd, &read_fds);
							}else{
								end = 1;
							}
							break;
						}
					}while(1);			
					if(end == 1)
						break;
				}
			}
			if(FD_ISSET(socket_fd, &exp_fds)){
				FD_CLR(socket_fd, &exp_fds);
				QL_SSL_LOG("tcp socket occur exception!!!!");
				shutdown(socket_fd, SHUT_WR);
				break;
			}
		}
	}
	if(ssl_hs_completed == 1){
		ql_ssl_conf_free(&ssl_cfg);
		ql_ssl_free(&ssl_ctx);
		shutdown(socket_fd, SHUT_WR);
	}
	close(socket_fd);
}

static void ssl_test_tls_with_blocking(int profile_idx)
{
	ql_data_call_info_s info;
	struct sockaddr_in local4, server_ipv4;
	struct sockaddr_in6 local6, server_ipv6;
	struct addrinfo *pres = NULL;
	char ip_addr_str[64] = {0};
	int socket_fd = -1;
	int ret = 0;
	ql_ssl_config  ssl_cfg;
	ql_ssl_context ssl_ctx;

	ql_get_data_call_info(0, profile_idx, &info);
	
	ret = getaddrinfo_with_pcid(TEST_HOST_ADDR, NULL, NULL, &pres, (uint32)profile_idx);
	if (ret < 0 || pres == NULL) 
	{
		QL_SSL_LOG("DNS getaddrinfo failed! ret=%d; pres=%p!\n",ret,pres);
		return;
	}

	QL_SSL_LOG("local ip ver:%d", info.ip_version);
	
	if(info.ip_version == QL_PDP_TYPE_IPV6){
		struct addrinfo *temp = NULL;
		for(temp = pres; temp != NULL; temp = temp->ai_next){
			if(temp->ai_family == AF_INET6){
				socket_fd = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
				if(socket_fd < 0){
					break;
				}
			
				local6.sin6_family = AF_INET6;
			  	local6.sin6_port = 0;
			    local6.sin6_len = sizeof(struct sockaddr_in6);
				inet6_aton(ip6addr_ntoa(&info.v6.addr.ip), &local6.sin6_addr);
				ret = bind(socket_fd,(struct sockaddr *)&local6,sizeof(struct sockaddr));
		 		if(ret != 0){
					close(socket_fd);
					socket_fd = -1;
					break;
				}
				memcpy(&server_ipv6, (struct sockaddr_in6 *)temp->ai_addr, sizeof(struct sockaddr_in6));
				server_ipv6.sin6_family = AF_INET6;
				server_ipv6.sin6_port = htons(TEST_HOST_PORT);
				inet_ntop(AF_INET6, &server_ipv6.sin6_addr, ip_addr_str, sizeof(ip_addr_str));
				QL_SSL_LOG("By DNS,server_ipv6.sin_addr: %s\n", ip_addr_str);
				ret = connect(socket_fd, (struct sockaddr *)&server_ipv6, sizeof(server_ipv6));
				if(ret ==0){
					break;
				}else{
					close(socket_fd);
					socket_fd = -1;
					continue;
				}
			}
		}
	}else{
		struct addrinfo *temp = NULL;
		for(temp = pres; temp != NULL; temp = temp->ai_next){
			QL_SSL_LOG("temp->ai_family:%d", temp->ai_family);
			if(temp->ai_family == AF_INET){
				struct sockaddr_in * sin_res = (struct sockaddr_in *)temp->ai_addr;
				socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				QL_SSL_LOG("socket fd:%d",socket_fd);
				if(socket_fd < 0){
					break;
				}

				local4.sin_family = AF_INET;
			    local4.sin_port = 0;
				inet_aton(ip4addr_ntoa(&info.v4.addr.ip), &local4.sin_addr);	
				ret = bind(socket_fd,(struct sockaddr *)&local4,sizeof(struct sockaddr));
				QL_SSL_LOG("bind ret:%d, errno:%d",ret, lwip_get_error(socket_fd));
		 		if(ret != 0){
					close(socket_fd);
					socket_fd = -1;
					break;
				}
				memset(&server_ipv4, 0x00, sizeof(struct sockaddr_in));
				server_ipv4.sin_addr = sin_res->sin_addr;
				server_ipv4.sin_family = AF_INET;
				server_ipv4.sin_port = htons(TEST_HOST_PORT);
				inet_ntop(AF_INET, &server_ipv4.sin_addr, ip_addr_str, sizeof(ip_addr_str));
				QL_SSL_LOG("By DNS,server_ipv4.sin_addr: %s\n", ip_addr_str);
				ret = connect(socket_fd, (struct sockaddr *)&server_ipv4, sizeof(server_ipv4));				
				QL_SSL_LOG("connect ret:%d, errno:%d",ret, lwip_get_error(socket_fd));
				if(ret ==0){
					break;
				}else{
					close(socket_fd);
					socket_fd = -1;
					continue;
				}
			}
		}
	}
	QL_SSL_LOG("socket fd:%d",socket_fd);
	if(socket_fd < 0)
		return;
	//ssl handshark
	do{
		int *ciphersuit = NULL;	
		ret = ql_ssl_conf_init(&ssl_cfg);
		ciphersuit = malloc(sizeof(int)*2);
		if(ciphersuit == NULL){
			ret = QL_SSL_ERROR_OUT_OF_MEM;
			break;
		}
								
		ciphersuit[0] = QL_SSL_TLS_RSA_WITH_RC4_128_MD5;
		ciphersuit[1] = QL_SSL_TLS_RSA_WITH_RC4_128_SHA;
							
		ql_ssl_conf_set(&ssl_cfg, QL_SSL_CONF_VERSION, QL_SSL_VERSION_ALL);
		ql_ssl_conf_set(&ssl_cfg, QL_SSL_CONF_TRANSPORT, QL_SSL_TLS_PROTOCOL);
		ql_ssl_conf_set(&ssl_cfg, QL_SSL_CONF_AUTHMODE, QL_SSL_VERIFY_NULL);
		ql_ssl_conf_set(&ssl_cfg, QL_SSL_CONF_HS_TIMEOUT, 60);
		ql_ssl_conf_set(&ssl_cfg, QL_SSL_CONF_CIPHERSUITE, ciphersuit);
		if((ret =ql_ssl_new(&ssl_ctx)) != QL_SSL_SUCCESS){
			break;
		}
		ql_ssl_set_hostname(&ssl_ctx, TEST_HOST_ADDR);
		ql_ssl_set_socket_fd(&ssl_ctx, socket_fd);
		if((ret= ql_ssl_setup(&ssl_ctx, &ssl_cfg)) != QL_SSL_SUCCESS){
			break;
		}
		ret = ql_ssl_handshark(&ssl_ctx);							
	}while(0);
	if(ret != QL_SSL_SUCCESS){
		QL_SSL_LOG("ssl handshark failed ！！！！");
		ql_ssl_conf_free(&ssl_cfg);
		ql_ssl_free(&ssl_ctx);
		shutdown(socket_fd, SHUT_WR);
		close(socket_fd);
		return ;
	}else{
		unsigned char test_recv_buf[120] = {0};
		QL_SSL_LOG("SSL HandShark completed !!!!");
							
		if(ql_ssl_write(&ssl_ctx, (const unsigned char *)TEST_SEND_DATA, strlen(TEST_SEND_DATA)) >0){
			while((ret = ql_ssl_read(&ssl_ctx, test_recv_buf,120)) > 0){
				
				QL_SSL_LOG("recv: %s", test_recv_buf);
				if(ret < 120)
					break;
			}
		}
		ql_ssl_close_notify(&ssl_ctx);
		ql_ssl_conf_free(&ssl_cfg);
		ql_ssl_free(&ssl_ctx);
		shutdown(socket_fd, SHUT_RDWR);
		close(socket_fd);
		return ;
	}
}
static void ssl_test_dtls(int profile_idx)
{
	ql_data_call_info_s info;
	struct sockaddr_in local4, server_ipv4;
	struct sockaddr_in6 local6, server_ipv6;
	struct addrinfo *pres = NULL;
	char ip_addr_str[64] = {0};
	int socket_fd = -1;
	int ret = 0;
	int flags = 0;
	fd_set read_fds;
	fd_set write_fds;
	fd_set exp_fds;
	int  fd_changed;
	ql_ssl_config  ssl_cfg;
	ql_ssl_context ssl_ctx;
	int *ciphersuit = NULL;	
	unsigned char psk[] = {0x31,0x32,0x33,0x34,0x35,0x36};
	unsigned char psk_id[] = "123";
	int  ssl_hs_completed = 0;
	
	FD_ZERO(&read_fds);
	FD_ZERO(&write_fds);
	FD_ZERO(&exp_fds);
	
	ql_get_data_call_info(0, profile_idx, &info);
	
	ret = getaddrinfo_with_pcid(DTLS_TEST_HOST_ADDR, NULL, NULL, &pres, (uint32)profile_idx);
	if (ret < 0 || pres == NULL) 
	{
		QL_SSL_LOG("DNS getaddrinfo failed! ret=%d; pres=%p!\n",ret,pres);
		return;
	}

	QL_SSL_LOG("local ip ver:%d", info.ip_version);
	
	if(info.ip_version == QL_PDP_TYPE_IPV6){
		struct addrinfo *temp = NULL;
		for(temp = pres; temp != NULL; temp = temp->ai_next){
			if(temp->ai_family == AF_INET6){
				socket_fd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
				if(socket_fd < 0){
					break;
				}
				flags |= O_NONBLOCK;
				fcntl(socket_fd, F_SETFL,flags);
				local6.sin6_family = AF_INET6;
			  	local6.sin6_port = 0;
			    local6.sin6_len = sizeof(struct sockaddr_in6);
				inet6_aton(ip6addr_ntoa(&info.v6.addr.ip), &local6.sin6_addr);
				ret = bind(socket_fd,(struct sockaddr *)&local6,sizeof(struct sockaddr));
		 		if(ret != 0){
					close(socket_fd);
					socket_fd = -1;
					break;
				}
				memcpy(&server_ipv6, (struct sockaddr_in6 *)temp->ai_addr, sizeof(struct sockaddr_in6));
				server_ipv6.sin6_family = AF_INET6;
				server_ipv6.sin6_port = htons(DTLS_TEST_HOST_PORT);
				inet_ntop(AF_INET6, &server_ipv6.sin6_addr, ip_addr_str, sizeof(ip_addr_str));
				QL_SSL_LOG("By DNS,server_ipv6.sin_addr: %s\n", ip_addr_str);
				ret = connect(socket_fd, (struct sockaddr *)&server_ipv6, sizeof(server_ipv6));
				if(ret ==0){
					break;
				}else{
					close(socket_fd);
					socket_fd = -1;
					continue;
				}
			}
		}
	}else{
		struct addrinfo *temp = NULL;
		for(temp = pres; temp != NULL; temp = temp->ai_next){
			QL_SSL_LOG("temp->ai_family:%d", temp->ai_family);
			if(temp->ai_family == AF_INET){
				struct sockaddr_in * sin_res = (struct sockaddr_in *)temp->ai_addr;
				socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
				QL_SSL_LOG("socket fd:%d",socket_fd);
				if(socket_fd < 0){
					break;
				}
				flags |= O_NONBLOCK;
				fcntl(socket_fd, F_SETFL,flags);
				local4.sin_family = AF_INET;
			    local4.sin_port = 0;
				inet_aton(ip4addr_ntoa(&info.v4.addr.ip), &local4.sin_addr);	
				ret = bind(socket_fd,(struct sockaddr *)&local4,sizeof(struct sockaddr));
				QL_SSL_LOG("bind ret:%d, errno:%d",ret, lwip_get_error(socket_fd));
		 		if(ret != 0){
					close(socket_fd);
					socket_fd = -1;
					break;
				}
				memset(&server_ipv4, 0x00, sizeof(struct sockaddr_in));
				server_ipv4.sin_addr = sin_res->sin_addr;
				server_ipv4.sin_family = AF_INET;
				server_ipv4.sin_port = htons(DTLS_TEST_HOST_PORT);
				inet_ntop(AF_INET, &server_ipv4.sin_addr, ip_addr_str, sizeof(ip_addr_str));
				QL_SSL_LOG("By DNS,server_ipv4.sin_addr: %s\n", ip_addr_str);
				ret = connect(socket_fd, (struct sockaddr *)&server_ipv4, sizeof(server_ipv4));				
				QL_SSL_LOG("connect ret:%d, errno:%d",ret, lwip_get_error(socket_fd));
				if(ret ==0){
					break;
				}else{
					close(socket_fd);
					socket_fd = -1;
					continue;
				}
			}
		}
	}	
	QL_SSL_LOG("socket fd:%d",socket_fd);
	if(socket_fd < 0)
		return;
	//ssl handshark
	
	ret = ql_ssl_conf_init(&ssl_cfg);
	ciphersuit = malloc(sizeof(int)*2);
	if(ciphersuit == NULL){
		close(socket_fd);
		return;
	}
								
	ciphersuit[0] = QL_SSL_TLS_PSK_WITH_RC4_128_SHA;
	ciphersuit[1] = QL_SSL_TLS_PSK_WITH_AES_128_CBC_SHA;
							
	ql_ssl_conf_set(&ssl_cfg, QL_SSL_CONF_VERSION, QL_SSL_VERSION_3);
	ql_ssl_conf_set(&ssl_cfg, QL_SSL_CONF_TRANSPORT, QL_SSL_DTLS_PROTOCOL);
	ql_ssl_conf_set(&ssl_cfg, QL_SSL_CONF_AUTHMODE, QL_SSL_VERIFY_SERVER);
	ql_ssl_conf_set(&ssl_cfg, QL_SSL_CONF_HS_TIMEOUT, 5000);
	ql_ssl_conf_set(&ssl_cfg, QL_SSL_CONF_CIPHERSUITE, ciphersuit);
	ql_ssl_conf_set(&ssl_cfg, QL_SSL_CONF_PSK, psk, 6, psk_id, 3);
	if((ret =ql_ssl_new(&ssl_ctx)) != QL_SSL_SUCCESS){
		ql_ssl_conf_free(&ssl_cfg);
		close(socket_fd);
		return;
	}
	ql_ssl_set_hostname(&ssl_ctx, DTLS_TEST_HOST_ADDR);
	ql_ssl_set_socket_fd(&ssl_ctx, socket_fd);
	if((ret= ql_ssl_setup(&ssl_ctx, &ssl_cfg)) != QL_SSL_SUCCESS){
		ql_ssl_conf_free(&ssl_cfg);
		ql_ssl_free(&ssl_ctx);
		close(socket_fd);
		return;
	}
	ret = ql_ssl_handshark(&ssl_ctx);
	if(!(ret == QL_SSL_SUCCESS || ret == QL_SSL_ERROR_WANT_READ||ret == QL_SSL_ERROR_WANT_WRITE)){
		QL_SSL_LOG("ssl handshark failed!!!!");
		ql_ssl_conf_free(&ssl_cfg);
		ql_ssl_free(&ssl_ctx);
		close(socket_fd);
		return;
	}
	
	if(ret != QL_SSL_SUCCESS){
		if(ret == QL_SSL_ERROR_WANT_READ)	
			FD_SET(socket_fd, &read_fds);
		else
			FD_SET(socket_fd, &write_fds);
		
		FD_SET(socket_fd, &exp_fds);

		while(1){
			fd_changed = select(socket_fd+1, &read_fds, &write_fds, &exp_fds, NULL);
			if(fd_changed > 0){
				if(FD_ISSET(socket_fd, &write_fds)){
					FD_CLR(socket_fd, &write_fds);	
					if(ssl_hs_completed == 0){
						ret = ql_ssl_handshark(&ssl_ctx);
						if(!(ret == QL_SSL_SUCCESS || ret == QL_SSL_ERROR_WANT_READ||ret == QL_SSL_ERROR_WANT_WRITE)){
							QL_SSL_LOG("ssl handshark failed!!!!");
							break;
						}else if(ret == QL_SSL_SUCCESS){
							QL_SSL_LOG("ssl handshark completed!!!!");
							ssl_hs_completed = 1;
							break;
						}else if(ret == QL_SSL_ERROR_WANT_READ){
							FD_SET(socket_fd, &read_fds);
						}else if(ret == QL_SSL_ERROR_WANT_WRITE){
							FD_SET(socket_fd, &write_fds);
						}
					}
				}
				if(FD_ISSET(socket_fd, &read_fds)){
					FD_CLR(socket_fd, &read_fds);	
					if(ssl_hs_completed == 0){
						ret = ql_ssl_handshark(&ssl_ctx);
						if(!(ret == QL_SSL_SUCCESS || ret == QL_SSL_ERROR_WANT_READ||ret == QL_SSL_ERROR_WANT_WRITE)){
							QL_SSL_LOG("ssl handshark failed!!!!");
							break;
						}else if(ret == QL_SSL_SUCCESS){
							QL_SSL_LOG("ssl handshark completed!!!!");
							ssl_hs_completed = 1;
							break;
						}else if(ret == QL_SSL_ERROR_WANT_READ){
							FD_SET(socket_fd, &read_fds);
						}else if(ret == QL_SSL_ERROR_WANT_WRITE){
							FD_SET(socket_fd, &write_fds);
						}
					}
				}
				if(FD_ISSET(socket_fd, &exp_fds)){
					FD_CLR(socket_fd, &exp_fds);
					break;
				}
			}
		}
	}	
	
	ql_ssl_conf_free(&ssl_cfg);
	ql_ssl_free(&ssl_ctx);
	close(socket_fd);
	return;
}

static void ssl_app_thread(void * arg)
{
	int ret = 0;
	int i = 0, run_num = 1;
	int profile_idx = 1;
    ql_data_call_info_s info;
	char ip4_addr_str[16] = {0};
	uint8_t nSim = 0;
	

	ql_rtos_task_sleep_s(10);
	QL_SSL_LOG("========== ssl demo start ==========");
	QL_SSL_LOG("wait for network register done");
		
	while((ret = ql_network_register_wait(nSim, 120)) != 0 && i < 10){
    	i++;
		ql_rtos_task_sleep_s(1);
	}
	if(ret == 0){
		i = 0;
		QL_SSL_LOG("====network registered!!!!====");
	}else{
		QL_SSL_LOG("====network register failure!!!!!====");
		goto exit;
	}

	ql_set_data_call_asyn_mode(nSim, profile_idx, 0);

	QL_SSL_LOG("===start data call====");
	ret=ql_start_data_call(nSim, profile_idx, QL_PDP_TYPE_IP, "uninet", NULL, NULL, 0); 
	QL_SSL_LOG("===data call result:%d", ret);
	if(ret != 0){
		QL_SSL_LOG("====data call failure!!!!=====");
	}
	memset(&info, 0x00, sizeof(ql_data_call_info_s));
	
	ret = ql_get_data_call_info(nSim, profile_idx, &info);
	if(ret != 0){
		QL_SSL_LOG("ql_get_data_call_info ret: %d", ret);
		ql_stop_data_call(nSim, profile_idx);
		goto exit;
	}
    QL_SSL_LOG("info->profile_idx: %d", info.profile_idx);
	QL_SSL_LOG("info->ip_version: %d", info.ip_version);
            
	QL_SSL_LOG("info->v4.state: %d", info.v4.state); 
	inet_ntop(AF_INET, &info.v4.addr.ip, ip4_addr_str, sizeof(ip4_addr_str));
	QL_SSL_LOG("info.v4.addr.ip: %s\r\n", ip4_addr_str);

	inet_ntop(AF_INET, &info.v4.addr.pri_dns, ip4_addr_str, sizeof(ip4_addr_str));
	QL_SSL_LOG("info.v4.addr.pri_dns: %s\r\n", ip4_addr_str);

	inet_ntop(AF_INET, &info.v4.addr.sec_dns, ip4_addr_str, sizeof(ip4_addr_str));
	QL_SSL_LOG("info.v4.addr.sec_dns: %s\r\n", ip4_addr_str);
	while(run_num <= 100){
		int case_id = run_num%2;
		QL_SSL_LOG("==============ssl_demo_test_tls[%d]================\n",run_num);
		if(case_id == 0){
			ssl_test_tls_with_noblocking(profile_idx);
		}else if(case_id == 1){
			ssl_test_tls_with_blocking(profile_idx);
		}
		QL_SSL_LOG("==============ssl_demo_test_tls[%d] end================\n",run_num);
		run_num++;
		ql_rtos_task_sleep_s(1);
	}
	QL_SSL_LOG("==============ssl_demo_test_dtls================\n");
	ssl_test_dtls(profile_idx);
	QL_SSL_LOG("==============ssl_demo_test_dtls end================\n");
exit:  
   ql_rtos_task_delete(ssl_task);	
	
   return;	
}

int ql_ssl_app_init(void)
{
	QlOSStatus err = QL_OSI_SUCCESS;
	
    err = ql_rtos_task_create(&ssl_task, 16*1024, 23, "ssl_app", ssl_app_thread, NULL, 5);
	if(err != QL_OSI_SUCCESS)
    {
		QL_SSL_LOG("ssl_app init failed");
	}

	return err;
}

