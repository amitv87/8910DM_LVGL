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

#include "calclib/unlzma.h"
#include "vfs.h"
#include <stdio.h>

extern int lzmaDecompress(unsigned char *buf, long in_len,
                          long (*fill)(void *, void *, unsigned long), void *fill_ctx,
                          long (*flush)(void *, unsigned long),
                          unsigned char *output,
                          unsigned char *dec_buf,
                          long *posp,
                          void (*error)(char *x));

static long fdfill(void *ctx, void *buffer, unsigned long len)
{
    return vfs_read((int)ctx, buffer, len);
}

int lzmaDecompressFile(int fd, void *output)
{
    return lzmaDecompress(NULL, 0, fdfill, (void *)fd, NULL,
                          output, NULL, NULL, NULL);
}

int lzmaDecompressFileName(const char *fname, void *output, long *out_size)
{
    int fd = vfs_open(fname, O_RDONLY);
    if (fd < 0)
        return -1;

    char header[LZMA_HEADER_SIZE];
    if (vfs_read(fd, header, LZMA_HEADER_SIZE) != LZMA_HEADER_SIZE)
    {
        vfs_close(fd);
        return -1;
    }

    if (lzmaDecompressProbe(header, LZMA_HEADER_SIZE, out_size, NULL) < 0)
    {
        vfs_close(fd);
        return -1;
    }

    vfs_lseek(fd, 0, SEEK_SET);
    if (lzmaDecompress(NULL, 0, fdfill, (void *)fd, NULL,
                       output, NULL, NULL, NULL) < 0)
    {
        vfs_close(fd);
        return -1;
    }

    vfs_close(fd);
    return 0;
}
