CC = gcc

.PHONY : all build clean

all: build

vars = $(shell cat compile.conf | sed -r -n "s/[[:alnum:]_-]*=([[:alnum:][:space:][:punct:]]*)/\1/p" | sed -r "s/\#.*//" | tr '\n' '=')
strs = $(shell echo $(vars) | tr '=' '\n' | sed -r -n "/^[[:alpha:][:punct:][:space:]]+/p" | sed -r "s/^-[[:digit:]]+//" | tr '\n' '=' | sed -r 's/\=+/\\n/g')
ints = $(shell echo $(vars) | tr '=' '\n' | sed -r -n "/^-?[[:digit:]]+/p" | tr '\n' ',')

build:
	${CC} -o program.exe -DSTRS="\"$(strs)\"" -DINTS="{$(ints)}" solution.c

clean:
	rm -f program.exe