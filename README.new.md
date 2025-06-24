## generate compile_commands.json [1]
```sh
make clean; bear -- make
```

## generate .clang-format [2]
```sh
    clang-format -style=llvm -dump-config > .clang-format
```

By default, `IndentWidth: 2`

## install `fd` (a friendly `find` alternative) for Makefile use
```sh
    sudo apt install fd-find
    ln -s /usr/bin/fdfind ~/.local/bin/fd
```

## add `cpp` file type in Neovim clangd configuration for header (`.h`) search

```lua
  -- Overrides the clangd configuration in <rtp>/lsp/clangd.lua
  filetypes = { 'c', 'cpp' },
```


[1]: https://clangd.llvm.org/installation#compile_commandsjson
[2]: https://clang.llvm.org/docs/ClangFormat.html#standalone-tool
[3]: https://github.com/sharkdp/fd

