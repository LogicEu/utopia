#!/bin/bash

name=libutopia

flags=(
    -std=c99
    -Wall
    -Wextra
    -O2
)

inc=(
    -I.
)

fail_op() {
    echo "Run with -dlib to build dynamically, or -slib to build statically."
    exit
}

fail_os() {
    echo "OS is not supported yet..."
    exit
}

mac_dlib() {
    gcc ${flags[*]} ${inc[*]} -dynamiclib src/*.c -o $name.dylib &&\
    install_name_tool -id @executable_path/$name.dylib $name.dylib
}

linux_dlib() {
    gcc -shared ${flags[*]} ${inc[*]} -fPIC src/*.c -o $name.so 
}

dlib() {
    if echo "$OSTYPE" | grep -q "darwin"; then
        mac_dlib
    elif echo "$OSTYPE" | grep -q "linux"; then
        linux_dlib
    else
        fails_os
    fi
}

slib() {
    gcc ${flags[*]} ${inc[*]} -c src/*.c && ar -crv $name.a *.o && rm *.o
}

clean() {
    rm $name.a
}

case "$1" in
    "-dlib")
        dlib;;
    "-slib")
        slib;;
    "-clean")
        clean;;
    *)
        fail_op;;
esac
