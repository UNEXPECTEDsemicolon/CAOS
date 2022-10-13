#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int main(int argc, char** argv) {
    int sum = 0;
    bool str = 0;
    for (size_t i = 0; i < argc; ++i)
    {
        if (!str && !strcmp(argv[i], "#"))
        {
            str = true;
            continue;
        }
        if (str)
            printf("%s\n", argv[i]);
        else
            sum += atoi(argv[i]); 
    }
    printf("%d\n", sum);
}