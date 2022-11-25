#!/usr/bin/bash

gcc -g -o test_zip test.c -Wl,-rpath -Wl,'/home/belov/caos/libzip-1.9.2/build/lib/libzip.so.5' -lzip