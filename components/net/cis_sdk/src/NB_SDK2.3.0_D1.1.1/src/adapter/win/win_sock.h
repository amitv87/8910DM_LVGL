#ifndef _NETWORK_IO_HEADER_
#define _NETWORK_IO_HEADER_


#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
//#include <stdint.h>
#include <cis_def.h>


/*#   define ENETUNREACH WSAENETUNREACH*/
#   define net_errno (WSAGetLastError())

#ifndef IN_LOOPBACK
#define IN_LOOPBACK(a) ((((long int) (a)) & 0xff000000) == 0x7f000000)
#endif

/* Microsoft: same semantic, same value, different name... go figure */
# define SHUT_RD SD_RECEIVE
# define SHUT_WR SD_SEND
# define SHUT_RDWR SD_BOTH
# define net_Close( fd ) closesocket ((SOCKET)fd)


/* Portable network names/addresses resolution layer */

/* GAI( GetAddressInfo) error codes */
# ifndef EAI_BADFLAGS
#  define EAI_BADFLAGS -1
# endif
# ifndef EAI_NONAME
#  define EAI_NONAME -2
# endif
# ifndef EAI_AGAIN
#  define EAI_AGAIN -3
# endif
# ifndef EAI_FAIL
#  define EAI_FAIL -4
# endif
# ifndef EAI_NODATA
#  define EAI_NODATA -5
# endif
# ifndef EAI_FAMILY
#  define EAI_FAMILY -6
# endif
# ifndef EAI_SOCKTYPE
#  define EAI_SOCKTYPE -7
# endif
# ifndef EAI_SERVICE
#  define EAI_SERVICE -8
# endif
# ifndef EAI_ADDRFAMILY
#  define EAI_ADDRFAMILY -9
# endif
# ifndef EAI_MEMORY
#  define EAI_MEMORY -10
# endif
#ifndef EAI_OVERFLOW
#  define EAI_OVERFLOW -11
#endif
# ifndef EAI_SYSTEM
#  define EAI_SYSTEM -12
# endif


# ifndef NI_MAXHOST
#  define NI_MAXHOST 1025
#  define NI_MAXSERV 32
# endif
# define NI_MAXNUMERICHOST 64

#ifndef AI_NUMERICSERV
# define AI_NUMERICSERV 0
#endif

int net_Init(void);

void net_Uninit(void);

int net_SetNonBlock(int socket, int enable);
int net_Socket (int family, int socktype, int proto);

//return the socket fd for the connection, <=0 for failed case
//time_out is at 1/1000 second unit
int net_Connect( const char *host, int port, 
	int socktype, int protocol, int time_out);

int net_getNameInfo( const struct sockaddr *sa, int salen,	
	char *host, int hostlen, int *portnum, int flags );

// ref doc: http://en.wikipedia.org/wiki/Getaddrinfo
int net_getAddrInfo( const char *node,	int i_port, 
	const struct addrinfo *p_hints, struct addrinfo **res );


static int net_setSockoptInt( int fd, int level, int optname, int optval)
{
    return setsockopt( fd, level, optname, (char*)&optval, sizeof( int));
}

static int net_setSockoptPtr( int fd, int level, int optname, void* optval, int optlen)
{
    return setsockopt( fd, level, optname, (char*)optval, optlen);}

static int net_getSockoptInt( int fd, int level, int optname, int* val)
{
	int len = sizeof( int);
    return getsockopt( fd ,level, optname, (char*)val, &len);
}

static int net_getSockoptPtr( int fd, int level, int optname, void* optval, int* optlen)
{
	int len = sizeof( int);
    return getsockopt( fd ,level, optname, (char*)optval, optlen);
}

//other interfaces
/*
connect select listen inet_pton inet_ntop bind accept and etc
*/

#define TX_EINTR                   WSAEINTR
#define TX_EAGAIN                  WSAEWOULDBLOCK
// Same as EAGAIN, just like many definitions under gnu and other system, so it's ok to use one of them
#define TX_EWOULDBLOCK             WSAEWOULDBLOCK   
#define TX_EINPROGRESS             WSAEINPROGRESS
#define TX_EALREADY                WSAEALREADY
#define TX_ENOTSOCK                WSAENOTSOCK
#define TX_EDESTADDRREQ            WSAEDESTADDRREQ
#define TX_EMSGSIZE                WSAEMSGSIZE
#define TX_EPROTOTYPE              WSAEPROTOTYPE
#define TX_ENOPROTOOPT             WSAENOPROTOOPT
#define TX_EPROTONOSUPPORT         WSAEPROTONOSUPPORT
#define TX_ESOCKTNOSUPPORT         WSAESOCKTNOSUPPORT
#define TX_EOPNOTSUPP              WSAEOPNOTSUPP
#define TX_EPFNOSUPPORT            WSAEPFNOSUPPORT
#define TX_EAFNOSUPPORT            WSAEAFNOSUPPORT
#define TX_EADDRINUSE              WSAEADDRINUSE
#define TX_EADDRNOTAVAIL           WSAEADDRNOTAVAIL
#define TX_ENETDOWN                WSAENETDOWN
#define TX_ENETUNREACH             WSAENETUNREACH
#define TX_ENETRESET               WSAENETRESET
#define TX_ECONNABORTED            WSAECONNABORTED
#define TX_ECONNRESET              WSAECONNRESET
#define TX_ENOBUFS                 WSAENOBUFS
#define TX_EISCONN                 WSAEISCONN
#define TX_ENOTCONN                WSAENOTCONN
#define TX_ESHUTDOWN               WSAESHUTDOWN
#define TX_ETOOMANYREFS            WSAETOOMANYREFS
#define TX_ETIMEDOUT               WSAETIMEDOUT
#define TX_ECONNREFUSED            WSAECONNREFUSED
#define TX_ELOOP                   WSAELOOP
#define TX_ENAMETOOLONG            WSAENAMETOOLONG
#define TX_EHOSTDOWN               WSAEHOSTDOWN
#define TX_EHOSTUNREACH            WSAEHOSTUNREACH
#define TX_ENOTEMPTY               WSAENOTEMPTY
#define TX_EPROCLIM                WSAEPROCLIM
#define TX_EUSERS                  WSAEUSERS
#define TX_EDQUOT                  WSAEDQUOT
#define TX_ESTALE                  WSAESTALE
#define TX_EREMOTE                 WSAEREMOTE



#endif /* _BASE_NETWORK_IO_HEADER_ */

