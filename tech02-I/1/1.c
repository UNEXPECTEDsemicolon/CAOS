int sum(int x0, size_t N, int* X)
{
    for (size_t i=0; i < N; ++i)
        x0 += X[i];
    return x0;
}