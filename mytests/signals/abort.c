
#include <stdlib.h>
int main(int argc, char *argv[]) {
    abort();
}

/*
 * NOTE:

On macOS
                                # set core file size
> ulimit -c unlimited
                                # check if core dump enabled
> sysctl kern.coredump
kern.coredump: 1
                                # check core file location
> sysctl kern.corefile
kern.corefile: /cores/core.%P
                                # list core files
> ls -lh /cores
total 7.1G
-r-------- 1 gpanda wheel 3.6G Aug 17 17:37 core.36330
-r-------- 1 gpanda wheel 3.6G Aug 17 17:54 core.44110
                                # use debugger `lldb` to inspect core file
> lldb ./Debug/signal/abort -c /cores/core.44110
(lldb) bt
(lldb) thread list
(lldb) thread backtrace all
(lldb) image list
(lldb) register read
(lldb) frame variable
 */

// vim: tw=78:ts=8:sts=2:sw=2:et:ft=c:norl:
