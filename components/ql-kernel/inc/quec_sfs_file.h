#ifndef __QUEC_SFS_FILE__
#define __QUEC_SFS_FILE__

#ifdef CONFIG_QUEC_PROJECT_FEATURE_SFS_FILE

#define QUEC_SFS_DEBUG_LEVEL0		0		// Close
#define QUEC_SFS_DEBUG_LEVEL1		1		// Print Log
#define QUEC_SFS_DEBUG_LEVEL2		2		// STD Can Read SFS and Print Log
#define QUEC_SFS_DEBUG_LEVEL		QUEC_SFS_DEBUG_LEVEL1

int quec_sfs_file_proc_info_init(int fd);

void quec_sfs_file_proc_info_delete(int fd);

void quec_sfs_file_set_done_flag(int fd, bool flag);

int quec_sfs_file_set_aes_key(void *key, uint8_t len);


int sfs_fstat(int fd, char *fname, char *path, struct stat *st);

off_t sfs_lseek(int fd, off_t offset, int mode);

ssize_t sfs_write(int fd, const void *data, size_t size);

ssize_t sfs_read(int fd, void *dst, size_t size);

#endif //#ifdef CONFIG_QUEC_PROJECT_FEATURE_SFS_FILE

#endif //#ifndef __QUEC_SFS_FILE__

