#include <stdio.h>
#include <stdlib.h>

int main()
{
    void *ptr = malloc(10);
    free(ptr+5);
}