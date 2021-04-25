#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>
#include <mupnp/net/socket.h>
#include <mupnp/net/interface.h>
#include <mupnp/util/time.h>
/*
#include <mupnp/ssdp/ssdp.h>
*/

#include <mupnp/util/log.h>

#include <string.h>

#if !defined(WINCE)
#include <errno.h>
#endif

int mupnp_mbedtlssocket_connect(mUpnpSocket *sock, const char *addr, int port);

#if defined(MUPNP_USE_OPENSSL)

/**** HTTPS certificate type ****/
#define CG_HTTPS_CA_CRT 0
#define CG_HTTPS_CLIENT_CRT 1
#define CG_HTTPS_CLIENT_PRIVATE_KEY 2

#define CG_HTTPS_NO_CERT MBEDTLS_SSL_VERIFY_NONE
#define CG_HTTPS_CERT MBEDTLS_SSL_VERIFY_REQUIRED
#define CG_HTTPS_VERIFY_OPTIONAL MBEDTLS_SSL_VERIFY_OPTIONAL

#define CG_HTTPS_NEED_CERT CG_HTTPS_VERIFY_OPTIONAL

#endif
