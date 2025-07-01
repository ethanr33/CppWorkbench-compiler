
build_exec=compiler
if [ -f $1 ]; then
    ./$build_exec $1

    if [ $? == 0 ]; then
        nasm out.asm -f elf64 -o out.o
        ld out.o -o out
        ./out
        exit $?
    fi
else
    echo "$1: file not found"
fi