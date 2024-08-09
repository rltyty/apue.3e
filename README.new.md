## generate compile_commands.json [1]

    make clean; bear -- make

## generate .clang-format [2]

    clang-format -style=llvm -dump-config > .clang-format


By default, `IndentWidth: 2`


[1]: https://clangd.llvm.org/installation#compile_commandsjson
[2]: https://clang.llvm.org/docs/ClangFormat.html#standalone-tool

