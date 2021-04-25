# dtools keyexp8910, imgverify8910, pacverify8910

These are utilities for 8910 image and pac signature verification.
The feature of image sign won't be integrated, and `vlrsign` should be used.

The `key.db` mentioned below the same as the one used by `vlrsign`. The
dynamic library (`rdasign.dll` for Windows, `lib/librdasign.so`) should
exist to make the commands work.

> Command `sign8910` is the same with `vlrsign`, and `vlrsign` is the
> recommended tool.

## keyexp8910

To export public key from `key.db`. For example:

```
$ dtools keyexp8910 --pn test pubkey.pem
```

In the example, `test` is the product name, `pubkey.pem` is the output file
name of public key in pem format, which can be used by verify commands.

> Due to it will only access public key, password of `key.db` is not needed.

By default, it will try to find `key.db` under the same directory of
executable. If `key.db` is located in other places, the path can be specified.
For example:

```
$ dtools keyexp8910 --keydb ../../key.db --pn test pubkey.pem
```

## imgverify8910

To verify signed image, either `key.db` under the same directory of
executable, or exported public key pem file can be specified. Similar to
`vlrsign`, product name shall be specified when `key.db` is used.

> Ther are no options to specify `key.db` in other locations. And due to it
> will access only access public key, password of `key.db` is not needed.

Examples:

```
$ dtools imgverify8910 --keydb --pn test fdl2.sign.img
$ dtools imgverify8910 --pub pubkey.pem fdl2.sign.img
```

When `--boot` option is specified, the public key embedded inside
`boot.sign.img` will be verified. Also, the size of boot image will be
checked.

> Public key is only embedded inside boot image.

> Boot image size is fixed.

```
$ dtools imgverify8910 --keydb --pn test --boot boot.sign.img
```

When signature verify passed, the return value is 0 and there are no output.
With option `-d v`, a brief pass message will be shown.

## pacverify8910

To verify signed images inside pac, it is needed to specify images to be
checked inside pac by ID. Multiple IDs can be specified.

> ID will be shown in *FileID* column in ResearchDownload.

```
$ dtools pacverify8910 --keydb --pn test \
    --id AP --id APPIMG 8910DM.pac
$ dtools pacverify8910 --pub pubkey.pem \
    --id AP --id APPIMG 8910DM.pac
```

When signature verify passed, the return value is 0 and there are no output.
With option `-d v`, a brief pass message for each ID will be shown.

The ID in pac can be modified in building script. For example:

|     ID     |     Comment     |
| ---------- | --------------- |
| HOST_FDL   | fdl1.sign.img   |
| FDL2       | fdl2.sign.img   |
| BOOTLOADER | boot.sign.img   |
| AP         | 8910DM.sign.img |
| APPIMG     | appimg.sign.img |
