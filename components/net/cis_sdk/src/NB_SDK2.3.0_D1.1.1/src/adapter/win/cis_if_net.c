#include "sockets.h"

#include <cis_if_net.h>
#include <cis_list.h>
#include <cis_log.h>
#include <cis_if_sys.h>
#include <cis_internals.h>
#include <osi_log.h>
#define MAX_PACKET_SIZE			(1024)

static int prvCreateSocket(uint16_t localPort,int ai_family);

static void callbackRecvThread(void* lpParam);
//static void callbackAttachThread(void* lpParam);
#if SAMPLE_TCP
static void callbackConnectThread(void* lpParam);
#endif


bool  cisnet_attached_state(void * ctx)
{
    return ((struct st_cis_context *)(ctx))->netAttached;
}

cis_ret_t cisnet_init(void *context,const cisnet_config_t* config,cisnet_callback_t cb)
{

    OSI_LOGI(0x10007692, "fall in cisnet_init\r\n");
    cis_memcpy(&((struct st_cis_context *)context)->netConfig,config,sizeof(cisnet_config_t));
    ((struct st_cis_context *)context)->netCallback.onEvent = cb.onEvent;
	osiThreadSleep(1000);
    ((struct st_cis_context *)context)->netAttached = 1;
    return CIS_RET_OK;
}

cis_ret_t cisnet_create(cisnet_t* netctx,const char* host,void* context)
{
     char szBuffer[24] = {0};
     //uint32_t readlen = 0;
     //uint8_t len = 0;

	 st_context_t * ctx = (struct st_cis_context *)context;
     
	 if (ctx->netAttached != true)return CIS_RET_ERROR;

    (*netctx) = (cisnet_t)cis_malloc(sizeof(struct st_cisnet_context));
    (*netctx)->sock = 0;
	(*netctx)->port = atoi((const char *)(ctx->serverPort));
    (*netctx)->state = 0;
    (*netctx)->quit = 1;
	(*netctx)->g_packetlist=NULL;
    (*netctx)->context = context;
    cissys_lockcreate(&((*netctx)->lockpacket));

    FILE* f = fopen("host","r");
    if(f != NULL && fread(szBuffer,1,sizeof(szBuffer) - 1,f) > 0)
    {
        memset((*netctx)->host, 0, sizeof((*netctx)->host));
        strncpy((*netctx)->host,szBuffer,sizeof((*netctx)->host)-1);
    }else
    {
        memset((*netctx)->host, 0, sizeof((*netctx)->host));
        strncpy((*netctx)->host,host,sizeof((*netctx)->host)-1);
    }

    if(f != NULL){
        fclose(f);
    }

    return CIS_RET_OK;
}

void     cisnet_destroy(cisnet_t netctx)
{
    OSI_LOGI(0, "cisnet_destroy wait thread exit...");
    closesocket(netctx->sock);
    while (netctx->quit == 0)
        osiThreadSleep(5);
    OSI_LOGI(0, "cisnet_destroy will to free");
    cissys_lockdestory(netctx->lockpacket);
    cis_free(netctx);
}


cis_ret_t cisnet_connect(cisnet_t netctx)
{
#if SAMPLE_TCP
    HANDLE hThread;
    DWORD dwThread;

    if(netctx->state == 0){
        netctx->state = 1;
        hThread = CreateThread(NULL,0,callbackConnectThread,(LPVOID)netctx,0,&dwThread);
        CloseHandle(hThread);
    }
#else//UDP
    int sock = -1;
    sock = prvCreateSocket(0,AF_INET);
    if (sock < 0)
    {
        OSI_LOGI(0x10007694, "Failed to open socket: %d %s\r\n", errno, strerror(errno));
        return CIS_RET_ERROR;
    }
    netctx->sock = sock;
    netctx->state = 1;

   ((struct st_cis_context *)(netctx->context))->netCallback.onEvent(netctx,cisnet_event_connected,NULL,netctx->context);
   ((struct st_cis_context *)(netctx->context))->netCallback.hander = osiThreadCreate("cisnet_recv", callbackRecvThread,(void *)netctx, OSI_PRIORITY_NORMAL, 8192 * 4, 32);
   if(((struct st_cis_context *)(netctx->context))->netCallback.hander == NULL)
   {
        OSI_LOGI(0x10007695, "Failed to create cisnet_recv thread!");
        return CIS_RET_ERROR;
   }
#endif

    return CIS_RET_OK;
}

cis_ret_t cisnet_disconnect(cisnet_t netctx)
{
    netctx->state = 0;
   ((struct st_cis_context *)(netctx->context))->netCallback.onEvent(netctx,cisnet_event_disconnect,NULL,netctx->context);
    return 1;
}

cis_ret_t cisnet_write(cisnet_t netctx,const uint8_t * buffer,uint32_t  length)
{
    int nbSent;
    size_t addrlen;
    size_t offset;
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET; 
    saddr.sin_port = htons(netctx->port);
    saddr.sin_addr.s_addr = inet_addr(netctx->host);
    
    addrlen = sizeof(saddr);
    offset = 0;
    while (offset != length)
    {
        nbSent = sendto(netctx->sock, (const char*)buffer + offset, length - offset, 0, (struct sockaddr *)&saddr, addrlen);

        if (nbSent == -1){
            OSI_LOGXI(OSI_LOGPAR_SI, 0x10007696, "socket sendto [%s:%d] failed.\r\n",netctx->host,ntohs(saddr.sin_port));
            return -1;
        }else{
            OSI_LOGXI(OSI_LOGPAR_SII, 0x10007697, "socket sendto [%s:%d] %d bytes\r\n",netctx->host,ntohs(saddr.sin_port),nbSent);
        }
        offset += nbSent;
    }

    return CIS_RET_OK;
}

cis_ret_t cisnet_read(cisnet_t netctx,uint8_t** buffer,uint32_t *length)
{
    struct st_net_packet * target = NULL;
    cissys_lock(netctx->lockpacket, CIS_CONFIG_LOCK_INFINITY);
    for(target=netctx->g_packetlist;target!=NULL;target=target->next){
        OSI_LOGI(0x10007698, "cisnet_read target address:%p, target length:%ld", target, target->length);
    }
    if(netctx->g_packetlist != NULL){
        struct st_net_packet* delNode;
        *buffer = netctx->g_packetlist->buffer;
        *length = netctx->g_packetlist->length;
         delNode =netctx->g_packetlist;
         netctx->g_packetlist = netctx->g_packetlist->next;
         cis_free(delNode);
         cissys_unlock(netctx->lockpacket);
         return CIS_RET_OK;
    }

    cissys_unlock(netctx->lockpacket);
    return CIS_RET_ERROR;
}

cis_ret_t cisnet_free(cisnet_t netctx,uint8_t* buffer,uint32_t length)
{
    cis_free(buffer);
    return CIS_RET_ERROR;
}



//////////////////////////////////////////////////////////////////////////

static int prvCreateSocket(uint16_t localPort,int ai_family)
{
    int sock = socket(ai_family,SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0){
        return -1;
    }
    return sock;
}



static void callbackRecvThread(void* lpParam)
{
    uint32_t packetid = 0;
    cisnet_t netctx = (cisnet_t)lpParam;
	int sock = netctx->sock;
	netctx->quit = 0;
	while(netctx->state == 1)
	{
		struct timeval tv = {5,0};
		fd_set readfds;
		int result;

		FD_ZERO(&readfds);
		FD_SET(sock, &readfds);
		/*
			* This part will set up an interruption until an event happen on SDTIN or the socket until "tv" timed out (set
			* with the precedent function)
			*/
		result = select(FD_SETSIZE, &readfds, NULL, NULL, &tv);

		if (result < 0)
		{
			OSI_LOGXI(OSI_LOGPAR_IS, 0x10003fdd, "Error in select(): %d %s\r\n", errno, strerror(errno));
            goto TAG_END;
		}
		else if (result > 0)
		{
			uint8_t buffer[MAX_PACKET_SIZE];
			int numBytes;

			/*
				* If an event happens on the socket
				*/
			if (FD_ISSET(sock, &readfds))
			{
				struct sockaddr_storage addr;
				socklen_t addrLen;

				addrLen = sizeof(addr);

				/*
					* We retrieve the data received
					*/
				numBytes = recvfrom(sock, (char*)buffer, MAX_PACKET_SIZE, 0, (struct sockaddr *)&addr, &addrLen);

				if (numBytes < 0)
				{
					if(errno == 0)continue;
					OSI_LOGXI(OSI_LOGPAR_IS, 0x10003fdf, "Error in recvfrom(): %d %s\r\n", errno, strerror(errno));
				}
				else if (numBytes > 0)
				{
					char s[INET_ADDRSTRLEN];
			
					struct sockaddr_in *saddr = (struct sockaddr_in *)&addr;
					inet_ntop(saddr->sin_family, &saddr->sin_addr, s, INET_ADDRSTRLEN);

					OSI_LOGXI(OSI_LOGPAR_ISI, 0x10007699, "%d bytes received from [%s]:%hu", numBytes, s, ntohs(saddr->sin_port));
					
                    uint8_t* data = (uint8_t*)cis_malloc(numBytes);
                    cis_memcpy(data,buffer,numBytes);

                    struct st_net_packet *packet = (struct st_net_packet*)cis_malloc(sizeof(struct st_net_packet));
                    packet->next = NULL;
                    packet->id = packetid++;
                    packet->buffer = data;
                    packet->length = numBytes;
                    //packet->netctx = netctx;
                    cissys_lock(netctx->lockpacket, CIS_CONFIG_LOCK_INFINITY);
                    netctx->g_packetlist = (struct st_net_packet*)CIS_LIST_ADD(netctx->g_packetlist,packet);
                    struct st_net_packet * target = NULL;
                    for(target=netctx->g_packetlist;target!=NULL;target=target->next){
                        OSI_LOGI(0x1000769a, "callbackRecvThread target address:%p, target length:%ld", target, target->length);
                    }
                    cissys_unlock(netctx->lockpacket);
				}
			}
		}
	}
TAG_END:
	netctx->quit = 1;
	OSI_LOGI(0x1000769b, "Error in socket recv thread exit..\n");
    osiThreadExit();
}
