## Generate compile_commands.json [1]

### For current project

```sh
make clean; bear -- make -j$(nproc)  BUILD=Debug
```

### Add Libc and XNU kernel source to clangd indexing

#### macOS

```sh
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

```

#### Linux

##### Install build essentials
```sh
sudo apt install -y make gcc texinfo make binutils gawk bison perl sed \
  python3 python3-pexpect autoconf gettext
```

##### Build and generate `compile_commands.json`

```sh
cd $SRC_ROOT

mkdir build
cd build
../configure --prefix=$SRC_ROOT/install
bear -- make -j$(nproc)
# make install          # no need if only navigate sources in editor
```

## Configure `.clangd` with local / system / Libc / XNU headers

### macOS

```yaml
CompileFlags:
  Add: [
    # System headers (macOS SDK)
    -isystem/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include,
    # Framework headers
    -iframework/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks,

    # Libc headers
    -I/Users/gpanda/repos.3rdparty/Libc-Libc-1592.100.35/include,
    -I/Users/gpanda/repos.3rdparty/Libc-Libc-1592.100.35/gen,

    # XNU headers (bsd part)
    -I/Users/gpanda/repos.3rdparty/xnu-xnu-10063.121.3,
    -I/Users/gpanda/repos.3rdparty/xnu-xnu-10063.121.3/bsd,
    -I/Users/gpanda/repos.3rdparty/xnu-xnu-10063.121.3/bsd/sys

  ]
```

### Linux

```yaml
CompileFlags:
  Add: [
    -I/home/scv/repos.3rdparty/glibc-2.36/include,
    -I/home/scv/repos.3rdparty/glibc-2.36,
    -I/home/scv/repos.3rdparty/glibc-2.36/posix,
    -I/home/scv/repos.3rdparty/glibc-2.36/sysdeps/unix/sysv/linux,
    -I/home/scv/repos.3rdparty/glibc-2.36/sysdeps/unix/sysv/linux/x86_64,

    -isystem /usr/lib/clang/22/include,

    -isystem /usr/local/include,
    -isystem /usr/include
  ]
```

When open a C source file, `clangd` will incrementally generate cross
reference information database, i.e. indexing (`.cache`). When this process is
done, code navigation (jump via <C-]> in Neovim) chain can be established
according to `.clangd` configuration.

For example, Linux glibc:

```
project: mytests/signal/sigaction.c
   -> glibc-2.36/include/signal.h
       -> glibc-2.36/signal/signal.h (real declarations)
           -> glibc-2.36/signal/sigaction.c (implementation)
```

## Generate .clang-format [2]
```sh
    #clang-format -style=LLVM -dump-config > .clang-format
    clang-format -style=Google -dump-config > .clang-format
```

By default, `IndentWidth: 2`

## Install `fd` (a friendly `find` alternative) for Makefile use
```sh
    sudo apt install fd-find
    ln -s /usr/bin/fdfind ~/.local/bin/fd
```

## Add `cpp` file type in Neovim clangd configuration for header (`.h`) search

```lua
  -- Overrides the clangd configuration in <rtp>/lsp/clangd.lua
  filetypes = { 'c', 'cpp' },
```


[1]: https://clangd.llvm.org/installation#compile_commandsjson
[2]: https://clang.llvm.org/docs/ClangFormat.html#standalone-tool
[3]: https://github.com/sharkdp/fd

