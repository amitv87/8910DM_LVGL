/**  
  @file
  ql_ftp_client.h

  @brief
  This file provides the definitions for ftp client, and declares the 
  API functions.

*/
/*============================================================================
  Copyright (c) 2017 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
 =============================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


WHEN        WHO            WHAT, WHERE, WHY
----------  ------------   ----------------------------------------------------

=============================================================================*/


#ifndef __QL_FTP_CLIENT_H__
#define __QL_FTP_CLIENT_H__

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif


#define MAX_FILE_NAME_SIZE		256
#define MAX_OWNER_SIZE		64
#define QL_FTP_CLIENT_FREE(p)   if(p){free(p);p=NULL;}

typedef enum
{
	QL_FTP_CLIENT_ERR_SUCCESS = 0,  //成功
	QL_FTP_CLIENT_ERR_INVALID_CLIENT, //不可用的http client句柄
	QL_FTP_CLIENT_ERR_INVALID_HOST, //不可用的请求主机地址
	QL_FTP_CLIENT_ERR_DNS_FAIL, //对服务器进行DNS解析失败
	QL_FTP_CLIENT_ERR_SOCK_CREATE_FAIL, //创建socket失败
	QL_FTP_CLIENT_ERR_SOCK_BIND_FAIL, //对socket进行bind操作失败
	QL_FTP_CLIENT_ERR_SOCK_CONN_FAIL, //执行socket连接失败
	QL_FTP_CLIENT_ERR_SOCK_SEND_FAIL, //发送数据失败
	QL_FTP_CLIENT_ERR_SOCK_RECV_FAIL, //接收数据失败
	QL_FTP_CLIENT_ERR_SOCK_CLOSE_FAIL, //断开连接失败
	QL_FTP_CLIENT_ERR_SOCK_DISCONNECTED, //连接断开
	QL_FTP_CLIENT_ERR_SSL_CONN_FAIL, // SSL 连接失败
	QL_FTP_CLIENT_ERR_RESP_TIMEOUT, //请求超时
	QL_FTP_CLIENT_ERR_CREATE_FILE_FAIL,//创建文件失败
	QL_FTP_CLIENT_ERR_NO_FILE,//文件不存在
	QL_FTP_CLIENT_ERR_NO_DIR,//目录不存在
	QL_FTP_CLIENT_ERR_UNKNOWN, //其他错误
} QL_FTP_CLIENT_ERR_E;

typedef enum {
    QL_FTP_CLIENT_SIM_CID,//设置SIM卡的CID
    QL_FTP_CLIENT_SSL_CTX,//设置FTP客户端使用的SSL Context
    QL_FTP_CLIENT_SSL_ENABLE,//设置FTP客户端是否使用SSL连接
	QL_FTP_CLIENT_OPT_PDP_CID, //设置ftp client使用的数据通道CID,执行数据拨号操作时使用的CID
	QL_FTP_CLIENT_OPT_SSL_CTX, //设置SSL 
	QL_FTP_CLIENT_OPT_START_POS,//设置下载的起始偏移位置
}QL_FTP_CLIENT_OPT_E;


typedef struct{
	char filename[MAX_FILE_NAME_SIZE];//文件或目录名
	int type;//类型，0：未知；1：目录；2：文件
	int permissions;//权限
	int links;//链接数
	char owner[MAX_OWNER_SIZE];//所属用户
	char group[MAX_OWNER_SIZE];//所属用户组
	struct tm timestamp;//创建时间
	unsigned long size;//文件大小，字节
}QL_FTP_CLIENT_FLIE_INFO_T;


/*****************************************************************
* Function: QL_FTP_CLIENT_WRITE_CB
*
* Description:
* 	使用回调函数接收从FTP服务器获取的文件数据。
* 
* Parameters:
* 	data	  		[in] 	接收到的数据。
* 	data_len  		[in] 	接收到的数据长度。
* 	user_data  		[in] 	用户数据
*   nmemb           [in]    接受到数据重复的次数
* Return:
* 	大于 0, 继续接收后面数据。
* 	0, 不再继续接收后面数据。
*
*****************************************************************/
typedef size_t (*QL_FTP_CLIENT_WRITE_CB_EX)(void *ptr, size_t size, size_t nmemb, void *stream);


/*****************************************************************
* Function: QL_FTP_CLIENT_READ_CB
*
* Description:
* 	使用回调函数发送数据到FTP服务器。
* 
* Parameters:
* 	data	  		[out] 	需要发送的数据。
* 	data_len  		[out] 	存储发送数据的最大长度。
* 	user_data  		[in] 	用户数据
*   nmemb           [in]    发送数据重复的次数
* Return:
* 	大于 0，实际需要发送的数据长度。
*  	0，没有需要发送的数据了。
*
*****************************************************************/
typedef size_t (*QL_FTP_CLIENT_READ_CB_EX)(void *ptr, size_t size, size_t nmemb, void *stream);



/*****************************************************************
* Function: ql_ftp_client_new
*
* Description:
* 	创建ftp client 句柄。
* 
* Parameters:
* 	无
*
* Return:
* 	不为 NULL, ftp client 句柄。
* 	NULL, 创建失败。
*
*****************************************************************/
void *ql_ftp_client_new(void);


/*****************************************************************
* Function: ql_ftp_client_release
*
* Description:
* 	释放ftp client资源。
* 
* Parameters:
* 	client	  		[in] 	ftp client 句柄。
*
* Return:
* 	0，成功。
*  	-1，失败。
*
*****************************************************************/
void ql_ftp_client_release(void *client);


/*****************************************************************
* Function: ql_ftp_client_setopt
*
* Description:
* 	配置ftp client相关属性。
* 
* Parameters:
* 	client	  		[in] 	ftp client 句柄。
* 	tag	  			[in] 	属性标签，见QL_FTP_CLIENT_OPT_E。
* 	...	  			[in] 	属性参数。
*							tag为QL_FTP_CLIENT_OPT_PDP_CID是，此参数为int型，执行数据拨号操作时使用的CID值。
*							tag为QL_FTP_CLIENT_OPT_SSL_CTX是，此参数为指针，SSL 句柄。
*
* Return:
* 	0，成功。
*  	-1，失败。
*
*****************************************************************/
int ql_ftp_client_setopt(void *client, QL_FTP_CLIENT_OPT_E tag, ...);


/*****************************************************************
* Function: ql_ftp_client_open
*
* Description:
* 	连接ftp服务器。
* 
* Parameters:
* 	client	  		[in] 	ftp client 句柄。
* 	hostname	  	[in] 	ftp 服务器地址。
* 	username	  	[in] 	用户名。
* 	password	  	[in] 	密码。
*
* Return:
* 	0，成功。
*  	-1，失败。
*
*****************************************************************/
int ql_ftp_client_open(void *client,char *hostname,char *username,char *password);


/*****************************************************************
* Function: ql_ftp_client_close
*
* Description:
* 	断开与ftp服务器的连接。
* 
* Parameters:
* 	client	  		[in] 	ftp client 句柄。
*
* Return:
* 	0，成功。
*  	-1，失败。
*
*****************************************************************/
int ql_ftp_client_close(void *client);


/*****************************************************************
* Function: ql_ftp_client_get
*
* Description:
* 	从ftp服务器下载文件。
* 
* Parameters:
* 	client	  		[in] 	ftp client 句柄。
* 	remotefile	  	[in] 	需要获取的ftp服务器上文件的文件名，可以包含相对或绝对路径。
* 	localfile	  	[in] 	存储到本地的文件名，可以包含相对或绝对路径，如果为NULL，则和ftp服务器上文件同名，本地默认使用的目录为U:/。
* 	write_cb	  	[in] 	使用回调函数接收从FTP服务器获取的文件数据。
* 	user_data	  	[in] 	用户数据。
*
* Return:
* 	0，成功。
*  	-1，失败。
*
*****************************************************************/
int ql_ftp_client_get_ex(void *client,char *remotefile,char *localfile,QL_FTP_CLIENT_WRITE_CB_EX write_cb,void* user_data);
/*****************************************************************
* Function: ql_ftp_client_put
*
* Description:
* 	上传本地文件到服务器。
* 
* Parameters:
* 	client	  		[in] 	ftp client 句柄。
* 	localfile	  	[in] 	需要上传的本地文件名，可以包含相对或绝对路径，本地默认使用的目录为U:/。
* 	remotefile	  	[in] 	存储到ftp服务器上文件的文件名，可以包含相对或绝对路径，如果为NULL，则和本地文件同名，
* 	read_cb	  		[in] 	使用回调函数发送数据到FTP服务器，如果设置了此回调函数，localfile参数无效，remotefile参数必须设置。
* 	user_data	  	[in] 	用户数据。
*
* Return:
* 	0，成功。
*  	-1，失败。
*
*****************************************************************/
int ql_ftp_client_put_ex(void *client,char *localfile,char *remotefile,QL_FTP_CLIENT_READ_CB_EX read_cb,void* user_data);


/*****************************************************************
* Function: ql_ftp_client_delete
*
* Description:
* 	删除ftp服务器上文件。
* 
* Parameters:
* 	client	  		[in] 	ftp client 句柄。
* 	remotefile	  	[in] 	需要删除的ftp服务器上文件的文件名，可以包含相对或绝对路径，
*
* Return:
* 	0，成功。
*  	-1，失败。
*
*****************************************************************/
int ql_ftp_client_delete(void *client,char *remotefile);


/*****************************************************************
* Function: ql_ftp_client_cwd
*
* Description:
* 	更改ftp服务器当前使用的目录。
* 
* Parameters:
* 	client	  		[in] 	ftp client 句柄。
* 	path	  		[in] 	ftp服务器目录，可以是相对或绝对路径，
*
* Return:
* 	0，成功。
*  	-1，失败。
*
*****************************************************************/
int ql_ftp_client_cwd(void *client, char *path);


/*****************************************************************
* Function: ql_ftp_client_lcwd
*
* Description:
* 	更改本地当前使用的目录。
* 
* Parameters:
* 	client	  		[in] 	ftp client 句柄。
* 	path	  		[in] 	本地目录，可以是相对或绝对路径，
*
* Return:
* 	0，成功。
*  	-1，失败。
*
*****************************************************************/
int ql_ftp_client_lcwd(void *client, char *path);


/*****************************************************************
* Function: ql_ftp_client_pwd
*
* Description:
* 	获取ftp服务器当前使用的目录。
* 
* Parameters:
* 	client	  		[in] 	ftp client 句柄。
* 	path	  		[out] 	ftp服务器目录，绝对路径。
*	path_len		[in]	path的最大存储长度。
*
* Return:
* 	0，成功。
*  	-1，失败。
*
*****************************************************************/
int ql_ftp_client_pwd(void *client, char *path, int path_len);


/*****************************************************************
* Function: ql_ftp_client_lpwd
*
* Description:
* 	获取本地当前使用的目录。
* 
* Parameters:
* 	client	  		[in] 	ftp client 句柄。
* 	path	  		[out] 	本地目录，绝对路径。
*	path_len		[in]	path的最大存储长度。
*
* Return:
* 	0，成功。
*  	-1，失败。
*
*****************************************************************/
int ql_ftp_client_lpwd(void *client, char *path, int path_len);


/*****************************************************************
* Function: ql_ftp_client_mkdir
*
* Description:
* 	在ftp服务器上新建目录。
* 
* Parameters:
* 	client	  		[in] 	ftp client 句柄。
* 	path	  		[in] 	需要新建的ftp服务器目录，可以是相对或绝对路径。
*
* Return:
* 	0，成功。
*  	-1，失败。
*
*****************************************************************/
int ql_ftp_client_mkdir(void *client, char *path);


/*****************************************************************
* Function: ql_ftp_client_rmdir
*
* Description:
* 	删除ftp服务器上的目录。
* 
* Parameters:
* 	client	  		[in] 	ftp client 句柄。
* 	path	  		[in] 	需要删除的ftp服务器目录，可以是相对或绝对路径。
*
* Return:
* 	0，成功。
*  	-1，失败。
*
*****************************************************************/
int ql_ftp_client_rmdir(void *client, char *path);


/*****************************************************************
* Function: ql_ftp_client_list
*
* Description:
* 	获取指定ftp服务器上目录下文件和目录列表。
* 
* Parameters:
* 	client	  		[in] 	ftp client 句柄。
* 	path_or_file	[in] 	ftp服务器上的目录或文件，可以是相对或绝对路径。
* 	match			[in] 	只列出包含match字符的结果。
* 	file_info_list	[out] 	获取到的文件和目录列表。
* 	max_file_num	[in] 	file_info_list可以存储结果的最大个数。
*
* Return:
* 	大于0，实际获取到的结果个数。
*  	-1，失败。
*
*****************************************************************/
int ql_ftp_client_list(void *client, char *path_or_file,char *match,QL_FTP_CLIENT_FLIE_INFO_T *file_info_list, int max_file_num);


/*****************************************************************
* Function: ql_ftp_client_get_error_code
*
* Description:
* 	获取当前函数执行的错误码。
* 
* Parameters:
* 	client	  		[in] 	ftp client 句柄。
*
* Return:
* 	大于100，为ftp服务器返回的响应码。
*  	小于100，为函数执行的错误码，见QL_FTP_CLIENT_ERR_E。
*
*****************************************************************/
int ql_ftp_client_get_error_code(void *client);

#ifdef __cplusplus
}
#endif

#endif
