/**
 ******************************************************************************
 * @file    quec_debug.h
 * @author  Chavis.Chen
 * @version V1.0.0
 * @date    26-Sep-2018
 * @brief   This file contains the common definitions, macros and functions to
 *          be shared throughout the project.
 ******************************************************************************
 *
 *  The MIT License
 *  Copyright (c) 2014 QUECTEL Inc.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is furnished
 *  to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 *  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ******************************************************************************
 */

#ifndef _QUEC_DEBUG_H
#define _QUEC_DEBUG_H

#include "ql_log.h"
#define debug_log(msg, ...)	QL_LOG_PUSH("quec_debug", msg, ##__VA_ARGS__)


/*> debug print assertion definition	*/

#if !defined(unlikely)
	#define unlikely(expression)				!!(expression)
#endif

/*---------------------------------*/

#if !defined(debug_print_error)
	#define debug_print_error(str) 				do {debug_log("**ERROR** %s""", (str != NULL) ? str : "" );}while(0==1)
#endif

#if !defined(debug_print_assert)
	#define debug_print_assert(str) 			do {debug_log("**ASSERT** %s""", (str != NULL) ? str : "" );}while(0==1)
#endif


/*---------------------------------*/

#if !defined(require)
	#define require(x, lable)																					\
			do                                                                                                  \
			{                                                                                                   \
				if( unlikely( !(x) ) )                                                                          \
				{                                                                                               \
					debug_print_error(NULL);               														\
					goto lable;                                                                                 \
				}                                                                                               \
			} while( 1==0 )
#endif

/*---------------------------------*/

#if !defined(require_quiet)
	#define require_quiet(x, lable)																				\
			do                                                                                                  \
			{                                                                                                   \
				if( unlikely( !(x) ) )                                                                          \
				{                                                                                               \
					goto lable;                                                                                 \
				}                                                                                               \
			} while( 1==0 )
#endif

/*---------------------------------*/

#if !defined(require_string)
	#define require_string(x, lable, str)																		\
			do                                                                                                  \
			{                                                                                                   \
				if( unlikely( !(x) ) )                                                                          \
				{                                                                                               \
					debug_print_error(str);																		\
					goto lable;                                                                                 \
				}                                                                                               \
			} while( 1==0 )
#endif

/*---------------------------------*/

#if !defined(require_action)
	#define require_action(x, lable, action)																	\
			do                                                                                                  \
			{                                                                                                   \
				if( unlikely( !(x) ) )                                                                          \
				{                                                                                               \
					debug_print_error(NULL);																	\
					{action;}																					\
					goto lable;                                                                                 \
				}                                                                                               \
			} while( 1==0 )
#endif

/*---------------------------------*/

#if !defined(require_action_quiet)
	#define require_action_quiet(x, lable, action)																\
			do                                                                                                  \
			{                                                                                                   \
				if( unlikely( !(x) ) )                                                                          \
				{                                                                                               \
					{action;}																					\
					goto lable;                                                                                 \
				}                                                                                               \
			} while( 1==0 )
#endif


/*---------------------------------*/

#if !defined(require_action_string)
	#define require_action_string(x, lable, action, str)														\
			do                                                                                                  \
			{                                                                                                   \
				if( unlikely( !(x) ) )                                                                          \
				{                                                                                               \
					debug_print_error(str);																		\
					{action;}																					\
					goto lable;                                                                                 \
				}                                                                                               \
			} while( 1==0 )
#endif

/*---------------------------------*/

#if !defined(require_noerr)
	#define require_noerr(err, lable)																			\
			do                                                                                                  \
			{                                                                                                   \
				QuecOSStatus localErr;																			\
				localErr = (QuecOSStatus)(err);																	\
				if( unlikely( localErr ) )                                                                      \
				{                                                                                               \
					debug_print_error(NULL);               														\
					goto lable;                                                                                 \
				}                                                                                               \
			} while( 1==0 )
#endif

/*---------------------------------*/

#if !defined(require_noerr_string)
	#define require_noerr_string(err, lable, str)																\
			do                                                                                                  \
			{                                                                                                   \
				QuecOSStatus localErr;																			\
				localErr = (QuecOSStatus)(err);																	\
				if( unlikely( localErr ) )                                                                      \
				{                                                                                               \
					debug_print_error(str);    	           														\
					goto lable;                                                                                 \
				}                                                                                               \
			} while( 1==0 )
#endif

/*---------------------------------*/

#if !defined(require_noerr_quiet)
	#define require_noerr_quiet(err, lable)																		\
			do                                                                                                  \
			{                                                                                                   \
				QuecOSStatus localErr;																			\
				localErr = (QuecOSStatus)(err);																	\
				if( unlikely( localErr ) )                                                                      \
				{                                                                                               \
					goto lable;                                                                                 \
				}                                                                                               \
			} while( 1==0 )
#endif

/*---------------------------------*/

#if !defined(require_noerr_action)
	#define require_noerr_action(err, lable, action)															\
			do                                                                                                  \
			{                                                                                                   \
				QuecOSStatus localErr;																			\
				localErr = (QuecOSStatus)(err);																	\
				if( unlikely( localErr ) )                                                                     	\
				{                                                                                               \
					debug_print_error(NULL);               														\
					{action;}																					\
					goto lable;                                                                                 \
				}                                                                                               \
			} while( 1==0 )
#endif

/*---------------------------------*/

#if !defined(require_noerr_action_quiet)
	#define require_noerr_action_quiet(err, lable, action)														\
			do                                                                                                  \
			{                                                                                                   \
				QuecOSStatus localErr;																			\
				localErr = (QuecOSStatus)(err);																	\
				if( unlikely( localErr ) )                                                                     	\
				{                                                                                               \
					{action;}																					\
					goto lable;                                                                                 \
				}                                                                                               \
			} while( 1==0 )
#endif

/*---------------------------------*/

#if !defined(require_noerr_action_string)
	#define require_noerr_action_string(err, lable, action, str)												\
			do                                                                                                  \
			{                                                                                                   \
				QuecOSStatus localErr;																			\
				localErr = (QuecOSStatus)(err);																	\
				if( unlikely( localErr ) )                                                                     	\
				{                                                                                               \
					debug_print_error(str); 	              													\
					{action;}																					\
					goto lable;                                                                                 \
				}                                                                                               \
			} while( 1==0 )
#endif


#endif

