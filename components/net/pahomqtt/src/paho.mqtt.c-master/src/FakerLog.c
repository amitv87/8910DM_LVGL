/*******************************************************************************
 * Copyright (c) 2009, 2018 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution. 
 *
 * The Eclipse Public License is available at 
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at 
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial API and implementation and/or initial documentation
 *    Ian Craggs - updates for the async client
 *    Ian Craggs - fix for bug #427028
 *******************************************************************************/

/**
 * @file
 * \brief Logging and tracing module
 *
 * 
 */

#include "Log.h"
#include "MQTTPacket.h"
#include "MQTTProtocol.h"
#include "MQTTProtocolClient.h"
//#include "Messages.h"
#include "LinkedList.h"
#include "StackTrace.h"
#include "Thread.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>

//#if !defined(WIN32) && !defined(WIN64)
//#include <syslog.h>
//#include <sys/stat.h>
//#define GETTIMEOFDAY 1
//#else
//#define snprintf _snprintf
//#endif

//#if defined(GETTIMEOFDAY)
//	#include <sys/time.h>
//#else
//	#include <sys/timeb.h>
//#endif

//#if !defined(WIN32) && !defined(WIN64)
/**
 * _unlink mapping for linux
 */
//#define _unlink unlink
//#endif


//#if !defined(min)
//#define min(A,B) ( (A) < (B) ? (A):(B))
//#endif

trace_settings_type trace_settings =
{
	TRACE_MINIMUM,
	400,
	INVALID_LEVEL
};

typedef struct
{
//#if defined(GETTIMEOFDAY)
//        struct timeval ts;
//#else
//        struct timeb ts;
//#endif
//        int sametime_count;
//        int number;
//        int thread_id;
//        int depth;
//        char name[256 + 1];
//        int line;
//        int has_rc;
//        int rc;
//        enum LOG_LEVELS level;
} traceEntry;

int Log_initialize(Log_nameValue* info)
{
	int rc = 0;	
	return rc;
}


void Log_setTraceCallback(Log_traceCallback* callback)
{
	return ;
}


void Log_setTraceLevel(enum LOG_LEVELS level)
{
	return ;
}


void Log_terminate(void)
{
	return ;
}

#if 0
static traceEntry* Log_pretrace(void)
{
	return NULL;
}

static char* Log_formatTraceEntry(traceEntry* cur_entry)
{
	return NULL;
}


static void Log_output(enum LOG_LEVELS log_level, const char *msg)
{
	return ;
}


static void Log_posttrace(enum LOG_LEVELS log_level, traceEntry* cur_entry)
{
	return ;
}


static void Log_trace(enum LOG_LEVELS log_level, const char *buf)
{
	return ;
}
#endif

/**
 * Log a message.  If possible, all messages should be indexed by message number, and
 * the use of the format string should be minimized or negated altogether.  If format is
 * provided, the message number is only used as a message label.
 * @param log_level the log level of the message
 * @param msgno the id of the message to use if the format string is NULL
 * @param aFormat the printf format string to be used if the message id does not exist
 * @param ... the printf inserts
 */
void Log(enum LOG_LEVELS log_level, int msgno, const char *format, ...)
{
    return ;
}


/**
 * The reason for this function is to make trace logging as fast as possible so that the
 * function exit/entry history can be captured by default without unduly impacting
 * performance.  Therefore it must do as little as possible.
 * @param log_level the log level of the message
 * @param msgno the id of the message to use if the format string is NULL
 * @param aFormat the printf format string to be used if the message id does not exist
 * @param ... the printf inserts
 */
void Log_stackTrace(enum LOG_LEVELS log_level, int msgno, int thread_id, int current_depth, const char* name, int line, int* rc)
{
	return;
}



