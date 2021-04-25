# dtools stacklist

Though the name is *stacklist*, it just show words and try to find function
for each word. If a function is found, the function name and offset to the
function will be shown.

Typically, it will be used to parse stack content. The functions inside the
stack will be a hint for backtrace.

One or more elf file names should be provided. For appimg application, it
it important to provide all elf file names, including the base elf file
name and appimg elf file name.

It can be used with online connection, and offline connection.
