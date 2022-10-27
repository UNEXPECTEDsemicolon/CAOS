CC=gcc-arm

build: main.o 1.o
	"/home/belov/caos/gcc-linaro-7.5.0-2019.12-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-gcc" -g -o main main.o 1.o

main.o: main.c
	"/home/belov/caos/gcc-linaro-7.5.0-2019.12-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-gcc" -g -c -o main.o -g main.c

1.o: 1.S
	"/home/belov/caos/gcc-linaro-7.5.0-2019.12-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-as" -g -c  -o 1.o -g 1.S

clean:
	rm -f main
	rm -f main.o
	rm -f 1.o

rebuild: clean build