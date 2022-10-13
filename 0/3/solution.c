#include <stdio.h>

// #define STRS "abc\nqwerty\nxyz\n"
// #define INTS {1, 2, 3, 4}

int main() {
    printf("%s", STRS);
    int sum = 0;
    int ints[] = INTS;
    for (size_t i = 0; i < sizeof(ints) / sizeof(int); ++i)
        sum += ints[i];
    printf("%d\n", sum);
}