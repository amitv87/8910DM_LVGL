/**
 * Copyright (c) 2017 China Mobile IOT.
 * All rights reserved.
 * Reference:
 *  tinydtls - https://sourceforge.net/projects/tinydtls/?source=navbar
**/

#include "peer.h"
#include "cis_internals.h"
#include "sockets.h"

static inline dtls_peer_t* dtls_malloc_peer()
{
    return (dtls_peer_t *)cis_malloc( sizeof(dtls_peer_t) );
}

void dtls_free_peer( dtls_peer_t *peer )
{
    dtls_handshake_free( peer->handshake_params );
    dtls_security_free( peer->security_params[0] );
    dtls_security_free( peer->security_params[1] );
	if(peer->session!=NULL)
		cis_free((void *)(peer->session));
    cis_free( peer );
}

dtls_peer_t* dtls_new_peer( const session_t *session )
{
    dtls_peer_t *peer;

    peer = dtls_malloc_peer();
    if ( peer )
    {
        cis_memset( peer,0,sizeof(dtls_peer_t) );
		peer->session=(session_t *)cis_malloc(sizeof(session_t ));
		if(!peer->session)
		{

            dtls_free_peer( peer );
            return NULL;
		}
		cis_memcpy((void *)(peer->session),(void *)session,sizeof(session_t ));
        peer->security_params[0] = dtls_security_new();

        if ( !peer->security_params[0] )
        {
            dtls_free_peer( peer );
            return NULL;
        }
    }

    return peer;
}