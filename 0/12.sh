#!/usr/bin/env bash

# MANPATH=`manpath`

find_include() {
    echo $1 | sed -n -r 's/[.B ]*#include <([.a-zA-Z0-9_/]*)>/\1/p' | head -n 1
}

process_input() {
    IFS=':'
    for dir in $MANPATH
    do
        compressed_page="$dir/man3/$INPUT.3.gz"
        uncompressed_page="$dir/man3/$INPUT.3"
        if [ -f "$uncompressed_page" ]; then
            find_include "$(cat $uncompressed_page)" && return 0
        elif [ -f "$compressed_page" ]; then
            find_include "$(gunzip -c $compressed_page)" && return 0
        fi
    done
    return 1
}

while read INPUT
do
    process_input $INPUT || echo "---"
done
