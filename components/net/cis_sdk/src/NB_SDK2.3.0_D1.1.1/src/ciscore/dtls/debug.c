/**
 * Copyright (c) 2017 China Mobile IOT.
 * All rights reserved.
 * Reference:
 *  tinydtls - https://sourceforge.net/projects/tinydtls/?source=navbar
**/
#include "cis_def.h"
#include "debug.h"

#if CIS_DTLS_LOG
void dsrv_hexdump_log( const char          *name,
                       const unsigned char *buf,
                       size_t               length,
                       int                  extend )
{
    int n = 0;

    if ( extend )
    {
        LOG_PRINT( "%s: (%d bytes):\n", name, length );

        while ( length-- )
        {
            if ( n % 16 == 0 )
                LOG_PRINT( "%08X ", n );

            LOG_PRINT( "%02X ", *buf++ );

            n++;
            if ( n % 8 == 0 )
            {
                if ( n % 16 == 0 )
                    LOG_PRINT( "\n" );
                else
                    LOG_PRINT( " " );
            }
        }
    }
    else
    {
        LOG_PRINT( "%s: (%u bytes): ", name, length );
        while ( length-- )
            LOG_PRINT( "%02X", *buf++ );
    }
    LOG_PRINT( "\n" );
}
#endif