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

if echo "$OSTYPE" | grep -q "darwin"; then
    dlib=(
        -dynamiclib
    )
    suffix=.dylib
elif echo "$OSTYPE" | grep -q "linux"; then
    dlib=(
        -shared
        -fPIC
    )
    suffix=.so
else
    echo "This OS is not supported by this shell script yet..." && exit
fi

comp() {
    echo "$@" && $@
}

shared() {
    mkdir -p tmp
    comp $cc -c $src ${flags[*]} && mv *.o tmp/ || exit
    
    mkdir bin
    comp $cc tmp/*.o -o bin/$name$suffix ${dlib[*]}
}

static() {
    mkdir -p tmp
    comp $cc ${flags[*]} -c $src && mv *.o tmp/ || exit
    
    mkdir -p bin
    ar -cr bin/$name.a tmp/*.o
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
    
    make all -j
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
    "all")
        shared && static;;
    "make")
        make all -j;;
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
