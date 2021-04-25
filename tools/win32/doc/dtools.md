# dtools

`dtools` is a collection of various command line tools for development and
debugging.

## version

Example:

    $ dtools --version
    dtools v1.0-91-g541e4eb


There is a global version for `dtools`, and there are no separated for each
individual tool.

## Commands List

    $ dtools -h

It will show the supported command list and brief description.

## Bash Completion

    $ . setting/dtools_completion
    $ dtools g<TAB><TAB>

A bash completion script is provided. After it is sourced, bash completion
can be used to complete the command name.

## Subcommand help

Each subcommand will support `-h` option, and show a brief description of
the subcommand. For example:

    $ dtools heapreport -h

## Connection

For commands which needs to access target, *connection* options should be
provided.

|     Type      |                                                                                                                            Description                                                                                                                            | Supported |
| ------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | --------- |
| uart          | Connect target through UART. The protocol is *host* command.                                                                                                                                                                                                      | 8910      |
| debughost     | Debughost is a special hardware in target. The physical connection is the same as UART, and hardware can process some kind of *host* commands. So, CPU won't be involved during reading and writing memory or registers.                                          | 8910      |
| debuguart     | It is a special usage model of debughost. It uses debughost port, and using software *host* packets. The *host* packet will be processed by CPU. So, there are no cache coherency issue. Also it supports reading and writing ADI register with a single command. | 8910      |
| coolhost      | *coolhost* is a socket server. Typically, *coolhost* itself connects target through debughost.                                                                                                                                                                    | 8910      |
| diagport      | Connect target through UART. The protocol is *diag*.                                                                                                                                                                                                              | 8811      |
| channelserver | Channelserver is a socket server. Typically, channelsever itself connects target through diag.                                                                                                                                                                    | 8811      |
| cmm           | There are no physical connection to target. Rather, a `.cmm` file will be parsed. It will simulate target read and write.                                                                                                                                         | all       |

These connection types will be classified as:

* online
* offline: *cmm* is regarded as offline connection.
