#ifndef _QL_FS_HANDLER_H_
#define _QL_FS_HANDLER_H_

#include "fs_mount.h"
#include "vfs.h"
#include "hal_config.h"
#include "quec_vfs_file.h"
#ifdef CONFIG_QUEC_PROJECT_FEATURE_SFS_FILE
#include "quec_sfs_file.h"
#endif

#define CONFIG_RESERVE_SIZE   128*1024

#define QL_FS_ALIGEN_RESERVE_SIZE  3*1024	   //reserve it for byte alignment
#define UNISOC_EFS_RESERVE_SIZE  64*1024   //reserved by unisoc, we cannot write to ext flash when free size less than 64k, so cut it out from free space

#define FILE_OPEN_FLAGS_R		(O_RDONLY)
#define FILE_OPEN_FLAGS_W		(O_WRONLY | O_CREAT | O_TRUNC)
#define FILE_OPEN_FLAGS_A		(O_WRONLY | O_CREAT | O_APPEND)
#define FILE_OPEN_FLAGS_RPluse	(O_RDWR)
#define FILE_OPEN_FLAGS_WPluse	(O_RDWR | O_CREAT | O_TRUNC)
#define FILE_OPEN_FLAGS_APluse	(O_RDWR | O_CREAT | O_APPEND)

#ifndef QFILE
typedef int QFILE;          /* file handler, returned by ql_fopen  */
#endif


typedef enum
{
	QL_FILE_UFS = 0,               /* ufs       	*/
	QL_FILE_SFS, 				   /* sfs 			*/
	QL_FILE_SD,                    /* sd        	*/
    QL_FILE_EMMC,                  /* emmc          */   
    QL_FILE_RAM,                   /* ram       	*/
    QL_FILE_EFS,                   /* efs       	*/
    QL_FILE_NVM,
    QL_FILE_FOTA,            
    QL_FILE_MAX,						
}QL_FILETYPE;

#define QL_FILE_PRXFIX_UFS 		"UFS:"     //attention: ufs file prefix 
#define QL_FILE_PRXFIX_RAM 		"RAM:"
#define QL_FILE_PRXFIX_SD  		"SD:"
#define QL_FILE_PRXFIX_EFS		"EFS:"
#define QL_FILE_PRXFIX_NVM 		"NVM:"
#define QL_FILE_PRXFIX_FOTA     "FOTA:"

#define QL_UFS_ROOT_DIR         QUEC_USER_DIR_ROOT
#define QL_NVM_ROOT_DIR         QUEC_OPEN_NVM_ROOT_DIR
#define QL_FOTA_ROOT_DIR        QUEC_OPEN_FOTA_ROOT_DIR
#define QL_EFS_ROOT_DIR         "/ext/"
#define QL_SD_ROOT_DIR          "/sdcard0/"

#ifdef CONFIG_QUEC_PROJECT_FEATURE_SFS_FILE
#define QL_FILE_PRXFIX_SFS 		"SFS:"
#define QL_SFS_ROOT_DIR         QUEC_SFS_FILE_ROOT_DIR
#else
#define QL_FILE_PRXFIX_SFS 		NULL
#define QL_SFS_ROOT_DIR         NULL
#endif

#ifdef CONFIG_QUEC_PROJECT_FEATURE_EMMC
#define QL_FILE_PRXFIX_EMMC     "EMMC:"
#define QL_EMMC_ROOT_DIR        QL_SD_ROOT_DIR
#else
#define QL_FILE_PRXFIX_EMMC     NULL
#define QL_EMMC_ROOT_DIR        NULL
#endif


/*  add file system type below, current only support UFS  */
#define ql_file_type_list_default											\
{																			\
	{QL_FILE_UFS,	QL_FILE_PRXFIX_UFS,	 	   QL_UFS_ROOT_DIR	  		},	\
	{QL_FILE_SFS,	QL_FILE_PRXFIX_SFS,	 	   QL_SFS_ROOT_DIR	  	  	},	\
	{QL_FILE_SD,   	QL_FILE_PRXFIX_SD,     	   QL_SD_ROOT_DIR   		},	\
	{QL_FILE_EMMC,  QL_FILE_PRXFIX_EMMC,       QL_EMMC_ROOT_DIR   		},	\
	{QL_FILE_RAM,	NULL,	  				   NULL	  	  				},	\
	{QL_FILE_EFS,	QL_FILE_PRXFIX_EFS,		   QL_EFS_ROOT_DIR	  	  	},	\
	{QL_FILE_NVM,   NULL,        			   NULL						},  \
	{QL_FILE_FOTA,  QL_FILE_PRXFIX_FOTA,       QL_FOTA_ROOT_DIR 		},  \
	{QL_FILE_MAX,  	NULL,				   	   NULL 	  				},	\
} 

typedef struct
{
    unsigned long f_bsize;   /* file system block size */
    unsigned long f_frsize;  /* fragment size */
    unsigned long f_blocks;  /* size of fs in f_frsize units */
    unsigned long f_bfree;   /* free blocks in fs */
    unsigned long f_bavail;  /* free blocks avail to non-superuser */
    unsigned long f_files;   /* total file nodes in file system */
    unsigned long f_ffree;   /* free file nodes in fs */
    unsigned long f_favail;  /* avail file nodes in fs */
    unsigned long f_fsid;    /* file system id */
    unsigned long f_flag;    /* mount flags */
    unsigned long f_namemax; /* maximum length of filenames */
}qstat_vfs;


typedef struct
{
	QL_FILETYPE		file_type;
	char 			*type_name;
	char 			*path_name;
}ql_file_type_list_t;

extern ql_file_type_list_t ql_file_type_list[];


typedef struct
{
	char file_name[QUEC_FILE_FILENAME_MAX_LEN];
	int  file_open_mode;
	int  fd;
}ql_file_open_list_t;


int ql_file_check_path(char *input_path, char *real_path, int *file_type);

int ql_file_get_mode(char *mode);

int ql_file_get_mode_by_fd(int fd);

int ql_file_is_opened(char *file_name)     ;

int ql_file_is_opened_by_fd(int fd);

int ql_open_file_get_info(int position, ql_file_open_list_t *file_info);

int ql_filesys_get_state(qstat_vfs *state, char *path);

int ql_get_file_name_by_fd(int fd, char *name);

int ql_open_file_push(ql_file_open_list_t *file_info);

int ql_open_file_pop(int fd);

int ql_open_file_get_cnt(void);

int ql_open_file_init(void);
	
QL_FILETYPE ql_file_type_get(char *param_str);

QL_FILETYPE ql_file_type_get_by_fd(int fd);

/********************* Not Open ********************/
QFILE ql_open(const char *file_name, int mode);

int64 ql_fs_total_size(const char *path);

int64 ql_fs_total_size_by_fd(int fd);
/********************* In ql_fs.c ********************/


#endif
