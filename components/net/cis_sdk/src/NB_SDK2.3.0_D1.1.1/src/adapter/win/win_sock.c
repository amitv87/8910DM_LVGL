#include "win_sock.h"
#include <stdlib.h>
#include <stdio.h>
#include <cis_log.h>

int net_Init(void)
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(1,1), &wsaData))
        return 0;
    return 1;
}

void net_Uninit(void)
{
    WSACleanup();
}

int net_SetNonBlock(int socket, int enable)
{
    return ioctlsocket(socket, FIONBIO, (u_long*)(&enable)) == 0;
}

int net_Socket (int family, int socktype, int proto)
{
	int fd;

	fd = socket (family, socktype, proto);
	if (fd == -1)
	{
		LOGE( "cannot create socket, error no: %d.\n", net_errno);
		return -1;
	}
	
	net_SetNonBlock( fd, 1);
    return fd;
}
