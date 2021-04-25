/* Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
 * All rights reserved.
 *
 * This software is supplied "AS IS" without any warranties.
 * RDA assumes no responsibility or liability for the use of the software,
 * conveys no license or title under any patent, copyright, or mask work
 * right to the product. RDA reserves the right to make changes in the
 * software without notification.  RDA also make no representation or
 * warranty that such application will be suitable for the specified use
 * without further testing or modification.
 */

#ifndef _FTP_PROTOCOL_H_
#define _FTP_PROTOCOL_H_

#ifdef __cplusplus
extern "C" {
#endif
#define FTP_STRING_SIZE 256

#define FTP_RET_SUCCESS 0
#define FTP_RET_API_ERR -1
#define FTP_RET_FTP_NOT_INITED -2
#define FTP_RET_FTP_NOT_CONNECTED -3
#define FTP_RET_FTP_HAD_CONNECTED -4
#define FTP_RET_FTP_IS_BUSY -5

#define FTP_RET_CMD_ERR -1000
#define FTP_RET_CMD_CONNECT_FAIL (FTP_RET_CMD_ERR - 1)  //ftp command tcp connect failed
#define FTP_RET_CMD_TIMEOUT (FTP_RET_CMD_ERR - 2)       //ftp response timeout
#define FTP_RET_CMD_REP_ERR (FTP_RET_CMD_ERR - 3)       //receive incorrect response code
#define FTP_RET_CMD_BROKEN (FTP_RET_CMD_ERR - 4)        //tcp connection is broken
#define FTP_RET_CMD_DNS_ERR (FTP_RET_CMD_ERR - 5)       //dns resolve failure
#define FTP_RET_CMD_USER_PASS_ERR (FTP_RET_CMD_ERR - 6) //receive incorrect response code

#define FTP_RET_DATA_ERR -2000
#define FTP_RET_DATA_CONNECT_FAIL (FTP_RET_DATA_ERR - 1) //ftp data tcp connect failed
#define FTP_RET_DATA_TIMEOUT (FTP_RET_DATA_ERR - 2)      //ftp data timeout
#define FTP_RET_DATA_BROKEN (FTP_RET_DATA_ERR - 3)       //data connection is broken
#define FTP_RET_DATA_DNS_ERR (FTP_RET_DATA_ERR - 4)      //dns resolve failure

#define FTP_RET_INTERNAL_ERR -3000

typedef uint32_t ftp_ret_t;

/* ftp async message */
typedef enum _ftp_msg_t
{
    FTP_MSG_BASE = 0x1100,
    FTP_MSG_CONNECT,
    FTP_MSG_DISCONNECT,
    FTP_MSG_LOGIN,
    FTP_MSG_LOGOUT,
    FTP_MSG_SIZE,
    FTP_MSG_GET,
    FTP_MSG_PUT_START,
    FTP_MSG_PUT_STOP,
    FTP_MSG_CONNECT_DATA,
    FTP_MSG_DISCONNECT_DATA,
    FTP_MSG_TRANSFER_DATA,
    FTP_MSG_UNKOWN,
} ftp_msg_t;

static const char *const ftp_msg_names[] = {
    "BASE",
    "CONNECT",
    "DISCONNECT",
    "LOGIN",
    "LOGOUT",
    "SIZE",
    "GET",
    "PUT_START",
    "PUT_STOP",
    "CONNECT_DATA",
    "DISCONNECT_DATA",
    "TRANSFER_DATA",
    "UNKOWN"};

typedef void (*ftplib_cb_t)(uint32_t nEventId, uint32_t nResult, uint32_t nParam1, uint32_t nParam2);

typedef enum _ftp_api_t
{
    FTP_API_ASYNC = 0,
    FTP_API_SYNC,
} ftp_api_t;

typedef enum _ftp_mode_t
{
    FTP_ACTIVE = 0,
    FTP_PASSIVE,
} ftp_mode_t;

typedef enum _ftp_transfer_t
{
    FTP_ASCII = 0,
    FTP_BINARY,
} ftp_transfert_t;

typedef struct _ftpconfig_t
{
    ftp_api_t apiType;    // 0 async, 1 sync
    bool encrypto;        // ssl or not
    ftp_mode_t mode;      // 0 means active,  1 passive
    ftp_transfert_t type; // 0 means ascii,   1 binary
    ftplib_cb_t callback;
} ftpconfig_t;

typedef struct _ftp_loginopt_t
{
    char host[FTP_STRING_SIZE];
    char port[FTP_STRING_SIZE];
    char username[FTP_STRING_SIZE];
    char passwd[FTP_STRING_SIZE];
} ftp_loginopt_t;

bool FTPLib_init(ftpconfig_t *cfg);
bool FTPLib_uninit(void);
ftp_ret_t FTPLib_login(char *host, char *port, char *user, char *pass);
ftp_ret_t FTPLib_logout();
ftp_ret_t FTPLib_size(char *file, uint32_t *size);
ftp_ret_t FTPLib_getStart(char *file, uint32_t offset, uint32_t size);
int32_t FTPLib_get(uint8_t *buf, uint32_t buflen);
ftp_ret_t FTPLib_getStop();
ftp_ret_t FTPLib_putStart(char *file);
ftp_ret_t FTPLib_put(uint8_t *buf, uint32_t buflen);
ftp_ret_t FTPLib_putStop();

#ifdef __cplusplus
}
#endif

#endif
