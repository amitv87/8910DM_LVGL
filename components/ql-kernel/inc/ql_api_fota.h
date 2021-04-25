/**  
  @file
  ql_api_fota.h

  @brief
  quectel fota interface header definition.

*/
/*================================================================
  Copyright (c) 2020 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
=================================================================*/
/*=================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN              WHO         WHAT, WHERE, WHY
------------     -------     -------------------------------------------------------------------------------

=================================================================*/


#ifndef QL_API_FOTA_H
#define QL_API_FOTA_H


#include "ql_api_common.h"

#ifdef __cplusplus
    extern "C" {
#endif

/*========================================================================
 *  Macro Definition
 *========================================================================*/
#define QL_FOTA_ERRCODE_BASE (QL_COMPONENT_FOTA<<16)
#define QUEC_FOAT_PACK_NAME_CFG  QUEC_CONFIG_DIR_ROOT"/quec_fota_packname.nv"
#define QL_FOTA_PACK_NAME_MAX_LEN         (172)
/*========================================================================
 *  Enumeration Definition
 *========================================================================*/
//fota component detail errcode, 4 bytes
typedef enum
{
    QL_FOTA_SUCCESS = 0,
    QL_FOTA_EXECUTE_ERR                         = 502|QL_FOTA_ERRCODE_BASE,//异常操作
    QL_FOTA_IMAGE_VERIFY_READY_ERR              = 510|QL_FOTA_ERRCODE_BASE,//fota更新包校验设置准备状态失败
    QL_FOTA_IMAGE_VERIFY_ERR                    = 547|QL_FOTA_ERRCODE_BASE,//fota更新包校验不设置准备状态失败
    
    QL_FOTA_MEM_ADDR_NULL_ERR,                              //操作空指针错误。
    QL_FOTA_INVALID_PARAM_ERR,                              //输入参数无效
    QL_FOTA_BUSY_ERR,//550                                  fota总线繁忙
    QL_FOTA_SEMAPHORE_CREATE_ERR,                           //信号量创建失败
    QL_FOTA_SEMAPHORE_TIMEOUT_ERR,                          //信号量超时
    QL_FOTA_FLAG_SET_ERR,                                   //设置FOTA标志失败
    QL_FOTA_PACKFILE_PATH_INVALID,                          //输入的文件路径是无效的错误。
    QL_FOTA_PACK_NAME_RESET_FAILED,                       //重置fota  更新包的路径信息，使其恢复为默认值失败。
    QL_FOTA_PACK_NAME_SAVE_FAILED,                        //保存fota  更新包的路径信息到配置文件失败
    QL_FOTA_PACK_NAME_DELETE_FAILED,                      //删除fota更新包的路径信息的配置文件失败。
    QL_FOTA_PACK_NAME_LOAD_FAILED,//560                     读取fota更新包的路径信息的配置文件失败。
    QL_FOTA_PACK_NAME_LENGTH_FAILED,                      //保存 fota pack name 空间出错。 
    
} ql_errcode_fota_e;

typedef enum
{
    QL_FOTA_FINISHED = 0,   // FOTA 升级操作完成
    QL_FOTA_NOT_EXIST,      // 没有fota更新包，这不是一个错误状态，需要下载fota更新包
    QL_FOTA_READY,          // fota更新包已经被设置准备状态，等待更新
    QL_FOTA_STATUS_INVALID, // FOTA状态是无效的
} ql_fota_result_e;


/*========================================================================
 *  Variable Definition
 *========================================================================*/

/*========================================================================
 *  function Definition
 *========================================================================*/
/*****************************************************************
* Function: ql_fota_get_pack_name
*
* Description:
* 	获得获得当前fota更新包的路径信息。
* Parameters:
*   p_pac_file_name  [in]   存放当前fota更新包路径空间的首地址      
*   length           [in]   空间的大小
* Return:
*   QL_FOTA_MEM_ADDR_NULL_ERR, 操作空指针错误。
*   QL_FOTA_PACK_NAME_LENGTH_FAILED, 保存 fota pack name 空间出错。
*   QL_FOTA_PACKFILE_PATH_INVALID, 输入的文件路径是无效的错误。
*   QL_FOTA_SUCCESS，操作成功。
*
*****************************************************************/
ql_errcode_fota_e ql_fota_get_pack_name(char *p_pac_file_name,int* length);
/*****************************************************************
* Function: ql_fota_image_verify
*
* Description:
* 	验证fota文件，如果有效，将fota标志设置为ready。如果无效，函数应该返回QL_FOTA_IMAGE_VERIFY_READY_ERR。检查文件要花些时间。
* Parameters:
*       PackFileName    [in]   设置fota更新包路径字符串的首地址 
* Return:
*   QL_FOTA_PACKFILE_PATH_INVALID, 输入的文件路径是无效的错误。
*   QL_FOTA_PACK_NAME_LENGTH_FAILED, 保存 fota pack name 空间出错
*   QL_FOTA_IMAGE_VERIFY_READY_ERR, fota更新包校验并设置准备状态失败。
*   QL_FOTA_PACK_NAME_SAVE_FAILED,保存fota  更新包的路径信息到配置文件失败
*   QL_FOTA_SUCCESS，操作成功。
*
*****************************************************************/
ql_errcode_fota_e ql_fota_image_verify(char* PackFileName);
/*****************************************************************
* Function: ql_fota_image_verify_without_setflag
*
* Description:
* 	彻底更新fota文件内容，如果无效，函数应该返回QL_FOTA_IMAGE_VERIFY_ERR。更新文件要花些时间。建议直接调用ql_fota_image_verify函数
* Parameters:
*       PackFileName    [in]   设置fota更新包路径字符串的首地址 
* Return:
*   QL_FOTA_PACKFILE_PATH_INVALID, 输入的文件路径是无效的错误。
*   QL_FOTA_PACK_NAME_LENGTH_FAILED, 保存 fota pack name 空间出错
*   QL_FOTA_IMAGE_VERIFY_ERR, fota更新包校验不设置准备状态失败
*   QL_FOTA_PACK_NAME_SAVE_FAILED,保存fota  更新包的路径信息到配置文件失败
*   QL_FOTA_SUCCESS，操作成功。
*
*****************************************************************/
ql_errcode_fota_e ql_fota_image_verify_without_setflag(char* PackFileName);
/*****************************************************************
* Function: ql_fota_file_reset
*
* Description:
* 	清除fota文件。当你将del_image设置为true时，你将删除fota图像文件，无论如何函数应该返回成功。
* Parameters:
*       del_image       [in]   del_image设置为true时，将删除fota图像文件,否则不删除 
* Return:
*   QL_FOTA_PACK_NAME_DELETE_FAILED, 删除fota更新包的路径信息的配置文件失败。
*   QL_FOTA_SUCCESS，操作成功。
*
*****************************************************************/
ql_errcode_fota_e ql_fota_file_reset(bool del_image);
/*****************************************************************
* Function: ql_fota_get_result
*
* Description:
* 	通过param p_fota_result得到fota结果输出。如果fota结果为“QL_FOTA_FINISHED”，则该功能将自动删除所有关于fota的文件。如果p_fota_result不为NULL，函数应该返回成功
* Parameters:
*       p_fota_result       [in]   获取fota升级之后的完成状态。
* Return:
*   QL_FOTA_MEM_ADDR_NULL_ERR, 操作空指针错误。
*   QL_FOTA_SUCCESS，操作成功。
*
*****************************************************************/
ql_errcode_fota_e ql_fota_get_result(ql_fota_result_e *p_fota_result);
/*****************************************************************
* Function: ql_fota_pac_name_convert_fs
*
* Description:
*   将FOTA模块的使用的文件路径信息转化为模块文件系统使用的文件路径信息。
* Parameters:
*   fs_path         [in]   文件系统使用的文件路径信息。   
*   fota_path       [in]   FOTA模块的使用的文件路径信息。
*   fs_path_length  [in]   fs_path的空间大小，当返回QL_FOTA_SUCCESS时，表示转化后的fota更新包路径信息的长度。
* Return:
*   QL_FOTA_MEM_ADDR_NULL_ERR, 操作空指针错误。 
*   QL_FOTA_PACKFILE_PATH_INVALID,输入的文件路径是无效的错误。
*   QL_FOTA_PACK_NAME_LENGTH_FAILED，保存 fota pack name 空间出错。
*   QL_FOTA_SUCCESS, 操作成功。
*
*****************************************************************/
ql_errcode_fota_e ql_fota_pack_name_convert_fs(const char *fota_path,char* fs_path,int *fs_path_length);
/*****************************************************************
* Function: ql_fs_convert_fota_pac_name
*
* Description:
*   将模块文件系统使用的文件路径信息转化为FOTA模块的使用的文件路径信息。
* Parameters:
*   fs_path         [in]   文件系统使用的文件路径信息。   
*   fota_path       [in]   FOTA模块的使用的文件路径信息。
*   fs_path_length  [in]   fota_path的空间大小，当返回QL_FOTA_SUCCESS时，表示转化后的fota更新包路径信息的长度。
* Return:
*   QL_FOTA_MEM_ADDR_NULL_ERR, 操作空指针错误。 
*   QL_FOTA_PACKFILE_PATH_INVALID,输入的文件路径是无效的错误。
*   QL_FOTA_PACK_NAME_LENGTH_FAILED，保存 fota pack name 空间出错。
*   QL_FOTA_SUCCESS, 操作成功。
*
*****************************************************************/
ql_errcode_fota_e ql_fs_convert_fota_pack_name(char *fs_path,char *fota_path,int *fota_path_length);
#ifdef __cplusplus
    } /*"C" */
#endif

#endif /* QL_API_FOTA_H */

