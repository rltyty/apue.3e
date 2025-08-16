#!/bin/sh

SIZE_CMD=size
if [ "$os" = "macOS" ]; then
  SIZE_CMD="size -m"
fi
$SIZE_CMD ./Debug/procenv/printargv ./Debug/procenv/execve

#
# On Linux:
# :!sh procenv/inspect_binary.sh
#    text    data     bss     dec     hex filename
#    1392     584       8    1984     7c0 ./Debug/procenv/printargv
#    1815     648      16    2479     9af ./Debug/procenv/execve
#
# On macOS:
# :!sh procenv/inspect_binary.sh
# __TEXT  __DATA  __OBJC  others  dec     hex
# 16384   16384   0       4295000064      4295032832      100010000       ./Debug/procenv/printargv
# 16384   16384   0       4295000064      4295032832      100010000       ./Debug/procenv/execve
#
# ./Debug/procenv/printargv:
# Segment __PAGEZERO: 4294967296 (zero fill)
# Segment __TEXT: 16384
#         Section __text: 93
#         Section __stubs: 6
#         Section __stub_helper: 26
#         Section __cstring: 14
#         Section __unwind_info: 72
#         total 211
# Segment __DATA_CONST: 16384
#         Section __got: 8
#         total 8
# Segment __DATA: 16384
#         Section __la_symbol_ptr: 8
#         Section __data: 8
#         total 16
# Segment __LINKEDIT: 16384
# total 4295032832
# ./Debug/procenv/execve:
# Segment __PAGEZERO: 4294967296 (zero fill)
# Segment __TEXT: 16384
#         Section __text: 118
#         Section __stubs: 24
#         Section __stub_helper: 56
#         Section __cstring: 69
#         Section __unwind_info: 72
#         total 339
# Segment __DATA_CONST: 16384
#         Section __got: 16
#         total 16
# Segment __DATA: 16384
#         Section __la_symbol_ptr: 32
#         Section __data: 48
#         Section __bss: 8 (zerofill)
#         total 88
# Segment __LINKEDIT: 16384
# total 4295032832
