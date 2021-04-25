# fatfs Porting

This will port *fatfs*, and integrate *fatfs* into vfs. The upstream version
is 0.14 with 2 patches.

## fatfs configuration

`FF_USE_MKFS`:<br>
It should be defined as 1, to support format fat file system.

`FF_USE_EXPAND`:<br>
It should be defined as 1, to support `truncate` to larger size.

`FF_USE_LABEL`:<br>
It should be defined as 1, to support `fatfs_vfs_set_volume` and
`fatfs_vfs_get_volume`.

`FF_CODE_PAGE`:<br>
Now, only 936 is supported.

`FF_STR_VOLUME_ID`:<br>
It should be defined as 0. vfs layer will construct path as
`<volume id>:<local path>`.

`FF_USE_LFN`:<br>
It should be defined as 4. It is unreasonable to disable LFN. Option 1
is not thread safe. There are stack overflow risk for option 2.

There are too many dynamic memory allocation and free with option 3.
Though it doesn't hurt, pre-allocated memory may be better (more
determinstic).

`FF_LFN_UNICODE`:<br>
It shoule be defined as 2. APIs will use UTF-8.

`FF_FS_REENTRANT`:<br>
It sould be defined as 0. Thread safe will be considered in vfs layer.

## unicode

The unicode related functions inside *fatfs* aren't used:

* `ff_wtoupper`
* `ff_uni2oem`
* `ff_oem2uni`

They will be implemented in portable layer. They should be implemented based
on platform character encoding APIs.

## volumes

Even in most cases, only one fat file system is enough, the design should
support multiple fat file systems.

In platform, *block device* is the layer for storage. In *fatfs*, `pdrv` is
used as parameter of diskio. Then a mapping from `pdrv` to block device
instance is implemented in portable layer.

Also, `get_ldnumber` will convert a path into volume and local path inside
volume. `FF_STR_VOLUME_ID` should be configured 0, and caller shall
construct path as `<volume id>:<local path>`. For example: `0:dir/file`
for volume 0, `1:dir/file` for volume 1.
