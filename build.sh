#!/bin/bash

cc=gcc
src=utopia/*.h
name=libutopia
tmp=tmp
bin=bin

path=/usr/local
incdir=$path/include
libdir=$path/lib

flags=(
    -std=c89
    -Wall
    -Wextra
    -pedantic
    -O2
    -x
    c
    -DUTOPIA_IMPLEMENTATION
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
fi

cmd() {
    echo "$@" && $@
}

objs() {
    cmd mkdir -p $tmp
    for f in ${src[*]}
    do
        fname=$(basename -- $f)
        obj=${fname%.h}.o
        cmd $cc ${flags[*]} -c $f -o $tmp/$obj
    done
}

shared() {
    [ -d $tmp ] || objs
    cmd mkdir -p $bin
    cmd $cc $tmp/*.o -o $bin/$name$suffix ${dlib[*]}
}

static() {
    [ -d $tmp ] || objs
    cmd mkdir -p $bin
    cmd ar -cr $bin/$name.a $tmp/*.o
}

cleand() {
    [ -d $1 ] && cmd rm -rf $1
}

cleanf() {
    [ -f $1 ] && cmd rm -f $1
}

clean() {
    cleand $bin
    cleand $tmp
    return 0
}

install() {
    [ "$EUID" -ne 0 ] && echo "$0: run with sudo to install" && exit
    
    make all -j # or static && shared
    cmd cp -r utopia $incdir/utopia

    [ -f $bin/$name.a ] && cmd mv $bin/$name.a $libdir
    [ -f $bin/$name.so ] && cmd mv $bin/$name.so $libdir
    [ -f $bin/$name.dylib ] && cmd mv $bin/$name.dylib $libdir
    
    echo "successfully installed $name"
    return 0
}

uninstall() {
    [ "$EUID" -ne 0 ] && echo "$0: run with sudo to uninstall" && exit

    cleand $incdir/utopia
    cleanf $libdir/$name.a
    cleanf $libdir/$name.so
    cleanf $libdir/$name.dylib

    echo "successfully uninstalled $name"
    return 0
}

usage() {
    echo "$0 usage:"
    echo -e "shared\t\t: build $name as a shared library: $name$suffix"
    echo -e "static\t\t: build $name as a static library: $name.a"
    echo -e "all\t\t: build $name as shared and static libraries"
    echo -e "help\t\t: print usage information and available commands"
    echo -e "clean\t\t: delete local builds and executables"
    echo -e "install\t\t: install $name in $path (run with sudo)"
    echo -e "uninstall\t: uninstall $name from $path (run with sudo)"
}

case "$1" in
    "help")
        usage && exit;;
    "shared")
        shared;;
    "static")
        static;;
    "all")
        shared && static;;
    "clean")
        clean;;
    "install")
        install;;
    "uninstall")
        uninstall;;
    *)
        usage && exit;;
esac
