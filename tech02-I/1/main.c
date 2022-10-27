#include <stdio.h>
#include <stdlib.h>
// #define int long long

extern int sum(int x0, size_t N, int *X);

signed main() {
    // int X[] = {1000000};
    int* X = malloc(2 * sizeof(int));
    X[0] = 100000;
    X[1] = -150000;
    // size_t N = sizeof(X) / sizeof(int);
    size_t N = 0;
    int x0 = 10;
    printf("%d\n", sum(x0, N, X));
    free(X);
}
