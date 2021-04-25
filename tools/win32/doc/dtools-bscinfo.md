# dtools bscinfo

This will parse *blue screen core* dump file, and generate `.cmm` file
for debugging.

When target enters blue screen, it will write some information into reserved
memory. There are several user cases:

* Dump the reserved memory online.
* At next boot without power loss, the memory will be kept. Application can
  write the memory into file system, or send to server after network is ready.

During parsing, it will:

* Show software version, if encountered.
* Show exception information, if encountered.
* Write profile data into `profile.bin`, if encountered.
* Write memory blocks into `nnnnnnnn.bin`. The file name is the address of
  memory block.
* Write `ap.cmm`, if 8910 AP register block or 8811 register block is
  encountered.
* Write `cp.cmm`, if 8910 CP register block is encountered.

`profile.bin` can be converted to `.prf` file by `dtools bin2prf`. And `.prf`
can be opened by coolprofile tool.


## Blue Screen Core Data Format

The *blue screen core* data are separated into multiple blocks, with a 12
bytes header. The header format is:

    4B: magic, 'BSCR'
    4B: total size
    4B: crc, except the 12 bytes header
    (blocks)

The format of the first 8 bytes of each block is the same:

    4B: type
    4B: block size. When block size is not 4 bytes aligned, next block
        will be started at 4 bytes aligned location

### Blue Screen Core End Block

    4B: type, value is 0
    4B: block size
    8B: "COREEND"


### SWVER Block

    4B: type, value is 1
    4B: block size
    (variable size): software version string

### Memory Block

    4B: type, value is 2
    4B: block size
    4B: flags, reserved now
    4B: memory address
    (variable size): memory content

### Profile Data

    4B: type, value is 3
    4B: block size
    (variable size): profile data

### Exception Information

    4B: type, value is 4
    4B: block size
    (variable size): exception information string

### 8910 CP Exception Information

    4B: type, value is 5
    4B: block size
    (variable size): 8910 CP exception information string

### 8910 AP Registers

    4B: type, value is 0x40
    4B: block size
    (registers, refer to hal_blue_screen_imp.h for definition)

### 8910 CP Registers

    4B: type, value is 0x41
    4B: block size
    (registers, refer to hal_blue_screen_imp.h for definition)

### 8811 Registers

    4B: type, value is 0x42
    4B: block size
    (registers, refer to hal_blue_screen_imp.h for definition)
