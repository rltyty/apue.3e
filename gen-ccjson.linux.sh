#!/bin/sh

# Generate compile_commands.json for current project
make clean; bear -- make -j$(nproc)  BUILD=Debug

# Generate FAKE compile_commands.json for Libc and XNU (kernel)
# Currently, it seems that cross reference indexing database (.cache)
# generated from real (bear -- make) build compile_commands.json works.
# Fake compile_commands.json seems not to work.
#
cd /home/scv/repos.3rdparty/glibc-2.36

find . -name '*.c' | jq -R -s -c '
  split("\n")[:-1]
  | map({
      directory: "/home/scv/repos.3rdparty/glibc-2.36",
      command: ("clang -isystem/usr/include -Iinclude -I. -Iposix -Istdlib -Istdio-common -Istring -Iwcsmbs -Imisc -Ilinuxthreads -Isysdeps/unix/sysv/linux -Isysdeps/unix/sysv/linux/x86_64 -Isysdeps/unix/sysv/linux/x86 -c " + .),
      file: .
    })
' > compile_commands.json

cd -

cd /home/scv/repos.3rdparty/linux-6.1.147

find . -name '*.c' | jq -R -s -c '
  split("\n")[:-1] |
  map({
    directory: "/home/scv/repos.3rdparty/linux-6.1.147",
    command: "clang -Iinclude -Iarch/x86/include -Iarch/x86/include/generated -include include/linux/kconfig.h -c \(. )",
    file: .
  })
' > compile_commands.json

cd -
