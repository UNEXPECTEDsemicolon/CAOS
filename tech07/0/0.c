#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    execlp("python3", "python3", "-c", "print(eval(input()))", NULL);
    perror("exec");
    exit(1);
}