#!/usr/bin/bash

gcc -g -o main -I/usr/include/fuse3 2.c -lfuse3 -fsanitize=address