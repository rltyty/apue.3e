#!/bin/sh

# Generate compile_commands.json for current project
make clean; bear -- make -j$(nproc)  BUILD=Debug

# Generate FAKE compile_commands.json for Libc and XNU (kernel)

cd /Users/gpanda/repos.3rdparty/Libc-Libc-1592.100.35
find . -name '*.c' | jq -R -s -c 'split("\n")[:-1] | map({
directory: "/Users/gpanda/repos.3rdparty/Libc-Libc-1592.100.35",
command: "clang -isystem/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include -Iinclude -Igen -c " + .,
file: .
})' > compile_commands.json

cd -

cd /Users/gpanda/repos.3rdparty/xnu-xnu-10063.121.3
find bsd -name '*.c' | jq -R -s -c 'split("\n")[:-1] | map({
 directory: "/Users/gpanda/repos.3rdparty/xnu-xnu-10063.121.3",
 command: "clang -isystem/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include -I../EXTERNAL_HEADERS -I. -Ibsd -Ibsd/sys -c " + .,
 file: .
})' > compile_commands.json

cd -
