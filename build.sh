#!/bin/bash

cc=gcc
src=src/*.c
name=libutopia

flags=(
    -std=c99
    -Wall
    -Wextra
    -O2
    -I.
)

shared() {
    if echo "$OSTYPE" | grep -q "darwin"; then
        $cc ${flags[*]} -mmacos-version-min=10.10 -dynamiclib $src -o $name.dylib
    elif echo "$OSTYPE" | grep -q "linux"; then
        $cc -shared ${flags[*]} -fPIC $src -o $name.so 
    else
        echo "This OS is not supported by this shell script yet..." && exit
    fi
}

static() {
    $cc ${flags[*]} $arg ${inc[*]} -c $src && ar -cr $name.a *.o && rm *.o
}

cleanf() {
    [ -f $1 ] && rm $1 && echo "deleted $1"
}

clean() {
    cleanf $name.a
    cleanf $name.so
    cleanf $name.dylib
    return 0
}

install() {
    [ "$EUID" -ne 0 ] && echo "Run with sudo to install" && exit
    
    shared && static
    cp utopia.h /usr/local/include

    [ -f $name.a ] && mv $name.a /usr/local/lib
    [ -f $name.so ] && mv $name.so /usr/local/lib
    [ -f $name.dylib ] && mv $name.dylib /usr/local/lib
    
    echo "Successfully installed $name"
    return 0
}

uninstall() {
    [ "$EUID" -ne 0 ] && echo "Run with sudo to uninstall" && exit

    cleanf /usr/local/include/utopia.h
    cleanf /usr/local/lib/$name.a
    cleanf /usr/local/lib/$name.so
    cleanf /usr/local/lib/$name.dylib

    echo "Successfully uninstalled $name."
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
