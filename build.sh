#!/bin/bash

cc=gcc
src=src/*.c
name=libutopia

flags=(
    -std=c89
    -Wall
    -Wextra
    -pedantic
    -O2
    -I.
)

comp() {
    echo "$@" && $@
}

shared() {
    mkdir -p bin
    if echo "$OSTYPE" | grep -q "darwin"; then
        comp $cc ${flags[*]} -dynamiclib $src -o bin/$name.dylib
    elif echo "$OSTYPE" | grep -q "linux"; then
        comp $cc -shared ${flags[*]} -fPIC $src -o bin/$name.so 
    else
        echo "This OS is not supported by this shell script yet..." && exit
    fi
}

static() {
    mkdir -p tmp
    mkdir -p bin
    comp $cc ${flags[*]} $arg ${inc[*]} -c $src && mv *.o tmp && ar -cr bin/$name.a tmp/*.o
}

cleand() {
    [ -d $1 ] && rm -r $1 && echo "deleted $1"
}

cleanf() {
    [ -f $1 ] && rm $1 && echo "deleted $1"
}

clean() {
    cleand bin
    cleand tmp
    return 0
}

install() {
    [ "$EUID" -ne 0 ] && echo "Run with sudo to install" && exit
    
    make -j && make shared -j
    cp -r utopia /usr/local/include/utopia

    [ -f bin/$name.a ] && mv bin/$name.a /usr/local/lib
    [ -f bin/$name.so ] && mv bin/$name.so /usr/local/lib
    [ -f bin/$name.dylib ] && mv bin/$name.dylib /usr/local/lib
    
    echo "Successfully installed $name"
    return 0
}

uninstall() {
    [ "$EUID" -ne 0 ] && echo "Run with sudo to uninstall" && exit

    cleand /usr/local/include/utopia
    cleanf /usr/local/lib/$name.a
    cleanf /usr/local/lib/$name.so
    cleanf /usr/local/lib/$name.dylib

    echo "Successfully uninstalled $name"
    return 0
}

case "$1" in
    "shared")
        shared;;
    "static")
        static;;
    "clean")
        clean;;
    "install")
        install;;
    "uninstall")
        uninstall;;
    *)
        echo "Run with 'shared' or 'static' to build."
        echo "Use 'install' to build and install in /usr/local."
        echo "Use 'clean' to remove local builds."
esac
