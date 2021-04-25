/*
* Copyright (c) 2006-2016 RDA Microelectronics, Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
/***********************************************************************
 *
 * MODULE NAME:    platform_config.h
 * DESCRIPTION:    Host Side Stack Win32 specific configuration options
 * AUTHOR:         Martin Mellody
 *
 * SOURCE CONTROL: $Id: platform_config.h,v 1.1 2008/01/03 06:55:12 tianwq Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2000-2002 RDA Microelectronics.
 *     All rights reserved.
 *
 ***********************************************************************/

#ifndef PLATFORM_CONFIG_H
#define PLATFORM_CONFIG_H

/****************** Features ***************/
#ifdef PLATFORM_ANTISW3
#define RDABT_SSP_SUPPORT 1 //close by dch
#else
#define RDABT_SSP_SUPPORT 1
#endif
#define RDABT_EIR_SUPPORT 1
#ifdef CHIP_SUPPORT_LE
#define RDABT_LE_SUPPORT 1
#define RDABT_INQUIRY_SUPPORT 1
#else
#ifdef WIN32
#define RDABT_LE_SUPPORT 1
#define ENCRYPTION_SOFTWATE_ENCODE_SUPPORT 1
#else

#define RDABT_LE_SUPPORT 1
#define RDABT_INQUIRY_SUPPORT 1
#endif
#endif

#define RDABT_AT_PBAP_SUPPORT 0

#define RDABT_OOB_SUPPORT 0

/* for bt 3.0 */
#define AMP_MANAGER_SUPPORT 0
#define PAL_IN_HOST 0

#define RDABT_THREAD_EVENT_SUPPORT 0 // event for call bt profile from other thread

#ifndef COMBINED_HOST
#define COMBINED_HOST 0
#endif

#if COMBINED_HOST == 0 && CHIP_HAS_BTCPU == 0
#define RDABT_DHKEY_SUPPORT 0 //MOCOR not use
#else
#define RDABT_DHKEY_SUPPORT 0
#endif

#ifndef MEMORY_POOL_SUPPORT
#ifndef WIN32
#if CHIP_HAS_BTCPU == 1
#define MEMORY_POOL_SUPPORT 1 // stack internal memory manager
#else
#define MEMORY_POOL_SUPPORT 0 // stack internal memory manager
#endif
#else
#define MEMORY_POOL_SUPPORT 0
#endif
#endif

//#define BT_MSG_EXTNAL_FLAG  0X8000

/******************* Manager ****************/
/* The local and remote names are truncated to the defines below - see GAP spec. */
#define MGR_MAX_REMOTE_NAME_LEN 32
#define MGR_MAX_LOCAL_NAME_LEN 32

/****************** L2CAP *******************/
#define L2CAP_CONFIG_VALUES_BELOW_HCI 1
#ifdef WIN32 //Simulator build error
#define L2CAP_FRAGMENT_ACL_SUPPORT 1
#else
#define L2CAP_FRAGMENT_ACL_SUPPORT 1
#endif
#define L2CAP_RETRANS_SUPPORT 0

/*************** Test and Debug *************/
/* Debug option */
#ifdef WIN32
#define pDEBUG 1
#else
#ifndef CT_NO_DEBUG
#define pDEBUG 1
#else
#define pDEBUG 0
#endif
#endif
#define NO_SEC_CHECK

#if pDEBUG
#define pDEBUGLEVEL pLOGNOTICE
#define pDEBUGLAYERS pLOGALWAYS
#endif

/*************************************** platform sepcific defines ************************************************/

#define PLATFORMSTRING "RDA5856E2"

/* define endian of platform */

#define BTHOST_BIGENDIAN 0

#endif /* PLATFORM_CONFIG_H */
