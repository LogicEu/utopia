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

dlib() {
    if echo "$OSTYPE" | grep -q "darwin"; then
        $cc ${flags[*]} -mmacos-version-min=10.10 -dynamiclib $src -o $name.dylib
    elif echo "$OSTYPE" | grep -q "linux"; then
        $cc -shared ${flags[*]} -fPIC $src -o $name.so 
    else
        echo "This OS is not supported by this shell script yet..." && exit
    fi
}

slib() {
    if echo "$OSTYPE" | grep -q "darwin"; then
        arg=-mmacos-version-min=10.10 
    fi
    $cc ${flags[*]} $arg ${inc[*]} -c $src && ar -cr $name.a *.o && rm *.o
}

cleanf() {
    [ -f $1 ] && rm $1
}

clean() {
    cleanf $name.a
    cleanf $name.so
    cleanf $name.dylib
    return 0
}

install() {
    [ "$EUID" -ne 0 ] && echo "Run with sudo to install in /usr/local" && exit
    
    dlib && slib
    cp utopia.h /usr/local/include

    [ -f $name.a ] && mv $name.a /usr/local/lib
    [ -f $name.so ] && mv $name.so /usr/local/lib
    [ -f $name.dylib ] && mv $name.dylib /usr/local/lib
    return 0
}

case "$1" in
    "shared")
        dlib;;
    "static")
        slib;;
    "clean")
        clean;;
    "install")
        install;;
    *)
        echo "Run with 'shared' or 'static' to build."
        echo "Use 'install' to build and install in /usr/local directory."
        echo "Use 'clean' to remove builds."
esac
