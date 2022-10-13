#!/usr/bin/env bash

while read INPUT
do
    if man 3 $INPUT 2>/dev/null > temp; then
        sed -n -r 's/ *#include <([.a-zA-Z0-9_]*)>/\1/p' temp #| sed -r 's/^$/---/' || echo "---"
    else
        echo "---"
    fi
done
rm temp 2> /dev/null